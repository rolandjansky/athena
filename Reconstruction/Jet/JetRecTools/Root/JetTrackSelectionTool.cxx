/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetRecTools/JetTrackSelectionTool.h"

JetTrackSelectionTool::JetTrackSelectionTool(const std::string &name):
  asg::AsgTool(name)
  , m_selector("InDetDetailedTrackSelectorTool/TrackSelectorForJets")
  , m_ptMin(500)
  , m_etaMin(-5)
  , m_etaMax(5)
  , m_inputContainer("InDetTrackParticle")
  , m_outputContainer("InDetTrackParticle")
{

  declareProperty("Selector",m_selector);
  declareProperty("PtMin",m_ptMin= 500);
  declareProperty("EtaMin",m_etaMin= -5);
  declareProperty("EtaMax",m_etaMax= 5);
 
  declareProperty("InputContainer",m_inputContainer);
  declareProperty("OutputContainer",m_outputContainer);
  
}
StatusCode JetTrackSelectionTool::initialize() {
  StatusCode sc = m_selector.retrieve();
  if (sc.isFailure()) {ATH_MSG_ERROR("Can't retrieve ITrackSelectorTool "<< m_selector.name() ); return sc;}
  return StatusCode::SUCCESS;
}

int JetTrackSelectionTool::execute() const {
  ATH_MSG_DEBUG(" execute() ... ");
  const xAOD::TrackParticleContainer* inCont;
  StatusCode sc = evtStore()->retrieve(inCont,m_inputContainer);
  if( sc.isFailure() ) {ATH_MSG_ERROR("Can't retrieve input track container "<< m_inputContainer); return 1;}

  ConstDataVector< xAOD::TrackParticleContainer> *outCont = new ConstDataVector< xAOD::TrackParticleContainer> (SG::VIEW_ELEMENTS);
  selectTracks( *inCont, *outCont);
  ATH_MSG_DEBUG(" in size = "<< inCont->size() << " outSize="<< outCont->size());

  sc = evtStore()->record( outCont, m_outputContainer);
  if( sc.isFailure() ) {ATH_MSG_ERROR("Can't record output track container "<< m_outputContainer); return 1;}

  return 0;
}


template<class T>
void JetTrackSelectionTool::selectionLoop(const xAOD::TrackParticleContainer& inCont, T& outCont) const {
  outCont.reserve(inCont.size());
  for( const xAOD::TrackParticle* trk : inCont) { 
    if( (trk->pt() > m_ptMin) &&
        (trk->eta() > m_etaMin) &&
        (trk->eta() < m_etaMax) &&
        (m_selector->decision( *trk ) ) ) outCont.push_back(trk) ; 
  }
}


void JetTrackSelectionTool::selectTracks(const xAOD::TrackParticleContainer& inCont, ConstDataVector<xAOD::TrackParticleContainer>& outCont) const {
  selectionLoop(inCont, outCont);
}
void JetTrackSelectionTool::selectTracks(const xAOD::TrackParticleContainer& inCont, std::vector<const xAOD::TrackParticle*> outCont) const {
  selectionLoop(inCont, outCont);
}

