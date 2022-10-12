map = Map("iot_reporterd");

section = map:section(NamedSection, "iot_reporterd_sct", "iot_reporterd", "Settings")

flag = section:option(Flag, "enable", "Enable", "Enable program")

time = section:option(Value, "time", "Time", "Time between messages is seconds")
time.datatype = "uinteger"

groupId = section:option(Value, "orgId", "Organisation ID")

typeId = section:option(Value, "typeId", "Type ID")

deviceId = section:option(Value, "deviceId", "Device ID")

token = section:option(Value, "token", "Device token")
token.datatype = "string"
token.maxlength = 20

return map