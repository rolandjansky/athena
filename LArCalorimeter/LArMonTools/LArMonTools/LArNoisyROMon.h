/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @class LArNoisyROMon
 * @author Laurent Duflot <duflot at lal.in2p3.fr>
 *
 */

#ifndef LArNoisyROMon_h
#define LArNoisyROMon_h

#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TTree.h"


#include <string>

class TH1I_LW;
class TH1F_LW;
class TH2F_LW;
class LWHist2D;
class LWHist1D;
class LArOnlineID;
class LArOnlineIDStrHelper;

class LArNoisyROMon: public ManagedMonitorToolBase
{
public:
  LArNoisyROMon(const std::string& type, 
		const std::string& name, 
		const IInterface* parent);
  
  virtual ~LArNoisyROMon();
  
  StatusCode initialize();
  StatusCode finalize();
  StatusCode bookHistograms();
  StatusCode fillHistograms();
  StatusCode checkHists(bool fromFinalize);
  StatusCode procHistograms();
  
private:
  
  struct NoiseTime
  {
    unsigned int time;
    unsigned int time_ns;
    UChar_t algo;
  };
  NoiseTime m_NoiseTime;
  TTree* m_NoiseTimeTree;
  
  struct CandidateMNB
  {
    unsigned int candidate_MNB_time;
    unsigned int candidate_MNB_time_ns;
    int n_candidate_MNBTight_FEB;
    int n_candidate_MNBLoose_FEB;
    std::vector<int>* v_candidate_MNBTightFEB;
    std::vector<int>* v_candidate_MNBLooseFEB;
  };
  CandidateMNB m_CandidateMNB;
  TTree* m_CandidateMNBTree;
  
  struct partitionHistos
  {
    partitionHistos() {
      h_NoisyFEBFracPerEvt = NULL;
      h_MNBTightFEBFracPerEvt = NULL;
      h_MNBLooseFEBFracPerEvt = NULL;
      h_CandidateMNBTightFEBFracPerEvt = NULL;
      h_CandidateMNBLooseFEBFracPerEvt = NULL;
      h_NoisyEventFrac = NULL;
      h_NoisyEventTimeVetoFrac = NULL;
      h_MNBTightEventFrac = NULL;
      h_MNBTightEventTimeVetoFrac = NULL;
      h_MNBLooseEventFrac = NULL;
      h_MNBLooseEventTimeVetoFrac = NULL;
      h_CandidateMNBTightEventFrac = NULL;
      h_CandidateMNBTightEventTimeVetoFrac = NULL;
      h_CandidateMNBLooseEventFrac = NULL;
      h_CandidateMNBLooseEventTimeVetoFrac = NULL;
      h_NoisyWEventFrac = NULL;
      h_NoisyWEventTimeVetoFrac = NULL;
      h_NoisyEventTrigger = NULL;
      h_NoisyFEBPerEvt = NULL;
      h_MNBTightFEBPerEvt = NULL;
      h_MNBLooseFEBPerEvt = NULL;
      h_CandidateMNBTightFEBPerEvt = NULL;
      h_CandidateMNBLooseFEBPerEvt = NULL;
      h_NoisyEvent = NULL;
      h_NoisyEventTimeVeto = NULL;
      h_MNBTightEvent = NULL;
      h_MNBLooseEvent = NULL;
      h_MNBTightEventTimeVeto = NULL;
      h_MNBLooseEventTimeVeto = NULL;
      h_CandidateMNBTightEvent = NULL;
      h_CandidateMNBLooseEvent = NULL;
      h_CandidateMNBTightEventTimeVeto = NULL;
      h_CandidateMNBLooseEventTimeVeto = NULL;
      h_NoisyWEvent = NULL;
      h_NoisyWEventTimeVeto = NULL;
      h_NoisyEventTriggerL1 = NULL;
    }      
    TH2F_LW* h_NoisyFEBFracPerEvt;
    TH2F_LW* h_MNBTightFEBFracPerEvt;
    TH2F_LW* h_MNBLooseFEBFracPerEvt;
    TH2F_LW* h_CandidateMNBTightFEBFracPerEvt;
    TH2F_LW* h_CandidateMNBLooseFEBFracPerEvt;
    TH1F_LW* h_NoisyEventFrac;
    TH1F_LW* h_MNBTightEventFrac;
    TH1F_LW* h_MNBLooseEventFrac;
    TH1F_LW* h_CandidateMNBTightEventFrac;
    TH1F_LW* h_CandidateMNBLooseEventFrac;
    TH1F_LW* h_NoisyWEventFrac;
    TH1F_LW* h_NoisyEventTimeVetoFrac;
    TH1F_LW* h_MNBTightEventTimeVetoFrac;
    TH1F_LW* h_MNBLooseEventTimeVetoFrac;
    TH1F_LW* h_CandidateMNBTightEventTimeVetoFrac;
    TH1F_LW* h_CandidateMNBLooseEventTimeVetoFrac;
    TH1F_LW* h_NoisyWEventTimeVetoFrac;
    TH1I_LW* h_NoisyEventTrigger;
    TH1I_LW* h_NoisyEventTriggerL1;
    
