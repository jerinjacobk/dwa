/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(C) 2021 Marvell.
 */

#ifndef RTE_DWA_PROFIE_ADMIN_H
#define RTE_DWA_PROFIE_ADMIN_H

/**
 * @file
 *
 * @warning
 * @b EXPERIMENTAL:
 * All functions in this file may be changed or removed without prior notice.
 *
 * RTE API related to admin profile which includes administrative functions
 * such as FW updates, resource partitioning in a DWA and items in global in
 * nature that is applicable for all DWA device under the DWA.
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <rte_uuid.h>

/**
 * Payload of RTE_DWA_STAG_PROFILE_ADMIN_H2D_ATTACH message.
 */
struct rte_dwa_profile_admin_h2d_attach {
	rte_uuid_t uuid; /**< uuid to validate the integrity of actor */
} __rte_packed;

/**
 * Payload of RTE_DWA_STAG_PROFILE_ADMIN_H2D_FW_UPDATE message.
 */
struct rte_dwa_profile_admin_h2d_fw_update {
	char fw[PATH_MAX]; /**< Firmware filename to update */
} __rte_packed;

/**
 * Enumerates the stag list for RTE_DWA_TAG_PROFILE_ADMIN tag.
 *
 * @note A successful RTE_DWA_STAG_PROFILE_ADMIN_H2D_ATTACH operation
 * must be prerequisite for all the admin operation.
 */
enum rte_dwa_profile_admin {
	/**
	 * Attribute |  Value
	 * ----------|--------
	 * Tag       | RTE_DWA_TAG_PROFILE_ADMIN
	 * Stag      | RTE_DWA_STAG_PROFILE_ADMIN_H2D_ATTACH
	 * Direction | H2D
	 * Type      | TYPE_ATTACHED
	 * Payload   | struct rte_dwa_profile_admin_h2d_attach
	 * Pair TLV  | RTE_DWA_STAG_COMMON_D2H_SUCCESS
	 * ^         | RTE_DWA_STAG_COMMON_D2H_ERR
	 *
	 * Request to attach DWA for administrative operation.
	 * A universally unique identifier (UUID) used to validate
	 * the validate the actor.
	 */
	RTE_DWA_STAG_PROFILE_ADMIN_H2D_ATTACH,
	/**
	 * Attribute |  Value
	 * ----------|--------
	 * Tag       | RTE_DWA_TAG_PROFILE_ADMIN
	 * Stag      | RTE_DWA_STAG_PROFILE_ADMIN_H2D_FW_UPDATE
	 * Direction | H2D
	 * Type      | TYPE_ATTACHED
	 * Payload   | struct rte_dwa_profile_admin_h2d_fw_update
	 * Pair TLV  | RTE_DWA_STAG_COMMON_D2H_SUCCESS
	 * ^         | RTE_DWA_STAG_COMMON_D2H_ERR
	 *
	 * Request DWA host ethernet port information.
	 */
	RTE_DWA_STAG_PROFILE_ADMIN_H2D_FW_UPDATE,
	RTE_DWA_STAG_PROFILE_ADMIN_MAX = UINT16_MAX,
	/**< Max stags for RTE_DWA_TAG_PROFILE_ADMIN tag*/
};

#ifdef __cplusplus
}
#endif

#endif /* RTE_DWA_PROFILE_ADMIN_H */
