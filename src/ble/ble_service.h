#ifndef BLE_SERVICE_H_
#define BLE_SERVICE_H_

#include <stdint.h>

#include <zephyr/sys/slist.h>


#define SERVICE_UUID   BT_UUID_DECLARE_128(BT_UUID_128_ENCODE(0x17DFAF86,0x8765,0x4A44,0xB6B8,0xC838F7FA5620))
/**
 *  @brief Characteristic UUID Value
 */
#define CHARACTERISTIC_UUID_VAL 0xFF01
/**
 *  @brief Characteristic Control Point
 */
#define CHARACTERISTIC_UUID \
	BT_UUID_DECLARE_16(CHARACTERISTIC_UUID_VAL)


#ifdef __cplusplus
extern "C" {
#endif

/** @brief service callback structure */
struct bt_service_cb {
	/** @brief Sevice notifications changed
	 *
	 * @param enabled Flag that is true if notifications were enabled, false
	 *                if they were disabled.
	 */
	void (*ntf_changed)(bool enabled);

	/**
	 * @brief service write callback
	 *
	 * @param request control point request code
	 *
	 * @return 0 on successful handling of control point request
	 * @return -ENOTSUP if not supported. It can be used to pass handling to other
	 *         listeners in case of multiple listeners
	 * @return other negative error codes will result in immediate error response
	 */
	int (*write_val)(const uint8_t *data,uint16_t len);

	/**
	 * @brief service read callback
	 * @param buff buffer
	 * @param len length of the buffer
	 * 
	 * @return 
	 */
	int (*read_val)(uint8_t *buff,uint16_t len);

	/** Internal member to form a list of callbacks */
	sys_snode_t _node;
};

/** @brief Heart rate service callback register
 *
 * This function will register callbacks that will be called in
 * certain events related to Heart rate service.
 *
 * @param cb Pointer to callbacks structure. Must point to memory that remains valid
 * until unregistered.
 *
 * @return 0 on success
 * @return -EINVAL in case @p cb is NULL
 */
int bt_service_cb_register(struct bt_service_cb *cb);

/** @brief Heart rate service callback unregister
 *
 * This function will unregister callback from Heart rate service.
 *
 * @param cb Pointer to callbacks structure
 *
 * @return 0 on success
 * @return -EINVAL in case @p cb is NULL
 * @return -ENOENT in case the @p cb was not found in registered callbacks
 */
int bt_service_cb_unregister(struct bt_service_cb *cb);

/** @brief Notify heart rate measurement.
 *
 * This will send a GATT notification to all current subscribers.
 *
 *  @param heartrate The heartrate measurement in beats per minute.
 *
 *  @return Zero in case of success and error code in case of error.
 */
int bt_service_notify(uint16_t heartrate);


#ifdef __cplusplus
}
#endif

#endif /* !BLE_SERVICE_H_ */
