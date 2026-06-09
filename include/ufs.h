/* SPDX-License-Identifier: GPL-2.0+ */
#ifndef _UFS_CMD_H
#define _UFS_CMD_H

#include <linux/types.h>

/* Descriptor idn for Query requests */
enum desc_idn {
	QUERY_DESC_IDN_DEVICE		= 0x0,
	QUERY_DESC_IDN_CONFIGURATION	= 0x1,
	QUERY_DESC_IDN_UNIT		= 0x2,
	QUERY_DESC_IDN_RFU_0		= 0x3,
	QUERY_DESC_IDN_INTERCONNECT	= 0x4,
	QUERY_DESC_IDN_STRING		= 0x5,
	QUERY_DESC_IDN_RFU_1		= 0x6,
	QUERY_DESC_IDN_GEOMETRY		= 0x7,
	QUERY_DESC_IDN_POWER		= 0x8,
	QUERY_DESC_IDN_HEALTH           = 0x9,
	QUERY_DESC_IDN_MAX,
};

/* Flag idn for Query Requests*/
enum flag_idn {
	QUERY_FLAG_IDN_FDEVICEINIT			= 0x01,
	QUERY_FLAG_IDN_PERMANENT_WPE			= 0x02,
	QUERY_FLAG_IDN_PWR_ON_WPE			= 0x03,
	QUERY_FLAG_IDN_BKOPS_EN				= 0x04,
	QUERY_FLAG_IDN_LIFE_SPAN_MODE_ENABLE		= 0x05,
	QUERY_FLAG_IDN_PURGE_ENABLE			= 0x06,
	QUERY_FLAG_IDN_RESERVED2			= 0x07,
	QUERY_FLAG_IDN_FPHYRESOURCEREMOVAL		= 0x08,
	QUERY_FLAG_IDN_BUSY_RTC				= 0x09,
	QUERY_FLAG_IDN_RESERVED3			= 0x0A,
	QUERY_FLAG_IDN_PERMANENTLY_DISABLE_FW_UPDATE	= 0x0B,
	QUERY_FLAG_IDN_F_WRITE_BOOSTER_EN               = 0x0E,
	QUERY_FLAG_IDN_F_WB_BUF_FLUSH_EN                = 0x0F,
	QUERY_FLAG_IDN_F_WB_BUF_FLUSH_HIBERN8           = 0x10
};

enum ufs_desc_def_size {
	QUERY_DESC_DEVICE_DEF_SIZE		= 0x40,
	QUERY_DESC_CONFIGURATION_DEF_SIZE	= 0x90,
	QUERY_DESC_UNIT_DEF_SIZE		= 0x23,
	QUERY_DESC_INTERCONNECT_DEF_SIZE	= 0x06,
	QUERY_DESC_GEOMETRY_DEF_SIZE		= 0x48,
	QUERY_DESC_POWER_DEF_SIZE		= 0x62,
	QUERY_DESC_HEALTH_DEF_SIZE		= 0x25,
};

/* UTP QUERY Transaction Specific Fields OpCode */
enum query_opcode {
	UPIU_QUERY_OPCODE_NOP		= 0x0,
	UPIU_QUERY_OPCODE_READ_DESC	= 0x1,
	UPIU_QUERY_OPCODE_WRITE_DESC	= 0x2,
	UPIU_QUERY_OPCODE_READ_ATTR	= 0x3,
	UPIU_QUERY_OPCODE_WRITE_ATTR	= 0x4,
	UPIU_QUERY_OPCODE_READ_FLAG	= 0x5,
	UPIU_QUERY_OPCODE_SET_FLAG	= 0x6,
	UPIU_QUERY_OPCODE_CLEAR_FLAG	= 0x7,
	UPIU_QUERY_OPCODE_TOGGLE_FLAG	= 0x8,
};

struct udevice;

/**
 * ufs_probe() - initialize all devices in the UFS uclass
 *
 * Return: 0 if Ok, -ve on error
 */
int ufs_probe(void);

