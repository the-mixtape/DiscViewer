#include "DiscReader.h"

#include <iostream>
#include <string>
#include <bitset>
#include <vector>
#include <Windows.h>
#include <cstdio>
#include <tchar.h>
#include <windows.h>
#include <clocale>
#include <map>


using namespace std;


DiscReader::DiscReader()
{
	
}

DiscReader::~DiscReader()
{
}

void DiscReader::ShowDiscsInfo()
{
    DISK_EXTENT* PDISK_EXTENT;
    VOLUME_DISK_EXTENTS* PVOLUME_DISK_EXTENTS;

    bitset<32> drives(GetLogicalDrives());
    vector<char> goodDrives;
    for (char c = 'A'; c <= 'Z'; ++c) {
        if (drives[c - 'A']) {
            if (GetDriveTypeA((c + string(":\\")).c_str()) == DRIVE_FIXED) {
                goodDrives.push_back(c);
            }
        }
    }
    for (auto& drive : goodDrives) {
	    string s = string("\\\\.\\") + drive + ":";
        HANDLE h = CreateFileA(
            s.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
            OPEN_EXISTING, FILE_FLAG_NO_BUFFERING | FILE_FLAG_RANDOM_ACCESS, NULL
        );
        if (h == INVALID_HANDLE_VALUE) {

            HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
			SetConsoleTextAttribute(handle, FOREGROUND_RED);
        	
            cerr << "Drive " << drive << ":\\ cannot be opened";

        	SetConsoleTextAttribute(handle, FOREGROUND_INTENSITY);
            continue;
        }
        DWORD bytesReturned;
        VOLUME_DISK_EXTENTS vde;
        if (!DeviceIoControl(
            h, IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS,
            NULL, 0, &vde, sizeof(vde), &bytesReturned, NULL
        )) {
            cerr << "Drive " << drive << ":\\ cannot be mapped into physical drive";
            continue;
        }
    	
        cout << "Drive ";
    	
        HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(handle, 14);

        cout << drive << ":\\";

        SetConsoleTextAttribute(handle, 15);
    	
    	cout << " is on the following physical drives: ";
    	
        CurrentDiscCount = vde.NumberOfDiskExtents;
    	
    	for (int i = 0; i < CurrentDiscCount; ++i) {
            SetConsoleTextAttribute(handle, 9);
            cout << "disc " << vde.Extents[i].DiskNumber << ' ';
            SetConsoleTextAttribute(handle, 15);
        }

        cout << endl;
    }

}

void DiscReader::ShowDiscInfo(BYTE ucDriveIndex)
{
    BOOL res = ReadSMARTInfo(ucDriveIndex);
    ST_SMART_INFO* pSmart;

    for (int i = 0; i < MAX_ATTRIBUTES; i++)
    {
        pSmart = GetSMARTValue(ucDriveIndex, i);
        if (pSmart == NULL)continue;
        printf("0x%02x %s: %u\n", pSmart->m_ucAttribIndex, SmartIndexToString(pSmart->m_ucAttribIndex), (UINT)pSmart->m_ucValue);

    }
}

