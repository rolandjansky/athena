/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetRecTools/SimpleJetTrackSelectionTool.h"

SimpleJetTrackSelectionTool::SimpleJetTrackSelectionTool(const std::string &name):
  asg::AsgTool(name) {
  declareProperty("PtMin", m_ptmin =500.0);
  declareProperty("InputContainer", m_inputContainer);
  declareProperty("OutputContainer", m_outputContainer);
}

StatusCode SimpleJetTrackSelectionTool::initialize() {
  return StatusCode::SUCCESS;
}

int SimpleJetTrackSelectionTool::execute() const {
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

int SimpleJetTrackSelectionTool::keep(const xAOD::TrackParticle& trk) const {
  if ( trk.pt() < m_ptmin ) return false;
  return true;
}

template<class T>
void SimpleJetTrackSelectionTool::selectionLoop(const xAOD::TrackParticleContainer& inCont, T& outCont) const {
  for ( const xAOD::TrackParticle* ptrk : inCont ) { 
    if ( ptrk != 0 && keep(*ptrk) ) outCont.push_back(ptrk) ; 
  }
}


void SimpleJetTrackSelectionTool::selectTracks(const xAOD::TrackParticleContainer& inCont, ConstDataVector<xAOD::TrackParticleContainer>& outCont) const {
  selectionLoop(inCont, outCont);
}

void SimpleJetTrackSelectionTool::selectTracks(const xAOD::TrackParticleContainer& inCont, std::vector<const xAOD::TrackParticle*> outCont) const {
  selectionLoop(inCont, outCont);
}

