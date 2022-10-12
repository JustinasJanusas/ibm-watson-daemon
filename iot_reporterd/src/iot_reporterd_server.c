#include "iot_reporterd_server.h"

int initiate_device(char* args[], IoTPConfig **config, IoTPDevice **device)
{
	char *option_names[] = { IoTPConfig_identity_orgId, IoTPConfig_identity_typeId,
						IoTPConfig_identity_deviceId, IoTPConfig_auth_token };
	int rc = 0;
	rc = IoTPConfig_setLogHandler(IoTPLog_FileDescriptor, &LogCallback);
    if ( rc ) {
        syslog(LOG_WARNING, "WARN: Failed to set IoTP Client log handler: rc=%d\n", rc);
    }
	rc = IoTPConfig_create(config, NULL);
	if( rc ){
		syslog(LOG_ERR, "ERR: Failed to set up config: %d", rc);
		return rc;
	}
	for(int i = 0; i < 4; i++){
		IoTPConfig_setProperty(*config, option_names[i], args[i]);
	}
	rc = IoTPDevice_create(device, *config);
	if( rc ){
		syslog(LOG_ERR, "ERR: Failed to set up device %d", rc);
		return rc;
	}
	rc = IoTPDevice_connect(*device);
	if( rc ){
		syslog(LOG_ERR, "ERR: Failed to connect to Watsion IoT platform: %d", rc);
		return rc;
	}
	return 0;
}

int send_data_to_server(char data[], IoTPDevice *device){
	IOTPRC rc;
	syslog(LOG_DEBUG, "data: %s", data);
	rc = IoTPDevice_sendEvent(device,"status", data, "json", QoS0, NULL);
	return rc;
}

void LogCallback (int level, char * message)
{
    if ( level > 0 )
        syslog(LOG_INFO, "%s", message? message:"NULL");
}