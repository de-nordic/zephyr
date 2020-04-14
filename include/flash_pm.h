#ifndef ZEPHYR_INCLUDE_FLASH_PM_H_
#define ZEPHYR_INCLUDE_FLASH_PM_H_

/* TODO: Add conditional selection of custom PM manager instead of code
 * below */
/* ??? How to do automatic ID increment? */
/* ??? How to automatically update FLASH_AREA_NUM? */


#define FLASH_AREA_OFFSET(lbl)	DT_REG_ADDR(DT_NODELABEL(lbl))
#define FLASH_AREA_SIZE(lbl)	DT_REG_SIZE(DT_NODELABEL(lbl))
#define FLASH_AREA_PTABLE(lbl)	DT_PARENT(DT_NODELABEL(lbl)
#define FLASH_AREA_DEV(lbl)	DT_PARENT(FLASH_AREA_PTABLE(lbl))
#define FLASH_AREA_DEV_LABEL(lbl)	DT_LABEL(FLASH_AREA_DEV(lbl))
#define FLASH_AREA_HAS_PARTITION(lbl) DT_HAS_NODE(DT_NODELABEL(lbl))


#if FLASH_AREA_HAS_PARTITION(boot_partition)
#define FLASH_AREA_MCBOOT_ID	0
#define FLASH_AREA_0_OFFSET	FLASH_AREA_OFFSET(boot_partition)
#define FLASH_AREA_0_DEV_LABEL	FLASH_AREAD_DEV_LABEL(boot_partition)
#define FLASH_AREA_0_SIZE	FLASH_AREA_SIZE(boot_partition)
#endif


#if FLASH_AREA_HAS_PARTITION(slot0_partition)
#define FLASH_AREA_IMAGE_0_ID	1
#define FLASH_AREA_1_OFFSET	FLASH_AREA_OFFSET(slot0_partition)
#define FLASH_AREA_1_DEV_LABEL	FLASH_AREAD_DEV_LABEL(slot0_partition)
#define FLASH_AREA_1_SIZE	FLASH_AREA_SIZE(slot0_partition)
#endif

#if FLASH_AREA_HAS_PARTITION(slot1_partition)
#define FLASH_AREA_IMAGE_1_ID	2
#define FLASH_AREA_2_OFFSET    FLASH_AREA_OFFSET(slot1_partition)
#define FLASH_AREA_2_DEV_LABEL FLASH_AREAD_DEV_LABEL(slot1_partition)
#define FLASH_AREA_2_SIZE      FLASH_AREA_SIZE(slot1_partition)
#endif


#if FLASH_AREA_HAS_PARTITION(scratch_partition)
#define FLASH_AREA_IMAGE_SCRATCH_ID 3
#define FLASH_AREA_3_OFFSET    FLASH_AREA_OFFSET(scratch_partition)
#define FLASH_AREA_3_DEV_LABEL FLASH_AREAD_DEV_LABEL(scratch_partition)
#define FLASH_AREA_3_SIZE      FLASH_AREA_SIZE(scratch_partition)
#endif


#if FLASH_AREA_HAS_PARTITION(storage_partition)
#define FLASH_AREA_STORAGE_ID	4
#define FLASH_AREA_4_OFFSET	FLASH_AREA_OFFSET(storage_partition)
#define FLASH_AREA_4_DEV_LABEL	FLASH_AREAD_DEV_LABEL(storage_partition)
#define FLASH_AREA_4_SIZE	FLASH_AREA_SIZE(storage_partition)
#endif

#define FLASH_AREA_NUM		5

#endif /* ndef ZEPHYR_INCLUDE_FLASH_PM_H_ */
