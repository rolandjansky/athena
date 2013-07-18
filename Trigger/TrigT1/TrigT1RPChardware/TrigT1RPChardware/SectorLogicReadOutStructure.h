/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SectorLogicReadOutStructure_H
#define SectorLogicReadOutStructure_H

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
  static const ubit16 headerNum=2;
  static const ubit16 headerPos[headerNum];
  static const ubit16 headerLen[headerNum];
  static const ubit16 headerVal;
  static const ubit16 footerNum=2;
  static const ubit16 footerPos[footerNum];
  static const ubit16 footerLen[footerNum];
  static const ubit16 footerVal;
  //
};

#endif
