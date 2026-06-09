.. SPDX-License-Identifier: GPL-2.0+

ufs command
===========

Synopsis
--------

::

    ufs query <idn> [lun]
    ufs set_cfg_desc <lun> <parameter> <value>
    ufs getflag <flag>
    ufs set_hba <hbaid>
    ufs init
    ufs commit
    ufs clearall
    ufs luns
    

Query Description
------------------

The ``ufs`` command is used to query, display and modify Universal Flash
Storage (UFS) device configuration and descriptor information.

The ``ufs query`` command displays UFS descriptors.

::

    ufs query <idn> [lun]

    idn
        Descriptor identifier.

        ===== =================================
        Value Descriptor
        ===== =================================
        0x00  Device Descriptor
        0x01  Configuration Descriptor
        0x02  Unit Descriptor
        0x07  Geometry Descriptor
        0x08  Power Descriptor
        ===== =================================

    lun
        Logical Unit Number.

        Required only for Unit Descriptor (IDN 0x02).


Descriptor Information
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Device Descriptor (IDN 0x00)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The Device Descriptor contains general information about the UFS device,
including:

* Device identification
* Manufacturer information
* Product name and revision
* Number of logical units
* Queue depth
* Boot configuration
* WriteBooster capabilities
* Supported UFS features

Example:

::

    => ufs query 0

          Name | Value | Description
          ---------------------------------------------  
          Length | 0x59
          DescriptorIdn | 0x0
          Device | 0x0
          DeviceClass | 0x0
          DeviceSubClass | 0x0
          protocol | 0x0
          NumberLu | 0
          NumberWLu | 4
          BootEnable | 0x1
          DescrAccessEn | 0x1
          
          ...
    

Geometry Descriptor (IDN 0x07)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The Geometry Descriptor provides information about the physical layout
and capabilities of the device, including:

* Total raw device capacity
* Maximum number of logical units
* Allocation unit size
* Segment size
* Supported memory types
* RPMB capabilities
* Secure removal support
* WriteBooster limits

Example:

::

    => ufs query 7

        Name   | Value              | Description
        ---------------------------------------------
        Length | 0x57  | Size of descriptor
        DescriptorIdn   | 0x7 | geometry Descriptor ID
        MediaTechnology | 0x0 | Reserved (00h)
        TotalRawDeviceCapacity | 119 GB
        MaxNumberLu | 1 | Max LUs (00h: 8, 01h: 32)
        SegmentSize | 8192 | Segment size (512B units)
        AllocationunitSize | 1 | Alloc unit size (in segments)
   ...


Unit Descriptor (IDN 0x02)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The Unit Descriptor contains information about a specific Logical Unit
(LUN), including:

* LUN enable state
* Boot LUN assignment
* Write protection configuration
* Memory type
* Logical block size
* Provisioning type
* Data reliability settings
* WriteBooster allocation

Example:

