/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(C) 2021 Marvell.
 */

#ifndef RTE_DWA_DEV_H
#define RTE_DWA_DEV_H

/**
 * @file
 *
 * @warning
 * @b EXPERIMENTAL:
 * All functions in this file may be changed or removed without prior notice.
 *
 * RTE DWA Device API
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

#include <rte_common.h>

/* Device utils */

/**
 * Check if dev_id of DWA device is valid.
 *
 * @param dev_id
 *   DWA device id.
 *
 * @return
 *   true if device is valid, false otherwise.
 */
bool
rte_dwa_dev_is_valid(uint16_t dev_id);

/**
 * Get the total number of DWA devices that have been successfully
 * initialised.
 *
 * @return
 *   The total number of usable DWA devices.
 */
uint16_t
rte_dwa_dev_count(void);

/* Discovery */

/**
 * Get the list of profiles available in a DWA device.
 *
 * @param dev_id
 *   DWA device id.
 *
 * @param [out] pfs
 *   An rte_dwa_tag_profile array to be filled with available profiles.
 *   If set to NULL, this function returns the number of available profiles.
 *
 * @return
 *   Number of profiles available.
 */
int
rte_dwa_dev_disc_profiles(uint16_t dev_id, enum rte_dwa_tag_profile *pfs);

/* Attach */

/**
 * Attach a list of profiles on a DWA device.
 *
 * Upon successful attach operation, devices moves to `ATTACHED` state.
 * `TYPE_ATTACHED` and `TYPE_STOPPED` types of TLV message are valid
 * in this state.
 *
 * @param dev_id
 *   DWA device id.
 *
 * @param name
 *   Unique name for getting DWA object on secondary process.
 *   @see rte_dwa_dev_lookup()
 * @param pfs
 *   An array profiles as enum rte_dwa_tag_profile to be attached.
 * @param nb_pfs
 *   Number of profiles to be attached.
 *
 * @return
 *   DWA object.
 */
rte_dwa_obj_t
rte_dwa_dev_attach(uint16_t dev_id, const char *name,
		   enum rte_dwa_tag_profile pfs[], uint16_t nb_pfs);

/* Lookup */

/**
 * Search the DWA object from its name.
 *
 * @param dev_id
 *   DWA device id.
 *
 * @param name
 *   The name provided in rte_dwa_dev_attach().
 *
 * @return
 *   DWA object or NULL if not found.
 */
rte_dwa_obj_t
rte_dwa_dev_lookup(uint16_t dev_id, const char *name);

/* Detach */

/**
 * Detach all profiles from the given DWA device.
 *
 * Detach all profile and move the DWA device to `DETACHED` state.
 * The device and profile must be in the `STOPPED` state prior to invoking the
 * this API. @see rte_dwa_stop()
 *
 * @param dev_id
 *   DWA device id.

 * @param obj
 *   DWA object.
 *
 * @return
 *   0 on success, error otherwise.
 */
int
rte_dwa_dev_detach(uint16_t dev_id, rte_dwa_obj_t obj);

/* Close */

/**
 * Close a DWA device. The device cannot be restarted!
 * The device and profile must be in the `DETACHED` state prior to invoking
 * this API. @see rte_dwa_dev_detach()
 *
 * @param dev_id
 *   DWA device id.
 *
 * @return
 *   0 on success, error otherwise.
 */
int
rte_dwa_dev_close(uint16_t dev_id);

#ifdef __cplusplus
}
#endif

#endif /* RTE_DWA_DEV_H */
