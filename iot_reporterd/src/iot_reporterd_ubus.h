#include <libubus.h>
#include <syslog.h>



void read_memory_info(struct ubus_request *req, int type, struct blob_attr *msg);