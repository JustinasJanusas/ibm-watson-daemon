#include "iot_reporterd_ubus.h"

enum {
	TOTAL_MEMORY,
	FREE_MEMORY,
	SHARED_MEMORY,
	BUFFERED_MEMORY,
	__MEMORY_MAX,
};

enum {
	MEMORY_DATA,
	__INFO_MAX,
};


const struct blobmsg_policy memory_policy[__MEMORY_MAX] = {
	[TOTAL_MEMORY] = { .name = "total", .type = BLOBMSG_TYPE_INT64 },
	[FREE_MEMORY] = { .name = "free", .type = BLOBMSG_TYPE_INT64 },
	[SHARED_MEMORY] = { .name = "shared", .type = BLOBMSG_TYPE_INT64 },
	[BUFFERED_MEMORY] = { .name = "buffered", .type = BLOBMSG_TYPE_INT64 },
};
const struct blobmsg_policy info_policy[__INFO_MAX] = {
	[MEMORY_DATA] = { .name = "memory", .type = BLOBMSG_TYPE_TABLE },
};

void read_memory_info(struct ubus_request *req, int type, 
							struct blob_attr *msg) {
	struct blob_buf *buf = (struct blob_buf *)req->priv;
	struct blob_attr *tb[__INFO_MAX];
	struct blob_attr *memory[__MEMORY_MAX];
	blobmsg_parse(info_policy, __INFO_MAX, tb, blob_data(msg), blob_len(msg));

	if ( !tb[MEMORY_DATA] ) {
		syslog(LOG_WARNING, "No memory data received\n");
		return;
	}
	blobmsg_parse(memory_policy, __MEMORY_MAX, memory,
				blobmsg_data(tb[MEMORY_DATA]), blobmsg_data_len(tb[MEMORY_DATA]));
	snprintf((char *) buf, 400, "{\"d\":{\"Total memory\":%lld, \"Free memory\":%lld, "
			"\"Shared memory\":%lld, \"Buffered memory\":%lld}}", 
			blobmsg_get_u64(memory[TOTAL_MEMORY]), blobmsg_get_u64(memory[FREE_MEMORY]),
			blobmsg_get_u64(memory[SHARED_MEMORY]), blobmsg_get_u64(memory[BUFFERED_MEMORY]));
}

int handle_ubus_error(int error, struct ubus_context *ctx, uint32_t *id)
{
	int rc = 0;
	switch (error)
	{
	case UBUS_STATUS_CONNECTION_FAILED:
		ctx = ubus_connect(NULL);
		if(!ctx)
			break;
	case UBUS_STATUS_NOT_FOUND | UBUS_STATUS_METHOD_NOT_FOUND:
		rc = ubus_lookup_id(ctx, "system", id);
		if(rc)
			break;
		return 0;
	default:
		rc = 1;
	}
	syslog(LOG_ERR, "Failed to handle error: %d", error);
	return rc;
}