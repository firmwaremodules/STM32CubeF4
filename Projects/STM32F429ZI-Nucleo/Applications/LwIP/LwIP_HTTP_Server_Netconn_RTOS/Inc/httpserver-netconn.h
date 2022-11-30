#ifndef __HTTPSERVER_NETCONN_H__
#define __HTTPSERVER_NETCONN_H__

#include "lwip/api.h"

void http_server_netconn_init(void);
void DynWebPage(struct netconn *conn);
void FwUpdWebPage(struct netconn* conn);
void print_buf(uint8_t* buf, int len);

#endif /* __HTTPSERVER_NETCONN_H__ */
