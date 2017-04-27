#include <pthread.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <my_global.h>
#include <mysql.h>
#include <unistd.h>
#include <time.h>

#include "control.h"
#include "spectrometer.h"
#include "queue.h"


void LamportQueue_init(struct LamportQueue *queue, enum queue_type type)
{
    atomic_init(&queue->front_, 0);
    atomic_init(&queue->back_, 0);
    queue->cached_front_ = queue->cached_back_ = 0;
    queue->type = type;

    if(type == LASER_TYPE){   // lidar queue
        queue->l_data = (struct laser*)malloc( SIZE_L * sizeof(struct laser*) );
    }
    else if(type == ANGLE_TYPE){            // gimbal queue
        queue->a_data = (struct angle*)malloc( SIZE_A * sizeof(struct angle*) );
    }
    else{ // (type == SPECTRAL_TYPE)
        queue->s_data = (struct spectral*)malloc( SIZE_S * sizeof(struct spectral*) );
    }
}

bool LamportQueue_push(struct LamportQueue *queue, void* elem)
{
    int size;

    if(queue->type == LASER_TYPE){
        size = SIZE_L;
    }
    else if(queue->type == ANGLE_TYPE){
        size = SIZE_A;
    }
    else if(queue->type == SPECTRAL_TYPE){
        size = SIZE_S;
    }

    size_t b, f;
    b = atomic_load_explicit(&queue->back_, memory_order_relaxed);
    f = queue->cached_front_;
    if ((b + 1) % size == f)
    {
        queue->cached_front_ = f = atomic_load_explicit(&queue->front_, memory_order_acquire);
    }
    else{
        // front can only increase since the last time we read it, which means we can only get more space to push into.
       // If we still have space left from the last time we read, we don't have to read again.
      }
    if ((b + 1) % size == f)
    {
        return false;
    }
    else
    { // not full
    }

    if(queue->type == LASER_TYPE){
        queue->l_data[b] = *(struct laser*)elem;
    }
    else if(queue->type == ANGLE_TYPE){ 
        queue->a_data[b] = *(struct angle*)elem;

        //if( queue->a_data[b].time <3 )
        //    printf("error pushed 0\n");
        
    }
    else{ // (queue->type == SPECTRAL_TYPE){
        queue->s_data[b] = *(struct spectral*)elem;
    }

    atomic_store_explicit(&queue->back_, (b + 1) % size, memory_order_release);
    
    return true;
    
}

bool LamportQueue_pop(struct LamportQueue *queue, void* elem)
{

    size_t b, f;
    f = atomic_load_explicit(&queue->front_, memory_order_relaxed);
    b = queue->cached_back_;
    if (b == f)
    {
        queue->cached_back_ = b = atomic_load_explicit(&queue->back_, memory_order_acquire);
    }
    else
    { // back can only increase since the last time we read it, which means we can only get more items to pop from.
     // If we still have items left from the last time we read, we don't have to read again.
     }
    if (b == f)
    {
        return false;
    }
    else
    { // not empty
     }
    
    if(queue->type == LASER_TYPE){

        struct laser* popped = (struct laser*)elem;
        *popped = queue->l_data[f];
       
        atomic_store_explicit(&queue->front_, (f + 1) % SIZE_L, memory_order_release);
    }
    else if(queue->type == ANGLE_TYPE){ // ANGLE_TYPE

        struct angle* popped = (struct angle*)elem;
        *popped = queue->a_data[f];

        //if( queue->a_data[f].time <3 )
        //    printf("error popped 0\n");
        

        atomic_store_explicit(&queue->front_, (f + 1) % SIZE_A, memory_order_release);
    }
    else{ // (queue->type == SPECTRAL_TYPE){

        struct spectral* popped = (struct spectral*)elem;
        *popped = queue->s_data[f];

        atomic_store_explicit(&queue->front_, (f + 1) % SIZE_S, memory_order_release);
    }
    
    return true;
}