::

    => ufs query 2 3

        --- UFS unit Descriptor (IDN: 02h) ---
        Name   | Value              | Description
        ---------------------------------------------
        Length | 45 | Size of descriptor
        DescriptorIdn | 0x2 | unit Descriptor ID
        BunitIndex  | 0x3 | Logical unit Index
        luEnable | 0x0 | LU Enabled (01h) or Disabled (00h)
        BootLunId | 0x0| Boot LU ID (01h: A, 02h: B
        luWriteProtect | 0x0 | WP (01h: power-on, 02h: Perm)
        luQueueDepth | 0x0 | Queue depth (0 if shared)
    ...


Power Descriptor (IDN 0x08)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Queries command of Power IDN. 

  => ufs query 8
        --- UFS power Parameters Descriptor (IDN: 08h) ---

        Descriptor Length: 0x62 (98 bytes)

        Descriptor IDN:    0x8
        
        Level | VCC Current | VCCQ Current | VCCQ2 Current
    
         0  | 0 n_a      | 0 n_a       | 0 n_a
    
         1  | 0 n_a      | 0 n_a       | 0 n_a
    
         2  | 0 n_a      | 0 n_a       | 0 n_a
    
         3  | 0 n_a      | 0 n_a       | 0 n_a
    
         4  | 0 n_a      | 0 n_a       | 0 n_a
        

Configuration Descriptor (IDN 0x01)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The Configuration Descriptor defines the layout and configuration of all
Logical Units on the device.

It contains:

* Boot configuration
* RPMB configuration
* WriteBooster settings
* Logical Unit definitions
* Allocation unit assignments

etc..

Example:

::

    => ufs query 1
        Config Descriptor: (0x00000008ffff6628)
        Length                          : 0x0e6
        DescriptorIdn                   : 0x01
        ConfDescContinue                : 0x00
        BootEnable                      : 0x01
        DescrAccess_en                   : 0x01
        InitPowerMode                   : 0x01
        HighPriorityLun                 : 0x07f
        SecureRemovalType               : 0x02
        InitActiveIccLevel              : 0x01
        PeriodicRtcUpdate               : 0x00
        Reserved_HPB                    : 0x00
        RpmbRegionEnable                : 0x00
        RpmbRegion1Size                 : 0x00
        RpmbRegion2Size                 : 0x00
        RpmbRegion3Size                 : 0x00
        WriteBoosterBufferPreserveUserSpaceEN : 0x01
        WriteBoosterBufferType           : 0x00
        NumSharedWriteBoosterBufferAllocUnits : 0x00
        -------------------------------------------------
        LUN 0 Capacity 16 MB
        Name  | Value      | Description
        -------------------------------------------------
        LuEnable  | 0x1  | Enabled, 00h: Disabled
        BootLunId | 0x1  | Boot LU A, 02h: Boot LU B
        LuWriteProtect | 0x0 | power-on, 02h: Permanent
        MemoryType| 0x3 | Normal, 01h: System Code
        NumAllocunits | 12 | Number of Allocation units for LU
        DataReliability | 0x1 | power failure protection
        LogicalBlockSize | 0xc | Block size exponent (Min: 0Ch)
        ProvisioningType | 0x0 | Full, 02h/03h: Thin
    ...


Examining a LUN
----------------------

The command get_lun reports a lun from the device unit descriptor.

Example:

::


    => ufs get_lun 1

        LuEnable  | 0x1  | Enabled, 00h: Disabled
        BootLunId | 0x2  | Boot LU A, 02h: Boot LU B
        LuWriteProtect | 0x0 | power-on, 02h: Permanent
        MemoryType| 0x3 | Normal, 01h: System Code
        NumAllocunits | 12 | Number of Allocation units for LU
        DataReliability | 0x1 | power failure protection
        LogicalBlockSize | 0xc | Block size exponent (Min: 0Ch)
        ProvisioningType | 0x0 | Full, 02h/03h: Thin
        ContextCapabilities     | 0x0 | LU Context support/multipliers
        LuNumWriteBoosterBufferAllocunits| 16777220 | WB Buffer size (LUs 0-7 only)
    

Creating a LUN
----------------------

The command set_lun updates a LUN configuration. It does not write the LUN to the controller.
To write to the controller issue the command ufs commmit.

    ufs set_lun <lun> <parameter> <value>

Supported parameters
    * lu_enable
    * boot_lun_id
    * lu_write_protect
    * memory_type
    * data_reliability
    * num_allocunits
    * logical_block_size
    * provisioning_type
    * context_capabilities
    * lu_num_write_booster_buffer_allocunits

Example:

::

    => ufs set_lun 0 num_allocunits 4

        LuEnable  | 0x0  | Enabled, 00h: Disabled
        BootLunId | 0x1  | Boot LU A, 02h: Boot LU B
        LuWriteProtect | 0x0 | power-on, 02h: Permanent
        MemoryType| 0x3 | Normal, 01h: System Code
        NumAllocunits | 4 | Number of Allocation units for LU
        DataReliability | 0x1 | power failure protection
        LogicalBlockSize | 0xc | Block size exponent (Min: 0Ch)
        ProvisioningType | 0x0 | Full, 02h/03h: Thin
        ContextCapabilities     | 0x0 | LU Context support/multipliers
        LuNumWriteBoosterBufferAllocunits| 16908291 | WB Buffer size (LUs 0-7 only)
    

Getting A flag
------------------
The command get_flag retrieves a flag value..
    ufs get_flag <parameter>

Supported flags::
    * deviceinit
    * permanent_wpe
    * pwr_on_wpe
    * bkops_en
    * life_span_mode
    * purge_enable
    * fphy_resource_removal
    * busy_rtc
    * permanently_disable_fw_update
    * write_booster_en
    * wb_buf_flush_en
    * wb_buf_flush_h8

Example:

::

    => ufs getflag bkops_en
        bkops_en = 1


Modify Configuration Descriptor fields
-----------------------------------------
    ufs set_cfg_desc [parameter] [value]

Supported parameters::
    * length
    * descriptor_idn
    * conf_desc_continue
    * boot_enable
    * descr_access_en
    * initpower_mode
    * high_priority_lun
    * secure_removal_type
    * init_active_icc_level
    * periodic_rtc_update
    * reserved_HPB
    * rpmb_region_enable
    * rpmb_region1_size
    * rpmb_region2_size
    * rpmb_region3_size
    * write_booster_buffer_preserve_user_space_en
    * write_booster_buffer_type
    * num_shared_write_booster_buffer_allocunits
    
Example:

::

    => ufs set_cfg_desc num_shared_write_booster_buffer_allocunits 100
        

Show enabled Luns    
~~~~~~~~~~~~~~~~~~~~~~~~~~ 

The command ufs luns prints the current enabled luns.

Example:

::
    => ufs luns


        `| LUN 0       | LUN 1       | LUN 2       | LUN 3       | LUN 4       |`

        `| 3 MB       | 10 MB       | 1 MB        | 2312 MB     | 114364 MB   |`
        

Clear all LUN configurations
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The command ufs clearall, clears all LUNs, and auto-commit the zeroed LUNs to the controller.


Set HBA ID
~~~~~~~~~~~~~~~

    ufs set_hba [hbaid]

Use an hba id differs from 0.


Init
~~~~~

    ufs init

Initialize the HBA controller. This command is not necessary and used for debugging.
All other commands initializes the HBA controller automatically if required.

Commit
~~~~~~~~~

    ufs commit

The command ufs commit writes the current configuration descriptor to the controller. In case the configuration 
is incorrect, the commit will fail. Ufs commit is usually done after creating a lun.

Provisioning workflow
----------------------

Source system
~~~~~~~~~~~~~~~

1. Display the Configuration Descriptor::

       ufs query 1

   The output includes the Configuration Descriptor address and size.

2. Save the Configuration Descriptor to a file::

       tftpput <cfg_desc_addr> <size> <filename>

   Example::

       tftpput 0x8ffffc610 0xe6 luns.bin

   This exports the current UFS configuration, including LUN settings.

Target system
^^^^^^^^^^^^^

1. Display the current Configuration Descriptor:

         ufs query 1

2. Load the configuration file into the Configuration Descriptor buffer
   using XMODEM or TFTP:

       loadx 0x8ffffce00

   or:

       tftpget 0x8ffffce00 luns.bin

3. Commit the configuration to the UFS device:

       ufs commit

   The imported Configuration Descriptor is written to the device and
   becomes active after the commit operation completes.
