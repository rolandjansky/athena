/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @class LArNoisyROMonAlg
 * @author Pavol Sgtrizenec
 * based on LArNoisyROMon toool
 */

#ifndef LARMONITORING_LARNOISYROMONALG_H
#define LARMONITORING_LARNOISYROMONALG_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "TrigDecisionTool/TrigDecisionTool.h"

#include "Identifier/HWIdentifier.h"
#include "LArIdentifier/LArOnlineID.h"

#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadCondHandleKey.h"

#include "LArRecConditions/LArBadChannelCont.h"
#include "LArRecEvent/LArNoisyROSummary.h"

#include <string>
#include <array>

class LArNoisyROMonAlg: public AthMonitorAlgorithm
{
public:
  LArNoisyROMonAlg(const std::string& name, ISvcLocator* pSvcLocator);
  
  virtual ~LArNoisyROMonAlg() override final {};
  
  virtual StatusCode initialize() override final;
  virtual StatusCode fillHistograms(const EventContext& ctx) const override final;

  size_t partitionNumber(const HWIdentifier) const;
  
private:
  
  const LArOnlineID* m_LArOnlineIDHelper;

  Gaudi::Property<std::string> m_MonGroupName  {this, "NoisyROGroupName", "LArNoisyROMonGroup"};
  Gaudi::Property<std::vector<std::string> > m_SubDetNames{this, "SubDetNames", {} };
  Gaudi::Property<std::vector<std::string> > m_partitions {this, "PartitionNames", {} };
  Gaudi::Property<bool> m_IsOnline {this, "IsOnline", false};
  Gaudi::Property<bool> m_doTrigger {this, "doTrigger", true};
  Gaudi::Property<bool> m_storeLooseMNBFEBs {this, "storeLooseMNBFEBs", false};
  Gaudi::Property<std::string> m_MNBTightFEBDefStr {this, "MNBTightFEBDefStr", "(unknown)"};
  Gaudi::Property<std::string> m_MNBTight_PsVetoFEBDefStr {this, "MNBTight_PsVetoFEBDefStr", "(unknown)"};
  Gaudi::Property<std::string> m_MNBLooseFEBDefStr {this, "MNBLooseFEBDefStr", "(unknown)"};
  Gaudi::Property<unsigned int> m_BadFEBCut {this, "BadFEBCut", 999999};

  Gaudi::Property<std::vector<std::string> > m_EF_NoiseBurst_Triggers {this, "EFNoiseBurstTriggers", {} };
  Gaudi::Property<std::vector<std::string> > m_L1_NoiseBurst_Triggers {this, "L1NoiseBurstTriggers", {} };

  Gaudi::Property<bool> m_doHisto {this, "doHisto", true};

  SG::ReadHandleKey<LArNoisyROSummary> m_inputKey {this, "inputKey", "LArNoisyROSummary"};

  SG::ReadCondHandleKey<LArBadFebCont> m_badFebKey{this,"KonwnBadFebKey","LArKnownBadFEBs","Key of known Bad-Feb object"};
  SG::ReadCondHandleKey<LArBadFebCont> m_MNBFebKey{this,"KonwnMNBFebKey","LArKnownMNBFEBs","Key of known MNB-Feb object"};

  struct CandidateMNBStruct
  {
    unsigned int candidate_MNB_time;
    unsigned int candidate_MNB_time_ns;
    int n_candidate_MNBTight_FEB;
    int n_candidate_MNBTight_PsVeto_FEB;
    int n_candidate_MNBLoose_FEB;
    std::vector<int>* v_candidate_MNBTightFEB;
    std::vector<int>* v_candidate_MNBTight_PsVetoFEB;
    std::vector<int>* v_candidate_MNBLooseFEB;
  };
  
  /* Histogram grouping (part) */
  std::vector<std::map<std::string,int> > m_histoGroups;

