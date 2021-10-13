/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(C) 2021 Marvell.
 */

#ifndef RTE_DWA_PORT_HOST_ETHERNET_H
#define RTE_DWA_PORT_HOST_ETHERNET_H

/**
 * @file
 *
 * @warning
 * @b EXPERIMENTAL:
 * All functions in this file may be changed or removed without prior notice.
 *
 * RTE API related to host ethernet based port.
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Payload of RTE_DWA_STAG_PORT_HOST_ETHERNET_D2H_INFO message.
 */
struct rte_dwa_port_host_ethernet_d2h_info {
	uint16_t nb_rx_queues; /**< Number of Rx queues available */
	uint16_t nb_tx_queues; /**< Number of Tx queues available */
} __rte_packed;

/**
 * Payload of RTE_DWA_STAG_PORT_HOST_ETHERNET_H2D_CONFIG message.
 */
struct rte_dwa_port_host_ethernet_config {
	uint16_t nb_rx_queues; /**< Number of Rx queues to configure */
	uint16_t nb_tx_queues; /**< Number of Tx queues to configure */
	uint16_t max_burst; /**< Max burst size */
	RTE_STD_C11
	union {
		struct rte_mempool *pkt_pool;
		/**< Packet pool to allocate packets */
		uint64_t pkt_pool_u64;
		/**< uint64_t representation of packet pool */
	};
	RTE_STD_C11
	union {
		struct rte_mempool *tlv_pool;
		/**< TLV pool to allocate TLVs */
		uint64_t tlv_pool_u64;
		/**< uint64_t representation of TLV pool */
	};
} __rte_packed;

/**
 * Payload of RTE_DWA_STAG_PORT_HOST_ETHERNET_H2D_QUEUE_CONFIG message.
 */
struct rte_dwa_port_host_ethernet_queue_config {
	uint16_t id;
	uint8_t enable;
	uint8_t is_tx;
	uint16_t depth;
} __rte_packed;

/**
 * Enumerates the stag list for RTE_DWA_TAG_PORT_HOST_ETHERNET tag.
 */
enum rte_dwa_port_host_ethernet {
	/**
	 * Attribute |  Value
	 * ----------|--------
	 * Tag       | RTE_DWA_TAG_PORT_HOST_ETHERNET
	 * Stag      | RTE_DWA_STAG_PORT_HOST_ETHERNET_H2D_INFO
	 * Direction | H2D
	 * Type      | TYPE_ATTACHED
	 * Payload   | NA
	 * Pair TLV  | RTE_DWA_STAG_PORT_HOST_ETHERNET_D2H_INFO
	 *
	 * Request DWA host ethernet port information.
	 */
	RTE_DWA_STAG_PORT_HOST_ETHERNET_H2D_INFO,
	/**
	 * Attribute |  Value
	 * ----------|---------
	 * Tag       | RTE_DWA_TAG_PORT_HOST_ETHERNET
	 * Stag      | RTE_DWA_STAG_PORT_HOST_ETHERNET_D2H_INFO
	 * Direction | H2D
	 * Type      | TYPE_ATTACHED
	 * Payload   | struct rte_dwa_port_host_ethernet_d2h_info
	 * Pair TLV  | RTE_DWA_STAG_PORT_HOST_ETHERNET_H2D_INFO
	 *
	 * Response for DWA host ethernet port information.
	 */
	RTE_DWA_STAG_PORT_HOST_ETHERNET_D2H_INFO,
	/**
	 * Attribute |  Value
	 * ----------|---------
	 * Tag       | RTE_DWA_TAG_PORT_HOST_ETHERNET
	 * Stag      | RTE_DWA_STAG_PORT_HOST_ETHERNET_H2D_CONFIG
	 * Direction | H2D
	 * Type      | TYPE_STOPPED
	 * Payload   | struct rte_dwa_port_host_ethernet_config
	 * Pair TLV  | RTE_DWA_STAG_COMMON_D2H_SUCCESS
	 * ^         | RTE_DWA_STAG_COMMON_D2H_ERR
	 *
	 * Request DWA host ethernet port configuration.
	 */
	RTE_DWA_STAG_PORT_HOST_ETHERNET_H2D_CONFIG,
	/**
	 * Attribute |  Value
	 * ----------|---------
	 * Tag       | RTE_DWA_TAG_PORT_HOST_ETHERNET
	 * Stag      | RTE_DWA_STAG_PORT_HOST_ETHERNET_H2D_QUEUE_CONFIG
	 * Direction | H2D
	 * Type      | TYPE_STOPPED
	 * Payload   | struct rte_dwa_port_host_ethernet_queue_config
	 * Pair TLV  | RTE_DWA_STAG_COMMON_D2H_SUCCESS
	 * ^         | RTE_DWA_STAG_COMMON_D2H_ERR
	 *
	 * Request DWA host ethernet port queue configuration.
	 *
	 * @note RTE_DWA_STAG_PORT_HOST_ETHERNET_H2D_CONFIG must be called
	 * before invoking this message.
	 */
	RTE_DWA_STAG_PORT_HOST_ETHERNET_H2D_QUEUE_CONFIG,
	RTE_DWA_STAG_PORT_HOST_ETHERNET_MAX = UINT16_MAX,
	/**< Max stags for RTE_DWA_TAG_PORT_HOST_ETHERNET tag*/
};

/**
 * Transmit a burst of TLVs of type `TYPE_USER_PLANE` on the Tx queue
 * designated by its *queue_id* of DWA object *obj*.
 *
 * @param obj
 *   DWA object.
 * @param queue_id
 *   The identifier of Tx queue id. The queue id should in the range of
 *   [0 to rte_dwa_port_host_ethernet_config::nb_tx_queues].
 * @param[out] tlvs
 *   Points to an array of *nb_tlvs* tlvs of type *rte_dwa_tlv* structure
 *   to be transmitted.
 * @param nb_tlvs
 *   The maximum number of TLVs to transmit.
 *
 * @return
 * The number of TLVs actually transmitted on the Tx queue. The return
 * value can be less than the value of the *nb_tlvs* parameter when the
 * Tx queue is full.
 */
uint16_t rte_dwa_port_host_ethernet_tx(rte_dwa_obj_t obj, uint16_t queue_id,
			      struct rte_dwa_tlv **tlvs, uint16_t nb_tlvs);
/**
 * Receive a burst of TLVs of type `TYPE_USER_PLANE` from the Rx queue
 * designated by its *queue_id* of DWA object *obj*.
 *
 * @param obj
 *   DWA object.
 * @param queue_id
 *   The identifier of Rx queue id. The queue id should in the range of
 *   [0 to rte_dwa_port_host_ethernet_config::nb_rx_queues].
 * @param[out] tlvs
 *   Points to an array of *nb_tlvs* tlvs of type *rte_dwa_tlv* structure
 *   to be received.
 * @param nb_tlvs
 *   The maximum number of TLVs to received.
 *
 * @return
 * The number of TLVs actually received on the Rx queue. The return
 * value can be less than the value of the *nb_tlvs* parameter when the
 * Rx queue is not full.
 */
uint16_t rte_dwa_port_host_ethernet_rx(rte_dwa_obj_t obj, uint16_t queue_id,
			      struct rte_dwa_tlv **tlvs, uint16_t nb_tlvs);

#ifdef __cplusplus
}
#endif

#endif /* RTE_DWA_PORT_HOST_ETHERNET_H */
