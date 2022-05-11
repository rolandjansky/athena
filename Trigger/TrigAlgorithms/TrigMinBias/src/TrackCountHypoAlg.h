/*
Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGMINBIAS_TRACKCOUNTHYPOALG_H
#define TRIGMINBIAS_TRACKCOUNTHYPOALG_H

// Framework includes
#include "TrackCountHypoTool.h"
#include "DecisionHandling/HypoBase.h"
#include "xAODTrigger/TrigCompositeContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "Gaudi/Property.h"

// STL includes
#include <string>

class TrackCountHypoAlg : public ::HypoBase  {
public:
  TrackCountHypoAlg(const std::string& name, ISvcLocator* pSvcLocator);

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& context) const override;

private:
  ToolHandleArray< TrackCountHypoTool > m_hypoTools {this, "HypoTools", {}, "Tools that perform actual selection"};
  SG::ReadHandleKey<xAOD::TrackParticleContainer> m_tracksKey{this, "tracksKey", "Undefined", ""};
  SG::WriteHandleKey<xAOD::TrigCompositeContainer> m_trackCountKey{this, "trackCountKey", "Undefined", ""};
  Gaudi::Property<std::vector<float>> m_minPt{this, "minPt", {100, 200, 200, 500, 1000, 2000, 4000, 6000, 8000}, "Accept events with momentum higher than this limit in MeV"};
  Gaudi::Property<std::vector<float>> m_maxZ0{this, "maxZ0", {401, 401, 401, 401, 401, 401, 401, 401, 401}, "Accept events with absolute value of z position of track passing beamline lower than this limit in mm"};
  Gaudi::Property<std::vector<float>> m_vertexZ{this, "vertexZ", {803, 803, 10, 803, 803, 803, 803, 803, 803}, "Accept events with distance to vertex z position lower than this limit in mm"};

  ToolHandle<GenericMonitoringTool> m_monTool{this,"MonTool", "", "Monitoring tool"};
};

#endif
