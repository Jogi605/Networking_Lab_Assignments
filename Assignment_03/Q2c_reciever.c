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
#include <sys/time.h>

#define MAX_BUFFER 500
#define PORT 8080


//Driver Function
int main(){

       int server_sock, client_sock, n,m=0;
       struct sockaddr_in6 server_addr, client_addr;
       char buffer[MAX_BUFFER];
       socklen_t addr_size;
       
       
       
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
     
     int ack = 0;
     int count = 5;
     while(count--)
     {
          memset(buffer, 0, MAX_BUFFER);
          read(client_sock, buffer, sizeof(buffer));
                 
          //If the sender sends frame 0
          if(buffer[0] == '0')
                 {
                    memset(buffer, 0, MAX_BUFFER);
                    char s[1] = "0";
                    strncpy(buffer, s, 1);
                    write(client_sock, buffer, sizeof(buffer));
                    continue;
                 }
             
               //If the sender sends frame 1
          else if(buffer[0] == '1')
               {
                    memset(buffer, 0, MAX_BUFFER);
                    char s[1] = "0";
                    strncpy(buffer, s, 1);
                    write(client_sock, buffer, sizeof(buffer));
                    continue;
               }
               
       //If the sender finishes and close the connection         
        else if(strncmp("exit",buffer, 4) == 0)
         {
                        memset(buffer, 0, MAX_BUFFER);
	                m=0;
	                char prompt[4] = "exit";
	                strncpy(buffer,prompt,4);
	                write(client_sock, buffer, sizeof(buffer));
			 printf("Server Exit...\n"); 
			 break;
         }
     }
     
     //close the connection
    close(server_sock);  
return 0;
}
