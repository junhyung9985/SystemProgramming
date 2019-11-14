// Partly taken from https://www.geeksforgeeks.org/socket-programming-cc/

#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 

#include <string.h> 
#include <pthread.h>
#include <dirent.h>

#include <sys/socket.h> 
#include <sys/types.h>
#include <netinet/in.h> 

void *
worker (void * arg)
{
	int conn ;
	char buf[1024] ;

	char * data = 0x0, * orig = 0x0 ;
	int len = 0 ;
	int s ;

	conn = *((int *)arg) ; 
	free(arg) ;

	while ( (s = recv(conn, buf, 1023, 0)) > 0 ) {
		buf[s] = 0x0 ;
		if (data == 0x0) {
			data = strdup(buf) ;
			len = s ;
		}
		else {
			data = realloc(data, len + s + 1) ;
			strncpy(data + len, buf, s) ;
			data[len + s] = 0x0 ;
			len += s ;
		}
	}
	printf(">%s\n", data) ;

	if (strcmp(data, "#list") == 0) {
		DIR * d ;
		d = opendir(".") ;
		if (d == 0x0) {
			perror("fail") ;
			exit(EXIT_FAILURE) ;
		}

		struct dirent * e ;
		for (e = readdir(d) ; 
			 e != 0x0 ; 
			 e = readdir(d)) {
			if (e->d_type == DT_REG) {
				//printf("%s\n", e->d_name) ;
				char * orig ;
				data = malloc(sizeof(char) * (strlen(e->d_name) + 2)) ;
				strcpy(data, e->d_name) ;
				strcat(data, "\n") ;
				len = strlen(data) ;
				orig = data ;
				printf("> %s", data) ;
				while (len > 0 && (s = send(conn, data, len, 0)) > 0) {
					data += s ;
					len -= s ;
				}
				free(orig) ;
			}
		}
	}
	else /* get file */ {
        
        FILE* pFile;
        long lSize;
        char* buffer;
        size_t result;

        pFile = fopen(data, "rb");
        if (pFile == NULL) {
          fputs("File error", stderr);
          exit(1);
        }

        // 파일의 크기를 ISize 에 저장한다.
        fseek(pFile, 0, SEEK_END);
        lSize = ftell(pFile);
        rewind(pFile);

        // 전체 파일의 내용을 받을 수 있을 정도의 크기로 메모리를 할당한다.
        buffer = (char*)malloc(sizeof(char) * lSize);
        if (buffer == NULL) {
          fputs("Memory error", stderr);
          exit(2);
        }

        // 그 파일의 내용을 버퍼에 저장한다.
        result = fread(buffer, 1, lSize, pFile);
        if (result != lSize) {
          fputs("Reading error", stderr);
          exit(3);
        }
        
        send(conn, buffer, lSize, 0);
        
        fclose(pFile);
        free(buffer);
		//TODO
	}

	shutdown(conn, SHUT_WR) ;
	return 0x0 ;
}

int 
main (int argc, char const *argv[]) 
{ 
	int listen_fd, new_socket ; 
	struct sockaddr_in address; 
	int opt = 1; 
	int addrlen = sizeof(address); 
	
	if (argc != 2) {
		fprintf(stderr, "Wrong number of arguments") ;
		exit(EXIT_FAILURE) ;
	}

	char buffer[1024] = {0}; 

	listen_fd = socket(AF_INET /*IPv4*/, SOCK_STREAM /*TCP*/, 0 /*IP*/) ;
	if (listen_fd == 0)  { 
		perror("socket failed : "); 
		exit(EXIT_FAILURE); 
	}
	
	memset(&address, '0', sizeof(address)); 
	address.sin_family = AF_INET; 
	address.sin_addr.s_addr = INADDR_ANY /* the localhost*/ ; 
	address.sin_port = htons(atoi(argv[1])); 

	if (bind(listen_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
		perror("bind failed : "); 
		exit(EXIT_FAILURE); 
	} 

	while (1) {
		if (listen(listen_fd, 16 /* the size of waiting queue*/) < 0) { 
			perror("listen failed : "); 
			exit(EXIT_FAILURE); 
		} 

		new_socket = accept(listen_fd, (struct sockaddr *) &address, (socklen_t*)&addrlen) ;
		if (new_socket < 0) {
			perror("accept"); 
			exit(EXIT_FAILURE); 
		} 

		pthread_t worker_tid ;
		int * worker_arg = (int *) malloc(sizeof(int)) ;
		*worker_arg = new_socket ;

		pthread_create(&worker_tid, 0x0, worker, (void *) worker_arg) ;
	}
} 

