
/*************************************************************************
   LAB 1

	Edit this file ONLY!

*************************************************************************/


#include <stdio.h>
#include "dns.h"

struct Pair
{
	IPADDRESS ip;
	char* domen;
	struct Pair* nextPair;
};

const int SIZE = 12838;


DNSHandle InitDNS()
{
	DNSHandle hDNS = (unsigned int)(Pair*)calloc(SIZE, sizeof(Pair));
	if ((Pair*)hDNS != NULL)
	{
		return hDNS;
	}
	return INVALID_DNS_HANDLE;
}

/*
* hash function
*/
unsigned int HashFunc(const char* str)
{
	unsigned int hash = 0;

	while (c = *str++)
	{
		hash = ((hash << 5) + hash) + c;
	}

	return hash % SIZE;

}

/*
* Function that adds element to hash table
*/
void AddToTable(DNSHandle hDNS, char* str, IPADDRESS ip)
{
	unsigned int lengthOfHost = strlen(str);
	unsigned int indexOfElement = HashFunc(str);

	if (((Pair*)hDNS)[indexOfElement].domen == NULL)
	{
		((Pair*)hDNS)[indexOfElement].ip = ip;
		((Pair*)hDNS)[indexOfElement].domen = (char*)malloc(lengthOfHost + 1);
		strcpy(((Pair*)hDNS)[indexOfElement].domen, str);
	}
	else
	{
		Pair* p;
		p = (Pair*)malloc(sizeof(Pair));
		p->ip = ip;
		p->domen = (char*)malloc(lengthOfHost + 1);
		strcpy(p->domen, str);
		p->nextPair = ((Pair*)hDNS)[indexOfElement].nextPair;
		((Pair*)hDNS)[indexOfElement].nextPair = p;
	}

}
void LoadHostsFile(DNSHandle hDNS, const char* hostsFilePath)
{
	FILE* fInput = NULL;

	fInput = fopen(hostsFilePath, "r");

	unsigned int ip1 = 0, ip2 = 0, ip3 = 0, ip4 = 0;
	char* string = (char*)malloc(201);

	if (NULL == fInput)
		return;
	while (fscanf_s(fInput, "%d.%d.%d.%d %s", &ip1, &ip2, &ip3, &ip4, string, 200) != EOF)
	{
		IPADDRESS ip = (ip1 & 0xFF) << 24 | (ip2 & 0xFF) << 16 | (ip3 & 0xFF) << 8 | (ip4 & 0xFF);
		AddToTable(hDNS, string, ip);
	}
	free(string);
	fclose(fInput);
}

IPADDRESS DnsLookUp(DNSHandle hDNS, const char* hostName)
{
	unsigned int index = HashFunc(hostName);
	Pair* it = &((Pair*)hDNS)[index];
	while (it != NULL && strcmp(it->domen, hostName) != 0)
	{
		it = it->nextPair;
	}
	if (it != NULL)
	{
		return it->ip;
	}
	return INVALID_IP_ADDRESS;
}

void ShutdownDNS(DNSHandle hDNS)
{
	Pair* p;
	Pair* q;
	for (int i = 0; i < SIZE; i++)
	{
		q = &(((Pair*)hDNS)[i]);
		while (q != NULL)
		{
			p = q;
			q = q->nextPair;
			free(p->domen);
			free(p);
		}
	}
	if ((Pair*)hDNS != NULL)
	{
		free((Pair*)hDNS);
	}
}
