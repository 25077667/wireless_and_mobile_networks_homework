#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

void* behavior(void*);
void create_divices(int numbers);
bool* channels = (bool*)malloc(sizeof(bool) * 79);
int collisions=0;
int get_random_channel(){
    srand(time(NULL)+rand());
    return ((rand()%17)*(rand()%23)) % 80;
}


int main(){
    memset(channels,0,sizeof(channels));
    printf("1. please wait\n");
    int i;
    for(i=0; i<10000;i++)
        create_divices(2);
    printf("collisions %d in %d times.\n",collisions,i);
    collisions = 0;
    return 0;
}

void create_divices(int numbers){
    for(int i=0;i<numbers;i++){
        pthread_t new_divice;
        pthread_create(&new_divice, NULL, behavior,NULL);
    }
    //pthread_join(behavior,NULL);
}
void* behavior(void*){
    int used = get_random_channel();
    if(channels[used] == true){
        collisions ++;
        printf("%d \n", used);
        usleep(1);
        behavior(NULL);
    }
    else{
        channels[used] = true;
        usleep(1);
        channels[used] = false;
    }
    pthread_exit(NULL);
}
