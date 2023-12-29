#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "socket_server.h"
#include "hwmoniter.h"
#include "toJson.h"


void* socketServer(void *client);
void* updateData(void *hw_info);

pthread_mutex_t g_mutex;
Client client; // getCommand 구조체를 선언한다.

void main()
{
    char buff[BUFFERSIZE]; // 버퍼를 512로 정의
    Hw_info *hw_info;
    hw_info = Hw_info_new();
    
    pthread_mutex_init(&g_mutex, NULL);

    pthread_t update;
    pthread_create(&update, NULL, updateData, (void*)hw_info);

   pthread_t socket;
   pthread_create(&socket, NULL, socketServer, (void*)&client);
    
    while(1);
}

void* updateData(void *hw_info)
{
    Hw_info *hw = (Hw_info*)hw_info;
    while(1)
    {
        sleep(1);
        pthread_mutex_lock(&g_mutex);
        getCpusInfo(hw);
        getRamInfo(hw);
        getDiskInfo(hw);
        getNetworkInfo(hw);
        client.data = json_object_to_json_string(hwInfoToJson(hw));
        printf("%s",client.data);
        pthread_mutex_unlock(&g_mutex);        
    }
}

void* socketServer(void *client)
{
    Client *this = (Client *)client;
    while (true)
    {
        connection(this);
    }
    
}