/**
 * ufs_probe_dev() - initialize a particular device in the UFS uclass
 *
 * @index: index in the uclass sequence
 *
 * Return: 0 if successfully probed, -ve on error
 */
int ufs_probe_dev(int index);

/*
 * In a QUERY REQUEST UPIU, the Device Descriptor
 * is addressed setting: DESCRIPTOR IDN = 00h
 */
struct ufs_device_desc {
	u8 length;		/* Size of this descriptor */
	u8 descriptor_idn;	/* Descriptor Type Identifier (00h) */
	u8 device;		/* Device type (00h: Device) */
	u8 device_class;
	/* UFS Device Class (00h: Mass Storage) */
	u8 device_sub_class;
	/* UFS Mass Storage Subclass (e.g., 00h: Embedded Bootable) */
	u8 protocol;
	/* Protocol supported by UFS Device (00h: SCSI) */
	u8 number_lu;
	/* Number of Logical units (non-well known)  */
	u8 number_wlu;
	/* Number of Well known Logical units (MDV = 04h) */
	u8 boot_enable;	/* Boot Enable (01h: enabled) */
	u8 descr_access_en;
	/* Descriptor Access Enable during partial boot initialization */

	/* --- power Management & Priority (Offset 0x0A) --- */
	u8 init_power_mode;
	/* Initial power Mode after device initi (00h: UFS-Sleep, 01h: */
	/*   Active) [4, 5] */
	u8 high_priority_lun;
	/* High Priority Logical unit (7Fh if all LUs have same priority) */
	u8 secure_removal_type;
	/* Secure Removal Type (00h: erase, 01h: overwrite/erase, etc.) */
	u8 security_lu;
	/* Support for security LU (01h: RPMB) */
	u8 background_ops_term_lat;
	/* Background Operations Termination Latency (in units of 10 ms) */
	u8 init_active_icc_level;
	/* Initial Active ICC Level after power on/reset (00h to 0Fh) */

	/* --- Version, Naming, & configuration Pointers (Offset 0x10) --- */
	u16 spec_version;
	/* Specification version in BCD format (e.g., 3.1 = 0310h) */
	u16 manufacture_date;	/* Manufacturing Date in BCD format */
	u8 manufacturer_name;
	/* Index to the Manufacturer Name string descriptor */
	u8 product_name;
	/* Index to the Product Name string descriptor */
	u8 serial_number;
	/* Index to the Serial Number string descriptor */
	u8 oem_id;
	/* Index to the OEM ID string descriptor */
	u16 manufacturer_id;
	/* Manufacturer ID code and Bank Index (JEP106 definition) */
	u8 ud0_base_offset;
	/* Offset of unit Descriptor 0 configurable parameters within the */
	/* configuration Descriptor */
	u8 config_length;
	/* Total size of the configurable unit Descriptor parameters */
	u8 device_rtt_cap;
	/* Maximum number of outstanding Ready To Transfer (RTT) UPIUs */
	/* supported  (min 2) */
	u16 periodic_rtc_update;
	/* Frequency and method of Real-Time Clock update */

	/* --- Feature Support & Device Status (Offset 0x1F) --- */
	u8 ufs_features_support;
	/* Bitmap indicating supported UFS features (FFU, PSA, etc.) */
	u8 ffu_timeout;
	/* Field Firmware Update Timeout (in seconds) */
	u8 queue_depth;
	/* Queue Depth (0: per-LU queueing; 1-255: shared queue depth) */
	u16 device_version;	/* Device Version */
	u8 num_secure_wp_area;
	/* Total number of Secure Write Protect */
	/* Areas supported (max 32) */
	u32 psa_max_data_size;
	/* PSA Maximum Data Size (in units of 4 Kbyte) */
	u8 psa_state_timeout;
	/* PSA State Timeout (max command timeout for b_psa_state change) */
	u8 product_revision_level;
	/* Index to the Product Revision Level string descriptor */
	u8 reserved_2bh[18];	/* reserved */
	u8 reserved_30h[19];
	/* Reserved for Unified Memory Extension standard */
	u8 reserved_40h[20];
	/* Reserved for Host Performance Booster (HPB) Extension Standard */
	u8 reserved_43h[21];	/* Reserved */

