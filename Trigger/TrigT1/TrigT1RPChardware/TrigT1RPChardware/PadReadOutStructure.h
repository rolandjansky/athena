/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigT1RPChardware_PadReadOutStructure_H
#define TrigT1RPChardware_PadReadOutStructure_H

#include "TrigT1RPChardware/RPCReadOut.h"
#include "TrigT1RPChardware/MatrixReadOut.h"
#include "TrigT1RPChardware/MatrixReadOutStructure.h"
#include "TrigT1RPChardware/Lvl1Def.h"


class PadReadOutStructure : public RPCReadOut {
 public:
  PadReadOutStructure();
  PadReadOutStructure(ubit16 inputData);
  ~PadReadOutStructure() = default;

  void setInit();
  //
  virtual ubit16 makeHeader(ubit16 *inputData);
  virtual ubit16 makeBody(ubit16 *inputData) {return *inputData;};
  //ubit16 makeFooter_new(ubit16* errorCode);
  virtual ubit16 makeSubHeader(){ return 0;};
  virtual ubit16 makeFooter(ubit16 errorCode){return errorCode;};
  virtual ubit16 makeFooter(ubit16 *errorCode);

  //**************************************************//
  // general user methods to decode ReadOut stream    //
  //**************************************************//
  ubit16 decodeFragment(ubit16 inputWord, char &field);
  ubit16 padid()      {return m_padid;};
  ubit16 status()     {return m_status;};
  ubit16 errorCode()  {return m_errorCode;}; 
  ubit16 l1id()      {return m_l1id;};
  ubit16 bcid()      {return m_bcid;};
  ubit16 fifoCM()    {return m_fifoCM;};
  ubit16 fifoOR()    {return m_fifoOR;};
  ubit16 fifoL1()    {return m_fifoL1;};
  ubit16 fifoPAD()   {return m_fifoPAD;};
  ubit16 errorSH()   {return m_errorSH;};
  ubit16 errorL1()   {return m_errorL1;};
  ubit16 errorH()    {return m_errorH;};
  ubit16 errorCM()   {return m_errorCM;};
  ubit16 errorCMID() {return m_errorCMID;};



  virtual bool isHeader();
  bool         isSubHeader();
  virtual bool isBody ();
  virtual bool isFooter();
  bool isPreFooter();
  //  
 private:
  //
  //header
  ubit16 m_padid;
  ubit16 m_l1id;
  //subheader
  ubit16 m_bcid;

  //prefooter
  ubit16 m_fifoCM ;//busy status (fifo on CMs) OR
  ubit16 m_fifoOR;//busy status (fifo of CMs on pad) OR (fifo L1)
  ubit16 m_fifoL1 ;//busy status (fifo on L1)
  ubit16 m_fifoPAD; //busy status (fifo of CMs on pad)
  ubit16 m_status;

  //footer
  ubit16 m_errorSH;//error in subheader
  ubit16 m_errorL1;//error in L1ID 
  ubit16 m_errorH ;//error in Header
  ubit16 m_errorCM; //error in CM
  ubit16 m_errorCMID;// Id of CM with error
  ubit16 m_errorCode;
  //
  // internal objects;
  //
  MatrixReadOutStructure m_MRS;
  MatrixReadOut m_MR;

  //
  // some static parameter describing the structure
  //
  static const ubit16 s_headerNum=3;
  static const ubit16 s_headerPos[s_headerNum];
  static const ubit16 s_headerLen[s_headerNum];
  static const ubit16 s_headerVal;
  static const ubit16 s_subHeaderNum=2;
  static const ubit16 s_subHeaderPos[s_subHeaderNum];
  static const ubit16 s_subHeaderLen[s_subHeaderNum];
  static const ubit16 s_subHeaderVal;
  static const ubit16 s_prefooterNum=5;
  static const ubit16 s_prefooterPos[s_prefooterNum];
  static const ubit16 s_prefooterLen[s_prefooterNum];
  static const ubit16 s_prefooterVal;
  static const ubit16 s_footerNum=6;
  static const ubit16 s_footerPos[s_footerNum];
  static const ubit16 s_footerLen[s_footerNum];
  static const ubit16 s_footerVal;
  //
  // pointer to a temporary array
  //
  ubit16* m_vectorStruct;
  //
};

#endif
