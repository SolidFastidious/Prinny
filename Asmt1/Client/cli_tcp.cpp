// CLIENT TCP PROGRAM
// Revised and tidied up by
// J.W. Atwood
// 1999 June 30



char* getmessage(char *);



/* send and receive codes between client and server */
/* This is your basic WINSOCK shell */
#pragma comment( linker, "/defaultlib:ws2_32.lib" )
#include <winsock2.h>
#include <ws2tcpip.h>

#include <winsock.h>
#include <stdio.h>
#include <iostream>
#include <fstream>

#include <string.h>

#include <windows.h>

#include <tchar.h> 

using namespace std;

//user defined port number
#define REQUEST_PORT 0x7070;

int port=REQUEST_PORT;



//socket data types
SOCKET s;
SOCKADDR_IN sa;         // filled by bind
SOCKADDR_IN sa_in;      // fill with server info, IP, port



//buffer data types
char szbuffer[128];
char buff[128];
char *buffer;

int ibufferlen=0;

int ibytessent;
int ibytesrecv=0;

int inFileSize, outFileSize;

//host data types
HOSTENT *hp;
HOSTENT *rp;

char fileName[50];
char direction[10];
char localhost[11],
     remotehost[11],
	 username[11];
char myDirectory[] = "C:\\Users\\hp\\Documents\\GitHub\\PrinnyDude\Asmt1";
typedef enum Mode
{GET = 0, PUT = 1, LIST = 2};
Mode transfer;


//other

HANDLE test;

DWORD dwtest;



//reference for used structures

/*  * Host structure

    struct  hostent {
    char    FAR * h_name;             official name of host *
    char    FAR * FAR * h_aliases;    alias list *
    short   h_addrtype;               host address type *
    short   h_length;                 length of address *
    char    FAR * FAR * h_addr_list;  list of addresses *
#define h_addr  h_addr_list[0]            address, for backward compat *
};

 * Socket address structure

 struct sockaddr_in {
 short   sin_family;
 u_short sin_port;
 struct  in_addr sin_addr;
 char    sin_zero[8];
 }; */



bool ListDirectoryContents(const char *sDir) //list all the files within the client directory
{
    WIN32_FIND_DATA fdFile;
    HANDLE hFind = NULL;
 
    char sPath[2048];
	char str;
    //Specify a file mask
    sprintf(sPath, "%s\\*.*", sDir);
 
    if((hFind = FindFirstFile(sPath, &fdFile)) == INVALID_HANDLE_VALUE)
    {
        printf("Path not found: [%s]\n", sDir);
        return false;
    }
 
    do
    {
        //Find first file will always return "."
        //    and ".." as the first two directories.
        if(strcmp(fdFile.cFileName, ".") != 0
                && strcmp(fdFile.cFileName, "..") != 0)
        {
            //Build up our file path using the passed in
            //  [sDir] and the file/foldername we just found:
            sprintf(sPath, "%s\\%s", sDir, fdFile.cFileName);
 
            //File or Folder
            if(fdFile.dwFileAttributes &FILE_ATTRIBUTE_DIRECTORY)
            {
                printf("Directory: %s\n", sPath);
                ListDirectoryContents(sPath);
            }
            else{
				
				
                printf("File: %s\n", sPath);
				
            }
        }
    }
    while(FindNextFile(hFind, &fdFile)); //Find the next file.
 
    FindClose(hFind); 
 
    return true;
}

void sendPacket(const char* packet) {

	sprintf(szbuffer, packet);
	ibytessent = 0;
	ibufferlen = strlen(szbuffer);
	ibytessent = send(s,szbuffer,ibufferlen,0);

	if (ibytessent == SOCKET_ERROR)
		throw "Send failed\n";
}

char* receivePacket() {

	ibytesrecv=0;
	if((ibytesrecv = recv(s,szbuffer,128,0)) == SOCKET_ERROR)
		throw "Receive failed\n";

	return szbuffer;
}

