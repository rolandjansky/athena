/*
Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGDJDISPHYPOALG_H
#define TRIGDJDISPHYPOALG_H

// Framework includes
#include "DecisionHandling/HypoBase.h"
#include "xAODTrigger/TrigCompositeContainer.h"
#include "DisplacedJetDispHypoTool.h"
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
class DisplacedJetDispHypoAlg : public ::HypoBase  {
public:
  DisplacedJetDispHypoAlg(const std::string& name, ISvcLocator* pSvcLocator);

  virtual ~DisplacedJetDispHypoAlg() = default;
  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& context) const override;

private:
  DisplacedJetDispHypoAlg();
  ToolHandleArray< DisplacedJetDispHypoTool > m_hypoTools {this, "HypoTools", {},"Tools that perform actual selection"};
  SG::ReadHandleKey<xAOD::TrackParticleContainer> m_lrtTracksKey{this,"lrtTracksKey","Undefined",""};
  SG::ReadHandleKey<xAOD::VertexContainer> m_vtxKey{this,"vtxKey","Undefined",""};
  SG::WriteHandleKey<xAOD::TrigCompositeContainer> m_infoKey{this,"infoKey","Undefined",""};

  Gaudi::Property<float> m_drcut{this, "dr_cut",{0.4}, "dR cut used for jet matching of trakcs"};
  Gaudi::Property<float> m_min_trk_pt{this, "min_trk_pt",{0}, "Minimum pt for a track [GeV]"};
};
#endif
