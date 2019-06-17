/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 *
 * NAME:     EFTauInvHypo.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigTauHypo
 *
 * AUTHOR:   R. Soluk based on TrigEFEgammaHypo
 * CREATED:  May 12, 2006
 *
 * co-author : Stefania Xella
 *********************************************************************/
#ifndef TRIGEFTAUINVHYPO_H
#define TRIGEFTAUINVHYPO_H

#include "TrigInterfaces/HypoAlgo.h"

class StoreGateSvc;
namespace HLT {
  class TriggerElement;
}

class EFTauInvHypo : public HLT::HypoAlgo {

 public:

  /** constructor of EF tauRec hypo algo */
  EFTauInvHypo(const std::string& name, ISvcLocator* pSvcLocator);
  /** destructor */
  ~EFTauInvHypo();

  /** HLT method for initialize */
  HLT::ErrorCode hltInitialize();

  /** HLT method for finalize */
  HLT::ErrorCode hltFinalize();

  /** HLT method for execute Hypo on  a TE.
      input is TE, output is pass=True or False */
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);

 private:

  /** Cuts to be applied */
  
  //** require exactly 1 or 3 tracks, nmin/nmax ingored if this cut set to true*/
  bool    m_numTrkOnly1or3;
  
  /** minimum number of tracks for tau object (inclusive) */
  int     m_numTrackMin;

  /** maximum number of tracks for tau object (inclusive) */
  int     m_numTrackMax;

  /** maximum EM radius 1 prong*/
  double  m_EMRadiusMax_1P;

  /** maximum EM radius multi prong */
  double  m_EMRadiusMax_MP;

  /** minimum calibrated full Et */
  double  m_EtCalibMin;

  /** maximum DR of trk wrt calo center  1 prong*/
  double  m_DrTrkAvgMax_1P;

  /** maximum DR of trk wrt calo center 3 prong */
  double  m_DrTrkAvgMax_MP;

  /** maximum Et over pt lead 1 prong */
  double m_EtovPtLeadMax_1P;

  /** maximum Et over pt lead 3 prong */
  double m_EtovPtLeadMax_MP;

  /** formula for emradius and trkradius is */
  /**  emrad_cut = a0 + a1*x + a2*x^-1 */

  /** emradius parameters */

  double m_emrad_OP_a0;
  double m_emrad_OP_a1;
  double m_emrad_OP_a2;
  double m_emrad_MP_a0;
  double m_emrad_MP_a1;
  double m_emrad_MP_a2;

  /** trkradius parameters */

  double m_trkrad_OP_a0;
  double m_trkrad_OP_a1;
  double m_trkrad_OP_a2;
  double m_trkrad_MP_a0;
  double m_trkrad_MP_a1;
  double m_trkrad_MP_a2;

  /** variables used for cuts in hypo algorithm */   
  int    m_numTrack;
  double m_EMRadius;
  //double m_Ncand;
  double m_DrTrkAvg;
  double m_EtovPtLead;

  /** for monitoring */
  int  m_cutCounter;


};
#endif

