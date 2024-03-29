/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : usb_prop.c
* Author             : MCD Application Team
* Version            : V2.2.1
* Date               : 09/22/2008
* Description        : All processing related to Virtual Com Port Demo
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "usb_lib.h"
#include "usb_conf.h"
#include "usb_prop.h"
#include "usb_desc.h"
#include "usb_pwr.h"
#include "hw_config.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
typedef struct  _VideoControl
{
	u8    bmHint[2];                      // 0x02
	u8    bFormatIndex[1];                // 0x03
	u8    bFrameIndex[1];                 // 0x04
	u8    dwFrameInterval[4];             // 0x08
	u8    wKeyFrameRate[2];               // 0x0A
	u8    wPFrameRate[2];                 // 0x0C
	u8    wCompQuality[2];                // 0x0E
	u8    wCompWindowSize[2];             // 0x10
	u8    wDelay[2];                      // 0x12
	u8    dwMaxVideoFrameSize[4];         // 0x16
	u8    dwMaxPayloadTransferSize[4];    // 0x1A
	u8    dwClockFrequency[4];            // 0x1C
	u8    bmFramingInfo[1];
	u8    bPreferedVersion[1];
	u8    bMinVersion[1];
	u8    bMaxVersion[1];
}   VideoControl;

//VideoStreaming Requests应答，参考USB_Video_Class_1_1.pdf p103~
VideoControl    videoCommitControl =
{
	{ 0x01, 0x00 },                      // bmHint
	{ 0x01 },                           // bFormatIndex
	{ 0x01 },                           // bFrameIndex
	{ MAKE_DWORD(FRAME_INTERVEL) },		// dwFrameInterval
	{ 0x00, 0x00, },                     // wKeyFrameRate
	{ 0x00, 0x00, },                     // wPFrameRate
	{ 0x00, 0x00, },                     // wCompQuality
	{ 0x00, 0x00, },                     // wCompWindowSize
	{ 0x00, 0x00 },                      // wDelay
	{ MAKE_DWORD(MAX_FRAME_SIZE) },     // dwMaxVideoFrameSize
	{ 0x00, 0x00, 0x00, 0x00 },         // dwMaxPayloadTransferSize
	{ 0x00, 0x00, 0x00, 0x00 },         // dwClockFrequency
	{ 0x00 },                           // bmFramingInfo
	{ 0x00 },                           // bPreferedVersion
	{ 0x00 },                           // bMinVersion
	{ 0x00 },                           // bMaxVersion
};

VideoControl    videoProbeControl =
{
	{ 0x01, 0x00 },                      // bmHint
	{ 0x01 },                           // bFormatIndex
	{ 0x01 },                           // bFrameIndex
	{ MAKE_DWORD(FRAME_INTERVEL) },          // dwFrameInterval
	{ 0x00, 0x00, },                     // wKeyFrameRate
	{ 0x00, 0x00, },                     // wPFrameRate
	{ 0x00, 0x00, },                     // wCompQuality
	{ 0x00, 0x00, },                     // wCompWindowSize
	{ 0x00, 0x00 },                      // wDelay
	{ MAKE_DWORD(MAX_FRAME_SIZE) },    // dwMaxVideoFrameSize
	{ 0x00, 0x00, 0x00, 0x00 },         // dwMaxPayloadTransferSize
	{ 0x00, 0x00, 0x00, 0x00 },         // dwClockFrequency
	{ 0x00 },                           // bmFramingInfo
	{ 0x00 },                           // bPreferedVersion
	{ 0x00 },                           // bMinVersion
	{ 0x00 },                           // bMaxVersion
};

/* -------------------------------------------------------------------------- */
/*  Structures initializations */
/* -------------------------------------------------------------------------- */

DEVICE Device_Table =
{
	EP_NUM,
	1
};

DEVICE_PROP Device_Property =
{
	UsbCamera_init,
	UsbCamera_Reset,
	UsbCamera_Status_In,
	UsbCamera_Status_Out,
	UsbCamera_Data_Setup,
	UsbCamera_NoData_Setup,
	UsbCamera_Get_Interface_Setting,
	UsbCamera_GetDeviceDescriptor,
	UsbCamera_GetConfigDescriptor,
	UsbCamera_GetStringDescriptor,
	0,
	0x40 /*MAX PACKET SIZE*/
};

