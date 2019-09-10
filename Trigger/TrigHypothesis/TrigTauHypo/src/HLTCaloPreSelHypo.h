/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HLTCALOPRESELHYPO_H
#define HLTCALOPRESELHYPO_H

/********************************************************************
 *
 * NAME:     HLTCaloPreSelHypo.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigTauHypo
 *
 * AUTHORS:   Marcelo Vogel (mavogel@cern.ch)
 *            Soshi Tsuno
 *            P.O. DeViveiros
 * CREATED:  July 18, 2013
 *           April 30, 2014 (simplify & adapt to xAOD)
 *
 * DESCRIPTION:
 *
 * Implementation of calorimeter based pre-selection on
 * tau candidates starting from L1 seeds
 *
 *********************************************************************/

#include "TrigInterfaces/HypoAlgo.h"

class StoreGateSvc;

namespace HLT {
  class TriggerElement;
}

class HLTCaloPreSelHypo : public HLT::HypoAlgo
{

 public:

  /** constructor of EF tauRec hypo algo */
  HLTCaloPreSelHypo(const std::string& name, ISvcLocator* pSvcLocator);
  /** destructor */
  ~HLTCaloPreSelHypo();

  /** HLT method for initialize */
  HLT::ErrorCode hltInitialize();

  /** HLT method for finalize */
  HLT::ErrorCode hltFinalize();

  /** HLT method for execute Hypo on  a TE.
      input is TE, output is pass=True or False */
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);

 private:

  /** Configurables */
  
  // Cluster-based pT cut
  float m_lowerPtCut;
  float m_clusterCone;

  // Cell-based shower variables
  bool m_cellCut;
  float m_cellCore;
  float m_cellOuter;
  float m_coreFractionCut;
  float m_hadRadiusCut;


  int  m_cutCounter;
  //Monitoring: pT of tau candidate
  float m_seedPt;
  //Monitoring: fractional core eT
  float m_fCore;
  //Monitoring: average eT-weighted cell radius
  float m_hadRadius;
  //Monitoring: DeltaR between tau barycenter and RoI
  float m_dRBaryCenterRoI;

};
#endif

