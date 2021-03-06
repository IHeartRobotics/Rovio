#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>

#include "test_remotefunc.h"

void *test_netread_client(void *parg);

void test_netread_entry(void)
{
    pthread_t netread_client[TEST_NETREAD_CLIENT_NUM];
    int thread_arg[TEST_NETREAD_CLIENT_NUM];
    int i;
    
    for(i = 0; i < TEST_NETREAD_CLIENT_NUM; i++)
    {
        thread_arg[i] = TEST_NETREAD_SERVER_BEGIN_PORT + i;
        pthread_create(&netread_client[i], 0, &test_netread_client, &thread_arg[i]);
    }
    
    for(i = 0; i < TEST_NETREAD_CLIENT_NUM; i++)
    {
        pthread_join(netread_client[i], NULL);
    }
}

void *test_netread_client(void *parg)
{
    int s, i, len;
    struct sockaddr_in sa = {0}, r_sa = {0};
    int port = *(int*)parg;
    char msg[TEST_REMOTEFUNC_MSG_LEN];
    
    if(inet_aton(TEST_NETREAD_SERVER_ADDR, &r_sa.sin_addr) == 0)
    {
        printf("inet_aton error\n");
        return NULL;
    }
    r_sa.sin_family = AF_INET;
    r_sa.sin_port = htons(IPPORT_USERRESERVED + port);
    //printf("sending to %s : %d\n", inet_ntoa(*((struct in_addr*)hp->h_addr)), r_sa.sin_port);
    
    sa.sin_addr.s_addr = INADDR_ANY;
    sa.sin_family = AF_INET;
    sa.sin_port = htons(IPPORT_USERRESERVED + port);
    
    if((s = socket(AF_INET, SOCK_STREAM, PF_UNSPEC)) == -1)
    {
        printf("socket error\n");
        return NULL;
    }
    printf("socket success...\n");
    
    set_reuseaddr(s);
    if(bind(s, (struct sockaddr*)&sa, sizeof(sa)) == -1)
    {
        printf("bind error\n");
        return NULL;
    }
    printf("bind success...\n");
    
    if(connect(s, (struct sockaddr*)&r_sa, sizeof(sa)) == -1)
    {
        printf("connect error\n");
        return NULL;
    }
    printf("connect success...\n");
    
    for(i = 0; i < TEST_NETREAD_WRITE_TIMES; i++)
    {
        len = sprintf(msg, "%s", MYREQUEST);
        len++;
        if((len = write(s, msg, len)) == -1)
        {
            printf("write error\n");
            break;
        }
    }
    printf("write all ok\n");
    
    close(s);
    return NULL;
 }

int main(int argc, char *argv[])
{
    test_netread_entry();
    
    return 0;
}
