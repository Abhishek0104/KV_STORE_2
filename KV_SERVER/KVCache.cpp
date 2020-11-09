#include "KVHeader.h"

// LISTENING_PORT=8080
// THREAD_POOL_SIZE_INITIAL=10
// THREAD_POOL_GROWTH=2
// CLIENTS_PER_THREAD=2
// CACHE_SIZE=10
// CACHE_REPLACEMENT=LRU

int cache_size;
list<char *> cache_queue;
unordered_map<char *, CACHE_STORE> cache_map;
char key[257];
char kvalue[257];
// char *cache_policy;

void init_cache(CONFIGRATION * config_file)
{
    cache_size = config_file->cache_size;
    // cache_policy = config_file->cache_policy;

}

int put(char * message)
{
    memcpy(key, &message[1], 256);
    key[257] = '\0';
    memcpy(kvalue, &message[257], 256);
    kvalue[257] = '\0';
    printf("PUT:Key is %s\n", key);
    printf("kvalue is %s\n", kvalue);
    

    if(cache_map.find(key) == cache_map.end()){
        cout<<"Not in cache"<<"\n";
        if(cache_queue.size() == cache_size){//modified write in KVstore
            char * last_elem = cache_queue.back();
            printf("Replaced %s\n",last_elem);
            // key_queue.erase(last_elem);
            // key_kvalue.erase(last_elem);
            cache_map.erase(last_elem);
            cache_queue.pop_back();
        }
        cache_queue.push_front(key);
        cache_map[key].value = kvalue;
        cache_map[key].iter = cache_queue.begin();
        
    }
    else//kvalue update
    {
        cout<<"kvalue updated\n";
        cache_queue.erase(cache_map[key].iter);
        cache_queue.push_front(key);
        cache_map[key].value = kvalue;
        cache_map[key].iter = cache_queue.begin();
    }
    return 1;
    //todo Store first to KV_store

}

char * get(char* message)
{
    memcpy(key, &message[1], 256);
    key[257] = '\0';
    printf("GET:Key is %s\n", key);

    if(cache_map.find(key)==cache_map.end()){
        // todo search in kvstore if present add to cache else error
        return NULL;
    }
    else
    {
        cache_queue.erase(cache_map[key].iter);
        cache_queue.push_front(key);
        cache_map[key].iter = cache_queue.begin();
        return cache_map[key].value;
    }

}

int del(char * message)
{
    memcpy(key, &message[1], 256);
    key[257] = '\0';
    printf("DEL:Key is %s\n", key);


    if(cache_map.find(key) != cache_map.end())
    {   
        cout<<key<<" Deleted from cache :("<<"\n";
        cache_queue.erase(cache_map[key].iter);
        // key_kvalue.erase(key);
        // key_queue.erase(key);
        cache_map.erase(key);
        return 1;
    }
    return 0;
    //todo delete from store if present or sent error
}


