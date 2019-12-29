#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "list.h"
#include "limits.h"
#include "hash.h"
#include "debug.h"
#include "bitmap.h"

#define swap_e(a,b){struct list_elem *tmp=a;a=b;b=tmp;}
#define Hasing(a){int tmp=a; a=((tmp*tmp)+tmp)%10;}

struct list_item
{
	struct list_elem elem;
	int data;
};
struct hash_item
{
	struct hash_elem elem;
	int data;
};

struct list * list_arr[10];
struct bitmap * bitmap_arr[10];
struct hash * hash_arr[10];

bool create(char * which,char * what,char * val);
bool dumpData(char *Dump);
bool compare(const struct list_elem *a , const struct list_elem * b, void *aux)
{
	struct list_item *a_item = list_entry(a,struct list_item,elem);
	struct list_item *b_item = list_entry(b,struct list_item,elem);
	if(aux==NULL){
		if((a_item->data)<(b_item->data)) return true;
		else return false;
	}
	else return false;
}
//typedef unsigned hash_hash_func(const struct hash_elem *e , void *aux);
unsigned hash_hash_function(const struct hash_elem *e, void *aux)
{
	return hash_int(hash_entry(e,struct hash_item,elem)->data);
}
//bool hash_less(const struct hash_elem * a, const struct hash_elem *b,void *aux);
bool hash_less(const struct hash_elem * a, const struct hash_elem * b ,void *aux)
{
	return (hash_entry(a,struct hash_item,elem)->data)<(hash_entry(b,struct hash_item,elem)->data);
}
//typedef void hash_action_func(struct hash_elem *e,void *aux);
void hash_action_triple(struct hash_elem *e, void *aux)
{
	struct hash_item * item= hash_entry(e,struct hash_item,elem);
	item->data=(item->data)*(item->data)*(item->data);
}
void hash_action_square(struct hash_elem * e,void *aux)
{
	struct hash_item * item= hash_entry(e,struct hash_item,elem);
	item->data=(item->data)*(item->data);
}
void hash_action_destroy(struct hash_elem * e,void *aux)
{
	struct hash_item * item=hash_entry(e,struct hash_item,elem);
	free(item);	
}
bool delete(char * what);
void list_swap(struct list_elem *a , struct list_elem *b);
struct bitmap * bitmap_expand(struct bitmap * bitmap, int size);
int main(void)
{
	int i=0,j=0,str_count=0,value,cnt=0,target=0,tmp;
	char str[100];
	char strarr[7][25];
	char * token=NULL;
	char valuestr[5];
	struct list_elem *e,*before,*first,*last;
	struct list_item *item;
	struct list * node;
	size_t bit_idx,map_idx;
	bool flag;
	struct hash_item * Hitem;
	struct hash_elem * elem;
	for(int i=0;i<10;i++){
		list_arr[i]=NULL;
		bitmap_arr[i]=NULL;
	}
	memset(str,'\0',sizeof(char)*100);
	for(fgets(str,sizeof(char)*100,stdin);strcmp(str,"quit\n");fgets(str,sizeof(char)*100,stdin)){
		str[strlen(str)-1]='\0';
		str_count=cnt=target=tmp=0;
		memset(valuestr,'\0',sizeof(char)*5);
		for(i=0;i<7;i++) memset(strarr[i],'\0',sizeof(char)*25);
		e=before=first=last=NULL;
		item=NULL;
		token = strtok(str, " ");   // There are two delimiters here
		while (token != NULL){
			strcpy(strarr[str_count],token);
			str_count++;
			token = strtok(NULL, " ");
		}
		//printf("1:%s 2:%s 3:%s 4:%s\n",strarr[0],strarr[1],strarr[2],strarr[3]);
		if(!strcmp(strarr[0],"create")){
			if(!create(strarr[1],strarr[2],strarr[3])) // call create function
				printf("create error\n");
		}
		else if(!strcmp(strarr[0],"dumpdata")){
			dumpData(strarr[1]); // call dumpdata
				//printf("Dump error\n");
		}
		else if(!strcmp(strarr[0],"list_push_front")){
			valuestr[0]=strarr[1][strlen(strarr[1])-1];
			value=atoi(valuestr);
			item=(struct list_item *)malloc(sizeof(struct list_item));
			item->data=atoi(strarr[2]);
			list_push_front(list_arr[value],&(item->elem));//return_front : list-> head.next
		}
		else if(!strcmp(strarr[0],"list_push_back")){
			valuestr[0]=strarr[1][strlen(strarr[1])-1];
			value=atoi(valuestr);
			item=(struct list_item *)malloc(sizeof(struct list_item));
			item->data=atoi(strarr[2]);
			list_push_back(list_arr[value],&(item->elem));
		}
		else if(!strcmp(strarr[0],"list_max")){		// print list_max
			valuestr[0]=strarr[1][strlen(strarr[1])-1];
			value=atoi(valuestr);
			e=list_max(list_arr[value],compare,NULL);
			printf("%d\n",list_entry(e,struct list_item,elem)->data);
		}
		else if(!strcmp(strarr[0],"list_min")){  // print list_min
			valuestr[0]=strarr[1][strlen(strarr[1])-1];
			value=atoi(valuestr);
			e=list_min(list_arr[value],compare,NULL);
			printf("%d\n",list_entry(e,struct list_item,elem)->data);
		}
		else if(!strcmp(strarr[0],"list_pop_front")){ // print list pop front
			valuestr[0]=strarr[1][strlen(strarr[1])-1];
			value=atoi(valuestr);
			e=list_pop_front(list_arr[value]);
			item=list_entry(e,struct list_item,elem);
		}
		else if(!strcmp(strarr[0],"list_pop_back")){  // print list pop back
			valuestr[0]=strarr[1][strlen(strarr[1])-1];
			value=atoi(valuestr);
			e=list_pop_back(list_arr[value]);
			item=list_entry(e,struct list_item,elem);
		}
		else if(!strcmp(strarr[0],"delete")){ // delete function
			if(!delete(strarr[1])){
				printf("delete error\n");
			}
		}
		else if(!strcmp(strarr[0],"list_front")){  
			valuestr[0]=strarr[1][strlen(strarr[1])-1];
			value=atoi(valuestr);
			e=list_front(list_arr[value]); //struct list_elem * list_front(struct list * list)
			item=list_entry(e,struct list_item,elem);
			printf("%d\n",item->data);
		}
		else if(!strcmp(strarr[0],"list_back")){
			valuestr[0]=strarr[1][strlen(strarr[1])-1];
			value=atoi(valuestr);
			e=list_back(list_arr[value]);//struct list_elem * list_back(struct list * list)
			item=list_entry(e,struct list_item,elem);
			printf("%d\n",item->data);
		}
		else if(!strcmp(strarr[0],"list_insert")){   // insert list
			//make node
			valuestr[0]=strarr[1][strlen(strarr[1])-1];
			value=atoi(valuestr);
			item=(struct list_item*)malloc(sizeof(struct list_item));
			item->data=atoi(strarr[3]);
			target=atoi(strarr[2]);
			for(e=list_begin(list_arr[value]),cnt=0;e!=list_end(list_arr[value])&&cnt<target;e=list_next(e)) 
				cnt++;
			list_insert(e,&(item->elem));
		}
		else if(!strcmp(strarr[0],"list_insert_ordered")){ // insert list by ordered
			valuestr[0]=strarr[1][strlen(strarr[1])-1];
			value=atoi(valuestr);
			item=(struct list_item*)malloc(sizeof(struct list_item));
			item->data=atoi(strarr[2]);
			list_insert_ordered(list_arr[value],&(item->elem),compare,NULL);
		}
		else if(!strcmp(strarr[0],"list_empty")){ // list empty
			valuestr[0]=strarr[1][strlen(strarr[1])-1];
			value=atoi(valuestr);
			if(list_empty(list_arr[value])) printf("true\n");
			else printf("false\n");		
		}
		else if(!strcmp(strarr[0],"list_size")){ // return list_size
			valuestr[0]=strarr[1][strlen(strarr[1])-1];
			value=atoi(valuestr);
			printf("%zu\n",list_size(list_arr[value])); // size_t list_size(struct list* list)
		}
		else if(!strcmp(strarr[0],"list_remove")){ // list remove
			valuestr[0]=strarr[1][strlen(strarr[1])-1];
			value=atoi(valuestr);
			target=atoi(strarr[2]);
			for(e=list_begin(list_arr[value]),cnt=0;e!=list_end(list_arr[value])&&cnt<target;e=list_next(e)) 
				cnt++;
			list_remove(e);
		}
		else if(!strcmp(strarr[0],"list_reverse")){  // sort list_reverse
			valuestr[0]=strarr[1][strlen(strarr[1])-1];
			value=atoi(valuestr);
			list_reverse(list_arr[value]); // void list_reverse(struct list * list)
		}
		else if(!strcmp(strarr[0],"list_sort")){
			valuestr[0]=strarr[1][strlen(strarr[1])-1];
			value=atoi(valuestr);
			list_sort(list_arr[value],compare,NULL);//void list_sort(struct list * list,list_less_function * less
													//void * aux)
		}
		else if(!strcmp(strarr[0],"list_splice")){		// make list_splice
			valuestr[0]=strarr[1][strlen(strarr[1])-1];
			value=atoi(valuestr);
			target=atoi(strarr[2]);	
			for(e=list_begin(list_arr[value]),cnt=0;e!=list_end(list_arr[value])&&cnt<target;e=list_next(e))
				cnt++;
			before=e;
			valuestr[0]=strarr[3][strlen(strarr[3])-1];
			value=atoi(valuestr);
			target=atoi(strarr[4]);
			for(e=list_begin(list_arr[value]),cnt=0;e!=list_end(list_arr[value])&&cnt<target;e=list_next(e)) 
				cnt++;
			first=e;
			target=atoi(strarr[5]);
			for(e=list_begin(list_arr[value]),cnt=0;e!=list_end(list_arr[value])&&cnt<target;e=list_next(e)) 
				cnt++;
			last=e;
			list_splice(before,first,last);
		}
		else if(!strcmp(strarr[0],"list_swap")){//make list swap
			valuestr[0]=strarr[1][strlen(strarr[1])-1];
			value=atoi(valuestr);
			target=atoi(strarr[2]);
			for(e=list_begin(list_arr[value]),cnt=0;e!=list_end(list_arr[value])&&cnt<target;e=list_next(e))
				cnt++;
			before=e;
			target=atoi(strarr[3]);
			for(e=list_begin(list_arr[value]),cnt=0;e!=list_end(list_arr[value])&&cnt<target;e=list_next(e))
				cnt++;
			last=e;
			list_swap(before,e);//list swap function
		}
		else if(!strcmp(strarr[0],"list_unique")){
			valuestr[0]=strarr[1][strlen(strarr[1])-1];
			value=atoi(valuestr);
			if(strlen(strarr[2])==0)
				 list_unique(list_arr[value],NULL,compare,NULL);
			else{
				valuestr[0]=strarr[2][strlen(strarr[2])-1];	
				target=atoi(valuestr);
				if(list_arr[target]==NULL)
					create("list",strarr[2],NULL);
				list_unique(list_arr[value],list_arr[target],compare,NULL);
			}	
		}
		else if(!strcmp(strarr[0],"bitmap_mark")){  // bitmap mark
			valuestr[0]=strarr[1][strlen(strarr[1])-1];	
			value=atoi(valuestr);
			bit_idx=atoi(strarr[2]);
			bitmap_mark(bitmap_arr[value],bit_idx);
		}
		else if(!strcmp(strarr[0],"bitmap_all")){  // bitmap _all
			valuestr[0]=strarr[1][strlen(strarr[1])-1];	
			value=atoi(valuestr);
			bit_idx=atoi(strarr[2]);
			map_idx=atoi(strarr[3]);
			if(bitmap_all(bitmap_arr[value],bit_idx,map_idx)) printf("true\n");
			else printf("false\n");
		}
		else if(!strcmp(strarr[0],"bitmap_any")){ // bitmap any
			valuestr[0]=strarr[1][strlen(strarr[1])-1];	
			value=atoi(valuestr);
			bit_idx=atoi(strarr[2]);
			map_idx=atoi(strarr[3]);
			if(bitmap_any(bitmap_arr[value],bit_idx,map_idx)) printf("true\n");
			else printf("false\n");
		}
		else if(!strcmp(strarr[0],"bitmap_contains")){ // bitmap _contain
			valuestr[0]=strarr[1][strlen(strarr[1])-1];	
			value=atoi(valuestr);
			bit_idx=atoi(strarr[2]);
			map_idx=atoi(strarr[3]);
			if(!strcmp(strarr[4],"true")) flag=true;
			else flag=false;
			if(bitmap_contains(bitmap_arr[value],bit_idx,map_idx,flag)) printf("true\n");
			else printf("false\n");
		}
		else if(!strcmp(strarr[0],"bitmap_dump")){ // print bitmap
			valuestr[0]=strarr[1][strlen(strarr[1])-1];	
			value=atoi(valuestr);
			bitmap_dump(bitmap_arr[value]);
		}
		else if(!strcmp(strarr[0],"bitmap_expand")){// bitmap expand
			valuestr[0]=strarr[1][strlen(strarr[1])-1];	
			value=atoi(valuestr);
			bit_idx=atoi(strarr[2]);
			bitmap_arr[value]=bitmap_expand(bitmap_arr[value],bit_idx);
		}
		else if(!strcmp(strarr[0],"bitmap_set_all")){  // bitmap set
			valuestr[0]=strarr[1][strlen(strarr[1])-1];	
			value=atoi(valuestr);		
			if(!strcmp(strarr[2],"true")) flag=true;
			else flag=false;
			bitmap_set_all(bitmap_arr[value],flag);
		}
		else if(!strcmp(strarr[0],"bitmap_flip")){ // bitmap flip
			valuestr[0]=strarr[1][strlen(strarr[1])-1];	
			value=atoi(valuestr);
			bit_idx=atoi(strarr[2]);
			bitmap_flip(bitmap_arr[value],bit_idx);		
		}
		else if(!strcmp(strarr[0],"bitmap_none")){
			valuestr[0]=strarr[1][strlen(strarr[1])-1];	
			value=atoi(valuestr);
			bit_idx=atoi(strarr[2]);
			map_idx=atoi(strarr[3]);
			if(bitmap_none(bitmap_arr[value],bit_idx,map_idx)) printf("true\n");
			else printf("false\n");
		}
		else if(!strcmp(strarr[0],"bitmap_reset")){  // make bitmap reset
			valuestr[0]=strarr[1][strlen(strarr[1])-1];	
			value=atoi(valuestr);
			bit_idx=atoi(strarr[2]);
			bitmap_reset(bitmap_arr[value],bit_idx);
		}
		else if(!strcmp(strarr[0],"bitmap_scan_and_flip")){
			valuestr[0]=strarr[1][strlen(strarr[1])-1];	
			value=atoi(valuestr);
			bit_idx=atoi(strarr[2]);
			map_idx=atoi(strarr[3]);
			if(!strcmp(strarr[4],"true")) flag=true;
			else flag=false;
			printf("%u\n",(unsigned int)bitmap_scan_and_flip(bitmap_arr[value],bit_idx,map_idx,flag));//여기 프린트 할때 이상함
		}
		else if(!strcmp(strarr[0],"bitmap_scan")){  // scan bitmap
			valuestr[0]=strarr[1][strlen(strarr[1])-1];	
			value=atoi(valuestr);
			bit_idx=atoi(strarr[2]);
			map_idx=atoi(strarr[3]);
			if(!strcmp(strarr[4],"true")) flag=true;
			else flag=false;
			printf("%u\n",(unsigned int)bitmap_scan(bitmap_arr[value],bit_idx,map_idx,flag));//여기 프린트 할때 이상함
		}
		else if(!strcmp(strarr[0],"bitmap_set_all")){   //bitmap set
			valuestr[0]=strarr[1][strlen(strarr[1])-1];	
			value=atoi(valuestr);
			if(!strcmp(strarr[2],"true")) flag=true;
			else flag=false;
			bitmap_set_all(bitmap_arr[value],flag);
		}
		else if(!strcmp(strarr[0],"bitmap_set")){  // bitmap set
			valuestr[0]=strarr[1][strlen(strarr[1])-1];	
			value=atoi(valuestr);
			bit_idx=atoi(strarr[2]);
			if(!strcmp(strarr[3],"true")) flag=true;
			else flag=false;
			bitmap_set(bitmap_arr[value],bit_idx,flag);
		}
		else if(!strcmp(strarr[0],"bitmap_count")){ //print bitmap_count
			valuestr[0]=strarr[1][strlen(strarr[1])-1];	
			value=atoi(valuestr);
			bit_idx=atoi(strarr[2]);
			map_idx=atoi(strarr[3]);
			if(!strcmp(strarr[4],"true")) flag=true;
			else flag=false;
			printf("%zu\n",bitmap_count(bitmap_arr[value],bit_idx,map_idx,flag));
		}
		else if(!strcmp(strarr[0],"bitmap_set_multiple")){ //bitmap set
			valuestr[0]=strarr[1][strlen(strarr[1])-1];	
			value=atoi(valuestr);
			bit_idx=atoi(strarr[2]);
			map_idx=atoi(strarr[3]);
			if(!strcmp(strarr[4],"true")) flag=true;
			else flag=false;
			bitmap_set_multiple(bitmap_arr[value],bit_idx,map_idx,flag);
		}
		else if(!strcmp(strarr[0],"bitmap_size")){ //print bitmap_size
			valuestr[0]=strarr[1][strlen(strarr[1])-1];
			value=atoi(valuestr);
			printf("%zu\n",bitmap_size(bitmap_arr[value]));
		}
		else if(!strcmp(strarr[0],"bitmap_test")){ // bitmap_test
			valuestr[0]=strarr[1][strlen(strarr[1])-1];
			value=atoi(valuestr);
			bit_idx=atoi(strarr[2]);
			if(bitmap_test(bitmap_arr[value],bit_idx)) printf("true\n");
			else printf("false\n");
		}
		else if(!strcmp(strarr[0],"hash_insert")){  //btimap insert
			valuestr[0]=strarr[1][strlen(strarr[1])-1];
			value=atoi(valuestr);
			Hitem=(struct hash_item *)malloc(sizeof(struct hash_item));
			Hitem->data=atoi(strarr[2]);
			hash_insert(hash_arr[value],&(Hitem->elem));
		}
		else if(!strcmp(strarr[0],"hash_apply")){  //hash_appy  with function pointer
			valuestr[0]=strarr[1][strlen(strarr[1])-1];
			value=atoi(valuestr);
			if(!strcmp(strarr[2],"triple")) hash_apply(hash_arr[value],hash_action_triple);
			else if(!strcmp(strarr[2],"square")) hash_apply(hash_arr[value],hash_action_square);
			else printf("hash_apply error\n");
		}
		else if(!strcmp(strarr[0],"hash_delete")){  //hash_delete
			valuestr[0]=strarr[1][strlen(strarr[1])-1];
			value=atoi(valuestr);
			Hitem=(struct hash_item*)malloc(sizeof(struct hash_item));
			Hitem->data=atoi(strarr[2]);
			hash_delete(hash_arr[value],&(Hitem->elem));
		}
		else if(!strcmp(strarr[0],"hash_empty")){  // hash_empty
			valuestr[0]=strarr[1][strlen(strarr[1])-1];
			value=atoi(valuestr);
			if(hash_empty(hash_arr[value])) printf("true\n");
			else printf("false\n");
		}
		else if(!strcmp(strarr[0],"hash_size")){ // print hash_size
			valuestr[0]=strarr[1][strlen(strarr[1])-1];
			value=atoi(valuestr);
			printf("%zu\n",hash_size(hash_arr[value]));
		}
		else if(!strcmp(strarr[0],"hash_clear")){ /// hash reset _ clear
			valuestr[0]=strarr[1][strlen(strarr[1])-1];
			value=atoi(valuestr);
			hash_clear(hash_arr[value],hash_action_destroy);
		}
		else if(!strcmp(strarr[0],"hash_find")){ // hash find
			valuestr[0]=strarr[1][strlen(strarr[1])-1];
			value=atoi(valuestr);
			Hitem=(struct hash_item *)malloc(sizeof(struct hash_item));
			Hitem->data=atoi(strarr[2]);
			if(hash_find(hash_arr[value],&(Hitem->elem))) printf("%d\n",Hitem->data);
		}
		else if(!strcmp(strarr[0],"hash_replace")){ //hash replace
			valuestr[0]=strarr[1][strlen(strarr[1])-1];
			value=atoi(valuestr);
			Hitem=(struct hash_item *)malloc(sizeof(struct hash_item));
			Hitem->data=atoi(strarr[2]);
			hash_replace(hash_arr[value],&(Hitem->elem));
		}
	}
}

