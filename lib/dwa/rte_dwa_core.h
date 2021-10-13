/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(C) 2021 Marvell.
 */

#ifndef RTE_DWA_CORE_H
#define RTE_DWA_CORE_H

/**
 * @file
 *
 * @warning
 * @b EXPERIMENTAL:
 * All functions in this file may be changed or removed without prior notice.
 *
 * RTE DWA core API
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include <rte_common.h>
#include <rte_memcpy.h>

/* Types */
typedef void *rte_dwa_obj_t; /**< DWA object type. */

/* Tag partitions */
/**
 * Enumerates different tag sections.
 */
enum rte_dwa_tag {
	RTE_DWA_TAG_COMMON, /**< Tags common for all profile and ports. */
	RTE_DWA_TAG_PORT_DWA_BASE = 0x400, /**< Base tag value for DWA port. */
	RTE_DWA_TAG_PORT_HOST_BASE = 0x800, /**< Base tag value for host port */
	RTE_DWA_TAG_VENDOR_EXTENSION = 0x1000,
	/**< Base tag value for vendor extension TLVs. */
	RTE_DWA_TAG_PROFILE_BASE = 0x3000, /**< Base tag value for profile. */
	RTE_DWA_TAG_MAX = UINT16_MAX, /* Max available tags space. */
};

/**
 * Enumerates DWA port types.
 */
enum rte_dwa_tag_port_dwa {
	RTE_DWA_TAG_PORT_DWA_ETHERNET = RTE_DWA_TAG_PORT_DWA_BASE,
	/**< Tag value for DWA ethernet port. */
};

/**
 * Enumerates host port types.
 */
enum rte_dwa_tag_port_host {
	RTE_DWA_TAG_PORT_HOST_ETHERNET = RTE_DWA_TAG_PORT_HOST_BASE,
	/**< Tag value for host ethernet port. */
};

/**
 * Enumerates profiles types.
 */
enum rte_dwa_tag_profile {
	RTE_DWA_TAG_PROFILE_ADMIN = RTE_DWA_TAG_PROFILE_BASE,
	/**< Tag value for admin profile. */
	RTE_DWA_TAG_PROFILE_L3FWD,
	/**< Tag value for l3fwd profile. */
};

/* Common sub tags */
#define RTE_DWA_ERROR_STR_LEN_MAX 128 /**< Max error string length.*/

/**
 * Payload of RTE_DWA_STAG_COMMON_D2H_ERR message.
 */
struct rte_dwa_common_d2h_err {
	int32_t dwa_errno; /**< Error number of the failure */
	uint8_t has_reason_str; /**< If set, valid string in reason */
	char reason[RTE_DWA_ERROR_STR_LEN_MAX]; /**< Failure reason as string */
} __rte_packed;

/**
 * Enumerates the stag list for RTE_DWA_TAG_COMMON tag.
 */
enum rte_dwa_stag_common {
	/**
	 * Attribute |  Value
	 * ----------|--------
	 * Tag       | RTE_DWA_TAG_COMMON
	 * Stag      | RTE_DWA_STAG_COMMON_D2H_SUCCESS
	 * Direction | D2H
	 * Type      | TYPE_ATTACHED
	 * Payload   | NA
	 * Pair TLV  | NA
	 *
	 * D2H response for successful TLV action.
	 */
	RTE_DWA_STAG_COMMON_D2H_SUCCESS,
	/**
	 * Attribute |  Value
	 * ----------|--------
	 * Tag       | RTE_DWA_TAG_COMMON
	 * Stag      | RTE_DWA_STAG_COMMON_D2H_ERR
	 * Direction | D2H
	 * Type      | TYPE_ATTACHED
	 * Payload   | struct rte_dwa_common_d2h_err
	 * Pair TLV  | NA
	 *
	 * D2H response for unsuccessful TLV action.
	 */
	RTE_DWA_STAG_COMMON_D2H_ERR,
	RTE_DWA_STAG_COMMON_MAX = UINT16_MAX, /**< Max stags for common tag.*/
};

