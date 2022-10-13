#include <libubus.h>
#include <syslog.h>



void read_memory_info(struct ubus_request *req, int type, struct blob_attr *msg);
int handle_ubus_error(int error, struct ubus_context *ctx, uint32_t *id);