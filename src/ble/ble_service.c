#include <zephyr/types.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zephyr/sys/check.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>
#include "ble_service.h"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(bt_sevice,LOG_LEVEL_DBG);


static uint8_t service_blsc;
static sys_slist_t service_cbs = SYS_SLIST_STATIC_INIT(&service_cbs);

static void service_ccc_cfg_changed(const struct bt_gatt_attr *attr, uint16_t value){
	ARG_UNUSED(attr);

	struct bt_service_cb *listener;

	bool notif_enabled = (value == BT_GATT_CCC_NOTIFY);

	LOG_INF("Service notifications %s", notif_enabled ? "enabled" : "disabled");

	SYS_SLIST_FOR_EACH_CONTAINER(&service_cbs, listener, _node) {
		if (listener->ntf_changed) {
			listener->ntf_changed(notif_enabled);
		}
	}
}

static ssize_t read_blsc(struct bt_conn *conn, const struct bt_gatt_attr *attr,void *buf, uint16_t len, uint16_t offset){
	return bt_gatt_attr_read(conn, attr, buf, len, offset, &service_blsc,sizeof(service_blsc));
}

static ssize_t ctrl_point_write(struct bt_conn *conn, const struct bt_gatt_attr *attr,const void *buf, uint16_t len, uint16_t offset, uint8_t flags){
	int err = -ENOTSUP;
	struct bt_service_cb *listener;

	LOG_INF("CTRL Point Written %d", len);

	SYS_SLIST_FOR_EACH_CONTAINER(&service_cbs, listener, _node) {
		if (listener->write_val) {
			const uint8_t *in_buf = buf;
			err = listener->write_val(in_buf,len);
			/* If we get an error other than ENOTSUP then immediately
			 * break the loop and return a generic gatt error, assuming this
			 * listener supports this request code, but failed to serve it
			 */
			if ((err != 0) && (err != -ENOTSUP)) {
				return BT_GATT_ERR(BT_ATT_ERR_UNLIKELY);
			}
		}
	}
	if (err) {
		return BT_GATT_ERR(-1);
	} else {
		return len;
	}
}

/* Heart Rate Service Declaration */
BT_GATT_SERVICE_DEFINE(my_svc,
	BT_GATT_PRIMARY_SERVICE(SERVICE_UUID),
	BT_GATT_CHARACTERISTIC(CHARACTERISTIC_UUID, 
                BT_GATT_CHRC_WRITE | BT_GATT_CHRC_READ|BT_GATT_CHRC_NOTIFY,
			    BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
                read_blsc,ctrl_point_write, NULL),
	BT_GATT_CCC(service_ccc_cfg_changed,
		        BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
);

static int bt_service_init(void){
	service_blsc = 0x01;
	return 0;
}

int bt_service_cb_register(struct bt_service_cb *cb){
    CHECKIF(cb == NULL) {
		return -EINVAL;
	}
	sys_slist_append(&service_cbs, &cb->_node);
	return 0;
}

int bt_service_cb_unregister(struct bt_service_cb *cb){
    CHECKIF(cb == NULL) {
		return -EINVAL;
	}
	if (!sys_slist_find_and_remove(&service_cbs, &cb->_node)) {
		return -ENOENT;
	}
	return 0;
}

int bt_service_notify(uint16_t heartrate){
    int rc;
	static uint8_t hrm[2];

	hrm[0] = 0x06; /* uint8, sensor contact */
	hrm[1] = heartrate;

	rc = bt_gatt_notify(NULL, &my_svc.attrs[1], &hrm, sizeof(hrm));

	return rc == -ENOTCONN ? 0 : rc;
}


SYS_INIT(bt_service_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);


