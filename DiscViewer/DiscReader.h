#pragma once

#include <map>

#include "StructLibrary.h"

//SMART attribute constants
#define SMART_ATTRIB_RAW_READ_ERROR_RATE                    1
#define SMART_ATTRIB_THROUGHPUT_PERFORMANCE                 2
#define SMART_ATTRIB_SPIN_UP_TIME                           3
#define SMART_ATTRIB_START_STOP_COUNT                       4
#define SMART_ATTRIB_START_REALLOCATION_SECTOR_COUNT        5
#define SMART_ATTRIB_SEEK_ERROR_RATE                        7
#define SMART_ATTRIB_POWER_ON_HOURS_COUNT                   9
#define SMART_ATTRIB_SPIN_RETRY_COUNT                       10
#define SMART_ATTRIB_RECALIBRATION_RETRIES                  11
#define SMART_ATTRIB_DEVICE_POWER_CYCLE_COUNT               12
#define SMART_ATTRIB_SOFT_READ_ERROR_RATE                   13
#define SMART_ATTRIB_LOAD_UNLOAD_CYCLE_COUNT                193
#define SMART_ATTRIB_TEMPERATURE                            194
#define SMART_ATTRIB_ECC_ON_THE_FLY_COUNT                   195
#define SMART_ATTRIB_REALLOCATION_EVENT_COUNT               196
#define SMART_ATTRIB_CURRENT_PENDING_SECTOR_COUNT           197
#define SMART_ATTRIB_UNCORRECTABLE_SECTOR_COUNT             198
#define SMART_ATTRIB_ULTRA_DMA_CRC_ERROR_COUNT              199
#define SMART_ATTRIB_WRITE_ERROR_RATE                       200
#define SMART_ATTRIB_TA_COUNTER_INCREASED                   202
#define SMART_ATTRIB_GSENSE_ERROR_RATE                      221
#define SMART_ATTRIB_POWER_OFF_RETRACT_COUNT                228
#define MAX_ATTRIBUTES  256

#define INDEX_ATTRIB_INDEX                                  0
#define INDEX_ATTRIB_UNKNOWN1                               1
#define INDEX_ATTRIB_UNKNOWN2                               2
#define INDEX_ATTRIB_VALUE                                  3
#define INDEX_ATTRIB_WORST                                  4
#define INDEX_ATTRIB_RAW                                    5

#define DRIVE_HEAD_REG  0xA0

#define CTL_CODE(DeviceType, Function, Method, Access) \
    (((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method))
#define IOCTL_VOLUME_BASE ((DWORD)'V')
#define METHOD_BUFFERED 0
#define FILE_ANY_ACCESS 0x00000000
#define IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS CTL_CODE(IOCTL_VOLUME_BASE, 0, METHOD_BUFFERED, FILE_ANY_ACCESS)


#define _WIN32_WINNT 0x500
//Windows DDK header files
#include "devioctl.h"
#include "ntdddisk.h"
#pragma warning(disable:4786)

typedef std::map<DWORD, LPVOID> SMARTINFOMAP;



class DiscReader
{
public:
	DiscReader();
	~DiscReader();

public:
	//вывод дисков
	void ShowDiscsInfo();

	// вывод SMART регистров выбранного диска
	void ShowDiscInfo(BYTE ucDriveIndex);

	// возвращает количество физических дисков
	inline int GetDiscCount() const { return CurrentDiscCount; }

private:
	//преобразование идентификатора SMART-показателя в строку
	const char* SmartIndexToString(BYTE index);

	//вывод сообщения об ошибке
	void ErrorMes(LPTSTR lpszFunction);

	//получение значения показателя из m_oSmartInfo
	ST_SMART_INFO* GetSMARTValue(BYTE ucDriveIndex, BYTE ucAttribIndex);

	BOOL ReadSMARTAttributes(HANDLE hDevice, UCHAR ucDriveIndex);
	BOOL CollectDriveInfo(HANDLE hDevice, UCHAR ucDriveIndex);
	BOOL IsSmartEnabled(HANDLE hDevice, UCHAR ucDriveIndex);

	//Считывает SMART-показатели для диска с указанным индексом
	BOOL ReadSMARTInfo(BYTE ucDriveIndex);

private:
	SMARTINFOMAP m_oSmartInfo; //map для хранения SMART-показателей
	ST_DRIVE_INFO m_stDrivesInfo[32]; //массив с данными о жестких дисках
	int CurrentDiscCount;
	
};

