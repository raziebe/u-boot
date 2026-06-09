// SPDX-License-Identifier: GPL-2.0+
/**
 * ufs.c - UFS specific U-Boot commands
 *
 * Copyright (C) 2019 Texas Instruments Incorporated - https://www.ti.com
 * Copyright (C) 2026 Mobileye Incorporated - https://www.mobileye.com
 */

#include <dm/uclass.h>
#include <dm/device.h>
#include <command.h>
#include <malloc.h>
#include <ufs.h>

static struct ufs_config_desc g_cfg_desc;
static int hba_id;

struct ufs_hba *ufstool_probe_dev(int index)
{
	struct udevice *dev;
	struct ufs_hba *hba = NULL;

	if (!uclass_get_device(UCLASS_UFS, index, &dev))
		hba = dev_get_uclass_priv(dev);

	return hba;
}

int ufshcd_query_desc_idn_device(struct ufs_hba *hba,
				 struct ufs_device_desc *desc)
{
	int param_offset = 0;
	int desc_index = 0;	/* LUNS 0-7 controlled by index 0 */

	return ufshcd_read_desc_param(hba, QUERY_DESC_IDN_DEVICE, desc_index,
				      param_offset, (void *)desc,
				      UFS_DESC_SIZE);
}

void ufshcd_print_device_descriptor(struct ufs_device_desc *descriptor)
{
	u32 val = descriptor->num_shared_write_booster_buffer_allocunits;

	printf("--- UFS Device Descriptor (IDN: 00h %ld) ---\n",
	       sizeof(struct ufs_device_desc));

	printf(" Name | Value | Description\n");
	printf("-----------------------------------------------------------\n");
	/* Descriptor Header (Offset 00h – 01h) */
	printf("Length | 0x%x\n", descriptor->length);
	printf("DescriptorIdn | 0x%x\n", descriptor->descriptor_idn);

	/* Device Identification & Core configuration (Offset 02h – 0Dh) */
	printf("Device | 0x%x\n", descriptor->device);
	printf("DeviceClass | 0x%x\n", descriptor->device_class);
	printf("DeviceSubClass | 0x%x\n", descriptor->device_sub_class);
	printf("protocol | 0x%x\n", descriptor->protocol);
	printf("NumberLu | %d\n", descriptor->number_lu);
	printf("NumberWLu | %d\n", descriptor->number_wlu);
	printf("BootEnable | 0x%x\n", descriptor->boot_enable);
	printf("DescrAccessEn | 0x%x\n", descriptor->descr_access_en);
	printf("InitPowerMode | 0x%x\n", descriptor->init_power_mode);
	printf("highPriorityLun | 0x%x\n",
	       descriptor->high_priority_lun);
	printf("SecureRemovalType | 0x%x\n",
	       descriptor->secure_removal_type);
	printf("SecurityLu | 0x%x\n", descriptor->security_lu);

	/* power Management, Version & Pointers (Offset 0Eh – 2Ah) */
	printf("b_background_ops_term_lat | 0x%x\n",
	       descriptor->background_ops_term_lat);
	printf("b_init_active_icc_level | 0x%x\n",
	       descriptor->init_active_icc_level);
	printf("SpecVersion | 0x%x\n",
	       __builtin_bswap16(descriptor->spec_version));
	printf("ManufactureDate | 0x%x\n", descriptor->manufacture_date);
	printf("ManufacturerName | 0x%x\n", descriptor->manufacturer_name);
	printf("ProductName | 0x%x\n", descriptor->product_name);
	printf("SerialNumber | 0x%x\n", descriptor->serial_number);
	printf("OEMId | 0x%x\n", descriptor->oem_id);
	printf("ManufacturerId | 0x%x\n", descriptor->manufacturer_id);
	printf("Ud0BaseOffset | %d\n", descriptor->ud0_base_offset);
	printf("DConfigLength | %d\n",
	       descriptor->config_length);
	printf("DeviceRttCap | 0x%x\n", descriptor->device_rtt_cap);
	printf("PeriodicRtcUpdate | 0x%x\n",
	       descriptor->periodic_rtc_update);
	printf("UfsFeaturesSupport | 0x%x\n",
	       descriptor->ufs_features_support);
	printf("FfuTimeout | %d\n", descriptor->ffu_timeout);
	printf("QueueDepth | %d\n", descriptor->queue_depth);
	printf("DeviceVersion | 0x%x\n", descriptor->device_version);
	printf("NumSecureWpArea | 0x%x\n",
	       descriptor->num_secure_wp_area);
	printf("PsaMaxDataSize | 0x%x\n",
	       descriptor->psa_max_data_size);
	printf("PsaStateTimeout | 0x%x\n",
	       descriptor->psa_state_timeout);
	printf("ProductRevisionLevel | 0x%x\n",
	       descriptor->product_revision_level);

	/* Extended Features (Offset 4Fh – 58h) */
	printf("ExtendedUfsFeaturesSupport ");
	printf("| 0x%x |(includes  WriteBooster/Throttling)\n",
	       be32_to_cpu(descriptor->extended_ufs_features_support));

	printf("WriteBoosterBufferPreserveUserSpaceEn");
	printf("| 0x%x | Preserve User Space mode for WriteBooster Buffer\n",
	       descriptor->write_booster_buffer_preserve_user_space_en);

	printf("write booster buffer type");
	printf("| 0x%x | WriteBooster Buffer Type",
	       descriptor->write_booster_buffer_type);
	puts("(00h: LU dedicated, 01h: shared)\n");

	printf("NumSharedWriteBooster_buffer_allocunits |");
	printf("%d | Shared WriteBooster Buf size (in Allocation units)\n",
	       be32_to_cpu(val));
}

int ufshcd_query_desc_idn_geometry(struct ufs_hba *hba,
				   struct ufs_geometry_desc *desc)
{
	int param_offset = 0;
	int desc_index = 0;

	return ufshcd_read_desc_param(hba, QUERY_DESC_IDN_GEOMETRY, desc_index,
				      param_offset, (void *)desc,
				      QUERY_DESC_GEOMETRY_DEF_SIZE);
}

