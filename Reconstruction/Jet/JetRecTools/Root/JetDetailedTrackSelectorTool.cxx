/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetDetailedTrackSelectorTool.cxx

#include "JetRecTools/JetDetailedTrackSelectorTool.h"

JetDetailedTrackSelectorTool::
JetDetailedTrackSelectorTool(const std::string& t, const std::string& n, const IInterface*  p)
: AthAlgTool(t,n,p) {
  declareInterface<ITrackSelectorTool>(this);
}

JetDetailedTrackSelectorTool::~JetDetailedTrackSelectorTool() { }
  
StatusCode JetDetailedTrackSelectorTool::initialize() {
  return StatusCode::SUCCESS;
}

StatusCode JetDetailedTrackSelectorTool::finalize() {
  return StatusCode::SUCCESS;
}

bool JetDetailedTrackSelectorTool::decision(const Trk::Track&, const Trk::Vertex*) const {
  return true;
}

bool JetDetailedTrackSelectorTool::decision(const Trk::TrackParticleBase&, const Trk::Vertex*) const {
  return true;
}

bool JetDetailedTrackSelectorTool::decision(const xAOD::TrackParticle&,const xAOD::Vertex*) const  {
  return true;
}
