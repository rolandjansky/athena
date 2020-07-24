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

class LArNoisyROMonAlg final: public AthMonitorAlgorithm
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
  Gaudi::Property<bool> m_doTrigger {this, "doTrigger", false};
  Gaudi::Property<bool> m_storeLooseMNBFEBs {this, "storeLooseMNBFEBs", false};

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
  
  // fill histogram of triggers
  void fillTriggerHisto(size_t partition, unsigned long triggerbits, unsigned long L1triggerbits) const;

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
