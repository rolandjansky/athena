/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigT1RPChardware_RXReadOutStructure_H
#define TrigT1RPChardware_RXReadOutStructure_H

#include "TrigT1RPChardware/RPCReadOut.h"
#include "TrigT1RPChardware/MatrixReadOut.h"
#include "TrigT1RPChardware/MatrixReadOutStructure.h"
#include "TrigT1RPChardware/PadReadOutStructure.h"
#include "TrigT1RPChardware/Lvl1Def.h"

class RXReadOutStructure : public RPCReadOut {
 public:
 enum DataVersion {Atlas, Simulation0, Simulation1 };
     
 RXReadOutStructure(DataVersion vers = Atlas);
 RXReadOutStructure(ubit16 inputData, DataVersion vers = Atlas);
 ~RXReadOutStructure() = default;
 void setInit();
//
virtual ubit16 makeHeader(ubit16 *inputData);
virtual ubit16 makeSubHeader(){ return 0;};
//virtual ubit16 makeBody(ubit16 *inputData) {return (*inputData)&0x0;};
virtual ubit16 makeBody(ubit16 *inputData) {return *inputData;};
virtual ubit16 makeFooter(ubit16 errorCode);
virtual ubit16 makeFooter(ubit16 *errorCode);

 //**************************************************//
 // general user methods to decode ReadOut stream    //
 //**************************************************// 
ubit16 RXid()       {return m_secid;};
ubit16 status()     {return m_status;};
ubit16 errorCode()  {return m_errorCode;}; 
ubit16 decodeFragment(ubit16 inputWord, char &field);
virtual bool isHeader();
virtual bool isBody () { return false;};
virtual bool isFooter();
//  
 private:
//
 ubit16 m_secid;
 ubit16 m_status;
 ubit16 m_errorCode;
 DataVersion m_dataVersion;
//
// some static parameter describing the structure
//
static const ubit16 s_headerNum=3;
static const ubit16 s_headerPos[s_headerNum];
static const ubit16 s_headerLen[s_headerNum];
static const ubit16 s_headerVal;
static const ubit16 s_footerNum=2;
static const ubit16 s_footerPos[s_footerNum];
static const ubit16 s_footerLen[s_footerNum];
static const ubit16 s_footerVal;
};//end-of-class RXReadOutStructure

#endif
