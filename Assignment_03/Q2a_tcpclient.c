#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>


#define MAX_BUFFER 200
#define MAX_LINE 500
#define PORT 8024
#define SIZE 500

ssize_t count = 0;

void write_file(int client_sock)
{
                       char msg[500];
			 memset(msg, 0, MAX_LINE);
			 
			 //Start recieving the file
			if(recv(client_sock, msg, sizeof(msg), 0) > 0){
				if(strcmp(msg, "Sending file") == 0){
				
				//open the file and write into it
					FILE * fp;
					fp = fopen("recv", "w");
					 memset(msg, 0, MAX_LINE);
					 
					 //Now continously recieve the file
					while(recv(client_sock, msg, sizeof(msg), 0) >= 1){
						
						//Stop if the trnasfer is finished
						if(strcmp(msg, "stop") == 0)break;
						
						
						char buffer[500];
						sprintf(buffer,"Received");
						
						send(client_sock, buffer, strlen(buffer), 0);
						
						fprintf(fp, "%s", msg);
						 memset(msg, 0, MAX_LINE);
						} 
					     printf("File recieved successsfully\n");
						
				}
				else{
				printf("%s", msg);
				fflush(stdout);
				}
			}
			
}
/*
void write_file(int client_sock)
{
     FILE *fp = fopen("recv.txt", "w");
    // ssize_t count = 0; // To count number of bytes
     int n;
     
     char buffer[MAX_LINE];
     if(fp == NULL)
         {
         perror("Couldn't write into the file!\n");
         }
     
     n = recv(client_sock, buffer, sizeof(buffer), 0);
     for(int i=0; i<100000; i++)
     {
        
        count+=n;
        
        if(n<=0)
            break;
            
         //writing into the file
         fprintf(fp, "%s", buffer);
         memset(msg, 0, MAX_LINE);
         
         n = recv(client_sock, buffer, sizeof(buffer), 0);
     }
     
     printf("File recieved successfully!\n");
     return;    
}
*/
int main()
{
       int server_sock, client_sock, n=0;
       struct sockaddr_in6 server_addr, client_addr;
       char buffer[MAX_BUFFER];
       char *file_name = "recv.txt";
       
       socklen_t addr_size;
       
       //Create a socket and validate it: socket(IPV6, TCP, IP);
       client_sock = socket(AF_INET6, SOCK_STREAM, 0);
       
       //Validate
       if(server_sock<0)
       { 
            perror("Socket Creation failed\n");
            exit(1);
       }
       
       else
           printf("Server Socket Created Successfully!\n");
           
      memset(&server_addr, 0, sizeof(server_addr));
      
      //Assign IP address, port
      server_addr.sin6_family  = AF_INET6;
      inet_pton(AF_INET6, "::1", &server_addr.sin6_addr);
      server_addr.sin6_port = htons(PORT);
      
      //connect to the server
      server_sock = connect(client_sock, (struct sockaddr*)&server_addr,sizeof(server_addr));
      
      //Now Validate
      if(server_sock<0)
      {
           perror("Connection to the socket failed!\n");
           exit(1);
      }
      
      else
          printf("Successfully connected to the server\n");
      
      //Now listen to the queries      
      while(1)
      {
         //Clear the buffer
         memset(buffer, 0, MAX_BUFFER);
         printf("Message to server : ");
         
         //Send Queries to the client
         n=0;
         while ((buffer[n++] = getchar()) != '\n') ;
         write(client_sock, buffer, sizeof(buffer));
         
          //Exit the connection	
         if (strncmp("Bye",buffer, 3) == 0) {
			printf("Client Exit...\n"); 
			break;
         }
         //If the client requests video then write data into file
         else if(strncmp("GivemeYourVideo", buffer, 15) == 0)
         {
              
              write_file(client_sock); 
         }
         
         else
         {
        //Retrieve Queries from the server 
	 memset(buffer, 0, MAX_BUFFER);
	 read(client_sock, buffer, sizeof(buffer));
	 printf("Message from Server : %s",buffer); 
	 }
		
         
      }
    
       //Close the coonection
       close(client_sock); 
          
return 0;
}
