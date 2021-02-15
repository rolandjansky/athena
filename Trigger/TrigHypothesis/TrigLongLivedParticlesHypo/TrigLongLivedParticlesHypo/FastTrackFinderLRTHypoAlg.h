/*
Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGFTFLRTHYPOALG_H
#define TRIGFTFLRTHYPOALG_H

// Framework includes
#include "DecisionHandling/HypoBase.h"
#include "xAODTrigger/TrigCompositeContainer.h"
#include "FastTrackFinderLRTHypoTool.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include <map>
#include "Gaudi/Property.h"
#include "AthViews/View.h"

// STL includes
#include <string>
class FastTrackFinderLRTHypoAlg : public ::HypoBase  {
public:
  FastTrackFinderLRTHypoAlg(const std::string& name, ISvcLocator* pSvcLocator);

  virtual ~FastTrackFinderLRTHypoAlg() = default;
  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& context) const override;

private:
  FastTrackFinderLRTHypoAlg();
  ToolHandleArray< FastTrackFinderLRTHypoTool > m_hypoTools {this, "HypoTools", {},"Tools that perform actual selection"};
  SG::ReadHandleKey<xAOD::TrackParticleContainer> m_tracksKey{this,"tracksKey","Undefined",""};
  SG::WriteHandleKey<xAOD::TrigCompositeContainer> m_trackCountKey{this,"trackCountKey","Undefined",""};
  Gaudi::Property<std::vector<float>> m_min_pt{this, "min_pt",{0}, "Accept events with momentum higher than this limit"};
  Gaudi::Property<std::vector<float>> m_min_d0{this, "min_d0",{0}, "Accept events with d0 above this limit"};
  Gaudi::Property<std::vector<float>> m_max_d0{this, "max_d0",{900}, "Accept events with d0 below this limit"};
  Gaudi::Property<std::vector<float>> m_max_xi2{this, "max_xi2",{10}, "Accept events with Xi2 of the track less than this limit"};
  ToolHandle<GenericMonitoringTool> m_monTool{this,"MonTool","","Monitoring tool"};
};
#endif // TRIGT2MINBIAS_TRACKCOUNTHYPOALGMT_H
