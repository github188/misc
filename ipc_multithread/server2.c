#include "message.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include "debug.h"

pthread_t p1;

int32 handle_test(const int32 cmd, void *ibuf, int32 ilen, void *obuf, int32 *olen)
{
	//memset((char*)ibuf+ilen, 0, 1);
	printf("gateway_handle_test %d : %s\n", ilen, (char*)ibuf);
	memcpy(obuf, ibuf, ilen);
	*olen = ilen;
	return 0;
}

int32 handle_test_netlink(const int32 cmd, void *ibuf, int32 ilen, void *obuf, int32 *olen)
{
	//memset((char*)ibuf+ilen, 0, 1);
	printf("gateway_handle_netlink_test %d : %s\n", ilen, (char*)ibuf);
	memcpy(obuf, ibuf, ilen);
	*olen = ilen;
	return 0;
}

typedef struct  statistic {
	unsigned int success;
	unsigned int notcompare;
	unsigned int error;
}sta;
void sig_hander( int a )  
{  
	pthread_cancel(p1);
	pthread_join(p1, NULL);
	msg_final(); 
	exit(0);
} 
static void *thread_func(void *arg)
{
	struct timeval tv_bef;
	struct timeval current;
	sta test;
	test.success = 0;
	test.notcompare = 0;
	test.error = 0;
	char *get = NULL;
	int slen = 0;
	int rlen = 0;
	char temp_buf[1000] = {0};

	
	while (1) {
		usleep(1000*2);
		memset(temp_buf, 0, 1000);
		gettimeofday(&current, NULL);
		slen = 100 * (current.tv_sec & 7) + 1;
		memset(temp_buf, current.tv_sec & 7, slen - 1);

		if (test.success%6000 == 0)
			TEST_LOG("server1 thread1 success:%d, notcmp:%d, error:%d  time:%d\n", test.success, test.notcompare, test.error, current.tv_sec);

		if (msg_send_syn(DPI_TEST1, temp_buf, slen, (void *)&get, &rlen) != 0 ) {
			test.error++;
			continue;
		} else {
			if ((memcmp(temp_buf, get, slen) == 0) && (slen == rlen))
				test.success++;
			else
				test.notcompare++;
			free_rcv_buf(get);
		}
		if (msg_send_syn(GATEWAY_KERNEL_TEST1, temp_buf, slen, (void *)&get, &rlen) != 0 ) {
			test.error++;
			continue;
		} else {
			if ((memcmp(temp_buf, get, slen) == 0) && (slen == rlen))
				test.success++;
			else
				test.notcompare++;
			free_rcv_buf(get);
		}
		if (msg_send_syn(DPI_KERNEL_TEST1, temp_buf, slen, (void *)&get, &rlen) != 0 ) {
			test.error++;
			continue;
		} else {
			if ((memcmp(temp_buf, get, slen) == 0) && (slen == rlen))
				test.success++;
			else
				test.notcompare++;
			free_rcv_buf(get);
		}
	}
	
}

int main()
{ 
	struct timeval tv_bef;
	struct timeval current;
	sta test;
	test.success = 0;
	test.notcompare = 0;
	test.error = 0;
	
	msg_init(MODULE_GATEWAY, 3, "server1_rcv", "server1_snd");
	signal(SIGINT, sig_hander);
	if (msg_cmd_register(GATEWAY_TEST1, handle_test))
		printf("register error\n");
	if (msg_cmd_register(GATEWAY_TEST2, handle_test_netlink))
		printf("register error\n");

	msg_dst_module_register_unix(MODULE_DPI, "client1_rcv");
	msg_dst_module_register_netlink(MODULE_GATEWAY_KERNEL);
	msg_dst_module_register_netlink(MODULE_DPI_KERNEL);
	
	pthread_create(&p1, NULL, thread_func, NULL);
	
	char *get = NULL;
	int slen = 0;
	int rlen = 0;
	char temp_buf[1000] = {0};

	
	while (1) {
		usleep(1000*2);
		memset(temp_buf, 0, 1000);
		gettimeofday(&current, NULL);
		slen = 100 * (current.tv_sec & 7) + 1;
		memset(temp_buf, current.tv_sec & 7, slen - 1);

		if (test.success%6000 == 0)
			TEST_LOG("server1 success:%d, notcmp:%d, error:%d  time:%d\n", test.success, test.notcompare, test.error, current.tv_sec);

		if (msg_send_syn(DPI_TEST1, temp_buf, slen, (void *)&get, &rlen) != 0 ) {
			test.error++;
			continue;
		} else {
			if ((memcmp(temp_buf, get, slen) == 0) && (slen == rlen))
				test.success++;
			else
				test.notcompare++;
			free_rcv_buf(get);
		}
		if (msg_send_syn(GATEWAY_KERNEL_TEST1, temp_buf, slen, (void *)&get, &rlen) != 0 ) {
			test.error++;
			continue;
		} else {
			if ((memcmp(temp_buf, get, slen) == 0) && (slen == rlen))
				test.success++;
			else
				test.notcompare++;
			free_rcv_buf(get);
		}
		if (msg_send_syn(DPI_KERNEL_TEST1, temp_buf, slen, (void *)&get, &rlen) != 0 ) {
			test.error++;
			continue;
		} else {
			if ((memcmp(temp_buf, get, slen) == 0) && (slen == rlen))
				test.success++;
			else
				test.notcompare++;
			free_rcv_buf(get);
		}
	}
	
}