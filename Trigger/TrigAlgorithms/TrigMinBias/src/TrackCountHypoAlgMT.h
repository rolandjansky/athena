/*
Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGMINBIAS_TRACKCOUNTHYPOALGMT_H
#define TRIGMINBIAS_TRACKCOUNTHYPOALGMT_H

// Framework includes
#include "DecisionHandling/HypoBase.h"
#include "xAODTrigger/TrigCompositeContainer.h"
#include "TrackCountHypoTool.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "AthenaMonitoring/Monitored.h"

// STL includes
#include <string>
class TrackCountHypoAlgMT : public ::HypoBase  {
public:
  TrackCountHypoAlgMT(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~TrackCountHypoAlgMT() override;

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& context) const override;
  virtual StatusCode finalize() override;

private:
  TrackCountHypoAlgMT();
  ToolHandleArray< TrackCountHypoTool > m_hypoTools {this, "HypoTools", {},"Tools that perform actual selection"};
  SG::ReadHandleKey<xAOD::TrackParticleContainer> m_tracksKey{this,"tracksKey","Undefined",""};
  SG::WriteHandleKey<xAOD::TrigCompositeContainer> m_trackCountKey{this,"trackCountKey","Undefined",""};
  Gaudi::Property<std::vector<float>> m_min_pt{this, "min_pt",{0.1,0.2,4.0}, "Accept events with momentum higher than this limit"};

  ToolHandle<GenericMonitoringTool> m_monTool{this,"MonTool","","Monitoring tool"};
};
#endif // TRIGT2MINBIAS_TRACKCOUNTHYPOALGMT_H
