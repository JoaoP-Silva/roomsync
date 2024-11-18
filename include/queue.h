#ifndef QUEUE_H
#define QUEUE_H

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <unistd.h>

/** struct queue - A simple queue
 * int size - The number of nodes
 * struct node* head - The first element in the queue 
 * struct node* tail - The last element in the queue
 */
struct queue
{
    int size;
    struct node* head;
    struct node* tail;
};

/** struct node - A node in the queue
 * int val - The value of the node
 * struct node* next - A pointer to the next element
 */
struct node
{
    int val;
    struct node* next;
};

/** queue_append - Append an item to the end of the queue
 * val - Value to be appended
 * queue - Pointer to the queue
 */
void queue_append(int val, struct queue* queue);


/** queue_pop - Remove the first element from the queue.
 * struct queue* queue - Pointer to the queue.
 * Return - The value stored in the first element.
 */
int queue_pop(struct queue* queue);


#endif