
# UAVCAN boot loadable Module ID
set(uavcanblid_sw_version_major 0)
set(uavcanblid_sw_version_minor 1)
add_definitions(
	-DAPP_VERSION_MAJOR=${uavcanblid_sw_version_major}
	-DAPP_VERSION_MINOR=${uavcanblid_sw_version_minor}
)

set(uavcanblid_hw_version_major 3)
set(uavcanblid_hw_version_minor 233)
set(uavcanblid_name "\"org.cuav.can-gps-v1\"")

add_definitions(
	-DHW_UAVCAN_NAME=${uavcanblid_name}
	-DHW_VERSION_MAJOR=${uavcanblid_hw_version_major}
	-DHW_VERSION_MINOR=${uavcanblid_hw_version_minor}
)

px4_add_board(
	PLATFORM nuttx
	VENDOR cuav
	MODEL can-gps-v1
	LABEL canbootloader
	TOOLCHAIN arm-none-eabi
	ARCHITECTURE cortex-m4
	CONSTRAINED_MEMORY
	DRIVERS
		bootloaders
)