/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "JetRecTools/JetTrackSelectionTool2.h"

JetTrackSelectionTool2::JetTrackSelectionTool2(const std::string &name):
  asg::AsgTool(name), m_hidselector("",this) {
  declareProperty("Selector", m_hidselector);
}

StatusCode JetTrackSelectionTool2::initialize() {
  ATH_MSG_INFO("Initializing tool " << "...");
  ATH_MSG_DEBUG("initializing version with data handles");


  StatusCode sc = m_hidselector.retrieve();
  if (sc.isFailure()) {ATH_MSG_ERROR("Can't retrieve ITrackSelectorTool "<< m_hidselector.name() ); return sc;}

  return StatusCode::SUCCESS;
}

int JetTrackSelectionTool2::keep(const xAOD::TrackParticle& trk) const {
  return static_cast<bool>(m_hidselector->accept(trk, 0));
}

