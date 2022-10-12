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
  { 0 }
};

struct arguments{
	char* args[4];
	int time;
};


void term_proc(int sigterm);
static error_t parse_opt (int key, char *arg, struct argp_state *state);


static struct argp argp = { options, parse_opt, 0, doc };

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
	arguments.time = 1;
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
	 
	rc = initiate_device(arguments.args, &config, &device);
	if( rc ){
		syslog(LOG_ERR, "Failed to set up device");
		goto end;
	}
	syslog(LOG_INFO, "Device connected successfully");
	while( deamonize ) {
		rc = ubus_invoke(ctx, id, "info", NULL, read_memory_info, &buff, 0);
		if( rc ){
			syslog(LOG_ERR, "couldn't invoke ubus method");
			break;
		}
		printf("Buffer -> %s\n", buff);
		rc = send_data_to_server(buff, device);
		if( rc ){
			syslog(LOG_ERR, "couldn't send data to the server: %d", rc);
			break;
		}
		syslog(LOG_INFO, "Data sent to the server");
		sleep(arguments.time);
	}
	
end:
	syslog(LOG_INFO, "iot_reporterd was stopped");
	IoTPDevice_destroy(device);
	IoTPConfig_clear(config);
	ubus_free(ctx);
	closelog();
	return rc;
}

void term_proc(int sigterm) 
{
	deamonize = 0;
}



static error_t parse_opt (int key, char *arg, struct argp_state *state)
{
	struct arguments *arguments = state->input;
	
	switch ( key ){
		
		case 't':
			arguments->time = atoi(arg);
			break;
		case ARGP_KEY_ARG:
			if (state->arg_num > 3)
				argp_usage (state);
			arguments->args[state->arg_num] = arg;
			break;
		case ARGP_KEY_END:
			if (state->arg_num < 4)
			argp_usage (state);
			break;
		default:
			return ARGP_ERR_UNKNOWN;
			
	}
	return 0;
}

