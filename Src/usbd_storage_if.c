/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : usbd_storage_if.c
  * @version        : v1.0_Cube
  * @brief          : Memory management layer.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "usbd_storage_if.h"

/* USER CODE BEGIN INCLUDE */

/* USER CODE END INCLUDE */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @brief Usb device.
  * @{
  */

/** @defgroup USBD_STORAGE
  * @brief Usb mass storage device module
  * @{
  */

/** @defgroup USBD_STORAGE_Private_TypesDefinitions
  * @brief Private types.
  * @{
  */

/* USER CODE BEGIN PRIVATE_TYPES */

/* USER CODE END PRIVATE_TYPES */

/**
  * @}
  */

/** @defgroup USBD_STORAGE_Private_Defines
  * @brief Private defines.
  * @{
  */

#define STORAGE_LUN_NBR                  1
#define STORAGE_BLK_NBR                  0x10000
#define STORAGE_BLK_SIZ                  0x200

/* USER CODE BEGIN PRIVATE_DEFINES */
#ifdef STORAGE_BLK_SIZ
#undef STORAGE_BLK_SIZ
#endif
#define STORAGE_BLK_SIZ                  512

#ifdef STORAGE_BLK_NBR
#undef STORAGE_BLK_NBR
#endif
#define STORAGE_CAPPACITY                1024*118
#define STORAGE_BLK_NBR                  STORAGE_CAPPACITY / STORAGE_BLK_SIZ
#define MEDIA_DESCRIPTOR                 0xf8

/* USER CODE END PRIVATE_DEFINES */

/**
  * @}
  */

/** @defgroup USBD_STORAGE_Private_Macros
  * @brief Private macros.
  * @{
  */

/* USER CODE BEGIN PRIVATE_MACRO */
uint8_t RamDisk[STORAGE_BLK_NBR * STORAGE_BLK_SIZ];

void STORAGE_FileSystem_Init (void);

const struct __attribute__ ((packed)) BPB_STRUCTURE
		{
	uint8_t   BS_jmpBoot[3] ;
	int8_t    BS_OEMName[8] ;
	uint16_t  BPB_BytsPerSec;
	uint8_t   BPB_SecPerClus;
	uint16_t  BPB_RsvdSecCnt;
	uint8_t   BPB_NumFATs;
	uint16_t  BPB_RootEntCnt;
	uint16_t  BPB_TotSec16;
	uint8_t   BPB_Media;
	uint16_t  BPB_FATSz16;
	uint16_t  BPB_SecPerTrk;
	uint16_t  BPB_NumHeads;
	uint32_t  BPB_HiddSec;
	uint32_t  BPB_TotSec32;
	uint8_t   BS_DrvNum;
	uint8_t   BS_Reserved1;
	uint8_t   BS_BootSig;
	uint32_t  BS_VolID;
	int8_t    BS_VolLab[11];
	int8_t    BS_FilSysType[8];
		} BIOS_Parameter_Block  = { \
				.BS_jmpBoot = {0xeb,0xfe,0x90}, \
				.BS_OEMName = {'M', 'S', 'D', 'O', 'S', '5', '.', '0'}, \
				.BPB_BytsPerSec = STORAGE_BLK_SIZ, \
				.BPB_SecPerClus = 1, \
				.BPB_RsvdSecCnt = 1, \
				.BPB_NumFATs = 1, \
				.BPB_RootEntCnt = STORAGE_BLK_SIZ / 0x20, \
				.BPB_TotSec16 = STORAGE_BLK_NBR, \
				.BPB_Media = MEDIA_DESCRIPTOR, \
				.BPB_FATSz16 = 1, \
				.BPB_SecPerTrk = 0x3f, \
				.BPB_NumHeads = 0xff, \
				.BPB_HiddSec = 0, \
				.BPB_TotSec32 = 0, \
				.BS_DrvNum = 0x80, \
				.BS_Reserved1 = 0, \
				.BS_BootSig = 0x29, \
				.BS_VolID = 0xcafecafe, \
				.BS_VolLab = {'S', 'T', 'M', '_', 'R', 'A', 'M', 'D', 'I', 'S', 'K'}, \
				.BS_FilSysType = {'F', 'A', 'T', '1', '2', ' ', ' ', ' '}, \
		};

const uint8_t Text_File_Data[] = {"STM32 CubeMX Mass Storage Device Ram Disk - done by wegi\r\n"};

const uint8_t FAT_TABLE[] = {MEDIA_DESCRIPTOR, 0xff,0xff,0xff,0x0f};
const uint8_t File_Name[] = {'T', 'E', 'X', 'T', 'F', 'I', 'L', 'E', 'T', 'X', 'T', \
		0x20, \
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
		0x02, 0x00, \
		sizeof(Text_File_Data), \
};

/* USER CODE END PRIVATE_MACRO */

/**
  * @}
  */

/** @defgroup USBD_STORAGE_Private_Variables
  * @brief Private variables.
  * @{
  */

/* USER CODE BEGIN INQUIRY_DATA_FS */
/** USB Mass storage Standard Inquiry Data. */
const int8_t STORAGE_Inquirydata_FS[] = {/* 36 */
  
  /* LUN 0 */
  0x00,
  0x80,
  0x02,
  0x02,
  (STANDARD_INQUIRY_DATA_LEN - 5),
  0x00,
  0x00,	
  0x00,
  'S', 'T', 'M', ' ', ' ', ' ', ' ', ' ', /* Manufacturer : 8 bytes */
  'P', 'r', 'o', 'd', 'u', 'c', 't', ' ', /* Product      : 16 Bytes */
  ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
  '0', '.', '0' ,'1'                      /* Version      : 4 Bytes */
}; 
/* USER CODE END INQUIRY_DATA_FS */

/* USER CODE BEGIN PRIVATE_VARIABLES */

/* USER CODE END PRIVATE_VARIABLES */

/**
  * @}
  */

/** @defgroup USBD_STORAGE_Exported_Variables
  * @brief Public variables.
  * @{
  */

extern USBD_HandleTypeDef hUsbDeviceFS;

/* USER CODE BEGIN EXPORTED_VARIABLES */

/* USER CODE END EXPORTED_VARIABLES */

/**
  * @}
  */

/** @defgroup USBD_STORAGE_Private_FunctionPrototypes
  * @brief Private functions declaration.
  * @{
  */

static int8_t STORAGE_Init_FS(uint8_t lun);
static int8_t STORAGE_GetCapacity_FS(uint8_t lun, uint32_t *block_num, uint16_t *block_size);
static int8_t STORAGE_IsReady_FS(uint8_t lun);
static int8_t STORAGE_IsWriteProtected_FS(uint8_t lun);
static int8_t STORAGE_Read_FS(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len);
static int8_t STORAGE_Write_FS(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len);
static int8_t STORAGE_GetMaxLun_FS(void);

/* USER CODE BEGIN PRIVATE_FUNCTIONS_DECLARATION */

/* USER CODE END PRIVATE_FUNCTIONS_DECLARATION */

/**
  * @}
  */

USBD_StorageTypeDef USBD_Storage_Interface_fops_FS =
{
  STORAGE_Init_FS,
  STORAGE_GetCapacity_FS,
  STORAGE_IsReady_FS,
  STORAGE_IsWriteProtected_FS,
  STORAGE_Read_FS,
  STORAGE_Write_FS,
  STORAGE_GetMaxLun_FS,
  (int8_t *)STORAGE_Inquirydata_FS
};

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Initializes over USB FS IP
  * @param  lun:
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
int8_t STORAGE_Init_FS(uint8_t lun)
{
  /* USER CODE BEGIN 2 */
  return (USBD_OK);
  /* USER CODE END 2 */
}

/**
  * @brief  .
  * @param  lun: .
  * @param  block_num: .
  * @param  block_size: .
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
int8_t STORAGE_GetCapacity_FS(uint8_t lun, uint32_t *block_num, uint16_t *block_size)
{
  /* USER CODE BEGIN 3 */
  *block_num  = STORAGE_BLK_NBR;
  *block_size = STORAGE_BLK_SIZ;
  return (USBD_OK);
  /* USER CODE END 3 */
}

