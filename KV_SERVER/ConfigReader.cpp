#include "KVHeader.h"
// LISTENING_PORT=8080
// THREAD_POOL_SIZE_INITIAL=10
// THREAD_POOL_GROWTH=2
// CLIENTS_PER_THREAD=2
// CACHE_SIZE=10
// CACHE_REPLACEMENT=LRU

//Config file directory
const char *config_filename = "./Config/server.config";

CONFIGRATION* read_config_file()
{
    FILE * file;

    if((file = fopen(config_filename, "r")) == NULL)
    {
        printf("Configuration file could not be read\n");
    }
    
    CONFIGRATION* config_file = (CONFIGRATION *) malloc(sizeof(CONFIGRATION));
    size_t len = 0;
    ssize_t read;
    char * line = NULL;
    


    if((read = getline(&line, &len, file))!=-1)
    {
        sscanf(line, "LISTENING_PORT=%d", &(config_file->port_no));
    }
    if((read = getline(&line, &len, file))!=-1)
    {
        sscanf(line, "THREAD_POOL_SIZE_INITIAL=%d", &(config_file->thread_pool_size));
    }
    if((read = getline(&line, &len, file))!=-1)
    {
        sscanf(line, "THREAD_POOL_GROWTH=%d", &(config_file->thread_pool_growth));
    }
    if((read = getline(&line, &len, file))!=-1)
    {
        sscanf(line, "CLIENTS_PER_THREAD=%d", &(config_file->client_per_thread));
    }
    if((read = getline(&line, &len, file))!=-1)
    {
        sscanf(line, "CACHE_SIZE=%d", &(config_file->cache_size));
    }
    if((read = getline(&line, &len, file))!=-1)
    {
        sscanf(line, "CACHE_REPLACEMENT=%s", (config_file->cache_policy));
        config_file->cache_policy[3] = '\0';
    }

    return config_file;

}