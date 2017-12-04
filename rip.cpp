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

struct RIPResponse //response sending
{
	RIPHeader rip_header;
	RIPEntry rip_entry[25];
};

void boardcast_route(UINT8 iNo)
{
	RIPResponse *rip_response = new RIPResponse();
		rip_response->rip_header.version = 2;
		rip_response->rip_header.command = 2;
		rip_response->rip_header.must_be_zero = 0;

		stud_rip_route_node* tmp_table = g_rip_route_table;
		int place = 0;
		while(tmp_table != NULL)
		{
			if(tmp_table->if_no != iNo && tmp_table->metric < 16) //it is OK to add it
			{
				//htons in ppt to change short to network
				rip_response->rip_entry[place].address_family_identifies = htons(2);
				rip_response->rip_entry[place].route_tag = htons(0);
				//htonl in ppt to change unsigned int to network
				rip_response->rip_entry[place].ip_address = htonl(tmp_table->dest);
				rip_response->rip_entry[place].subnet_mask = htonl(tmp_table->mask);
				rip_response->rip_entry[place].next_hop = htonl(tmp_table->nexthop);
				rip_response->rip_entry[place].metric = htonl(tmp_table->metric);
				place++;
			}
			if(place == 25)
			{
				place = 0;
				RIPResponse *rip_response = new RIPResponse();
				rip_response->rip_header.version = 2;
				rip_response->rip_header.command = 2;
				rip_response->rip_header.must_be_zero = 0;//build a new one
				rip_sendIpPkt((unsigned char*)rip_response, 504, 520, iNo);
			}
			tmp_table = tmp_table->next;
		}
		UINT16 len = 4 + 20 * place;
		rip_sendIpPkt((unsigned char*)rip_response, len, 520, iNo);
}

int stud_rip_packet_recv(char *pBuffer,int bufferSize,UINT8 iNo,UINT32 srcAdd)
{	
	RIPHeader *header = (RIPHeader*)pBuffer;
	if(header->version != 2)	//check version
	{
		ip_DiscardPkt(pBuffer, STUD_RIP_TEST_VERSION_ERROR);
		return -1;
	}
	if(header->command != 1 && header->command != 2)	//check command
	{
		ip_DiscardPkt(pBuffer, STUD_RIP_TEST_COMMAND_ERROR);
		return -1;
	}

	if(header->command == 1) //for request
	{
		boardcast_route(iNo);
	}

	else if(header->command == 2) //for response
	{
		//update

	}

	return 0;
}

void stud_rip_route_timeout(UINT32 destAdd, UINT32 mask, unsigned char msgType)
{
	if(msgType == RIP_MSG_SEND_ROUTE)	//boardcast route for every 30 seconds
	{
		//boardcast to No.1
		UINT8 iNo = 1;
		boardcast_route(iNo);

		//boardcast to NO.2
		iNo = 2;
		boardcast_route(iNo);
	}	
	else if(msgType == RIP_MSG_DELE_ROUTE)
	{

	}
	return;
}
