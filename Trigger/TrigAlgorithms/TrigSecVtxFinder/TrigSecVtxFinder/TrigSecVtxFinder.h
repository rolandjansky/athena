/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ************************************************
//
// NAME:     TrigSecVtxFinder.h
// PACKAGE:  Trigger/TrigAlgorithms/TrigSecVtxFinder
//
// AUTHOR:   Xinlu Huang
// EMAIL:    xinlu.huang@cern.ch
// 
// ************************************************

#ifndef TRIGSECVTXFINDER_H
#define TRIGSECVTXFINDER_H

#include "GaudiKernel/ToolHandle.h"
#include "TrigTimeAlgs/TrigTimer.h"
#include "TrigInterfaces/FexAlgo.h"

#include "TrigTrackJetFinderTool/ITrigTrackJetFinderTool.h"
#include "TrigInDetVxInJetTool/ITrigInDetVxInJetTool.h"
#include "BeamSpotConditionsData/BeamSpotData.h"

#include "CxxUtils/checker_macros.h"
ATLAS_NO_CHECK_FILE_THREAD_SAFETY;  // legacy trigger code

namespace CLHEP {
  class HepLorentzVector;
}


class ISecVertexInJetFinder;

class TrigSecVtxFinder: public HLT::FexAlgo {

public:
  TrigSecVtxFinder(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~TrigSecVtxFinder();
  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement*, HLT::TriggerElement* outputTE);    
  HLT::ErrorCode hltFinalize();

private:

  int  m_algo;
  bool m_useBeamSpotFlag;
  bool m_trackJetTrkSelExt;
  unsigned int m_useJetDirection;

  unsigned int m_secVtx_num;
  unsigned int m_secVtx_numTrk;
  unsigned int m_secVtx_twoTrk;
  float m_secVtx_mass;
  float m_secVtx_energy;
  float m_secVtx_dl;
  float m_secVtx_dls;

  /** @brief TrigTrackJetFinder tool. */
  ToolHandle<ITrigTrackJetFinderTool> m_trackJetFinderTool;
  /** @brief SV algo tool. */
  ToolHandle<ITrigInDetVxInJetTool> m_secvtxFinderTool;

  HLT::ErrorCode getPrmVtxForFit(Trk::RecVertex&, const HLT::TriggerElement*);
  HLT::ErrorCode getSelectedTracks(std::vector<const TrigInDetTrack*>&, const HLT::TriggerElement*);
  HLT::ErrorCode getJetDirection(CLHEP::HepLorentzVector&, std::vector<const TrigInDetTrack*>&, float, const HLT::TriggerElement*);
  SG::ReadCondHandleKey<InDet::BeamSpotData> m_beamSpotKey { this, "BeamSpotKey", "BeamSpotData", "SG key for beam spot" };

};

#endif
