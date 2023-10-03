#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct head{	//the first header, the pointer to the first free block, the size of the entire reserved memory block, and the size of the first free block (after the first allocation, it is no longer the first free block)
	struct nexts *free;
	int size;
	int sizeb;
}Head;

typedef struct nexts{	//header of the concatenated list, points to the next free record in memory
	struct nexts *next;
}Next;

Head *head;

void memory_init(void *ptr, unsigned int size){ 	
	head = ptr;									//I set the initial values based on arguments
	head->size = size;
	head->free = (Next*) head+2;
	head->sizeb = size - sizeof(Head);	
	
	head->free->next=NULL;
}
	
void *memory_alloc(unsigned int size){	
	if (head->free == NULL)
		return NULL;
	
	Next *act,*prev,*pom, *besta=NULL, *bestp=NULL;
	act = head->free;
	prev = (Next*) head;
	
	void *ptr=NULL;
	int size_al,residue, min = head->size;
	
	while (1){								//best fit (best free block from list)
		size_al = abs(*((int*)act-1));
		if (size_al == size){				//if sizes are the same, block is evaluated as the best one
			residue = size_al - size;
			*((int*)act-1) = -size;
			ptr = (void*) act;
				break;
		}
		if (size_al > size)					
			if (min >= size_al){			//if size is smaller than currently smallest, I assign sizes and save actual and previous pointers
				min = size_al;
				besta = act;
				bestp = prev;
				residue = size_al - size;
			}	
		if ((void*) act->next >= (void*) head && (void*)act->next <= (void*) head + head->size ){	//searching for next free pointer
			prev = act;
			act = act->next;
		}
		else 								
			if (besta == NULL)
				return NULL;
			else
				break;				
	}		
	if (besta != NULL && bestp != NULL && ptr==NULL){	
		act = besta;
		prev = bestp;
		residue = abs(*((int*)act-1)) - size;
		*((int*)act-1) = -size;
		ptr = (void*) act;
		
	}
	if (residue >= sizeof(Next)+sizeof(int)){				//setting next pointer from current block
		prev->next = (void *) act + size + sizeof(int);
		pom = act;
		act = prev->next;
		*((int*)act-1) = residue-sizeof(int);
		act->next = pom->next;
	}	
	else 
		if ((void*) act->next >= (void*) head && (void*)act->next <= (void*) head + head->size )
			prev->next = act->next;
		else
			prev->next = NULL;		
	return ptr;
}

int memory_check(void *ptr){		//checking if pointer is not NULL, is free, and points to the assigned block in memory
	if (ptr!= NULL && *((int*)ptr - 1) < 0 && ptr < (void*) head + head->size)
		return 1;
	return 0;	
}

int memory_free(void *valid_ptr){	
	Next *ptr = valid_ptr;

	if (*((int*) valid_ptr - 1) > 0)
		return 1; 
	
	*((int*)ptr - 1) = abs(*((int*)ptr - 1));
	
	int size = abs(*((int*)ptr - 1));
	
	Next *act,*prev;
	prev = (Next*) head;
	act = head->free;
	
	if (head->free != NULL){		//releasing the blocks in the order they are in memory (it helps me when connecting blocks)		
		while (1){
			if ((void*)act + *((int*)act - 1) + sizeof(int) == ptr){	//if free is in front of the released block, I will connect them
				*((int*)act - 1) += 4 + *((int*)ptr - 1);
				
				if ((void*)ptr + size + sizeof(int) == act->next){		//if free is also behinf the released block, I will connect all 3
					*((int*)act - 1) += 4 + *((int*)act->next - 1);
					if (act->next->next != NULL)
						act->next = act->next->next;
					else 
						act->next = NULL;	
				}
				return 0;
			}
			if (ptr < act){				
				if ((void*)ptr + size + sizeof(int) == act){			
					size += 4 + *((int*)act - 1);
					*(((int*) ptr-1)) = size; 
					prev->next = ptr;
					ptr->next = act->next;
				}
				else{													
					prev->next = ptr;
					ptr->next = act;
				}
				return 0;		
			}
			
				
			if ((void*) act->next >= (void*) head && (void*)act->next <= (void*) head + head->size){
				prev = act;
				act = act->next;	
			}	
			else 
				break;
		}
	}
	else{				// if first free is NULL, assigning freed blok
		head->free=ptr;
		return 0;	
	}	
	act->next = ptr;	

	return 0;
}
int main(){
  
  char pole[50];
  memory_init(pole, 50);
  
  char *p1 = (char*) memory_alloc(8);
  char *p2 = (char*) memory_alloc(12);
  memory_free(p1);
  char *p3 = (char*) memory_alloc(8);
  
  printf("%p\n",p1);
  printf("%p\n",p2);
  printf("%p\n",p3);
  
  
  /*char region[100];
  memory_init(region, 100);
  
  
  
  char* a = (char*) memory_alloc(8);
  char* b = (char*) memory_alloc(8);
  char* c = (char*) memory_alloc(10);
  
  char *text = (char*)head;	
	
	int i;
  
  char* d = (char*) memory_alloc(8);
  char* e = (char*) memory_alloc(8);
  memory_free(c);
  printf("uvolnil C\n");
  char* f = (char*) memory_alloc(8);
  char* g = (char*) memory_alloc(8);
  memory_free(e);
  printf("Uvolnil E\n");
  
  printf("head = %p\n",head);
  printf("a = %p\n",a);
  printf("b = %p\n",b);
  printf("c = %p\n",c);
  printf("d = %p\n",d);
  printf("e = %p\n",e);
  printf("f = %p\n",f);
  printf("g = %p\n\n",g);
  memory_free(d);
  printf("Uvolnil D\n");
  memory_free(b);
  printf("Uvolnil B\n");
  memory_free(g);
  printf("Uvolnil G\n");
 
  Next *act = (Next*) head;
  printf("%p\n", act);
  while (1){
		if ((void*) act->next >= (void*) head && (void*)act->next <= (void*) head + head->size +50){
			printf("while cyklus v maine : %p\n",act->next);
			act = act->next;
		}
		else 
			break;	
	}
	printf("po while : %p",act->next);
	
  printf("\n");
  char* h = (char*) memory_alloc(8);
  char* o = (char*) memory_alloc(8);
  char* j = (char*) memory_alloc(8);
  char* l = (char*) memory_alloc(10);
  char* k = (char*) memory_alloc(8);
  char* m = (char*) memory_alloc(8);
  char* n = (char*) memory_alloc(8);

  printf("h = %p\n",h);
  printf("o = %p\n",o);
  printf("j = %p\n",j);
  printf("k = %p\n",k);
  printf("l = %p\n",l);
  printf("m = %p\n",m);
  printf("n = %p\n",n);
	
	memset(a,'a',8);
	memset(l,'l',10);
	memset(o,'o',8);
	memset(j,'j',8);
	memset(h,'h',8);
	memset(k,'k',8);
	memset(f,'f',8);
	
  	text = (char*)head;	

	for (i=0; i<100; i++)
		if (*(text+i) <= 'z' && *(text+i) >= 'a')
			printf("%c\n",*(text+i));
		else 
			printf("%d\n",*(text+i));	
		
 */
  return 0;

}
