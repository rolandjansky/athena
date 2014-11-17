/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBXCRYYTABLEREAD
#define TBXCRYYTABLEREAD
// 
// class TBXCryYTableRead 
//
#include "AthenaBaseComps/AthAlgorithm.h"

class StoreGateSvc;
class IToolSvc;
class TBEventInfo;

class TBXCryYTableRead: public AthAlgorithm {
 public:    
  
  TBXCryYTableRead(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~TBXCryYTableRead();
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
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
