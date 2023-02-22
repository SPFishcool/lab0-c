#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <linux/kernel.h>

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
    if (!new)
        return NULL;

    INIT_LIST_HEAD(new);
    return new;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l)
        return;
    struct list_head *indirect = l->next;
    struct list_head *tmp = NULL;
    while (indirect != l) {
        tmp = indirect;
        indirect = indirect->next;
        q_release_element(list_entry(tmp, element_t, list));
    }
    free(l);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!(head && s))
        return false;

    element_t *new_ele = malloc(sizeof(element_t));
    int len = strlen(s);
    if (!new_ele)
        return false;

    new_ele->value = (char *) malloc(len * sizeof(char) + 1);
    if (!new_ele->value) {
        free(new_ele);
        return false;
    }

    strncpy(new_ele->value, s, len + 1);

    // struct list_head *new_node = (new_ele -> list);

    list_add(&new_ele->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!(head && s))
        return false;

    element_t *new_ele = malloc(sizeof(element_t));
    int len = strlen(s);
    if (!new_ele)
        return false;

    new_ele->value = (char *) malloc(sizeof(char) * len + 1);
    if (!new_ele->value) {
        free(new_ele);
        return false;
    }
    strncpy(new_ele->value, s, len + 1);
    // struct list_head *new_node = (new_ele -> list);

    list_add_tail(&new_ele->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    struct list_head *indirect = head->next;
    list_del(indirect);

    element_t *ele = list_entry(indirect, element_t, list);
    if (sp) {
        strncpy(sp, ele->value, bufsize - 1);
        *(sp + bufsize - 1) = '\0';
    }

    return ele;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    struct list_head *indirect = head->prev;
    list_del(indirect);

    element_t *ele = list_entry(indirect, element_t, list);
    if (sp) {
        strncpy(sp, ele->value, bufsize - 1);
        *(sp + bufsize - 1) = '\0';
    }
    return ele;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return -1;

    struct list_head *indirect = head->next;
    int i = 0;
    while (indirect != head) {
        i++;
        indirect = indirect->next;
    }
    return i;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    while (!head || list_empty(head))
        return false;

    struct list_head *left_node = head->next;
    struct list_head *right_node = head->prev;
    while ((left_node->next != right_node) && (left_node != right_node)) {
        left_node = left_node->next;
        right_node = right_node->prev;
    }
    list_del(right_node);
    q_release_element(list_entry(right_node, element_t, list));
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    if (!head || list_empty(head))
        return false;

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

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    if (!head || list_empty(head))
        return;

    struct list_head *indirect = head->next;
    while (indirect != head && indirect->next != head) {
        list_move(indirect, indirect->next);
        indirect = indirect->next;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head))
        return;
    struct list_head **indirect = &((head->next)->next);
    while (*indirect != head)
        list_move(*indirect, head);
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    if (!head || list_empty(head))
        return;

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

struct list_head *mergeList(struct list_head *l1, struct list_head *l2)
{
    struct list_head *head = NULL;
    struct list_head **current = &head;
    while (l1 && l2) {
        if (strcmp(list_entry(l1, element_t, list)->value,
                   list_entry(l2, element_t, list)->value) >= 0) {
            *current = l2;
            l2 = l2->next;
        } else {
            *current = l1;
            l1 = l1->next;
        }
        current = &((*current)->next);
    }
    *current = (struct list_head *) ((__intptr_t) l1 | (__intptr_t) l2);
    return head;
}

struct list_head *merge_sort(struct list_head *head)
{
    if (!head || head->next == NULL)
        return head;

    struct list_head *slow = head;


    for (struct list_head *fast = head->next; fast && fast->next;
         fast = fast->next->next) {
        slow = slow->next;
    }

    struct list_head *list2 = slow->next;

    slow->next = NULL;
    return mergeList(merge_sort(head), merge_sort(list2));
}

/* Sort elements of queue in ascending order */
void q_sort(struct list_head *head)
{
    if (!head || list_empty(head))
        return;

    struct list_head *start = head->next;
    struct list_head *tail = head->prev;
    tail->next = NULL;

    struct list_head *sorted_list = merge_sort(start);

    head->next = sorted_list;
    struct list_head *cur = head;
    while (cur->next) {
        cur->next->prev = cur;
        cur = cur->next;
    }
    cur->next = head;
    head->prev = cur;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;
    struct list_head *current = head->prev;
    struct list_head *cur_prev = current->prev;
    while (cur_prev != head) {
        if (strcmp(list_entry(current, element_t, list)->value,
                   list_entry(cur_prev, element_t, list)->value) > 0) {
            list_del(cur_prev);
            q_release_element(list_entry(cur_prev, element_t, list));
        } else {
            current = current->prev;
        }
        cur_prev = current->prev;
    }
    int len = q_size(head);
    return len;
}

/* Merge all the queues into one sorted queue, which is in ascending order */
int q_merge(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;
    else if (list_is_singular(head))
        return list_entry(head->next, queue_contex_t, chain)->size;
    queue_contex_t *target = list_entry(head->next, queue_contex_t, chain);
    queue_contex_t *que = NULL;
    list_for_each_entry (que, head, chain) {
        if (que == target)
            continue;
        list_splice_init(que->q, target->q);
        target->size = target->size + que->size;
        que->size = 0;
    }
    q_sort(target->q);
    return target->size;
}
