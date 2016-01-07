/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONEF_TRIGMUONEFROIAGGREGATOR_H__
#define TRIGMUONEF_TRIGMUONEFROIAGGREGATOR_H__

// ************************************************
//
// NAME:     TrigMuonEFRoiAggregator.h
// PACKAGE:  Trigger/TrigAlgorithms/TrigMuonEF
//
// AUTHORS:   Alex Oh, Martin Woudstra, Mark Owen.
//
// ************************************************

#include "TrigInterfaces/AllTEAlgo.h"
#include "TrigInDetEvent/TrigInDetTrack.h"

class TrigMuonEFRoiAggregator : public HLT::AllTEAlgo {

 public:

  /// Constructor
  TrigMuonEFRoiAggregator(const std::string & name, ISvcLocator* pSvcLocator);

  /// Destructor
  ~TrigMuonEFRoiAggregator();

  /// Initialize
  HLT::ErrorCode hltInitialize();

  /// Execute for each event
  HLT::ErrorCode hltExecute(std::vector<std::vector<HLT::TriggerElement*> >&,unsigned int);
  
  /// Finalize
  HLT::ErrorCode hltFinalize();

 protected:

  //
  // JobOptions Properties
  //
  /** Min Pt for muon 1 */
  double m_PtLo;
  /** Min Pt for muon 2 */
  double m_PtHi;
  /** Option to also copy trackparticles */
  bool m_copyTracks;

  //
  // Monitoring variables
  //
  int m_nRoiIn;
  int m_nTrkIn;

};//class TrigMuonEFRoiAggregator


#endif // TRIGMUONEF_TRIGMUONEFROIAGGREGATOR_H__

