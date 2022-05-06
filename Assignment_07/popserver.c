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
#include <netinet/in.h>
#include <pthread.h>

#define MY_PORT 5000
#define MAX_BUFFER 1024
#define MAX_MSG 256
#define MAX_HEADER 8 

//Structure for credential_info  
typedef struct detials{
      char uname[50];
      char pwd[50];
} Details;

//Structure for client_info
typedef struct client{
     char *uname;
     char *pwd;
     int sock_fd;
     int users;
     Details *D;
     struct client *head;
     struct client *next;
     } Client;

//Function to send packet
int send_packet(int client_sock, char *buffer, int length){
    
    char header[MAX_HEADER], info[20];
    
    //Store both header and info
    sprintf(info, "%%0%dd", MAX_HEADER - 1);
    sprintf(header, info, length);
    
    header[MAX_HEADER-1] = 0;
    
    //Now send the header and data
    send(client_sock, header, MAX_HEADER, 0); 
    //printf("%s %s\n", header, buffer);
    return send(client_sock, buffer, length, 0);
    
}

//Function to recieve the packet
int recieve_packet(int client_sock, char **data){
    char header[MAX_HEADER];
    int length = -1, len;
    
    //recieve header 
    recv(client_sock, header, MAX_HEADER, 0);
    
    //Get length from header
    sscanf(header, "%d", &length);
    
    //Verify
    if(length<=0)
     {
      printf("Invalid Header Format\n");
      return -1;
     } 
     
     //Declare buffer to copy from data
     char *buffer = calloc(length, sizeof(char));
     *data  = buffer;
     
     //recieve data
     len = recv(client_sock, buffer, length, 0);
     buffer[len]  = '\0';
     
     return len+1;
}
 
//Function to create a new client
Client *new_client(Client *head, Details *D, int client_sock, int users){
   //Initialsie all values
   Client *C = (Client *)malloc(sizeof(Client));
   C->uname = NULL;
   C->pwd = NULL;
   C->sock_fd = client_sock;
   C->users = users;
   C->D = D;
   C->head  = head;
   C->next = NULL;
   
   return C;
}

//Function to add client
void add_client(Client *C, Client *head){
   Client *p = head;
   
   while(p->next!=NULL)
      p = p->next;
     
  p->next = C;
}

//Function to create client list
Client *new_list(){
    Client *head  = new_client(NULL, NULL, 0, 0);
    return head;
}

//Function to remove client
int remove_client(Client *C, Client *head){
    Client *p = head;
    
    while(p){
          
          if(p->next == C)
          {
             p->next = C->next;
             free(C);
             return 1;
         }
         p = p->next;
   }
   return 0;
   
}

//Function to verify reciepent is there or not 
int verify_reciepent(char *mail, Details *D, int users){
     char uname[50];
     int i, n = strlen(mail), flag  = 0;
     for(i=0; i<n; i++){
         if(mail[i] == '@'){
            flag =1;
            break;
         }
    }
    
    if(flag  == 0)
       return -1;
      
   strncpy(uname, mail, i);
   uname[i] = 0;
   
   for(int j=0; j<users; j++)
     if(strcmp(D[j].uname, uname) == 0)
        return j;
    
   return -1;
}

//Function to get current_time
void get_time_str(char *time_str){
   time_t current_time;
   struct tm *tm_info;
   current_time = time(NULL);
   tm_info = gmtime(&current_time);
   strftime(time_str, 100, "%a, %d %b %Y %I:%M:%S GMT", tm_info);
   
}

//Function to verify mail format
int verify_format(char *mail){
   int flag = 0, i, n = strlen(mail), a= 0, b=0;
   for(i=0; i<n; i++){
     if(mail[i] == '@'){
        flag = 1;
        continue;
    }
    if(flag == 0)
       a++;
    
    else
        b++;
   
   if(b>0)
      break;
   }
   
   return a>0 && b>0;
}

//Function to get start of mail
int begin_with(char *full, char *sub){
  int m = strlen(full), n = strlen(sub), i, j;
  
  if(n>m)
     return 0;
  for(i=0; i<n; i++)
      if(full[i]!=sub[i])
         return 0;
  return n;
}

