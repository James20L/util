//
// Created by hujianzhe
//

#ifndef	UTIL_C_SYSLIB_SOCKET_H
#define	UTIL_C_SYSLIB_SOCKET_H

#include "../platform_define.h"

#if defined(_WIN32) || defined(_WIN64)
	#include <ws2tcpip.h>
	#include <iphlpapi.h>
	#include <winnetwk.h>
	#include <mswsock.h>
	#include <mstcpip.h>
	typedef struct if_nameindex {
		unsigned int if_index;
		char if_name[IF_NAMESIZE];
	} if_nameindex_t;
	#define	SHUT_RD					SD_RECEIVE
	#define	SHUT_WR					SD_SEND
	#define	SHUT_RDWR				SD_BOTH
	#pragma comment(lib, "wsock32.lib")
	#pragma comment(lib, "ws2_32.lib")
	#pragma comment(lib, "iphlpapi.lib")
#else
	#include <sys/socket.h>
	#if defined(__FreeBSD__) || defined(__APPLE__)
		#include <net/if_dl.h>
		#include <net/if_types.h>
	#elif __linux__
		#include <netpacket/packet.h>
		#include <linux/rtnetlink.h>
	#endif
	#include <sys/select.h>
	#include <poll.h>
	#include <netdb.h>
	#include <arpa/inet.h>
	#include <net/if.h>
	#include <net/if_arp.h>
	#include <netinet/tcp.h>
	#include <ifaddrs.h>
	#define	SD_RECEIVE				SHUT_RD
	#define	SD_SEND					SHUT_WR
	#define	SD_BOTH					SHUT_RDWR
	typedef struct if_nameindex		if_nameindex_t;
#endif

typedef char IPString_t[INET6_ADDRSTRLEN];
enum {
	IP_TYPE_UNKNOW,
	IPv4_TYPE_A,
	IPv4_TYPE_B,
	IPv4_TYPE_C,
	IPv4_TYPE_D,
	IPv4_TYPE_E,
	IPv6_TYPE_GLOBAL,
	IPv6_TYPE_LINK,
	IPv6_TYPE_SITE,
	IPv6_TYPE_v4MAP
};
enum {
	NETWORK_INTERFACE_UNKNOWN,
	NETWORK_INTERFACE_LOOPBACK,
	NETWORK_INTERFACE_PPP,
	NETWORK_INTERFACE_TOKENRING,
	NETWORK_INTERFACE_ETHERNET,
	NETWORK_INTERFACE_WIRELESS,
	NETWORK_INTERFACE_FIREWIRE
};
struct address_list {
	struct address_list* next;
	struct sockaddr_storage ip;
	struct sockaddr_storage mask;
};
typedef struct NetworkInterfaceInfo_t {
	struct NetworkInterfaceInfo_t* next;
	unsigned int if_index;
	char if_name[IF_NAMESIZE];
	int if_type;
	unsigned int mtu;
	unsigned int phyaddrlen;
#if defined(_WIN32) || defined(_WIN64)
	unsigned char phyaddr[MAX_ADAPTER_ADDRESS_LENGTH];
#else
	unsigned char phyaddr[8];
#endif
	struct address_list* address;
} NetworkInterfaceInfo_t;

