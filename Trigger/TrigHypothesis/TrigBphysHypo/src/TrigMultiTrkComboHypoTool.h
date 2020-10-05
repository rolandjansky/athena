/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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

#include "DecisionHandling/ComboHypoToolBase.h"

#include "xAODTrigBphys/TrigBphys.h"
#include "xAODTrigBphys/TrigBphysContainer.h"

#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"

#include "AthenaMonitoringKernel/Monitored.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"

using TrigCompositeUtils::Decision;
using TrigCompositeUtils::DecisionIDContainer;


class TrigMultiTrkComboHypoTool: public ComboHypoToolBase {
 public:
  TrigMultiTrkComboHypoTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual StatusCode initialize() override;
  virtual StatusCode decideOnSingleObject(Decision*, const std::vector<DecisionIDContainer*>&) const override;

 private:
  bool passed(const xAOD::TrigBphys*) const;
  virtual bool executeAlg(std::vector<LegDecision>&) const override { return true; }


  // Mass window cuts
  Gaudi::Property<int> m_nTrk { this, "nTrk",2,"Number of tracks in the vertex"};
  Gaudi::Property< int > m_TotChargeCut{this, "TotChargeCut", 0, "The Magnitude of the total charge to accept, negative is none" };
  Gaudi::Property< float > m_lowerMassCut{this, "LowerMassCut", -99. , "Lower mass cut for vertex " };
  Gaudi::Property< float > m_upperMassCut{this, "UpperMassCut", -99. , "Upper mass cut for vertex" };
  Gaudi::Property< bool >m_applyUpperMassCut{this, "ApplyUpperMassCut", false, "Apply the upper mass cut" };
  Gaudi::Property< bool > m_applyChi2Cut{this, "ApplyChi2Cut", false, "Apply a chi2 cut to vertex" };
  Gaudi::Property< float > m_chi2VtxCut{this, "Chi2VtxCut", -99. , "Chi2 cut for vertex (0 < chi2 < cut)" };
  Gaudi::Property<std::vector<float>> m_ptTrkMin { this, "trkPtThresholds", {3650, 3650} ,"minimum track pTs (one per track, sorted descending!)"};

  // to set Accept-All mode: should be done with force-accept when possible
  Gaudi::Property< bool > m_acceptAll {this, "AcceptAll", false, "Ignore selection" };

  ToolHandle<GenericMonitoringTool> m_monTool { this, "MonTool", "", "Monitoring tool" };

};

#endif  // TRIG_TrigMultiTrkComboHypoTool_H
