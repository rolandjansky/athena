/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGIDTAUHYPO_H
#define TRIGIDTAUHYPO_H
/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : T2IDTauHypo.h
/// Package : Trigger/TrigHypothesis/TrigTauHypo/
/// Author  : Olya Igonkina (Olga.Igonkina@cern.ch)
/// Created : March 2006
///
/// DESCRIPTION:
///
/// checks ID LVL2 output for Tau hypothesis
///
///////////////////////////////////////////////////////////////////////////////////////////////////////
#include <vector>

#include "TrigInterfaces/HypoAlgo.h"

namespace HLT {
  class TriggerElement;
}

class T2IDTauHypo : public HLT::HypoAlgo {

 public:
 
/** Constructor */
  T2IDTauHypo(const std::string& name, ISvcLocator* pSvcLocator);
/** Destructor */
  ~T2IDTauHypo();

/** HLT method to execute */
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* inputTE, bool& pass);
/** HLT method to initialize */
  HLT::ErrorCode hltInitialize();
/** HLT method to finalize */
  HLT::ErrorCode hltFinalize();


 private:

   SimpleProperty<float> m_prop_sumPtRatio_MPCut;
   SimpleProperty<float> m_prop_sumPtRatio_1PCut;


  // define the properties:
  //----------------------------
  /** Cut on minimum number of tracks Ntrk >= NtrkMinCut */
  int   m_nTrkMin_Cut;
  /** Cut on maximu number of tracks Ntrk <= NtrkMaxCut */
  int   m_nTrkMax_Cut;
  /** cut on ratio of sumPt in isolation to core, any prongs */
  float m_sumPtRatio_Cut ;
  /** cut on ratio of sumPt in isolation to core, 1P */
  float m_sumPtRatio_1PCut ;
  /** cut on ratio of sumPt in isolation to core, MP */
  float m_sumPtRatio_MPCut ;
  /** cut on leading track pt */
  float m_ldTrkPt_Cut;

  /** Monitoring: Cut counter */
  int   m_cutCounter;
  /** Monitoring: Ntracks */
  int   m_nTracks;
  /** Monitoring: sumPtRatio */
  float m_sumPtRatio;
  /** Monitoring: ldTrkPt */
  float m_ldTrkPt;

};
#endif
