/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "./TrackHypoTool.h"


TrackHypoTool::TrackHypoTool(const std::string& type, const std::string& name,  const IInterface* parent) 
  : AthAlgTool(type, name, parent),
    m_trackMultiplicity(0){  
  declareInterface<TrackHypoTool>( this );
  declareProperty("TrackMultiplicity", m_trackMultiplicity);
}

StatusCode TrackHypoTool::initialize() {
  return StatusCode::SUCCESS;
}
DecoratedStatusCode<bool> TrackHypoTool::decision(ElementLinkVector<TrackCollection> links) const {
  size_t size=links.size();
  ATH_MSG_DEBUG("N.tracks " << size << " threshold " << m_trackMultiplicity);
  return DecoratedStatusCode<bool>(StatusCode::SUCCESS, size > m_trackMultiplicity);
}