void ufshcd_print_idn_geometry_desc(struct ufs_geometry_desc *desc)
{
	u64 cap = be64_to_cpu(desc->total_raw_device_capacity);

	cap = ((cap << 9) >> 30);

	printf("--- UFS geometry Descriptor (IDN: 07h) ---\n");
	printf(" Name   | Value              | Description\n");
	printf("-------------------------------------------------------\n");

	/* Header and Core Capacity (Offset 00h - 0Ch) */
	printf("Length | 0x%x  | Size of descriptor\n", desc->length);
	printf("DescriptorIdn   | 0x%x | geometry Descriptor ID\n",
	       desc->descriptor_idn);
	printf("MediaTechnology | 0x%x | Reserved (00h)\n",
	       desc->media_technology);

	printf("TotalRawDeviceCapacity | %lld GB\n", cap);

	printf("MaxNumberLu | %d | Max LUs (00h: 8, 01h: 32)\n",
	       desc->max_number_lu);

	/* Segment and Allocation unit constraints (Offset 0Dh - 12h) */
	printf("SegmentSize | %d | Segment size (512B units)\n",
	       be32_to_cpu(desc->segment_size));

	printf("AllocationunitSize | %d | Alloc unit size (in segments)\n",
	       desc->allocationunit_size);

	printf("MinAddrBlockSize | %d | Min addressable block size\n",
	       desc->min_addr_block_size);

	/* Performance and Buffer parameters (Offset 13h - 17h) */
	printf("OptimalReadBlockSize");
	printf("| %d | Optimal read size (512B units)\n",
	       desc->optimal_read_block_size);

	printf("OptimalWriteBlockSize ");
	printf("| 0x%x | Optimal write size (512B units)\n",
	       desc->optimal_write_block_size);

	printf("MaxInBufferSize | 0x%x | Max Data-In buffer (512B units)\n",
	       desc->max_in_buffer_size);

	printf("MaxOutBufSize | 0x%x | Max Data-Out buffer (512B)\n",
	       desc->max_out_buffer_size);

	printf("bRPMBReadWriteSize | 0x%x | Max RPMB frames per command\n",
	       desc->rpmb_read_write_size);

	/* Feature Support Bitmaps (Offset 18h - 1Fh) */
	printf("DynamicCapacityResourcePolicy| 0x%x |",
	       desc->dynamic_capacity_resource_policy);

	printf("00h: per LU, 01h: per Memory Type\n");
	printf("DataOrdering | 0x%x | 01h: Out-of-order supported\n",
	       desc->data_ordering);
	printf("MaxContexIdNumber | 0x%x | Max supported contexts (min 5)\n",
	       desc->max_contex_id_number);

	printf("SysDataTagUnitSize | 0x%x | System data tag granularity\n",
	       desc->sys_data_tagunit_size);

	printf("SysDataTagResSize  | 0x%x | Max area for tagged system data\n",
	       desc->sys_data_tag_res_size);

	printf("SupportedSecRTypes | 0x%x | Supported secure removal types\n",
	       desc->supported_sec_r_types);

	printf("SupportedMemoryTypes | 0x%x | Bitmap of memory types\n",
	       be16_to_cpu(desc->supported_memory_types));
	/*
	 * Memory Type Specific Capacities and Adjustment Factors
	 */
	printf("SystemCodeMaxNAlloc | 0x%x | Max Alloc units\n",
	       desc->system_code_max_n_alloc_u);

	printf("SystemCodeCapAdjFac | 0x%x | cap_adj_factor: System Code\n",
	       desc->system_code_cap_adj_fac);

	printf("NonPersistMaxNAlloc | 0x%x | Max Alloc units: Non-Persist\n",
	       desc->non_persist_max_n_alloc_u);

	printf("NonPersistCapAdjFac   | 0x%x | cap_adj_factor: Non-Persist\n",
	       desc->non_persist_cap_adj_fac);

	printf("Enhanced1MaxNAlloc | %d | Max Alloc units: Enhanced 1\n",
	       be32_to_cpu(desc->enhanced1_max_n_alloc_u));

	printf("Enhanced1CapAdjFac | %d | cap_adj_factor: Enhanced 1\n",
	       be16_to_cpu(desc->enhanced1_cap_adj_fac));

	printf("Enhanced2MaxNAlloc_u | %d | Max Alloc units: Enhanced 2\n",
	       be32_to_cpu(desc->enhanced2_max_n_alloc_u));

	printf("Enhanced2CapAdjFac | %d | cap_adj_factor: Enhanced 2\n",
	       be16_to_cpu(desc->enhanced2_cap_adj_fac));
	printf("Enhanced3MaxNAlloc | %d | Max Alloc units: Enhanced 3\n",
	       be32_to_cpu(desc->enhanced3_max_n_alloc_u));

	printf("Enhanced3CapAdjFac | %d | cap_adj_factor: Enhanced 3\n",
	       be32_to_cpu(desc->enhanced3_cap_adj_fac));

	printf("Enhanced4MaxNAlloc | %d | Max Alloc units: Enhanced 4\n",
	       be32_to_cpu(desc->enhanced4_max_n_alloc_u));

	printf("Enhanced4CapAdjFac | %d | cap_adj_factor: Enhanced 4\n",
	       be16_to_cpu(desc->enhanced4_cap_adj_fac));

	/* Performance and WriteBooster (Offset 44h - 56h) */
	printf("OptimalLogicalBlockSize | %d | Optimal Blk Size per type\n",
	       be32_to_cpu(desc->optimal_logical_block_size));

	printf("WriteBoosterBufferMaxNAllocunits");
	printf("| %d | Max tot WB buf size\n",
	       be32_to_cpu(desc->write_booster_buffer_max_alloc_units));

	printf("DeviceMaxWriteBoosterLus ");
	printf("| %d | Max LUs supporting WB (MDV:1)\n",
	       desc->device_max_write_booster_lus);

	printf("WriteBoosterBufferCapAdjFac");
	printf("| %d | WB Capacity Adjustment Factor\n",
	       desc->write_booster_buffer_capadjfac);

	printf("SupportedwbBufferUserSpaceReductionType");
	printf(" | 0x%x | Reduction/Preserve mode support\n",
	       desc->supported_write_booster_buffer_userspace_reduction_types);

	printf("SupportedWriteBoosterBufferTypes");
	printf("| 0x%x | 00h: LU based, 01h: Shared\n",
	       desc->supported_write_booster_buffer_types);
}

