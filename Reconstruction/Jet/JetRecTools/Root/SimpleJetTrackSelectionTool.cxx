/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetRecTools/SimpleJetTrackSelectionTool.h"

SimpleJetTrackSelectionTool::SimpleJetTrackSelectionTool(const std::string &name):
  asg::AsgTool(name) {

  declareProperty("PtMin", m_ptmin =500.0);
  declareProperty("InputContainer", m_inputContainer_key);
  declareProperty("OutputContainer", m_outputContainer_key);
}

StatusCode SimpleJetTrackSelectionTool::initialize() {
  ATH_MSG_INFO("Initializing tool " << name() << "...");
  ATH_MSG_DEBUG("initializing version with data handles");

  ATH_CHECK(m_inputContainer_key.initialize());
  ATH_CHECK(m_outputContainer_key.initialize());
  return StatusCode::SUCCESS;
}

int SimpleJetTrackSelectionTool::execute() const {
  ATH_MSG_DEBUG(" execute() ... ");

  auto handle_in = SG::makeHandle(m_inputContainer_key);
  if(!handle_in.isValid()){
    ATH_MSG_ERROR("Can't retrieve input track container "
                  << m_inputContainer_key.key()); 
    return 1;
  }

  auto inCont = handle_in.cptr();

  using OutContType = ConstDataVector<xAOD::TrackParticleContainer>;
  OutContType* outCont = new OutContType(SG::VIEW_ELEMENTS);

  selectTracks(*inCont, *outCont);
  ATH_MSG_DEBUG(" in size = "<< inCont->size() << " outSize="<< outCont->size());

  auto handle = SG::makeHandle(m_outputContainer_key);
  if(!handle.record(std::unique_ptr<OutContType>(outCont))){
    ATH_MSG_ERROR("Can't record output track container "
                  << m_outputContainer_key.key());
    return 1;
  }
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
