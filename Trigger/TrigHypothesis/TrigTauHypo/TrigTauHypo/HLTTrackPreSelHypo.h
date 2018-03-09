/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HLTTRACKPRESELHYPO_H
#define HLTTRACKPRESELHYPO_H

/********************************************************************
 *
 * NAME:     HLTTrackPreSelHypo.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigTauHypo
 *
 * AUTHORS:   Marcelo Vogel (mavogel@cern.ch)
 *            Soshi Tsuno
 *            P.O. DeViveiros
 * CREATED:   May 26, 2014
 *
 * DESCRIPTION:
 *
 * Implementation of fast-tracking based pre-selection on
 * tau candidates starting from L1 seeds
 *
 *********************************************************************/

#include "TrigInterfaces/HypoAlgo.h"

class StoreGateSvc;

namespace HLT {
  class TriggerElement;
}

class HLTTrackPreSelHypo : public HLT::HypoAlgo
{

 public:

  /** constructor of EF tauRec hypo algo */
  HLTTrackPreSelHypo(const std::string& name, ISvcLocator* pSvcLocator);
  /** destructor */
  ~HLTTrackPreSelHypo();

  /** HLT method for initialize */
  HLT::ErrorCode hltInitialize();

  /** HLT method for finalize */
  HLT::ErrorCode hltFinalize();

  /** HLT method for execute Hypo on  a TE.
      input is TE, output is pass=True or False */
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);

 private:

  /** Configurables */

  float m_lowerPtCut;
  float m_lowerTrackPtCut;
  float m_clusterCone;
  float m_coreSize;
  float m_outerSize;
  float m_deltaRLeadTrkRoI;
  int m_tracksInCoreCut;
  int m_tracksInIsoCut;
  float m_deltaZ0Cut;

  bool m_rejectNoTracks;
  bool m_relax_highpt;
  double m_highpt_threshold;

  bool m_mva_tracking;

  int  m_cutCounter;
  //Monitoring: number of tracks in core region
  int m_nTracksInCore;
  //Monitoring: number of tracks in outer region
  int m_nTracksInIso;

};
#endif

