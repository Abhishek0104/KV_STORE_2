/*
KVServer will consist of a ​main thread​ that will perform the following steps:
1. It will read a config file and set up a listening socket on a port specified in the config file.
2. Create​ n​ threads at startup which will be specified in the config file.
3. Perform accept() on the listening socket and pass each established connection to one of
the ​n threads in a round robin fashion. For example, the first connection is passed on to
the first thread of those n thread for processing, and so on.
The KVServer will also have ​worker threads​ that are responsible for the following actions:
1. Each worker thread will set up and monitor an epoll context for the set of connections it
is responsible for.
2. It runs an infinite loop that will monitor its client connections for work (in addition, if there
are new connections for the thread, it must add it to the epoll context). After reading
requests from clients, it will process those requests, and write a response back to the
client
*/

#include "KVHeader.h"

int server_socket;
struct sockaddr_in client;
struct sockaddr_in addr;
CONFIGRATION* config_file;
extern vector<Worker_Thread*> worker_list;

void sigterm_handler(int sig)
{
    printf("Got SIGTERM-Leaving\n");
    close(server_socket);
    exit(0);
}


int main()
{

    /*--SIGNAL HANDLER COMMANDS--*/
    signal(SIGINT, sigterm_handler);
    signal(SIGTERM, sigterm_handler);
    /*--------END_SIGNALS--------*/

    config_file = read_config_file();

    printf("Port_No:%d\n", config_file->port_no);
    printf("Thread Pool Size: %d\n", config_file->thread_pool_size);
    printf("Thread Pool Growth: %d\n", config_file->thread_pool_growth);
    printf("Clients per thread: %d\n", config_file->client_per_thread);
    printf("Cache Size: %d\n", config_file->cache_size);
    printf("Cache Policy: %s\n", config_file->cache_policy);

    printf("Start Server\n");
    start_server();
    printf("Create Thread Pool\n");
    create_thread_pool(config_file->thread_pool_size);
    printf("Accept Connection\n");
    accept_connection();

    return 0;
}

void start_server()
{
    // Create a socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    // Reusable Socket
    int temp = 1;
    if(setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &temp, sizeof(int)) < 0)
    {
        perror("Setsockopt failed.\n");
    }

    
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    printf("Here\n");
    addr.sin_port = htons(config_file->port_no);
    printf("there\n");
    addr.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket
    if (bind(server_socket, (struct sockaddr *)&addr, sizeof(addr)) != 0)
    {
        perror("Address cannot bind.\n");
        exit(1);
    }

    // Listen for connection
    if (listen(server_socket, 10) != 0)
    {
        perror("Error in Listen.\n");
        exit(1);
    }
    printf("Server Connected..\n");
}

void accept_connection()
{
    while(1)
    {
        memset(&client, 0, sizeof(client));
        int addrlen = sizeof(client);
        int fd = accept(server_socket, (struct sockaddr *)&client, (socklen_t *)&addrlen);
        printf("New Client accepted.\n");
        for(long int k = 0; k < worker_list.size(); k++)
        {
            if(worker_list[k]->no_client < config_file->client_per_thread)
            {
                printf("Goes to thread no. %d\n", k);
                pthread_mutex_lock(&(worker_list[k]->mutex));
                worker_list[k]->client_fd_queue.push_back(fd);
                pthread_cond_signal(&(worker_list[k]->cond));
                pthread_mutex_unlock(&(worker_list[k]->mutex));
                worker_list[k]->no_client++;
                break;
            }
        }
    }
}