void ufshcd_print_config_descriptor(const struct config_descriptor *cd)
{
	if (!cd)
		return;

	printf("Config Descriptor: (0x%p)\n", (void *)cd);

	printf("Length                          : 0x0%x\n", cd->length);
	printf("DescriptorIdn                   : 0x0%x\n",
	       cd->descriptor_idn);
	printf("ConfDescContinue                : 0x0%x\n",
	       cd->conf_desc_continue);

	printf("BootEnable                      : 0x0%x\n",
	       cd->boot_enable);
	printf("DescrAccess_en                   : 0x0%x\n",
	       cd->descr_access_en);
	printf("InitPowerMode                   : 0x0%x\n",
	       cd->initpower_mode);
	printf("HighPriorityLun                 : 0x0%x\n",
	       cd->high_priority_lun);
	printf("SecureRemovalType               : 0x0%x\n",
	       cd->secure_removal_type);
	printf("InitActiveIccLevel              : 0x0%x\n",
	       cd->init_active_icc_level);
	printf("PeriodicRtcUpdate               : 0x0%x\n",
	       cd->periodic_rtc_update);
	printf("Reserved_HPB                    : 0x0%x\n",
	       cd->reserved_HPB);

	printf("RpmbRegionEnable                : 0x0%x\n",
	       cd->rpmb_region_enable);
	printf("RpmbRegion1Size                 : 0x0%x\n",
	       cd->rpmb_region1_size);
	printf("RpmbRegion2Size                 : 0x0%x\n",
	       cd->rpmb_region2_size);
	printf("RpmbRegion3Size                 : 0x0%x\n",
	       cd->rpmb_region3_size);

	printf("WriteBoosterBufferPreserveUserSpaceEN : 0x0%x\n",
	       cd->write_booster_buffer_preserve_user_space_en);
	printf("WriteBoosterBufferType           : 0x0%x\n",
	       cd->write_booster_buffer_type);
	printf("NumSharedWriteBoosterBufferAllocUnits : %d\n",
	       be32_to_cpu(cd->num_shared_write_booster_buffer_allocunits));
}

int ufshcd_query_desc_idn_config(struct ufs_hba *hba,
				 struct ufs_config_desc *cfg)
{
	int param_offset = 0;
	int desc_index = 0;
	int ret;

	ret = ufshcd_read_desc_param(hba, QUERY_DESC_IDN_CONFIGURATION,
				     desc_index, param_offset,
				     (void *)cfg,
				     QUERY_DESC_CONFIGURATION_DEF_SIZE);
	if (ret)
		log_err("Failed to read config\n");

	return ret;
}

void ufshcd_print_unit_param_descriptor(struct ufs_unit_configurable_parameters
					*unit)
{
	/* Header segment (Offset 00h - 03h) */
	printf("LuEnable  | 0x%x  | Enabled, 00h: Disabled\n",
	       unit->lu_enable);
	printf("BootLunId | 0x%x  | Boot LU A, 02h: Boot LU B\n",
	       unit->boot_lun_id);
	printf("LuWriteProtect | 0x%x | power-on, 02h: Permanent\n",
	       unit->lu_write_protect);
	printf("MemoryType| 0x%x | Normal, 01h: System Code\n",
	       unit->memory_type);
	/* Allocation and Sizing (Offset 04h - 0Ah) */
	printf("NumAllocunits | %d | Number of Allocation units for LU\n",
	       be32_to_cpu(unit->num_allocunits));
	printf("DataReliability | 0x%x | power failure protection\n",
	       unit->data_reliability);
	printf("LogicalBlockSize | 0x%x | Block size exponent (Min: 0Ch)\n",
	       unit->logical_block_size);
	printf("ProvisioningType | 0x%x | Full, 02h/03h: Thin\n",
	       unit->provisioning_type);

	/* Context and HPB (Offset 0Bh - 15h) */
	puts("ContextCapabilities");
	printf("	| 0x%x | LU Context support/multipliers\n",
	       unit->context_capabilities);
	/* printf("Reserved  | --   | Reserved\n"); */
	/* printf("Reserved (HPB) | -- | Reserved for HPB Extension\n"); */
	/* WriteBooster (Offset 16h) */
	printf("LuNumWriteBoosterBufferAllocunits");
	printf("| %d | WB Buffer size (LUs 0-7 only)\n",
	       be32_to_cpu(unit->lu_num_write_booster_buffer_allocunits));
}

void ufshcd_clearall_luns(struct ufs_config_desc *user_cfg,
			  u32 base_offset, u32 config_length)

{
	struct ufs_unit_configurable_parameters *unit;
	u8 *config = (u8 *)user_cfg;
	int off;
	int i;

	for (i = 0; i < 8; i++) {
		off = base_offset + (i * config_length);

		unit = (struct ufs_unit_configurable_parameters *)&config[off];

		unit->num_allocunits = cpu_to_be32(0);
		unit->memory_type = 0;
		unit->lu_enable = 0;
		unit->boot_lun_id = 0;
		unit->logical_block_size = 0xC;
		unit->data_reliability = 0;
		unit->provisioning_type = 0;
		unit->context_capabilities = cpu_to_be16(0);
		unit->lu_num_write_booster_buffer_allocunits = cpu_to_be32(0);
	}
}

static float calc_lun_capacity(struct ufs_unit_configurable_parameters *unit,
			       struct ufs_geometry_desc *geometry)
{
	u16 cap_adj_fac = 1;
	float cap_adj_facfl = 1;
	u64 lun_capacity;

