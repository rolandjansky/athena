/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 *
 * NAME:     EFTauDiKaonHypo.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigTauHypo
 *
 * AUTHOR:   D. Zanzi based on EFTauMVHypo
 * CREATED:  Aug. 17, 2015
 *
  *********************************************************************/
#ifndef TRIGEFTAUDIKAONHYPO_H
#define TRIGEFTAUDIKAONHYPO_H

#include "TrigInterfaces/HypoAlgo.h"
#include "TGraph.h"

class StoreGateSvc;
namespace HLT {
  class TriggerElement;
}

class EFTauDiKaonHypo : public HLT::HypoAlgo {

 public:

  /** constructor of EF tauRec hypo algo */
  EFTauDiKaonHypo(const std::string& name, ISvcLocator* pSvcLocator);
  /** destructor */
  ~EFTauDiKaonHypo();

  /** HLT method for initialize */
  HLT::ErrorCode hltInitialize();

  /** HLT method for finalize */
  HLT::ErrorCode hltFinalize();

  /** HLT method for execute Hypo on  a TE.
      input is TE, output is pass=True or False */
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);

 private:

  /** Cuts to be applied */
  float m_massTrkSysMin;
  float m_massTrkSysMax;
  float m_massTrkSysKaonMin;
  float m_massTrkSysKaonMax;
  float m_massTrkSysKaonPiMin;
  float m_massTrkSysKaonPiMax;
  float m_targetMassTrkSysKaonPi;
  float m_EtCalibMin;
  float m_leadTrkPtMin;
  int m_nTrackMax;
  int m_nTrackMin;
  int m_nWideTrackMax;
  float m_EMPOverTrkSysPMax;
  float m_dRmaxMax;
  float m_etOverPtLeadTrkMax;
  float m_etOverPtLeadTrkMin;

  /** variables used for cuts in hypo algorithm */   
  float m_massTrkSys;
  float m_massTrkSysKaon;
  float m_massTrkSysKaonPi;
  float m_leadTrkPt;
  float m_EtCalib;
  int m_nTrack;
  int m_nWideTrack;
  float m_EMPOverTrkSysP;
  float m_dRmax;
  float m_etOverPtLeadTrk;

  /** for monitoring */
  int  m_cutCounter;
  float m_massTrkSysAccepted;
  float m_massTrkSysKaonAccepted;
  float m_massTrkSysKaonPiAccepted;
  float m_leadTrkPtAccepted;
  float m_ptAccepted;
  float m_nTrackAccepted;
  float m_nWideTrackAccepted;
  float m_dRAccepted;
  float m_etOverPtLeadTrkAccepted;
  float m_EMOverTrkSysPAccepted;  

};
#endif

