#include "socket_server.h"
// pthread_mutex_t 타입의 전역 변수 g_mutex를 선언한다.


void connection(Client *client) // getCommand 구조체를 인자로 받는 connection 함수를 정의한다.
{
    int optvalue = 1; // setsockopt 함수의 인자로 사용할 변수를 선언한다.
    int serv_sock;                  // 서버 소켓 파일 디스크립터를 저장할 변수를 선언한다.
    struct sockaddr_in serv_addr;   // 서버 주소 정보를 저장할 구조체를 선언한다.
    struct sockaddr_in clint_addr;  // 클라이언트 주소 정보를 저장할 구조체를 선언한다.
    socklen_t clnt_addr_size;    // 클라이언트 주소 정보 구조체의 크기를 저장할 변수를 선언한다.
    
    printf("Server init\n"); // 서버를 초기화한다.
    

    printf("Server Start\n"); // 서버가 시작되었음을 출력한다.
    // pthread_mutex_t 타입의 g_mutex 변수를 초기화한다.
    //pthread_mutex_init(&g_mutex, NULL);

    printf("set sever socket\n"); // 서버 소켓을 설정한다.
    serv_sock = socket(PF_INET, SOCK_STREAM, 0); // 서버 소켓을 생성하고 소켓 파일 디스크립터를 serv_sock 변수에 저장한다.
    if (serv_sock == -1)                         // 소켓 생성에 실패한 경우
    {
        perror("Server error!"); // 에러 메시지를 출력한다.
        exit(EXIT_FAILURE);      // 프로그램을 종료한다.
    }

    printf("set server address\n"); // 서버 주소를 설정한다.
    memset(&serv_addr, 0, sizeof(serv_addr)); // 서버 주소 정보 구조체를 초기화한다.
    serv_addr.sin_family = AF_INET;           // IPv4 주소 체계를 사용한다.
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); // 모든 인터페이스에서 접속을 허용한다.
    serv_addr.sin_port = htons(SERV_PORT);          // 서버 포트 번호를 저장한다.

    printf("bind server socket\n"); // 서버 소켓에 주소 정보를 바인딩한다.
    if (bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
    {
        perror("Server-bind() error!"); // 에러 메시지를 출력한다.
        exit(EXIT_FAILURE);             // 프로그램을 종료한다.
    }

    printf("listen server socket\n"); // 서버 소켓을 대기 상태로 전환한다.
    if (listen(serv_sock, BACKLOG) == -1)
    {
        perror("listen() error!"); // 에러 메시지를 출력한다.
        exit(EXIT_FAILURE);        // 프로그램을 종료한다.
    }

    printf("wait before accept client socket\n"); // 클라이언트의 접속을 대기한다.
    clnt_addr_size = sizeof(clint_addr);          // 클라이언트 주소 정보 구조체의 크기를 저장한다.
    client->socket = accept(serv_sock, (struct sockaddr *)&clint_addr, &clnt_addr_size);
    if (client->socket == -1) // 클라이언트의 접속에 실패한 경우
    {
        perror("accept() error!"); // 에러 메시지를 출력한다.
        exit(EXIT_FAILURE);        // 프로그램을 종료한다.
    }

    printf("accept client socket\n"); // 클라이언트의 접속을 수락한다.
    printf("client ip : %s\n", inet_ntoa(clint_addr.sin_addr)); // 클라이언트의 IP 주소를 출력한다.
    printf("client port : %d\n", ntohs(clint_addr.sin_port));   // 클라이언트의 포트 번호를 출력한다.

    response(client); // 클라이언트로부터 받은 명령어를 처리하는 함수를 호출한다.

    close(serv_sock); // 서버 소켓을 닫는다.

}

void response(Client* client)
{
    
    while(1)
    {
        char data[strlen(client->data)+1]; // 클라이언트로부터 받은 명령어를 저장할 배열을 선언한다.
        memset(data, 0, sizeof(data)); // data 배열을 초기화한다.
        
        int str_len = read(client->socket, data, sizeof(data)); // 클라이언트로부터 명령어를 받는다.
        if (str_len == -1 || str_len == 0)                            // 클라이언트와의 연결이 끊어진 경우
        {
            printf("FD[%d]의 연결이 끊어졌습니다.\n", client->socket); // 연결이 끊어졌음을 출력한다.
            close(client->socket);                                     // 소켓을 닫는다.
            break;                                    // 스레드를 종료한다.
        }
        printf("FD[%d] : RCV : %s",client->socket, data);

        memset(data, 0, sizeof(data));
        strcpy(data, client->data);
        str_len = write(client->socket, data, strlen(data)+1);
        printf("FD[%d] : SND : %d byte",client->socket, str_len);

    }
}