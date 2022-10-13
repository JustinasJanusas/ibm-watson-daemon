#include "iot_reporterd_server.h"

int initiate_config(char *organisationId, char *deviceId, char *typeId, char *token,
					IoTPConfig **config)
{
	int rc = 0;
	rc = IoTPConfig_setLogHandler(IoTPLog_FileDescriptor, &LogCallback);
    if ( rc ) {
        syslog(LOG_WARNING, "WARN: Failed to set IoTP Client log handler: rc=%d\n", rc);
    }
	rc = IoTPConfig_create(config, NULL);
	if( rc ){
		syslog(LOG_ERR, "ERR: Failed to initiate config: %d", rc);
		return rc;
	}
	rc = IoTPConfig_setProperty(*config, IoTPConfig_identity_orgId, organisationId);
	if( rc ){
		syslog(LOG_ERR, "ERR: Failed to set organisation ID: %d", rc);
		IoTPConfig_clear(config);
		return rc;
	}
	rc = IoTPConfig_setProperty(*config, IoTPConfig_identity_typeId, typeId);
	if( rc ){
		syslog(LOG_ERR, "ERR: Failed to set type ID: %d", rc);
		IoTPConfig_clear(config);
		return rc;
	}
	rc = IoTPConfig_setProperty(*config, IoTPConfig_identity_deviceId, deviceId);
	if( rc ){
		syslog(LOG_ERR, "ERR: Failed to set device ID: %d", rc);
		IoTPConfig_clear(config);
		return rc;
	}
	rc = IoTPConfig_setProperty(*config, IoTPConfig_auth_token, token);
	if( rc ){
		syslog(LOG_ERR, "ERR: Failed to set token: %d", rc);
		IoTPConfig_clear(config);
		return rc;
	}
	return 0;
}

int initiate_device(IoTPConfig *config, IoTPDevice **device)
{
	int rc = 0;
	rc = IoTPDevice_create(device, config);
	if( rc ){
		syslog(LOG_ERR, "ERR: Failed to initiate device %d", rc);
		return rc;
	}
	rc = IoTPDevice_connect(*device);
	if( rc ){
		syslog(LOG_ERR, "ERR: Failed to connect to Watsion IoT platform: %d", rc);
		IoTPDevice_destroy(device);
		return rc;
	}
	return 0;
}

int send_data_to_server(char data[], IoTPDevice *device){
	IOTPRC rc;
	rc = IoTPDevice_sendEvent(device,"status", data, "json", QoS0, NULL);
	return rc;
}

void LogCallback (int level, char * message)
{
    if ( level > 0 )
        syslog(LOG_INFO, "%s", message? message:"NULL");
}

int handle_server_error(int error, IoTPDevice **device, IoTPConfig **config,
	char *organisationId, char *deviceId, char *typeId, char *token)
{
	int rc = 0;
	switch (error)
	{
		case IOTPRC_INVALID_HANDLE:
			if(device)
				IoTPDevice_destroy(device);
			if(config)
				IoTPConfig_clear(config);
			rc = initiate_config(organisationId, deviceId, 
						typeId, token, config);
			if( rc ){
				syslog(LOG_ERR, "Failed to set up config: %d", rc);
				break;
			}
			rc = initiate_device(*config, device);
			if( rc ){
				syslog(LOG_ERR, "Failed to set up device: %d", rc);
				break;
			}
			return 0;
		case IOTPRC_NOT_CONNECTED:
			rc = IoTPDevice_connect(device);
			if(rc)
				break;
			return 0;
		default:
			rc = 1;
	}
	syslog(LOG_ERR, "Failed to handle error: %d", error);
	return rc;
}