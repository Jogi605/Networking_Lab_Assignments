#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include <fcntl.h>

#define PORT 3000

void main(int argc, char *argv[])
{
    
    //If the user doesnot provide the username
    if(argc<2)
    {
        printf("Enter the username and come back\n");
        exit(1);
    }
    
    //declaring  buffers for input and output and for other purposes
    char i_buffer[300], o_buffer[300], name[25], new_line[]="\n",pwd[25], result[25];
    
    //declare socket
    int client_sock, n, m;
    
    //Declare timeout bound
    struct timeval tv;
    tv.tv_sec  = 1;
    tv.tv_usec = 100;
    
    //Declare the address
    struct sockaddr_in server_addr, client_addr;
    
    //Size of address
    socklen_t client_len;
    
    //Declare server to connect
    struct hostent  *server;
    
    //Declare File descriptors
    fd_set file_d1;
    FD_ZERO(&file_d1);
    
    //Create a socket with IPv4 and TCP connection
    client_sock = socket(AF_INET,SOCK_STREAM,0);
    
    //Validate
    if (client_sock<0)
    {
        perror("Socket creation failed\n");
        exit(1);     
    }
    
    //Get ip address
    server = gethostbyname("127.0.0.1");
    
    //Error in IP address
    if(server == NULL)
    {
        fprintf(stderr,"error from host\n");
        exit(1);
         
    }
    //Clear the server address
    bzero((char *)&server_addr,sizeof(server_addr));
    
    //Assign Ipv4 address and Port
    server_addr.sin_family = AF_INET;
    
    //Copy the server IP address
    bcopy((char *)server->h_addr,(char*)&server_addr.sin_addr.s_addr,server->h_length); 

    // set the port
    server_addr.sin_port=htons(PORT);

    // Now connect
    m = connect(client_sock,(struct sockaddr *)&server_addr,sizeof(server_addr));
    if(m<0)
    {
        perror("Connection Error\n");
        exit(1);
    }
    
   //enter the credentials
    printf("Enter the password: ");
   
    //clear the buffer
    bzero(pwd,sizeof(pwd));
    
    //get the password
    fgets(pwd,sizeof(pwd),stdin);
    pwd[strcspn(pwd,new_line)] = '\0';
    
    //Now send the password
    n = write(client_sock,pwd,strlen(pwd));
  
    //validate
    if(n<0)
    {
        perror("Unable to write to the client\n");
        exit(1);
    }
    
    //Get name form argument
    strcpy(name,argv[1]);
    
    //Get result from server 
    n= read(client_sock,result,sizeof(result));
    
    //Validate
    if(strcmp(result,"Accepted")==0)
    {
    
    //Write your name into the socket
    
    n = write(client_sock,name,strlen(name));
    
    //print welcome
    printf("\nWelcome to Project COBRA\n");
    
    //get and set flags to non block
    int flags=fcntl(0,F_GETFL);
    flags |=O_NONBLOCK;
    fcntl(0,F_SETFL,flags);
    
    //now talk with other
    while(1)
    {
        //clear buffer
        bzero(i_buffer,sizeof(i_buffer));
       
       //Get msg from input
        fgets(i_buffer,sizeof(i_buffer),stdin);
        i_buffer[strcspn(i_buffer,new_line)] = '\0';
        
        //Write the accepted message into the socket
        n = write(client_sock,i_buffer,strlen(i_buffer));
        
        //validate
        if(n<0)
        {
            perror("Unable to write to the client\n");
            exit(1);
        }
        
        //exit
        if(strncmp("bye",i_buffer,3)==0)
        {
            printf("You left the Discussion\n");
            break;
        }
        
        //clear the buffer
        bzero(i_buffer,sizeof(i_buffer));
        
        
        //set the descriptor
        FD_SET(client_sock, &file_d1);
        
        //Read msg from client
        n= select(client_sock+1,&file_d1,NULL,NULL,&tv);
        
        //Ifd there are no msg continue
        if(n==0)
            continue;
            
         //read msg   
        else
        {   
              //clear output buffer
               bzero(o_buffer,sizeof(o_buffer));
               n=read(client_sock,o_buffer,sizeof(o_buffer));
             
        }
        
        if(n<0)
        {
            perror("Unable to read the client\n");
            exit(1);
        }
        
        //print msg
        fprintf(stdout,"%s\n",o_buffer);
        
     }
    }
    
    
    //password incorrect
    else printf("Incorrect Password!!\n");
    
    //Close the socket
    close(client_sock); 
