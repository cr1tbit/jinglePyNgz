/*
 * turbopinger - python C module made for https://jinglepings.com/ and
 * Copyright (c) 2019 Jakub Sadowski <js@vcc.earth>
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

/*
 * This file is modified version of Vladislav Grishenko's ping-draw.
 * The original .c file may be found in the same repo, under 
 * helpers/ping-draw
 */

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

/*
 * Methods around socket stuff
 */

int get_sock(){
	int sock = socket(AF_INET6, SOCK_RAW, IPPROTO_ICMPV6);
	if (sock < 0){
		//err(1, "socket error");
		return -1;
	}
		
	int csum = offsetof(struct icmp6_hdr, icmp6_cksum);
	if (setsockopt(sock, SOL_RAW, IPV6_CHECKSUM, &csum, sizeof(csum)) < 0){
		//err(1, "setsockopt error");
		return -1;
	}
		
	return sock;
}

static int ping6(int sock_fd, struct in6_addr *addr)
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
		ret = sendto(sock_fd, &packet, sizeof(packet), 0, (struct sockaddr *)&sin6, sizeof(sin6));
	} while (ret < 0 && errno == EINTR);

	return ret;
}

/*
 * Python wrappers
 */

static PyObject *method_blast_ip6_list(PyObject *self, PyObject *args) {
    //pings fuckload of IPs coming from a bytearray list 
	int sock = get_sock();	
	int counter = 0;
	PyObject *obj;

	if (sock == -1){
		printf("error creating socket. Are you did sudo?");
		return NULL;
	}

	if (!PyArg_ParseTuple(args, "O", &obj)) {
		printf("The received object is not object. Bailing out.");
		return NULL;
	}

	PyObject *iter = PyObject_GetIter(obj);
	if (!iter) {
		printf("The received object is not a list. Bailing out.");
	}

	while (1) {
		PyObject *next = PyIter_Next(iter);
		if (!next) {
			// nothing left in the iterator
			break;
		}

		if (PyBytes_Check(next)) {
			counter++;
			struct in6_addr *addr = PyBytes_AsString(next);
			ping6(sock, addr);
		}
	}

	printf("blasted %d IPs",counter);
    return PyLong_FromLong(1);
}

static PyMethodDef TurbopingerMethods[] = {
	{"blast_ip6_list", method_blast_ip6_list, METH_VARARGS, "Blast array of ipv6 addreses, provided as raw bytearray"},
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

/*
 * The module may be compiled and run stand-alone, for 
 * testing I suppose. if __name__=="__main__":
 */

int main(int argc, char *argv[])
{
	int sock = get_sock();
	struct in6_addr addr;

	/*
	 example - pinging my local IP:
	 fe80::17d2:7100:f6b6:757
	 
	 Please note - endianness is absolutely reversed.

	 To check if it works, you may use tcpdump:
	 	sudo tcpdump -i any icmp6

	 Please note2 - imcp6 != imcp
	*/

	addr.s6_addr32[0]=0x000080fe;
	addr.s6_addr32[1]=0x00000000;
	addr.s6_addr32[2]=0x0071d217;
	addr.s6_addr32[3]=0x5707b6f6;
	
	printf("pinging ip6 address: ");
	for (int i=0;i<4;i++){
		printf("%08x ",  addr.s6_addr32[i]);
	}
	printf("\n");

	ping6(sock, &addr);
	
	return 0;
}

