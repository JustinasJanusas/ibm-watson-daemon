include $(TOPDIR)/rules.mk

PKG_NAME:=libiot_c_device
PKG_VERSION:=1.0.0
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/ibm-watson-iot/iot-c.git
PKG_SOURCE_VERSION:=f921dce616af815da2236954c608a2f377c928c5

include $(INCLUDE_DIR)/package.mk

define Package/libiot_c_device
	SECTION:=libs
	CATEGORY:=Libraries
	TITLE:=IBM iot library
	DEPENDS:= +openssl-util
endef

define Package/libiot_c_device/description
	Library for sending messages to IBM Watson cloud
endef


define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/src/wiotp/sdk/*.h $(1)/usr/include/ 
	$(CP) $(PKG_BUILD_DIR)/paho.mqtt.c/src/*.h $(1)/usr/include/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/build/libiotp-as-device.so.1.0 $(1)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/build/libiotp-as-device.so.1 $(1)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/build/libiotp-as-device.so $(1)/usr/lib/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/paho.mqtt.c/build/output/libpaho-mqtt3as.so.1 $(1)/usr/lib/
endef

define Package/libiot_c_device/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/paho.mqtt.c/build/output/libpaho-mqtt3as.so.1 $(1)/usr/lib/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/build/libiotp-as-device.so.1.0 $(1)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/build/libiotp-as-device.so.1 $(1)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/build/libiotp-as-device.so $(1)/usr/lib/
endef

$(eval $(call BuildPackage,libiot_c_device))
