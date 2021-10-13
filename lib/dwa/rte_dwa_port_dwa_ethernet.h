/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(C) 2021 Marvell.
 */

#ifndef RTE_DWA_PORT_DWA_ETHERNET_H
#define RTE_DWA_PORT_DWA_ETHERNET_H

/**
 * @file
 *
 * @warning
 * @b EXPERIMENTAL:
 * All functions in this file may be changed or removed without prior notice.
 *
 * RTE API related to DWA ethernet based port.
 *
 */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Payload of RTE_DWA_STAG_PORT_DWA_ETHERNET_D2H_INFO message.
 */
struct rte_dwa_port_dwa_ethernet_d2h_info {
	uint16_t nb_ports; /**< Number of available ports. */
	uint16_t avail_ports[]; /**< Array of available port of size nb_ports */
} __rte_packed;

/**
 * Enumerates the stag list for RTE_DWA_TAG_PORT_DWA_ETHERNET tag.
 */
enum rte_dwa_stag_port_dwa_ethernet {
	/**
	 * Attribute |  Value
	 * ----------|---------
	 * Tag       | RTE_DWA_TAG_PORT_DWA_ETHERNET
	 * Stag      | RTE_DWA_STAG_PORT_DWA_ETHERNET_H2D_INFO
	 * Direction | H2D
	 * Type      | TYPE_ATTACHED
	 * Payload   | NA
	 * Pair TLV  | RTE_DWA_STAG_PORT_DWA_ETHERNET_D2H_INFO
	 *
	 * Request DWA ethernet port information.
	 */
	RTE_DWA_STAG_PORT_DWA_ETHERNET_H2D_INFO,
	/**
	 * Attribute |  Value
	 * ----------|---------
	 * Tag       | RTE_DWA_TAG_PORT_DWA_ETHERNET
	 * Stag      | RTE_DWA_STAG_PORT_DWA_ETHERNET_D2H_INFO
	 * Direction | D2H
	 * Type      | TYPE_ATTACHED
	 * Payload   | struct rte_dwa_port_dwa_ethernet_d2h_info
	 * Pair TLV  | RTE_DWA_STAG_PORT_DWA_ETHERNET_H2D_INFO
	 *
	 * Response for DWA ethernet port information.
	 */
	RTE_DWA_STAG_PORT_DWA_ETHERNET_D2H_INFO,
	RTE_DWA_STAG_PORT_DWA_ETHERNET_MAX = UINT16_MAX,
	/**< Max stags for RTE_DWA_TAG_PORT_DWA_ETHERNET tag*/
};

#ifdef __cplusplus
}
#endif

#endif /* RTE_DWA_PORT_DWA_ETHERNET_H */