	/* --- Extended Feature Support (Offset 0x4F) --- */
	u32 extended_ufs_features_support;
	/* Extended UFS Features Support (including WriteBooster, */
	/* offset 53h Performance Throttling) */
	u8 write_booster_buffer_preserve_user_space_en;
	/* offset 54h Preserve User Space mode for WriteBooster Buffer */
	u8 write_booster_buffer_type;
	/*  offset 55h WriteBooster Buffer Type
	 *  (0h:dedicated buffer,01h: shared buffer)
	 */
	u32 num_shared_write_booster_buffer_allocunits;
	/* WriteBooster Buffer size for shared configuration */
	/* (in Allocation units) */
} __packed;

struct unit_desc {
	u8 length;		/* Size of this descriptor (Default: 2Dh) */
	u8 descriptor_idn;	/* unit Descriptor Type Identifier (02h) */
	u8 bunit_index;		/* The index of the Logical unit */

	/* --- configuration Parameters --- */
	u8 lu_enable;		/* 01h: LU enabled, 00h: LU disabled */
	u8 boot_lun_id;
	/* 01h: Boot LU A, 02h: Boot LU B, 00h: Not bootable */
	u8 lu_write_protect;
	/* 01h: power-on WP, 02h: Permanent WP, 00h: None */
	u8 lu_queue_depth;
	/* LU-specific queue depth (0 if shared queuing is used) */
	u8 psa_sensitive;
	/* 01h: Sensitive to soldering, 00h: Not sensitive */
	u8 memory_type;
	/* Memory type (e.g., 00h: Normal, 01h: System Code) */
	u8 data_reliability;
	/* 01h: Data protected against power failure */
	u8 logical_block_size;
	/* Logical Block Size exponent (e.g., 0Ch = 4 Kbyte) */

	/* --- Capacity and geometry --- */
	u64 logical_block_count;
	/* Total number of addressable logical blocks in the LU */
	u32 erase_block_size;
	/* Optimal granularity for erase/discard operations */
	u8 provisioning_type;
	/* 00h: Full, 02h: Thin (TPRZ=0), 03h: Thin (TPRZ=1) */
	u64 phy_mem_resource_count;
	/* Total physical memory resources available to the LU */

	/* --- Performance and Extended Features --- */
	u16 context_capabilities;
	/* Bits [3:0]: MaxContextID; Bits [6:4]: Large unit Multiplier */
	u8 largeunit_granularity_M1;
	/* Large unit Granularity = 1MB * (value + 1) */
	u8 reserved[11];	/* Reserved for HPB Extension Standard */
	u32 lu_num_write_booster_buffer_allocunits;
	/* WriteBooster Buffer size in Allocation units */
} __packed;

/*
 *
 * UFS Geometry Descriptor (IDN 07h)
 * Total Size: 87 bytes (0x57)
 */
struct ufs_geometry_desc {
	/* --- Descriptor Header (Offset 0x00) --- */
	u8 length;		/* Size of this descriptor (57h) */
	u8 descriptor_idn;	/* geometry Descriptor ID (07h) */
	u8 media_technology;	/* Reserved (00h) */
	u8 reserved_03h;	/* Reserved (00h) */

