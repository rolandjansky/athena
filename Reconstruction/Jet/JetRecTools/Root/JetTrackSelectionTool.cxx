/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetRecTools/JetTrackSelectionTool.h"

JetTrackSelectionTool::JetTrackSelectionTool(const std::string &name):
  asg::AsgTool(name), m_hidselector("") {
  declareProperty("Selector", m_hidselector);
  declareProperty("InputContainer",m_inputContainer);
  declareProperty("OutputContainer",m_outputContainer);
}

StatusCode JetTrackSelectionTool::initialize() {
  StatusCode sc = m_hidselector.retrieve();
  if (sc.isFailure()) {ATH_MSG_ERROR("Can't retrieve ITrackSelectorTool "<< m_hidselector.name() ); return sc;}
  return StatusCode::SUCCESS;
}

int JetTrackSelectionTool::execute() const {
  ATH_MSG_DEBUG(" execute() ... ");
  const xAOD::TrackParticleContainer* inCont;
  StatusCode sc = evtStore()->retrieve(inCont,m_inputContainer);
  if ( sc.isFailure() ) {ATH_MSG_ERROR("Can't retrieve input track container "<< m_inputContainer); return 1;}
  ConstDataVector<xAOD::TrackParticleContainer> *outCont = new ConstDataVector< xAOD::TrackParticleContainer> (SG::VIEW_ELEMENTS);
  selectTracks(*inCont, *outCont);
  ATH_MSG_DEBUG(" in size = "<< inCont->size() << " outSize="<< outCont->size());
  sc = evtStore()->record( outCont, m_outputContainer);
  if ( sc.isFailure() ) {ATH_MSG_ERROR("Can't record output track container "<< m_outputContainer); return 1;}
  return 0;
}

int JetTrackSelectionTool::keep(const xAOD::TrackParticle& trk) const {
  return m_hidselector->accept(trk, 0);
}

template<class T>
void JetTrackSelectionTool::selectionLoop(const xAOD::TrackParticleContainer& inCont, T& outCont) const {
  for ( const xAOD::TrackParticle* ptrk : inCont ) { 
    if ( ptrk != 0 && keep(*ptrk) ) outCont.push_back(ptrk);
  }
}

void JetTrackSelectionTool::selectTracks(const xAOD::TrackParticleContainer& inCont, ConstDataVector<xAOD::TrackParticleContainer>& outCont) const {
  selectionLoop(inCont, outCont);
}

void JetTrackSelectionTool::selectTracks(const xAOD::TrackParticleContainer& inCont, std::vector<const xAOD::TrackParticle*> outCont) const {
  selectionLoop(inCont, outCont);
}

