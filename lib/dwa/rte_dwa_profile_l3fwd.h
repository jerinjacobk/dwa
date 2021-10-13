/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(C) 2021 Marvell.
 */

#ifndef RTE_DWA_PROFILE_L3FWD_H
#define RTE_DWA_PROFILE_L3FWD_H

/**
 * @file
 *
 * L3FWD Profile
 *
 * \code{.c}
 *                           +-------------->--[1]--------------+
 *                           |                                  |
 *               +-----------|----------+                       |
 *               |           |          |                       |
 *               |  +--------|-------+  |                       |
 *               |  |                |  |                       |
 *               |  | L3FWD Profile  |  |                       |
 *    \          |  |                |  |                       |
 *<====\========>|  +----------------+  |                       |
 *  DWA \Port0   |     Lookup Table     |             +---------|----------+
 *       \       |  +----------------+  |             | DPDK DWA|Device[0] |
 *        \      |  | IP    | Dport  |  |  Host Port  | +-------|--------+ |
 *         \     |  +----------------+  |<===========>| |       |        | |
 *          +~[3]~~~|~~~~~~~|~~~~~~~~|~~~~~~~~~~~~~~~~~>|->L3FWD Profile | |
 *<=============>|  +----------------+  |             | |                | |
 *  DWA Port1    |  |       |        |  | Control Port| +-|---------|----+ |
 *               |  +----------------+  |<===========>|   |         |      |
 *  ~~~>~~[5]~~~~|~~|~~~+   |        |  |             +---|---------|------+
 *               |  +---+------------+  |                 |         |
 *  ~~~<~~~~~~~~~|~~|~~~+   |        |<-|------[2]--------+         |
 *               |  +----------------+<-|------[4]------------------+
 *               |    Dataplane         |
 *<=============>|    Workload          |
 *  DWA PortN    |    Accelerator       |
 *               |    (HW/FW/SW)        |
 *               +----------------------+
 * \endcode
 *
 * L3FWD profile offloads Layer-3 forwarding between the DWA Ethernet ports.
 *
 * The above diagram depicts the profile and application programming sequence.
 *
 * -# DWA device attaches the L3FWD profile using rte_dwa_dev_attach().
 * -# Configure the L3FWD profile:
 *    - The application requests L3FWD profile capabilities of the DWA
 *      by using RTE_DWA_STAG_PROFILE_L3FWD_H2D_INFO, On response, the
 *      RTE_DWA_STAG_PROFILE_L3FWD_D2H_INFO returns the lookup modes supported,
 *      max rules supported, and available host ports for this profile.
 *    - The application configures a set of DWA ports to use a
 *      lookup mode(EM, LPM, or FIB) via RTE_DWA_STAG_PROFILE_L3FWD_H2D_CONFIG.
 *    - The application configures a valid host port to receive exception
 *      packets.
 * -# The exception that is not matching forwarding table entry comes as
 * RTE_DWA_STAG_PROFILE_L3FWD_D2H_EXCEPTION_PACKETS TLV to host. DWA stores the
 * exception packet send back destination ports after completing step (4).
 * -# Parse the exception packet and add rules to the FWD table using
 * RTE_DWA_STAG_PROFILE_L3FWD_H2D_LOOKUP_ADD. If the application knows the rules
 * beforehand, it can add the rules in step 2.
 * -# When DWA ports receive the matching flows in the lookup table, DWA
 *  forwards to DWA Ethernet ports without host CPU intervention.
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <rte_common.h>

/**
 * Enumerates L3FWD profile lookup modes.
 */
enum rte_dwa_profile_l3fwd_lookup_mode {
	RTE_DWA_PROFILE_L3FWD_MODE_EM = 1U << 0, /**< Exact match mode. */
	RTE_DWA_PROFILE_L3FWD_MODE_LPM = 1U << 1,
	/**< Longest prefix match mode. */
	RTE_DWA_PROFILE_L3FWD_MODE_FIB = 1U << 2,
	/**< Forwarding information base mode. */
	RTE_DWA_PROFILE_L3FWD_MODE_MAX = 1U << 15, /**< Max modes. */
};

/**
 * Payload of RTE_DWA_STAG_PROFILE_L3FWD_D2H_INFO message.
 */
