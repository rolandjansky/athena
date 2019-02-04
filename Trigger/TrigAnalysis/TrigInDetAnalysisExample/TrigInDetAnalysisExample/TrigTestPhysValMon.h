/* emacs: this is -*- c++ -*- */
/**
 **     @file    TrigTestPhysValMon.h
 **
 **     @author  mark sutton
 **     @date    Fri 11 Jan 2019 07:06:38 CET 
 **
 **     Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/


#ifndef TIDA_TRIGTESTPHYSVALMONTOOL_H
#define TIDA_TRIGTESTPHYSVALMONTOOL_H


#include "InDetBeamSpotService/IBeamCondSvc.h"

#include "TrigHLTMonitoring/IHLTMonTool.h"

#include "TrigInDetAnalysis/TrackFilter.h"
#include "TrigInDetAnalysis/TIDARoiDescriptor.h"
#include "TrigInDetAnalysis/TIDDirectory.h"
#include "TrigInDetAnalysis/Efficiency.h"

#include "TrigInDetAnalysisUtils/Filter_Track.h"
#include "TrigInDetAnalysisUtils/Filter_RoiSelector.h"
#include "TrigInDetAnalysisUtils/T_AnalysisConfig.h"
#include "TrigInDetAnalysisUtils/Associator_BestMatch.h"
#include "TrigInDetAnalysisUtils/TrackMatchDeltaR.h"
#include "TrigInDetAnalysisUtils/TrackMatchDeltaRCosmic.h"


#include "TrigInDetAnalysisExample/Analysis_Tier0.h"
#include "TrigInDetAnalysisExample/AnalysisConfig_Tier0.h"

#include "TrigInDetAnalysisExample/TrigTestBase.h"

#include "TrigInDetAnalysisExample/SigAnalysis.h"
#include "TrigInDetAnalysisExample/TrackEfficiency.h"


class TrigTestPhysValMon : public TrigTestBase {   

public:

  using TrigTestBase::addHistogram;
  using TrigTestBase::addMonGroup;

public:

  TrigTestPhysValMon( const std::string & type, const std::string & name, const IInterface* parent); 
  
  ~TrigTestPhysValMon();
  
  
  virtual StatusCode initialize(); //     {  return TrigTestBase::init(); } 

#ifdef ManagedMonitorToolBase_Uses_API_201401
  virtual StatusCode bookHistograms(); 
  virtual StatusCode fillHistograms(); 
  virtual StatusCode procHistograms(); 
#else
  virtual StatusCode bookHistograms( bool newEventsBlock, bool newLumiBlock, bool newRun ); 
  virtual StatusCode fillHistograms();
  virtual StatusCode procHistograms( bool /*newEventsBlock*/, bool /*newLumiBlock*/, bool newRun ); 
#endif

  // histogram registration
  // Inheriting class independant histogram registration                                                                                                                                
  virtual void addHistogram(TH1* h, const std::string& system) {
    msg(MSG::INFO) << "TrigTestPhysValMon::addHistogram() " << endmsg;
    if (regHist(h, m_sliceTag+system, run).isFailure()) {
      msg(MSG::ERROR) << "TrigTestPhysValMon::addHistorgram() failed to add histogram" << endmsg;
    }
  }

  // Dummy method to maintain compatibility with T_AnalysisConfig_Tier0                                                                                                                 
  virtual void addMonGroup(MonGroup *) {
    return;
  }
  
};


#endif //  TIDA_TRIGTESTPHYSVALMONTOOL_H
