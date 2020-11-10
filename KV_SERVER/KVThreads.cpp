#include "KVHeader.h"

extern CONFIGRATION* config_file;
vector<Worker_Thread*> worker_list;
char * value = (char *)malloc(sizeof(char)*256);
int put_success;
pthread_mutex_t cache_lock;
char send_buffer[MAXBUF];

void add_padding(char * str)
{
	int len = 0;
	printf("%s\n", str);
	while (str[len]!='\n'&&str[len]!='\0')
	{
		len++;
	}
	
	while(len < 513)
	{
		str[len++] = '-';
	}
	str[len] = '\0';
}

void create_thread_pool(int no_workers)
{  
    pthread_mutex_init(&cache_lock, NULL); 
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
        if(nfds > 0)
        {
            for (int i = 0; i < nfds; i++)
            {
                    memset(buffer, 0, MAXBUF);
                    read(events[i].data.fd, buffer, MAXBUF);
                    int fd = events[i].data.fd;
                    if(buffer[0] == '1')
                    {
                        
                        memset(value, 0, sizeof(value));
                        pthread_mutex_lock(&cache_lock);
                        value = get(buffer);
                        pthread_mutex_unlock(&cache_lock);
                        if(value == NULL)
                        {
                            //send error
                            memset(send_buffer, 0, MAXBUF);
                            strcpy(send_buffer, "0");
                            strcat(send_buffer, "Key not found in cache.");
                            strcat(send_buffer, "");
                            add_padding(send_buffer);
                            printf("Thread:key not in cache\n");
                            send(events[i].data.fd, send_buffer, strlen(send_buffer), 0);
                        } else
                        {
                            // send value
                            printf("Thread:%s\n", value);
                            memset(send_buffer, 0, MAXBUF);
                            strcpy(send_buffer, "1");
                            strcat(send_buffer, value);
                            strcat(send_buffer, "");
                            add_padding(send_buffer);
                            send(events[i].data.fd, send_buffer, strlen(send_buffer), 0);
                        }
                        
                    } else if(buffer[0] == '2')
                    {
                        // memset(value, 0, sizeof(value));
                        // cout << "Put Request" << endl;
                        // printf("Key:");
                        // for(int i = 1; i<257; i++)
                        // {
                        //     printf("%s", buffer[i]);
                        // }
                        // printf("\n");
                        // printf("Value:");
                        // for(int i = 257; i<513; i++)
                        // {
                        //     printf("%s", buffer[i]);
                        // }
                        // printf("\n");
                        pthread_mutex_lock(&cache_lock);
                        put_success = put(buffer);
                        pthread_mutex_unlock(&cache_lock);
                        if(put_success == -1)
                        {
                            printf("Thread:Error while putting\n");
                        } else
                        {
                            //send success
                            printf("Thread:Success in put\n");
                        }
                        
                    } else if(buffer[0] == '3')
                    {
                        // cout<<"Del Request"<<endl;
                        // printf("Key:");
                        // for(int i = 1; i<257; i++)
                        // {
                        //     printf("%s", buffer[i]);
                        // }
                        // printf("\n");
                        memset(value, 0, sizeof(value));
                        pthread_mutex_lock(&cache_lock);
                        put_success = del(buffer);
                        pthread_mutex_unlock(&cache_lock);
                        if(put_success == 1)
                        {
                            //send error
                            printf("Thread:value deleted\n");
                        } else
                        {
                            //send success
                            printf("Thread:value to be deleted not found \n");
                        }
                    } else 
                    {
                        // invalid status code.
                        printf("Thread:how this is invalid\n");
                    }
                    // send to kv_cache

                    //recieve from kv_cache
                    
                    // puts(buffer);
                
                // else if(events[i].events & EPOLLOUT)
                // {
                //     // printf("here epollout\n");
                //     write(events[i].data.fd, send_buffer, strlen(send_buffer));
                // }
                
            }
        }
    }
    pthread_exit(NULL);
}