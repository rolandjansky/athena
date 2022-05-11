/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#include "JetRecTools/JetTrackSelectionAlg.h"
#include "AsgDataHandles/ReadHandle.h"
#include "AsgDataHandles/WriteHandle.h"


StatusCode JetTrackSelectionAlg::initialize() {
  ATH_MSG_DEBUG("Initializing  " );


  StatusCode sc = m_selector.retrieve();
  if (sc.isFailure()) {ATH_MSG_ERROR("Can't retrieve ITrackSelectorTool "<< m_selector.name() ); return sc;}

  ATH_CHECK(m_input.initialize());
  ATH_CHECK(m_output.initialize());

  ATH_CHECK(m_decorDeps.initialize(m_input, m_output));
  
  return StatusCode::SUCCESS;
}


StatusCode JetTrackSelectionAlg::execute(const EventContext& ctx) const {
  ATH_MSG_DEBUG(" execute() ... ");

  auto inputTracksH =  SG::makeHandle(m_input,ctx);
  if (! inputTracksH.isValid()){
    ATH_MSG_ERROR("Can't retrieve input track container "<< m_input.key());                  
    return StatusCode::FAILURE;
  }

  // create and fill the VIEW container of selected tracks 
  using OutContType = ConstDataVector<xAOD::TrackParticleContainer>;
  auto selectedTracks = std::make_unique<OutContType>(SG::VIEW_ELEMENTS);

  for ( const xAOD::TrackParticle* trk : *inputTracksH.cptr() ) {
    if ( trk != 0 && m_selector->accept(*trk, 0) ) {
      selectedTracks->push_back(trk);
    }
  }


  // Write out in the event store 
  auto handle_out =  SG::makeHandle(m_output, ctx);
  
  std::unique_ptr<const xAOD::TrackParticleContainer> selectedTracks_tc(selectedTracks.release()->asDataVector());
  if (handle_out.put(std::move(selectedTracks_tc))==nullptr ){
    ATH_MSG_ERROR("Can't record output track container "<< m_output.key());
    return StatusCode::FAILURE;
  }

  ATH_CHECK(m_decorDeps.linkDecors (m_input, ctx));
  return StatusCode::SUCCESS; 
}
