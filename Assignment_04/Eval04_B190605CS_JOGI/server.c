#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<sys/select.h>

#define PORT 4000

int main()
{
    // declare sockets
    int client_sock, server_sock, m, n,flag,k,j,i,f;
    //Declare buffers 
    char i_buffer[300],o_buffer[300],name[50][25],result[25],pwd[]="234513",msg[500],buffer[300];
    bzero(names,sizeof(names));
    
    //Declare the address
    struct sockaddr_in server_addr, client_addr;
    
    //declare address size
    socklen_t client_len;


    //Creating the Socket with IPv4 and TCP
    server_sock = socket(AF_INET,SOCK_STREAM,0);
    
    //Validate
    if (server_sock<0)
    {
        perror("Socket creaton failed!\n");
        exit(1);     
    }
    //Clear the server address
    bzero(&server_addr,sizeof(server_addr));
    
    //Assign IPv4 address
    server_addr.sin_family=AF_INET;
    server_addr.sin_addr.s_addr=INADDR_ANY;
    
    //Set the port number of the destination
    server_addr.sin_port=htons(PORT);
     
    //Bind the socket to the destination IP and port number 
    m = bind(server_sock,(struct sockaddr *)&server_addr,sizeof(server_addr));
    
    //validate
    if(m<0)
    {
        perror("unable to bind\n");
        exit(1);
    }
    
    //Declare and set flags
    fd_set curr_sock,ready_sock; 
    FD_ZERO(&current_sockets);
    FD_SET(sockfd,&current_sockets);
    
    //Now listen to client
    m = listen(sockfd,50);
    
    //Validate
    if(m<0)
    {
       perror("fialed to listen\n");
       exit(1);
    }
    
    //Success
    printf("I am listening on port %d\n",portno);
    
    //connect to clients
    while(1)
    {   
        ready_sock=curr_sock;
        
        
        //Select to reads msgs
        if(select(FD_SETSIZE+1,&ready_sock,NULL,NULL,NULL)<0)
        {
            perror("Unable to select()\n");
            exit(1);
        }
        
        //Now listen for queries
        for ( i = 0; i < 1000; i++)
        {   
            //check if any msg is in queue
            if(FD_ISSET(i,&ready_sock))
            {   
            
                //Handle new connection
                if(i==sockfd)
                {   
                    client_len = sizeof(client_addr);
                    
                    //Accept connection
                    client_sock= accept(server_sock,(struct sockaddr *)&client_addr,&client_len);

                    //Validate
                    if(client)_sock<0)
                    {
                        perror("Error Accepting");
                        exit(1);
                    }
                    
                    //clear buffers
                    bzero(result,sizeof(result));
                   
                    read(client_sock,result,sizeof(result));
                    
                    if(strcmp(result,pwd)!=0)
                    {
                    strcpy(buffer,"Incorrect Password\n");
                    write(client_sock,misc,sizeof(misc));
                    //strcpy(misc,"Rejected");
                    n=write(newsockfd,misc,sizeof(misc));
                    continue;}
                   // printf("accepted\n");
                    FD_SET(newsockfd,&current_sockets);
                    strcpy(misc,"Accepted");
                    n=write(newsockfd,misc,sizeof(misc));
                    read(newsockfd,names[newsockfd],sizeof(names[newsockfd]));
                    bzero(misc,sizeof(misc));
                    strcpy(misc,"The Co-IPs connected are:\n");
                    f=0;
                    for (int k = 0; k < FD_SETSIZE; k++)
                    {
                        if(FD_ISSET(k,&current_sockets)&&k!=newsockfd&&k!=sockfd)
                        {
                            f=1;
                            strcat(misc,names[k]);
                            strcat(misc,"\n");
                        }
                    }
                    if(f==0)
                    strcpy(misc,"Pl. Wait\n");
                    printf(" %s\n",misc);
                    n=write(newsockfd,misc,sizeof(misc));
                    fprintf(stdout,"%s Connected\n",names[newsockfd]);

                    for (int j = 0; j < FD_SETSIZE; j++)
                    {
                        if(FD_ISSET(j,&current_sockets))
                        {
                            if(j!=sockfd&&j!=newsockfd)
                            {
                                strcpy(misc,names[newsockfd]);
                                strcat(misc," Connected\n");
                                
                                 n=write(j,misc,sizeof(misc));//Write to the socket
                                if(n<0)
                                {
                                    perror("Error Writing");
                                    exit(1);
                                }
                            }
                        }
                    }
                }
                else{
                    bzero(ibuffer,255);
                    n = read(i,ibuffer,sizeof(ibuffer));//Read the message from the socket
                    if(n<0)//Error reading
                    {
                        perror("Error Reading");
                        exit(1);
                    }

                   
                    for (int j = 0; j < FD_SETSIZE; j++)
                    {
                        if(FD_ISSET(j,&current_sockets))
                        {
                            if(j!=sockfd&&j!=i&&(strncmp("bye",ibuffer,3)!=0))
                            {
                                
                                strcpy(message,names[i]);
                                strcat(message," : ");
                                strcat(message,ibuffer);
                                 n=write(j,message,sizeof(message));//Write to the socket
                                if(n<0)
                                {
                                    perror("Error Writing");
                                    exit(1);
                                }
   
                            }
                        }
                    }
                    
                    if(strncmp("bye",ibuffer,3)==0)
                    {
                        
                        //bye
                        FD_CLR(i,&curr_sock);
                        bzero(buffer,sizeof(buffer));
                        strcpy(buffer,names[i]);
                        strcat(buffer," : ");
                        strcat(buffer,i_buffer);
                        strcat(buffer,"\n\n");
                        strcat(buffer,name[i]);
                        strcat(buffer," left the Discussion.\n");
                        printf("%s has left Discussion.\n",name[i]);
                        
                        //Now send msgs
                        for (int j = 0; j < FD_SETSIZE; j++)
                    {
                        if(FD_ISSET(j,&curr_sock))
                        {
                            if(j!=server_sock)
                            {   
                                
                                //write
                                 n=write(j,buffer,sizeof(buffer));
                                if(n<0)
                                {
                                    perror("Error Writing");
                                    exit(1);
                                }
                            }
                        }
                    }
                    }
                    bzero(i_buffer,sizeof(i_buffer));
                }
            }
        }
        

    }
    //close the client socket
    close(client_sock);
    
    //close the server socket
    close(server_sock);
    return 0;
}