char* readFile(const char* filename) {
	//check if file exists in directory
	//...
	
	char* filedata;

	ifstream file(filename, ios::in|ios::binary|ios::ate);
	if (file.is_open()) {
		outFileSize = file.tellg();
		filedata = new char[outFileSize];
		file.seekg(0, ios::beg);
		file.read(filedata, outFileSize);
		file.close();
		return filedata;
	}
	else
		throw "unable to open file\n";
}

void writeFile(const char* filename, const char* data, int size) {

	ofstream file(filename, ios::out|ios::binary);
	if (file.is_open()) {
		file.write(data, size);
		file.close();
	}
	else
		throw "unable to open file\n";
}

void sendFile(const char* data) {

	string fullData(data);
	string packet;
	
	int i = 0;
	while (i < fullData.length()) {
		if (i < fullData.length() - 127)
			packet = fullData.substr(i, 128);
		else
			packet = fullData.substr(i, fullData.length() - i);
		
		char* cpacket = (char*)packet.c_str();
		sendPacket(cpacket);
		receivePacket();
		i = i + 128;
	}
}

char* receiveFile() {

	char* packet;
	char* buf;
	string fullData = "";

	int i = 0;
	while (i < inFileSize) {
		receivePacket();
		fullData.append(szbuffer);
		sendPacket(itoa(i, buf, 10));
		i = i + 128;
	}

	char* cfullData = (char*)fullData.c_str();
	return cfullData;
}

void PutFile(char* fileName)
{

	ifstream myFile(fileName, ios::ate|ios::binary);

	ifstream::pos_type nonIntSize;
	ifstream::pos_type currPos;

	int byteLeft= 0;

	if(myFile.is_open())
	{
		nonIntSize = myFile.tellg();
		int fileSize = (int) nonIntSize;
		cout << "The size of the file : " <<fileSize<<endl;
		ibytessent = send(s, itoa(fileSize,buff,10), sizeof(buff),0);

		ibytesrecv =0;
		if((ibytesrecv = recv(s,szbuffer,sizeof(szbuffer),0)) == SOCKET_ERROR)
				throw "Receive failed\n";
			else
			{
				int serverFileSize=atoi(szbuffer);
				cout<<"File size from server: " << serverFileSize<<endl;   
			}
	}
	else
	{
		cout<<"Error could not open file "<<endl; 
		cout<<"Exiting program "<<endl;
		cin.get();
		exit(1); 
	}
}

 

