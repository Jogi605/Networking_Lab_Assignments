#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_BUFFER 200
#define PORT 8080

int main()
{
       int server_sock, client_sock, n=0;
       struct sockaddr_in6 server_addr, client_addr;
       char buffer[MAX_BUFFER];
       
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
        
        //Retrieve Queries from the server 
	 memset(buffer, 0, MAX_BUFFER);
	 read(client_sock, buffer, sizeof(buffer));
	 printf("Message from Server : %s\n",buffer); 
	 
	 //Exit the connection		
         if (strncmp("exit",buffer, 4) == 0) { 
			printf("Client Exit...\n"); 
			break;
         }
      }
    
       //Close the coonection
       close(client_sock); 
          
return 0;
}
