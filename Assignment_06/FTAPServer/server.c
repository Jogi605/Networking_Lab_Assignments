#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#include <dirent.h>
#include <sys/types.h>
#include <time.h>
#define MAX_BUFFER 500
#define MAX_LINE 20
#define PORT 4035


//Function to gt directory
void ListDir(int client_sock)
{
    char search[1024];
    bzero(search, 1024);
    
    //pointing to current directory
	DIR* dir = opendir(".");  
	
	//If Directory does not contain anything
	if(dir == NULL){
		strcpy(search, "NO FILES FOUND!!");
	}
	
	//strcat(search, "\n");
	
	//reading from that directory
	struct dirent* entity = readdir(dir);
	
	//Copy file names to buffer  
	while(entity != NULL){
		strcat(search, entity->d_name);
		strcat(search, "\n");
		entity = readdir(dir);
	}
	//strcat(search, "************!>");
	
	//Close the directory
	closedir(dir);
	
	//Write to the client
	//printf("%s\n", search);
	write(client_sock, search, strlen(search));
}

//Function to store file in the server
void StoreFile(int client_sock)
{
    
    char msg[500];
	bzero(msg, 500);
	
	//To measure expected time
	clock_t time_req;
	
	//start counting time
    time_req = clock();
    
	//Read file from the client
	if(read(client_sock, msg, sizeof(msg)) > 0){
		if(strcmp(msg, "Sendme") == 0){
			FILE * fp;
			fp = fopen("recv", "w");
			int bytes;
			int i = 0;
			
			printf("File Transfer starting\n");
			//Now send
			bzero(msg, 500);
			while(read(client_sock, msg, sizeof(msg)) >= 1){
				
				if(strcmp(msg, "Finished") == 0)break;
				
				//acknowledge by sending msg
				char bf[500];
				sprintf(bf,"Received");
				
				//Write to the client
				write(client_sock, bf, sizeof(bf));
				
				fprintf(fp, "%s", msg);
				i += 500;
				
				//Check data sent over MB
				if(i % 5000000 == 0)
				  printf("%d mb received\n", i / 1000000);
				bzero(msg, 500);	
			}
			
		}
		
		//Print error
		else{
		printf("%s", msg);
		fflush(stdout);
		}
	}
			
}

//Function to send file
void SendFile(FILE *fp, int client_sock)
{
             
	    char bf[500], search[500];
	    sprintf(search, "Sendme");
	    write(client_sock, search, sizeof(search));
	    
	    //To measure Expected time
	    clock_t time_req;
	    //start counting time
          time_req = clock();
          
	    int bytes, i = 0;
	    printf("File Transfer starting\n");
	    while( (bytes = fread(search, 1, 500, fp)) >= 1){
		 write(client_sock, search, sizeof(search));
		
		//Conformation
		bzero(bf, 500);
		read(client_sock, bf, sizeof(bf));
		i += 500;
		if(i % 5000000 == 0)
			printf("%d mb sent\n", i/1000000);
	}
	sprintf(search, "Finished");
	write(client_sock, search, sizeof(search));
	
}

//Function to search credentials in the txt file
int search_value(char buffer[]){
    char *line = NULL;
       size_t l = 0;
       ssize_t tbyte;
       
       //TXT file which contain login credentials
       FILE *fp = fopen("logincredit.txt", "r");
       
       //check if it's NULL
       if(fp == NULL)
           exit(1);
           
   //Now Get details and verify with each line     
    while((tbyte = getline(&line, &l, fp)!= -1))
    {
          //printf("%s\n", line);
         //check if username is there in substr
         if(strstr(line, buffer)!=NULL)
         {
            //printf("1 %s\n", buffer);
            return 1;
         }
     }
   return 0;
   }
   
