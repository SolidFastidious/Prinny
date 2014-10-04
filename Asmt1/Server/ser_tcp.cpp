//    SERVER TCP PROGRAM
// revised and tidied up by
// J.W. Atwood
// 1999 June 30
// There is still some leftover trash in this code.

/* send and receive codes between client and server */
/* This is your basic WINSOCK shell */
#pragma once
#pragma comment( linker, "/defaultlib:ws2_32.lib" )
#include <winsock2.h>
#include <ws2tcpip.h>
#include <process.h>
#include <winsock.h>
#include <iostream>
#include <windows.h>
#include <fstream>



using namespace std;

//port data types

#define REQUEST_PORT 0x7070

int port=REQUEST_PORT;

//socket data types
SOCKET s;

SOCKET s1;
SOCKADDR_IN sa;      // filled by bind
SOCKADDR_IN sa1;     // fill with server info, IP, port
union {struct sockaddr generic;
	struct sockaddr_in ca_in;}ca;

	int calen=sizeof(ca); 

	//buffer data types
	char szbuffer[128];

	char *buffer;
	int ibufferlen;
	int ibytesrecv;

	int ibytessent;

	int inFileSize, outFileSize;


	//host data types
	char localhost[11],
			 fileName[25],
	 direction[5],
	 username[11];
	char myDirectory[] = "C:\\Users\\hp\\Documents\\GitHub\\PrinnyDude\\Asmt1\\Server";
	enum command {get, put, list};
	command transfer;

	HOSTENT *hp;

	//wait variables
	int nsa1;
	int r,infds=1, outfds=0;
	struct timeval timeout;
	const struct timeval *tp=&timeout;

	fd_set readfds;

	//others
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
bool ListDirectoryContents(const char *sDir)//list all the files within the server directory
{
    WIN32_FIND_DATA fdFile;
    HANDLE hFind = NULL;
 
    char sPath[2048];
 
    //Specify a file mask. *.* = We want everything!
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
 
            //Is the entity a File or Folder?
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

void GetFile( char* fileName)
{

}

void PutFileName (char* fileName)
{

}
int main(void){

		WSADATA wsadata;

		try{        		 
			if (WSAStartup(0x0202,&wsadata)!=0){  
				cout<<"Error in starting WSAStartup()\n";
			}else{
				buffer="WSAStartup was suuccessful\n";   
				WriteFile(test,buffer,sizeof(buffer),&dwtest,NULL); 

				/* display the wsadata structure */
				/*cout<< endl
					<< "wsadata.wVersion "       << wsadata.wVersion       << endl
					<< "wsadata.wHighVersion "   << wsadata.wHighVersion   << endl
					<< "wsadata.szDescription "  << wsadata.szDescription  << endl
					<< "wsadata.szSystemStatus " << wsadata.szSystemStatus << endl
					<< "wsadata.iMaxSockets "    << wsadata.iMaxSockets    << endl
					<< "wsadata.iMaxUdpDg "      << wsadata.iMaxUdpDg      << endl;*/
			}  

			//Display info of local host

			gethostname(localhost,10);
			cout<<"hostname: "<<localhost<< endl;

			if((hp=gethostbyname(localhost)) == NULL) {
				cout << "gethostbyname() cannot get local host info?"
					<< WSAGetLastError() << endl; 
				exit(1);
			}

			//Create the server socket
			if((s = socket(AF_INET,SOCK_STREAM,0))==INVALID_SOCKET) 
				throw "can't initialize socket";
			// For UDP protocol replace SOCK_STREAM with SOCK_DGRAM 


			//Fill-in Server Port and Address info.
			sa.sin_family = AF_INET;
			sa.sin_port = htons(port);
			sa.sin_addr.s_addr = htonl(INADDR_ANY);


			//Bind the server port

			if (bind(s,(LPSOCKADDR)&sa,sizeof(sa)) == SOCKET_ERROR)
				throw "can't bind the socket";
			cout << "Bind was successful" << endl;

			//Successfull bind, now listen for client requests.

			if(listen(s,10) == SOCKET_ERROR)
				throw "couldn't  set up listen on socket";
			else cout << "Listen was successful" << endl;

			FD_ZERO(&readfds);

			//wait loop

			while(1)

			{

				FD_SET(s,&readfds);  //always check the listener

				if(!(outfds=select(infds,&readfds,NULL,NULL,tp))) {}

				else if (outfds == SOCKET_ERROR) throw "failure in Select";

				else if (FD_ISSET(s,&readfds))  cout << "got a connection request" << endl; 

				//Found a connection request, try to accept. 

				if((s1=accept(s,&ca.generic,&calen))==INVALID_SOCKET)
					throw "Couldn't accept connection\n";

				//Connection request accepted.
				cout<<"accepted connection from "<<inet_ntoa(ca.ca_in.sin_addr)<<":"
					<<hex<<htons(ca.ca_in.sin_port)<<endl;

				//List all the file from the server directory
				ListDirectoryContents("C:\\Users\\hp\\Documents\\Visual Studio 2010\\Projects\\Asmt1\\Server");

				//Fill in szbuffer from accepted request.
				if((ibytesrecv = recv(s1,szbuffer,128,0)) == SOCKET_ERROR)
					throw "Receive error in server program\n";

		
#pragma region Receive request and file name
				//Print reciept of successful message. 
				cout << "This is message from client for transfer request: " << szbuffer << endl;
				strcpy(direction, szbuffer);
				//Send to Client the received message (echo it back).
				ibufferlen = strlen(szbuffer);

				if((ibytessent = send(s1,szbuffer,ibufferlen,0))==SOCKET_ERROR)
					throw "error in send in server program\n";
				

				//Fill in szbuffer from accepted request.
				if((ibytesrecv = recv(s1,szbuffer,128,0)) == SOCKET_ERROR)
					throw "Receive error in server program\n";

				//Print reciept of successful message. 
				cout << "This is message from client for file name: " << szbuffer << endl;
				strcpy(fileName, szbuffer);
				//Send to Client the received message (echo it back).
				ibufferlen = strlen(szbuffer);

				if((ibytessent = send(s1,szbuffer,ibufferlen,0))==SOCKET_ERROR)
					throw "error in send in server program\n";
				
#pragma endregion

				if (strcmp(direction, "get") == 0)
				{
					GetFile(fileName);
				}
				else
					if(strcmp(direction, "put") == 0)
					{
						PutFileName(fileName);
					}
					else
						cout <<"Awaiting response from client..." <<endl;

				

			}//wait loop

		} //try loop

		//Display needed error message.

		catch(char* str) { cerr<<str<<WSAGetLastError()<<endl;}

		//close Client socket
		closesocket(s1);		

		//close server socket
		closesocket(s);

		/* When done uninstall winsock.dll (WSACleanup()) and exit */ 
		WSACleanup();

		getchar();
		return 0;
	}




