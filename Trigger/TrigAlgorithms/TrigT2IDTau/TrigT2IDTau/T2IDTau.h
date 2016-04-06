/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT2IDTAU_T2IDTAU_H
#define TRIGT2IDTAU_T2IDTAU_H
// ********************************************************************
// 
// NAME:     T2IDTau.h
// PACKAGE:  Trigger/TrigAlgorithms/TrigT2IDTau
// 
// AUTHOR:   O.Igonkina  (Nikhef)
// 
// Purpose: Create tracks info object for taus
// ********************************************************************

#include "TrigInterfaces/FexAlgo.h"

namespace HLT {
  class TriggerElement;
}

class TrigVertexCollection;
class VxContainer;
class T2IDTauIPHelper;

class T2IDTau: public HLT::FexAlgo {
  
public:
  /** constructor */
    T2IDTau(const std::string & name, ISvcLocator* pSvcLocator);
  /** destructor */
    ~T2IDTau();

  /** HLT method to execute */
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* inputTE, HLT::TriggerElement* outputTE);
  /** HLT method to initialize */
  HLT::ErrorCode hltInitialize();
  /** HLT method to finalize */
  HLT::ErrorCode hltFinalize();
  
  private:

  /**  value of DR for corfe area */
  double m_dRCore; // RoI used for track reconstruction is sum of base cone and isolation cone. Use this parameter to distinquish between two.
  /** Upper value for DR for isolation, if different from RoI limits */
  double m_dRIso; 
  /** Cut for minimum track Pt */
  float  m_ptMin;
  /** Cut for maximum slow track Pt */
  float  m_ptSlowMax;
  /** Cut on maximum deltaZ0 wrt leadPtTrk */
  float  m_dZ0Max;
  /** if true center isolation area around leading track; otherwise around RoI center */
  bool   m_centerAroundLeadingTrack ;
  /** if true, find the leading track from the inputTE, ie, first step */
  bool   m_secondStep;

  HLT::ErrorCode getPrmVtxCollection(const TrigVertexCollection*&, const HLT::TriggerElement*);
  /** @brief To parametrize errors on track longitudinal impact parameter at LVL2. */
//  float getIP1DErr(float, float);
  /** @brief To parametrize errors on track transverse impact parameter at LVL2. */
//  float getIP2DErr(float, float);

  /** Flag to select tracking algorithm */
  std::string m_TrigTrackingAlgoLabel;
  /** Tracking algorithm ID: 1 SiTrack, 2 IDScan */
//  double      m_algo;

  /**Monitoring : Ntracks */
  int             m_nCoreTracks;
  /**Monitoring : Charge */
  int             m_charge ;
  /**Monitoring : N slow tracks */
  int             m_nSlowTracks ;
  /**Monitoring : N tracks in isolation*/
  int             m_nIsoTracks ;
  /**Monitoring : sumPT_core */
  float           m_sumPtCore;
  /**Monitoring : sumPT_iso */
  float           m_sumPtIso;
  /**Monitoring : sumPT_core/sumPt_isol */
  float           m_sumPtRatio;
  /**Monitoring : leading Pt*/
  float           m_maxPt ;
  /**Monitoring : pt weighted track radius */
  float           m_trkRadius;

  /** option to update RoiDescriptor after execution (important for following trigger chain steps) */
  bool            m_updateRoiDescriptor;
  
  /** IP INFO **/
  std::vector<float> m_mon_trk_alldZ0;
  std::vector<float> m_mon_trk_alloweddZ0;

  /** @brief Pointer to IPHelper class. */
  //T2IDTauIPHelper* m_IPHelper;
  
};





#endif //  TRIGT2IDTAU_T2IDTAU_H
