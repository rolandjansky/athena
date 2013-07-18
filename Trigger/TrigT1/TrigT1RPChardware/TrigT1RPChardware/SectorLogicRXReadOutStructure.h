/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SectorLogicRXReadOutStructure_H
#define SectorLogicRXReadOutStructure_H

#include "TrigT1RPChardware/RPCReadOut.h"
#include "TrigT1RPChardware/Lvl1Def.h"

#include<vector>

class SectorLogicRXReadOutStructure : public RPCReadOut {
 public:
  SectorLogicRXReadOutStructure();
  SectorLogicRXReadOutStructure(ubit16 inputData);
  ~SectorLogicRXReadOutStructure() {};

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
  virtual bool isSubHeader();
  bool isBody() { return false; }
  bool isInputHeader ();
  bool isOutputHeader ();
  bool isInputBody ();
  bool isOutputBody ();
  virtual bool isFooter();
//  
  //header
  ubit16 slid() {return m_slid;};
  ubit16 fel1id() {return m_fel1id;};
  // subheader
  ubit16 slbcid() {return m_slbcid;};
  // input header
  ubit16 inputRowinBcid() {return m_inputRowinBcid;};
  ubit16 inputPadId() {return m_inputPadId;};
  // output header
  ubit16 outputRowinBcid() {return m_outputRowinBcid;} ;
  // input body
  ubit16 inputTriggerBcid() {return m_inputTriggerBcid;};
  ubit16 inputThreshold() {return m_inputThreshold;} ;
  ubit16 inputRoi() {return m_inputRoi;};
  ubit16 inputOverlapEta() {return m_inputOverlapEta;};
  ubit16 inputOverlapPhi() {return m_inputOverlapPhi;};
  ubit16 inputOuterPlane() {return m_inputOuterPlane;};
  // output body
  ubit16 nTriggerCand() {return m_nTriggerCand;}
  ubit16 outputTriggerBcid(int /*nCand*/) {return m_outputTriggerBcid;}
  ubit16 outputThreshold(int nCand) {return m_outputThreshold[nCand];}
  ubit16 outputRoi(int nCand) {return m_outputRoi[nCand];}
  ubit16 outputOverlap(int nCand) {return m_outputOverlap[nCand];}
  ubit16 hasMoreThan2TriggerCand () {return m_moreThan2TriggerCand;}
  //footer
  ubit16 status() {return m_status;};
  ubit16 crc() {return m_crc;};

  ubit16 nOfTriggerWords() {return m_outTriggerWord.size();}
  bool isOutputDecoded() {return ( nOfTriggerWords()== 3);}

 private:
  //
  //header
  ubit16 m_slid;
  ubit16 m_fel1id;
  // subheader
  ubit16 m_slbcid;
  // input header
  ubit16 m_inputRowinBcid;
  ubit16 m_inputPadId;
  // output header
  ubit16 m_outputRowinBcid;
  //  8 or 12  bits of trigger output word 
  std::vector<ubit16> m_outTriggerWord;   
  // input body
  ubit16 m_inputTriggerBcid;
  ubit16 m_inputThreshold;
  ubit16 m_inputRoi;
  ubit16 m_inputOverlapEta;
  ubit16 m_inputOverlapPhi;
  ubit16 m_inputOuterPlane;
  // output body
  ubit16 m_outputTriggerBcid;
  ubit16 m_outputThreshold[2];
  ubit16 m_outputOverlap[2];
  ubit16 m_outputRoi[2];
  ubit16 m_nTriggerCand;
  bool m_moreThan2TriggerCand;
  //footer
  ubit16 m_status;
  ubit16 m_crc;

//
// internal objects;
//
  //
  // some static parameter describing the structure
  //
  static const ubit16 headerNum=3;
  static const ubit16 headerPos[headerNum];
  static const ubit16 headerLen[headerNum];
  static const ubit16 headerVal;

  static const ubit16 subHeaderNum=2;
  static const ubit16 subHeaderPos[subHeaderNum];
  static const ubit16 subHeaderLen[subHeaderNum];
  static const ubit16 subHeaderVal;

  static const ubit16 inputHeaderNum=3;
  static const ubit16 inputHeaderPos[inputHeaderNum];
  static const ubit16 inputHeaderLen[inputHeaderNum];
  static const ubit16 inputHeaderVal;

  static const ubit16 outputHeaderNum=3;
  static const ubit16 outputHeaderPos[outputHeaderNum];
  static const ubit16 outputHeaderLen[outputHeaderNum];
  static const ubit16 outputHeaderVal;

  static const ubit16 inputBodyNum=7;
  static const ubit16 inputBodyPos[inputBodyNum];
  static const ubit16 inputBodyLen[inputBodyNum];
  static const ubit16 inputBodyVal;

  static const ubit16 outputBodyNum=4;
  static const ubit16 outputBodyPos[outputBodyNum];
  static const ubit16 outputBodyLen[outputBodyNum];
  static const ubit16 outputBodyVal;

  static const ubit16 footerNum=3;
  static const ubit16 footerPos[footerNum];
  static const ubit16 footerLen[footerNum];
  static const ubit16 footerVal;
  //
};

#endif
