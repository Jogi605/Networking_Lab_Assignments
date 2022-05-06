#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include <time.h>

#define MAX_BUFFER 500
#define MAX_LINE 20
#define PORT 4035

//Function to get directory
void ListDir(int client_sock)
{
    char buff2[MAX_BUFFER];
    memset(buff2, 0, MAX_BUFFER);
	read(client_sock, buff2, sizeof(buff2));
	printf("%s\n", buff2);
}

//Function to store file in the server
void StoreFile(FILE *fp, int client_sock)
{
            
			char bf[500], search[500];
	       sprintf(search, "Sendme");
	       write(client_sock, search, sizeof(search));
	    int bytes, i = 0;
	    //printf("File Transfer starting\n");
	    clock_t time_req = clock(); 
	    while( (bytes = fread(search, 1, 500, fp)) >= 1){
		 write(client_sock, search, sizeof(search));
		
		//Conformation
		bzero(bf, 500);
		read(client_sock, bf, sizeof(bf));
		i += 500;
		
	}
	time_req = time_req - clock();
	sprintf(search, "Finished");
	write(client_sock, search, sizeof(search));
	float rtt = (float) time_req;
    printf("Time it took::%f \n", rtt);
	printf("%s\n", search);
}

//Function to retrieve the file
void GetFile(int client_sock)
{
           
	char msg[500];
	bzero(msg, 500);
	if(read(client_sock, msg, sizeof(msg)) > 0){
		if(strcmp(msg, "Sendme") == 0){
			FILE * fp;
			fp = fopen("recv", "w");
			int bytes;
			int i = 0;
			
			bzero(msg, 500);
			clock_t time_req = clock(); 
			while(read(client_sock, msg, sizeof(msg)) >= 1){
				
				if(strcmp(msg, "Finished") == 0)break;
				
				//acknowledgement
				char bf[500];
				sprintf(bf,"Received");
				
				write(client_sock, bf, sizeof(bf));
				
				fprintf(fp, "%s", msg);
				i += 500;
				
				bzero(msg, 500);	
			}
			printf("Recieved\n");
			
			time_req = time_req - clock();
			float rtt = (float) time_req;
            printf("Time it took::%f \n", rtt);	
		}
		else{
		printf("%s", msg);
		fflush(stdout);
		}
	}

}