/* TLV */
/** Macro to get TLV ID from tag and stag. */
#define RTE_DWA_TLV_ID(tag, stag) ((uint32_t)((uint32_t)(tag) << 16 | (stag)))
/** Macro to form TLV ID from tag and stag. */
#define RTE_DWA_TLV_MK_ID(tag, stag) \
	RTE_DWA_TLV_ID(RTE_DWA_TAG_ ## tag, RTE_DWA_STAG_ ## tag ##_ ## stag)

/** TLV representation of the DWA message */
struct rte_dwa_tlv {
	RTE_STD_C11
	union {
		uint32_t id; /**< ID as tag and stag tuple. */
		RTE_STD_C11
		union {
			uint16_t tag; /**< Tag. */
			uint16_t stag;/**< Sub Tag. */
		};
	};
	uint32_t len; /**< Length of payload. */
	char  msg[]; /**< Zero length array points to payload of TLV. */
} __rte_packed;

/** DWA TLV header size */
#define RTE_DWA_TLV_HDR_SZ offsetof(struct rte_dwa_tlv, msg)

/**
 * Fill DWA TLV.
 *
 * Fill the DWA TLV attributes from arguments.
 *
 * @param [out] tlv
 *   TLV pointer to fill.
 * @param id
 *   TLV ID value. @see RTE_DWA_TLV_MK_ID RTE_DWA_TLV_ID
 * @param len
 *   TLV payload length.
 * @param msg
 *   TLV payload message.
 */
static inline void
rte_dwa_tlv_fill(struct rte_dwa_tlv *tlv, uint32_t id, uint32_t len, void *msg)
{
	tlv->id = id;
	tlv->len = len;
	if (len)
		rte_memcpy(tlv->msg, msg, len);
}

/**
 * Get payload of a D2H TLV message.
 *
 * Get payload message pointer from the D2H TLV message.
 *
 * @param tlv
 *   TLV pointer.
 *
 * @return
 *   TLV payload on success, NULL otherwise.
 */
static inline void*
rte_dwa_tlv_d2h_to_msg(struct rte_dwa_tlv *tlv)
{
	if (tlv == NULL || tlv->id == RTE_DWA_TLV_MK_ID(COMMON, D2H_ERR))
		return NULL;
	else
		return tlv->msg;
}

/**
 * Get TLV name from ID.
 *
 * Get TLV name as string from ID.
 *
 * @param id
 *   TLV ID.
 *
 * @return
 *   TLV name string on success, NULL otherwise.
 */
char *rte_dwa_tlv_id_to_str(uint32_t id);

/**
 * Get TLV payload length.
 *
 * Get TLV payload length from the given TLV ID.
 *
 * @param id
 *   TLV ID.
 *
 * @return
 *   >=0 On success, <0 on invalid ID.
 */
int32_t rte_dwa_tlv_len(uint32_t id);

/* Control plane operation */

/**
 * Execute a control plane operation on DWA.
 *
 * @param obj
 *   DWA object.
 *
 * @param h2d
 *   H2D direction TLV to execute a control plane operation on DWA.
 *
 * @return
 *   D2H TLV response on success, NULL otherwise.
 *   Implementation allocates the memory for the response using malloc(),
 *   Application must free this memory using free() API after use.
 */
struct rte_dwa_tlv *rte_dwa_ctrl_op(rte_dwa_obj_t obj, struct rte_dwa_tlv *h2d);

/* State management */

/**
 * Move DWA device to `RUNNING` state.
 *
 * Move DWA device and its associated profiles to `RUNNING` state.
 * `TYPE_ATTACHED`, `TYPE_STARTED` and `TYPE_USER_PLANE` type messages are
 * valid in this state.
 *
 * @param obj
 *   DWA object.
 *
 * @return
 *   0 on success, error otherwise.
 */

int rte_dwa_start(rte_dwa_obj_t obj);

/**
 * Move DWA device to `STOPPED` state.
 *
 * Move DWA device and its associated profiles to `STOPPED` state.
 * `TYPE_ATTACHED`, `TYPE_STOPPED` type messages are valid in this state.
 *
 * @param obj
 *   DWA object.
 *
 * @return
 *   0 on success, error otherwise.
 */
int rte_dwa_stop(rte_dwa_obj_t obj);

#ifdef __cplusplus
}
#endif

#endif /* RTE_DWA_CORE_H */