	switch (unit->memory_type) {
	case 0:	/* Normal */
		cap_adj_fac = 1;
		break;
	case 1:
		cap_adj_fac = be16_to_cpu(geometry->system_code_cap_adj_fac);
		break;
	case 2:
		cap_adj_fac = be16_to_cpu(geometry->non_persist_cap_adj_fac);
		break;
	case 3:
		cap_adj_fac = be16_to_cpu(geometry->enhanced1_cap_adj_fac);
		break;
	case 4:
		cap_adj_fac = be16_to_cpu(geometry->enhanced2_cap_adj_fac);
		break;
	case 5:
		cap_adj_fac = be16_to_cpu(geometry->enhanced3_cap_adj_fac);
		break;
	case 6:
		cap_adj_fac = be16_to_cpu(geometry->enhanced4_cap_adj_fac);
		break;
	default:
		log_err("ilegal memory type\n");
	}

	if (unit->memory_type) {
		/* See geometry Descriptor/cap_adj_fac in Jedec. */
		cap_adj_facfl = (float)(cap_adj_fac / 256.00);
	}

	lun_capacity = 512 * (u64)be32_to_cpu(unit->num_allocunits) *
		    geometry->allocationunit_size *
		    (u64)be32_to_cpu(geometry->segment_size);
	lun_capacity /= cap_adj_facfl;

	return (u32)(lun_capacity >> 20);
}

void ufs_tool_print_luns(void)
{
	struct ufs_geometry_desc geometry;
	struct ufs_config_desc config_desc;
	struct ufs_device_desc device_desc;
	struct ufs_hba *hba;
	const int COL_WIDTH  = 14;
	u8 *config;
	int i;

	hba = ufstool_probe_dev(hba_id);
	if (!hba) {
		log_err("ufs_tool: Failed to get hba\n");
		return;
	}
	if (ufshcd_query_desc_idn_config(hba, &config_desc)) {
		log_err("Failed to read configuration\n");
		return;
	}

	if (ufshcd_query_desc_idn_device(hba, &device_desc)) {
		log_err("Failed to read DeviceConfig IDN\n");
		return;
	}

	if (ufshcd_query_desc_idn_geometry(hba, &geometry)) {
		log_err("Failed to read Geometry IDN\n");
		return;
	}

	config = (u8 *)&config_desc;

	for (i = 0; i < 8; i++) {
		struct ufs_unit_configurable_parameters *unit;
		char buf[32];
		int off;

		off = device_desc.ud0_base_offset +
		      (i * device_desc.config_length);

		unit = (struct ufs_unit_configurable_parameters *)&config[off];

		if (unit->lu_enable != 1)
			continue;

		snprintf(buf, sizeof(buf), "LUN %d", i);
		printf("| %-*s", COL_WIDTH - 2, buf);
	}

	printf("|\n");

	for (i = 0; i < 8; i++) {
		struct ufs_unit_configurable_parameters *unit;
		char size_str[32];
		int off;
		s64 mb;

		off = device_desc.ud0_base_offset + (i * device_desc.config_length);
		unit = (struct ufs_unit_configurable_parameters *)&config[off];

		if (unit->lu_enable != 1)
			continue;

		mb = calc_lun_capacity(unit, &geometry);
		snprintf(size_str, sizeof(size_str), "%lld MB", mb);
		printf("| %-*s", COL_WIDTH - 2, size_str);
	}

	printf("|\n");
}

static void ufshcd_print_idn_desc_config(struct ufs_config_desc *user_cfg,
					 struct ufs_geometry_desc *geometry,
					 u32 base_offset, u32 config_ength)
{
	struct ufs_unit_configurable_parameters *unit;
	u8 *config = (u8 *)user_cfg;
	u32 mb;
	int off;
	int i;

	ufshcd_print_config_descriptor(&user_cfg->cfg_desc);
	for (i = 0; i < 8; i++) {
		off = base_offset + (i * config_ength);

		unit = (struct ufs_unit_configurable_parameters *)&config[off];
		mb = calc_lun_capacity(unit, geometry);
		printf("-------------------------------------------------\n");
		printf("LUN %d Capacity %d MB\n", i, mb);
		printf("Name  | Value      | Description\n");
		printf("-------------------------------------------------\n");
		ufshcd_print_unit_param_descriptor(unit);
	}
}

void print_power_element(struct ufs_power_parameter_element desc)
{
	static const char * const units[] = { "n_a", "u_a", "m_a", "A" };
	u16 element = desc.upower_value;

	u8 unit_code = (element >> 14) & 0x03;
	u16 value = element & 0x03FF;

	printf("%d %s", value, units[unit_code]);
}

void print_ufs_power_parameters(struct ufs_power_param_desc *desc)
{
	printf("--- UFS power Parameters Descriptor (IDN: 08h) ---\n");
	printf("Descriptor Length: 0x%x (%u bytes)\n", desc->length, desc->length);
	printf("Descriptor IDN:    0x%x\n\n", desc->descriptor_idn);

	printf("Level | VCC Current | VCCQ Current | VCCQ2 Current\n");
	printf("--------------------------------------------------\n");

	for (int i = 0; i < 5; i++) {
		printf(" %x  | ", i);
		/* Accessing the  levels for each supply (VCC, VCCQ, VCCQ2) */
		/* Values represent worst-case maximum peak current */
		print_power_element(desc->active_icc_levels_vcc[i]);
		printf("      | ");
		print_power_element(desc->active_icc_levels_vccq[i]);
		printf("       | ");
		print_power_element(desc->active_icc_levels_vccq2[i]);
		printf("\n");
	}
}

int ufshcd_query_desc_idn_power(struct ufs_hba *hba, int lun,
				struct ufs_power_param_desc
				*desc)
{
	int ret;

	ret = ufshcd_read_desc_param(hba, QUERY_DESC_IDN_POWER,
				     lun, 0, (void *)desc, QUERY_DESC_POWER_DEF_SIZE);

	if (ret) {
		log_err("Err: UFS power Descriptor");
		return ret;
	}

	return ret;
}

int ufshcd_query_desc_idn_unit(struct ufs_hba *hba, int lun,
			       struct unit_desc *desc)
{
	int ret;