	/* --- Capacity and Device geometry (Offset 0x04) --- */
	u64 total_raw_device_capacity; /* Total memory available (units of 512B) */
	u8 max_number_lu;	/* Max LUs: 00h (8 LUs) or 01h (32 LUs) */
	u32 segment_size;	/* Segment size (units of 512B) */
	u8 allocationunit_size;	/* Allocation unit Size (number of segments) */
	u8 min_addr_block_size;
	/* Min addressable block size (units of 512B, min 08h) */
	u8 optimal_read_block_size;	/* Optimal read granularity (units of 512B) */
	u8 optimal_write_block_size;	/* Optimal write granularity (units of 512B) */
	u8 max_in_buffer_size;	/* Max data-in buffer size (units of 512B) */
	u8 max_out_buffer_size;	/* Max data-out buffer size (units of 512B) */
	u8 rpmb_read_write_size;	/* Max RPMB frames (256B) per command */
	u8 dynamic_capacity_resource_policy;	/* 00h: per LU, 01h: per memory type */
	u8 data_ordering;	/* 01h: Out-of-order data transfer supported */
	u8 max_contex_id_number;	/* Maximum supported contexts (min 5) */
	u8 sys_data_tagunit_size;	/* Granularity for System Data Tagging */
	u8 sys_data_tag_res_size;	/* Max storage area for system data tagging */
	u8 supported_sec_r_types;	/* Bitmap of supported Secure Removal Types */
	u16 supported_memory_types;
	/* Bitmap of supported memory types (Normal, Enhanced, etc.) */

	/* --- Memory Type Capacity and Factors (Offset 0x20) --- */
	u32 system_code_max_n_alloc_u;
	/* Max Allocation units for System Code memory */
	u16 system_code_cap_adj_fac;
	/* Capacity Adjustment Factor for System Code */
	u32 non_persist_max_n_alloc_u;
	/* Max Allocation units for Non-Persistent memory */
	u16 non_persist_cap_adj_fac;
	/* Capacity Adjustment Factor for Non-Persistent */
	u32 enhanced1_max_n_alloc_u;
	/* Max Allocation units for Enhanced memory */
	/* Capacity Adjustment Factor for Enhanced */
	u16 enhanced1_cap_adj_fac;
	u32 enhanced2_max_n_alloc_u;
	/* Max Allocation units for Enhanced memory */
	/* Capacity Adjustment Factor for Enhanced */
	u16 enhanced2_cap_adj_fac;
	u32 enhanced3_max_n_alloc_u;
	/* Max Allocation units for Enhanced memory */
	/* Capacity Adjustment Factor for Enhanced */
	u16 enhanced3_cap_adj_fac;
	u32 enhanced4_max_n_alloc_u;
	/* Max Allocation units for Enhanced memory */
	/* Capacity Adjustment Factor for Enhanced */
	u16 enhanced4_cap_adj_fac;
	/* --- Performance Optimization (Offset 0x44) --- */
	/* Optimal Logical Block Size per memory type */
	u32 optimal_logical_block_size;
	u8  reserved_hpb[4]; // 48h: Reserved for HPB Extension
	u16 reserved2;   // 4Dh: Reserved
	/* WriteBooster Parameters */
	/* 4Fh: Max total WriteBooster Buffer size */
	u32 write_booster_buffer_max_alloc_units;
	/* 53h: Max LUs supporting WriteBooster (Valid: 1) */
	u8  device_max_write_booster_lus;
	/* 54h: LBA space reduction factor (e.g., 3 for TLC) */
	u8  write_booster_buffer_capadjfac;
	/* 55h: User space vs Preserve types */
	u8  supported_write_booster_buffer_userspace_reduction_types;
	/* 56h: LU based vs Shared buffer type */
	u8  supported_write_booster_buffer_types;
} __packed;

/**
 * UFS configuration Descriptor - Header and Device configurable Parameters
 * Based on Table 14.10 (INDEX = 00h)
 */
struct config_descriptor {
	/* --- Descriptor Header --- */
	u8 length;
	/* 00h: Size of this descriptor (MDV: E6h) */
	u8 descriptor_idn;
	/* 01h: configuration Descriptor Type ID (01h) */

	/* --- configuration Flow Control --- */
	u8 conf_desc_continue;
	/* 02h: 00h = Last descriptor (apply config); */
	/* 01h = More descriptors follow */

