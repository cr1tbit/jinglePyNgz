/*
 * ping-draw for https://jinglepings.com/
 * Copyright (c) 2018 Vladislav Grishenko <themiron@mail.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 dated June, 1991, or
 * (at your option) version 3 dated 29 June, 2007.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


 //gcc -o ping-draw -O3 -Wall ping-draw.c -lpng
 //sudo ./ping-draw -x 1790 -y 560 -f 10 /mnt/nas-ceph/waifu_128a.png

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <err.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/times.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/icmp6.h>
#include <Python.h>

#undef offsetof
#ifdef __compiler_offsetof
#define offsetof(TYPE,MEMBER) __compiler_offsetof(TYPE,MEMBER)
#else
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif

#define ADDR "2001:610:1908:a000:0000:0000:0000:0000"


static int sock;

static long counter = 0;

static int ping6(struct in6_addr *addr)
{
	static union {
		struct icmp6_hdr hdr;
		unsigned char pad[20];
	} packet = {
		.hdr = {
			.icmp6_type = ICMP6_ECHO_REQUEST,
			.icmp6_seq = 1
		}
	};
	struct sockaddr_in6 sin6 = {
		.sin6_family = AF_INET6,
		.sin6_addr = *addr
	};
	int ret;

	packet.hdr.icmp6_id++;

	

	do {
		ret = sendto(sock, &packet, sizeof(packet), 0, (struct sockaddr *)&sin6, sizeof(sin6));
	} while (ret < 0 && errno == EINTR);
	printf("PINGED %ld\n",counter++);
	return ret;
}

static PyObject *method_blast_bytearray(PyObject *self, PyObject *args) {
    struct in6_addr *addr_array = NULL;
	
	long *len = NULL;

    /* Parse arguments */
    if(!PyArg_ParseTuple(args, "Ol", &addr_array, &len)) {
        return NULL;
    }

	for (int i=0;i<len;i++){
		ping6(&addr_array[i]);
	}

    return PyLong_FromLong(1);
}

static PyMethodDef TurbopingerMethods[] = {
    {"blast_bytearray", method_blast_bytearray, METH_VARARGS, "Blast array of ipv6 addreses, provided as raw bytearray"},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef TurbopingerModule = {
    PyModuleDef_HEAD_INIT,
    "turbopinger",
    "Blazingly-fast ipv6 pinger, made for jinglePings event and not for DDOSing.",
    -1,
    TurbopingerMethods
};

#ifndef STANDALONE
PyMODINIT_FUNC PyInit_turbopinger(void) {
	//main();
    return PyModule_Create(&TurbopingerModule);
}
#endif

// if __name__=="__main__":
int main(int argc, char *argv[])
{
	sock = socket(AF_INET6, SOCK_RAW, IPPROTO_ICMPV6);
	if (sock < 0)
		err(1, "socket error");

	int csum = offsetof(struct icmp6_hdr, icmp6_cksum);
	if (setsockopt(sock, SOL_RAW, IPV6_CHECKSUM, &csum, sizeof(csum)) < 0)
		err(1, "setsockopt error");

	struct in6_addr addr;
	//memset(&addr,0x00,8);

	//example - pinging my local IP:
	// fe80::17d2:7100:f6b6:757

	addr.s6_addr32[0]=0x000080fe;
	addr.s6_addr32[1]=0x00000000;
	addr.s6_addr32[2]=0x0071d217;
	addr.s6_addr32[3]=0x5707b6f6;
	
	for (int i=0;i<4;i++){
		printf("%04x ",  addr.s6_addr32[i]);
	}
	printf("\n");

	ping6(&addr);
	
	return 0;
}