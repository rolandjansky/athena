/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigT1RPChardware_SectorLogicReadOutStructure_H
#define TrigT1RPChardware_SectorLogicReadOutStructure_H

#include "TrigT1RPChardware/RPCReadOut.h"
#include "TrigT1RPChardware/Lvl1Def.h"


class SectorLogicReadOutStructure : public RPCReadOut {
 public:
  SectorLogicReadOutStructure();
  SectorLogicReadOutStructure(ubit16 inputData);
  ~SectorLogicReadOutStructure() {};

  void setInit();
  //
  virtual ubit16 makeHeader(ubit16 *inputData);
  virtual ubit16 makeSubHeader(){ return 0;};
  virtual ubit16 makeBody(ubit16 *inputData) {return (*inputData)&0x0;};
  virtual ubit16 makeFooter(ubit16  errorCode){return errorCode&0x0;};
  virtual ubit16 makeFooter(ubit16 *errorCode);

  //**************************************************//
  // general user methods to decode ReadOut stream    //
  //**************************************************//
  ubit16 decodeFragment(ubit16 inputWord, char &field);
//
  virtual bool isHeader();
  virtual bool isBody ();
  virtual bool isFooter();
//  
 private:
  //
  //header
  ubit16 m_secid;
//footer
  ubit16 m_footer;
//
// internal objects;
//
  //
  // some static parameter describing the structure
  //
  static const ubit16 s_headerNum=2;
  static const ubit16 s_headerPos[s_headerNum];
  static const ubit16 s_headerLen[s_headerNum];
  static const ubit16 s_headerVal;
  static const ubit16 s_footerNum=2;
  static const ubit16 s_footerPos[s_footerNum];
  static const ubit16 s_footerLen[s_footerNum];
  static const ubit16 s_footerVal;
  //
};

#endif
