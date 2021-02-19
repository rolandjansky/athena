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
  virtual bool executeAlg(std::vector<LegDecision>&) const override { return true; }
  bool checkPreviousDecisionIDs(const std::vector<const TrigCompositeUtils::DecisionIDContainer*>&) const;
  int totalCharge(const xAOD::TrigBphys*) const;
  bool isInMassRange(double mass) const;
  inline bool passedChi2Cut(int chi2) const { return (m_chi2 < 0. || chi2 < m_chi2); }
  inline bool passedChargeCut(int charge) const { return (m_totalCharge < 0 || std::abs(charge) == m_totalCharge); }

  Gaudi::Property<unsigned int> m_nTrk {this, "nTrk", 2, "number of tracks in the vertex"};
  Gaudi::Property<int> m_totalCharge {this, "totalCharge", 0, "magnitude of the total charge to accept, negative is none" };
  Gaudi::Property<std::pair<double, double>> m_massRange {this, "massRange", {-99., -9.}, "range for the fitted mass, no selection applied if negative"};
  Gaudi::Property<float> m_chi2 {this, "chi2", -99. , "Chi2 cut for vertex (0 < chi2 < cut), no selection applied if negative" };
  Gaudi::Property<bool> m_acceptAll {this, "AcceptAll", false, "if AcceptAll flag is set to true, no selection will be applied for xAOD::TrigBphys object" };
  Gaudi::Property<float> m_LxyCut {this, "LxyCut", -999., "Applies an Lxy Cut if set > -999"};
  ToolHandle<GenericMonitoringTool> m_monTool { this, "MonTool", "", "Monitoring tool" };
};

#endif  // TRIG_TrigMultiTrkComboHypoTool_H
