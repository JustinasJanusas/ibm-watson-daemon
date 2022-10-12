#include <iotp_device.h>
#include <syslog.h>

int send_data_to_server(char data[], IoTPDevice *device);
void LogCallback (int level, char * message);
int initiate_device(char* args[], IoTPConfig **config, IoTPDevice **device);

