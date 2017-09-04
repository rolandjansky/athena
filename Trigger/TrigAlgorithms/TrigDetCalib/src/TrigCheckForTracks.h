/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigCheckForTracks_H
#define TrigCheckForTracks_H

/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : TrigCheckForTracks.h
/// Package : Trigger/TrigAlgorithms/TrigDetCalib
/// Author  : Nicolas Berger
/// Created : Sep. 2008
///
/// DESCRIPTION: simple hypo algorithms to limit eta range
///
///////////////////////////////////////////////////////////////////////////////////////////////////////
#include <vector>
#include <string>

#include "TrigInterfaces/AllTEAlgo.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"

#include "TrigDetCalib/ITrigROBSelector.h"
#include "eformat/SourceIdentifier.h"

class TriggerElement;
class PartialEventBuildingInfo;

class TrigCheckForTracks : public HLT::AllTEAlgo {

 public:

  TrigCheckForTracks(const std::string& name, ISvcLocator* pSvcLocator);
  ~TrigCheckForTracks();

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& input, unsigned int output);
  

  HLT::ErrorCode hltBeginRun();
  HLT::ErrorCode hltEndEvent();
  
 private:

  bool m_executedEvent ; // This will ensure we don't execute the algorithm more than once in the event.
  
  
ToolHandle<ITrigROBSelector> m_robSelector;
  
  int m_acceptedEvts;
  int m_rejectedEvts;
  int m_errorEvts;

  // Switch to accept all the events.
  bool m_acceptAll;

  // Save HLT results
  bool m_addCTPResult, m_addL2Result, m_addEFResult, m_addHLTResult;

  // Switch on Monitoring:

  int                           m_n_IsoTracks ;
  std::vector<double>           m_pT ;
  std::vector<double>           m_pT_Iso ;
  std::vector<double>           m_dR ;
  std::vector<double>           m_eta ;
  std::vector<double>           m_phi ;
  std::vector<DETID>            m_dets;
  std::vector<int>              m_nROBs;
  std::vector<double>           m_ROB_eta;
  std::vector<double>           m_ROB_phi;


  double           m_dR0 ; 
  double           m_dR0_overlap ;
  double           m_pT_min ;
  double           m_pT_min_iso ;
  double           m_etaEdge ;
  double           m_etaLowEdge ;
  double           m_etaWidth ;
  double           m_phiWidth ;
  std::string      m_tracksName ;
  bool             m_doNotPass ;
  bool             m_lookForAnyTracks ;

  
  std::string m_pebLabel;

  // Hardcoded extra ROBs that should be shipped in all cases
  std::vector<uint32_t> m_extraROBs;    

  std::vector<eformat::SubDetector> m_trigResults;

  // Timing:
  ITrigTimerSvc*            m_timerSvc;
  std::vector<TrigTimer*>   m_timers;
};
#endif
