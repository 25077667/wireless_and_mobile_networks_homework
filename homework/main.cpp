#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <vector>
#include <cmath>
#define backoff 625
#define hopping_rate 1600
#define total_channel 80
#define standard_radius 3.4225704501373

using namespace std;

struct channel_struct{
    bool is_using;
    bool bad_channel;
}channels[80];

void create_divices(int device_mounts, int times, int select_function);
void* channel_sniffer(void*);
void* enhance_channel_sniffer(void*);
void try_a_channel(int selection);
void* Q2_behavior(void* times);
void* Q3a_behavior(void* times);
void* Q3b_behavior(void* times);
void* Q3c_behavior(void* times);
void* Q3d_behavior(void* times);
void* Q4_behavior(void* times);
void* Q5_behavior(void* times);

int collisions=0,hop=0;
int get_random_channel(int module){
    srand(time(NULL)+rand());
    return ((rand()%17)*(rand()%23)+rand()) % module;
}
bool* A1 = (bool*)malloc(sizeof(bool)*20);
bool* A2 = (bool*)malloc(sizeof(bool)*20);
bool* A3 = (bool*)malloc(sizeof(bool)*20);
bool* B1 = (bool*)malloc(sizeof(bool)*20);
bool* B2 = (bool*)malloc(sizeof(bool)*20);
bool* B3 = (bool*)malloc(sizeof(bool)*20);
bool* C1 = (bool*)malloc(sizeof(bool)*20);
bool* C2 = (bool*)malloc(sizeof(bool)*20);
bool* C3 = (bool*)malloc(sizeof(bool)*20);
bool* D1 = (bool*)malloc(sizeof(bool)*20);
bool* D2 = (bool*)malloc(sizeof(bool)*20);
bool* D3 = (bool*)malloc(sizeof(bool)*20);
bool time_for_Q4=false;

void initialize(){
    memset(A1,0,sizeof(bool)*20);
    memset(A2,0,sizeof(bool)*20);
    memset(A3,0,sizeof(bool)*20);
    memset(B1,0,sizeof(bool)*20);
    memset(B2,0,sizeof(bool)*20);
    memset(B3,0,sizeof(bool)*20);
    memset(C1,0,sizeof(bool)*20);
    memset(C2,0,sizeof(bool)*20);
    memset(C3,0,sizeof(bool)*20);
    memset(D1,0,sizeof(bool)*20);
    memset(D2,0,sizeof(bool)*20);
    memset(D3,0,sizeof(bool)*20);

    pthread_t sniffer = (pthread_t)malloc(sizeof(pthread_t));
    pthread_create(&(sniffer), NULL, channel_sniffer,NULL);
    pthread_t enhace_sniffer = (pthread_t)malloc(sizeof(pthread_t));
    pthread_create(&(enhace_sniffer), NULL, enhance_channel_sniffer,NULL);
}