bool create(char * which,char * what,char * val){
	int number;
	size_t size;
	struct list * node;
	struct bitmap * bit;
	if(!strcmp(which,"list")){// create list
		number=what[4]-'0';
		//printf("fgfg");
		if(number>=10){
			printf("Out of range list");
			return false;
		}
		node=(struct list*)malloc(sizeof(struct list));
		list_arr[number]=node;	
		list_init(list_arr[number]);//initialize list;
		return true;
	}
	else if(!strcmp(which,"bitmap")){//create bitmap;
		number=what[2]-'0';
		if(number>=10){
			printf("Out of range bitmap");
			return false;
		}
		size=atoi(val);
		bitmap_arr[number]=(struct bitmap*)bitmap_create(size);//create bitmap
		if(bitmap_arr[number]==NULL) return false;
		else return true;
	}
	else if(!strcmp(which,"hashtable")){
		number=what[4]-'0';
		if(number>=10){
			printf("Out of range hashtable");
			return false;
		}
		hash_arr[number]=(struct hash *)malloc(sizeof(struct hash));
		hash_init(hash_arr[number],hash_hash_function,hash_less,NULL);
	}
	
}
bool dumpData(char * Dump){
	struct list_elem * e;
	struct hash_iterator iterator;
	size_t size,i;
	int idx=Dump[strlen(Dump)-1]-'0';
	Dump[strlen(Dump)-1]='\0';
	if(!strcmp(Dump,"list")){
		if(list_empty(list_arr[idx])) 
			return false;
		//dump list
		//printf("dump data\n");
		for(e=list_begin(list_arr[idx]);e!=list_end(list_arr[idx]);e=list_next(e))
			printf("%d ",list_entry(e,struct list_item,elem)->data);
		printf("\n");	
		return true;
	}
	else if(!strcmp(Dump,"bm")){
		size=bitmap_size(bitmap_arr[idx]);//return bitmap size
		for(i=0;i<size;i++) printf("%d",bitmap_test(bitmap_arr[idx],i));//return bitmap's idx value
		printf("\n");
		return true;
	}
	else if(!strcmp(Dump,"hash")){
		if(hash_empty(hash_arr[idx])) return false;
		for(hash_first(&iterator,hash_arr[idx]);hash_next(&iterator);)
			printf("%d ",hash_entry(hash_cur(&iterator),struct hash_item,elem)->data);
		printf("\n");
		return true;
	}
}
bool delete(char * what){
	int value=what[strlen(what)-1]-'0';
	struct list_elem *e;
	if(!(0<=value&&value<10)) return false;
	what[strlen(what)-1]='\0';
	if(!strcmp(what,"list")){//delete list
		for(e=NULL;!list_empty(list_arr[value]);)
			e=list_pop_front(list_arr[value]);
		return true;
	}
	else if(!strcmp(what,"bm")){
		bitmap_destroy(bitmap_arr[value]);
		return true;
	}
	else if(!strcmp(what,"hash")){
		hash_destroy(hash_arr[value],hash_action_destroy);
		return true;
	}
	return false;
}
/*
List
1)void list_swap(struct list_elem *a, struct list_elem *b)
	- Parameter : Two list elements that will be swapped
	- Return value : None
	- Functionality : swap two list elements in parameters
*/
void list_swap(struct list_elem *a, struct list_elem * b)
{
	a->prev->next=b;
	a->next->prev=b;
	b->prev->next=a;
	b->next->prev=a;
	swap_e(a->prev,b->prev);
	swap_e(a->next,b->next);
}
/*
void list_shuffle(struct list * list)
	- Parameter : List that will be shuffled
	- Return value : Nonr
	- Functionality : Shuffle elements of LIST in the parameter
*/
void list_shuffle(struct list * list)
{
	struct list_elem * e1,*e2;
	int i,target=0,cnt,number;
	int size=list_size(list);// list_size;
	srand(time(NULL));
	number=rand()%size;
	number=rand()%number;
	for(i=0;i<number;i++){
		target= rand() % size;
		for(e1=list_begin(list),cnt=0;e1!=list_end(list)&&cnt<target;e1=list_next(e1))
			cnt++;
		target=rand()%size;
		for(e2=list_begin(list),cnt=0;e2!=list_end(list)&&cnt<target;e2=list_next(e2))
			cnt++;
		list_swap(e1,e2);
	}
}
/*Bitmap
1)struct bitmap * bitmap_expand(struct bitmap * bitmap,int size)
	- Parameter : Bitmap that you want to expand and the size of it
	- Return value : Expand bitmap if succeed, NULL if fail
	- Functionality : EXpand the given BITMAP to the SIZE(backward expansion)
*/
struct bitmap * bitmap_expand(struct bitmap * bitmap, int size)
{
	size_t now_size= bitmap_size(bitmap);
	size_t i=0;
	struct bitmap * node = bitmap_create(now_size+size);
	if(!node) return NULL;
	for(i=0;i<now_size;i++){
		if(bitmap_test(bitmap,i)) bitmap_set(node,i,true); //return value is bool
		else bitmap_set(node,i,false);
	}
	free(bitmap);
	return node;
}
/*Hash table
1) unsigned has_int_2(int i)
	- parameter:  Integer that will be hashed
	- return value : Hash value of integer I
	- Functionality : you can implement this function in yout own way and describe 
		you implement in the document
*/
unsigned hash_int_2(int i)
{ 
	Hasing(i);	
	return hash_bytes(&i,sizeof(i));
}