static void CreateFile(int client_sock, char uname[])
{
    char buffer[1024];
    bzero(buffer, 1024);
    
    //Create a new file
	FILE* fp;
	
	
	//Add extensions
	strcat(uname, ".txt"); 
	fp = fopen(uname, "w");
	
	//Close the file
	fclose(fp);
	//send acknowledgement
	strcpy(buffer, "File Created Successfully!!");
	write(client_sock, buffer, strlen(buffer));
	
	
}         
//Driver Function   
int main()
{
       //Declare the necessary variables
       int server_sock, client_sock, n,m=0, k=0, b=0, flag =1;
       struct sockaddr_in6 server_addr, client_addr;
       char buffer[MAX_BUFFER],buffer1[MAX_BUFFER];
       socklen_t addr_size;
       
       //File to store
       FILE *fp1;
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
        
      //Initialise the address   
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
     if(listen(server_sock, 10) !=0)
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
     printf("Connection is setup\n");
    
    //Validate the user credentials
    memset(buffer, 0, MAX_BUFFER); 
    memset(buffer1, 0, MAX_BUFFER);
    
    //Validate credentials
    while(flag)
    {
    
    //Read the username from the user
    read(client_sock, buffer, sizeof(buffer));
    
    buffer[strcspn(buffer, "\n")] = 0; 
    
            //Search username
            b = search_value(buffer);
            if(b == 1)
            {
            
            //Send OK to user
             strcpy(buffer1, "OK");
             
            write(client_sock, buffer1, sizeof(buffer1));
            printf("Matched Username\n");
            memset(buffer, 0, MAX_BUFFER);
            //memset(buffer1, 0, MAX_BUFFER);
            
            //Read the password from the user
            read(client_sock, buffer, sizeof(buffer));
            buffer[strcspn(buffer, "\n") ] = 0; 
            //printf("%s\n", buffer);
            
            //Search password
            b = search_value(buffer);
            if(b == 1)
            {
                memset(buffer1, 0, MAX_BUFFER);
            
                //Send OK to user
                strcpy(buffer1, "OK");
                printf("Matched Password\n");
                //Send OK to user
                write(client_sock, buffer1, sizeof(buffer1));
                //printf("here-%s\n", buffer1);
                memset(buffer1, 0, MAX_BUFFER);
                flag  = 0;
                break;
            }
            
            //Invalid Password
            else
            {
              memset(buffer1, 0, MAX_BUFFER);
            
              //Send NOT MATCHED to user
                strcpy(buffer1, "PWD NOT MATCHED");
                printf("%s\n", buffer1);
                
                //Send NOT MATCHED to user
                write(client_sock, buffer1, sizeof(buffer1));
                continue;
            }
          }       
            else
            {
               memset(buffer1, 0, MAX_BUFFER);
            
               //Send NOT MATCHED to user
                strcpy(buffer1, "USER NOT MATCHED");
                printf("%s\n", buffer1);
                
                //Send NOT MATCHED to user
                write(client_sock, buffer1, sizeof(buffer1));
                continue;
         }           
                               
     }
   
   //printf("check1\n");
   //Invalid credentials
   if(flag == 1)
   {
     memset(buffer1, 0, MAX_BUFFER);
            
     //Send INVALID CREDENTIALS to user
     strcpy(buffer1, "INVALID CREDENTIALS");
            
     //Send INVALID CREDENTIALS to user
     write(client_sock, buffer1, sizeof(buffer1));
     close(server_sock);
     exit(1);
   }
   
   //Now listen to the queries
   while(1)
   {
     //Clear the buffers
      memset(buffer, 0, MAX_BUFFER);
      memset(buffer1, 0, MAX_BUFFER);
      //printf("@@server@@->");
      
      //read query
      read(client_sock, buffer, sizeof(buffer));
      
      //Check for Exit condition
      if(strcmp(buffer, "QUIT\n") == 0)
      {
          //send to the server
          write(client_sock, buffer, sizeof(buffer));
          close(client_sock);
          printf("Server Exit....\n");
          break;
      }
      
      //List Directories
      else if(strcmp(buffer, "ListDir\n") == 0)
      {
           ListDir(client_sock);
      }
      
      //List Directories
      else if(strcmp(buffer, "CreateFile\n") == 0)
      {
           read(client_sock, buffer1, sizeof(buffer1));
           //printf("%s\n", buffer1);
          
           CreateFile(client_sock, buffer1);
           memset(buffer1, 0, MAX_BUFFER);
      }
      //Get file
      else if(strcmp(buffer, "GetFile\n") == 0)
      {
            fp1 = fopen(file_name, "r");
            printf("@@Server@@::The file is: %s\n",file_name);
			printf("The file is opened!\n");
			
            //If the file doesnot contain anything
            if(fp1 == NULL)
            { 
                perror("@@Server@@::Error in reading file"); 
                break;
            }
                        
            //Send the file
            SendFile(fp1, client_sock);
      }
      
      //Store file
      else if(strcmp(buffer, "StoreFile\n") == 0)
      {
           StoreFile(client_sock);
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
   
     //printf("Server Exit...\n"); 
    //close the connection
    close(server_sock);  
          
return 0;
}
