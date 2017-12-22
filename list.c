#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include "list.h"



void listInsertBeginning(struct ListNode **head, struct Data* value){

    struct ListNode *var,*temp;
    var=(struct ListNode *)malloc(sizeof(struct ListNode));
    var->data =  malloc(sizeof(value));
    var->data = value;
  
    if(*head==NULL){  
      *head = malloc(sizeof(var));
      (*head)->data = malloc(sizeof(value));
      *head=var;
      (*head)->data = var->data;
      (*head)->next=NULL;
    }
    else {
      temp=var;
      temp->next=*head;
      (*head)=temp;
    }
} 

 


int delete_head(struct ListNode **head)
{
      struct ListNode *temp;
      temp=(*head);
      if(temp->next==NULL)
      {
           free(temp);
           (*head)=NULL;
           return 0;
      }
      (*head)=temp->next;
      free(temp);
      return 0;
} 


int is_empty(struct ListNode *head){
  if((head==NULL)||(head->next ==NULL))
    return 1;
  else
    return 0;
}
