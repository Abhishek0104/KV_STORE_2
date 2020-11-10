#include "KVHeader.h"

// LISTENING_PORT=8080
// THREAD_POOL_SIZE_INITIAL=10
// THREAD_POOL_GROWTH=2
// CLIENTS_PER_THREAD=2
// CACHE_SIZE=10
// CACHE_REPLACEMENT=LRU
//todo locking
int cache_size;
list<char *> cache_queue;
// unordered_map<char *, CACHE_STORE > cache_map;
unordered_map<char *, list<char *>::iterator> key_queue;
unordered_map<char *, char *> key_value;

extern FILE* file;
int status;
// char *cache_policy;

void init_cache(CONFIGRATION * config_file)
{
    cache_size = config_file->cache_size;
    // cache_policy = config_file->cache_policy;

}

int put(char * message)
{
    char *key = (char*)malloc(sizeof(char)*257);
    char *kvalue = (char*)malloc(sizeof(char)*257);
    memset(key, 0, 257);
    memset(kvalue, 0, 257);
    memcpy(key, &message[1], 256);
    key[257] = '\0';
    memcpy(kvalue, &message[257], 256);
    kvalue[257] = '\0';
    printf("Mesaage:%s", message);
    printf("PUT:Key is %s\n", key);
    printf("kvalue is %s\n", kvalue);
    
    for (auto ele: key_value)
    {
        cout << "Key: "<<ele.first<<"\nValue:"<<ele.second<<endl;
    }

    //todo Store first to KV_store
    status =  insert(file, key, kvalue);

    if (status == 1)
    {
        if(key_queue.find(key) == key_queue.end()){
            cout<<"CACHE:Not in cache"<<"\n";
            if(cache_queue.size() == cache_size){//modified write in KVstore
                char * last_elem = cache_queue.back();
                printf("CACHE:Replaced \n");
                key_queue.erase(last_elem);
                key_value.erase(last_elem);
                // cache_map.erase(last_elem);
                // cache_queue.pop_back();
            }
            cache_queue.push_front(key);
            // cache_map[key].value = kvalue;
            // cache_map[key].iter = cache_queue.begin();
            key_value[key] = kvalue;
            key_queue[key] = cache_queue.begin();
            
        }
        else//kvalue update
        {
            cout<<"CACHE: kvalue updated\n";
            cache_queue.erase(key_queue[key]);
            cache_queue.push_front(key);
            // cache_map[key].value = kvalue;
            // cache_map[key].iter = cache_queue.begin();
            key_value[key] = kvalue;
            key_queue[key] = cache_queue.begin();
        }
    }

    
    // return 1;
    free(key);
    free(kvalue);
    return status;
}

char * get(char* message)
{
    char *key = (char*)malloc(sizeof(char)*257);
    memset(key, 0, 257);
    // memset(kvalue, 0, 257);
    memcpy(key, &message[1], 256);
    key[257] = '\0';
    // printf("GET:Key is %s\n", key);

    if(key_queue.find(key)==key_queue.end()){
        //todo store in cache
        return search_value(file, key);
    }
    else
    {
        cache_queue.erase(key_queue[key]);
        cache_queue.push_front(key);
        key_queue[key] = cache_queue.begin();
        return key_value[key];
    }
    
}

int del(char * message)
{
    char *key = (char*)malloc(sizeof(char)*257);
    memset(key, 0, 257);
    // memset(kvalue, 0, 257);
    memcpy(key, &message[1], 256);
    key[257] = '\0';
    // printf("CACHE:DEL:Key is %s\n", key);


    if(key_queue.find(key) != key_queue.end())
    {   
        cout<<"CACHE: Deleted from cache :("<<"\n";
        cache_queue.erase(key_queue[key]);
        key_value.erase(key);
        key_queue.erase(key);
        // cache_map.erase(key);
    }
    //todo delete from store if present or sent error
    status = delete_entry(file, key);
    free(key);
    return status;
    
}


