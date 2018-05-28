/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 *
 * NAME:     EFTauMVHypo.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigTauHypo
 *
 * AUTHOR:   m. Morgenstern based on EFTauInvHypo
 * CREATED:  Jun 20, 2011
 *
  *********************************************************************/
#ifndef TRIGEFTAUMVHYPO_H
#define TRIGEFTAUMVHYPO_H

#include "TrigInterfaces/HypoAlgo.h"
#include "TGraph.h"

class StoreGateSvc;
namespace HLT {
  class TriggerElement;
}

class EFTauMVHypo : public HLT::HypoAlgo {

 public:

  /** constructor of EF tauRec hypo algo */
  EFTauMVHypo(const std::string& name, ISvcLocator* pSvcLocator);
  /** destructor */
  ~EFTauMVHypo();

  /** HLT method for initialize */
  HLT::ErrorCode hltInitialize();

  /** HLT method for finalize */
  HLT::ErrorCode hltFinalize();

  /** HLT method for execute Hypo on  a TE.
      input is TE, output is pass=True or False */
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);

 private:

  /** Cuts to be applied */

  /** min numTrack cut **/
  int m_numTrackMin;
  int m_numTrackMax;
  int m_numWideTrackMax;

  double m_EtCalibMin;

  int m_level;
  int m_method;

  bool m_highpt;
  double m_highpttrkthr;
  double m_highptidthr;
  double m_highptjetthr;
  bool   m_applyIDon0p;

  /** variables used for cuts in hypo algorithm */   
  int m_numTrack;
  int m_numWideTrack;
  
  double m_LLHScore;
  double m_BDTScore;

  /** for monitoring */
  int  m_cutCounter;
  double m_mon_ptAccepted;
  int m_mon_nTrackAccepted;
  int m_mon_nWideTrackAccepted;
  int m_inputTaus;

  std::string m_cut_level;
  std::map<std::string,TGraph> m_cuts;
  TGraph *m_OneProngGraph,*m_MultiProngGraph;
};
#endif