int main(){
    initialize();
    int testing_length = 2; //2 seconds

    FILE* file_pointer;
    file_pointer = fopen("result5.txt","w");

    printf("1. please wait\n");
    create_divices(2,testing_length,1);
    printf("collisions %d times and hopping %d times in %d seconds.\n",collisions,hop,testing_length);

    printf("2. please wait\n");
    for(int i=2;i<total_channel;i++){
        hop = collisions = 0;
        create_divices(i,testing_length,1);
        printf("Q2 %d devices collision %d and hopping %d times in %d seconds.\n",i,collisions,hop,testing_length);
        fprintf(file_pointer,"Q2\t%d\t%d\t%d\r\n",i,collisions,hop);
    }

    printf("3. please wait\na.\n");
    for(int i=2;i<total_channel;i++){
        hop = collisions = 0;
        create_divices(i,testing_length,2);
        printf("Q3(a) %d devices collision %d and hopping %d times in %d seconds.\n",i,collisions,hop,testing_length);
        fprintf(file_pointer,"Q3(a)\t%d\t%d\t%d\r\n",i,collisions,hop);
    }

    printf("\nb.\n");
    for(int i=2;i<total_channel;i++){
        hop = collisions = 0;
        create_divices(i,testing_length,3);
        printf("Q3(b) %d devices collision %d and hopping %d times in %d seconds.\n",i,collisions,hop,testing_length);
        fprintf(file_pointer,"Q3(b)\t%d\t%d\t%d\r\n",i,collisions,hop);
    }

    printf("\nc.\n");
    for(int i=2;i<total_channel;i++){
        hop = collisions = 0;
        create_divices(i,testing_length,4);
        printf("Q3(c) %d devices collision %d and hopping %d times in %d seconds.\n",i,collisions,hop,testing_length);
        fprintf(file_pointer,"Q3(c)\t%d\t%d\t%d\r\n",i,collisions,hop);
    }

    printf("\nd.\n");
    for(int i=2;i<total_channel;i++){
        hop = collisions = 0;
        create_divices(i,testing_length,5);
        printf("Q3(d) %d devices collision %d and hopping %d times in %d seconds.\n",i,collisions,hop,testing_length);
        fprintf(file_pointer,"Q3(d)\t%d\t%d\t%d\r\n",i,collisions,hop);
    }

    printf("4. please wait.\n");
    time_for_Q4 = true;
    for(int i=2;i<total_channel;i++){
        hop = collisions =0;
        create_divices(i,testing_length,6);
        printf("Q4 %d devices collision %d and hopping %d times in %d seconds.\n",i,collisions,hop,testing_length);
        fprintf(file_pointer,"Q4\t%d\t%d\t%d\r\n",i,collisions,hop);
    }
    time_for_Q4 = false;

    //Q5 is a bad code, using Set theory to solve it, but it seems to be bad
    printf("5. please wait.\n");
    for(int i=2;i<total_channel;i++){
        hop = collisions =0;
        create_divices(i,testing_length,7);
        printf("Q5 %d devices collision %d and hopping %d times in %d seconds.\n",i,collisions,hop,testing_length);
        fprintf(file_pointer,"Q5\t%d\t%d\t%d\r\n",i,collisions,hop);
    }
    fclose(file_pointer);
    return 0;
}


void clean_all_channel(){
    for(int i=0;i<total_channel;i++)
         channels[i].is_using = channels[i].bad_channel = false;    //initialize all channel to be normal channel
}

void* channel_sniffer(void*){
    int channel_map[80][11] = {0};
    clean_all_channel();
    for(int i=0;i<10;i++){
        for(int j=0;j<total_channel;j++)
            channel_map[i][j] = (int)channels[j].is_using;  //initialize the channel map
        usleep(100000);     //sleep for 0.1 seconds
    }
    while(true){
        if(time_for_Q4) continue;

        for(int i=0;i<total_channel;i++)
            channel_map[10][i] += (int)channels[i].is_using;
        memcpy(channel_map,&(channel_map[1][0]),sizeof(int)*total_channel*10);

        for(int i=0;i<total_channel;i++){
            int appeared = 0;
            for(int j=0;j<10;j++)
                appeared += channel_map[j][i];
            if(appeared>6)
                channels[i].bad_channel = true;
            else
                channels[i].bad_channel = false;
        }
        usleep(100000);     //sleep for 0.1 seconds
    }
}

