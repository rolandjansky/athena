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
    TH2F_LW* h_NoisyFEBFracPerEvt=nullptr;
    TH2F_LW* h_MNBTightFEBFracPerEvt=nullptr;
    TH2F_LW* h_MNBLooseFEBFracPerEvt=nullptr;
    TH2F_LW* h_CandidateMNBTightFEBFracPerEvt=nullptr;
    TH2F_LW* h_CandidateMNBLooseFEBFracPerEvt=nullptr;
    TH1F_LW* h_NoisyEventFrac=nullptr;
    TH1F_LW* h_SaturatedNoisyEventFrac=nullptr;
    TH1F_LW* h_MNBTightEventFrac=nullptr;
    TH1F_LW* h_MNBLooseEventFrac=nullptr;
    TH1F_LW* h_NoisyWEventFrac=nullptr;
    TH1F_LW* h_NoisyEventTimeVetoFrac=nullptr;
    TH1F_LW* h_SaturatedNoisyEventTimeVetoFrac=nullptr;
    TH1F_LW* h_MNBTightEventTimeVetoFrac=nullptr;
    TH1F_LW* h_MNBLooseEventTimeVetoFrac=nullptr;
    TH1F_LW* h_NoisyWEventTimeVetoFrac=nullptr;
    TH1I_LW* h_NoisyEventTrigger=nullptr;
    TH1I_LW* h_NoisyEventTriggerL1=nullptr;
    
    // this are temporary histogram that never gets normalised
    // in 'online' mode they are used to produce regularly the 'Frac'
    // histograms
    TH2F_LW* h_NoisyFEBPerEvt=nullptr;
    TH2F_LW* h_MNBTightFEBPerEvt=nullptr;
    TH2F_LW* h_MNBLooseFEBPerEvt=nullptr;
    TH2F_LW* h_CandidateMNBTightFEBPerEvt=nullptr;
    TH2F_LW* h_CandidateMNBLooseFEBPerEvt=nullptr;
    TH1I_LW* h_NoisyEvent=nullptr;
    TH1I_LW* h_SaturatedNoisyEvent=nullptr;
    TH1I_LW* h_MNBTightEvent=nullptr;
    TH1I_LW* h_MNBLooseEvent=nullptr;
    TH1I_LW* h_NoisyWEvent=nullptr;
    TH1I_LW* h_NoisyEventTimeVeto=nullptr;
    TH1I_LW* h_SaturatedNoisyEventTimeVeto=nullptr;
    TH1I_LW* h_MNBTightEventTimeVeto=nullptr;
    TH1I_LW* h_MNBLooseEventTimeVeto=nullptr;
    TH1I_LW* h_NoisyWEventTimeVeto=nullptr;
  };

  TH1I_LW* m_h_NoisyFEB=nullptr;
  TH1F_LW* m_h_NoisyEventFrac=nullptr;
  TH1F_LW* m_h_NoisyEventTimeVetoFrac=nullptr;
  TH1I_LW* m_h_MNBTightFEB=nullptr;
  TH1F_LW* m_h_MNBTightEventFrac=nullptr;
  TH1F_LW* m_h_MNBTightEventTimeVetoFrac=nullptr;
  TH1I_LW* m_h_MNBLooseFEB=nullptr;
  TH1F_LW* m_h_MNBLooseEventFrac=nullptr;
  TH1F_LW* m_h_MNBLooseEventTimeVetoFrac=nullptr;
  TH1I_LW* m_h_CandidateMNBTightFEB=nullptr;
  TH1I_LW* m_h_CandidateMNBLooseFEB=nullptr;
  TH1F_LW* m_h_SaturatedTightFrac=nullptr;
  TH1I_LW* m_h_NoisyEvent=nullptr;
  TH1I_LW* m_h_NoisyEventTimeVeto=nullptr;
  TH1I_LW* m_h_MNBTightEvent=nullptr;
  TH1I_LW* m_h_MNBTightEventTimeVeto=nullptr;
  TH1I_LW* m_h_MNBLooseEvent=nullptr;
  TH1I_LW* m_h_MNBLooseEventTimeVeto=nullptr;
  TH1I_LW* m_h_SaturatedTight=nullptr;
  TH1I_LW* m_h_SaturatedTightTimeVeto=nullptr;
  TH1F_LW* m_h_SaturatedTightTimeVetoFrac=nullptr;
  TH1I_LW* m_h_LBN=nullptr;

  
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
  unsigned int m_lumi_blocks;
  bool m_doHisto;
  std::string m_inputKey;
};


#endif
