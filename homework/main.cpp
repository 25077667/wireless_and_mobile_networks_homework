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
    srand(time(NULL));
    //sleep(1);
    return ((rand()%17)*(rand()%251)) % 80;
}


int main(){
    memset(channels,0,sizeof(channels));
    printf("1. please wait\n");
    for(int i=0; i<100;i++)
        create_divices(2);
    printf("collisions %d in 100 times.\n",collisions);
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
int j=0;
void* behavior(void*){
    int used = get_random_channel();
    if(channels[used] == true){
        collisions ++;
        printf("%d %d \n",++j, used);
        sleep(10);
        behavior(NULL);
    }
    else{
        channels[used] = true;
        sleep(10);
        channels[used] = false;
    }
    pthread_exit(NULL);
}
