/*
Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGMINBIAS_TRACKCOUNTHYPOALGMT_H
#define TRIGMINBIAS_TRACKCOUNTHYPOALGMT_H

// Framework includes
#include "TrackCountHypoTool.h"
#include "DecisionHandling/HypoBase.h"
#include "xAODTrigger/TrigCompositeContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "Gaudi/Property.h"

// STL includes
#include <string>

class TrackCountHypoAlgMT : public ::HypoBase  {
public:
  TrackCountHypoAlgMT(const std::string& name, ISvcLocator* pSvcLocator);

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& context) const override;

private:
  ToolHandleArray< TrackCountHypoTool > m_hypoTools {this, "HypoTools", {}, "Tools that perform actual selection"};
  SG::ReadHandleKey<xAOD::TrackParticleContainer> m_tracksKey{this, "tracksKey", "Undefined", ""};
  SG::WriteHandleKey<xAOD::TrigCompositeContainer> m_trackCountKey{this, "trackCountKey", "Undefined", ""};
  Gaudi::Property<std::vector<float>> m_minPt{this, "minPt", {0.1,0.2,4.0,5.0}, "Accept events with momentum higher than this limit"};
  Gaudi::Property<std::vector<float>> m_maxZ0{this, "maxZ0", {100,401,200,300}, "Accept events with absolute value of vertex position lower \
      than this limit in mm; this vector has to be of the same size as minPt vector, repetition of cut values are allowed if that makes the number of elements the same"};
  ToolHandle<GenericMonitoringTool> m_monTool{this,"MonTool", "", "Monitoring tool"};
};

#endif
