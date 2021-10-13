/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(C) 2021 Marvell.
 */

#ifndef RTE_DWA_H
#define RTE_DWA_H

/**
 * @file
 *
 * RTE DWA API
 *
 * DWA components:
 *
 * \code{.c}
 *
 *
 *                                                 +--> rte_dwa_port_host_*()
 *                                                |  (User Plane traffic as TLV)
 *                                                |
 *               +----------------------+         |   +--------------------+
 *               |                      |         |   | DPDK DWA Device[0] |
 *               |  +----------------+  |  Host Port  | +----------------+ |
 *               |  |                |  |<========+==>| |                | |
 *               |  |   Profile 0    |  |             | |   Profile X    | |
 *               |  |                |  |             | |                | |
 *<=============>|  +----------------+  | Control Port| +----------------+ |
 *  DWA Port0    |  +----------------+  |<========+==>|                    |
 *               |  |                |  |         |   +--------------------+
 *               |  |   Profile 1    |  |         |
 *               |  |                |  |         +--> rte_dwa_ctrl_op()
 *               |  +----------------+  |         (Control Plane traffic as TLV)
 *<=============>|      Dataplane       |
 *  DWA Port1    |      Workload        |
 *               |      Accelerator     |             +---------- ---------+
 *               |      (HW/FW/SW)      |             | DPDK DWA Device[N] |
 *               |                      |  Host Port  | +----------------+ |
 *<=============>|  +----------------+  |<===========>| |                | |
 *  DWA PortN    |  |                |  |             | |   Profile Y    | |
 *               |  |    Profile N   |  |             | |           ^    | |
 *               |  |                |  | Control Port| +-----------|----+ |
 *               |  +-------|--------+  |<===========>|             |      |
 *               |          |           |             +-------------|------+
 *               +----------|-----------+                           |
 *                          |                                       |
 *                          +---------------------------------------+
 *                                                     ^
 *                                                     |
 *                                                     +--rte_dwa_dev_attach()
 *
 * \endcode
 *
 * **Dataplane Workload Accelerator**: It is an abstract model. The model is
 * capable of offloading the dataplane workload from application via
 * DPDK API over host and control ports of a DWA device.
 * Dataplane Workload Accelerator(DWA) typically contains a set of CPUs,
 * Network controllers, and programmable data acceleration engines for
 * packet processing, cryptography, regex engines, base-band processing, etc.
 * This allows DWA to offload compute/packet processing/base-band/cryptography
 * related workload from the host CPU to save cost and power. Also,
 * enable scaling the workload by adding DWAs to the host CPU as needed.
 *
 * **DWA device**: A DWA can be sliced to N number of DPDK DWA device(s)
 * based on the resources available in DWA.
 * The DPDK API interface operates on the DPDK DWA device.
 * It is a representation of a set of resources in DWA.
 *
 * **TLV**: TLV (tag-length-value) encoded data stream contain tag as
 * message ID, followed by message length, and finally the message payload.
 * The 32bit message ID consists of two parts, 16bit Tag and 16bit Subtag.
 * The tag represents ID of the group of the similar message,
 * whereas, subtag represents a message tag ID under the group.

 * **Control Port**: Used for transferring the control plane TLVs.
 * Every DPDK  DWA device must have a control port.
 * Only one outstanding TLV can be processed via this port by
 * a single DWA device.
 * This makes the control port suitable for the control plane.

 * **Host Port**: Used for transferring the user plane TLVs.
 * Ethernet, PCIe DMA, Shared Memory, etc.are the example of
 * different transport mechanisms abstracted under the host port.
 * The primary purpose of host port to decouple the user plane TLVs with
 * underneath transport mechanism differences.
 * Unlike control port, more than one outstanding TLVs can be processed by
 * a single DWA device via this port.
 * This makes, the host port transfer to be in asynchronous nature,
 * to support large volumes and less latency user plane traffic.

 * **DWA Port**: Used for transferring data between the external source and DWA.
 * Ethernet, eCPRI are examples of DWA ports. Unlike host ports,
 * the host CPU is not involved in transferring the data to/from DWA ports.
 * These ports typically connected to the Network controller inside the
 * DWA to transfer the traffic from the external source.
 *
 * **TLV direction**: `Host to DWA` and `DWA to Host` are the directions
 * of TLV messages. The former one is specified as `H2D`, and the later one is
 * specified as `D2H`. The `H2D` control TLVs, used for requesting DWA to
 * perform specific action and `D2H` control TLVs are used to respond to the
 * requested actions. The `H2D` user plane messages are used for transferring
 * data from the host to the DWA. The `D2H` user plane messages are used for
 * transferring data from the DWA to the host.
 *
 * **DWA device states**: Following are the different states of a DWA device.
 * - `READY`: DWA Device is ready to attach the profile.
 * See rte_dwa_dev_disc_profiles() API to discover the profile.
 * - `ATTACHED`: DWA Device attached to one or more profiles.
 * See rte_dwa_dev_attach() API to attach the profile(s).
 * - `STOPPED`: Profile is in the stop state.
 * TLV type `TYPE_ATTACHED` and `TYPE_STOPPED` messages are valid in this state.
 * After rte_dwa_dev_attach() or explicitly invoking the rte_dwa_stop() API
 * brings device to this state.
 * - `RUNNING`: Invoking rte_dwa_start() brings the device to this state.
 * TLV type `TYPE_STARTED` and `TYPE_USER_PLANE` are valid in this state.
 * - `DETACHED`: Invoking rte_dwa_dev_detach() brings the device to this state
 * The device and profile must be in the `STOPPED` state prior to invoking the
 * rte_dwa_dev_reattach().
 * - `CLOSED`: Closed a stopped/detached DWA device.
 *  The device cannot be restarted. Invoking rte_dwa_dev_close() brings the
 *  device to this state.
 *
 * **TLV types**: Following are the different TLV types
 * - `TYPE_ATTACHED`: Valid when the device is in `ATTACHED`, `STOPPED` and
 *   `RUNNING` state.
 * - `TYPE_STOPPED`: Valid when the device is in `STOPPED` state.
 * - `TYPE_STARTED`: Valid when the device is in `RUNNING` state.
 * - `TYPE_USER_PLANE`: Valid when the device is in `RUNNING` state and used to
 *   transfer only user plane traffic.
 *
 * **Profile**: Specifies a workload that dataplane workload accelerator
 * process on behalf of a DPDK application through a DPDK DWA device.
 * A profile is expressed as a set of TLV messages for control plane and
 * user plane functions.
 * Each TLV message must have Tag, SubTag, Direction, Type, Payload attributes.
 *
 * **Programming model**: Typical application programming sequence is as follows
 * -# In the EAL initialization phase, the DWA devices shall be probed,
 * the application can query the number of available DWA devices
 * using rte_dwa_dev_count() API.
 * -# Application discovers the available profile(s) in a DWA device using
 * rte_dwa_dev_disc_profiles() API.
 * -# Application attaches one or more profile(s) to a DWA device using
 * rte_dwa_dev_attach().
 * -# Once the profile is attached, The device shall be in the `STOPPED` state.
 * Configure the profile(s) with `TYPE_ATTACHED` and `TYPE_STOPPED` type TLVs
 * using rte_dwa_ctrl_op() API.
 * -# Once the profile is configured, move the profile to the `RUNNING` state
 * by invoking rte_dwa_start() API.
 * -# Once the profile is in running state and if it has user plane TLV,
 * transfer those TLVs using rte_dwa_port_host_() API based on the available
 * host port for the given profile attached.
 * -# Application can change the dynamic configuration aspects in
 * `RUNNING` state using rte_dwa_ctrl_op() API by issuing
 * `TYPE_STARTED` type of TLV messages.
 * -# Finally, use rte_dwa_stop(), rte_dwa_dev_detach(), rte_dwa_dev_close()
 * sequence for tear-down.
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

/* Core */
#include <rte_dwa_core.h>

/* DWA Device */
#include <rte_dwa_dev.h>

/* DWA Ports */
#include <rte_dwa_port_dwa_ethernet.h>

/* Host ports */
#include <rte_dwa_port_host_ethernet.h>

/* Profiles */
#include <rte_dwa_profile_admin.h>
#include <rte_dwa_profile_l3fwd.h>

#ifdef __cplusplus
}
#endif

#endif /* RTE_DWA_H */