void* enhance_channel_sniffer(void*){
    int channel_map[80][11] = {0};
    clean_all_channel();
    for(int i=0;i<10;i++){
        for(int j=0;j<total_channel;j++)
            channel_map[i][j] = (int)channels[j].is_using;  //initialize the channel map
        usleep(100000);     //sleep for 0.1 seconds
    }
    while(true){
        if(!time_for_Q4) continue;
        for(int i=0;i<total_channel;i++)
            channel_map[10][i] += (int)channels[i].is_using;
        memcpy(channel_map,&(channel_map[1][0]),sizeof(int)*total_channel*10);

        double using_counter=0;
        for(int i=0;i<sizeof(channel_map);i++)
            using_counter ++;

        for(int i=0;i<total_channel;i++){
            int appeared = 0;
            for(int j=0;j<10;j++)
                appeared += channel_map[j][i];
            if(appeared>round(using_counter/10))
                channels[i].bad_channel = true;
            else
                channels[i].bad_channel = false;
        }
        usleep(100000);     //sleep for 0.1 seconds
    }
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

void create_divices(int device_mounts, int times,int select_function){
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
    case 4:
        result = Q3c_behavior;
        break;
    case 5:
        result = Q3d_behavior;
        break;
    case 6:
        result = Q4_behavior;
        break;
    case 7:
        result = Q5_behavior;
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
        try_a_channel(get_random_channel(total_channel));
    pthread_exit(NULL);
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
        int selection = get_random_channel(total_channel);
        if (channels[selection].bad_channel == true){
            int result = search_nearby_normal_channel(selection);
            if(result == -1)
                try_a_channel(get_random_channel(total_channel));
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
    while(++left && left<=selection){
        if(channels[left].bad_channel == false && channels[left].is_using==false)
            break;
    }
    //find right
    while(--right && right>=selection){
        if(channels[right].bad_channel == false && channels[right].is_using==false)
            break;
    }

    if((selection-left)>(right - selection))
        result = (left==0)?selection:left;
    else
        result = (right==79)?selection:right;
    //printf("%d %d\n", result,selection);
    return result;
}

void* Q3b_behavior(void* times){
    int test_time = *(int*)(times);
    for(int i=0; i<test_time*hopping_rate;i+=2){
        //isomorphic Q3a_behavior
        int selection = get_random_channel(total_channel);
        if (channels[selection].bad_channel == true){
            int result = search_far_normal_channel(selection);
            if(result == -1)
                try_a_channel(get_random_channel(total_channel));
            else
                try_a_channel(result);
        }
        else
            try_a_channel(selection);
    }
}

int get_normal_channel_by_random(){
    vector<int> good_channel;
    int result = -1;
    for(int i=0;i<total_channel;i++)
        if(channels[i].is_using == false && channels[i].bad_channel == false)
            good_channel.push_back(i);

    result = (good_channel.empty())? -1 :good_channel[get_random_channel(good_channel.size())];
    good_channel.clear();
    return result;
}

void* Q3c_behavior(void* times){
    int test_time = *(int*)(times);
    for(int i=0; i<test_time*hopping_rate;i+=2){
        //isomorphic Q3b_behavior
        int selection = get_random_channel(total_channel);
        if (channels[selection].bad_channel == true){
            int result = get_normal_channel_by_random();
            if(result == -1)
                try_a_channel(get_random_channel(total_channel));
            else
                try_a_channel(result);
        }
        else
            try_a_channel(selection);
    }
}

void* Q3d_behavior(void* times){
    int test_time = *(int*)(times);
    for(int i=0; i<test_time*hopping_rate;i+=2){
        //isomorphic Q3c_behavior
        int selection = get_normal_channel_by_random();
        if (selection == -1)
            try_a_channel(get_random_channel(total_channel));
        else
            try_a_channel(selection);
    }
}

void* Q4_behavior(void* times){
    int test_time = *(int*)(times);
    for(int i=0;i<test_time*hopping_rate;i+=2){
        int selection = get_normal_channel_by_random();
        if (selection == -1)
            try_a_channel(get_random_channel(total_channel));
        else
            try_a_channel(selection);
    }
}

int get_random_value(){
    srand(time(NULL)+rand());
    return ((rand()%97)*(rand()%23)+rand()) % 11;
}

void try_it(bool* location,bool* location1,int selection){
    if((location1) == NULL){
        if(location[selection] == true){
            collisions++;
            usleep(backoff);
        }
        else{
            location[selection] = true;
            usleep(backoff);
            location[selection] = false;
        }
    }
    else{
        if(location[selection] == true){
            collisions++;
            usleep(backoff);
        }
        else if(location1[selection]==true){
            collisions ++;
            usleep(backoff);
        }
        else{
            location[selection] = true;
            location1[selection]=true;
            usleep(backoff);
            location[selection] = false;
            location1[selection] = false;
        }
    }
    hop++;
}

void* Q5_behavior(void* times){
    int test_time = *(int*)(times);
    for(int i=0; i<test_time*hopping_rate;i+=2){

    int device_x = get_random_value()*get_random_value()%11;
    int device_y = get_random_value()*get_random_value()%11;
    long double first_center_x = 10-standard_radius/sqrt(2);
    long double first_center_y = standard_radius/sqrt(2);
    long double second_center_x = standard_radius/sqrt(2);
    long double second_center_y = standard_radius/sqrt(2);
    long double third_center_x = standard_radius/sqrt(2);
    long double third_center_y = 10-standard_radius/sqrt(2);
    long double fourth_center_x = 10-standard_radius/sqrt(2);
    long double fourth_center_y = 10-standard_radius/sqrt(2);
    long double radius1 = sqrt((first_center_x-device_x)*(first_center_x-device_x)+(first_center_y-device_y)*(first_center_y-device_y));
    long double radius2 = sqrt((second_center_x-device_x)*(second_center_x-device_x)+(second_center_y-device_y)*(second_center_y-device_y));
    long double radius3 = sqrt((third_center_x-device_x)*(third_center_x-device_x)+(third_center_y-device_y)*(third_center_y-device_y));
    long double radius4 = sqrt((fourth_center_x-device_x)*(fourth_center_x-device_x)+(fourth_center_y-device_y)*(fourth_center_y-device_y));

    if(radius1<standard_radius){
        if(radius2<standard_radius){
            //A1B1C1D1
            int get_channel = get_random_channel(total_channel);
            if(get_channel<20)
                try_it(A1,NULL,get_channel);
            else if(get_channel<40)
                try_it(B1,NULL,get_channel-20);
            else if(get_channel<60)
                try_it(C1,NULL,get_channel-40);
            else
                try_it(D1,NULL,get_channel-60);
        }
        else if(radius4<standard_radius){
            //A1B2C2D1
            int get_channel = get_random_channel(total_channel);
            if(get_channel<20)
                try_it(A1,NULL,get_channel);
            else if(get_channel<40)
                try_it(B2,NULL,get_channel-20);
            else if(get_channel<60)
                try_it(C2,NULL,get_channel-40);
            else
                try_it(D1,NULL,get_channel-60);
        }
        else{
            //A1C1C2D1
            int get_channel = get_random_channel(total_channel-20);
            if(get_channel<20)
                try_it(A1,NULL,get_channel);
            else if(get_channel<40)
                try_it(C1,C2,get_channel-20);
            else
                try_it(D1,NULL,get_channel-40);
        }
    }
    else if(radius2<standard_radius){
        if(radius3<standard_radius){
            //A2B1C1D2
            int get_channel = get_random_channel(total_channel);
            if(get_channel<20)
                try_it(A2,NULL,get_channel);
            else if(get_channel<40)
                try_it(B1,NULL,get_channel-20);
            else if(get_channel<60)
                try_it(C1,NULL,get_channel-40);
            else
                try_it(D2,NULL,get_channel-60);
        }
        else{
            //B1C1D1D2
            int get_channel = get_random_channel(total_channel-20);
            if(get_channel<20)
                try_it(B1,NULL,get_channel);
            else if(get_channel<40)
                try_it(C1,NULL,get_channel-20);
            else
                try_it(D1,D2,get_channel-40);
        }
    }
    else if(radius3<standard_radius){
        if(radius4<standard_radius){
            //A2B2C2D2
            int get_channel = get_random_channel(total_channel);
            if(get_channel<20)
                try_it(A2,NULL,get_channel);
            else if(get_channel<40)
                try_it(B2,NULL,get_channel-20);
            else if(get_channel<60)
                try_it(C2,NULL,get_channel-40);
            else
                try_it(D2,NULL,get_channel-60);
        }
        else{
            //A2B1B2D2
            int get_channel = get_random_channel(total_channel-20);
            if(get_channel<20)
                try_it(A2,NULL,get_channel);
            else if(get_channel<40)
                try_it(B1,B2,get_channel-20);
            else
                try_it(D2,NULL,get_channel-40);
        }
    }
    else if(radius4<standard_radius){
        //A1A2B2C2
        int get_channel = get_random_channel(total_channel-20);
        if(get_channel<20)
            try_it(A1,A2,get_channel);
        else if(get_channel<40)
            try_it(B2,NULL,get_channel-20);
        else
            try_it(C2,NULL,get_channel-40);
    }

    else{
        int get_channel = get_random_channel(20);
        if(device_x>=5 && device_y<=5){
            //First quadrant, listen B
            try_it(B3,NULL,get_channel);
        }
        else if(device_x<5 && device_y<=5){
            //Second quadrant, listen A
            try_it(A3,NULL,get_channel);
        }
        else if(device_x<5 && device_y>5){
            //Third quadrant, listen C
            try_it(C3,NULL,get_channel);
        }
        else{
            //Fourth quadrant, listen D
            try_it(D3,NULL,get_channel);
        }
    }
    }
}