USER_STANDARD_REQUESTS User_Standard_Requests =
{
	UsbCamera_GetConfiguration,
	UsbCamera_SetConfiguration,
	UsbCamera_GetInterface,
	UsbCamera_SetInterface,
	UsbCamera_GetStatus,
	UsbCamera_ClearFeature,
	UsbCamera_SetEndPointFeature,
	UsbCamera_SetDeviceFeature,
	UsbCamera_SetDeviceAddress
};

ONE_DESCRIPTOR Device_Descriptor =
{
	(u8*)Camera_DeviceDescriptor,
	CAMERA_SIZ_DEVICE_DESC
};
ONE_DESCRIPTOR Config_Descriptor =
{
	(u8*)Camera_ConfigDescriptor,
	CAMERA_SIZ_CONFIG_DESC
};
ONE_DESCRIPTOR String_Descriptor[4] =
{
	{ (u8*)Camera_StringLangID, CAMERA_SIZ_STRING_LANGID },
	{ (u8*)Camera_StringVendor, CAMERA_SIZ_STRING_VENDOR },
	{ (u8*)Camera_StringProduct, CAMERA_SIZ_STRING_PRODUCT },
	{ (u8*)Camera_StringSerial, CAMERA_SIZ_STRING_SERIAL }
};

/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Extern function prototypes ------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  :
* Description    : init routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void UsbCamera_init(void)
{

	/* Update the serial number string descriptor with the data from the unique
	ID*/
	//Get_SerialNum();

	pInformation->Current_Configuration = 0;

	/* Connect the device */
	PowerOn();
	/* USB interrupts initialization */
	/* clear pending interrupts */
	_SetISTR(0);
	wInterrupt_Mask = IMR_MSK;
	/* set interrupts mask */
	_SetCNTR(wInterrupt_Mask);

	bDeviceState = UNCONNECTED;
}

/*******************************************************************************
* Function Name  :
* Description    : reset routine
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void UsbCamera_Reset(void)
{
	/* Set Usb device as not configured state */
	pInformation->Current_Configuration = 0;

	/* Current Feature initialization */
	pInformation->Current_Feature = Camera_ConfigDescriptor[7];
	SetBTABLE(BTABLE_ADDRESS);

	/* Initialize Endpoint 0 */
	SetEPType(ENDP0, EP_CONTROL);
	SetEPTxStatus(ENDP0, EP_TX_NAK);
	SetEPRxAddr(ENDP0, ENDP0_RXADDR);
	SetEPRxCount(ENDP0, Device_Property.MaxPacketSize);
	SetEPTxAddr(ENDP0, ENDP0_TXADDR);
	Clear_Status_Out(ENDP0);
	SetEPRxValid(ENDP0);

	/* Initialize Endpoint 1 */
	SetEPType(ENDP1, EP_ISOCHRONOUS);
	SetEPDoubleBuff(ENDP1);
	SetEPDblBuffAddr(ENDP1, ENDP1_BUF0Addr, ENDP1_BUF1Addr);
	SetEPDblBuffCount(ENDP1, EP_DBUF_IN, PACKET_SIZE);
	ClearDTOG_RX(ENDP1);
	ClearDTOG_TX(ENDP1);
	SetEPDblBuf0Count(ENDP1, EP_DBUF_IN, 0);
	SetEPDblBuf1Count(ENDP1, EP_DBUF_IN, 0);
	SetEPRxStatus(ENDP1, EP_RX_DIS);
	//    SetEPTxStatus(ENDP1, EP_TX_VALID);
	SetEPTxStatus(ENDP1, EP_TX_NAK);

	SetEPRxValid(ENDP0);
	/* Set this device to response on default address */
	SetDeviceAddress(0);

	bDeviceState = ATTACHED;
}


/*******************************************************************************
* Function Name  :
* Description    : Udpade the device state to configured.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void UsbCamera_SetConfiguration(void)
{
	DEVICE_INFO *pInfo = &Device_Info;

	if (pInfo->Current_Configuration != 0)
	{
		/* Device configured */
		bDeviceState = CONFIGURED;
	}
}