const char* DiscReader::SmartIndexToString(BYTE index)
{
    switch (index)
    {
    case SMART_ATTRIB_RAW_READ_ERROR_RATE: return "RAW_READ_ERROR_RATE";
    case  SMART_ATTRIB_THROUGHPUT_PERFORMANCE: return "THROUGHPUT_PERFORMANCE";
    case  SMART_ATTRIB_SPIN_UP_TIME: return "SPIN_UP_TIME";
    case  SMART_ATTRIB_START_STOP_COUNT: return "START_STOP_COUNT";
    case  SMART_ATTRIB_START_REALLOCATION_SECTOR_COUNT: return "START_REALLOCATION_SECTOR_COUNT";
    case  SMART_ATTRIB_SEEK_ERROR_RATE: return "SEEK_ERROR_RATE";
    case  SMART_ATTRIB_POWER_ON_HOURS_COUNT: return "POWER_ON_HOURS_COUNT";
    case  SMART_ATTRIB_SPIN_RETRY_COUNT: return "SPIN_RETRY_COUNT";
    case  SMART_ATTRIB_RECALIBRATION_RETRIES: return "RECALIBRATION_RETRIES";
    case  SMART_ATTRIB_DEVICE_POWER_CYCLE_COUNT: return "DEVICE_POWER_CYCLE_COUNT";
    case  SMART_ATTRIB_SOFT_READ_ERROR_RATE: return "SOFT_READ_ERROR_RATE";
    case  SMART_ATTRIB_LOAD_UNLOAD_CYCLE_COUNT: return "LOAD_UNLOAD_CYCLE_COUNT";
    case  SMART_ATTRIB_TEMPERATURE: return "TEMPERATURE";
    case  SMART_ATTRIB_ECC_ON_THE_FLY_COUNT: return "ECC_ON_THE_FLY_COUNT";
    case  SMART_ATTRIB_REALLOCATION_EVENT_COUNT: return "REALLOCATION_EVENT_COUNT";
    case  SMART_ATTRIB_CURRENT_PENDING_SECTOR_COUNT: return "CURRENT_PENDING_SECTOR_COUNT";
    case  SMART_ATTRIB_UNCORRECTABLE_SECTOR_COUNT: return "UNCORRECTABLE_SECTOR_COUNT";
    case  SMART_ATTRIB_ULTRA_DMA_CRC_ERROR_COUNT: return "ULTRA_DMA_CRC_ERROR_COUNT";
    case  SMART_ATTRIB_WRITE_ERROR_RATE: return "WRITE_ERROR_RATE  ";
    case  SMART_ATTRIB_TA_COUNTER_INCREASED: return "TA_COUNTER_INCREASED";
    case  SMART_ATTRIB_GSENSE_ERROR_RATE: return "GSENSE_ERROR_RATE";
    case  SMART_ATTRIB_POWER_OFF_RETRACT_COUNT: return "POWER_OFF_RETRACT_COUNT   ";

    default:
        return "";
    }
}

void DiscReader::ErrorMes(LPTSTR lpszFunction)
{
    // Retrieve the system error message for the last-error code

    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError();

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf,
        0, NULL);

    // Display the error message and exit the process

    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
        (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
    printf("%s failed with error %d: %s",
        lpszFunction, dw, lpMsgBuf);

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);

}

ST_SMART_INFO* DiscReader::GetSMARTValue(BYTE ucDriveIndex, BYTE ucAttribIndex)
{
    SMARTINFOMAP::iterator pIt;
    ST_SMART_INFO* pRet = NULL;

    pIt = m_oSmartInfo.find(MAKELPARAM(ucAttribIndex, ucDriveIndex));
    if (pIt != m_oSmartInfo.end())
        pRet = (ST_SMART_INFO*)pIt->second;
    return pRet;
}

