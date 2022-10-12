BIN:=iot_reporterd
LIBS:=-largp -lubus -lubox -lblobmsg_json -liotp-as-device

.PHONY: all clean

all: $(BIN)

$(BIN): iot_reporterd_main.c iot_reporterd_ubus.c iot_reporterd_server.c
	$(CC) $^ -o $@ $(LIBS)

clean:
	rm -f $(BIN)
