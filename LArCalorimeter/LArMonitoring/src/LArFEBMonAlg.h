/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARMONITORING_LARFEBMONALG_H
#define LARMONITORING_LARFEBMONALG_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"

#include "LArIdentifier/LArOnlineID.h"
#include "Identifier/HWIdentifier.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "LArRecConditions/LArBadChannelCont.h"
#include "LArRawConditions/LArDSPThresholdsComplete.h"
#include "LArRawEvent/LArFebHeaderContainer.h"
#include "LArRawEvent/LArFebErrorSummary.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

#include "TrigDecisionTool/TrigDecisionTool.h"

#include <string>
#include <vector>
#include <bitset>

#include<mutex>

class TTree;

class LArFEBMonAlg : public AthMonitorAlgorithm {
  
public:
  
  LArFEBMonAlg(const std::string& name, ISvcLocator* pSvcLocator); 
  virtual ~LArFEBMonAlg(){};
  
  virtual StatusCode initialize() override final;
  virtual StatusCode fillHistograms(const EventContext& ctx) const override final;
  StatusCode procHistograms();

private:
  
  SG::ReadCondHandleKey<LArBadFebCont> m_BFKey{this, "MissingFEBKey", "LArBadFeb", "SG key for missing FEBs"};
  SG::ReadCondHandleKey<LArDSPThresholdsComplete> m_run1DSPThresholdsKey
  {this, "Run1DSPThresholdsKey", "", "SG key for DSP thresholds, run1"};
  SG::ReadCondHandleKey<AthenaAttributeList> m_run2DSPThresholdsKey
  {this, "Run2DSPThresholdsKey", "", "SG key for DSP thresholds, run2"};
  SG::ReadHandleKey<LArFebHeaderContainer> m_hdrContKey{this, "LArFebHeaderKey", "LArFebHeader"};
  SG::ReadHandleKey<LArFebErrorSummary> m_lArFebErrorSummaryKey{this, "LArFebErrorSummaryKey", "LArFebErrorSummary"};
  Gaudi::Property<bool> m_ignoreMissingHeaderEMB{this, "IgnoreMissingHeaderEMB", false};
  Gaudi::Property<bool> m_ignoreMissingHeaderPS{this, "IgnoreMissingHeaderPS", false};
  Gaudi::Property<int> m_nFEBnominal{this,"NominalFEBNumber",1524};
  Gaudi::Property<std::vector<std::string> > m_excoscalo{this,"ExcludeInCosmicCalo",{}, "Triggers excluded in CosmicCalo stream"};
  Gaudi::Property<std::vector<std::string> > m_streams{this, "Streams", {}, "Which streams to monitor, if empty, only simple profile per partition (offline case)"};
  Gaudi::Property<std::vector<std::string> > m_partitions {this, "PartitionNames", {} };
  Gaudi::Property<std::vector<std::string> > m_SubDetNames{this, "SubDetNames", {} };

  Gaudi::Property<std::string> m_monGroupName  {this, "MonGroup", "FEBMon", "LArLARFEBMonGroup"};

  /* Histogram grouping (part) */
  std::vector<std::map<std::string,int> > m_histoGroups;

  // Variables, which should fill the TTree, waiting until it will be supported by central monitoring
  //TTree* m_CorruptTree;

  //mutable unsigned int m_eventTime ATLAS_THREAD_SAFE;
  //mutable unsigned int m_eventTime_ns ATLAS_THREAD_SAFE;
  //mutable std::vector<int> m_febInErrorTree ATLAS_THREAD_SAFE;
  //mutable std::vector<int> m_febErrorTypeTree ATLAS_THREAD_SAFE;
  
  // Id helper
  const LArOnlineID* m_onlineHelper;

  bool m_trigok;
  
  typedef std::map<std::pair<unsigned int, unsigned int>, unsigned int > MaskedFebs;
  
  void fillErrorsSummary(unsigned int partitNb_2,int ft,int slot,uint16_t error, bool lar_inerror, std::bitset<13> &rejectionBits, bool &currentFebStatus , bool &eventRejected) const;
  void plotMaskedFEB() const;
  unsigned int returnPartition(int be,int pn,int ft,int sl) const;

  mutable bool m_dspThrDone ATLAS_THREAD_SAFE;
  mutable bool m_maskedDone ATLAS_THREAD_SAFE;
  mutable std::atomic<int>  m_nbOfFebBlocksTotal;
  mutable MaskedFebs m_badFebs ATLAS_THREAD_SAFE;
  mutable std::mutex m_mut ATLAS_THREAD_SAFE;

};

#endif // LARFEBMONALG_H
