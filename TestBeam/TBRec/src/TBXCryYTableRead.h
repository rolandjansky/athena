/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBREC_TBXCRYYTABLEREAD_H
#define TBREC_TBXCRYYTABLEREAD_H
// 
// class TBXCryYTableRead 
//
#include "AthenaBaseComps/AthAlgorithm.h"

class IToolSvc;
class TBEventInfo;

class TBXCryYTableRead: public AthAlgorithm {
 public:    
  
  TBXCryYTableRead(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~TBXCryYTableRead();
  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override;
  
 private: 
  void clear();
  /** Get Xcryo and Ytable from a text file */
  StatusCode getXcryoYtable(float &x, float &y, float &eBeam);
  
  int m_nEvent;                     // counter
  int m_nEventRandomTrigger;        // counter
  bool m_first;                     // First event flag

  int m_nRun;               /** Run number */
  float m_beamMom;          /** Beam momentum */
  float m_xCryo;            /** CryoX */
  float m_yTable;           /** TableY */

  std::string m_txtFileWithXY;            // name of the file with X,y info

  TBEventInfo * m_eventinfo;
};

#endif
