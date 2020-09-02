/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "JetRecTools/JetTrackSelectionTool.h"

JetTrackSelectionTool::JetTrackSelectionTool(const std::string &name):
  asg::AsgTool(name), m_hidselector("",this) {
  declareProperty("Selector", m_hidselector);

  declareProperty("InputContainer",m_inCont_key);
  declareProperty("OutputContainer",m_outCont_key);
}

StatusCode JetTrackSelectionTool::initialize() {
  ATH_MSG_INFO("Initializing tool " << "...");
  ATH_MSG_DEBUG("initializing version with data handles");


  StatusCode sc = m_hidselector.retrieve();
  if (sc.isFailure()) {ATH_MSG_ERROR("Can't retrieve ITrackSelectorTool "<< m_hidselector.name() ); return sc;}

  if(!(m_inCont_key.key().empty() && m_outCont_key.key().empty())) {
    ATH_CHECK(m_inCont_key.initialize());
    ATH_CHECK(m_outCont_key.initialize());
  }

  return StatusCode::SUCCESS;
}

int JetTrackSelectionTool::execute() const {
  ATH_MSG_DEBUG(" execute() ... ");

  auto handle_in =  SG::makeHandle(m_inCont_key);
  if (! handle_in.isValid()){
    // ATH_MSG_ERROR("Can't retrieve input track container "<< m_inputContainer);
    ATH_MSG_ERROR("Can't retrieve input track container "
                  << m_inCont_key.key());
    return 1;
  }
  
  auto inCont = handle_in.cptr();

  using OutContType = ConstDataVector<xAOD::TrackParticleContainer>;
  OutContType* pOutCont = new OutContType(SG::VIEW_ELEMENTS);

  selectTracks(*inCont, *pOutCont);

  auto handle_out =  SG::makeHandle(m_outCont_key);

  if (!handle_out.record(std::unique_ptr<OutContType>(pOutCont))){
    // ATH_MSG_ERROR("Can't record output track container "<< m_outputContainer);
    ATH_MSG_ERROR("Can't record output track container "
                  << m_outCont_key.key());
    return 1;
  }

  return 0;
}

int JetTrackSelectionTool::keep(const xAOD::TrackParticle& trk) const {
  return static_cast<bool>(m_hidselector->accept(trk, 0));
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