//Function to get line
void get_line(char *data, int *field, char *out){
    int i=0, n = strlen(data);
    
    while(data[*field]!='\n' && *field < n){
           out[i] = data[*field];
           *field+=1;
           i++;
    }
    out[i] = '\0';
    
    if(data[*field] == '\n')
       *field+=1;
}

//Function to check whether exists or not
int has_char(char *ch, char c){
   int i=0, n = strlen(ch);
   
   for(i=0; i<n; i++)
       if(ch[i] == c)
           return i;
   return -1;
}

//Function to get fields
void get_data(char *data, char *field, char *out){
      int i=0, n = strlen(data), j=0;
      char buffer[100];
      
      while(i<n){
         get_line(data, &i, buffer); 
         if(has_char(buffer, ':') == -1){
            *out = 0;
            if(strcmp(field, "Body") == 0)
               sprintf(out, "%s\n%s", buffer, data+i);
            return;
         }
         if(j= begin_with(buffer, field)){
              strcpy(out, buffer+j+2);
              return;
         }
      }      
}

//Function to service client
void *handle_client(void *arg){
     printf("New Client connected\n");
     
     Client *C = (Client *)arg;
     
     //Declare necessary variables
     char *uname, *pwd;
     int n,i=0,flag = 0;
     
     //Recieve packet uname details
     n = recieve_packet(C->sock_fd, &uname);
     //printf("%s\n", uname);
     //Validate data
     if(n == -1)
     {
        printf("The data could not recieved and closing the connection\n");
        close(C->sock_fd);
        remove_client(C, C->head);
        return NULL;
     }
     
     //Recieve packet uname details
     n = recieve_packet(C->sock_fd, &pwd);
     //printf("%s\n", pwd);
     //Validate data
     if(n == -1)
     {
        printf("The data could not recieved and closing the connection\n");
        close(C->sock_fd);
        remove_client(C, C->head);
        return NULL;
     }
     
     char *msg1 = "Invalid Password";
     char *msg2 = "User not Found";
     char *msg3 = "Authenticated";
     
     //Validate the uname and password
     while(i<C->users){
        if(strcmp(C->D[i].uname, uname) == 0){
            if(strcmp(C->D[i].pwd, pwd)!=0){
                 send_packet(C->sock_fd, msg1, strlen(msg1));
                 //Closing the connection
                 close(C->sock_fd);
                 remove_client(C, C->head);
                 return NULL;
            }
           else{
               C->uname = uname;
               C->pwd = pwd;
               flag = 1;
               break;
          }       
       }
       i++;
    }
    
    //User not found
    if(flag == 0)
    {
                 send_packet(C->sock_fd, msg2, strlen(msg2));
                 //Closing the connection
                 close(C->sock_fd);
                 remove_client(C, C->head);
                 return NULL;
    }
    
    //Authenticated successfully
    send_packet(C->sock_fd, msg3, strlen(msg3)); 
    printf("%s Authenticated Successfully!\n", uname);
    
    //Now listen to queries
    while(1){
            char buffer[MAX_BUFFER], body[MAX_BUFFER], from[50], time[50], subject[50], line[100];
            char *data;
            int l, j=0;
            
            //Recieve the data
            l = recieve_packet(C->sock_fd, &data);
            //printf("%s\n", data);
            //Validate
            if(l == -1){
               printf("the data was not correct and closing the connection\n");
               break;
            }
            
            //To quit the server
            if(strcmp(data, "q") == 0)
                 break;
            
            //To delete mail                    
            else if(strcmp(data, "d") == 0)
            {
                char delete_file[100];
                char *res;
                sprintf(delete_file, "%s/mymailbox.mail", C->uname);
                int del = remove(delete_file);
                if (!del){
                    res = "Mail deleted Successfully\n";
                    send_packet(C->sock_fd, res, strlen(res));  
                }
                 
                else{
                    res = "Unable to delete Mailbox\n";
                    send_packet(C->sock_fd, res, strlen(res));
                }      
            }
              
            else if(strcmp(data, "m") == 0){ 
                          char mailbox_file[100];
                          int c=0, a = 1,len = 0;
                          sprintf(mailbox_file, "%s/mymailbox.mail", C->uname);
                          FILE *mailbox = fopen(mailbox_file, "r"); 
                          
                          printf("Listing all mails to %s from %s\n", C->uname, mailbox_file);
                          
                          while(mailbox && fscanf(mailbox, "%[^\n]%*c", line)!= EOF){
                              if(begin_with(line, "From")){
                                  strcpy(from, line+6);
                                  c++;
                              }
                              if(begin_with(line, "Subject")){
                                  strcpy(subject, line+9);
                                  c++;
                              }
                              if(begin_with(line, "Recieved")){
                                  strcpy(time, line+10);
                                  c++;
                              }
                              if(c == 3){
                                  len+=sprintf(buffer+len, "%d | %s | %s | %s \n", a, from, time, subject);
                                   c = 0;
                                   a++;
                              }
                              if (strcmp(line, ".") == 0){
                                        fseek(mailbox, 1, SEEK_CUR);
                              }
                            }  
                            if(len == 0)
                                len = sprintf(buffer, "NO MAILS IN YOUR BOX\n");
                                
                            int p = send_packet(C->sock_fd, buffer, len); 
                            //printf("%s\n", buffer);
              }               
           free(data);
           }
  //Disconnect
    printf("Disconnected from %s \n", uname);
    close(C->sock_fd);
    remove_client(C, C->head);    
}
//Driver function
int main(int argc, char *argv[]){
   int port = MY_PORT;
   
   //Validate port number
   if(argc == 2){
         
         if(sscanf(argv[1], "%d", &port)  != 1)
         {
             printf("U have entered Invalid Port Number\n");
             printf("Try with Port Number:5000\n");
             exit(1);
         }
         
   }
   
   //Ask to specify port number
   else if(argc == 1){
        printf("PORT number not specified and Use Port Number:5000\n");
        exit(1);
   }
   
   //Declare necessary variables
   int client_sock, server_sock, length,addr_len, option =1, n,users = 0,i;
   char buffer[MAX_BUFFER], msg[MAX_MSG];
   struct sockaddr_in server_addr, client_addr;
   addr_len  = sizeof(server_addr);
   socklen_t addr_size;
   char vu_name[50], vu_pwd[50];
   
   printf("Initiating POP3 Mail server.\n");
   
   //Create server socket
   server_sock = socket(AF_INET, SOCK_STREAM, 0);
   
   //Validate
   if(server_sock<0)
   {
      printf("Unable to create the socket\n");
      exit(1);
   }
   
   //set the socket options
   if(setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &option, sizeof(option))<0)
   {
       printf("Unable to set socket options\n");
       exit(1);
   }
   
   //Initialise the address   
   memset(&server_addr, 0, sizeof(server_addr));
      
   //Assign IP address
   server_addr.sin_family = AF_INET;
   server_addr.sin_addr.s_addr = INADDR_ANY;
   server_addr.sin_port = htons(port);
   
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
          
     // Now listen to the clients and validate
     if(listen(server_sock, 3) !=0)
     {
         perror("Listening failed\n");
         exit(1);
     }
     
     else
         printf("The server started and listening\n");
     
     //Open file to store credentials 
     FILE *fp = fopen("userlogincred.text", "r");
     
     if(fp == NULL)
            { 
                perror("@@Server@@::Error in reading file"); 
                exit(1);
            }
     
     //Count users 
     while (fscanf(fp, "%[^ ]%*c %[^\n]%*c", vu_name, vu_pwd) != EOF)
         users++;
    
    //Alllocate memory for user_detials     
    Details *D = calloc(users, sizeof(Details));
    
    fseek(fp, 0, SEEK_SET);
    i = 0;
    //Now copy user_detials
    while (fscanf(fp, "%[^ ]%*c %[^\n]%*c", vu_name, vu_pwd) != EOF)
    {
        strcpy(D[i].uname, vu_name);
        strcpy(D[i].pwd, vu_pwd);
        //printf("%s, %s\n", D[i].uname, D[i].pwd);
        i++;
    }
   
   //Create a new client list
   Client *head = new_list();
   //Accept the connections
   while (1)
    {
          // Accept the connection
          addr_size = sizeof(client_addr);
          client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_size);
         
         //Validate
         if(client_sock<0)
         {
            perror("Could not accept the connection\n");
            continue;
         }
         printf("Connection is setup\n");

        fflush(stdout);

        Client *C = new_client(head, D, client_sock, users);
        add_client(head, C);
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, handle_client, C);
    }  
   
   
return 0;
}
