#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <string.h> 

int 
main (int argc, char const *argv[]) 
{
	struct sockaddr_in serv_addr; 
	int sock_fd ;
	int s, len ;
	char buffer[1024] = {0}; 
	char * data ;
    int delim = 0;
    char* ptr;
    char *sSeparator = ";";
    int iLen = strlen(argv[1]);
    char *sInput = (char *)malloc((iLen+1) * sizeof(char));

    strcpy(sInput, argv[1]);
    ptr = strtok(sInput,":");        //arr1문자열을 띄어쓰기로 끊어서 문자열의 첫주소 반환
    char *ip;
    char *port;
    
    /*
    while (ptr != NULL && delim<2)            //strtok으로 잘린 문자
    {
        if (delim == 0 ){ip=ptr;delim ++;}
        if (delim == 1 ){port=ptr;delim ++;}
        
        //ptr 출력
        ptr = strtok(NULL, ":");        //NULL을 넣으면 strtok에서 이전에 문자를 자른
                                             //후에 다음 문자의 주소를 반환해줌.
    }
    */
    ip = strdup(argv[1]);
    ip = strtok(ip, ":");
    port = strtok(NULL, " ");

    printf("%s", ip);
    printf("%s", port);
	char * cmdline ; // 'list' or 'get'
	char * filename ;

	if (argc == 3 && strcmp(argv[2], "list") == 0) {
		cmdline = strdup("#list") ;
	}
	else if (argc == 4 && strcmp(argv[2], "get") == 0) {
		cmdline = strdup(argv[3]) ;
	}
	else {
		//argv[1]: IP
		//argv[2]: port
		//argv[3]: command = {list, get}
		//argv[4]: filename
		fprintf(stderr, "Wrong number of arguments\n") ;
		exit(EXIT_FAILURE) ;
	}
	
	sock_fd = socket(AF_INET, SOCK_STREAM, 0) ;
	if (sock_fd <= 0) {
		perror("socket failed : ") ;
		exit(EXIT_FAILURE) ;
	} 

	memset(&serv_addr, '0', sizeof(serv_addr)); 
	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_port = htons(atoi(port));
	if (inet_pton(AF_INET, ip, &serv_addr.sin_addr) <= 0) {
		perror("inet_pton failed : ") ; 
		exit(EXIT_FAILURE) ;
	} 

	if (connect(sock_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		perror("connect failed : ") ;
		exit(EXIT_FAILURE) ;
	}
	
	data = cmdline ;
	len = strlen(cmdline) ;
	s = 0 ;
	while (len > 0 && (s = send(sock_fd, data, len, 0)) > 0) {
		data += s ;
		len -= s ;
	}
	shutdown(sock_fd, SHUT_WR) ;

	if (strcmp(cmdline, "#list") == 0) {
		char buf[1024] ;
		//data = 0x0 ;
		//len = 0 ;
		while ( (s = recv(sock_fd, buf, 1023, 0)) > 0 ) {
			buf[s] = 0x0 ;
			printf("%s", buf) ;
			/*
			if (data == 0x0) {
				data = strdup(buf) ;
				len = s ;
			}
			else {
				data = realloc(data, len + s + 1) ;
				strncpy(data + len, buf, s) ;
				data[len + s] = 0x0 ;
				len += s ;
			}*/

		}
		printf("\n") ;
	}
	else /* get file */ {
        
        FILE* pf;
        char buffer[1024];
		pf = fopen(cmdline, "w") ;
		while ((s = recv(sock_fd, buffer, 1023, 0)) > 0 )
        { fwrite(buffer, 1, s, pf); }
		fclose(pf) ;
	}
} 

