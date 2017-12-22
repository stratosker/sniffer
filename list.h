#ifndef LIST_H_
#define LIST_H_

struct ListNode{
	struct Data *data;
    struct ListNode *next;
};


struct Data{
	pid_t pid;
	int fd;
};


void listInsertBeginning(struct ListNode **head, struct Data* value);
int delete_head(struct ListNode **head);
int is_empty(struct ListNode *head);

#endif