	ret = ufshcd_read_desc_param(hba, QUERY_DESC_IDN_UNIT, lun, 0,
				     (void *)desc, QUERY_DESC_UNIT_DEF_SIZE);
	if (ret) {
		log_err("Err: UFS unit Descriptor");
		return ret;
	}

	return ret;
}

void ufshcd_print_unit_desc(struct unit_desc *desc)
{
	printf("--- UFS unit Descriptor (IDN: 02h) ---\n");
	printf(" Name   | Value              | Description\n");
	printf("---------------------------------------------\n");

	/* Descriptor Header */
	printf("Length | %d    | Size of descriptor\n", desc->length);
	printf("DescriptorIdn | 0x%x | unit Descriptor ID\n",
	       desc->descriptor_idn);
	printf("BunitIndex     | 0x%x | Logical unit Index\n",
	       desc->bunit_index);

	/* configuration Parameters */
	printf("luEnable | 0x%x | LU Enabled (01h) or Disabled (00h)\n",
	       desc->lu_enable);
	printf("BootLunId | 0x%x| Boot LU ID (01h: A, 02h: B\n",
	       desc->boot_lun_id);
	printf("luWriteProtect | 0x%x | WP (01h: power-on, 02h: Perm)\n",
	       desc->lu_write_protect);
	printf("luQueueDepth | 0x%x | Queue depth (0 if shared)\n",
	       desc->lu_queue_depth);
	printf("psaSensitive | 0x%x  | Soldering sensitivity indicator\n",
	       desc->psa_sensitive);
	printf("MemoryType | 0x%x  | Memory type (00h: Normal\n",
	       desc->memory_type);
	printf("DataReliability | 0x%x | power failure protection enabled\n",
	       desc->data_reliability);
	printf("LogicalBlockSize| 0x%x | Block size exponent (2^n)\n",
	       desc->logical_block_size);

	printf("LogicalBlockCount | %d %d | Total addressable blocks\n",
	       be32_to_cpu(upper_32_bits(desc->logical_block_count)),
	       be32_to_cpu(lower_32_bits(desc->logical_block_count)));
	printf("EraseBlockSize | 0x%x | Optimal erase granularity\n",
	       desc->erase_block_size);
	printf("ProvisioningType | 0x%x | 00h: Full, 02h/03h: Thin\n",
	       desc->provisioning_type);
	printf("phyMemResourceCount| %d %d  Available physical resources\n",
	       lower_32_bits(desc->phy_mem_resource_count),
	       upper_32_bits(desc->phy_mem_resource_count));

	printf("ContextCapabilities | 0x%x | Context ID & LU Multiplier\n",
	       desc->context_capabilities);
	printf("LargeunitGranularity_M1|");
	printf("0x%x | Large unit granularity minus one\n",
	       desc->largeunit_granularity_M1);
	puts("luNumWriteBoosterBufferAllocunits");
	printf(" | %d | WB Buffer size in Alloc units\n",
	       be32_to_cpu(desc->lu_num_write_booster_buffer_allocunits));
}

int set_param_field(struct ufs_unit_configurable_parameters *ud,
		    const char *name, u32 value)
{
	if (!strcmp(name, "boot_lun_id"))
		ud->boot_lun_id = (u8)value;
	else if (!strcmp(name, "lu_enable"))
		ud->lu_enable = (u8)value;
	else if (!strcmp(name, "lu_write_protect"))
		ud->lu_write_protect = (u8)value;
	else if (!strcmp(name, "memory_type"))
		ud->memory_type = (u8)value;
	else if (!strcmp(name, "data_reliability"))
		ud->data_reliability = (u8)value;
	else if (!strcmp(name, "num_allocunits"))
		ud->num_allocunits = cpu_to_be32(value);
	else if (!strcmp(name, "provisioning_type"))
		ud->provisioning_type = (u8)value;
	else if (!strcmp(name, "context_capabilities"))
		ud->context_capabilities = (u16)value;
	else if (!strcmp(name, "lu_num_write_booster_buffer_allocunits"))
		ud->lu_num_write_booster_buffer_allocunits = (u32)value;
	else
		return -1;

	return 0;
}

int ufshcd_get_lun(struct ufs_hba *hba, int lun)
{
	struct ufs_unit_configurable_parameters *unit;
	struct ufs_device_desc dev_desc;
	u8 *config;
	int off;

	// Populate descriptor
	if (g_cfg_desc.cfg_desc.length == 0 &&
	    ufshcd_query_desc_idn_config(hba, &g_cfg_desc))
		return -EINVAL;

	if (ufshcd_query_desc_idn_device(hba, &dev_desc)) {
		log_err("Failed to read IDN DeviceDesc IDN\n");
		return -EINVAL;
	}

	config = (__u8 *)&g_cfg_desc;
	off = dev_desc.ud0_base_offset + (lun * dev_desc.config_length);
	unit = (struct ufs_unit_configurable_parameters *)&config[off];
	ufshcd_print_unit_param_descriptor(unit);
	return 0;
}

int ufshcd_update_lun(struct ufs_hba *hba, int lun,
		      const char *field, u32 val)
{
	struct ufs_unit_configurable_parameters *unit;
	struct ufs_device_desc dev_desc;
	u8 *config;
	int off;
	int ret;

	// Populate descriptor
	if (g_cfg_desc.cfg_desc.length == 0 &&
	    ufshcd_query_desc_idn_config(hba, &g_cfg_desc))
		return -EINVAL;

	if (ufshcd_query_desc_idn_device(hba, &dev_desc)) {
		log_err("Failed to read IDN DeviceDesc IDN\n");
		return -EINVAL;
	}

	config = (__u8 *)&g_cfg_desc;
	off = dev_desc.ud0_base_offset + (lun * dev_desc.config_length);
	unit = (struct ufs_unit_configurable_parameters *)&config[off];
	unit->logical_block_size = 0xC;
	// Always enable the Lun
	unit->lu_enable = 1;
	if (set_param_field(unit, field, val)) {
		log_err("Ilegal value to set %s\n", field);
		return -EINVAL;
	}
	ufshcd_print_unit_param_descriptor(unit);

	return ret;
}

