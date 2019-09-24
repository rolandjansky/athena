/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef MYNEWPACKAGE_ExampleAlg_STEP3_H
#define MYNEWPACKAGE_ExampleAlg_STEP3_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "xAODTracking/TrackParticleContainer.h"

/** An example algorithm that reads and writes objects from the event store
 using handles.*/
class ExampleAlg_step3
  : public AthReentrantAlgorithm
{
public:
  ExampleAlg_step3 (const std::string& name, ISvcLocator* svcLocator);
  virtual StatusCode initialize() override;
  virtual StatusCode execute (const EventContext& ctx) const override;
private:
  /// 
  SG::ReadHandleKey<xAOD::TrackParticleContainer>   m_trackParticleKey
     { this, "TrackParticleContainerKey", "InDetTrackParticles", "Key for TrackParticle Containers" };
};
#endif
