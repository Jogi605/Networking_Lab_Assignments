#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <netdb.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAX_BUFFER 200
#define PORT 8024
#define MAX_LINE 500
#define SIZE 500

ssize_t count = 0;

void send_file(FILE *fp, int client_sock){

                 char msg[500], buffer[500];
                 memset(buffer, 0, MAX_LINE);
                 memset(msg, 0, MAX_LINE);
                 //Acknowledge 
                 sprintf(msg, "Sending file");
                 
                 
                 //Send first 50 bytes
	         send(client_sock, msg, strlen(msg), 0);
			
			
			
			int bytes;
			printf("File Transfer started\n");
			
			//Now send remaining file
			while( (bytes = fread(msg, 1, 500, fp)) >= 1)
			{
				send(client_sock, msg, strlen(msg), 0);
				
				
				memset(buffer, 0, MAX_LINE);
				recv(client_sock, buffer, sizeof(buffer), 0);
				
			}
			
			//Stop sending when everything is finished
			sprintf(msg, "stop");
			
			//send this message to client
			send(client_sock, msg, strlen(msg), 0);
			printf("%s\n", msg);
}
/*
void send_file(FILE *fp, int client_sock){

     char buffer[MAX_LINE];
     memset(buffer, 0, MAX_LINE);
      int n = 0, m;
     n = fread(buffer, 1, MAX_LINE, fp);
     while(n)
     {
              
            if(send(client_sock, buffer, strlen(buffer), 0) == -1)
            {
                 perror("Unable to send");
                 exit(1);
            }
            
            memset(buffer, 0, MAX_LINE);
            n = fread(buffer, 1, MAX_LINE, fp);
           // bzero(buffer, SIZE);
     }
     memset(buffer, 0, MAX_LINE);
     }

*/
int main()
{
       int server_sock, client_sock, n,m=0;
       struct sockaddr_in6 server_addr, client_addr;
       char buffer[MAX_BUFFER];
       socklen_t addr_size;
       FILE *fp;
       
       char *file_name = "send";
       
       //Create a socket and validate it: socket(IPV6, TCP, IP);
       server_sock = socket(AF_INET6, SOCK_STREAM, 0);
      
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
      server_addr.sin6_port = htons(PORT);
      server_addr.sin6_addr = in6addr_any;
      
      //Now bind the address with created socket
      n = bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
      
      // Validate
      if(n<0)
      {
          perror("Socket Bind Failed\n");
          exit(1);
      }
      
      else     
          printf("Socket Binded Successfully!\n");
          
     // Now listen to the client and validate
     if(listen(server_sock, 5) !=0)
     {
         perror("Listening failed\n");
         exit(1);
     }
     
     else
         printf("The server is listening\n");
      
     // Accept the connection
     addr_size = sizeof(client_addr);
     client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_size);
     
     //Validate
     if(client_sock<0)
     {
          perror("Could not accept the connection\n");
          exit(1);
     }
     
     else
          printf("Accepted the Client\n");
     
    //Now listen to the queries      
    while(1)
    {
         memset(buffer, 0, MAX_BUFFER);
         
         //Read the Message from the Client 
         read(client_sock, buffer, sizeof(buffer));
         printf("Message From client:%s", buffer);
         
         //Exit if client sends 'exit' msg
         if(strncmp("Bye",buffer,3) == 0)
         {
                        memset(buffer, 0, MAX_BUFFER);
	               
	               //Closing the client connection
	                close(client_sock);
			 printf("Server Exit...\n"); 
			 break;
	          
         }
         
         //Sending the 50MB size video
         else if(strncmp("GivemeYourVideo",buffer,15) == 0)
         {
                      fp = fopen(file_name, "r");
                        printf("The file is: %s\n",file_name);
			 printf("The file is opened!\n");
			
                        //If the file doesnot contain anything
                        if(fp == NULL)
                        { 
                              perror("Error in reading file"); 
                              break;
                        }
                        
                        //Send the file
                        send_file(fp, client_sock);
                       
			 printf("Sent Successfully!\n");
			 memset(buffer, 0, MAX_BUFFER);
	          
         }
         else
         {
              
              memset(buffer, 0, MAX_BUFFER);
              m=0;
              char prompt[10] = "sabfnvginb";
              strncpy( buffer, prompt, 10);
              strcat(buffer, "\n");
              write(client_sock, buffer, sizeof(buffer));
               memset(buffer, 0, MAX_BUFFER);
         }
    }
    
    //close the server connection
    close(server_sock);  
          
return 0;
}