BOOL DiscReader::ReadSMARTAttributes(HANDLE hDevice, UCHAR ucDriveIndex)
{
    SENDCMDINPARAMS stCIP = { 0 };
    DWORD dwRet = 0;
    BOOL bRet = FALSE;
    BYTE    szAttributes[sizeof(ST_ATAOUTPARAM) + READ_ATTRIBUTE_BUFFER_SIZE - 1];
    UCHAR ucT1;
    PBYTE pT1, pT3; PDWORD pT2;
    ST_SMART_INFO* pSmartValues;

    stCIP.cBufferSize = READ_ATTRIBUTE_BUFFER_SIZE;
    stCIP.bDriveNumber = ucDriveIndex;
    stCIP.irDriveRegs.bFeaturesReg = READ_ATTRIBUTES;
    stCIP.irDriveRegs.bSectorCountReg = 1;
    stCIP.irDriveRegs.bSectorNumberReg = 1;
    stCIP.irDriveRegs.bCylLowReg = SMART_CYL_LOW;
    stCIP.irDriveRegs.bCylHighReg = SMART_CYL_HI;
    stCIP.irDriveRegs.bDriveHeadReg = DRIVE_HEAD_REG;
    stCIP.irDriveRegs.bCommandReg = SMART_CMD;
    bRet = DeviceIoControl(hDevice, SMART_RCV_DRIVE_DATA, &stCIP, sizeof(stCIP), szAttributes, sizeof(ST_ATAOUTPARAM) + READ_ATTRIBUTE_BUFFER_SIZE - 1, &dwRet, NULL);
    if (bRet)
    {
        m_stDrivesInfo[ucDriveIndex].m_ucSmartValues = 0;
        m_stDrivesInfo[ucDriveIndex].m_ucDriveIndex = ucDriveIndex;
        pT1 = (PBYTE)(((ST_ATAOUTPARAM*)szAttributes)->bBuffer);
        for (ucT1 = 0; ucT1 < 30; ++ucT1)
        {
            pT3 = &pT1[2 + ucT1 * 12];
            pT2 = (PDWORD)&pT3[INDEX_ATTRIB_RAW];
            pT3[INDEX_ATTRIB_RAW + 2] = pT3[INDEX_ATTRIB_RAW + 3] = pT3[INDEX_ATTRIB_RAW + 4] = pT3[INDEX_ATTRIB_RAW + 5] = pT3[INDEX_ATTRIB_RAW + 6] = 0;
            if (pT3[INDEX_ATTRIB_INDEX] != 0)
            {
                pSmartValues = &m_stDrivesInfo[ucDriveIndex].m_stSmartInfo[m_stDrivesInfo[ucDriveIndex].m_ucSmartValues];
                pSmartValues->m_ucAttribIndex = pT3[INDEX_ATTRIB_INDEX];
                pSmartValues->m_ucValue = pT3[INDEX_ATTRIB_VALUE];
                pSmartValues->m_ucWorst = pT3[INDEX_ATTRIB_WORST];
                pSmartValues->m_dwAttribValue = pT2[0];
                pSmartValues->m_dwThreshold = MAXDWORD;
                m_oSmartInfo[MAKELPARAM(pSmartValues->m_ucAttribIndex, ucDriveIndex)] = pSmartValues;
                m_stDrivesInfo[ucDriveIndex].m_ucSmartValues++;
            }
        }
    }
    else
        dwRet = GetLastError();

    stCIP.irDriveRegs.bFeaturesReg = READ_THRESHOLDS;
    stCIP.cBufferSize = READ_THRESHOLD_BUFFER_SIZE; // Is same as attrib size
    bRet = DeviceIoControl(hDevice, SMART_RCV_DRIVE_DATA, &stCIP, sizeof(stCIP), szAttributes, sizeof(ST_ATAOUTPARAM) + READ_ATTRIBUTE_BUFFER_SIZE - 1, &dwRet, NULL);
    if (bRet)
    {
        pT1 = (PBYTE)(((ST_ATAOUTPARAM*)szAttributes)->bBuffer);
        for (ucT1 = 0; ucT1 < 30; ++ucT1)
        {
            pT2 = (PDWORD)&pT1[2 + ucT1 * 12 + 5];
            pT3 = &pT1[2 + ucT1 * 12];
            pT3[INDEX_ATTRIB_RAW + 2] = pT3[INDEX_ATTRIB_RAW + 3] = pT3[INDEX_ATTRIB_RAW + 4] = pT3[INDEX_ATTRIB_RAW + 5] = pT3[INDEX_ATTRIB_RAW + 6] = 0;
            if (pT3[0] != 0)
            {
                pSmartValues = GetSMARTValue(ucDriveIndex, pT3[0]);
                if (pSmartValues)
                    pSmartValues->m_dwThreshold = pT2[0];
            }
        }
    }
    return bRet;    
}

