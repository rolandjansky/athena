/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef MYNEWPACKAGE_EXAMPLEALG_H
#define MYNEWPACKAGE_EXAMPLEALG_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "DevTutorialWriteAlgAndTool/IMyAlgTool.h"

/** An example algorithm that reads and writes objects from the event store
 using handles.*/
class ExampleAlg
  : public AthReentrantAlgorithm
{
public:
  ExampleAlg (const std::string& name, ISvcLocator* svcLocator);
  virtual StatusCode initialize() override;
  virtual StatusCode execute (const EventContext& ctx) const override;
private:
  SG::ReadHandleKey<xAOD::TrackParticleContainer>   m_trackParticleKey
     { this, "TrackParticleContainerKey", "InDetTrackParticles", "Key for TrackParticle Containers" };
  
  ToolHandle<IMyAlgTool> m_algTool {this, "SelectionTool", "MyAlgTool", "The selection tool"};
};
#endif
