
/*************************************************************************
   LAB 1

	Edit this file ONLY!

*************************************************************************/


#include <stdio.h>
#include "dns.h"

typedef struct _Pair
{
	char *domen;
	IPADDRESS ip;
	struct _Pair *next_pair;
}Pair;

const int SIZE = 12837;


DNSHandle InitDNS()
{
	DNSHandle hDNS = (unsigned int)(Pair *)calloc(SIZE, sizeof(Pair));
	if ((Pair *)hDNS == NULL) {
		return INVALID_DNS_HANDLE;
	}
	return hDNS;
}

unsigned int HashRs(const char *str)//Хеш-функция
{

	unsigned int hash = 0;

	for (; *str; str++)
	{
		hash += (unsigned char)(*str);
		hash += (hash << 10);
		hash ^= (hash >> 6);
	}
	hash += (hash << 3);
	hash ^= (hash >> 11);
	hash += (hash << 15);

	return hash % SIZE;

}
void Add_to_table(DNSHandle hDNS, char *str, IPADDRESS ip) { //Добавление элемента в хеш-таблицу
	Pair *p;
	unsigned int uHostNameLength = strlen(str);
	unsigned int index_element = HashRs(str);
	if (((Pair *)hDNS)[index_element].domen == NULL)
	{
		((Pair *)hDNS)[index_element].domen = (char *)malloc(uHostNameLength + 1);
		strcpy(((Pair *)hDNS)[index_element].domen, str);
		((Pair *)hDNS)[index_element].ip = ip;

	} else
	{
		p = (Pair *)malloc(sizeof(Pair));
		p->domen = (char *)malloc(uHostNameLength + 1);
		strcpy(p->domen, str);
		p->ip = ip;
		p->next_pair = ((Pair *)hDNS)[index_element].next_pair;
		((Pair *)hDNS)[index_element].next_pair = p;
	}

}
void LoadHostsFile(DNSHandle hDNS, const char *hostsFilePath)
{
	FILE *fInput = NULL;

	fInput = fopen(hostsFilePath, "r");

	unsigned int ip1 = 0, ip2 = 0, ip3 = 0, ip4 = 0;
	char *string = (char *)malloc(201);

	if (NULL == fInput)
		return;
	while (fscanf_s(fInput, "%d.%d.%d.%d %s", &ip1, &ip2, &ip3, &ip4, string, 200) != EOF)
	{
		IPADDRESS ip = (ip1 & 0xFF) << 24 |
			(ip2 & 0xFF) << 16 |
			(ip3 & 0xFF) << 8 |
			(ip4 & 0xFF);
		Add_to_table(hDNS, string, ip);
	}
	free(string);
	fclose(fInput);
}

IPADDRESS DnsLookUp(DNSHandle hDNS, const char *hostName)
{
	unsigned int index = HashRs(hostName);
	Pair *it = &((Pair *)hDNS)[index];
	while (it != NULL && strcmp(it->domen, hostName) != 0)
	{
		it = it->next_pair;
	}
	if (it != NULL)
		return it->ip;
	return INVALID_IP_ADDRESS;
}

void ShutdownDNS(DNSHandle hDNS)
{
	Pair *p;
	Pair *q;
	for (int i = 0; i < SIZE; i++) {
		q = &(((Pair *)hDNS)[i]);
		while (q != NULL)
		{
			p = q;
			q = q->next_pair;
			free(p->domen);
			free(p);
		}
	}
	if ((Pair *)hDNS != NULL)
		free((Pair *)hDNS);
}
