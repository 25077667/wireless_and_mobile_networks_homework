#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#define backoff 625
#define hopping_rate 1600

struct channel_struct{
    bool is_using;
    bool bad_channel;
}channels[80];

void Q2_create_divices(int device_mounts, int times, int select_function);
void* Q2_behavior(void* times);
void* channel_sniffer(void*);
void try_a_channel(int selection);
void* Q3a_behavior(void* times);
void* Q3b_behavior(void* times);
void initialize(){
    pthread_t sniffer = (pthread_t)malloc(sizeof(pthread_t));
    pthread_create(&(sniffer), NULL, channel_sniffer,NULL);
}

int collisions=0,hop=0;
pthread_t sniffer;
int get_random_channel(){
    srand(time(NULL)+rand());
    return ((rand()%17)*(rand()%23)+rand()) % 80;
}


int main(){
    initialize();
    printf("1. please wait\n");
    int testing_length = 2;
    Q2_create_divices(2,testing_length,1);
    printf("collisions %d times and hopping %d times in %d seconds.\n",collisions,hop,testing_length);

    printf("2. please wait\n");
    for(int i=2;i<=80;i++){
        hop = collisions = 0;
        Q2_create_divices(i,testing_length,1);
        printf("Q2 %d devices collision %d and hopping %d times in %d seconds.\n",i,collisions,hop,testing_length);
    }

    printf("3. please wait\n");
    for(int i=2;i<=80;i++){
        hop = collisions = 0;
        Q2_create_divices(i,testing_length,2);
        printf("Q3(a) %d devices collision %d and hopping %d times in %d seconds.\n",i,collisions,hop,testing_length);
    }

    printf("\n\n");
    for(int i=2;i<=80;i++){
        hop = collisions = 0;
        Q2_create_divices(i,testing_length,3);
        printf("Q3(b) %d devices collision %d and hopping %d times in %d seconds.\n",i,collisions,hop,testing_length);
    }

    return 0;
}

void try_a_channel(int selection){
    if(channels[selection].is_using == true){
        collisions ++;
        //printf("%d ",selection);
        usleep(backoff);
    }
    else{
        channels[selection].is_using = true;
        usleep(backoff);
        channels[selection].is_using = false;
    }
    hop++;
}

void Q2_create_divices(int device_mounts, int times,int select_function){
    void* (*result)(void*);  //call function by pointer
    switch (select_function){
    case 1:
        result = Q2_behavior;
        break;
    case 2:
        result = Q3a_behavior;
        break;
    case 3:
        result = Q3b_behavior;
        break;
    default:
        result = Q2_behavior;
        printf("selection error!!\n");
    }

    pthread_t* new_divice;
    new_divice = (pthread_t*)malloc(sizeof(pthread_t)*device_mounts);
    for(int i=0;i<device_mounts;i++)
        pthread_create(&(new_divice[i]), NULL, result,&times);
    for(int i=0;i<device_mounts;i++)
        pthread_join((new_divice[i]),NULL);
}

void* Q2_behavior(void* times){
    int test_time = *(int*)(times);
    for(int i=0; i<test_time*hopping_rate;i+=2)
        try_a_channel(get_random_channel());
    pthread_exit(NULL);
}

void clean_all_channel(){
    for(int i=0;i<80;i++)
         channels[i].is_using = channels[i].bad_channel = false;    //initialize all channel to be normal channel
}

void* channel_sniffer(void*){
    int channel_map[80][11] = {0};
    clean_all_channel();
    for(int i=0;i<10;i++){
        for(int j=0;j<80;j++)
            channel_map[i][j] = (int)channels[j].is_using;  //initialize the channel map
        usleep(100000);     //sleep for 0.1 seconds
    }
    while(true){
        for(int i=0;i<80;i++)
            channel_map[10][i] += (int)channels[i].is_using;
        memcpy(channel_map,&(channel_map[1][0]),sizeof(int)*80*10);

        for(int i=0;i<80;i++){
            int appeared = 0;
            for(int j=0;j<10;j++)
                appeared += channel_map[j][i];
            if(appeared>5)
                channels[i].bad_channel = true;
        }
        usleep(100000);     //sleep for 0.1 seconds
    }
}

int search_nearby_normal_channel(int selection){
    bool is_all_bad=true;
    int result = -1;
    for(int i=0;i<80;i++){
        if(channels[i].bad_channel == false){
            is_all_bad = false;
            break;
        }
    }
    if(is_all_bad){
        return result;
    }

    //find left
    int left=selection, right=selection;
    while(--left && left>0){
        if(channels[left].bad_channel == false && channels[left].is_using==false)
            break;
    }
    //find right
    while(++right && right<79){
        if(channels[right].bad_channel == false && channels[right].is_using==false)
            break;
    }

    if((selection-left)>(right - selection))
        result = (right==79)?selection:right;
    else
        result = (left==0)?selection:left;
    return result;
}

void* Q3a_behavior(void* times){
    int test_time = *(int*)(times);
    for(int i=0; i<test_time*hopping_rate;i+=2){
        //isomorphic try a channel
        int selection = get_random_channel();
        if (channels[selection].bad_channel == true){
            int result = search_nearby_normal_channel(selection);
            if(result == -1)
                try_a_channel(get_random_channel());
            else
                try_a_channel(result);
        }
        else
            try_a_channel(selection);
    }
}

int search_far_normal_channel(int selection){
    bool is_all_bad=true;
    int result = -1;
    for(int i=0;i<80;i++){
        if(channels[i].bad_channel == false){
            is_all_bad = false;
            break;
        }
    }
    if(is_all_bad){
        return result;
    }

    //find left
    int left=0, right=79;
    while(left++ && left!=selection){
        if(channels[left].bad_channel == false && channels[left].is_using==false)
            break;
    }
    //find right
    while(right-- && right!=selection){
        if(channels[right].bad_channel == false && channels[right].is_using==false)
            break;
    }

    if((selection-left)>(right - selection))
        result = (left==0)?selection:left;
    else
        result = (right==79)?selection:right;
    return result;
}

void* Q3b_behavior(void* times){
        int test_time = *(int*)(times);
    for(int i=0; i<test_time*hopping_rate;i+=2){
        //isomorphic Q3b_behavior
        int selection = get_random_channel();
        if (channels[selection].bad_channel == true){
            int result = search_far_normal_channel(selection);
            if(result == -1)
                try_a_channel(get_random_channel());
            else
                try_a_channel(result);
        }
        else
            try_a_channel(selection);
    }
}
