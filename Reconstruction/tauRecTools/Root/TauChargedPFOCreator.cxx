/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//system
#include <vector>

//tau
#include "xAODTau/TauJet.h"
#include "tauRecTools/TauChargedPFOCreator.h"

#include "xAODTracking/TrackParticleContainer.h"

/********************************************************************/
TauChargedPFOCreator::TauChargedPFOCreator(const std::string& name) :
  TauRecToolBase(name),
  m_chargedPFOContainer(0),
  m_chargedPFOAuxStore(0)
{
    declareProperty("chargedPFOContainerName", m_chargedPFOContainerName="TauChargedParticleFlowObjectsFix");
    declareProperty("TrackFlag", m_iTrackFlag = xAOD::TauJetParameters::classifiedCharged);
}

/********************************************************************/
TauChargedPFOCreator::~TauChargedPFOCreator() {
}

/********************************************************************/
StatusCode TauChargedPFOCreator::initialize() {
  return StatusCode::SUCCESS;
}

/********************************************************************/
StatusCode TauChargedPFOCreator::eventInitialize() { 

  m_chargedPFOContainer = new xAOD::PFOContainer();
  m_chargedPFOAuxStore = new xAOD::PFOAuxContainer();
  m_chargedPFOContainer->setStore(m_chargedPFOAuxStore);
  ATH_CHECK(evtStore()->record( m_chargedPFOContainer, m_chargedPFOContainerName ));
  ATH_CHECK(evtStore()->record( m_chargedPFOAuxStore, m_chargedPFOContainerName+"Aux." ));

  return StatusCode::SUCCESS; 
}

/********************************************************************/
StatusCode TauChargedPFOCreator::execute(xAOD::TauJet& pTau) 
{   

  std::vector<ElementLink< xAOD::PFOContainer > > chargedPFOLinks;

  const xAOD::TrackParticleContainer* trackContainer = 0;
  ATH_CHECK( evtStore()->retrieve( trackContainer, "InDetTrackParticles" ) );
  

  for( const xAOD::TauTrack* trk : pTau.tracks( (xAOD::TauJetParameters::TauTrackFlag) m_iTrackFlag) ){
    xAOD::PFO* chargedPFO = new xAOD::PFO();
    m_chargedPFOContainer->push_back(chargedPFO);
    ElementLink<xAOD::TrackParticleContainer> myTrackLink = trk->trackLinks()[0];
    //    chargedPFO->setTrackLink(myTrackLink);
    ElementLink<xAOD::IParticleContainer> myParticleLink;
    myParticleLink.toContainedElement( *( static_cast<const xAOD::IParticleContainer*> ((*myTrackLink)->container())), 
				       dynamic_cast<const xAOD::IParticle*> (*myTrackLink));
    chargedPFO->setAssociatedParticleLink(xAOD::PFODetails::Track, myParticleLink);
    chargedPFO->setCharge(trk->track()->charge());
    chargedPFO->setP4(trk->pt(),trk->eta(),trk->phi(),trk->m());
    ElementLink<xAOD::PFOContainer> PFOElementLink;
    PFOElementLink.toContainedElement( *m_chargedPFOContainer, chargedPFO );
    chargedPFOLinks.push_back(PFOElementLink);    
  }

  //  ATH_MSG_INFO(pTau.auxdata<int>("nTracks") << " " << pTau.auxdata<int>("nTracksOld"));

  pTau.clearProtoChargedPFOLinks();
  pTau.setProtoChargedPFOLinks(chargedPFOLinks);
    
  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
// Finalize
//-----------------------------------------------------------------------------

StatusCode TauChargedPFOCreator::finalize() {
  return StatusCode::SUCCESS;
}
