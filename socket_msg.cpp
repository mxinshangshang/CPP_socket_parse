#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "type.h"
#include "socket_msg.h"
#include "debug.h"

//initialize the socket_msg structure
void socket_msg_init(socket_msg *msg)
{
	msg->len = 0;
	msg->type = 0;
}

//initialize the socket_cache structure
void socket_cache_init(socket_cache *cache, tp_socket_msg_handle handle)
{
	cache->front = 0;
	cache->rear = 0;
	cache->current = 0;
	cache->len = 0;
	cache->tag = 0;
	cache->strategy = SEARCH_HEAD;
	cache->handle = handle;
	cache->args = NULL;

	socket_msg_init(&cache->recv_msg);	
}

//copy buffer to cache from buffer
int socket_msg_cpy_in(socket_cache *cache, unsigned char *buf, int len)
{
	int left_len;
	int copy_len;
	int count;
	unsigned char *src = buf;
	
	if(cache->tag == 1 &&cache->front == cache->rear){
		LOGE("socket_msg_cpy_in:socket cache is full!\n");
		return 	FALSE;	
	}
	
	left_len = SOCKET_MSG_CACHE_SIZE - cache->len;
	copy_len = len > left_len ? left_len : len;
	count = copy_len;
		
	while(count --)
	{
		*(cache->buf + cache->rear) = *src;
		src ++;
		cache->rear = (cache->rear + 1) % SOCKET_MSG_CACHE_SIZE;	
		cache->len ++;
		cache->tag = 1;
		
	}
	return copy_len;
}

//copy data to buffer from cache
int socket_msg_cpy_out(socket_cache *cache, unsigned char *buf,int start_index, int len)
{
	unsigned char* dest;
	int src;
	int count;
	if(cache == NULL || cache->buf == NULL || buf == NULL || len == 0){
		return FALSE;	
	}
	
	if(cache->front == cache->rear && cache->tag == 0){
		LOGE("socket_msg_cpy_out:socket cache is empty!\n");
		return FALSE;
	}
	
	if(cache->rear > cache->front ){
		if(start_index < cache->front || start_index > cache->rear){
			LOGE("socket_msg_cpy_out:invalid start index!\n");	
			return FALSE;
		}
	}
	else if(start_index > cache->rear && start_index < cache->front){
		LOGE("socket_msg_cpy_out:invalid start index!\n");	
		return FALSE;
	}
		
	src = start_index;
	dest = buf;
	count = len;
	while(count --)
	{
		*dest = *(cache->buf + src);
		 dest ++;
		 src  = (src + 1) % SOCKET_MSG_CACHE_SIZE;
		
	}
	return len;
}



