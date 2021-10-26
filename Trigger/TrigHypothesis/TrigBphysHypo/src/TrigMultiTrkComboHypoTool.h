/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigBphysHypo/TrigMultiTrkComboHypoTool.h
 **
 **   Description: multi-track hypothesis tool
 **
 **   Author: Heather Russell
 **
 **************************************************************************/

#ifndef TRIG_TrigMultiTrkComboHypoTool_H
#define TRIG_TrigMultiTrkComboHypoTool_H

#include <string>
#include <vector>
#include <cmath>

#include "DecisionHandling/ComboHypoToolBase.h"

#include "xAODTrigBphys/TrigBphys.h"
#include "xAODTrigBphys/TrigBphysContainer.h"

#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"

#include "AthenaMonitoringKernel/Monitored.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"


class TrigMultiTrkComboHypoTool: public ComboHypoToolBase {
 public:
  TrigMultiTrkComboHypoTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual StatusCode initialize() override;
  virtual StatusCode decideOnSingleObject(TrigCompositeUtils::Decision*, const std::vector<const TrigCompositeUtils::DecisionIDContainer*>&) const override;

 private:
  bool passed(const xAOD::TrigBphys*) const;
  virtual bool executeAlg(const std::vector<Combo::LegDecision>&) const override;
  bool checkPreviousDecisionIDs(const std::vector<const TrigCompositeUtils::DecisionIDContainer*>&) const;
  int totalCharge(const xAOD::TrigBphys*) const;
  bool isInMassRange(double mass) const;
  bool passedDeltaRcut(const xAOD::TrigBphys*) const;
  inline bool passedChi2Cut(int chi2) const { return (m_chi2 < 0. || chi2 < m_chi2); }
  inline bool passedChargeCut(int charge) const { return (m_totalCharge < 0 || std::abs(charge) == m_totalCharge); }

  Gaudi::Property<unsigned int> m_nTrk {this, "nTrk", 2, "number of tracks in the vertex"};
  Gaudi::Property<int> m_totalCharge {this, "totalCharge", 0, "magnitude of the total charge to accept, negative is none" };
  Gaudi::Property<std::pair<double, double>> m_massRange {this, "massRange", {-99., -9.}, "range for the fitted mass, no selection applied if negative"};
  Gaudi::Property<float> m_chi2 {this, "chi2", -99. , "Chi2 cut for vertex (0 < chi2 < cut), no selection applied if negative" };
  Gaudi::Property<bool> m_acceptAll {this, "AcceptAll", false, "if AcceptAll flag is set to true, no selection will be applied for xAOD::TrigBphys object"};
  Gaudi::Property<float> m_LxyCut {this, "LxyCut", -999., "Applies an Lxy Cut if set > -999"};
  Gaudi::Property<float> m_deltaRMax {this, "deltaRMax", std::numeric_limits<float>::max(),
    "maximum deltaR between tracks in a candidate"};
  Gaudi::Property<float> m_deltaRMin {this, "deltaRMin", std::numeric_limits<float>::lowest(),
    "maximum deltaR between tracks in a candidate"};
  Gaudi::Property<bool> m_isCombinedChain {this, "isCombinedChain", false, "true for chains with different signatures, e.g. HLT_e9_mu6"};
  Gaudi::Property<bool> m_isMergedElectronChain {this, "isMergedElectronChain", false, "true for close-by electrons, e.g. HLT_e5_lhvloose_L1BPH-0DR3-EM7J15"};
  Gaudi::Property<bool> m_isMuonTrkPEB {this, "isMuonTrkPEB", false, "chains like HLT_mu6_bJpsimutrk_MuonTrkPEB_L1MU5VF"};
  Gaudi::Property<std::vector<unsigned int>> m_legMultiplicities {this, "legMultiplicities", {2}, "taken from dict[chainMultiplicities]"};
  ToolHandle<GenericMonitoringTool> m_monTool {this, "MonTool", "", "Monitoring tool" };

  std::vector<TrigCompositeUtils::DecisionID> m_legDecisionIDs;
};

#endif  // TRIG_TrigMultiTrkComboHypoTool_H
