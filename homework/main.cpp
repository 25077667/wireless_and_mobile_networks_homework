#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#define backoff 625

static void* behavior(void*);
void create_divices(int device_mounts, int times);
bool* channels = (bool*)malloc(sizeof(bool) * 80);
int collisions=0;
int get_random_channel(){
    srand(time(NULL)+rand());
    return ((rand()%17)*(rand()%23)) % 80;
}


int main(){
    memset(channels,0,sizeof(channels));
    printf("1. please wait\n");
    create_divices(2,16000);
    printf("collisions %d in %d times.\n",collisions,16000);
    collisions = 0;
    return 0;
}

void create_divices(int device_mounts, int times){
    for(int i=0;i<device_mounts;i++){
        pthread_t new_divice;
        pthread_create(&new_divice, NULL, behavior,&times);
        printf("AAAAAAAA\n");
    }
    //pthread_join(behavior,NULL);
}
static void* behavior(void* times){
    int test_time = *(int*)(times);
    //printf("%d\n",test_time);
    usleep(backoff);
    for(int i=0; i<test_time;i++){
        int use = get_random_channel();
        if(channels[use] == true){
            collisions ++;
            printf("%d \n", use);
            usleep(backoff);
        }
        else{
            channels[use] = true;
            usleep(backoff);
            channels[use] = false;
        }
    }
    pthread_exit(NULL);
}
