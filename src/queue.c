#include "../include/queue.h"

/** queue_append - Append an item to the end of the queue
 * val - Value to be appended
 * queue - Pointer to the queue
 */
void queue_append(int val, struct queue* queue)
{
    // init new node
    struct node* new_node = (struct node*)malloc(sizeof(struct node));
    new_node->next = NULL;
    new_node->val = val;
    
    // update queue
    if( queue->size == 0)
        queue->head = new_node;
    else
        queue->tail->next = new_node;
        
    queue->tail = new_node;
    queue->size += 1;
}

/** queue_pop - Remove the first element from the queue.
 * struct queue* queue - Pointer to the queue.
 * Return - The value stored in the first element.
 */
int queue_pop(struct queue* queue)
{
    if(queue->size <= 0 || queue->head == NULL)
    {
        printf("You cannot pop an empty queue.\n");
        return -1;
    }

    int val = queue->head->val;
    struct node* old_head = queue->head;
    if(old_head == NULL)
        queue->tail = NULL;
    queue->head = old_head->next;
    queue->size -= 1;
    free(old_head);
    return val;    
}