static int ufshcd_write_desc_param(struct ufs_hba *hba, enum desc_idn desc_id,
				   int desc_index, unsigned char param_offset,
				   unsigned char *param_write_buf,
				   unsigned char param_size)
{
	int ret;
	int buff_len;
	int selector = 0;

	if (desc_id >= QUERY_DESC_IDN_MAX || !param_size)
		return -EINVAL;

	buff_len = param_size;
	ret = ufshcd_query_descriptor_retry(hba, UPIU_QUERY_OPCODE_WRITE_DESC,
					    desc_id, desc_index, selector,
					    param_write_buf, &buff_len);
	if (!ret)
		return 0;
	log_err("Err: desc_id %d desc_index %d, param_offset %d,buflen %d,ret=%d\n",
		(unsigned int)desc_id, desc_index, (unsigned int)param_offset, buff_len, ret);

	return ret;
}

enum flag_idn ufshcd_get_flag(const char *name)
{
	if (!strcmp(name, "deviceinit"))
		return QUERY_FLAG_IDN_FDEVICEINIT;

	if (!strcmp(name, "permanent_wpe"))
		return QUERY_FLAG_IDN_PERMANENT_WPE;

	if (!strcmp(name, "pwr_on_wpe"))
		return QUERY_FLAG_IDN_PWR_ON_WPE;

	if (!strcmp(name, "bkops_en"))
		return QUERY_FLAG_IDN_BKOPS_EN;

	if (!strcmp(name, "life_span_mode"))
		return QUERY_FLAG_IDN_LIFE_SPAN_MODE_ENABLE;

	if (!strcmp(name, "purge_enable"))
		return QUERY_FLAG_IDN_PURGE_ENABLE;

	if (!strcmp(name, "fphy_resource_removal"))
		return QUERY_FLAG_IDN_FPHYRESOURCEREMOVAL;

	if (!strcmp(name, "busy_rtc"))
		return QUERY_FLAG_IDN_BUSY_RTC;

	if (!strcmp(name, "permanently_disable_fw_update"))
		return QUERY_FLAG_IDN_PERMANENTLY_DISABLE_FW_UPDATE;

	if (!strcmp(name, "write_booster_en"))
		return QUERY_FLAG_IDN_F_WRITE_BOOSTER_EN;

	if (!strcmp(name, "wb_buf_flush_en"))
		return QUERY_FLAG_IDN_F_WB_BUF_FLUSH_EN;

	if (!strcmp(name, "wb_buf_flush_h8"))
		return QUERY_FLAG_IDN_F_WB_BUF_FLUSH_HIBERN8;

	return -EINVAL;
}

int ufshcd_query_user_flag(struct ufs_hba *hba, const char *flag_name)
{
	enum flag_idn idn;
	bool flag_res;
	int ret;

	if (!hba) {
		log_err("QueryFlag: invalid hba\n");
		return -EINVAL;
	}

	idn = ufshcd_get_flag(flag_name);
	if ((int)idn < 0) {
		log_err("QueryFlag: invalid flag name\n");
		return -EINVAL;
	}
	ret = ufshcd_query_flag(hba, UPIU_QUERY_OPCODE_READ_FLAG, idn, &flag_res);
	if (ret) {
		log_err("QueryFlag: flag %s idn query failed\n", flag_name);
		return ret;
	}
	printf("%s = %x\n", flag_name, flag_res);

	return 0;
}

#define CMD_HELP \
		"****************************\n" \
		"\n\tgetflag [flag name]\n" \
		"\t\tdeviceinit\n" \
		"\t\tpermanent_wpe\n" \
		"\t\tpwr_on_wpe\n" \
		"\t\tbkops_en\n" \
		"\t\tlife_span_mode\n" \
		"\t\tpurge_enable\n" \
		"\t\tfphy_resource_removal\n" \
		"\t\tbusy_rtc\n" \
		"\t\tpermanently_disable_fw_update\n" \
		"\t\twrite_booster_en\n" \
		"\t\twb_buf_flush_en\n" \
		"\t\twb_buf_flush_h8\n" \
		"****************************\n" \
		"\n\tset_cfg_desc [parm] [value]\n" \
		"\t\tlength\n" \
		"\t\tdescriptor_idn\n" \
		"\t\tconf_desc_continue\n" \
		"\t\tboot_enable\n" \
		"\t\tdescr_access_en\n" \
		"\t\tinitpower_mode\n" \
		"\t\thigh_priority_lun\n" \
		"\t\tsecure_removal_type\n" \
		"\t\tinit_active_icc_level\n" \
		"\t\tperiodic_rtc_update\n" \
		"\t\treserved_HPB\n" \
		"\t\trpmb_region_enable\n" \
		"\t\trpmb_region1_size\n" \
		"\t\trpmb_region2_size\n" \
		"\t\trpmb_region3_size\n" \
		"\t\twrite_booster_buffer_preserve_user_space_en\n" \
		"\t\twrite_booster_buffer_type\n" \
		"\t\tnum_shared_write_booster_buffer_allocunits\n" \
		"****************************\n" \
		"\tufs commit\n" \
		"\tclearall\n" \
		"\tset_hba [HBAID]\n" \
		"\tluns\n" \
		"\tget_lun [LUN]\n" \
		"****************************\n" \
		"\n\tset_lun [LUN] (parm)\n" \
		"\t\tlu_enable\n" \
		"\t\tboot_lun_id\n" \
		"\t\tlu_write_protect\n" \
		"\t\tmemory_type\n" \
		"\t\tdata_reliability\n" \
		"\t\tnum_allocunits\n" \
		"\t\tlogical_block_size\n" \
		"\t\tprovisioning_type\n" \
		"\t\tcontext_capabilities\n" \
		"\t\tlu_num_write_booster_buffer_allocunits\n\n" \
		"****************************\n" \
		"query:\n" \
		"\tDevice desc (0h)\n" \
		"\tGeometry desc(7h)\n" \
		"\tPower desc (8h)\n" \
		"\tUnit desc(2h)\n" \
		"\tConfig desc(1h)\n"

void ufs_tool_help(void)
{
	puts(CMD_HELP);
}

