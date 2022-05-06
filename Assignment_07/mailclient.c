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

#define MY_PORT 4000
#define POP3_PORT 5000
#define MAX_BUFFER 1024
#define MAX_MSG 256
#define MAX_HEADER 8
#define LINE_SIZE 81
#define MAX_LINE 51

//Function to send packet
int send_packet(int client_sock, char *buffer, int length){
    
    char header[MAX_HEADER], info[20];
    
    //Store both header and info
    sprintf(info, "%%0%dd", MAX_HEADER - 1);
    sprintf(header, info, length);
    
    header[MAX_HEADER-1] = 0;
    
    //Now send the header and data
    send(client_sock, header, MAX_HEADER, 0); 
    
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
 
//Driver function
int main(int argc, char *argv[]){
   int smtp_port = MY_PORT,pop3_port = POP3_PORT;

   //Validate port number
   if(argc == 2){ 
         if(sscanf(argv[1], "%d", &smtp_port)  != 1)
         {
             printf("U have entered Invalid Port Number\n");
             printf("Try with Port Number:4000\n");
             exit(1);
         }  
   }
   
   //Ask to specify port number
   else if(argc == 1){
        printf("PORT number not specified and Use Port Number:4000\n");
        exit(1);
   }
   char *addr = "127.0.0.1";
   char uname[50], pwd[50];
   char *res1;
   int smtp_sock = 0, pop3_sock = 0,n1,n2,l1,l2,option;
   struct sockaddr_in smtp_addr, pop3_addr;
   
   //Create smtp socket
   smtp_sock = socket(AF_INET, SOCK_STREAM, 0);
   
   //Validate
   if(smtp_sock<0)
   {
      printf("Unable to create the smtp socket\n");
      exit(1);
   }
   
   //Create pop3 socket
   pop3_sock = socket(AF_INET, SOCK_STREAM, 0);
   
   //Validate
   if(pop3_sock<0)
   {
      printf("Unable to create the smtp socket\n");
      exit(1);
   }
   
   //Assign smtp IP address
   smtp_addr.sin_family = AF_INET;
   smtp_addr.sin_port = htons(smtp_port);
   
   //Assign smtp IP address
   pop3_addr.sin_family = AF_INET;
   pop3_addr.sin_port = htons(pop3_port);
   
   //Convert smtp IP
   if((inet_pton(AF_INET, addr, &smtp_addr.sin_addr))<=0){
        printf("Address not supported\n");
        exit(1);
  }
  
  //Convert pop3 IP
   if((inet_pton(AF_INET, addr, &pop3_addr.sin_addr))<=0){
        printf("Address not supported\n");
        exit(1);
  }
  //connect to smtp
  n1 = connect(smtp_sock, (struct sockaddr*)&smtp_addr, sizeof(smtp_addr));
  
  //validate
  if(n1<0){
         printf("SMTP Connection Failed.\n");
         exit(1);
  }
  printf("Successfully Connected...\n");
  
  //Enter Credentials
  printf("Enter your username:");
  scanf("%s%*c", uname);
  printf("Enter your password:");
  scanf("%s%*c", pwd);
  
  send_packet(smtp_sock, uname, strlen(uname));
  send_packet(smtp_sock, pwd, strlen(pwd));
  l1 = recieve_packet(smtp_sock, &res1);
  
  //Validate
  if(strcmp(res1, "Authenticated") != 0){
      printf("Unable to authenticate: %s\n", res1);
      close(smtp_sock);
      exit(1);
  }
  
  //connect to pop3
  n2 = connect(pop3_sock, (struct sockaddr*)&pop3_addr, sizeof(pop3_addr));
  
  //validate
  if(n2<0){
    printf("POP3 Connection Failed.\n");
      exit(1);
  }
  printf("Successfully Connected to POP3 server...\n");
                
  char *res2;
  send_packet(pop3_sock, uname, strlen(uname));
  send_packet(pop3_sock, pwd, strlen(pwd));
  l2 = recieve_packet(pop3_sock, &res2);
  
  //Validate
  if(strcmp(res2, "Authenticated") != 0){
        printf("Unable to authenticate: %s\n", res2);
        close(pop3_sock);
        exit(1);
  }
  
  printf("Authenticated Successfully to both Servers\n");
  printf(".......................Welcome to SMTP server......................\n");
  
  //Now send queries
  while(1){
          printf(".......1.Manage Mail.......\n");
          printf(".......2.Send Mail.........\n");
          printf(".......3.Quit..............\n");
          printf("Enter your Choice from 1-3: ");
          scanf("%d%*c", &option);
          
          if(option == 3){
             break;
          }
          
          else if(option == 1){
                
                printf(".......................Welcome to POP3 server.....................\n");
                printf(".......1.q-quit.......\n");
                printf(".......2.d-delete........\n");
                printf(".......3.Any character-show all mails.......\n");
                printf("Enter the corresponding character:\n");
                char ch;
                scanf("%c%*c", &ch);
                while(ch){
                       char *prompt, *data;
                       if(ch == 'q'){
                           prompt = "q";
                           send_packet(pop3_sock, prompt, strlen(prompt));
                           printf("Good Bye!\n");
                           break;
                       }
                       
                       else if(ch == 'd'){
                          prompt = "d";
                          send_packet(pop3_sock, prompt, strlen(prompt));
                          int len = recieve_packet(pop3_sock, &data);
                          printf("%s\n", data);
                          //break;
                       }
                       else{
                          prompt = "m";
                          send_packet(pop3_sock, prompt, strlen(prompt));
                          int len = recieve_packet(pop3_sock, &data);
                          printf("MAILS IN YOUR MAILBOX: \n%s\n", data);
                          //break; 
                       }
                    scanf("%c%*c" , &ch); 
                    free(data);
               }
               
          }
          
          else if(option == 2){
            int max = LINE_SIZE*MAX_LINE +1, length = 0;
            char from[50], to[50], subject[50], body[max], buffer[LINE_SIZE] = {0}, mail[MAX_BUFFER];
            printf("Write Your Email..\n");
            printf("To end the body, the last line should have period(\".\")\n");
            printf("From: ");
            scanf("%s%*c", from);
            printf("To: ");
            scanf("%s%*c", to);
            printf("Subject: ");
            scanf("%[^\n]%*c", subject);
            
            //Verify mail and format
            if(!(verify_format(from))){
                printf("Incorrect Format\n The format is: X@Y\n");
                //printf("ch1\n");
                continue;
            }
            if(!(verify_format(to))){
                printf("Incorrect Format\n The format is: X@Y\n");
                //printf("ch2\n");
                continue;
            }
            printf("Message Body\n");
            //Get body from console
            while((strcmp(buffer, ".")!=0) && length<max){
                scanf("%[^\n]%*c", buffer);
                length+=sprintf(body+length, "%s\n", buffer);
            }
            length =0;
            //Now count the whole mail and send to server
            length+=sprintf(mail+length, "From: %s\n", from);
            length+=sprintf(mail+length, "To: %s\n", to);
            length+=sprintf(mail+length, "Subject: %s\n", subject);
            length+=sprintf(mail+length, "%s\n", body);
            //printf("%d\n", length);
            //Send mail
            char *prompt;
            //printf("%s\n", mail);
            send_packet(smtp_sock, mail, length);
            int k = recieve_packet(smtp_sock, &prompt);
            //Recieve response
            if(k==-1){
               printf("No data:Write again\n");
               continue;
            }
            
            if(strcmp("EMAIL_SENT", prompt) == 0){
             printf("Email sent Successfully!\n");
             continue;
            }
            
            else
              printf("Unable to send email: %s\n", prompt);
              
           free(prompt);  
          }  
            
          
  }
  free(res1);
  printf("Good Bye!\n");
  res1 = "QUIT";
  send_packet(smtp_sock, res1, strlen(res1));
  //send_packet(pop3_sock, res1, strlen(res1));
  close(smtp_sock);
  close(pop3_sock);   
return 0;
}
