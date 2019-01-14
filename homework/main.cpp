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
int collisions=0,hop=0;
int get_random_channel(){
    srand(time(NULL)+rand());
    return ((rand()%17)*(rand()%23)) % 80;
}

int main(){
    memset(channels,0,sizeof(channels));
    printf("1. please wait\n");
    int testing_length = 1;
    create_divices(2,testing_length);
    printf("collisions %d times and hopping %d times in %d seconds.\n",collisions,hop,testing_length);

    printf("2. please wait\n");
    for(int i=2;i<80;i++){
        collisions = 0;
        hop=0;
        create_divices(i,testing_length);
        printf("there are %d devices collision %d and hopping %d times in %d seconds.\n",i,collisions,hop,testing_length);
    }
    return 0;
}

void create_divices(int device_mounts, int times){
        pthread_t* new_divice;
        new_divice = (pthread_t*)malloc(sizeof(pthread_t)*device_mounts);

        for(int i=0;i<device_mounts;i++){
            pthread_create(&(new_divice[i]), NULL, behavior,&times);
        }

        for(int i=0;i<device_mounts;i++)
            pthread_join((new_divice[i]),NULL);
}

static void* behavior(void* times){
    int test_time = *(int*)(times);

    for(int i=0; i<test_time*hopping_rate;i+=2){
        int use = get_random_channel();
        if(channels[use] == true){
            collisions ++;
            //printf("%d \n", use);
            usleep(backoff);
        }
        else{
            channels[use] = true;
            usleep(backoff);
            channels[use] = false;
        }
        hop++;
    }
    pthread_exit(NULL);
}
