#include <iotp_device.h>
#include <syslog.h>

int send_data_to_server(char data[], IoTPDevice *device);
void LogCallback (int level, char * message);
int initiate_device(IoTPConfig *config, IoTPDevice **device);
int initiate_config(char *organisationId, char *deviceId, char *typeId, char *token,
                    IoTPConfig **config);
int handle_server_error(int error, IoTPDevice **device, IoTPConfig **config,
	char *organisationId, char *deviceId, char *typeId, char *token);
