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
// STL includes
#include <string>
class TrackCountHypoAlgMT : public ::HypoBase  {
public:
  TrackCountHypoAlgMT(const std::string& name, ISvcLocator* pSvcLocator);

  virtual ~TrackCountHypoAlgMT() = default;
  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& context) const override;

private:
  TrackCountHypoAlgMT();
  ToolHandleArray< TrackCountHypoTool > m_hypoTools {this, "HypoTools", {},"Tools that perform actual selection"};
  SG::ReadHandleKey<xAOD::TrackParticleContainer> m_tracksKey{this,"tracksKey","Undefined",""};
  SG::WriteHandleKey<xAOD::TrigCompositeContainer> m_trackCountKey{this,"trackCountKey","Undefined",""};
  Gaudi::Property<std::vector<float>> m_min_pt{this, "min_pt",{100,200,4000}, "Accept events with momentum higher than this limit in MeV"};
  Gaudi::Property<std::vector<float>> m_max_z0{this, "max_z0",{100,200,300}, "Accept events with absolute value of vertex position lower \
  than this limit in mm; this vector has to be of the same size as min_pT vector, repetition of cut values are allowed if that makes the number of elements the same"};
};
#endif // TRIGT2MINBIAS_TRACKCOUNTHYPOALGMT_H