void idn_read(struct ufs_hba *hba, int idn, int lun)
{
	void *desc = NULL;

	switch (idn) {
	case QUERY_DESC_IDN_DEVICE:
		desc = malloc(sizeof(struct ufs_device_desc));
		if (!desc) {
			log_err("Failed to allocate device descriptor\n");
			return;
		}

		if (!ufshcd_query_desc_idn_device(hba, desc))
			ufshcd_print_device_descriptor(desc);
		break;

	case QUERY_DESC_IDN_GEOMETRY:
		desc = malloc(sizeof(struct ufs_geometry_desc));
		if (!desc) {
			log_err("Failed to allocate geometry descriptor\n");
			return;
		}

		if (!ufshcd_query_desc_idn_geometry(hba, desc))
			ufshcd_print_idn_geometry_desc(desc);
		else
			log_err("Failed to read Geometry\n");
		break;

	case QUERY_DESC_IDN_UNIT:
		desc = malloc(sizeof(struct unit_desc));
		if (!desc) {
			log_err("Failed to allocate unit descriptor\n");
			return;
		}

		if (!ufshcd_query_desc_idn_unit(hba, lun, desc))
			ufshcd_print_unit_desc(desc);
		else
			log_err("Failed to read Unit Descriptor\n");
		break;

	case QUERY_DESC_IDN_POWER:
		desc = malloc(sizeof(struct ufs_power_param_desc));
		if (!desc) {
			log_err("Failed to allocate power descriptor\n");
			return;
		}

		if (!ufshcd_query_desc_idn_power(hba, 0, desc))
			print_ufs_power_parameters(desc);
		else
			log_err("Failed to read Power Descriptor\n");
		break;

	case QUERY_DESC_IDN_CONFIGURATION: {
		struct ufs_device_desc *device_desc;
		struct ufs_geometry_desc *geometry;

		device_desc = malloc(sizeof(*device_desc));
		if (!device_desc) {
			log_err("Failed to allocate device descriptor\n");
			return;
		}

		geometry = malloc(sizeof(*geometry));
		if (!geometry) {
			log_err("Failed to allocate geometry descriptor\n");
			free(device_desc);
			return;
		}

		if (ufshcd_query_desc_idn_device(hba, device_desc)) {
			log_err("Failed to read IDN DeviceDesc\n");
			goto cfg_out;
		}

		if (ufshcd_query_desc_idn_geometry(hba, geometry)) {
			log_err("Failed to read Geometry\n");
			goto cfg_out;
		}

		if (ufshcd_query_desc_idn_config(hba, &g_cfg_desc)) {
			log_err("Failed to read Configuration Descriptor\n");
			goto cfg_out;
		}

		ufshcd_print_idn_desc_config(&g_cfg_desc, geometry,
					     device_desc->ud0_base_offset,
					     device_desc->config_length);

cfg_out:
		free(geometry);
		free(device_desc);
		return;
	}

	default:
		log_err("Illegal IDN %d\n", idn);
		ufs_tool_help();
		return;
	}

	free(desc);
}

static int set_config_desc_field(struct config_descriptor *desc,
				 const char *fieldname, u32 value)
{
	if (!strcmp(fieldname, "length"))
		desc->length = (u8)value;
	else if (!strcmp(fieldname, "descriptor_idn"))
		desc->descriptor_idn = (u8)value;
	else if (!strcmp(fieldname, "conf_desc_continue"))
		desc->conf_desc_continue = (u8)value;
	else if (!strcmp(fieldname, "boot_enable"))
		desc->boot_enable = (u8)value;
	else if (!strcmp(fieldname, "descr_access_en"))
		desc->descr_access_en = (u8)value;
	else if (!strcmp(fieldname, "initpower_mode"))
		desc->initpower_mode = (u8)value;
	else if (!strcmp(fieldname, "high_priority_lun"))
		desc->high_priority_lun = (u8)value;
	else if (!strcmp(fieldname, "secure_removal_type"))
		desc->secure_removal_type = (u8)value;
	else if (!strcmp(fieldname, "init_active_icc_level"))
		desc->init_active_icc_level = (u8)value;
	else if (!strcmp(fieldname, "periodic_rtc_update"))
		desc->periodic_rtc_update = (u16)value;
	else if (!strcmp(fieldname, "reserved_HPB"))
		desc->reserved_HPB = (u8)value;
	else if (!strcmp(fieldname, "rpmb_region_enable"))
		desc->rpmb_region_enable = (u8)value;
	else if (!strcmp(fieldname, "rpmb_region1_size"))
		desc->rpmb_region1_size = (u8)value;
	else if (!strcmp(fieldname, "rpmb_region2_size"))
		desc->rpmb_region2_size = (u8)value;
	else if (!strcmp(fieldname, "rpmb_region3_size"))
		desc->rpmb_region3_size = (u8)value;
	else if (!strcmp(fieldname, "write_booster_buffer_preserve_user_space_en"))
		desc->write_booster_buffer_preserve_user_space_en = (u8)value;
	else if (!strcmp(fieldname, "write_booster_buffer_type"))
		desc->write_booster_buffer_type = (u8)value;
	else if (!strcmp(fieldname, "num_shared_write_booster_buffer_allocunits"))
		desc->num_shared_write_booster_buffer_allocunits =
		cpu_to_be32(value);
	else
		return -EINVAL;

	return 0;
}

int ufs_tool_set_config_desc(const char *name, u32 value)
{
	struct ufs_hba *hba = ufstool_probe_dev(hba_id);

	if (g_cfg_desc.cfg_desc.length == 0) {
		if (ufshcd_query_desc_idn_config(hba, &g_cfg_desc)) {
			log_err("Failed to read Device Descriptor\n");
			return -EINVAL;
		}
	}

	if (set_config_desc_field(&g_cfg_desc.cfg_desc, name, value)) {
		log_err("Failed to set parameters %s descriptor\n", name);
		return -EINVAL;
	}

	ufshcd_print_config_descriptor(&g_cfg_desc.cfg_desc);

	return 0;
}

