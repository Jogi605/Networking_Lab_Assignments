#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>

#define MAX_BUFFER 200
#define PORT 3000

int main()
{
       int server_sock, client_sock, n,m=0, l =1,max_fd,k,i,j,a;
       struct sockaddr_in6 server_addr, client_addr;
       socklen_t addr_size;
       
       // Declare arrays for messages
       char buffer[MAX_BUFFER]; // To handle input output buffer
       char details[MAX_BUFFER][15];  //To store the details of each client
       char msg[MAX_BUFFER]; // to send the msg to all clients
       char get_name[15]; //to get name of client
       memset(details, 0, sizeof( *details) *15);
       
       
       
       //declare file descriptors
       fd_set file_d1;
       fd_set read_fds;
       
       //Reset all to 0
       FD_ZERO(&file_d1);
       FD_ZERO(&read_fds);
       
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
      
      //Set socketopt
      if(setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &l, sizeof(int)) == -1)
	{
		perror("Setsocketopt-error!\n");
		exit(1);
	}
	printf("Setsockopt() is fine\n");
      
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
          
     FD_SET(server_sock, &file_d1);
     // Now listen to the client and validate
     if(listen(server_sock, 50) !=0)
     {
         perror("Listening failed\n");
         exit(1);
     }
     
     else
         printf("The server is listening\n");
      
     // Accept the connection
     addr_size = sizeof(client_addr);
    
     
    max_fd = server_sock;
    
     
     
    //Now listen to the queries      
    while(1)
    {
                  read_fds = file_d1;
                  k = select(FD_SETSIZE+1, &read_fds, NULL, NULL, NULL);
                  if(k<0)
                  {
                      perror("Select error!\n");
                      exit(1);
                  }
                  
                  for(i=0; i<FD_SETSIZE; i++)
                  {
                      //check whether the listener socket is ready or not and any new connection is making
                      if(FD_ISSET(i, &read_fds))
                      {
                         //Now handle this new connection
                         if(i == server_sock)
                         {
                             addr_size = sizeof(client_addr);
                             client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_size);
                             
                             //Validate
                             if(client_sock<0)
                             {
                                    perror("Could not accept the connection\n");
                                     
                             }
                             // printf("Connected with Client:%d\n", client_sock-3);
                           
                            //Now connection has been established
                            else
                            {
                             //Add this descriptor to set
                             FD_SET(client_sock, &file_d1);
                             
                             //Update maximum file descriptors
                             if(client_sock > max_fd)
                                    max_fd = client_sock;
                                    
                             //Now get the name of the client
                             memset(get_name, 0, 15);
                             memset(details[client_sock], 0, sizeof(details[client_sock]));
                             
                             //get the name of the client
                             read(client_sock, details[client_sock], sizeof(details[client_sock]));
                             strncpy(get_name, details[client_sock], sizeof(details[client_sock]));
                             
                             //Print in the server
                             printf("%s connected to the server from client: %d\n", get_name, client_sock);
                             
                             //Make a message to inform all the other clients 
                             memset(msg, 0, MAX_BUFFER);
                             strncpy(msg, get_name, sizeof(get_name));
                             char prompt[18] = "has joined to talk";
                             strncat(msg,prompt,18);
                             strcat(msg, "\n");
                             
                             //Now get the active users and inform the new client 
                             int check = 1;
                             char active[MAX_BUFFER];
                             memset(active, 0, MAX_BUFFER);
                             strcpy(active, "Active users are:\n");
                             //Get all the active users
                             for(j=0; j<= max_fd; j++)
                             {
                                //the other client which is neither server nor new client
                                if(FD_ISSET(j, &file_d1) && ( j!=client_sock && j!=server_sock ))
                                {
                                   strncat(active, details[j], sizeof(details[j]));
                                   strcat(active, "\n");
                                   check = 0;
                                }
                                
                             }
                             
                             //If there are no active users
                             if(check == 1)
                                 {
                                  char prompt1[16] = "No one is there!";
                                  strncat(active,prompt1,16);
                                  strcat(active, "\n");
                                 }
                             
                             
                             //Now send this message
                             write(client_sock, active, sizeof(active));
                             
                             //Now inform all the users regarding the new user
                             for(j=0; j<= FD_SETSIZE; j++)
                             {
                                 if(FD_ISSET(j, &file_d1) && ( j!=client_sock && j!=server_sock ))
                                 {
                                     if(write(j, msg, sizeof(msg)) == -1)
                                     {
                                          perror("Could not send the joined user!\n");
                                     }
                                     
                                     // If the user sends exit to leave the chat
                                     if(strncmp("tata", buffer, 4) == 0)
                                     {
                                        printf("You left the chat\n");
                                        break;
                                     }
                                 } 
                           }         
                             
                      }
                    }
                         
                      //Now get the data from already existing clients
                      else
                      {
                            memset(buffer, 0, MAX_BUFFER);
                            a = recv(i, buffer, sizeof(buffer), 0);
                            //get data
                            if(a <= 0)
                            {
                                if(n<0)
                                    perror("Unable to read the message!\n");
                                  
                               else if(n == 0)
                                    printf(" %s:socket %d is hung up!\n", get_name, i);
                               
                               //Now close the connection 
                               close(i);
                               
                               //Remove form the file descriptor
                               FD_CLR(i, &file_d1);
                      
                          }
                          
                          //since a>0 ; we got the data
                          else
                          {
                            //Get the username
                            memset(msg, 0, MAX_BUFFER);
                            strncpy(msg, details[i], sizeof(details[i]));
                            strcat(msg, ": ");
                            
                            // copy the data which the user sent
                            strncat(msg, buffer, sizeof(buffer));
                            
                            //If the user send exit
                            if(strncmp("exit", buffer, 4) == 0)
                             {
                                 //send back the exit
                                 write(i, msg, sizeof(msg));
                                 printf("%s has left the chat!\n", details[i]);
                                 
                             }
                             
                            else
                                printf("%s : %s\n", details[i], buffer);
                            
                            //Now send the message to everyone
                            for(j=0; j <= FD_SETSIZE; j++)
                            {
                                 if(FD_ISSET(j, &file_d1) && ( j!=i && j!=server_sock ))
                                 {
                                     if(write(j, msg, sizeof(msg)) == -1)
                                     {
                                          perror("Could not send the joined user!\n");
                                     }
                                     
                                     /*// If the user sends exit to leave the chat
                                     if(strncmp("exit", buffer, 4) == 0)
                                     {
                                        printf("You lef the chat\n");
                                        break;
                                     }*/
                                 } 
                            }
                            
                            //If the client exit
                            if(strncmp("tata", buffer, 4) == 0)
                            {
                               //clear the descriptor
                               FD_CLR(i, &file_d1);
                               
                            }
                        }
                            //clear the buffers
                            memset(buffer, 0, MAX_BUFFER);
                            memset(msg, 0, MAX_BUFFER);    
                                  
                 }    
        }
     }
  }
     printf("Server Exit...\n"); 
    //close the connection
    close(server_sock);  
          
return 0;
}
