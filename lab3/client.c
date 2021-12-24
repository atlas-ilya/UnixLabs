#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>

int main()
{
	int sockfd;
	int len;
	struct sockaddr_in address;
	int result;
	char ch[20];

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr("127.0.0.1");
	address.sin_port = htons (5000); /* любой порт >1024 согласованный с сервером */
	len = sizeof(address);
	result = connect(sockfd, (struct sockaddr *)&address, len);

	
	if(result == -1) {
		perror("oops1: client1");
		exit(1);
	}
	
	int num;
	while(1){
		memset(ch,0, 20);
		scanf("%s", ch);
		send(sockfd, ch, strlen(ch), 0);
	}
	close(sockfd);
	puts("Connection closed");
	exit(0);
}
