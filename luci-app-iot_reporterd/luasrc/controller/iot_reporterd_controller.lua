module("luci.controller.iot_reporterd_controller", package.seeall)

function index()
    entry({"admin", "services", "iotservice"}, cbi("iot_reporterd_model"), "IoT service", 100)
end