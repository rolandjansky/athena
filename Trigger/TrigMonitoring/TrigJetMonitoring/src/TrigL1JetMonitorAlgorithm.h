/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETMONITORING_TRIGL1JETMONITORALGORITHM_H
#define TRIGJETMONITORING_TRIGL1JETMONITORALGORITHM_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "xAODTrigger/JetRoIContainer.h"
#include "AsgDataHandles/ReadDecorHandleKey.h"

class TrigL1JetMonitorAlgorithm : public AthMonitorAlgorithm {
 public:
  TrigL1JetMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~TrigL1JetMonitorAlgorithm();
  virtual StatusCode initialize() override;
  virtual StatusCode fillHistograms( const EventContext& ctx ) const override;
  void fillMatchedHistograms(const xAOD::JetRoIContainer & jets, const EventContext& ctx) const ;

  // Name of the L1 jet collection to be monitored
  SG::ReadHandleKey<xAOD::JetRoIContainer> m_l1jetContainerkey;
  // Variables to add matched histograms
  Gaudi::Property<bool> m_isMatched {this, "IsMatched", false, "Plotting response histograms for L1 jets matched to online/offline jets"};
  Gaudi::Property<std::string> m_matchedOfflineJetContainer {this, "MatchedOfflineJets", "", "Name of matched offline jet collection"};
  Gaudi::Property<std::string> m_matchedHLTJetContainer {this, "MatchedHLTJets", "", "Name of matched HLT jet collection"};

  SG::ReadDecorHandleKey<xAOD::JetRoIContainer> m_offmatchedKey{this, "offmatched", "offmatched", "SG key for input matched decoration"};
  SG::ReadDecorHandleKey<xAOD::JetRoIContainer> m_offptdiffKey{this, "offptdiff", "offptdiff", "SG key for input matched ptdiff decoration"};
  SG::ReadDecorHandleKey<xAOD::JetRoIContainer> m_offenergydiffKey{this, "offenergydiff", "offenergydiff", "SG key for input matched energydiff decoration"};
  SG::ReadDecorHandleKey<xAOD::JetRoIContainer> m_offmassdiffKey{this, "offmassdiff", "offmassdiff", "SG key for input matched massdiff decoration"};
  SG::ReadDecorHandleKey<xAOD::JetRoIContainer> m_offptrespKey{this, "offptresp", "offptresp", "SG key for input matched ptresp decoration"};
  SG::ReadDecorHandleKey<xAOD::JetRoIContainer> m_offenergyrespKey{this, "offenergyresp", "offenergyresp", "SG key for input matched energyresp decoration"};
  SG::ReadDecorHandleKey<xAOD::JetRoIContainer> m_offmassrespKey{this, "offmassresp", "offmassresp", "SG key for input matched massresp decoration"};
  SG::ReadDecorHandleKey<xAOD::JetRoIContainer> m_offptrefKey{this, "offptref", "offptref", "SG key for input matched ptref decoration"};
  SG::ReadDecorHandleKey<xAOD::JetRoIContainer> m_offetarefKey{this, "offetaref", "offetaref", "SG key for input matched etaref decoration"};

  SG::ReadDecorHandleKey<xAOD::JetRoIContainer> m_hltmatchedKey{this, "hltmatched", "hltmatched", "SG key for input matched decoration"};
  SG::ReadDecorHandleKey<xAOD::JetRoIContainer> m_hltptdiffKey{this, "hltptdiff", "hltptdiff", "SG key for input matched ptdiff decoration"};
  SG::ReadDecorHandleKey<xAOD::JetRoIContainer> m_hltenergydiffKey{this, "hltenergydiff", "hltenergydiff", "SG key for input matched energydiff decoration"};
  SG::ReadDecorHandleKey<xAOD::JetRoIContainer> m_hltmassdiffKey{this, "hltmassdiff", "hltmassdiff", "SG key for input matched massdiff decoration"};
  SG::ReadDecorHandleKey<xAOD::JetRoIContainer> m_hltptrespKey{this, "hltptresp", "hltptresp", "SG key for input matched ptresp decoration"};
  SG::ReadDecorHandleKey<xAOD::JetRoIContainer> m_hltenergyrespKey{this, "hltenergyresp", "hltenergyresp", "SG key for input matched energyresp decoration"};
  SG::ReadDecorHandleKey<xAOD::JetRoIContainer> m_hltmassrespKey{this, "hltmassresp", "hltmassresp", "SG key for input matched massresp decoration"};
  SG::ReadDecorHandleKey<xAOD::JetRoIContainer> m_hltptrefKey{this, "hltptref", "hltptref", "SG key for input matched ptref decoration"};
  SG::ReadDecorHandleKey<xAOD::JetRoIContainer> m_hltetarefKey{this, "hltetaref", "hltetaref", "SG key for input matched etaref decoration"};

};
#endif
