/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigT1RPChardware_RPCROD_H
#define TrigT1RPChardware_RPCROD_H

#include<iostream>
#include "TrigT1RPChardware/RXReadOut.h"
#include "TrigT1RPChardware/RPCRODStructure.h"
#include "TrigT1RPChardware/Lvl1Def.h"
#include "MuonCablingTools/BaseObject.h"

class RPCROD : public BaseObject {

public:
RPCROD(ubit16 subsystem, ubit16 RODID, 
        RODword LVL1ID, ubit16 ECRID,
        ubit16 BCID, ubit16 LVL1Type, RODword EvType,  
        RXReadOut *RX1, RXReadOut *RX2);
~RPCROD();
void reset();
void makeFragment();
void makeHeader();
void makeFooter();
void makeBody();
void makeStatus();
void makeData();
RODword readHeaderMarker();
RODword readHeaderSize();
RODword readHeaderword();
RODword readFormatVersion();
RODword readSourceID();
RODword readLVL1ID();
RODword readBunchID();
RODword readLVL1Type();
RODword readDetectorType(); 
RODword readFooter();
RODword readBody();
RODword readStatus();
RODword readData();
void   topRODBody(); 
RODword readRODWord();
//RPCRODStructure getHeader();
//RPCRODStructure getFooter();
//ubit16 numberOfFragmentWords (){return numberOfWordsInFragment;};
void bytestream(std::ostream &stream);
//void decodeBytestream();

private:
//
static const ubit16 s_numberOfSectors=2;
static const ubit16 s_maxNumberOfWordsInHeader   =15;
static const ubit16 s_maxNumberOfWordsInTrailer  = 5;
//
// control flags for Body scanninng ...
//
ubit16 m_addressOfWordScanned;
ubit16 m_numberOfRXFragments;
RODword m_numberOfWordsInFragment;
RODword m_numberOf16bitWords;
RODword m_numberOfDataElements;
static const RODword s_numberOfStatusElements=1;
static const RODword s_statusBlockPosition=0;
ubit16 m_newRXIndex;
ubit16 m_newRXRO;
ubit16 m_numberOfWordsRead;
ubit16 m_numberOfWordsInRXRO;
ubit16 m_endOfRXFragments;
ubit16 m_numberOfTrailerWordsScanned;
ubit16 m_numberOfStatusWordsScanned;
ubit16 m_numberOfDataWordsScanned;
RXReadOut *m_currentRXRO;
//
//
//RPCRODStructure RROS;
RXReadOut *m_RXlist[2];
ubit16  m_subsystem;
ubit16  m_RODID;
RODword m_LVL1ID; 
ubit16  m_ECRID;
ubit16  m_BunchCrossingID2;
ubit16  m_Level1TriggerType2;
ubit16  m_EventType;
//
//Header
//
RODword m_HeaderMarker;
RODword m_HeaderSize;
RODword m_FormatVersionNumber;
RODword m_SourceIdentifier;
RODword m_Level1ID;
RODword m_BunchCrossingID;
RODword m_Level1TriggerType;
RODword m_DetectorEventType;
//
// Status and Data
//
RODword m_StatusElements[s_numberOfStatusElements];
RXReadOut *m_RXFragment[2];
//
// Trailer
//
RODword m_NumberOfStatusElements;
RODword m_NumberOfDataElements;
RODword m_StatusBlockPosition;
//
// useful arrays
//
RODword m_HeaderArray[s_maxNumberOfWordsInHeader];
RODword m_FooterArray[s_maxNumberOfWordsInTrailer];
};
#endif