//parsed the packaged data, and invoke callback function
void socket_msg_parse(int fd, socket_cache *cache)
{
	int current_len;
	int p, q;
	int i;
	int find;
	
	if(cache->front == cache->rear && cache->tag == 0){
		//D("socket cache is empty!\n");
		return;	
	}
	
	//calculate the current length of cache
	if(cache->current >= cache->front){
		current_len = cache->len - (cache->current - cache->front);
	}
	else{
		current_len = cache->rear - cache->current;	
	}
	
	switch(cache->strategy){
		case SEARCH_HEAD://to find a Head format in cache
			if(current_len < SOCKET_MSG_HEAD_SIZE){
				return;	
			}
			find = FALSE;
			for(i = 0; i < current_len - 1; i++){
				p = cache->current;
				q = (cache->current + 1) % SOCKET_MSG_CACHE_SIZE;
				if(	(cache->buf[p] == (SOCKET_MSG_HEAD >> 8))&&
					(cache->buf[q] == (SOCKET_MSG_HEAD & 0xff))){
					
					find = TRUE;
					break; //exit for loop
				}
				else{
					//current pointer move to next
					cache->current = q;
					//delete one item
					cache->front = cache->current;
					cache->len --;
					cache->tag = 0;
				}
			}
			
			if(find == TRUE){	
				//move 2 items towards next
				cache->current = (cache->current + 2) % SOCKET_MSG_CACHE_SIZE;	
				//we found the head format, go on to find Type byte
				cache->strategy = SEARCH_TYPE;		
			}
			else{
				//if there is no head format ,delete previouse items
				LOGE("socket message without head: %x!\n",SOCKET_MSG_HEAD);
				//go on to find Head format
				cache->strategy = SEARCH_HEAD;	
			}
			break;
			
		case SEARCH_TYPE://to find the type byte in cache
			if(current_len < SOCKET_MSG_TYPE_SIZE){
				return ;	
			}
			//get the value of type
			//cache->type = cache->buf[cache->current];
			cache->recv_msg.type = cache->buf[cache->current];
			cache->current = (cache->current + 1) % SOCKET_MSG_CACHE_SIZE;
			//we found Type byte, go on to find Datalen format
			cache->strategy = SEARCH_LEN;
			break;
			
		case SEARCH_LEN://to find the datalen byte in cache
			if(current_len < SOCKET_MSG_LEN_SIZE){
				return ;	
			}
			if(cache->buf[cache->current] > SOCKET_MSG_DATA_SIZE){
				LOGE("the data len of message out of size: %d!\n",SOCKET_MSG_DATA_SIZE);
				//delete the frist item 'a5'
				//move back 2 items
				cache->current = cache->current >= 2 ? (cache->current - 2) : (SOCKET_MSG_CACHE_SIZE - 2 + cache->current);
				cache->front = cache->current;
				//length sub 2
				cache->len -= 2;
				cache->tag = 0;
				//go on to find Head format
				cache->strategy = SEARCH_HEAD;
			}
			else{
				//get the value of datalen
				//cache->data_len = cache->buf[cache->current];
				cache->recv_msg.len = cache->buf[cache->current];
				cache->current = (cache->current + 1) % SOCKET_MSG_CACHE_SIZE;
				//we found datalen byte, go on to find End format
				cache->strategy = SEARCH_END;
			}
			break;
			
		
			
		case SEARCH_END:
			if(current_len < (cache->recv_msg.len + SOCKET_MSG_END_SIZE)){
				return;	
			}
			//because we have known the data bytes' len, so we move the very  
			//distance of datalen to see if there is End format. 
			p = (cache->current + cache->recv_msg.len) % SOCKET_MSG_CACHE_SIZE; 
			q = (cache->current + cache->recv_msg.len + 1) % SOCKET_MSG_CACHE_SIZE; 
			if(	(cache->buf[p] == (SOCKET_MSG_END >> 8))&&
				(cache->buf[q] == (SOCKET_MSG_END & 0xff)) ){
				socket_msg_cpy_out(cache, cache->recv_msg.data, cache->current, cache->recv_msg.len);
				if(cache->handle != NULL){
					//cache->handle(fd, cache->buf + cache->data_index, cache->data_len);
					cache->handle(fd, &cache->recv_msg, cache->args);	
				}
				//delete all previous items
				cache->current = (q + 1) % SOCKET_MSG_CACHE_SIZE;
				cache->front = cache->current;
				cache->len -= (cache->recv_msg.len + SOCKET_MSG_FORMAT_SIZE);
				cache->tag =0;
				
			}
			else{
				LOGE("socket message without end: %x!\n",SOCKET_MSG_END);
				//delete the frist item 'a5'
				//move back 3 items
				cache->current = cache->current >= 3 ? (cache->current - 3) : (SOCKET_MSG_CACHE_SIZE - 3 + cache->current);
				cache->front = cache->current;
				//length sub 3
				cache->len -= 3;
				cache->tag = 0;
					
			}
			//go on to find Head format
			cache->strategy = SEARCH_HEAD;
			break;
			
		default:
				break;
				
				
	}
	
	//parse new socket message
	socket_msg_parse(fd,cache);
}

//copy the unparsed data to cache, and parsed them
int socket_msg_pre_parse(
int fd, 
socket_cache *cache,
unsigned char *buf, 
int len, 
void *args)
{
	int n = 0;
	unsigned char *p = buf;
	//when reading buffer's length is greater than cache's left length,
	//we should copy many times.
	cache->args = args;
	while(1){
		n = socket_msg_cpy_in(cache, p, len);
		if(n == 0){
			return FALSE;//cache is full	
		}
		//parse and handle socket message from cache
		socket_msg_parse(fd, cache);
		
		if(n == len){
			return TRUE; //copy completed
		}
		//move the pointer
		p 	= p + n;
		len = len - n;
	}
	
	return TRUE;
		
}


//before you send data,you should package them
void socket_msg_package(socket_msg *msg, unsigned char type,unsigned char *buf, int len)
{
	if(msg == NULL || buf == NULL || len <= 0){
		return;	
	}
	//head
	msg->data[0] = SOCKET_MSG_HEAD >> 8;
	msg->data[1] = SOCKET_MSG_HEAD & 0xff;
	//type
	msg->data[2] = type;
	//data len
	msg->data[3] = len;
	//data
	memcpy(msg->data + 4, buf, len);
	//end
	msg->data[len + 4] = SOCKET_MSG_END >> 8;
	msg->data[len + 5] = SOCKET_MSG_END & 0xff;
	
	//message len
	msg->len = len + SOCKET_MSG_FORMAT_SIZE;
}