///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// METHelpers.cxx
// Implementation file for standalone METHelpers functions
// Author: T.J.Khoo<khoo@cern.ch>
// Author: Bill Balunas <bill.balunas@cern.ch>
///////////////////////////////////////////////////////////////////

// METUtilities includes
#include "METUtilities/METHelpers.h"

// Muon EDM
#include "xAODMuon/MuonContainer.h"

// Electron EDM
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/EgammaxAODHelpers.h"

// Jet EDM
#include "xAODJet/JetContainer.h"

#include "FourMomUtils/xAODP4Helpers.h"

#include <iostream>

namespace met {

  ANA_MSG_SOURCE (msgMET, "METUtilities")

  void addGhostMuonsToJets(const xAOD::MuonContainer& muons, xAOD::JetContainer& jets)
  {
    std::vector<const xAOD::TrackParticle*> jet_tracks;
    for (const auto& jet: jets) {
      // Fill this with muons to be associated
      std::vector<const xAOD::Muon*> muons_in_jet;
      // Get the tracks associated to the jet 
      jet_tracks.clear(); 
      if ( jet->getAssociatedObjects("GhostTrack", jet_tracks) ) {

        for(const auto& muon : muons) {
          const xAOD::TrackParticle* idtrack = muon->trackParticle( xAOD::Muon::InnerDetectorTrackParticle );
          if(!idtrack) continue;
          // check if this is ghost associated to a jet
          // make sure the jet is close first
          if(xAOD::P4Helpers::deltaR2(*jet,*muon)>0.36) continue;

          for(unsigned jtrk=0; jtrk<jet_tracks.size(); ++jtrk) {
            // check if the track pointers match
            if(jet_tracks.at(jtrk)==idtrack) {
              muons_in_jet.push_back(muon);
              break; 
            }
          } // loop over jet tracks
        } // loop over muons
      } // jet has associated tracks
      jet->setAssociatedObjects( "GhostMuon", muons_in_jet) ;
    } // loop over jets
  }

  StatusCode buildMETSum(const std::string& totalName,
                         xAOD::MissingETContainer* metCont)
  {
    using namespace msgMET;
    ANA_MSG_DEBUG("Build MET total: " << totalName);
 
    xAOD::MissingET* metFinal = nullptr;
    if(fillMET(metFinal, metCont, totalName, MissingETBase::Source::total()) != StatusCode::SUCCESS){
      ANA_MSG_ERROR("failed to fill MET term");
      return StatusCode::FAILURE;
    }
 
    for(xAOD::MissingETContainer::const_iterator iMET=metCont->begin(); iMET!=metCont->end(); ++iMET) {
      if(*iMET==metFinal) continue;
      *metFinal += **iMET;
    }
 
    ANA_MSG_DEBUG( "Rebuilt MET Final --"
                  << " mpx: " << metFinal->mpx()
                  << " mpy: " << metFinal->mpy()
                 );
 
    return StatusCode::SUCCESS;
  }

  StatusCode buildMETSum(const std::string& totalName,
                         xAOD::MissingETContainer* metCont,
                         MissingETBase::Types::bitmask_t softTermsSource)
  {
    using namespace msgMET;
    ANA_MSG_DEBUG("Build MET total: " << totalName);

    xAOD::MissingET* metFinal = nullptr;
    if( fillMET(metFinal, metCont, totalName, MissingETBase::Source::total()) != StatusCode::SUCCESS) {
      ANA_MSG_ERROR("failed to fill MET term \"" << totalName << "\"");
      return StatusCode::FAILURE;
    }

    for(const auto& met : *metCont) {
      if(MissingETBase::Source::isTotalTerm(met->source())) continue;
      if(met->source()==invisSource) continue;
      if(softTermsSource && MissingETBase::Source::isSoftTerm(met->source())) {
        if(!MissingETBase::Source::hasPattern(met->source(),softTermsSource)) continue;
      }
      ANA_MSG_VERBOSE("Add MET term " << met->name() );
      *metFinal += *met;
    }

    ANA_MSG_DEBUG( "Rebuilt MET Final --"
                  << " mpx: " << metFinal->mpx()
                  << " mpy: " << metFinal->mpy()
                  );

    return StatusCode::SUCCESS;
  }

  // This is used to avoid creating a private store
  // It puts the given new MET object into the container
  StatusCode fillMET(xAOD::MissingET *& met,
                      xAOD::MissingETContainer * metCont,
                      const std::string& metKey,
                      const MissingETBase::Types::bitmask_t metSource){
    using namespace msgMET;
    if(met != nullptr){
      ANA_MSG_ERROR("You can't fill a filled MET value");
      return StatusCode::FAILURE;
    }
    if(metCont->find(metKey)!=metCont->end()){
      ANA_MSG_ERROR("Attempted to add a duplicate MET term: " << metKey);
      return StatusCode::FAILURE;
    }
    
    met = new xAOD::MissingET();
    metCont->push_back(met);

    met->setName  (metKey);
    met->setSource(metSource);

    return StatusCode::SUCCESS;
  }
    
}