	/* --- Device Descriptor configurable Parameters --- */
	u8 boot_enable;
	/* 03h: Enables boot feature (01h: enabled) */
	u8 descr_access_en;
	/* 04h: Enables Device Descriptor access after partial init */
	u8 initpower_mode;
	/* 05h: Initial power Mode after reset (01h: Active) */
	u8 high_priority_lun;
	/* 06h: LUN of the high priority logical unit */
	u8 secure_removal_type;
	/* 07h: configures secure removal (e.g., 00h: erase) */
	u8 init_active_icc_level;
	/* 08h: Initial ICC level after reset (00h-0Fh) */
	u16 periodic_rtc_update;
	/* 09h: Frequency/method of Real-Time Clock update */
	u8 reserved_HPB;
	/* 0Bh: Reserved for Host Performance Booster */

	/* --- RPMB configuration Parameters --- */
	u8 rpmb_region_enable;
	/* 0Ch: Bitmask to enable RPMB regions 1-3 */
	u8 rpmb_region1_size;
	/* 0Dh: Size of RPMB region 1 in 128KB units */
	u8 rpmb_region2_size;
	/* 0Eh: Size of RPMB region 2 in 128KB units */
	u8 rpmb_region3_size;
	/* 0Fh: Size of RPMB region 3 in 128KB units */

	/* --- WriteBooster configuration Parameters --- */
	u8 write_booster_buffer_preserve_user_space_en;
	/* 10h: 01h = Preserve user space */
	u8 write_booster_buffer_type;
	/* 11h: 00h = Dedicated LU; 01h = Shared */
	u32 num_shared_write_booster_buffer_allocunits;
	/* 12h: Shared buffer size in Alloc units */
} __packed;

/**
 * UFS unit Descriptor configurable Parameters
 * Found within the configuration Descriptor (IDN: 01h)
 * Based on Table 14.12
 */
struct ufs_unit_configurable_parameters {
	u8 lu_enable;		/* 01h: Enabled, 00h: Disabled */

	u8 boot_lun_id;		/* 0 Not Bootable,1-A or 2-B */

	/* 01h: Boot LU A, 02h: Boot LU B, 00h: Not bootable */
	u8 lu_write_protect;
	/* 00h: None, 01h: power-on WP, 02h: Permanent WP */
	u8 memory_type;
	/* Memory type (00h: Normal, 01h: System Code) */

	/* Number of allocation units assigned to the LU. */
	u32 num_allocunits;

	u8 data_reliability; /* 01h: Protected against power failure */

	/* Block size exponent (e.g., 0Ch = 4 KB) */
	u8 logical_block_size;

	/* 00h: Full, 02h: Thin (TPRZ=0), 03h: Thin (TPRZ=1) */
	u8 provisioning_type;

	u16 context_capabilities; /* LU Context support and multipliers */

	u8 reserved_0dh[6];	/* Reserved */

	u8 reserved_hpb[7];
	/* Reserved for Host Performance Booster Extension */

	u32 lu_num_write_booster_buffer_allocunits;
	/* WriteBooster Buffer size in Allocation units. */
} __packed;

#define UFS_DESC_SIZE (unsigned char)0xff

struct ufs_config_desc {
	struct config_descriptor cfg_desc;
	/*
	 * NOTE: For INDEX = 00h, configurable parameters
	 * for unit Descriptors 0 to 7
	 */
	/* follow this header at the offset defined
	 * by b_ud0_base_offset
	 */
	unsigned char space[UFS_DESC_SIZE];
} __packed;

/**
 * UFS power Parameter Element Format
 * Based on Table 7.13
 */
struct ufs_power_parameter_element {
	/* Bits [15:14]: unit (00b:n_a, 01b:u_a, 10b:m_a, 11b:A) */
	/* Bits [13:10]: Reserved (0000b) */
	/* Bits [9:0]:   Value (Maximum current expected) */
	u16 upower_value;
};

/**
 * UFS power Parameters Descriptor (IDN: 08h)
 * Based on Table 14.16
 */
struct ufs_power_param_desc {
	/* Offset 00h */
	u8 length;		/* Size of descriptor (62h) */
	u8 descriptor_idn;	/* Descriptor Type ID (08h) */

