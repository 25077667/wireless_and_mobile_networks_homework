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

int main(){
    srand(time(NULL));
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
    int used = rand()%80;
    if(channels[used] == true){
        collisions ++;
        printf("%d\n",++j);
        //behavior(NULL);
    }
    else{
        channels[used] = true;
        sleep(10);
        channels[used] = false;
    }
    pthread_exit(NULL);
}
