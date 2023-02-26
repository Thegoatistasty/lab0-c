#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head = (struct list_head *) malloc(sizeof(*head));
    if (!head)
        return NULL;

    INIT_LIST_HEAD(head);
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l)
        return;
    element_t *save, *temp;

    list_for_each_entry_safe (temp, save, l, list)
        q_release_element(temp);
    free(l);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *new = (element_t *) malloc(sizeof(element_t));
    if (!new)
        return false;
    short length = strlen(s);
    new->value = (char *) malloc(length * sizeof(char) + 1);  //+1 for '/0'
    if (!new->value) {
        free(new);
        return false;
    }
    strncpy(new->value, s, length);
    new->value[length] = '\0';
    list_add(&new->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *new = (element_t *) malloc(sizeof(element_t));
    if (!new)
        return false;
    short length = strlen(s);
    new->value = (char *) malloc(length * sizeof(char) + 1);  //+1 for '/0'
    if (!new->value) {
        free(new);
        return false;
    }
    strncpy(new->value, s, length);
    new->value[length] = '\0';
    list_add_tail(&new->list, head);

    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || !sp || list_empty(head))
        return NULL;


    element_t *first_entry = list_first_entry(head, element_t, list);
    strncpy(sp, first_entry->value, bufsize - 1);
    sp[bufsize - 1] = '\0';
    list_del(head->next);
    return first_entry;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head)
        return NULL;
    if (!sp)
        return NULL;

    element_t *last_entry = list_last_entry(head, element_t, list);
    strncpy(sp, last_entry->value, bufsize - 1);
    sp[bufsize - 1] = '\0';
    list_del(head->prev);
    return last_entry;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int len = 0;
    struct list_head *li;

    list_for_each (li, head)
        len++;
    return len;
}
/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    // Hare & Tortoise Algorithm
    /*if(list_empty(head))
        return false;*/
    struct list_head *fast, *slow;
    fast = slow = head->next;
    do {
        fast = fast->next;
        if (fast == head)
            break;
        slow = slow->next;
        fast = fast->next;
    } while (fast != head);
    list_del(slow);
    q_release_element(list_entry(slow, element_t, list));
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    for (struct list_head *node = head->next;
         node->next != head && node != head; node = node->next) {
        list_del(node);
        list_add(node, node->next);
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_is_singular(head))
        return;

    struct list_head *node, *safe;
    list_for_each_safe (node, safe, head)
        list_move(node, head);
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}
struct list_head *merge_two_lists(struct list_head *list1,
                                  struct list_head *list2)
{
    struct list_head *temp = NULL;
    struct list_head **indirect = &temp;
    struct list_head *last = NULL;
    while (list1 && list2) {
        element_t *v1 = list_entry(list1, element_t, list);
        element_t *v2 = list_entry(list2, element_t, list);
        if (strcmp(v1->value, v2->value) <= 0) {
            list1->prev = *indirect;
            *indirect = list1;
            list1 = list1->next;
        } else {
            list2->prev = *indirect;
            *indirect = list2;
            list2 = list2->next;
        }
        last = *indirect;
        indirect = &((*indirect)->next);
    }
    if (!list1) {
        last->next = list2;
        list2->prev = last;
    }
    if (!list2) {
        last->next = list1;
        list1->prev = last;
    }
    return temp;
}
struct list_head *merge_sort(struct list_head *head)
{
    if (!head || !head->next)
        return head;

    struct list_head *fast, *slow;
    fast = slow = head;
    while (fast && fast->next) {
        fast = fast->next->next;
        slow = slow->next;
    }
    slow->prev->next = NULL;
    slow->prev = NULL;
    struct list_head *list1 = merge_sort(head);
    struct list_head *list2 = merge_sort(slow);

    return merge_two_lists(list1, list2);
}
/* Sort elements of queue in ascending order */
void q_sort(struct list_head *head)
{
    if (list_empty(head))
        return;
    // break the list circle
    head->prev->next = NULL;
    head->prev = NULL;
    // merge sort
    head->next = merge_sort(head->next);
    // reconnect
    head->next->prev = head;
    struct list_head *t = head;
    while (t->next != NULL) {
        t->next->prev = t;
        t = t->next;
    }

    head->prev = t;
    t->next = head;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Merge all the queues into one sorted queue, which is in ascending order */
int q_merge(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;

    queue_contex_t *headchain = list_entry(head->next, queue_contex_t, chain);
    if (list_is_singular(headchain->q))
        return headchain->size;

    for (struct list_head *h = head->next->next; h != head; h = h->next) {
        queue_contex_t *qctx = list_entry(h, queue_contex_t, chain);
        headchain->size += qctx->size;
        list_splice_tail_init(qctx->q, headchain->q);
    }
    q_sort(headchain->q);
    return headchain->size;
    // https://leetcode.com/problems/merge-k-sorted-lists/
}