	/* Offset 02h: Maximum current for VCC (16 levels) */
	struct ufs_power_parameter_element active_icc_levels_vcc[5];
	/* Offset 22h: Maximum current for VCCQ (16 levels) */
	struct ufs_power_parameter_element active_icc_levels_vccq[5];
	/* Offset 42h: Maximum current for VCCQ2 (16 levels) */
	struct ufs_power_parameter_element active_icc_levels_vccq2[5];
} __packed;

struct ufs_hba;

/*
 * ufshcd_read_desc_param() - Read a parameter or an entire UFS descriptor.
 * @hba: UFS host bus adapter instance.
 * @desc_id: Descriptor identifier (IDN) to read.
 * @desc_index: Descriptor index (for example, LUN number for Unit
 * descriptors).
 * @param_offset: Byte offset within the descriptor to start reading from.
 * @param_read_buf: Destination buffer for the returned data.
 * @param_size: Number of bytes to read.
 *
 * Reads a descriptor from the UFS device and copies the requested portion
 * of the descriptor into @param_read_buf. If a partial descriptor read is
 * requested, a temporary buffer is allocated internally and the requested
 * bytes are extracted from the full descriptor.
 *
 * Return: 0 on success, negative errno on failure.
 */
int ufshcd_read_desc_param(struct ufs_hba *hba, enum desc_idn desc_id,
			   int desc_index, u8 param_offset,
			   u8 *param_read_buf, u8 param_size);

/*
 *
 * ufshcd_cache_flush() - Flush a memory range from the CPU cache.
 * @addr: Start address of the memory range.
 * @size: Size of the memory range in bytes.
 *
 * Flushes the cache lines covering the supplied address range. The range is
 * aligned to the architecture DMA alignment requirements before flushing.
 */
void ufshcd_cache_flush(void *addr, unsigned long size);

/*
 * ufshcd_query_descriptor_retry() - Execute a descriptor query with retries.
 * @hba: UFS host bus adapter instance.
 * @opcode: Query opcode (read or write descriptor).
 * @idn: Descriptor identifier (IDN).
 * @index: Descriptor index.
 * @selector: Descriptor selector value.
 * @desc_buf: Descriptor data buffer.
 * @buf_len: In/out descriptor length in bytes.
 *
 * Sends a UFS descriptor query request and automatically retries the
 * operation on transient failures.
 *
 * Return: 0 on success, negative errno on failure.
 */
int ufshcd_query_descriptor_retry(struct ufs_hba *hba, enum query_opcode opcode,
				  enum desc_idn idn, u8 index,
				  u8 selector, u8 *desc_buf, int *buf_len);

/*
 * ufshcd_query_desc_idn_geometry() - Read the Geometry Descriptor.
 * @hba: UFS host bus adapter instance.
 * @desc: Buffer receiving the geometry descriptor.
 *
 * Reads the UFS Geometry Descriptor (IDN 0x07) from the device.
 *
 * Return: 0 on success, negative errno on failure.
 */
int ufshcd_query_desc_idn_geometry(struct ufs_hba *hba,
				   struct ufs_geometry_desc *desc);

/*
 * ufshcd_query_desc_idn_config() - Read the Configuration Descriptor.
 * @hba: UFS host bus adapter instance.
 * @cfg: Buffer receiving the configuration descriptor.
 *
 * Reads the UFS Configuration Descriptor (IDN 0x01).
 *
 * Return: 0 on success, negative errno on failure.
 */
int ufshcd_query_desc_idn_config(struct ufs_hba *hba, struct ufs_config_desc *cfg);

/*
 * ufshcd_query_desc_idn_device() - Read the Device Descriptor.
 * @hba: UFS host bus adapter instance.
 * @desc: Buffer receiving the device descriptor.
 *
 * Reads the UFS Device Descriptor (IDN 0x00).
 *
 * Return: 0 on success, negative errno on failure.
 */
int ufshcd_query_desc_idn_device(struct ufs_hba *hba,
				 struct ufs_device_desc *desc);

