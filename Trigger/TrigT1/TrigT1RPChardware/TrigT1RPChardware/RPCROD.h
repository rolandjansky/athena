/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RPCROD_H
#define RPCROD_H

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
static const ubit16 numberOfSectors=2;
static const ubit16 maxNumberOfWordsInHeader   =15;
static const ubit16 maxNumberOfWordsInTrailer  = 5;
//
// control flags for Body scanninng ...
//
ubit16 addressOfWordScanned;
ubit16 numberOfRXFragments;
RODword numberOfWordsInFragment;
RODword numberOf16bitWords;
RODword numberOfDataElements;
static const RODword numberOfStatusElements=1;
static const RODword statusBlockPosition=0;
ubit16 newRXIndex;
ubit16 newRXRO;
ubit16 numberOfWordsRead;
ubit16 numberOfWordsInRXRO;
ubit16 endOfRXFragments;
ubit16 numberOfTrailerWordsScanned;
ubit16 numberOfStatusWordsScanned;
ubit16 numberOfDataWordsScanned;
RXReadOut *currentRXRO;
//
//
//RPCRODStructure RROS;
RXReadOut *m_RXlist[2];
ubit16  m_subsystem;
ubit16  m_RODID;
RODword m_LVL1ID; 
ubit16  m_ECRID;
ubit16  BunchCrossingID;
ubit16  Level1TriggerType;
ubit16  EventType;
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
RODword m_StatusElements[numberOfStatusElements];
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
RODword m_HeaderArray[maxNumberOfWordsInHeader];
RODword m_FooterArray[maxNumberOfWordsInTrailer];
};
#endif