BOOL DiscReader::CollectDriveInfo(HANDLE hDevice, UCHAR ucDriveIndex)
{
    BOOL bRet = FALSE;
    SENDCMDINPARAMS stCIP = { 0 };
    DWORD dwRet = 0;
#define OUT_BUFFER_SIZE IDENTIFY_BUFFER_SIZE+16
    char szOutput[OUT_BUFFER_SIZE] = { 0 };

    stCIP.cBufferSize = IDENTIFY_BUFFER_SIZE;
    stCIP.bDriveNumber = ucDriveIndex;
    stCIP.irDriveRegs.bFeaturesReg = 0;
    stCIP.irDriveRegs.bSectorCountReg = 1;
    stCIP.irDriveRegs.bSectorNumberReg = 1;
    stCIP.irDriveRegs.bCylLowReg = 0;
    stCIP.irDriveRegs.bCylHighReg = 0;
    stCIP.irDriveRegs.bDriveHeadReg = DRIVE_HEAD_REG;
    stCIP.irDriveRegs.bCommandReg = ID_CMD;

    bRet = DeviceIoControl(hDevice, SMART_RCV_DRIVE_DATA, &stCIP, sizeof(stCIP), szOutput, OUT_BUFFER_SIZE, &dwRet, NULL);
    if (bRet)
    {
        CopyMemory(&m_stDrivesInfo[ucDriveIndex].m_stInfo, szOutput + 16, sizeof(ST_IDSECTOR));
    }
    else
        dwRet = GetLastError();
    return bRet;
}

BOOL DiscReader::IsSmartEnabled(HANDLE hDevice, UCHAR ucDriveIndex)
{
    SENDCMDINPARAMS stCIP = { 0 };
    SENDCMDOUTPARAMS stCOP = { 0 };
    DWORD dwRet = 0;
    BOOL bRet = FALSE;

    stCIP.cBufferSize = 0;
    stCIP.bDriveNumber = ucDriveIndex;
    stCIP.irDriveRegs.bFeaturesReg = ENABLE_SMART;
    stCIP.irDriveRegs.bSectorCountReg = 1;
    stCIP.irDriveRegs.bSectorNumberReg = 1;
    stCIP.irDriveRegs.bCylLowReg = SMART_CYL_LOW;
    stCIP.irDriveRegs.bCylHighReg = SMART_CYL_HI;
    stCIP.irDriveRegs.bDriveHeadReg = DRIVE_HEAD_REG;
    stCIP.irDriveRegs.bCommandReg = SMART_CMD;

    bRet = DeviceIoControl(hDevice, SMART_SEND_DRIVE_COMMAND, &stCIP, sizeof(stCIP), &stCOP, sizeof(stCOP), &dwRet, NULL);
    if (bRet)
    {

    }
    else
    {
        dwRet = GetLastError();
        sprintf_s(m_stDrivesInfo[ucDriveIndex].m_csErrorString, "Error %d in reading SMART Enabled flag", dwRet);
    }
    return bRet;
}

BOOL DiscReader::ReadSMARTInfo(BYTE ucDriveIndex)
{
    HANDLE hDevice = NULL;
    WCHAR szT1[260] = { 0 };
    BOOL bRet = FALSE;
    DWORD dwRet = 0;

    wsprintf(szT1, L"\\\\.\\PHYSICALDRIVE%d", ucDriveIndex);
    hDevice = CreateFile(szT1, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_SYSTEM, NULL);
    if (hDevice != INVALID_HANDLE_VALUE)
    {
        bRet = DeviceIoControl(
            hDevice,
            SMART_GET_VERSION,
            NULL,
            0,
            &m_stDrivesInfo[ucDriveIndex].m_stGVIP,
            sizeof(GETVERSIONINPARAMS),
            &dwRet,
            NULL);
        if (bRet)
        {
            if ((m_stDrivesInfo[ucDriveIndex].m_stGVIP.fCapabilities & CAP_SMART_CMD) == CAP_SMART_CMD)
            {
                if (IsSmartEnabled(hDevice, ucDriveIndex))
                {
                    bRet = CollectDriveInfo(hDevice, ucDriveIndex);
                    bRet = ReadSMARTAttributes(hDevice, ucDriveIndex);
                }
            }
        }
        else
        {
            cout << "Is Not SMART HDD" << endl;
        }
    	
        CloseHandle(hDevice);
    }
    else ErrorMes(LPTSTR("CreateFile"));
    return bRet;
}
