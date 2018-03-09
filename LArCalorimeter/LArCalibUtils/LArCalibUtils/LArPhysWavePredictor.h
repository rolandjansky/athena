//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef LARPHYSWAVEPREDICTOR_H
#define LARPHYSWAVEPREDICTOR_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "Identifier/HWIdentifier.h"
#include <vector>
#include <string>

class ILArBadChannelMasker;
class ILArBadChanTool;
class LArOnlineID_Base;

class LArPhysWavePredictor : public AthAlgorithm
{
 public:
  LArPhysWavePredictor(const std::string & name, ISvcLocator * pSvcLocator);

  ~LArPhysWavePredictor();

  StatusCode initialize() ; 
  StatusCode execute() {return StatusCode::SUCCESS;}
  StatusCode stop();
  StatusCode finalize(){return StatusCode::SUCCESS;}

 private:
  ToolHandle<ILArBadChannelMasker> m_maskingTool;
  ToolHandle<ILArBadChanTool> m_badChanTool;

  const LArOnlineID_Base* m_onlineHelper;
  bool m_testmode;
  bool m_storeEmpty;
  bool m_dumpMphysMcali;
  bool m_normalizeCali;
  bool m_isSC;
  
  std::vector<std::string> m_keyCali;
  std::string m_keyPhys;
  std::string m_keyMphysMcali;
  std::string m_keyIdealPhys;

  std::string m_groupingType;
  
  bool m_useJOCaliPulseParams;
  double m_Tcali;
  double m_Fstep;
  
  bool m_useJODetCellParams;
  double m_Omega0;
  double m_Taur;
  
  bool m_isHEC;
  
  bool m_useJOTdrift;
  std::vector<double> m_Tdrift ;
  bool m_doubleTriangle ;
  std::vector<double> m_Tdrift2 ;
  std::vector<double> m_wTriangle2 ;
  
  bool m_useJOPhysCaliTdiff;
  int  m_timeShiftByIndex ;
  bool m_timeShiftByHelper ;
  bool m_timeShiftByLayer ;
  std::vector<unsigned int> m_TshiftLayer ;
  bool m_timeShiftByFEB ;
  int m_timeShiftGuardRegion ;
  

  void notFoundMsg(const HWIdentifier chid, const int gain, const char* value);
};

#endif
