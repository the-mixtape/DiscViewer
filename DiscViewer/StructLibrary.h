#pragma once
#include <Windows.h>


//SMART structures

typedef struct
{
    BYTE m_ucAttribIndex;
    DWORD m_dwAttribValue;
    BYTE m_ucValue;
    BYTE m_ucWorst;
    DWORD m_dwThreshold;
}ST_SMART_INFO;


typedef struct
{
    WORD wGenConfig;
    WORD wNumCyls;
    WORD wReserved;
    WORD wNumHeads;
    WORD wBytesPerTrack;
    WORD wBytesPerSector;
    WORD wSectorsPerTrack;
    WORD wVendorUnique[3];
    BYTE sSerialNumber[20];
    WORD wBufferType;
    WORD wBufferSize;
    WORD wECCSize;
    BYTE sFirmwareRev[8];
    BYTE sModelNumber[39];
    WORD wMoreVendorUnique;
    WORD wDoubleWordIO;
    WORD wCapabilities;
    WORD wReserved1;
    WORD wPIOTiming;
    WORD wDMATiming;
    WORD wBS;
    WORD wNumCurrentCyls;
    WORD wNumCurrentHeads;
    WORD wNumCurrentSectorsPerTrack;
    WORD ulCurrentSectorCapacity;
    WORD wMultSectorStuff;
    DWORD ulTotalAddressableSectors;
    WORD wSingleWordDMA;
    WORD wMultiWordDMA;
    BYTE bReserved[127];
}ST_IDSECTOR;

typedef struct
{
    BYTE  bDriverError;
    BYTE  bIDEStatus;
    BYTE  bReserved[2];
    DWORD dwReserved[2];
} ST_DRIVERSTAT;

typedef struct
{
    DWORD      cBufferSize;
    ST_DRIVERSTAT DriverStatus;
    BYTE       bBuffer[1];
} ST_ATAOUTPARAM;


typedef struct
{
    GETVERSIONINPARAMS m_stGVIP;
    ST_IDSECTOR m_stInfo;
    ST_SMART_INFO m_stSmartInfo[256];
    BYTE m_ucSmartValues;
    BYTE m_ucDriveIndex;
    char m_csErrorString[1000];
}ST_DRIVE_INFO;

typedef struct
{
    BOOL m_bCritical;
    BYTE m_ucAttribId;
    char m_csAttribName[100];
    char m_csAttribDetails[1000];
}ST_SMART_DETAILS;