/*******************************************************************************
* Function Name  :
* Description    : Udpade the device state to addressed.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void UsbCamera_SetDeviceAddress(void)
{
	bDeviceState = ADDRESSED;
}

/*******************************************************************************
* Function Name  :
* Description    : Status In Routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void UsbCamera_Status_In(void)
{}

/*******************************************************************************
* Function Name  :
* Description    : Status OUT Routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void UsbCamera_Status_Out(void)
{}

/*******************************************************************************
* Function Name  :
* Description    : handle the data class specific requests
*              对Class-specific Requests的应答
* Input          : Request Nb.
* Output         : None.
* Return         : USB_UNSUPPORT or USB_SUCCESS.
*******************************************************************************/
RESULT UsbCamera_Data_Setup(u8 RequestNo)
{
	u8 *(*CopyRoutine)(u16);
	CopyRoutine = NULL;


	if ((RequestNo == GET_CUR) || (RequestNo == SET_CUR))
	{
		if (pInformation->USBwIndex == 0x0100)
		{
			if (pInformation->USBwValue == 0x0001)
			{
				// Probe Control
				CopyRoutine = VideoProbeControl_Command;
			} else if (pInformation->USBwValue == 0x0002)
			{
				// Commit control
				CopyRoutine = VideoCommitControl_Command;
			}
		}
	} else
	{
		return USB_UNSUPPORT;
	}

	pInformation->Ctrl_Info.CopyData = CopyRoutine;
	pInformation->Ctrl_Info.Usb_wOffset = 0;
	(*CopyRoutine)(0);
	return USB_SUCCESS;
}



/*******************************************************************************
* Function Name  :
* Description    : handle the no data class specific requests.
* Input          : Request Nb.
* Output         : None.
* Return         : USB_UNSUPPORT or USB_SUCCESS.
*******************************************************************************/
RESULT UsbCamera_NoData_Setup(u8 RequestNo)
{
	return USB_UNSUPPORT;
}

/*******************************************************************************
* Function Name  :
* Description    : Gets the device descriptor.
* Input          : Length.
* Output         : None.
* Return         : The address of the device descriptor.
*******************************************************************************/
u8 *UsbCamera_GetDeviceDescriptor(u16 Length)
{
	return Standard_GetDescriptorData(Length, &Device_Descriptor);
}

/*******************************************************************************
* Function Name  :
* Description    : get the configuration descriptor.
* Input          : Length.
* Output         : None.
* Return         : The address of the configuration descriptor.
*******************************************************************************/
u8 *UsbCamera_GetConfigDescriptor(u16 Length)
{
	return Standard_GetDescriptorData(Length, &Config_Descriptor);
}

/*******************************************************************************
* Function Name  :
* Description    : Gets the string descriptors according to the needed index
* Input          : Length.
* Output         : None.
* Return         : The address of the string descriptors.
*******************************************************************************/
u8 *UsbCamera_GetStringDescriptor(u16 Length)
{
	u8 wValue0 = pInformation->USBwValue0;
	if (wValue0 > 4)
	{
		return NULL;
	} else
	{
		return Standard_GetDescriptorData(Length, &String_Descriptor[wValue0]);
	}
}

/*******************************************************************************
* Function Name  :
* Description    : test the interface and the alternate setting according to the
*                  supported one.
* Input1         : u8: Interface : interface number.
* Input2         : u8: AlternateSetting : Alternate Setting number.
* Output         : None.
* Return         : The address of the string descriptors.
*******************************************************************************/
RESULT UsbCamera_Get_Interface_Setting(u8 Interface, u8 AlternateSetting)
{
	if (AlternateSetting > 1)
	{
		return USB_UNSUPPORT;
	} else if (Interface > 1)
	{
		return USB_UNSUPPORT;
	}
	return USB_SUCCESS;
}


/*******************************************************************************
* Function Name  :
* Description    :
* Input          :
* Output         :
* Return         :
*******************************************************************************/
u8* VideoProbeControl_Command(u16 Length)
{
	if (Length == 0)
	{
		pInformation->Ctrl_Info.Usb_wLength = pInformation->USBwLengths.w;
		return NULL;
	} else
	{
		return((u8*)(&videoProbeControl));
	}
}

/*******************************************************************************
* Function Name  :
* Description    :
* Input          :
* Output         :
* Return         :
*******************************************************************************/
u8* VideoCommitControl_Command(u16 Length)
{
	if (Length == 0)
	{
		pInformation->Ctrl_Info.Usb_wLength = pInformation->USBwLengths.w;
		return NULL;
	} else
	{
		return((u8*)(&videoCommitControl));
	}
}




/******************* (C) COPYRIGHT 2011 xxxxxxxxxxxxxxx *****END OF FILE****/