#ifdef	__cplusplus
extern "C" {
#endif

/* Network */
#if defined(_WIN32) || defined(_WIN64)
__declspec_dll if_nameindex_t* if_nameindex(void);
__declspec_dll void if_freenameindex(if_nameindex_t* ptr);
#else
#if	__linux__
unsigned long long ntohll(unsigned long long val);
unsigned long long htonll(unsigned long long val);
#endif
unsigned int htonf(float val);
float ntohf(unsigned int val);
unsigned long long htond(double val);
double ntohd(unsigned long long val);
#endif
__declspec_dll BOOL networkSetupEnv(void);
__declspec_dll BOOL networkCleanEnv(void);
__declspec_dll NetworkInterfaceInfo_t* networkInterfaceInfo(void);
__declspec_dll void networkFreeInterfaceInfo(NetworkInterfaceInfo_t* info);
/* SOCKET ADDRESS */
__declspec_dll int sockaddrIPType(const struct sockaddr* sa);
__declspec_dll const char* ipstrGetLoopback(int family);
__declspec_dll BOOL ipstrIsLoopback(const char* ip);
__declspec_dll BOOL ipstrIsInner(const char* ip);
__declspec_dll int ipstrFamily(const char* ip);
__declspec_dll int sockaddrLength(const struct sockaddr* saddr);
__declspec_dll BOOL sockaddrEncode(struct sockaddr_storage* saddr, int af, const char* strIP, unsigned short port);
__declspec_dll BOOL sockaddrDecode(const struct sockaddr_storage* saddr, char* strIP, unsigned short* port);
__declspec_dll BOOL sockaddrSetPort(struct sockaddr_storage* saddr, unsigned short port);
/* SOCKET */
__declspec_dll BOOL socketBindAddr(FD_t sockfd, const struct sockaddr_storage* saddr);
__declspec_dll BOOL socketGetLocalAddr(FD_t sockfd, struct sockaddr_storage* saddr);
__declspec_dll BOOL socketGetPeerAddr(FD_t sockfd, struct sockaddr_storage* saddr);
#if defined(_WIN32) || defined(_WIN64)
#define	socketClose(sockfd)	(closesocket((SOCKET)(sockfd)) == 0)
#else
#define	socketClose(sockfd)	(close(sockfd) == 0)
#endif
__declspec_dll int socketError(FD_t sockfd);
#define socketUdpConnect(sockfd, saddr, addrlen)	(connect(sockfd, saddr, addrlen) == 0)
__declspec_dll BOOL socketUdpDisconnect(FD_t sockfd);
__declspec_dll FD_t socketTcpConnect(const struct sockaddr* addr, int addrlen, int msec);
__declspec_dll BOOL socketIsConnected(FD_t fd, BOOL* bool_value);
#define socketTcpListen(sockfd)		(listen(sockfd, SOMAXCONN) == 0)
__declspec_dll BOOL socketIsListened(FD_t sockfd, BOOL* bool_value);
__declspec_dll FD_t socketTcpAccept(FD_t listenfd, int msec, struct sockaddr_storage* from);
#define	socketShutdown(sockfd, how)	(shutdown(sockfd, how) == 0)
__declspec_dll BOOL socketPair(int type, FD_t sockfd[2]);
__declspec_dll int socketRead(FD_t sockfd, void* buf, unsigned int nbytes, int flags, struct sockaddr_storage* from);
#define socketWrite	sendto
__declspec_dll int socketReadv(FD_t sockfd, Iobuf_t iov[], unsigned int iovcnt, int flags, struct sockaddr_storage* saddr);
__declspec_dll int socketWritev(FD_t sockfd, Iobuf_t iov[], unsigned int iovcnt, int flags, const struct sockaddr* to, int tolen);
__declspec_dll int socketTcpReadAll(FD_t sockfd, void* buf, unsigned int nbytes);
__declspec_dll int socketTcpWriteAll(FD_t sockfd, const void* buf, unsigned int nbytes);
#define socketTcpSendOOB(sockfd, oob) (send(sockfd, (char*)&(oob), 1, MSG_OOB) == 1)
__declspec_dll int socketTcpCanRecvOOB(FD_t sockfd);
__declspec_dll BOOL socketTcpReadOOB(FD_t sockfd, unsigned char* oob);
__declspec_dll int socketSelect(int nfds, fd_set* rset, fd_set* wset, fd_set* xset, int msec);
__declspec_dll int socketPoll(struct pollfd* fdarray, unsigned long nfds, int msec);
__declspec_dll BOOL socketNonBlock(FD_t sockfd, BOOL bool_val);
__declspec_dll int socketTcpReadableBytes(FD_t sockfd);
__declspec_dll BOOL socketSetSendTimeout(FD_t sockfd, int msec);
__declspec_dll BOOL socketSetRecvTimeout(FD_t sockfd, int msec);
__declspec_dll BOOL socketSetUnicastTTL(FD_t sockfd, int family, unsigned char ttl);
__declspec_dll BOOL socketSetMulticastTTL(FD_t sockfd, int family, int ttl);
__declspec_dll BOOL socketUdpMcastGroupJoin(FD_t sockfd, const struct sockaddr* grp);
__declspec_dll BOOL socketUdpMcastGroupLeave(FD_t sockfd, const struct sockaddr* grp);
__declspec_dll BOOL socketUdpMcastEnableLoop(FD_t sockfd, int family, BOOL bool_val);

#ifdef	__cplusplus
}
#endif

#endif
