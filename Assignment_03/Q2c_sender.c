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
#include <time.h>


#define MAX_BUFFER 500
#define PORT 8080


int main(){

       int server_sock, client_sock, n=0;
       struct sockaddr_in6 server_addr, client_addr;
       char buffer[MAX_BUFFER];
       struct timeval tv;
       clock_t time_req;
       socklen_t addr_size;
       int m;
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
      
       //Give timebound of 1 second 
      tv.tv_sec = 1;
      tv.tv_usec = 0;
      
      //Initial frame will be 0
      int frame = 0;
      int count =5;
      char buffer1[MAX_BUFFER];
      memset(buffer1, 0, MAX_BUFFER);
      
     while(count--){
     
          //start counting time
          time_req = clock();
          memset(buffer, 0, MAX_BUFFER);
         if( frame==0)
             strcat(buffer, "0");
             
          else
              strcat(buffer, "1");
          n = 1;
          while ((buffer[n++] = getchar()) != '\n') ;
          printf("%s\n", buffer);
          write(client_sock, buffer, sizeof(buffer));
          
          
          memset(buffer1, 0, MAX_BUFFER);
          time_req = time_req - clock();
          read(client_sock, buffer1, sizeof(buffer));
          
         
          if(buffer1[0] == '1')
          {
               if(time_req <= tv.tv_sec && buffer[0] == '0')
                  {
                     memset(buffer,0,MAX_BUFFER);
                     frame = 1;
                  }
                  
                  else 
                  {
                     write(client_sock, buffer, sizeof(buffer));
                     time_req = clock();
                  }
          }
          
          else if(buffer1[0] == '0')
          { 
                if(time_req <= tv.tv_sec && buffer[0] == '1')
                {
                   memset(buffer,0,MAX_BUFFER);
                   frame = 0;
                }
                else 
                  {
                     write(client_sock, buffer, sizeof(buffer));
                     time_req = clock();
                  }
          }
          else if(count == 0)
               {
                        memset(buffer, 0, MAX_BUFFER);
	                 char prompt[4] = "exit";
	                strncpy(buffer,prompt,4);
	                write(client_sock, buffer, sizeof(buffer));
                        if (strncmp("exit",buffer, 4) == 0) { 
			printf("Client Exit...\n"); 
			break;
         }}
          float rtt = (float) time_req/CLOCKS_PER_SEC;
          printf("%f\n", rtt);
          
          
     
      }
      
       //Close the coonection
       close(client_sock); 

return 0;
}