//Driver Function
int main()
{
       int server_sock, client_sock, n=0;
       struct sockaddr_in6 server_addr, client_addr;
       char buffer[MAX_BUFFER], input[MAX_LINE], buffer1[MAX_BUFFER], uname[MAX_LINE];
       FILE *fp;
       char *file_name = "send";
        
       //socket length
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
           printf("Client Socket Created Successfully!\n");
      
      //Get START command to initiate the connection 
      fgets(input, MAX_LINE, stdin);
       
       //Let the client open the connection
       if(strcmp(input, "START\n") != 0)
          exit(1);
        
       memset(input, 0, MAX_LINE);
           
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
          printf("200 OK Connection is set up\n");
      
      memset(input, 0, MAX_LINE);
      memset(buffer, 0, MAX_BUFFER);
      //Now validate with user credentials 
      while(1)
      {
         memset(uname, 0, MAX_LINE);
         //Get input from console
         fgets(input, MAX_LINE, stdin);
         
         //Compare with commands
         if(strcmp(input, "USERN\n") == 0)
         {
            memset(buffer, 0, MAX_BUFFER);
            memset(buffer1, 0, MAX_BUFFER);
            
            //Enter username for validation
            fgets(buffer, MAX_BUFFER, stdin);
            strcpy(uname, buffer);
            uname[strcspn(uname, "\n")] = 0; 
            
            //send to the server
            write(client_sock, buffer, sizeof(buffer));
            
            //Get reply from the server
	       // memset(buffer1, 0, MAX_BUFFER);
	        read(client_sock, buffer1, sizeof(buffer1));
	        
	        //Validation check
	        if(strcmp(buffer1, "OK") == 0)
	        {
	           printf("300 Correct username::Need Password\n");
	           
	           //Enter the password
	           //memset(input, 0, MAX_LINE);
	           fgets(input, MAX_LINE, stdin);
	           if(strcmp(input, "PASSWD\n") == 0)
	           {
	                  memset(buffer, 0, MAX_BUFFER);
                      memset(buffer1, 0, MAX_BUFFER);
                      
                     //Enter username for validation
                     fgets(buffer, MAX_BUFFER, stdin);
            
                     //send to the server
                     write(client_sock, buffer, sizeof(buffer));
            
                     //Get reply from the server
	                  memset(buffer1, 0, MAX_BUFFER);
	                  read(client_sock, buffer1, sizeof(buffer1));
	                  //printf("%s\n", buffer1);
	                  //strcpy(buffer1, "OK");
	                  //Validate
	                  if(strcmp(buffer1, "OK") == 0)
	                  {
	                      //successfully authenticated
	                      printf("305 User Authenticated with password\n");
	                      printf("  ***WELCOME!! :: %s ***\n", uname);
	                      break;
	                  }
	                  
	                  else
	                  {
	                    printf("310 Incorrect Password\n");
	                    continue;
	                    //continue;
	                  }
	           }
	           else
	           {
	               printf("505 Command not Supported\n");
                   continue;
	           }
	        }
	        
	        //Entered wrong user name
	        else
	        {
	            printf("301 Incorrect Username\n");
	            continue;
	        } 
         }
         
         //Enter a valid coomand
         else
         {
           printf("505 Command not Supported\n");
           continue;
         }
             
    }
    
    //Now Move to the queries
    while(1)
    {
      //Clear the buffers
      memset(buffer, 0, MAX_BUFFER);
      memset(buffer1, 0, MAX_BUFFER);
      printf("@@client@@->");
      
      //Get queries
      fgets(buffer, MAX_BUFFER, stdin);
      
      //Check for Exit condition
      if(strcmp(buffer, "QUIT\n") == 0)
      {
          //send to the server
          write(client_sock, buffer, sizeof(buffer));
          printf("Client Exit....\n");
          break;
      }
      
      //send to the server
      write(client_sock, buffer, sizeof(buffer));
      
      //List Directories
      if(strcmp(buffer, "ListDir\n") == 0)
      {
          // ListDir(client_sock);
          read(client_sock, buffer, sizeof(buffer));
          printf("%s\n", buffer);
          memset(buffer, 0, MAX_BUFFER);
      }
      
      //Get file
      else if(strcmp(buffer, "GetFile\n") == 0)
      {
            GetFile(client_sock);
      }
      
      //Create file
      else if(strcmp(buffer, "CreateFile\n") == 0)
      {
            printf("Enter the File name:");
            //Get input from console
            memset(input, 0, MAX_LINE);
            fgets(input, MAX_LINE, stdin);
            write(client_sock, input, sizeof(input));
            
            //Clear and get acknowledgement
            memset(buffer, 0, MAX_BUFFER);
            read(client_sock, buffer, sizeof(buffer));
            printf("%s\n", buffer);
            memset(buffer, 0, MAX_BUFFER);
      }
      //Store file
      else if(strcmp(buffer, "StoreFile\n") == 0)
      {
            fp = fopen(file_name, "r");
            printf("The file is: %s\n",file_name);
			//printf("The file is opened!\n");
			
            //If the file doesnot contain anything
            if(fp == NULL)
            { 
                perror("Error in reading file"); 
                break;
            }
           StoreFile(fp, client_sock);
      }
      
      //Print the result from server
      else
      {
          read(client_sock, buffer1, sizeof(buffer1));
          printf("%s\n", buffer1);
          memset(buffer1, 0, MAX_BUFFER);
      }
      
      //clear the buffer
      memset(buffer, 0, MAX_BUFFER);
    }
    
  //Close the coonection
  close(client_sock); 
          
return 0;
}
