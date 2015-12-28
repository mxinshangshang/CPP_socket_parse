#include <debug.h>
#include <stdio.h>
#include <string.h>


#define MAX_DEBUG_BUFFER 512
static char out_buf[MAX_DEBUG_BUFFER];

void debug_buffer(const char *prefix,unsigned char *buffer,int len)
{

#if USING_DEBUG
	//LOGD("debug_buffer");
	int i = 0;
	char *p = out_buf;
	if(prefix != NULL){
		int plen = strlen(prefix);
		memcpy(p, prefix, plen);
		p = p + plen;
	}
	
	for(i = 0; i < len; i++){
		sprintf(p, "%02x ",buffer[i]);
		p = p + 3;
	}
	
	p --;
	*p = '\0';
	
	LOGD(out_buf);
#endif

}

void debug_info(const char *prefix,unsigned char *buffer,int len)
{
#if USING_DEBUG
	//LOGD("debug_info");
	char *p = out_buf;
	if(prefix != NULL){
		int plen = strlen(prefix);
		memcpy(p, prefix, plen);
		p = p + plen;
	}
	
	memcpy(p ,buffer,len);
	p = p + len;
	
	*p = '\0';
	
	LOGD(out_buf);
#endif

}

