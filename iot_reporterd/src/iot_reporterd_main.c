#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <argp.h>
#include <stdlib.h>
#include "iot_reporterd_ubus.h"
#include "iot_reporterd_server.h"


volatile sig_atomic_t deamonize = 1;

const char *argp_program_version =  "iot_reporterd 1.0.0";
const char *argp_program_bug_address =  "<bug-gnu-utils@gnu.org>";
static char doc[] = "iot reporter -- sends messages to IBM watson cloud";

static struct argp_option options[] = {
  {"time",  't', "time", 0, "Time between messages" },
  {"organisationid", 'o', "organisationId", 0, "Organisation ID"},
  {"deviceid", 'd', "deviceId", 0, "Device ID"},
  {"typeId", 'y', "typeId", 0, "Type ID"},
  {"token", 'k', "token", 0, "Token"},
  { 0 }
};

struct arguments{
	int time;
	char *organisationId;
	char *deviceId;
	char *typeId;
	char *token;
};



static error_t parse_opt (int key, char *arg, struct argp_state *state)
{
	struct arguments *arguments = state->input;
	
	switch ( key ){
		
		case 't':
			arguments->time = atoi(arg);
			break;
		case 'o':
			arguments->organisationId = arg;
			break;
		case 'd':
			arguments->deviceId = arg;
			break;
		case 'y':
			arguments->typeId = arg;
			break;
		case 'k':
			arguments->token = arg;
			break;
		default:
			return ARGP_ERR_UNKNOWN;
			
	}
	return 0;
}

static struct argp argp = { options, parse_opt, 0, doc };

static void term_proc(int sigterm) 
{
	deamonize = 0;
}

int main(int argc, char **argv)
{
	IoTPConfig *config = NULL;
	IoTPDevice *device = NULL;
	int rc = 0;
	struct ubus_context *ctx;
	char buff[400];
	uint32_t id;
	struct sigaction action;
	memset(&action, 0, sizeof(struct sigaction));
	action.sa_handler = term_proc;
	sigaction(SIGTERM, &action, NULL);
	openlog("iot_reporterd", LOG_PID, LOG_USER);
	struct arguments arguments;
	argp_parse(&argp, argc, argv, 0, 0, &arguments);
	ctx = ubus_connect(NULL);
	if( !ctx ){
		syslog(LOG_ERR, "Couldn't connect to ubus");
		goto end;
	}
	rc = ubus_lookup_id(ctx, "system", &id);
	if ( rc ) {
        syslog(LOG_ERR, "Failed to look up the object\n");
        goto end;
    }
	rc = initiate_config(arguments.organisationId, arguments.deviceId, 
						arguments.typeId, arguments.token, &config);
	if( rc ){
		syslog(LOG_ERR, "Failed to set up config: %d", rc);
		goto end;
	}
	rc = initiate_device(config, &device);
	if( rc ){
		syslog(LOG_ERR, "Failed to set up device: %d", rc);
		goto end;
	}
	syslog(LOG_INFO, "Device connected successfully");
	int error_count = 0;
	while( deamonize && error_count < 5) {
		rc = ubus_invoke(ctx, id, "info", NULL, read_memory_info, &buff, 0);
		if( rc ){
			syslog(LOG_ERR, "couldn't invoke ubus method: %d", rc);
			handle_ubus_error(rc, ctx, &id);
			error_count++;
			continue;
		}
		rc = send_data_to_server(buff, device);
		if( rc ){
			syslog(LOG_ERR, "couldn't send data to the server: %d", rc);
			handle_server_error(rc,  &device, &config, arguments.organisationId, 
						arguments.deviceId, arguments.typeId, arguments.token);
			error_count++;
			continue;
		}
		syslog(LOG_INFO, "Data sent to the server");
		error_count = 0;
		sleep(arguments.time);
	}
end:
	if(device){
		IoTPDevice_disconnect(device);
		IoTPDevice_destroy(device);
	}
	if(config)
		IoTPConfig_clear(config);
	if(ctx)
		ubus_free(ctx);
	syslog(LOG_INFO, "iot_reporterd was stopped");
	closelog();
	return rc;
}






