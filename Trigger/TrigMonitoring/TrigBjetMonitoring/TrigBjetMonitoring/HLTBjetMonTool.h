/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ***************************************************
//
// NAME:     HLTBjetMonTool.h
// PACKAGE:  Trigger/TrigMonitoring/TrigHLTMonitoring
//
// AUTHOR:   Andrea Coccaro
// EMAIL:    Andrea.Coccaro@cern.ch
// 
// ***************************************************

#ifndef HLTBJETMONTOOL_H
#define HLTBJETMONTOOL_H

#include "TrigHLTMonitoring/IHLTMonTool.h"
#include <string>
#include <vector>


#include "TrigBjetMonitoring/BjetLikelihoodMon.h"
#include "TrigBjetMonitoring/BjetProbabilityMon.h"
#include "TrigBjetMonitoring/TuningLikelihood.h"

#include "TrigTrackJetFinderTool/ITrigTrackJetFinderTool.h"

class BjetLikelihoodMon;
class BjetProbabilityMon;


class HLTBjetMonTool : public IHLTMonTool {
  
 public:

  friend class BjetLikelihoodMon;
  friend class BjetProbabilityMon;

  HLTBjetMonTool(const std::string & type, const std::string & name, const IInterface* parent); 
  ~HLTBjetMonTool();
  
  StatusCode init();

  //  StatusCode book(bool isNewEventsBlock, bool isNewLumiBlock, bool isNewRun); //EN
#ifdef ManagedMonitorToolBase_Uses_API_201401
  StatusCode book();
#else
  StatusCode book(bool newEventsBlock, bool newLumiBlock, bool newRun);  
#endif


  StatusCode fill();

  //  StatusCode proc( bool /*isEndOfEventsBlock*/, bool /*isEndOfLumiBlock*/, bool /*isEndOfRun*/ ){ return StatusCode::SUCCESS; } //EN

  //  /*    Temporarily I take it out
#ifdef ManagedMonitorToolBase_Uses_API_201401
  StatusCode proc();
#else
  StatusCode proc(bool endOfEventsBlock, bool endOfLumiBlock, bool endOfRun);  
#endif
  //  */



 private:

  ToolHandle<ITrigTrackJetFinderTool> m_trackJetFinderTool;

  BjetLikelihoodMon *m_likelihood_EF;
  BjetProbabilityMon *m_probability_EF;

  TuningLikelihood *m_tuningLikelihoodIP1D_EF, *m_tuningLikelihoodIP2D_EF, *m_tuningLikelihoodIP3D_EF;

  const BjetLikelihoodMon *m_constLikelihood_EF;
  const BjetProbabilityMon *m_constProbability_EF;

  float m_xBeamSpot,m_yBeamSpot,m_zBeamSpot;
  float m_sigmaX, m_sigmaY, m_sigmaZ;
  float m_prmVtxEFBj;

  std::vector<std::string> m_taggers, m_TriggerChainBjet, m_TriggerChainMujet,m_TriggerChainMujet_phys ;
  int m_Menu_ppV0;

  bool m_useErrIPParam, m_jetDirection, m_useEtaPhiTrackSel;
  float m_setBeamSpotWidth;
  
  float m_efTrkSelChi2;

  int   m_efTrkSelBLayer, m_efTrkSelPixHits, m_efTrkSelSiHits;
  float m_efTrkSelD0, m_efTrkSelZ0, m_efTrkSelPt;

  std::vector<float> m_listCutAppliedLikelihood;
  std::vector<float> m_listCutAppliedLikelihood_EF;

  std::vector<float> m_listCutAppliedProbability;
  std::vector<float> m_listCutAppliedProbability_EF;

  std::vector<float> m_mon_ef_trk_a0, m_mon_ef_trk_a0_sel, m_mon_ef_trk_Sa0_sel;
  std::vector<float> m_mon_ef_trk_z0, m_mon_ef_trk_z0_sel, m_mon_ef_trk_z0_sel_PV, m_mon_ef_trk_Sz0_sel;
  std::vector<float> m_mon_ef_trk_prob;

  std::vector<int> m_sizeIP1D_EF;
  std::vector<float> m_bIP1D_EF, m_uIP1D_EF;
  std::vector<int> m_sizeIP2D_EF;
  std::vector<float> m_bIP2D_EF, m_uIP2D_EF;
  std::vector<int> m_sizeIP3D_EF;
  std::vector<float> m_bIP3D_EF, m_uIP3D_EF;

  std::vector<float> m_par_EF0;
  std::vector<float> m_par_EF1;

  /** @brief Pointer to TaggerHelper class. */ 
  TaggerHelper* m_taggerHelper;
 
  ToolHandle<Trig::TrigDecisionTool> m_trigDecisionTool;



};

#endif
