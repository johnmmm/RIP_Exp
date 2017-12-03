#include "sysinclude.h"

extern void rip_sendIpPkt(unsigned char *pData, UINT16 len,unsigned short dstPort,UINT8 iNo);

extern struct stud_rip_route_node *g_rip_route_table;

struct RIPHeader //Header
{
	UINT8 command;
	UINT8 version;
	UINT16 must_be_zero;
};	//here are headers

struct RIPEntry //Entry
{
	UINT16 address_family_identifies;
	UINT16 route_tag;
	UINT32 ip_address;
	UINT32 subnet_mask;
	UINT32 next_hop;
	UINT32 metric;
};	//here are entrys

int stud_rip_packet_recv(char *pBuffer,int bufferSize,UINT8 iNo,UINT32 srcAdd)
{	
	RIPHeader *header = (RIPHeader*)pBuffer;
	if(header->version != 2)
	{
		ip_DiscardPkt(pBuffer, STUD_RIP_TEST_VERSION_ERROR);
		return -1;
	}
	if(header->command != 1 && header->command != 2)
	{
		ip_DiscardPkt(pBuffer, STUD_RIP_TEST_COMMAND_ERROR);
		return -1;
	}

	return 0;
}

void stud_rip_route_timeout(UINT32 destAdd, UINT32 mask, unsigned char msgType)
{
		
}
