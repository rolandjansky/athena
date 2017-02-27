///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// METMaker.cxx
// Implementation file for class METMaker
// Author: T.J.Khoo<khoo@cern.ch>
///////////////////////////////////////////////////////////////////

// METUtilities includes
#include "METUtilities/METMaker.h"

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
	  if(idtrack) {
	    // check if this is ghost associated to a jet
	    //make sure the jet is close first
	    if(xAOD::P4Helpers::deltaR2(*jet,*muon)>0.36) continue;
	    // std::cout << "METHelpers::addGhostMuonsToJets -- Muon/jet deltaR^2 = " << xAOD::P4Helpers::deltaR2(*jet,*muon) << std::endl;

	    for(unsigned jtrk=0; jtrk<jet_tracks.size(); ++jtrk) {
	      if(jet_tracks.at(jtrk)==idtrack) {
		//check if the track pointers match
		// std::cout << "METHelpers::addGhostMuonsToJets -- Muon/jet tracks match!" << std::endl;
		muons_in_jet.push_back(muon);
		break; 
	      }
	    } // loop over jet tracks

	  } // if muon has ID track
	} // loop over muons

      } // jet has associated tracks
       // std::cout << "METHelpers::addGhostMuonsToJets -- Jet has = " << muons_in_jet.size() << " ghost muons" << std::endl;
      jet->setAssociatedObjects( "GhostMuon", muons_in_jet) ;
    } // loop over jets

  } // end addGhostMuonsToJets(...)

  // void addGhostElecsToJets(const xAOD::ElectronContainer& elecs, xAOD::JetContainer& jets)
  // {
  //   std::vector<const xAOD::TrackParticle*> jet_tracks;
  //   for (const auto& jet: jets) {
  //     // Fill this with muons to be associated
  //     std::vector<const xAOD::Electron*> elecs_in_jet;
  //     // Get the tracks associated to the jet 
  //     jet_tracks.clear(); 
  //     if ( jet->getAssociatedObjects("GhostTrack", jet_tracks) ) {

  // 	for(const auto& elec : elecs) {
  // 	  bool eleMatch=false;
  // 	  for(size_t iTrk=0; iTrk<elec->nTrackParticles(); ++iTrk) {
  // 	    const xAOD::TrackParticle* idtrack = xAOD::EgammaHelpers::getOriginalTrackParticleFromGSF(elec->trackParticle(iTrk));
  // 	    if(idtrack) {
  // 	      // check if this is ghost associated to a jet
  // 	      //make sure the jet is close first
  // 	      if(xAOD::P4Helpers::deltaR2(*jet,*elec)>0.3) continue;
  // 	      for(unsigned jtrk=0; jtrk<jet_tracks.size(); ++jtrk) {
  // 		if(jet_tracks.at(jtrk)==idtrack) {
  // 		  //check if the track pointers match
  // 		  elecs_in_jet.push_back(elec);
  // 	      	  eleMatch=true;
  // 		  break; 
  // 		}
  // 	      } // loop over jet tracks
  // 	    } // if elec has ID track
  // 	    if(eleMatch) break;
  // 	  }
  // 	} // loop over elecs
  //     } // jet has associated tracks
  //     //std::cout << "METHelpers::addGhostElecsToJets -- Jet has = " << elecs_in_jet.size() << " ghost electrons" << std::endl;
  //     jet->setAssociatedObjects( "GhostElec", elecs_in_jet) ;
  //   } // loop over jets

  // } // end addGhostElecsToJets(...)

}
