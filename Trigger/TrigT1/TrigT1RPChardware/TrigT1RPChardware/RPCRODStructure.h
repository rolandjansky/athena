/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigT1RPChardware_RPCRODStructure_H
#define TrigT1RPChardware_RPCRODStructure_H

#include "TrigT1RPChardware/RPCReadOut.h"
#include "TrigT1RPChardware/MatrixReadOut.h"
#include "TrigT1RPChardware/MatrixReadOutStructure.h"
#include "TrigT1RPChardware/PadReadOutStructure.h"
#include "TrigT1RPChardware/RXReadOutStructure.h"
#include "TrigT1RPChardware/SectorLogicReadOut.h"
#include "TrigT1RPChardware/Lvl1Def.h"

class RPCRODStructure : public RPCReadOut {
 public:
 RPCRODStructure();
 RPCRODStructure(ubit16 inputData);
 ~RPCRODStructure();
 void    decodeSourceID(RODword sourceID); 
 ubit16  getSourceIDReserved(){return m_sourceIDreserved;};
 ubit16  getSourceIDModuleType(){return m_moduleType;};
 ubit16  getSourceIDSubdetectorID(){return m_subDetectorID;};
 ubit16  getSourceIDRODID(){return m_RODID;};
//
//virtual ubit16 makeHeader(ubit16 *inputData){return (*inputData)&0x0;};
virtual ubit16 makeHeader(ubit16 *inputData){return *inputData;};
virtual ubit16 makeSubHeader(){ return 0;};
//virtual ubit16 makeBody(ubit16 *inputData) {return (*inputData)&0x0;};
virtual ubit16 makeBody(ubit16 *inputData) {return *inputData;};
//virtual ubit16 makeFooter(ubit16 errorCode){return errorCode&0x0;};
virtual ubit16 makeFooter(ubit16 errorCode){return errorCode;};
virtual ubit16 makeFooter(ubit16 *errorCode){return (*errorCode)&0x0;};
//**************************************************//
// general user methods to decode ReadOut stream    //
//**************************************************// 
//ubit16 decodeFragment(ubit16 inputWord, char &field);
virtual bool isHeader(){return false;};
virtual bool isBody () {return false;};
virtual bool isFooter(){return false;};
//  
RODword getHeaderMarker(){return s_headerMarker;};
RODword getHeaderSize()  {return s_headerSize;};
RODword getFooterSize()  {return s_footerSize;};
RODword getFormatVersion(){return s_formatVersion;};
RODword getSourceID(ubit16 subsystem, ubit16 RODID);
ubit16  getEmptyDataWord(){return s_emptyDataWord;};

 private:
ubit16 m_sourceIDreserved;
ubit16 m_moduleType;
ubit16 m_subDetectorID;
ubit16 m_RODID;
//
// some static parameter describing the structure
//
static const RODword s_headerMarker      =0xee1234ee;
static const RODword s_headerSize        =8;
static const RODword s_formatVersion     =0x00000000;
// parameters needed to define SourceID
static const ubit16  s_sourceIDreserved  =0x00;
static const ubit16  s_moduleType        =0x00;
static const ubit16  s_subDetectorIDA    =0x65;
static const ubit16  s_subDetectorIDC    =0x66;
//
static const ubit16  s_emptyDataWord     =0xf000;
//
// some auxiliary parameters
//
static const RODword s_footerSize        =3;
//
void setInit();
ubit16  getSubDetectorID(ubit16 subsystem);
};//end-of-class RPCRODStructure

#endif
