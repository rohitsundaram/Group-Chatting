#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
	
#define BUFSIZE 1024
static flag=0;
char acname[BUFSIZE];
void send_user(int sockfd)
{

char recv_buf[BUFSIZE];
int response,ichoice;
			//printf("sending name");
			send(sockfd, acname, strlen(acname), 0);
			recv(sockfd, recv_buf, sizeof(recv_buf), 0);
			response=atoi(recv_buf);
			if(response==0){
				printf("Do you wanna Signup?\n1:Yes\n2:No\n");
				scanf("%d",&ichoice);
				if(ichoice==1){
					send(sockfd, acname, strlen(acname), 0);
					response=1;
				}
				else{
					printf("Bye!");	
					exit(0);
				   }
				}
				if(response==1)
				printf("Welcome %s",acname);
		fflush(stdout);
}
void send_recv(int i, int sockfd)
{	flag++;
	char send_buf[BUFSIZE];
	char recv_buf[BUFSIZE];
	int nbyte_recvd;
	char acname1[BUFSIZE];
	if (i == 0){
		//printf("\nU:");
		fgets(send_buf, BUFSIZE, stdin);
		if (strcmp(send_buf , "quit\n") == 0) {
			exit(0);
		}else if (strcmp(send_buf , "$cu\n") == 0) {
                        //printf("client sending %s\n","$cu");
                        send(sockfd,"$cu\n", strlen("$cu\n"), 0);
                        //fflush(stdout);
                        //printf("Completed sending");
                        //fflush(stdout);
                }else{
			strcpy(acname1,acname);
			strcat(acname1,":");
			strcat(acname1,send_buf);
			strcpy(send_buf,acname1);
			 
			send(sockfd, send_buf, strlen(send_buf), 0);
			strcpy(acname1,"\0");
		}
	}else {
		nbyte_recvd = recv(sockfd, recv_buf, BUFSIZE, 0);
		recv_buf[nbyte_recvd] = '\0';
		printf("%s\n",recv_buf);
		fflush(stdout);
	}
}
		
		
void connect_request(int *sockfd, struct sockaddr_in *server_addr)
{
	if ((*sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("Socket");
		exit(1);
	}
	server_addr->sin_family = AF_INET;
	server_addr->sin_port = htons(4950);
	server_addr->sin_addr.s_addr = inet_addr("127.0.0.1");
	memset(server_addr->sin_zero, '\0', sizeof server_addr->sin_zero);
	
	if(connect(*sockfd, (struct sockaddr *)server_addr, sizeof(struct sockaddr)) == -1) {
		perror("connect");
		exit(1);
	}
}
	
int main()
{
	int sockfd, fdmax, i;
	struct sockaddr_in server_addr;
	fd_set master;
	fd_set read_fds;
	
	connect_request(&sockfd, &server_addr);
	FD_ZERO(&master);
        FD_ZERO(&read_fds);
        FD_SET(0, &master);
        FD_SET(sockfd, &master);
	fdmax = sockfd;
	printf("\nName:");
	scanf("%s",acname);	
	getchar();
		send_user(sockfd);
	while(1){
		read_fds = master;
		if(select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1){
			perror("select");
			exit(4);
		}
		
		for(i=0; i <= fdmax; i++ )
			if(FD_ISSET(i, &read_fds)){
				//printf("%s:",acname);
				send_recv(i, sockfd);
			}
	}
	printf("client-quited\n");
	close(sockfd);
	return 0;
}

