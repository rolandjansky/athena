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
  declareProperty("MuonTruthParticleContainerName", m_muonTruthParticleContainerName = "MuonTruthParticle");
  declareProperty("MuonContainerName"             , m_muonName = "Muons");
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
    
  // loop over truth coll
  for( const auto& truth : *truthContainer ){
    if( truth->status() != 1 ) continue;
    // add link to reco muons
    addMuon(*truth,*muons);
  }
    
  return StatusCode::SUCCESS;
}

void MuonTruthAssociationAlg::addMuon( const xAOD::TruthParticle& truthParticle, const xAOD::MuonContainer& muons ) {
    
    
  ElementLink< xAOD::MuonContainer > muonLink;

  // loop over muons 
  for( const auto& muon : muons ){
    ATH_MSG_VERBOSE(" Muon " << muon << " pt " << muon->pt() << " eta " << muon->eta() << " phi " << muon->phi());
    // get truth of primary track particle
    xAOD::TrackParticle* tp  = const_cast<xAOD::TrackParticle*>(muon->primaryTrackParticle());
    if( !tp ){
      ATH_MSG_WARNING("Found muon without primary TrackParticle : author " << muon->author() << " type " << muon->muonType() 
                      << " links CB " << muon->combinedTrackParticleLink().isValid() 
                      << " ID " << muon->inDetTrackParticleLink().isValid() 
                      << " MS " << muon->muonSpectrometerTrackParticleLink().isValid() );
      continue;
    }
	
    // special case for STACO muons where the combined TP is not truth matched
    if( muon->muonType() == xAOD::Muon::Combined && !tp->track() ){
      ATH_MSG_VERBOSE("Resetting link for Combined muon ");
      tp = const_cast<xAOD::TrackParticle*>(*muon->inDetTrackParticleLink());
    }
    if( !tp ) continue;
	
    ATH_MSG_VERBOSE(" Got track link " << muon->primaryTrackParticleLink());
    if( tp ){
      ElementLink< xAOD::TruthParticleContainer > truthLink = tp->auxdata<ElementLink< xAOD::TruthParticleContainer > >("truthParticleLink");
      if( truthLink.isValid() ){
        if( (*truthLink)->barcode() == truthParticle.barcode() ) {
          ATH_MSG_VERBOSE(" Got truth link -> creating link with truth particle " << (*truthLink)->barcode() );
          muonLink = ElementLink< xAOD::MuonContainer >(muon,muons);
          break;
        }
      }else{
        ATH_MSG_VERBOSE("Muon has no truth association");
      }
    }      
  }
  muonLink.toPersistent();
  const_cast<xAOD::TruthParticle&>(truthParticle).auxdata<ElementLink< xAOD::MuonContainer > >("recoMuonLink") = muonLink;
}

// Finalize method:
StatusCode MuonTruthAssociationAlg::finalize() 
{
  return StatusCode::SUCCESS;
}

