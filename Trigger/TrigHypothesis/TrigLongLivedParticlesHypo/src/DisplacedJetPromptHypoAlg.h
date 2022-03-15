/*
Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGDJPROMPTHYPOALG_H
#define TRIGDJPROMPTHYPOALG_H

// Framework includes
#include "DecisionHandling/HypoBase.h"
#include "xAODTrigger/TrigCompositeContainer.h"
#include "DisplacedJetPromptHypoTool.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODJet/JetContainer.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include <map>
#include "Gaudi/Property.h"
#include "AthViews/View.h"

// STL includes
#include <string>
class DisplacedJetPromptHypoAlg : public ::HypoBase  {
public:
  DisplacedJetPromptHypoAlg(const std::string& name, ISvcLocator* pSvcLocator);

  virtual ~DisplacedJetPromptHypoAlg() = default;
  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& context) const override;

private:
  DisplacedJetPromptHypoAlg();
  ToolHandleArray< DisplacedJetPromptHypoTool > m_hypoTools {this, "HypoTools", {},"Tools that perform actual selection"};
  SG::ReadHandleKey<xAOD::JetContainer> m_jetContainerKey{this,"jetContainerKey", "Undefined", ""};
  SG::ReadHandleKey<xAOD::TrackParticleContainer> m_stdTracksKey{this,"stdTracksKey","Undefined",""};
  SG::ReadHandleKey<xAOD::VertexContainer> m_vtxKey{this,"vtxKey","Undefined",""};
  SG::WriteHandleKey<xAOD::TrigCompositeContainer> m_countsKey{this,"countsKey","Undefined",""};

  Gaudi::Property<float> m_drcut{this, "dr_cut",{0.4}, "dR cut used for jet matching of trakcs"};
  Gaudi::Property<float> m_min_trk_pt{this, "min_trk_pt",{0}, "Minimum pt for a track [GeV]"};
  Gaudi::Property<float> m_min_evt_jet_pt{this, "min_evt_jet_pt",{0}, "Minimum pt for the leading jet [GeV]"};

  ToolHandle<GenericMonitoringTool> m_monTool{this,"MonTool","","Monitoring tool"};
};
#endif
