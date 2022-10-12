include $(TOPDIR)/rules.mk

PKG_NAME:=iot_reporterd
PKG_RELEASE:=6
PKG_VERSION:=1.1.1

include $(INCLUDE_DIR)/package.mk

define Package/iot_reporterd
	CATEGORY:=Base system
	TITLE:=iot_reporterd
	DEPENDS:=+argp-standalone +libubus +libubox +libblobmsg-json +libiot_c_device
endef

define Package/iot_reporterd/description
	This is an daemon which sends router information to IBM Watson cloud
endef

define Package/iot_reporterd/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/iot_reporterd $(1)/usr/bin
	$(INSTALL_BIN) ./files/iot_reporterd.init $(1)/etc/init.d/iot_reporterd
	$(INSTALL_CONF) ./files/iot_reporterd.config $(1)/etc/config/iot_reporterd
endef

$(eval $(call BuildPackage,iot_reporterd))
