#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include <errno.h>
#include <netinet/in.h>


#define MAX_BUFFER 200
#define PORT 3000

int guard(int n, char *err)
{
   if(n == -1)
   {
      perror(err);
      exit(1);
   }
   return n;
 }
int main()
{
       int server_sock, client_sock, n=0, length, m = 0;
       struct sockaddr_in6 server_addr, client_addr;
       char buffer[MAX_BUFFER], details[15];
       memset(buffer, 0, MAX_BUFFER);
       memset(details, 0, 15);
       char buffer2[MAX_BUFFER];
       memset(buffer2, 0, MAX_BUFFER);
      // memset(details, 0, 15);
       
       
       //Give timeout values 
       struct timeval tv;
       tv.tv_sec = 1;
       tv.tv_usec = 100;
       
       //Declare a file descriptor set
       fd_set file_d1;
       
       //Initialize the set
       FD_ZERO(&file_d1);
       
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
      
      //Provide the identity before connecting to the server
      printf("Enter your Name: ");
      n=0;
     while ((details[n++] = getchar()) != '\n') ;
      
      length = strlen(details);
      details[length - 1] = '\0';
        
      //connect to the server
      server_sock = connect(client_sock, (struct sockaddr*)&server_addr,sizeof(server_addr));
      
      //Now Validate
      if(server_sock<0)
      {
           perror("Connection to the socket failed!\n");
           exit(1);
      }
      
      else
          printf("Connected to the server\n");
      
      //Welcome note
      printf("****************WELCOME TO THE CHAT SYSTEM**************\n");
      printf("Heartly Welcome: %s\n", details);
      
      //Send your details to the server
      m = write(client_sock, details, sizeof(details));
      memset(buffer, 0, MAX_BUFFER);
      //read(client_sock, buffer, sizeof(buffer));
      //printf("%s\n", buffer);
      //Validate
      if(m<0)
      {
           perror("Couldn't write to the client\n");
           exit(1);
      }
      
     int status = fcntl(client_sock, F_SETFL, fcntl(client_sock, F_GETFL, 0) | O_NONBLOCK);
     if(status == -1)
        perror("calling fcntl!\n");
      while(1)
      {
         
         //Clear the buffer
         memset(buffer, 0, MAX_BUFFER);
         memset(buffer2, 0, MAX_BUFFER);
         n = read(client_sock, buffer2, sizeof(buffer2));
	      if(n>0)
	      {
	      printf("%s\n", buffer2);
	       
	     }
	  memset(buffer2, 0, MAX_BUFFER);
         scanf("%[^\n]s", buffer);
        buffer[strcspn(buffer,"\n")] = 0;
        if(write(client_sock,buffer,strlen(buffer))<0)//Write the accepted message into the socke
        {
            perror("Could not write from the client");
            exit(1);
        }   
         
         //Exit the connection		
         if (strncmp("tata",buffer, 4) == 0) { 
			printf("I am leaving! Byee...\n"); 
			break;
         }
        
        //Retrieve Queries from the server 
	 memset(buffer, 0, MAX_BUFFER);
	 
	 //Register the fd_set
	 FD_SET(client_sock, &file_d1);
	 
	 //Now do select
	 m = select(client_sock+1, &file_d1, NULL, NULL, &tv);
	 
	 //If timeout has occured
	 if(m == 0)
	      continue;
	      
	//If unable to read the other messages or an error occured
        else if(m<0)
	 { 
	      perror("Unable to read other Messages\n");
	      exit(1);
	 } 
	      
			 
	      
	 
	 else
        {
             n=read(client_sock,buffer2,sizeof(buffer2));
			 if(n == 0)
			 {
				 printf("Server hung up..\n");
				 break;
			 }
			 length = strlen(buffer2);
			 if( buffer2[length-1] == '\n' )
    				buffer2[length-1] = 0;
		}
	 //If the other client exits
	 if(strstr(buffer2, "tata")!=NULL)
	 {
	      char strnew[33];
	      char* a = strchr(buffer, ':');
	      int b = (a == NULL ? -1 : a - buffer2);
	      printf("%.*s", b, buffer2);
	      printf(" has left the chat : \n");
	      memset(buffer2,0, sizeof(buffer2));
	      continue;
	 }   
	 if(!(strcmp(buffer2, " ")&&buffer2[0]=='\0')) 
         printf("%s\n",buffer2); 
	 memset(buffer, 0, sizeof(buffer));
	 memset(buffer2, 0, sizeof(buffer2));
	 
      }
    
   // clr_fl(STDOUT_FILENO, O_NONBLOCK);
       //Close the coonection
       close(client_sock); 
          
return 0;
}
