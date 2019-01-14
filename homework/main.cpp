#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#define backoff 625
#define hopping_rate 1600

void create_divices(int device_mounts, int times);
static void* behavior(void* times);
bool* channels = (bool*)malloc(sizeof(bool) * 80);
int collisions=0;
int get_random_channel(){
    srand(time(NULL)+rand());
    return ((rand()%17)*(rand()%23)) % 80;
}

int main(){
    memset(channels,0,sizeof(channels));
    printf("1. please wait\n");
    int testing_length = 2;
    create_divices(2,testing_length);
    printf("collisions %d in %d times.\n",collisions,testing_length*1000);
    collisions = 0;
    return 0;
}

void create_divices(int device_mounts, int times){
        pthread_t* new_divice;
        new_divice = (pthread_t*)malloc(sizeof(pthread_t)*device_mounts);

        for(int i=0;i<device_mounts;i++)
            pthread_create(&(new_divice[i]), NULL, behavior,&times);

        printf("AAAAAAAA\n");

        for(int i=0;i<device_mounts;i++)
            pthread_join((new_divice[i]),NULL);
}

static void* behavior(void* times){
    int test_time = *(int*)(times);

    for(int i=0; i<test_time*hopping_rate;i++){
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