int main(void){

	WSADATA wsadata;

	try {

		if (WSAStartup(0x0202,&wsadata)!=0){  
			cout<<"Error in starting WSAStartup()" << endl;
		} else {
			buffer="WSAStartup was successful\n";   
			WriteFile(test,buffer,sizeof(buffer),&dwtest,NULL); 

			/* Display the wsadata structure */
			/*cout<< endl
				<< "wsadata.wVersion "       << wsadata.wVersion       << endl
				<< "wsadata.wHighVersion "   << wsadata.wHighVersion   << endl
				<< "wsadata.szDescription "  << wsadata.szDescription  << endl
				<< "wsadata.szSystemStatus " << wsadata.szSystemStatus << endl
				<< "wsadata.iMaxSockets "    << wsadata.iMaxSockets    << endl
				<< "wsadata.iMaxUdpDg "      << wsadata.iMaxUdpDg      << endl;*/
		}  


		//Display name of local host.

		gethostname(localhost,10);
		cout<<"Local host name is \"" << localhost << "\"" << endl;

		if((hp=gethostbyname(localhost)) == NULL) 
			throw "gethostbyname failed\n";

		//Ask for user name
		cout << "Please enter your user name: " << flush ;   
		cin >> username ;

		//Ask for name of remote server
		int cMode;
		cout << "please enter your ftp server name :" << flush ;   
		cin >> remotehost ;
		if (strcmp(remotehost, "quit") == 0)
		exit(0);
		if((rp=gethostbyname(remotehost)) == NULL)
		throw "remote gethostbyname failed\n";


		
		cout << "Type direction of transfer (0-get, 1-put, 2-list) :" << flush;
		cin >> cMode;
		cout << "Remote host name is: \"" << remotehost << "\"" << endl;

		
		
		cout << "Type name of file to be transferred :" << flush;
		cin >> fileName;

		if(cMode == 1)
		{
			ifstream temp(fileName, ios::in||ios::binary);
			if(!temp.is_open())
			{
				cout<< "Erro opening file!"<<endl;
				cin.get();
				exit(1);
			}
		}

		if((rp=gethostbyname(remotehost)) == NULL)
			throw "remote gethostbyname failed\n";

		//Create the socket
		if((s = socket(AF_INET,SOCK_STREAM,0))==INVALID_SOCKET) 
			throw "Socket failed\n";
		/* For UDP protocol replace SOCK_STREAM with SOCK_DGRAM */

		//Specify server address for client to connect to server.
		memset(&sa_in,0,sizeof(sa_in));
		memcpy(&sa_in.sin_addr,rp->h_addr,rp->h_length);
		sa_in.sin_family = rp->h_addrtype;   
		sa_in.sin_port = htons(port);

		//Display the host machine internet address

		cout << "Connecting to remote host:";
		cout << inet_ntoa(sa_in.sin_addr) << endl;

		//Connect Client to the server
		if (connect(s,(LPSOCKADDR)&sa_in,sizeof(sa_in)) == SOCKET_ERROR)
			throw "connect failed\n";

		/* Have an open connection, so, server is 

		   - waiting for the client request message
		   - don't forget to append <carriage return> 
		   - <line feed> characters after the send buffer to indicate end-of file */




#pragma region Send request and fileName
		//Send a request to the server either get or puts
		if (cMode == 0) strcpy(direction, "get");
		if (cMode == 1) strcpy(direction, "put");
		sprintf(szbuffer,direction); 
		ibytessent=0;    
		ibufferlen = strlen(szbuffer);
		ibytessent = send(s,szbuffer,ibufferlen,0);
		if (ibytessent == SOCKET_ERROR)
			throw "Send transfer type failed\n";  


		//wait for reception of server response.
		ibytesrecv=0; 
		if((ibytesrecv = recv(s,szbuffer,128,0)) == SOCKET_ERROR)
			throw "Receive failed\n";
		else 
			cout<< "Receive transfer request from the server succeed: " << szbuffer <<endl; 


		//Send a file name to the server
		if (cMode == 0 || cMode == 1)
		{
		sprintf(szbuffer,fileName); 
		ibytessent=0;    
		ibufferlen = strlen(szbuffer);
		ibytessent = send(s,szbuffer,ibufferlen,0);
		if (ibytessent == SOCKET_ERROR)
			throw "Send fileName failed\n";  
		}
		
		
		//wait for reception of server response.
		ibytesrecv=0; 
		if((ibytesrecv = recv(s,szbuffer,128,0)) == SOCKET_ERROR)
			throw "Receive failed\n";
		else 
			cout<< "Receive file name succeed from the server: " << szbuffer <<endl; 
#pragma endregion 
		
		switch (cMode)
		{
		case GET:
			break;
		case PUT:
			PutFile(fileName);
			break;
		case LIST:
			ListDirectoryContents(myDirectory);
		default:
			cerr<< "The input is incorrect, the system will now exit" <<endl;
			exit(1);
			break;
		}

	} // try loop

	//Display any needed error response.

	catch (char *str) { cerr<<str<<":"<<dec<<WSAGetLastError()<<endl;}

	//close the client socket
	closesocket(s);

	/* When done uninstall winsock.dll (WSACleanup()) and exit */ 
	WSACleanup();  
	return 0;
}