struct rte_dwa_profile_l3fwd_d2h_info {
	uint32_t max_lookup_rules;
	/**< Maximum Supported lookup rules. */
	uint16_t modes_supported;
	/**< Each bit enabled represents a mode supported in
	 * enum rte_dwa_profile_l3fwd_lookup_mode
	 */
	uint16_t nb_host_ports;
	/**< Number of host ports in the host_ports. */
	uint16_t host_ports[];
	/**< Array of available host port of type enum rte_dwa_tag_port_host
	 * of size nb_host_ports.
	 */
} __rte_packed;

/**
 * Payload of RTE_DWA_STAG_PROFILE_L3FWD_H2D_CONFIG message.
 */
struct rte_dwa_profile_l3fwd_h2d_config {
	uint16_t mode;
	/**< L3FWD profile mode.
	 * @see enum rte_dwa_profile_l3fwd_modes
	 */
	uint16_t nb_eth_ports;
	/**< Number of DWA ethernet ports in the eth_ports list. */
	uint16_t eth_ports[];
	/**< List of DWA ethernet ports to apply the profile on.*/
} __rte_packed;

/** L3FWD profile IPv4 rule attributes. */
struct rte_dwa_profile_l3fwd_v4_5tpl {
	uint32_t ip_dst;
	/**< IPv4 destination address. */
	uint32_t ip_src;
	/**< IPv4 source address. */
	uint16_t port_dst;
	/**< Destination port. */
	uint16_t port_src;
	/**< Source port. */
	uint8_t proto;
	/**< Protocol. */
} __rte_packed;

/** L3FWD profile IPv4 rule prefix. */
struct rte_dwa_profile_l3fwd_v4_prefix {
	uint32_t ip_dst;
	/**< IPv4 destination address. */
	uint8_t depth;
	/**< LPM depth. */
} __rte_packed;

/** L3FWD profile IPv4 rule. */
struct rte_dwa_profile_l3fwd_v4_rule {
	RTE_STD_C11
	union {
		struct rte_dwa_profile_l3fwd_v4_5tpl match;
		/**< Match data. */
		struct rte_dwa_profile_l3fwd_v4_prefix prefix;
		/**< Prefix data. */
	};
} __rte_packed;

/* IPv6 rule attributes */

/** L3FWD profile IPV6 address length */
#define RTE_DWA_PROFILE_L3FWD_IPV6_ADDR_LEN 16

/** L3FWD profile IPv6 rule attributes. */
struct rte_dwa_profile_l3fwd_v6_5tpl {
	uint8_t ip_dst[RTE_DWA_PROFILE_L3FWD_IPV6_ADDR_LEN];
	/**< IPv6 destination address. */
	uint8_t ip_src[RTE_DWA_PROFILE_L3FWD_IPV6_ADDR_LEN];
	/**< IPv6 source address. */
	uint16_t port_dst;
	/**< Destination port. */
	uint16_t port_src;
	/**< Source port. */
	uint8_t proto;
	/**< Protocol. */
} __rte_packed;

/** L3FWD profile IPv6 rule prefix. */
struct rte_dwa_profile_l3fwd_v6_prefix {
	uint8_t ip_dst[RTE_DWA_PROFILE_L3FWD_IPV6_ADDR_LEN];
	/**< IPv6 destination address. */
	uint8_t depth;
	/**< LPM depth. */
} __rte_packed;

/** L3FWD profile IPv6 rule. */
struct rte_dwa_profile_l3fwd_v6_rule {
	RTE_STD_C11
	union {
		struct rte_dwa_profile_l3fwd_v6_5tpl match;
		/**< Match data. */
		struct rte_dwa_profile_l3fwd_v6_prefix prefix;
		/**< Prefix data. */
	};
} __rte_packed;

/** L3FWD profile rule type. */
enum rte_dwa_profile_l3fwd_rule_type {
	RTE_DWA_PROFILE_L3FWD_RULE_TYPE_IPV4 = 1U << 0,
	/**< Rule type IPv4. */
	RTE_DWA_PROFILE_L3FWD_RULE_TYPE_IPV6 = 1U << 1,
	/**< Rule type IPv6. */
};

/**
 * Payload of RTE_DWA_STAG_PROFILE_L3FWD_H2D_LOOKUP_ADD message.
 */