int ufs_tool_commit(void)
{
	struct ufs_hba *hba = ufstool_probe_dev(hba_id);
	u8 *config;
	int ret;

	config = (__u8 *)&g_cfg_desc;
	ufshcd_cache_flush(config, QUERY_DESC_UNIT_DEF_SIZE);
	ret = ufshcd_write_desc_param(hba,
				      QUERY_DESC_IDN_CONFIGURATION,
				      0, 0, config,
				      g_cfg_desc.cfg_desc.length);

	if (ret)
		log_err("Failed to write Configuration\n");

	return ret;
}

int ufs_tool_clearall(void)
{
	struct ufs_hba *hba = ufstool_probe_dev(hba_id);
	struct ufs_device_desc device_desc;
	u8 *config;
	int ret;

	// Populate descriptor
	if (g_cfg_desc.cfg_desc.length == 0 &&
	    ufshcd_query_desc_idn_config(hba, &g_cfg_desc)) {
		return -EINVAL;
	}
	config = (__u8 *)&g_cfg_desc;

	ufshcd_cache_flush(config, QUERY_DESC_UNIT_DEF_SIZE);
	ret = ufshcd_query_desc_idn_device(hba, &device_desc);
	if (ret) {
		log_err("Failed to read DeviceDesc\n");
		return ret;
	}

	ufshcd_clearall_luns(&g_cfg_desc,
			     device_desc.ud0_base_offset,
			     device_desc.config_length);

	ret = ufshcd_write_desc_param(hba,
				      QUERY_DESC_IDN_CONFIGURATION,
				      0, 0, config,
				      g_cfg_desc.cfg_desc.length);

	if (ret)
		log_err("Failed to clear Configuration\n");

	return ret;
}

int ufs_tool_get_lun(int lun)
{
	struct ufs_hba *hba = ufstool_probe_dev(hba_id);

	if (!hba)
		return -EINVAL;
	return ufshcd_get_lun(hba, lun);
}

int ufs_tool_set_lun(int lun, const char *parm, u32 val)
{
	struct ufs_hba *hba = ufstool_probe_dev(hba_id);

	if (!hba)
		return -EINVAL;
	return ufshcd_update_lun(hba, lun, parm, val);
}

int ufs_tool_query_flag(const char *flag)
{
	struct ufs_hba *hba = ufstool_probe_dev(hba_id);

	if (!hba)
		return -EINVAL;
	return ufshcd_query_user_flag(hba, flag);
}

int ufs_tool_query(int lun, int idn)
{
	struct ufs_hba *hba;

	hba = ufstool_probe_dev(hba_id);
	if (!hba) {
		log_err("ufs_tool: failed to get hba\n");
		return -1;
	}

	if (idn >= 0) {
		idn_read(hba, idn, lun);
		return 0;
	}
	ufs_tool_help();

	return -EINVAL;
}

int ufs_tool_set_hba(int val)
{
	if (val < 0) {
		printf("ufs_tool: invalid hba id\n");
		return -1;
	}
	hba_id = val;
	return 0;
}

static int do_ufs(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])
{
	int dev, ret;

	if (argc >= 2) {
		if (!strcmp(argv[1], "init")) {
			if (argc == 3) {
				dev = dectoul(argv[2], NULL);
				ret = ufs_probe_dev(dev);
				if (ret)
					return CMD_RET_FAILURE;
			} else {
				ufs_probe();
			}
			return CMD_RET_SUCCESS;
		}

		if (!strcmp(argv[1], "query")) {
			int lun = -1;
			int idn = -1;

			if (argc <= 2) {
				printf("usage: idn [lun]");
				return CMD_RET_FAILURE;
			}
			idn  = dectoul(argv[2], NULL);
			if (argc == 4) 
				lun  = dectoul(argv[3], NULL);
			return ufs_tool_query(lun, idn);
		}

		if (!strcmp(argv[1], "set_hba")) {
			int val;

			if (argc <= 2) {
				ufs_tool_help();
				return CMD_RET_FAILURE;
			}
			val  = dectoul(argv[2], NULL);
			return ufs_tool_set_hba(val);
		}

		if (!strcmp(argv[1], "get_lun")) {
			int lun;

			if (argc <= 2) {
				ufs_tool_help();
				return CMD_RET_FAILURE;
			}
			lun  = dectoul(argv[2], NULL);
			return ufs_tool_get_lun(lun);
		}

		if (!strcmp(argv[1], "set_lun")) {
			int lun;
			u32 val;

			if (argc <= 4) {
				ufs_tool_help();
				return CMD_RET_FAILURE;
			}
			lun  = dectoul(argv[2], NULL);
			val  = dectoul(argv[4], NULL);
			return ufs_tool_set_lun(lun, argv[3], val);
		}

		if (!strcmp(argv[1], "set_cfg_desc")) {
			const char *name;
			u32 val;

			if (argc <= 3) {
				ufs_tool_help();
				return CMD_RET_FAILURE;
			}
			name = argv[2];
			val  = dectoul(argv[3], NULL);
			return ufs_tool_set_config_desc(name, val);
		}

		if (!strcmp(argv[1], "getflag")) {
			if (argc == 2) {
				printf("usage: getflag [flag name]");
				ufs_tool_help();
				return CMD_RET_FAILURE;
			}
			return ufs_tool_query_flag(argv[2]);
		}

		if (!strcmp(argv[1], "commit"))
			return ufs_tool_commit();

		if (!strcmp(argv[1], "clearall"))
			return ufs_tool_clearall();

		if (!strcmp(argv[1], "luns")) {
			ufs_tool_print_luns();
			return 0;
		}
	}
	ufs_tool_help();
	return CMD_RET_FAILURE;
}

U_BOOT_CMD(ufs, 5, 1, do_ufs,
	   "UFS sub-system",
	   "init [dev] - init UFS subsystem\n"
	   "query [LUN] [IDN]\n"
	   "set_lun [LUN] [parm name] [value]\n"
	   "get_lun [LUN]\n"
	   "set_cfg_des  [parm name] [value]\n"
	   "luns - prints luns\n"
	   "clearall - zero out configuration descriptor\n"
	   "getflag [flag name]\n"
	   "commit - writes luns configuration\n"
);
