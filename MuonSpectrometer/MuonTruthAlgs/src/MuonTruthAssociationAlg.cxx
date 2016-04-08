/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MuonTruthAssociationAlg.cxx
//   Implementation file for class MuonTruthAssociationAlg
///////////////////////////////////////////////////////////////////

#include "MuonTruthAssociationAlg.h"

// Constructor with parameters:
MuonTruthAssociationAlg::MuonTruthAssociationAlg(const std::string &name, ISvcLocator *pSvcLocator) :
  AthAlgorithm(name,pSvcLocator)
{  
    
  // Get parameter values from jobOptions file
  declareProperty("MuonTruthParticleContainerName", m_muonTruthParticleContainerName = "MuonTruthParticles");
  declareProperty("MuonContainerName"             , m_muonName = "Muons");
  declareProperty("AssociateWithInDetTP"          , m_associateWithInDetTP = false); // hack: set true for conversion from release 17
}

// Initialize method:
StatusCode MuonTruthAssociationAlg::initialize()
{
  return StatusCode::SUCCESS;
}

// Execute method:
StatusCode MuonTruthAssociationAlg::execute() 
{
    
  // skip if no input data found
  if ( !evtStore()->contains<xAOD::TruthParticleContainer>(m_muonTruthParticleContainerName) ) return StatusCode::SUCCESS;
  if ( !evtStore()->contains<xAOD::MuonContainer>(m_muonName) ) return StatusCode::SUCCESS;

  // retrieve Truth, exit if fails
  const xAOD::TruthParticleContainer* truthContainer{0};
  ATH_CHECK(evtStore()->retrieve(truthContainer,m_muonTruthParticleContainerName ));

  // get muon container
  const xAOD::MuonContainer* muons = 0;
  ATH_CHECK(evtStore()->retrieve(muons, m_muonName));
    
  /////////DEBUG/////////
  //list all reco muons
  // ATH_MSG_DEBUG("All reco muons:");
  // for( const auto & muon : *muons ){
  //   ATH_MSG_DEBUG(" auth " <<  muon->author() <<" pt " << muon->pt() << " eta " << muon->eta() );
  //   //xAOD::TrackParticle* tp  = const_cast<xAOD::TrackParticle*>(muon->primaryTrackParticle());
  //   const xAOD::TrackParticle* tp  = muon->trackParticle(xAOD::Muon_v1::InnerDetectorTrackParticle);
  //   if( !tp ) continue;
  //   ATH_MSG_VERBOSE("  trackParticle pt " << tp->pt() << " eta " << tp->eta() );
  //   if (!tp->isAvailable<ElementLink< xAOD::TruthParticleContainer > >("truthParticleLink")) {
  //     ATH_MSG_VERBOSE("  truthParticleLink not available for this track particle");
  //     continue;
  //   }
  //   ElementLink<xAOD::TruthParticleContainer> truthLink = tp->auxdata<ElementLink<xAOD::TruthParticleContainer> >("truthParticleLink" );
  //   if (!truthLink.isValid()) { 
  //     ATH_MSG_VERBOSE("  Muon track particle has no truth association, skipping");
  //     continue;
  //   }
  //   ATH_MSG_DEBUG("  truthParticleLink status " << (*truthLink)->status() << " barcode " << (*truthLink)->barcode() << " pt "<< (*truthLink)->pt() << " eta "<< (*truthLink)->eta());
  // }
  //////////////////////////


  // add limk to reco muons and viceversa
  addMuon(*truthContainer, *muons);
  
  return StatusCode::SUCCESS;
}

void MuonTruthAssociationAlg::addMuon( const xAOD::TruthParticleContainer& truthParticles, const xAOD::MuonContainer& muons ) {

  // loop over truth particles
  for( const auto& truthParticle : truthParticles ){
    if( truthParticle->status() != 1 ) continue;   
    
    // Associate reco with truth muon. Loop over reconstructed muons, get track particle for each one. Each track particle should carry a link to the corresponding truth particle. Then compare this truth particle link with the given truth muon particle
    
    ATH_MSG_DEBUG("Adding recoMuonLink for truth muon with barcode " << truthParticle->barcode() << " pt "<< truthParticle->pt());
    ElementLink< xAOD::MuonContainer > muonLink;
    
    // loop over muons 
    for( const auto& muon : muons ){
      // use primary track particle to get the truth link
      //xAOD::TrackParticle* tp = const_cast<xAOD::TrackParticle*>(muon->primaryTrackParticle());
      //// hack: when converting from release 17, these truth links are absent (must fix); use the inDetParticle instead
      const xAOD::TrackParticle* tp(0);
      if (m_associateWithInDetTP || muon->author()==2) {
	tp = muon->trackParticle(xAOD::Muon_v1::InnerDetectorTrackParticle);
      } 
      else{
	tp = const_cast<xAOD::TrackParticle*>(muon->primaryTrackParticle());
      }
      
      if( !tp ) continue;
      
      // special case for STACO muons where the combined TP is not truth matched
      if( muon->muonType() == xAOD::Muon::Combined && !tp->track() ){
	tp = const_cast<xAOD::TrackParticle*>(*muon->inDetTrackParticleLink());
      }
      if( !tp ) continue;
      
      if( tp ){
	ElementLink< xAOD::TruthParticleContainer > truthLink = tp->auxdata<ElementLink< xAOD::TruthParticleContainer > >("truthParticleLink");
	if( truthLink.isValid() ){
	  if( (*truthLink)->barcode() == truthParticle->barcode() ) {
	    ATH_MSG_VERBOSE(" Got truth link -> creating link with truth particle " << (*truthLink)->barcode() );
	    muonLink = ElementLink< xAOD::MuonContainer >(muon,muons);
	    // add the link from xAOD::Muon to TruthParticle in m_muonTruthParticleContainerName
	    ElementLink< xAOD::TruthParticleContainer > muonTruthLink = ElementLink< xAOD::TruthParticleContainer >(truthParticle, truthParticles);
	    muonTruthLink.toPersistent();
	    const_cast<xAOD::Muon&>(*muon).auxdata<ElementLink< xAOD::TruthParticleContainer > >("truthParticleLink") = muonTruthLink;  	  
	    break;
	  }
	}
	else{
	  ATH_MSG_VERBOSE(" Reco muon has no truth association");
	}
      }      
    }
    muonLink.toPersistent();
    const_cast<xAOD::TruthParticle&>(*truthParticle).auxdata<ElementLink< xAOD::MuonContainer > >("recoMuonLink") = muonLink;
  }
}

// Finalize method:
StatusCode MuonTruthAssociationAlg::finalize() 
{
  return StatusCode::SUCCESS;
}

