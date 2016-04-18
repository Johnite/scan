/*************************************************************************
    > File Name: hebingList.c
    > Author: reus
    > Mail: dmarcoreus@gmail.com 
    > Created Time: 2016年04月04日 星期一 20时22分47秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>


typedef struct node
{
	int value;
	struct node* next;
}Node;

Node* init_list(int n)
{
	int i = 0;
	Node* list = (Node*)calloc(1,sizeof(Node));
	list->next = NULL;
	Node* head = list;
	list->value = 0;


    while(i < n)
	{
		Node* temp = (Node*)calloc(1,sizeof(Node));
		temp->value = i;
		temp->next = NULL;
		head->next = temp;
		head = temp;
		i++;
	}
	return list;
}

void createADetail(Node* a,Node* b,Node* head)
{
	if(a == NULL && b == NULL)
		return;
	else if(a == NULL)
	{
		head->next = b;
		return;
	}
	else if(b == NULL)
	{
		head->next = a;
		return;
	}

	if(a->value < b->value)
	{
		Node* p = a->next;
		head->next = a;
		head = a;
		createADetail(p,b,head);
	}
	else
	{
		Node* p = b->next;
		head->next = b;
		head = b;
		createADetail(a,p,head);
	}

}

Node* hebing(Node* a,Node* b)
{
	if(a==NULL||b==NULL)
		return NULL;
	else if(a == NULL)
		return b;
	else if(b == NULL)
		return a;

	Node* T = (Node*)calloc(1,sizeof(Node));
	T->value = 0;
	T->next = NULL;
	Node* head = T;

    createADetail(a,b,head);

	return T->next;
}

int main(void)
{
	Node* list1 = init_list(10);
	Node* list2 = init_list(12);

	Node* h = list1;
	Node* p = list2;

	printf("list1的值:\n");
	while(h!=NULL)
	{
		printf("%d\t",h->value);
		h = h->next;
	}
	printf("-----------\n");
	printf("list2的值:\n");
	while(p != NULL)
	{
		printf("%d\t",p->value);
		p = p->next;
	}

	printf("-----------\n");
	//Node* h = (Node*)calloc(1,sizeof(Node));
	Node* temp = hebing(list1,list2);

	printf("hebingzhihou-\n");
	Node* head = temp;
	while(head!=NULL)
	{
		printf("%d\t",head->value);
		head = head->next;
	}
	printf("\n============\n");
	return 0;

}