  /*
  struct partitionHistos
  {      
    TH2F_LW* h_NoisyFEBFracPerEvt=nullptr;
    TH2F_LW* h_MNBTightFEBFracPerEvt=nullptr;
    TH2F_LW* h_MNBTight_PsVetoFEBFracPerEvt=nullptr;
    TH2F_LW* h_MNBLooseFEBFracPerEvt=nullptr;
    TH2F_LW* h_CandidateMNBTightFEBFracPerEvt=nullptr;
    TH2F_LW* h_CandidateMNBTight_PsVetoFEBFracPerEvt=nullptr;
    TH2F_LW* h_CandidateMNBLooseFEBFracPerEvt=nullptr;
    TH1F_LW* h_NoisyEventFrac=nullptr;
    TH1F_LW* h_SaturatedNoisyEventFrac=nullptr;
    TH1F_LW* h_MNBTightEventFrac=nullptr;
    TH1F_LW* h_MNBTight_PsVetoEventFrac=nullptr;
    TH1F_LW* h_MNBLooseEventFrac=nullptr;
    //    TH1F_LW* h_NoisyWEventFrac=nullptr;
    TH1F_LW* h_NoisyEventTimeVetoFrac=nullptr;
    TH1F_LW* h_SaturatedNoisyEventTimeVetoFrac=nullptr;
    TH1F_LW* h_MNBTightEventTimeVetoFrac=nullptr;
    TH1F_LW* h_MNBTight_PsVetoEventTimeVetoFrac=nullptr;
    TH1F_LW* h_MNBLooseEventTimeVetoFrac=nullptr;
    //    TH1F_LW* h_NoisyWEventTimeVetoFrac=nullptr;
    TH2I_LW* h_MNBKnownFEB=nullptr;
    TH1I_LW* h_NoisyEventTrigger=nullptr;
    TH1I_LW* h_NoisyEventTriggerL1=nullptr;
    
    // this are temporary histogram that never gets normalised
    // in 'online' mode they are used to produce regularly the 'Frac'
    // histograms
    TH2F_LW* h_NoisyFEBPerEvt=nullptr;
    TH2F_LW* h_MNBTightFEBPerEvt=nullptr;
    TH2F_LW* h_MNBTight_PsVetoFEBPerEvt=nullptr;
    TH2F_LW* h_MNBLooseFEBPerEvt=nullptr;
    TH2F_LW* h_CandidateMNBTightFEBPerEvt=nullptr;
    TH2F_LW* h_CandidateMNBLooseFEBPerEvt=nullptr;
    TH1I_LW* h_NoisyEvent=nullptr;
    TH1I_LW* h_SaturatedNoisyEvent=nullptr;
    TH1I_LW* h_MNBTightEvent=nullptr;
    TH1I_LW* h_MNBTight_PsVetoEvent=nullptr;
    TH2F_LW* h_CandidateMNBTight_PsVetoFEBPerEvt=nullptr;
    TH1I_LW* h_MNBLooseEvent=nullptr;
    //    TH1I_LW* h_NoisyWEvent=nullptr;
    TH1I_LW* h_NoisyEventTimeVeto=nullptr;
    TH1I_LW* h_SaturatedNoisyEventTimeVeto=nullptr;
    TH1I_LW* h_MNBTightEventTimeVeto=nullptr;
    TH1I_LW* h_MNBTight_PsVetoEventTimeVeto=nullptr;
    TH1I_LW* h_MNBLooseEventTimeVeto=nullptr;
    //TH1I_LW* h_NoisyWEventTimeVeto=nullptr;
  };

  */
  
  // fill histogram of triggers
  //void fillTriggerHisto(partitionHistos& partition, uint8_t triggerbits, uint8_t L1triggerbits);

  //  partitionHistos m_BarrelA, m_BarrelC, m_EMECA, m_EMECC;
  //std::vector<partitionHistos> m_partHistos;


  //ToolHandle<Trig::TrigDecisionTool> m_trigDec;

  mutable bool m_knownFilled ATLAS_THREAD_SAFE;
  mutable std::mutex m_lock ATLAS_THREAD_SAFE;
  
};

inline size_t LArNoisyROMonAlg::partitionNumber(const HWIdentifier hwid) const {

  int pn=m_LArOnlineIDHelper->pos_neg(hwid);
  if (m_LArOnlineIDHelper->isEMECchannel(hwid)) {
    if (pn) 
      return 3; //positive EMECA side
    else
      return 2; //negative EMECC side
  }
  if (m_LArOnlineIDHelper->isEMBchannel(hwid)) {
    if (pn) 
      return 1; //positive EMBA side
    else
      return 0; //negative EMBC side
  }

  return 4;//Anything else
}



#endif
