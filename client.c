#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<fcntl.h>
#define SIZE 1024

void write_file(int sockfd,char *filename)
{
  int n,fd;
  char buffer[SIZE]={0};
  fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC,0644);
  char size_string[SIZE];
  int read_return=read(sockfd,size_string,SIZE);
  //printf("return value of read = %d\n",read_return);
  //printf("size string = %s\n",size_string);
  int total=atoi(size_string);
  int temp=total;
  //printf("total = %d\n",total);
  //printf("file opened\n");
  int sum=0;
  while (temp>0) 
  {
    n = read(sockfd, buffer, SIZE);
    sum=sum+n;
    float status= (((float) sum )/total)*100;
    printf("\rDownloading %.5f%%\n",status);
    if(n==-1)
    {
      perror("Receiving error");
      exit(1);
    }
    int return_value=write(fd,buffer,n);
    if(return_value<0)
    {
      perror("write:");
    }
    memset(buffer,0,SIZE);
    temp = temp - n;
  }
  return;
}

int main()
{
  char *ip = "127.0.0.1";
  int port = 8080;
  int e;

  int sockfd;
  struct sockaddr_in server_addr;
  FILE *fp;
  char *filename = "send.txt";

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd < 0) {
    perror("[-]Error in socket");
    exit(1);
  }
  printf("[+]Server socket created successfully.\n");

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = port;
  server_addr.sin_addr.s_addr = inet_addr(ip);

  e = connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
  if(e == -1) {
    perror("[-]Error in socket");
    exit(1);
  }
	printf("[+]Connected to Server.\n");
  while(1)
  {
    printf("client> ");
    char c;
    char input[1024]={0};
    int len_of_input=0;
    while( ( c = getchar()) != '\n')
    {
      strncat( input, &c, 1);
      len_of_input++;
    }
    if(len_of_input>0)
    {
        //printf("Input before sending = %s\n",input);
        send(sockfd,input,strlen(input),0);
        //printf("command sent\n");
        char *token;
        char *command[5000];
        int token_count=0;
        token=strtok(input," ");
        while(token!=NULL)
        {
          command[token_count]=token;
          token_count++;
          //printf("%s ",command[token_count-1]);
          token=strtok(NULL," ");
        }
        //printf("\n");
        if(strcmp(command[0],"exit")==0)
          break;
        else if(strcmp(command[0],"get")==0)
        {
          for(int i=1;i<token_count;i++)
          {
            //printf("i=%d\n",i);
            write_file(sockfd,command[i]);
            printf("[+]Data written in %s successfully.\n",command[i]);
          }
        }
        else
        {
          printf("Invalid command\n");
        }
        for(int i=0;i<strlen(input);i++)
        {
          input[i] = '\0';
        }
        for(int i=0;i<token_count;i++)
        {
          for(int j=0;j<strlen(command[i]);j++)
            command[i][j]='\0';
        }
    }
  }
  printf("[+]Closing the connection.\n");

  return 0;
}