    // this are temporary histogram that never gets normalised
    // in 'online' mode they are used to produce regularly the 'Frac'
    // histograms
    TH2F_LW* h_NoisyFEBPerEvt;
    TH2F_LW* h_MNBTightFEBPerEvt;
    TH2F_LW* h_MNBLooseFEBPerEvt;
    TH2F_LW* h_CandidateMNBTightFEBPerEvt;
    TH2F_LW* h_CandidateMNBLooseFEBPerEvt;
    TH1I_LW* h_NoisyEvent;
    TH1I_LW* h_MNBTightEvent;
    TH1I_LW* h_MNBLooseEvent;
    TH1I_LW* h_CandidateMNBTightEvent;
    TH1I_LW* h_CandidateMNBLooseEvent;
    TH1I_LW* h_NoisyWEvent;
    TH1I_LW* h_NoisyEventTimeVeto;
    TH1I_LW* h_MNBTightEventTimeVeto;
    TH1I_LW* h_MNBLooseEventTimeVeto;
    TH1I_LW* h_CandidateMNBTightEventTimeVeto;
    TH1I_LW* h_CandidateMNBLooseEventTimeVeto;
    TH1I_LW* h_NoisyWEventTimeVeto;
  };
  TH1I_LW* h_NoisyFEB;
  TH1F_LW* h_NoisyEventFrac;
  TH1F_LW* h_NoisyEventTimeVetoFrac;
  TH1I_LW* h_MNBTightFEB;
  TH1F_LW* h_MNBTightEventFrac;
  TH1F_LW* h_MNBTightEventTimeVetoFrac;
  TH1I_LW* h_MNBLooseFEB;
  TH1F_LW* h_MNBLooseEventFrac;
  TH1F_LW* h_MNBLooseEventTimeVetoFrac;
  TH1I_LW* h_CandidateMNBTightFEB;
  TH1F_LW* h_CandidateMNBTightEventFrac;
  TH1F_LW* h_CandidateMNBTightEventTimeVetoFrac;
  TH1I_LW* h_CandidateMNBLooseFEB;
  TH1F_LW* h_CandidateMNBLooseEventFrac;
  TH1F_LW* h_CandidateMNBLooseEventTimeVetoFrac;
  TH1F_LW* h_SaturatedTightFrac;
  TH1I_LW* h_NoisyEvent;
  TH1I_LW* h_NoisyEventTimeVeto;
  TH1I_LW* h_MNBTightEvent;
  TH1I_LW* h_MNBTightEventTimeVeto;
  TH1I_LW* h_MNBLooseEvent;
  TH1I_LW* h_MNBLooseEventTimeVeto;
  TH1I_LW* h_CandidateMNBTightEvent;
  TH1I_LW* h_CandidateMNBTightEventTimeVeto;
  TH1I_LW* h_CandidateMNBLooseEvent;
  TH1I_LW* h_CandidateMNBLooseEventTimeVeto;
  TH1I_LW* h_SaturatedTight;
  TH1I_LW* h_SaturatedTightTimeVeto;
  TH1F_LW* h_SaturatedTightTimeVetoFrac;
  TH1I_LW* h_LBN;

  
  void bookPartitionHistos(partitionHistos&, const std::string& name, MonGroup& group, MonGroup& groupfrac, MonGroup& groupfracbin);
  void CopyHisto(LWHist2D* from,LWHist2D* to);
  void CopyHisto(LWHist1D* from,LWHist1D* to);
  
  // divide num by denom and store in to, assuming num is a subset of denom
  void Divide(LWHist1D* to,LWHist1D* num,LWHist1D* denom);
  
  // fill histogram of triggers
  void fillTriggerHisto(partitionHistos& partition, uint8_t triggerbits, uint8_t L1triggerbits);

  bool m_IsOnline;
  bool m_doTrigger;
  unsigned int m_eventCounter;
  partitionHistos m_BarrelA, m_BarrelC, m_EMECA, m_EMECC;
  const LArOnlineID* m_LArOnlineIDHelper;
  LArOnlineIDStrHelper* m_strHelper;
  ToolHandle<Trig::TrigDecisionTool> m_trigDec;

  
  std::string m_NoisyFEBDefStr;
  unsigned int m_BadFEBCut;

  std::vector<std::string> m_EF_NoiseBurst_Triggers;
  std::vector<std::string> m_L1_NoiseBurst_Triggers;
};


#endif
