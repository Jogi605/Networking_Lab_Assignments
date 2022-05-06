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

int main()
{
       int server_sock,n,m=0;
       struct sockaddr_in6 server_addr, client_addr;
       char buffer[MAX_BUFFER];
       char buffer1[][10] ={ "Apple" , "Mango" , "Banana" , "Chikoo" , "Papaya" };
       int buffer2[] = { 100 , 100 , 100 , 100 , 100 };
       socklen_t addr_size;
       
       //Create a socket and validate it: socket(IPV6, TCP, IP);
       server_sock = socket(AF_INET6, SOCK_DGRAM, 0);
      
      //Validate 
       if(server_sock<0)
       { 
            perror("Socket Creation failed\n");
            exit(1);
       }
       
       else
           printf("Server Socket Created Successfully!\n");
           
      memset(&server_addr, 0, sizeof(server_addr));
      memset(&client_addr, 0, sizeof(client_addr));
      
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
          
      printf("The server is Listening!\n");   
     
     addr_size = sizeof(client_addr);
     
    // listen to the queries      
    while(1)
    {
         memset(buffer, 0, MAX_BUFFER);
         
         //Read the Message from the Client 
        // read(client_sock, buffer, sizeof(buffer));
         recvfrom(server_sock, (char *)buffer, MAX_BUFFER, MSG_WAITALL, ( struct sockaddr *) &client_addr, &addr_size);
         printf("From client:%s", buffer);
         
         //Exit if client sends 'exit' msg
         if(strncmp("exit",buffer,4) == 0)
         {
                 memset(buffer, 0, MAX_BUFFER);
	                m=0;
	                char prompt[4] = "exit";
	                strncpy(buffer,prompt,4);
	               // write(client_sock, buffer, sizeof(buffer));
	                sendto(server_sock, buffer, strlen(buffer), MSG_CONFIRM, (const struct sockaddr *) &client_addr, sizeof(client_addr)); 
			 printf("Server Exit...\n"); 
			 break;
	          
         }
         
         //If message contains fruit then send prompt
         else if(strncmp("Fruit",buffer,5) == 0)
         {
             //clear buffer
              memset(buffer, 0, MAX_BUFFER);
	      //m=0;
	      //copy to buffer and send
	      char prompt[17] = "Enter Fruit Name";
	      strncpy(buffer,prompt,17);
	     // write(client_sock, buffer, sizeof(buffer));
	       sendto(server_sock, buffer, strlen(buffer), MSG_CONFIRM, (const struct sockaddr *) &client_addr, sizeof(client_addr));
         } 
         
         //Send all the details of the fruits   
         else if (strncmp("SendInventory", buffer, 12) == 0)  
	   { 
	        //clear buffer
	        memset(buffer, 0, MAX_BUFFER);
	        strcat(buffer, "\n");
	        
	        //copy to buffer and send
	        for(int i=0; i<5; i++)
	        {
	            
	             strncat(buffer, buffer1[i], sizeof(buffer1[i]));
	             strcat(buffer, " ");
	             
	             char details[10];
	             sprintf(details, "%d", buffer2[i]);
	             strncat(buffer,details, sizeof(details));
	             
	             strcat(buffer, "\n");
	        }
	        //write(client_sock, buffer, sizeof(buffer));
	         sendto(server_sock, buffer, strlen(buffer), MSG_CONFIRM, (const struct sockaddr *) &client_addr, sizeof(client_addr));
	   }
	   
	   // read the fruit name and give count
	  else
	  {
	     int i=0,sum=0;
	     char s[10];
	     memset(s, 0, 10);
	     
	     //Get the fruit name
	     while(buffer[i]!= ' '){
	         s[i] = buffer[i];
	         i++;
	     }
	      
	    i++; 
	    //Get the count   
	    while(buffer[i]!= '\n')
	    {
	         int k = buffer[i] - '0';
	         sum = sum*10 + k;
	         i++;
	    }
	    
	    //Compare the fruit
	    for(i=0; i<5; i++)
	         if(strncmp(buffer1[i], s, sizeof(buffer1[i]))==0)
	            break; 
	             
	    //Available          
	    if(buffer2[i]>=sum)
	          {
	           buffer2[i] = buffer2[i]-sum;
	           memset(buffer, 0, MAX_BUFFER);
	           char prompt[9] = "Remaining";
	           strncpy(buffer,prompt,9);
	           strcat(buffer, " ");
	           char details[10];
	           sprintf(details, "%d", buffer2[i]);
	           strncat(buffer,details, sizeof(details));
	           //write(client_sock, buffer, sizeof(buffer));
	            sendto(server_sock, buffer, strlen(buffer), MSG_CONFIRM, (const struct sockaddr *) &client_addr, sizeof(client_addr));
	          }
	          
	    // Not available     
	    else
	         {
	          memset(buffer, 0, MAX_BUFFER);
	          char prompt[13] = "Not Available";
	          strncpy(buffer,prompt,13);
	          //write(client_sock, buffer, sizeof(buffer));
	           sendto(server_sock, buffer, strlen(buffer), MSG_CONFIRM, (const struct sockaddr *) &client_addr, sizeof(client_addr));
	         } 
	 
	}
    }
    
    //close the connection
    close(server_sock);  
          
return 0;
}
