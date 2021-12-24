#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <errno.h>

volatile sig_atomic_t wasSigHup = 0;

void sigHupHandler(int r)
{
    wasSigHup = 1;
}

int main()
{
	int fd;
	//Создание сокета 
	fd=socket(PF_INET, SOCK_STREAM, 0); //семейчство протоколов ipv4 
	if (fd==-1){
		perror("socket");
		return -1;
	}
	int enable = 1;
	// Добавление опции устранения конфликта привязки к адресу
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0){ 
		perror("setsockopt");
		return -1;
		}
	//Создание структуры, определяющей адрес
	struct sockaddr_in addr; 
	memset(&addr, 0, sizeof(struct sockaddr_in)); 
	addr.sin_family = AF_INET;	
	addr.sin_addr.s_addr = htonl(INADDR_ANY); 
	addr.sin_port = htons(3000); 
	//Привязка к сокету структуры адреса
	if (bind(fd, (struct sockaddr*)&addr, sizeof(struct sockaddr_in))== -1)  
	{
		perror("bind");
		close(fd);
		return -1;
	};
	listen (fd, SOMAXCONN); //
    if (fd == -1)
        exit(EXIT_FAILURE);
	 //Формируем обработку сигнала
    struct sigaction sa;
	sigaction (SIGHUP, NULL, &sa); 
    sa.sa_handler = sigHupHandler; 
    sa.sa_flags |= SA_RESTART;	
    sigaction (SIGHUP, &sa, NULL);
    
	//Блокируем сигнал
    sigset_t blockedMask, origMask;
    sigemptyset(&blockedMask);
    sigaddset(&blockedMask, SIGHUP);
    sigprocmask(SIG_BLOCK, &blockedMask, &origMask); 
   	char buf[20];
	int client_fd, ret;
	int numofclients= 0;
	while (1) {
		int maxFd = -1;
		//Создаем набор файловых дескрипторов
		fd_set fds;
		FD_ZERO(&fds); 
		FD_SET(fd, &fds);
		if (fd > maxFd) maxFd = fd;
		if (numofclients>0)
		{
			FD_SET(client_fd,&fds);
			if (client_fd > maxFd) maxFd = client_fd;
		}
		ret = pselect(maxFd + 1, &fds, NULL, NULL, NULL, &origMask); 
		if (ret  == -1 ) {
			if (errno == EINTR)
			{
				if (wasSigHup==1)
				{
					puts("Received SIGHUP");
					wasSigHup=0;	
				}
			}
		}
		if (ret>=0){
			if (FD_ISSET(fd, &fds)){
				//Принимаем клиента				
				int client_fd1 = accept(fd,NULL, NULL);
				if (numofclients>=1){
					close(client_fd1);
					/// continue;
				}
				else if(client_fd1>=0){
					client_fd=client_fd1;
					puts("New client!");
					numofclients++;
					printf("Client %i accepted\n", client_fd);
					
				}

				/// else perror("accept");
			}
			
				if (FD_ISSET(client_fd, &fds)){
					int NumBytesReceaved = recv(client_fd, buf,20,0);
					if (NumBytesReceaved>0)
					{
						printf("Received %i bytes from %i!\n", NumBytesReceaved, client_fd);
					}
					else{
						close(client_fd);
						printf("Connection closed with client #%i\n",client_fd);
						numofclients--;
					}
			}
		}
	}
    return 0;
}