struct rte_dwa_profile_l3fwd_h2d_lookup_add {
	enum rte_dwa_profile_l3fwd_rule_type rule_type;
	/**< Rule type that is being added. */
	struct rte_dwa_profile_l3fwd_v4_rule v4_rule;
	/**< IPv4 rule. */
	struct rte_dwa_profile_l3fwd_v6_rule v6_rule;
	/**< IPv6 rule. */
	uint16_t eth_port_dst;
	/**< Destination lookup port. */
} __rte_packed;

/**
 * Payload of RTE_DWA_STAG_PROFILE_L3FWD_D2H_LOOKUP_ADD message.
 */
struct rte_dwa_profile_l3fwd_d2h_lookup_add {
	uint64_t handle; /**< Lookup rule handle. */
} __rte_packed;

/**
 * Payload of RTE_DWA_STAG_PROFILE_L3FWD_H2D_LOOKUP_UPDATE message.
 */
struct rte_dwa_profile_l3fwd_h2d_lookup_update {
	uint64_t handle;
	/**< Rule handle to update a rule.
	 * @see rte_dwa_profile_l3fwd_d2h_lookup_add
	 */
	uint16_t eth_port_dst;
	/**< Destination lookup port to update. */
} __rte_packed;

/**
 * Payload of RTE_DWA_STAG_PROFILE_L3FWD_H2D_LOOKUP_DEL message.
 */
struct rte_dwa_profile_l3fwd_h2d_lookup_delete {
	uint64_t handle;
	/**< Rule handle to delete a rule.
	 * @see rte_dwa_profile_l3fwd_d2h_lookup_add
	 */
} __rte_packed;

/**
 * Payload of RTE_DWA_STAG_PROFILE_L3FWD_D2H_EXCEPTION_PACKETS message.
 */
struct rte_dwa_profile_l3fwd_d2h_exception_pkts {
	uint16_t nb_pkts;
	/**< Number of packets in the variable size array.*/
	uint16_t rsvd16;
	/**< Reserved field to make pkts[0] to be 64bit aligned.*/
	uint32_t rsvd32;
	/**< Reserved field to make pkts[0] to be 64bit aligned.*/
	struct rte_mbuf *pkts[0];
	/**< Array of rte_mbufs of size nb_pkts. */
} __rte_packed;

/**
 * Enumerates the stag list for RTE_DWA_TAG_PROFILE_L3FWD tag.
 *
 */