/**
 * ufshcd_query_desc_idn_unit() - Read a Unit Descriptor.
 * @hba: UFS host bus adapter instance.
 * @lun: Logical Unit Number whose descriptor should be read.
 * @desc: Buffer receiving the unit descriptor.
 *
 * Reads the Unit Descriptor (IDN 0x02) associated with @lun.
 *
 * Return: 0 on success, negative errno on failure.
 */
int ufshcd_query_desc_idn_unit(struct ufs_hba *hba, int lun,
			       struct unit_desc *desc);

/**
 * ufshcd_print_idn_geometry_desc() - Print a geometry descriptor.
 * @desc: Geometry descriptor to display.
 *
 * Dumps the contents of a UFS Geometry Descriptor in a human-readable
 * format.
 */
void ufshcd_print_idn_geometry_desc(struct ufs_geometry_desc *desc);

/**
 * ufshcd_print_device_descriptor() - Print a device descriptor.
 * @desc: Device descriptor to display.
 *
 * Dumps the contents of a UFS Device Descriptor in a human-readable format.
 */
void ufshcd_print_device_descriptor(struct ufs_device_desc *desc);

/**
 * ufshcd_print_unit_desc() - Print a unit descriptor.
 * @desc: Unit descriptor to display.
 *
 * Dumps the contents of a UFS Unit Descriptor in a human-readable format.
 */
void ufshcd_print_unit_desc(struct unit_desc *desc);

/*
 * ufshcd_query_desc_idn_power() - Read a Power Parameters Descriptor.
 * @hba: UFS host bus adapter instance.
 * @lun: Logical Unit Number associated with the descriptor query.
 * @desc: Buffer receiving the power parameters descriptor.
 *
 * Reads the UFS Power Parameters Descriptor (IDN 0x08).
 *
 * Return: 0 on success, negative errno on failure.
 */
int ufshcd_query_desc_idn_power(struct ufs_hba *hba, int lun,
				struct ufs_power_param_desc *desc);

/*
 * print_ufs_power_parameters() - Print a power parameters descriptor.
 * @desc: Power parameters descriptor to display.
 *
 * Displays the supported ICC current levels for the VCC, VCCQ and VCCQ2
 * power rails in a human-readable table.
 */
void print_ufs_power_parameters(struct ufs_power_param_desc *desc);

/**
 * ufshcd_query_user_flag() - Query and display a UFS flag value.
 * @hba: UFS host bus adapter instance.
 * @flag_name: User-visible flag name string.
 *
 * Converts @flag_name to the corresponding UFS flag IDN, reads the flag
 * value from the device and prints the result.
 *
 * Return: 0 on success, negative errno on failure.
 */

int ufshcd_query_user_flag(struct ufs_hba *hba, const char *flag_name);

/*
 * ufshcd_query_flag() - Send a UFS flag query request
 * @hba: pointer to the UFS host bus adapter instance
 * @opcode: query operation to perform
 * @idn: flag identifier (IDN) to access
 * @flag_res: pointer to store the returned flag value for
 *            %UPIU_QUERY_OPCODE_READ_FLAG operations
 *
 * Issues a UFS QUERY REQUEST UPIU for a device flag. Supported
 * operations are:
 *
 *   - %UPIU_QUERY_OPCODE_READ_FLAG
 *   - %UPIU_QUERY_OPCODE_SET_FLAG
 *   - %UPIU_QUERY_OPCODE_CLEAR_FLAG
 *   - %UPIU_QUERY_OPCODE_TOGGLE_FLAG
 *
 * For read operations, @flag_res must be non-NULL and receives the
 * least-significant bit of the value field returned in the QUERY
 * RESPONSE UPIU. For write-type operations, @flag_res may be NULL.
 *
 * Return:
 * * 0          - Success
 * * %-EINVAL   - Invalid opcode or NULL @flag_res for a read request
 * * Other negative errno values returned by ufshcd_exec_dev_cmd()
 *                if the query request fails
 */
int ufshcd_query_flag(struct ufs_hba *hba, enum query_opcode opcode,
		      enum flag_idn idn, bool *flag_res);
#endif
