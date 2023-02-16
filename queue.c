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
    struct list_head *new = malloc(sizeof(struct list_head));
    // check if space allowed
    if (new) {
        INIT_LIST_HEAD(new);
        return new;
    }
    return NULL;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (l) {
        struct list_head *indirect = l->next;
        struct list_head *tmp = NULL;
        while (indirect != l) {
            tmp = indirect;
            indirect = indirect->next;
            q_release_element(list_entry(tmp, element_t, list));
        }

        free(l);
    }
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (head && s) {
        element_t *new_ele = malloc(sizeof(element_t));

        if (!new_ele)
            return false;

        new_ele->value = (char *) malloc(strlen(s) * sizeof(char) + 1);

        if (!new_ele->value) {
            free(new_ele);
            return false;
        }

        strncpy(new_ele->value, s, strlen(s) + 1);

        // struct list_head *new_node = (new_ele -> list);

        list_add(&new_ele->list, head);
        return true;
    }
    return false;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (head && s) {
        element_t *new_ele = malloc(sizeof(element_t));
        if (!new_ele)
            return false;
        struct list_head *prev_list = head->prev;
        new_ele->value = (char *) malloc(sizeof(char) * strlen(s) + 1);
        if (!new_ele->value) {
            free(new_ele);
            return false;
        }
        strncpy(new_ele->value, s, strlen(s) + 1);
        // struct list_head *new_node = (new_ele -> list);

        list_add(&new_ele->list, prev_list);
        return true;
    }
    return false;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head)
        return NULL;
    if (head->next != head) {
        struct list_head *indirect = head->next;
        (indirect->prev)->next = (indirect->next);
        (indirect->next)->prev = (indirect->prev);

        element_t *ele = list_entry(indirect, element_t, list);
        if (sp)
            strncpy(sp, ele->value, bufsize);
        return ele;
    }
    return NULL;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head)
        return NULL;
    if (head->next != head) {
        struct list_head *indirect = head->prev;
        (indirect->prev)->next = (indirect->next);
        (indirect->next)->prev = (indirect->prev);

        element_t *ele = list_entry(indirect, element_t, list);
        if (sp)
            strncpy(sp, ele->value, bufsize);
        return ele;
    }
    return NULL;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (head) {
        struct list_head *indirect = head->next;
        int i = 0;
        while (indirect != head) {
            i++;
            indirect = indirect->next;
        }
        return i;
    }
    return -1;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    struct list_head *left_node = head->next;
    struct list_head *right_node = head->prev;
    while ((left_node->next != right_node) && (left_node != right_node)) {
        left_node = left_node->next;
        right_node = right_node->prev;
    }
    (right_node->next)->prev = right_node->prev;
    (right_node->prev)->next = right_node->next;
    q_release_element(list_entry(right_node, element_t, list));
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    if (head) {
        struct list_head *indirect = (head->next)->next;
        element_t *prev_ele = NULL;
        element_t *current_ele = NULL;
        int cmp = -1, prev_cmp = -1;
        while (indirect != head) {
            prev_ele = list_entry(indirect->prev, element_t, list);
            current_ele = list_entry(indirect, element_t, list);
            cmp = strcmp(prev_ele->value, current_ele->value);
            if (cmp == 0 || prev_cmp == 0) {
                list_del(indirect->prev);
                q_release_element(prev_ele);
            }
            prev_cmp = cmp;
            indirect = indirect->next;
        }
        if (cmp == 0) {
            list_del(indirect->prev);
            q_release_element(current_ele);
        }
        return true;
    }


    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    return false;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    if (head) {
        struct list_head *indirect = head->next;
        while (indirect != head && indirect->next != head) {
            list_move(indirect, indirect->next);
            indirect = indirect->next;
        }
    }
    // https://leetcode.com/problems/swap-nodes-in-pairs/
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (head) {
        struct list_head *indirect = head;
        struct list_head *tmp;
        tmp = indirect->next;
        indirect->next = indirect->prev;
        indirect->prev = tmp;
        indirect = indirect->next;
        while (indirect != head) {
            tmp = indirect->next;
            indirect->next = indirect->prev;
            indirect->prev = tmp;
            indirect = indirect->next;
        }
    }
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    if (head) {
        struct list_head **indirect = &(head->next);
        struct list_head *current = head->next;
        struct list_head *tmp = NULL;

        int i = 1;
        while (current != head) {
            if (i % k == 0) {
                tmp = current;
                while (*indirect != tmp) {
                    list_move(tmp->prev, current);
                    current = current->next;
                }
                indirect = &((current)->next);
            }
            current = current->next;
            i++;
        }
    }
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}

/* Sort elements of queue in ascending order */
void q_sort(struct list_head *head) {}

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
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
