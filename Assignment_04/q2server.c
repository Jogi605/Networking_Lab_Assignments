#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>

#define MAX_BUFFER 200
#define PORT 8080

void answer_query(int client_sock, char *buffer)
{
        int i = 0,j, num1 = 0, num2 = 0, temp = 0, sign  = 0,ans = 0;
        float ans1 = 0;
        char op;
        //Process the number1
        if(buffer[i] == '-')
        {
            sign  = 1;
            i++;
        }
        
        while(buffer[i]!=' ')
        {
            temp =  buffer[i]-'0';
            //printf("%d\n", temp);
            num1 = num1*10 + temp;
            i++;
       }
       if(sign == 1)
       {
             num1*=-1;
             sign = 0;
       }
       i++;
       
       //store the operator
       op = buffer[i];
       //printf("%c\n",op);
       i+=2;
       
       //Process the number2
       if(buffer[i] == '-')
        {
            sign  = 1;
            i++;
        }
        
        while(buffer[i]!='\n')
        {
            temp =  buffer[i]-'0';
            //printf("%d\n", temp);
            num2 = num2*10 + temp;
            i++;
       }
       if(sign == 1)
       {
             num2*=-1;
             sign = 0;
       }
       
       //Clear the buffer
       memset(buffer, 0, MAX_BUFFER);
       
       //Do the Arithmetic Operation
       //Addition
       if(op == '+')
       {
         ans = num1 + num2;
         printf("[Server]: %d\n", ans);
         char details[10];
	 sprintf(details, "%d", ans);
	 strncat(buffer,details, sizeof(details));
      }
      
      //Subtraction
      else if(op == '-')
      {
          ans = num1 - num2;
          printf("[Server]: %d\n", ans);
          char details[10];
	  sprintf(details, "%d", ans);
	  strncat(buffer,details, sizeof(details));
      }
      
      //Multiplication
      else if(op == '*')
      {
          ans = num1 * num2;
          printf("[Server]: %d\n", ans);
          char details[10];
	  sprintf(details, "%d", ans);
	  strncat(buffer,details, sizeof(details));
      }
      
      //Division
      else if(op == '/')
      {
          
          if(num2 == 0)
          {
                char prompt[10] = "Undefined";
                printf("[Server]: %s\n", prompt);
	         strncpy(buffer,prompt,10);
          }
          
          
          else
          {
             ans1 = (float) num1 / (float) num2;
             printf("[Server]: %f\n", ans1);
             char details[10];
	     sprintf(details, "%f", ans1);
	     strncat(buffer,details, sizeof(details));
          }
      }
                  
               
       write(client_sock, buffer, sizeof(buffer));     
         
}


int main()
{
       int server_sock, client_sock, n,m=0;
       struct sockaddr_in6 server_addr, client_addr;
       char buffer[MAX_BUFFER];
       socklen_t addr_size;
       pid_t child_pid;
       
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
     if(listen(server_sock, 10) !=0)
     {
         perror("Listening failed\n");
         exit(1);
     }
     
     else
         printf("The server is listening\n");
      
     // Accept the connection
     addr_size = sizeof(client_addr);
    
     
    
     
     
    //Now listen to the queries      
    while(1)
    {
                  client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_size);
                   
                  //Validate
                  if(client_sock<0)
                  {
                           //perror("Could not accept the connection\n");
                           exit(1);
                  }
                  printf("Connected with Client:%d\n", client_sock-3);
                  
                  //Create a child process to handle multiple clients
                  if((child_pid = fork()) == 0)
                  {
                           close(server_sock);
                           
                           while(1)
                           {
                               memset(buffer, 0, MAX_BUFFER);
     
                               //Read the Message from the Client 
                               read(client_sock, buffer, sizeof(buffer));
                               printf("[Client %d]: %s", client_sock-3, buffer);
                               
                               
                               //Exit if client sends 'exit' msg
                               if(strncmp("exit",buffer,4) == 0)
                               {
                                      memset(buffer, 0, MAX_BUFFER);
	                              m=0;
	                              char prompt[4] = "exit";
	                              strncpy(buffer,prompt,4);
	                              write(client_sock, buffer, sizeof(buffer));
	                              printf("Disconnected From the Client: %d\n", client_sock-3);
			               //close(client_sock);
			               break;
	                        }
	                        
	                        else
	                        {
	                              answer_query(client_sock, buffer); 
	                             // write(client_sock, buffer);
	                              memset(buffer, 0, MAX_BUFFER);
                                }
                           
                            }
	
	           }
     }
	
     printf("Server Exit...\n"); 
    //close the connection
    close(server_sock);  
          
return 0;
}