/**
  * @brief  .
  * @param  lun: .
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
int8_t STORAGE_IsReady_FS(uint8_t lun)
{
  /* USER CODE BEGIN 4 */
  return (USBD_OK);
  /* USER CODE END 4 */
}

/**
  * @brief  .
  * @param  lun: .
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
int8_t STORAGE_IsWriteProtected_FS(uint8_t lun)
{
  /* USER CODE BEGIN 5 */
  return (USBD_OK);
  /* USER CODE END 5 */
}

/**
  * @brief  .
  * @param  lun: .
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
int8_t STORAGE_Read_FS(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len)
{
	/* USER CODE BEGIN 6 */
	USBD_memcpy(buf, (void *)(RamDisk + (STORAGE_BLK_SIZ * blk_addr)), (STORAGE_BLK_SIZ * blk_len));
	HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_7);
	return (USBD_OK);
	/* USER CODE END 6 */
}

/**
  * @brief  .
  * @param  lun: .
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
int8_t STORAGE_Write_FS(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len)
{
	/* USER CODE BEGIN 7 */
	memcpy((void *)(RamDisk + (STORAGE_BLK_SIZ * blk_addr)), buf, (STORAGE_BLK_SIZ * blk_len));
	HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_6);
	return (USBD_OK);
	/* USER CODE END 7 */
}

/**
  * @brief  .
  * @param  None
  * @retval .
  */
int8_t STORAGE_GetMaxLun_FS(void)
{
  /* USER CODE BEGIN 8 */
  return (STORAGE_LUN_NBR - 1);
  /* USER CODE END 8 */
}

/* USER CODE BEGIN PRIVATE_FUNCTIONS_IMPLEMENTATION */
void STORAGE_FileSystem_Init (void)
{

    /*----------- FORMAT BOOT SECTOR -----------------*/
	memset(RamDisk, 0, sizeof RamDisk);
	memcpy((void *)(RamDisk), &BIOS_Parameter_Block , sizeof(BIOS_Parameter_Block));
	RamDisk[0x1fe] = 0x55;
	RamDisk[0x1ff] = 0xAA;
//---
    /*----------- FORMAT FAT TABLE -----------------*/
	memcpy((void *)(RamDisk + (STORAGE_BLK_SIZ * 1) ), &FAT_TABLE , (sizeof(FAT_TABLE)));
//---
    /*----------- FORMAT DIR ENTRIES AND DISK NAME  -----------------*/
    memcpy((void *)(RamDisk + (STORAGE_BLK_SIZ * 2) ), &BIOS_Parameter_Block.BS_VolLab , (sizeof(BIOS_Parameter_Block.BS_VolLab)));
    RamDisk[(STORAGE_BLK_SIZ * 2) + 11] = 0x08; //disk name attribute
    memcpy((void *)(RamDisk + 32 + (STORAGE_BLK_SIZ * 2) ), &File_Name , (sizeof(File_Name)));
//---
    /*----------- COPY ONE OF TEXTFILE  -----------------*/
    memcpy((void *)(RamDisk + (STORAGE_BLK_SIZ * 3) ), &Text_File_Data , (sizeof(Text_File_Data)));

//---
}
/* USER CODE END PRIVATE_FUNCTIONS_IMPLEMENTATION */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
