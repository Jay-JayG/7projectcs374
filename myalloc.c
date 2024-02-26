#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>

#define ALIGNMENT 16   // Must be power of 2
#define GET_PAD(x) ((ALIGNMENT - 1) - (((x) - 1) & (ALIGNMENT - 1)))

#define PADDED_SIZE(x) ((x) + GET_PAD(x))

struct block {
    int size;            // Size in bytes
    int in_use;          // Boolean
    struct block *next;
};

struct block *head;
int mmapped = 0;


void *myalloc(int size) {
    if (!mmapped) {
    void *heap = mmap(NULL, 1024, PROT_READ|PROT_WRITE, MAP_ANON|MAP_PRIVATE, -1, 0);
    mmapped = 1;
    struct block *head_node = heap;
    head_node->size = 1024 - PADDED_SIZE(sizeof(struct block));
    head_node->in_use = 0;
    head_node->next = NULL;
    head = heap;
    }
    struct block *temp = head;
    while (1) {
        if (temp->size > PADDED_SIZE(size) && temp->in_use == 0) {
            break;
        }
        temp = temp->next;
        if (temp == NULL) {
            return temp;
        }
    }
    temp->in_use = 1;
    return temp;
}


void print_data(void)
{
    struct block *b = head;

    if (b == NULL) {
        printf("[empty]\n");
        return;
    }

    while (b != NULL) {
        // Uncomment the following line if you want to see the pointer values
        //printf("[%p:%d,%s]", b, b->size, b->in_use? "used": "free");
        printf("[%d,%s]", b->size, b->in_use? "used": "free");
        if (b->next != NULL) {
            printf(" -> ");
        }

        b = b->next;
    }

    printf("\n");
}

void main(void) {
    void *p;

    print_data();
    p = myalloc(16);
    print_data();
    p = myalloc(16);
    printf("%p\n", p);
}