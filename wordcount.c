#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <ctype.h>
#include <semaphore.h>
sem_t sem;

//We are counting words
typedef struct word {
char *text;
int amount;
struct word *next;
} word;
//We have a list of words
typedef struct linked_list_T {
    word *head;
    word *tail;
} linked_list_T;
//MAKE A GLOBAL LIST SO THE THREADS CAN PLAY WITHOUT THIS CASTING
linked_list_T *list;
//making a new word
word *new_word(char *input){
    word *element;
    element = (word *)malloc(sizeof(word));
    if(!element){
        printf("Cannot allocate memory\n");
        return NULL;
    }
    element->text=input;
    element->amount=1;
    return element;
}
//make a new list
linked_list_T *NewLinkedList(void)
{
   linked_list_T *list;
  
   list = (linked_list_T *) malloc( sizeof(linked_list_T) );
   if( !list){
     printf("NewLinkedList cannot allocate memory\n");
     return NULL;
   }
   list->head = NULL;
   list->tail = NULL;
  return list;
}
//Remove first
word *Delist(linked_list_T *list)                                         
{
  word *temp;                                                               
  if(list->head==NULL){                                                               
    printf("The list is empty\n");                                                    
    return NULL;
  }
  else{                                                                               
    temp=list->head;                                                                 
    printf("%s-%d\n",temp->text,temp->amount);
    free(list->head->text);
    free(list->head);
    list->head=temp->next;                                                            
    return temp;                                                                      
  }
}
//Remove it all
void FreeLinkedList(linked_list_T *list){
 word *temp;
 if(list==NULL){                                                        
   printf("There is no list\n");
 }
 else{                                                                  
   printf("Total words:\n");                                          
   while(list->head!=NULL){
    temp = Delist(list);
    //free(temp->text);
    //free(temp);                                                      
   }
 }
 free(list);                                                        
 list=NULL;
}
//useless function
int LinkedListIsEmpty(linked_list_T *list)                                
{
  if(list->head==NULL){                                                    
    return 1;                                                             
    }
  else{
    return 0;
  }
}
//return the struct at index
word *GetLinkedListElement(linked_list_T *list, int index)
{
  word *temp;                                               
  temp=list->head;                                                  
  for(int i=0; i < index; i++){                                      
    temp=temp->next;
  }
  return temp;                                                          
}
//Get the list length
int LinkedListLength(linked_list_T  *list)                            
{
  int i=0;                                                            
  struct word *pointer;                          
  pointer=list->head; 
  while(pointer!=NULL){                                   
    pointer=pointer->next;                            
    i++;                                                  
  }
return i;                                                 
}
//add element
void Enlist(linked_list_T *list, word *element)                         
{ 
  word *temp, *temp2;                                                           
  temp=element;                                                                   
  int place = -1;
  if(list->head==NULL){                                                           
    list->head=temp;                                                              
    list->tail=temp;
    //printf("Adding %s\n\n",element->text);
  }
  else{

  temp2=list->head;
  for(int i=0;i<LinkedListLength(list);i++){
    if(strcmp(element->text,temp2->text)==0){
      place = i;
    }
    temp2=temp2->next;
  }
  if(place!=-1){
    //printf("incrementing\n");
    temp2 = GetLinkedListElement(list,place);
    temp2->amount++;
  }
  else{                                                                           
  list->tail->next=temp;                                                          
  list->tail=temp;
  list->tail->next=NULL;                                                          
  //printf("Adding %s\n",list->tail->text);
  }                                         
  }
}
//count the words in a file and add them to linked list
int processfile(char *filename){
    char buff[20];                                            
    int count = 0;                                            
    FILE *fp = fopen(filename,"r");                           
    if(fp == NULL){                                           
        printf(" Failed to open file.\n");
        return 0;                                               
    }
    while(!feof(fp)){                                         
        fscanf(fp,"%s",buff);
        char *place = malloc(strlen(buff)*sizeof(char));
        strcpy(place,buff);
        Enlist(list,new_word(place));
        count++;
    }
    //list->tail->amount--;
    fclose(fp);
    return count;                                               
}
//Remove an element wherever we want
void Remove_One(linked_list_T *list,int index){
  word *temp;
  word *remove;
  if(index==0){Delist(list);}
  else{
  temp = GetLinkedListElement(list,index-1);
  remove=temp->next;
  temp->next=remove->next;
  free(remove->text);
  free(remove);
  }
}
//Same as processfile but the thread version
void *processbythread(void *args){
    char *filename = (char *)args;
    char buff[20];                                            
    int count = 0;
    FILE *fp = fopen(filename,"r");                           
    if(fp == NULL){                                           
        printf(" Failed to open file.\n");
        return 0;                                               
    }
    while(!feof(fp)){                                         
        fscanf(fp,"%s",buff);
        char *place = malloc(strlen(buff)*sizeof(char));
        strcpy(place,buff);
        sem_wait(&sem);
        Enlist(list,new_word(place));
        sem_post(&sem);
        count++;
    }
    //list->tail->amount--;
    fclose(fp);
    printf("Wordcount of %s = %d\n",filename,count);                                               
}

int main(int argc, char* argv[]){  
    int num = argc-1;
    pthread_t threads[num];
    list = NewLinkedList();
    sem_init(&sem,0,1);
    for(int i=0;i<num;i++){
      pthread_create(&threads[i],NULL,processbythread,(void *)argv[i+1]);
    }
    for(int i=0;i<num;i++){
      pthread_join(threads[i],NULL);
    }
    FreeLinkedList(list);
    
return 0;
}