#include "KVHeader.h"

extern CONFIGRATION* config_file;
vector<Worker_Thread*> worker_list;

void create_thread_pool(int no_workers)
{   
    for(long i = 0; i < no_workers; i++)
    {
        Worker_Thread *worker = (Worker_Thread*)malloc(sizeof(Worker_Thread));
        worker_list.push_back(worker);
        pthread_mutex_init(&(worker->mutex), NULL);
        pthread_cond_init(&(worker->cond), NULL);
        pthread_create(&(worker->thread), NULL, do_work, (void *) i);
    }
}

void * do_work(void * args)
{
    long index = (long) args;
    int temp_client_count = 0;
    printf("I am thread %ld\n", index);

    char buffer[MAXBUF];

    // Epoll create
    struct epoll_event events[config_file->client_per_thread];
    int epoll_fds = epoll_create(config_file->client_per_thread);
    int nfds;


    pthread_mutex_lock(&(worker_list[index]->mutex));
    if(worker_list[index]->client_fd_queue.size() == 0)
        pthread_cond_wait(&(worker_list[index]->cond), &(worker_list[index]->mutex));
    pthread_mutex_unlock(&(worker_list[index]->mutex));
    //add to epoll list
    while(1)
    {
        if(temp_client_count < worker_list[index]->no_client)
        {
            static struct epoll_event ev;
            ev.data.fd = worker_list[index]->client_fd_queue[temp_client_count];
            ev.events = EPOLLIN;
            epoll_ctl(epoll_fds, EPOLL_CTL_ADD, ev.data.fd, &ev);
            temp_client_count++;
        }
        nfds = epoll_wait(epoll_fds, events, 5, 10000);
        for (int i = 0; i < nfds; i++)
        {
            memset(buffer, 0, MAXBUF);
            read(events[i].data.fd, buffer, MAXBUF);
            puts(buffer);
        }
    }
    pthread_exit(NULL);
}