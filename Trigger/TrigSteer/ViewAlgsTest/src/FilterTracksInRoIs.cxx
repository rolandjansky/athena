/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <sstream>
#include <algorithm>
#include "CxxUtils/make_unique.h"
#include "L1Decoder/TrigIdentifiers.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "ViewAlgs/TrigCompositeTraversal.h"

#include "./FilterTracksInRoIs.h"

FilterTracksInRoIs::FilterTracksInRoIs(const std::string& name, ISvcLocator* pSvcLocator) 
  : AthAlgorithm(name, pSvcLocator),
    m_assocTool(this),
    m_compContainer("MenuDecisions") {      
  declareProperty("CompContainer", m_compContainer, "Composites with links to RoIs");
  declareProperty("OutputTracksContainer", m_outputTracks, "Filtered output tracks container");
}

StatusCode FilterTracksInRoIs::initialize() {
  CHECK( m_compContainer.initialize() );
  CHECK( m_outputTracks.initialize() );  
  CHECK( m_assocTool.retrieve() );
  return StatusCode::SUCCESS;
}


StatusCode FilterTracksInRoIs::execute() {

  SG::ReadHandle< xAOD::TrigCompositeContainer > comps( m_compContainer );
  if ( not comps.isValid() ) {
    ATH_MSG_ERROR("No decisions object from previous stage");
    return StatusCode::FAILURE;
  }

  auto tracks = CxxUtils::make_unique< ConstDataVector<TrackCollection>>();
  tracks->clear(SG::VIEW_ELEMENTS);

  ATH_MSG_DEBUG("Got comps");

  // since there are links to tracks in the composites we do not need the source tracks at all, just links are enough
  for ( const auto comp: *comps.cptr() ) {
    const xAOD::TrigComposite* prev = comp->object<xAOD::TrigComposite>("seed");

    ATH_MSG_DEBUG("Got comp");
    ElementLinkVector<TrackCollection> elvec;
    if ( m_assocTool->getTrackLinks(prev, elvec).isFailure() )
      return StatusCode::FAILURE;
    ATH_MSG_DEBUG("Links to tracks size : " << elvec.size() );
    for ( auto el: elvec ) {
      const Trk::Track* tp = *el;

      ATH_MSG_DEBUG("Track in passing RoI : " <<  tp->info().dumpInfo() );
      tracks->push_back(tp);
    }
  }
  SG::WriteHandle<ConstDataVector<TrackCollection> > tracksHandle( m_outputTracks );
  tracksHandle.record( std::move(tracks) );
  return StatusCode::SUCCESS;
}