enum rte_dwa_profile_l3fwd {
	/**
	 * Attribute |  Value
	 * ----------|--------
	 * Tag       | RTE_DWA_TAG_PROFILE_L3FWD
	 * Stag      | RTE_DWA_STAG_PROFILE_L3FWD_H2D_INFO
	 * Direction | H2D
	 * Type      | TYPE_ATTACHED
	 * Payload   | NA
	 * Pair TLV  | RTE_DWA_STAG_PROFILE_L3FWD_D2H_INFO
	 *
	 * Request to L3FWD profile information.
	 */
	RTE_DWA_STAG_PROFILE_L3FWD_H2D_INFO,
	/**
	 * Attribute |  Value
	 * ----------|--------
	 * Tag       | RTE_DWA_TAG_PROFILE_L3FWD
	 * Stag      | RTE_DWA_STAG_PROFILE_L3FWD_D2H_INFO
	 * Direction | D2H
	 * Type      | TYPE_ATTACHED
	 * Payload   | struct rte_dwa_profile_l3fwd_d2h_info
	 * Pair TLV  | RTE_DWA_STAG_PROFILE_L3FWD_D2H_INFO
	 *
	 * Response for L3FWD profile information.
	 */
	RTE_DWA_STAG_PROFILE_L3FWD_D2H_INFO,
	/**
	 * Attribute |  Value
	 * ----------|--------
	 * Tag       | RTE_DWA_TAG_PROFILE_L3FWD
	 * Stag      | RTE_DWA_STAG_PROFILE_L3FWD_H2D_CONFIG
	 * Direction | H2D
	 * Type      | TYPE_STOPPED
	 * Payload   | struct rte_dwa_profile_l3fwd_h2d_config
	 * Pair TLV  | RTE_DWA_STAG_COMMON_D2H_SUCCESS
	 * ^         | RTE_DWA_STAG_COMMON_D2H_ERR
	 *
	 * Request to configure L3FWD profile.
	 */
	RTE_DWA_STAG_PROFILE_L3FWD_H2D_CONFIG,
	/**
	 * Attribute |  Value
	 * ----------|--------
	 * Tag       | RTE_DWA_TAG_PROFILE_L3FWD
	 * Stag      | RTE_DWA_STAG_PROFILE_L3FWD_H2D_LOOKUP_ADD
	 * Direction | H2D
	 * Type      | TYPE_STOPPED
	 * ^         | TYPE_STARTED
	 * Payload   | struct rte_dwa_profile_l3fwd_h2d_lookup_add
	 * Pair TLV  | RTE_DWA_STAG_PROFILE_L3FWD_D2H_LOOKUP_ADD
	 *
	 * Request to add rule in L3FWD profile.
	 */
	RTE_DWA_STAG_PROFILE_L3FWD_H2D_LOOKUP_ADD,
	/**
	 * Attribute |  Value
	 * ----------|--------
	 * Tag       | RTE_DWA_TAG_PROFILE_L3FWD
	 * Stag      | RTE_DWA_STAG_PROFILE_L3FWD_D2H_LOOKUP_ADD
	 * Direction | D2H
	 * Type      | TYPE_STOPPED
	 * ^         | TYPE_STARTED
	 * Payload   | struct rte_dwa_profile_l3fwd_d2h_lookup_add
	 * Pair TLV  | RTE_DWA_STAG_PROFILE_L3FWD_H2D_LOOKUP_ADD
	 *
	 * Response for L3FWD profile rule add.
	 * It contains the handle for further operation on this rule.
	 */
	RTE_DWA_STAG_PROFILE_L3FWD_D2H_LOOKUP_ADD,
	/**
	 * Attribute |  Value
	 * ----------|--------
	 * Tag       | RTE_DWA_TAG_PROFILE_L3FWD
	 * Stag      | RTE_DWA_STAG_PROFILE_L3FWD_H2D_LOOKUP_UPDATE
	 * Direction | H2D
	 * Type      | TYPE_STOPPED
	 * ^         | TYPE_STARTED
	 * Payload   | struct rte_dwa_profile_l3fwd_h2d_lookup_update
	 * Pair TLV  | RTE_DWA_STAG_COMMON_D2H_SUCCESS
	 * ^         | RTE_DWA_STAG_COMMON_D2H_ERR
	 *
	 * Request to update the rule in L3FWD profile.
	 */
	RTE_DWA_STAG_PROFILE_L3FWD_H2D_LOOKUP_UPDATE,
	/**
	 * Attribute |  Value
	 * ----------|--------
	 * Tag       | RTE_DWA_TAG_PROFILE_L3FWD
	 * Stag      | RTE_DWA_STAG_PROFILE_L3FWD_H2D_LOOKUP_DEL
	 * Direction | H2D
	 * Type      | TYPE_STOPPED
	 * ^         | TYPE_STARTED
	 * Payload   | struct rte_dwa_profile_l3fwd_h2d_lookup_delete
	 * Pair TLV  | RTE_DWA_STAG_COMMON_D2H_SUCCESS
	 * ^         | RTE_DWA_STAG_COMMON_D2H_ERR
	 *
	 * Request to delete the rule in L3FWD profile.
	 */
	RTE_DWA_STAG_PROFILE_L3FWD_H2D_LOOKUP_DEL,
	/**
	 * Attribute |  Value
	 * ----------|--------
	 * Tag       | RTE_DWA_TAG_PROFILE_L3FWD
	 * Stag      | RTE_DWA_STAG_PROFILE_L3FWD_D2H_EXCEPTION_PACKETS
	 * Direction | D2H
	 * Type      | TYPE_USER_PLANE
	 * Payload   | struct rte_dwa_profile_l3fwd_d2h_exception_pkts
	 * Pair TLV  | NA
	 *
	 * Response from DWA of exception packets.
	 */
	RTE_DWA_STAG_PROFILE_L3FWD_D2H_EXECPTION_PACKETS,
	RTE_DWA_STAG_PROFILE_L3FWD_MAX = UINT16_MAX,
	/**< Max stags for RTE_DWA_TAG_PROFILE_L3FWD tag*/
};

#ifdef __cplusplus
}
#endif

#endif /* RTE_DWA_PROFILE_L3FWD_H */
