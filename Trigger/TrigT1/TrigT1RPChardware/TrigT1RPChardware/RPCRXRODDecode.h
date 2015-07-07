/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RPCRXRODDecode_H
#define RPCRXRODDecode_H

#include "TrigT1RPChardware/Lvl1Def.h"
#include "TrigT1RPChardware/RPCRODStructure.h"
#include "TrigT1RPChardware/SectorLogicRXReadOut.h"
#include "MuonCablingTools/BaseObject.h"

class RPCRXRODDecode : public BaseObject {

public:
RPCRXRODDecode();
~RPCRXRODDecode();
sbit32 gimeRODDataAddress() { return RODDataAddress;};
void RODAddresses(const RODword *RODData,
                  const sbit32 numberOfStatusElements,
                  const sbit32 statusBlockPosition);
void RODHeader(const RODword *ROBData);
int pushWord(const ubit16 inword);
int pushWord(const RODword inword, ubit16 j);
MatrixReadOut      *CMFragment() {return &CMRO;};
SectorLogicRXReadOut *SLFragment() {return &SLRO;};
void gimeCMROData();
void RODHeaderDisplay();
//
// ROD structure control flags
//
ubit16 gimeCMFlag(){ return CMFlag;};
ubit16 gimePADFlag(){ return PADFlag;};
ubit16 gimeRXFlag(){ return RXFlag;};
void enablePrintOut();
void disablePrintOut();
//
// Special for sector logic
//
 void setSLFragmentFound(bool slFound) {m_slFound=slFound;};
 
// 
// ROD HEADER
//
RODword headerMarker;
RODword headerSize;
RODword formatVersion;
RODword sourceIdentifier;
RODword Level1ID;
RODword BunchXingID;
RODword Level1Type;
RODword DetectorEventType;
ubit16 SourceReserved;
ubit16 SourceModuleType;
ubit16 SourceSubDetectorID;
ubit16 SourceModuleID;
// 
// Sector, PAd and Matrix identifiers
//
ubit16 SectorID;
ubit16 PadID;
ubit16 CMID;
ubit16 CMFragCheck;
//
// service objects
//
RXReadOutStructure     RXROS;
PadReadOutStructure    PDROS;
MatrixReadOutStructure CMROS;
MatrixReadOut CMRO;
SectorLogicRXReadOutStructure SLROS;
SectorLogicRXReadOut SLRO;

private:
bool isSLBody;
//
// ROD Data address
//
sbit32 RODDataAddress;
//
// define "previous" type of 16-bit data record
//
enum recType {Empty,CMHead,CMSub,CMBod,CMFoot,
              PadHead,PadSub,PadPre,PadFoot,
	      SLHead,SLFoot,
	      RXHead,RXFoot};
recType previousRecord;
//
// RPC data markers
//
ubit16 field;
ubit16 noRecord16;
RODword noRecord32;
//ubit16 PADSubHeader;
//ubit16 PADPreFooter;
//ubit16 reserved3;
ubit16 reserved4;
//ubit16 reserved5;
//
// data structure control flags
//
ubit16 CMFlag;
ubit16 PADFlag;
ubit16 RXFlag;
//
// enable printouts
//
bool m_enablePrintOut;
// 
// the SL fragment was ound
bool m_slFound;

};

#endif

