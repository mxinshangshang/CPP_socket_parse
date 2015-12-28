#ifndef _SOCKET_MSG_H
#define _SOCKET_MSG_H
#ifdef _cplusplus
extern "C"{
#endif


//=============================================================================
//define macros
//=============================================================================

//=====================================
//socket message 



#define SOCKET_MSG_HEAD				0xa5a5		//head format of one message
#define SOCKET_MSG_END				0xbeef		//end format
#define SOCKET_MSG_HEAD_SIZE		2			//size of message head
#define SOCKET_MSG_TYPE_SIZE		1
#define SOCKET_MSG_LEN_SIZE			1
//#define SOCKET_MSG_CS_SIZE			1
#define SOCKET_MSG_END_SIZE			2			//size of message end

#define SOCKET_MSG_CACHE_SIZE		(512)			//client cache size
#define SOCKET_MSG_DATA_SIZE		(32)			//size of data buffer in one message
//size of format of one message
#define SOCKET_MSG_FORMAT_SIZE \
(SOCKET_MSG_HEAD_SIZE+ \
 SOCKET_MSG_TYPE_SIZE+ \
 SOCKET_MSG_LEN_SIZE + \
 SOCKET_MSG_END_SIZE)
//total size of one message
 #define SOCKET_MSG_SIZE \
(SOCKET_MSG_DATA_SIZE+\
 SOCKET_MSG_FORMAT_SIZE)
 
 #define SOCKET_MSG_TYPE_IDENTITY	0x01
 #define SOCKET_MSG_TYPE_DATA		0x02
		

//=============================================================================
//structure
//=============================================================================


typedef enum{
	SEARCH_HEAD,
	SEARCH_TYPE,
	SEARCH_LEN,
	//SEARCH_CS,
	SEARCH_END,
	SEARCH_NONE
}cache_strategy;


typedef struct{
	unsigned char data[SOCKET_MSG_SIZE];			//data
	int len;
	unsigned char type;
}socket_msg;


typedef void (*tp_socket_msg_handle)(int fd, socket_msg *msg,void *args); 

typedef struct{
	unsigned char buf[SOCKET_MSG_CACHE_SIZE]; 		//buffer for storing data read from client
	int front;
	int rear;
	int current;	
	int len;
	int tag;										//mark that whether the cache is full,1-full,0-not full
	cache_strategy strategy;
	tp_socket_msg_handle handle;		
	void* args;										//external 	parameter
	socket_msg recv_msg;
	
}socket_cache;



//=============================================================================
//function
//=============================================================================

//initialize the socket_msg structure
void socket_msg_init(socket_msg *msg);

//initialize the socket_cache structure
void socket_cache_init(socket_cache *cache, tp_socket_msg_handle handle);

//copy buffer to cache from buffer
int socket_msg_cpy_in(socket_cache *cache, unsigned char *buf, int len);

//copy data to buffer from cache
int socket_msg_cpy_out(socket_cache *cache, unsigned char *buf,int start_index, int len);

//parsed the packaged data, and invoke callback function
void socket_msg_parse(int fd, socket_cache *cache);

//copy the unparsed data to cache, and parsed them
int socket_msg_pre_parse(
int fd, 
socket_cache *cache,
unsigned char *buf, 
int len, 
void *args);

//before you send data,you should package them
void socket_msg_package(socket_msg *msg, unsigned char type,unsigned char *buf, int len);


#ifdef _cplusplus
}
#endif

#endif
