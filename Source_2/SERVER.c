/*
    Bind socket to port 8888 on localhost
		http://stackoverflow.com/questions/27834419/winsock-command-line-on-a-remote-server
*/
 
#include <windows.h>
#include <shlwapi.h>
#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
 
#pragma comment(lib,"ws2_32.lib") //Winsock Library

int total_connections;

// You cant use socket to redirect CreateProcess IO, insteand, you need to create CreatePipe.
HANDLE std_in;
HANDLE std_out;
    

void shell (int s, char* arg)
{
	PROCESS_INFORMATION pi;
	STARTUPINFO         si;

	ZeroMemory (&si, sizeof(si));
	ZeroMemory (&pi, sizeof(pi));

	si.cb         = sizeof(si);
	// redirect all input/output through socket
	si.dwFlags    = STARTF_USESTDHANDLES;
	si.hStdInput  = (HANDLE)s;
	si.hStdOutput = (HANDLE)s;
	si.hStdError  = (HANDLE)s;

	printf ("[SERVER]: Executing cmd.exe\n");
	// execute cmd.exe
	if (CreateProcess (NULL, "cmd", arg, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi))
	{
		printf ("[SERVER]: Running \n");
		// wait until it terminates
		WaitForSingleObject (pi.hProcess, INFINITE);

		CloseHandle (pi.hThread);
		CloseHandle (pi.hProcess);
		printf("[SERVER]: CMD closed \n");
	} else {
		printf("[SERVER]: Error while create process");
	}
}

 
int main(int argc , char *argv[])
{
    WSADATA wsa;
    SOCKET s , new_socket;
    struct sockaddr_in server , client;
    int c;
	int recv_size;
	char *message;
	char client_message[2000];
    printf("\n[SERVER]: Initialising Winsock...");
    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
    {
        printf("[SERVER]: Failed. Error Code : %d",WSAGetLastError());
        return 1;
    }
     
    printf("Initialised.\n");
     
    //Create a socket
    if((s = socket(AF_INET , SOCK_STREAM , 0 )) == INVALID_SOCKET)
    {
        printf("[SERVER]: Could not create socket : %d" , WSAGetLastError());
    }
 
    printf("[SERVER]: Socket created.\n");
     
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 2708 );
     
    //Bind
    if( bind(s ,(struct sockaddr *)&server , sizeof(server)) == SOCKET_ERROR)
    {
        printf("[SERVER]: Bind failed with error code : %d" , WSAGetLastError());
    }
     
    puts("[SERVER]: Bind done");
     
 
    //Listen to incoming connections
    listen(s , 3);
     
    //Accept and incoming connection
    puts("[SERVER]: Waiting for incoming connections...");
     
    c = sizeof(struct sockaddr_in);
    while( (new_socket = accept(s , (struct sockaddr *)&client, &c)) != INVALID_SOCKET )
    {
        total_connections++;
		
		puts("[SERVER]: Connection accepted");
        printf("[SERVER]: IP: %d.%d.%d.%d PORT: %d\n", int((client.sin_addr.s_addr&0xFF)), int((client.sin_addr.s_addr&0xFF00)>>8),
								int((client.sin_addr.s_addr&0xFF0000)>>16), int((client.sin_addr.s_addr&0xFF000000)>>24), client.sin_port);
        if((recv_size = recv(new_socket, client_message , 2000 , 0)) == SOCKET_ERROR)
		{
			puts("[SERVER]: recv failed");
		}
		printf("[CLIENT %d]: Message from client: %s\n", total_connections, client_message);
		//Reply to the client
        message = "[SERVER]: Message received\n";
        send(new_socket , message , strlen(message) , 0);
		if (!WriteFile(ti->hStdIn, buffer, ret, &BytesWritten, NULL))
            break;
		
		shell(new_socket, client_message);
		
    }
     
    if (new_socket == INVALID_SOCKET)
    {
        printf("accept failed with error code : %d" , WSAGetLastError());
        return 1;
    }
 
    closesocket(s);
    WSACleanup();
 
    return 0;
}


