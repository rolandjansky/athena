/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/*
  Author: Candice Basson (candice.basson@cern.ch)
  Based on code from CxAOD:
  https://gitlab.cern.ch/CxAODFramework/CxAODMaker_FlavourTagging/blob/master/Root/JetHandler_FlavourTagging.cxx
==============================
 NOTE
 This tool was made for the LF calib team in FTAG1. Be cautious of definitions here if using different samples 
 as definitions heavily depend on what truth info your sample has stored. For LF Calib defining PU as no valid
 TruthLink (NTruthLink) was deemed sufficient but may not be for you.
==============================
*/

#include "BTagLFJetAugmenter.h"

#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthVertex.h"
#include "xAODTruth/TruthEventContainer.h"

#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"

#include "xAODBTagging/BTagging.h"
#include "xAODJet/JetContainer.h"

BTagLFJetAugmenter::BTagLFJetAugmenter(
  const std::string& name, ISvcLocator* loc):
  AthAlgorithm(name, loc),
  m_jet_collection(),
  m_dec_nConversion("nConversion"),
  m_dec_nKs("nK_short"),
  m_dec_nLambda("nLambda"),
  m_dec_nshadron("ns_hadron"),
  m_dec_Conversion("Conversion"),
  m_dec_Ks("K_short"),
  m_dec_Lambda("Lambda"),
  m_dec_shadron("s_hadron"),
  m_dec_HadMatInt("MatInt"),  
  m_dec_FakeHS("FakeHS"),
  m_dec_HS("HS"),
  m_dec_NTruthLink("NTruthLink")

{
  declareProperty("JetCollectionName", m_jet_collection);
}

// Define vector for link to track particles
typedef ElementLink<xAOD::TrackParticleContainer> TrackLink;
typedef std::vector<TrackLink> TrackLinks;

StatusCode BTagLFJetAugmenter::execute() {
  
  const xAOD::JetContainer *jets = nullptr;
  CHECK( evtStore()->retrieve(jets, m_jet_collection) );
  for (const auto* jet: *jets) {
    // Parent barcode vector to prevent double counting
    // second vector to be sure Ks and Lambda are included in inclusive s-hadron category
    std::vector<int> parent_barcode;
    std::vector<int> shadron_barcode;

    // Define tracks from different low level b-tagging algos
    TrackLinks SV1Tracks;
    TrackLinks JFTracks;
    TrackLinks IP3DTracks;
    //set counters to zero for track loop
    int numPhotonCon = 0;
    int numKshort = 0;
    int numLambda = 0;
    int numStrange = 0;
    int numPhotonConTracks = 0;
    int numKshortTracks = 0;
    int numLambdaTracks = 0;
    int numStrangeTracks = 0;
    int numHadMatInt = 0;
    int numFakeHS =0;
    int numHS = 0;
    int numNTruthLink = 0;
   
    int particle_id = 999;

    //CHECKS
    bool hasConversion =false;
    bool hasStrange = false;
    bool DecayInFlight = false; 
    bool hasHadMatInt = false;
    
    // Access b-tagging object 
    const xAOD::BTagging * btag = jet->btagging();
    if (!btag) {
      ATH_MSG_ERROR("Track jet has no b-tagging pointer");
      // No track should be missing this object
      return StatusCode::FAILURE;
    }
    // Access b-tagged tracks from trackers
    SV1Tracks = btag->SV1_TrackParticleLinks();
    IP3DTracks = btag->auxdata<TrackLinks >("IP3D_TrackParticleLinks");
    // track associated with those vertices
    std::vector<ElementLink<xAOD::BTagVertexContainer> > jfvertices;
    jfvertices =  btag->auxdata<std::vector<ElementLink<xAOD::BTagVertexContainer> > >("JetFitter_JFvertices");
    
    // Create new track container for all tagger tracks
    // std::set will remove duplicates via compare
    std::set< const xAOD::TrackParticle* > trackParts;
    for (auto const &link: IP3DTracks) {
      if (link.isValid()) {
	const xAOD::TrackParticle* track = *link;
	if (!track) continue;
	// add to new track container
	trackParts.insert(track);
      }
    }
    for (auto const &link: SV1Tracks) {
      if (link.isValid()) { 
	const xAOD::TrackParticle* track = *link;
	if (!track) continue;
	// add to new track container
	trackParts.insert(track);
      }
    } 
    for (unsigned int jfv = 0; jfv < jfvertices.size(); jfv++) {
      if (jfvertices.at(jfv).isValid()) {
	const xAOD::BTagVertex *tmpVertex = *(jfvertices.at(jfv));
	const std::vector< ElementLink<xAOD::TrackParticleContainer> > tmpVect = tmpVertex->track_links(); // mod Remco
	JFTracks.insert(JFTracks.end(), tmpVect.begin(), tmpVect.end()); // mod Remco
	// Access JF Ttack particles
	for (auto const &link: JFTracks) {
	  if (link.isValid()) {
	    const xAOD::TrackParticle* track = *link;
	    if (!track) continue;
	    //add to new track container
	    trackParts.insert(track);	  
	  }
	} // end JFTrack 
      }
    } // end jfvertices

    // Access truth information for IP3D, SV1 and JF Tracks in new container
    for (const xAOD::TrackParticle * track : trackParts) {      
      // check validity
      if (!track) continue;
      typedef ElementLink< xAOD::TruthParticleContainer > Link_t;
      static const char* NAME = "truthParticleLink";
      if ( ! track->isAvailable< Link_t >( NAME ) ) continue;
      const Link_t& link = track->auxdata< Link_t >( NAME );
      // check for associated truth particle
      if ( ! link.isValid() ) {
	// PU is defined as 'no truthlink' for most samples
	numNTruthLink++;
	continue;
      }
      const xAOD::TruthParticle * truth = *link;

      // truth match probability
      // Less than 0.5 used to be def. for fakes
      static SG::AuxElement::Accessor<float> truthMatchProbAcc("truthMatchProbability");
      if (truthMatchProbAcc.isAvailable(*track)) {
	if (truthMatchProbAcc(*track) < 0.5 ) { 
	  numFakeHS++; 
	} else if (truthMatchProbAcc(*track) > 0.5) {
	  numHS++;
	}
      }
 
      particle_id = truth->pdgId();

      //-------------------------------
      // Photon Conversion
      //-------------------------------
      
      // bool to check for double counts
      bool isLogged = false;

      // Check for one e-/e+ and then exactly one parent 
      if (abs(particle_id) == 11 && truth->nParents() == 1 ) {
	const xAOD::TruthParticle * parent = truth;
	parent = truth->parent();
	// parent needs to be a photon
	if ( parent->pdgId() == 22){
	  hasConversion = true; 
	  numPhotonConTracks++;
	  // loop to check if we've found this photon before -> prevent double counting if both e+/e- daughters are seen
	  for (unsigned int i = 0; i < parent_barcode.size(); i++) {
	    if (parent_barcode[i] == parent->barcode())  isLogged = true;
	  } 
	  // if not found before, add to vector
	  if (! isLogged ) { numPhotonCon++;
	    parent_barcode.push_back(parent->barcode());
	  }
	}
      } // conversion

      //--------------------------------
      // s-hadrons
      //--------------------------------
      
      //---K-short--//
      // Ks decays to pi+pi- 69.20% and pi0pi0 30.69%
      // bool to check for double counts
      bool isKsLogged = false;

      // abs as -ve is antiparticle
      if (abs(particle_id) == 211) {
	const xAOD::TruthParticle * parent = truth;
        parent = truth->parent();
	for (size_t i = 0; i < truth->nParents(); ++i) {
	  // check parent is Ks
	  if (truth->parent(i) && truth->parent(i)->pdgId() == 310) {
	    numKshortTracks++;
	    // check if if this Ks has been counted before - ie if both daughter pi are seen
	    for (unsigned int i = 0; i < parent_barcode.size(); i++) {
	      if (parent_barcode[i] == parent->barcode())  isKsLogged = true;
	    }
	    // if not counted before, add parent barcode to vector
	    if (! isKsLogged ) { numKshort++;
	      parent_barcode.push_back(parent->barcode());
	    }
	  }
	}
      } // Ks

      //---Lambda---//
      // Lambda decays to p(pi-) 63.9% and n(pi0) 35.8%      
      // bool to check for double count
      bool isLambdaLogged = false;
      
      // check for proton or  pi- 
      if ( particle_id == 2212 || particle_id == -211 ) {
	const xAOD::TruthParticle * parent = truth;
        parent = truth->parent();
	for (size_t i = 0; i < truth->nParents(); ++i) {
	  // check parent is Lambda
	  if (truth->parent(i) && truth->parent(i)->pdgId() == 3122) {
	    numLambdaTracks++;
	    // check if parent was counted before - ie if both daughter pi and proton were seen
	    for (unsigned int i = 0; i < parent_barcode.size(); i++) {
	      if (parent_barcode[i] == parent->barcode())  isLambdaLogged = true;
	    }
	    // if not counted before, add parent barcode to vector
	    if (! isLambdaLogged ) { numLambda++;
	      parent_barcode.push_back(parent->barcode());
	    }
	  }
	}
      } // Lambda

      //---s-hadrons---//
      // inclusive - ie includes Ks and Lambda
      // bool to check double count
      bool isHadronLogged = false;

      for (size_t i = 0; i < truth->nParents(); ++i) {
	const xAOD::TruthParticle * parent = truth;
        parent = truth->parent();
	int pdg = abs(parent->pdgId());
	//Kshort/ Lambda{0}/ Sigma{0}/ Sigma{+}/ Sigma{-}/ Xi{0}/ Xi{-}/ Omega{-}/ Klong
	if (pdg ==310 || pdg == 3122 || pdg == 3212 || pdg == 3222 || pdg == 3112 || pdg == 3322 || pdg == 3312 || pdg == 3334 || pdg == 130) {
	  hasStrange = true;
	  numStrangeTracks++;
	  // check if parent was counted before
	  for (unsigned int i = 0; i < shadron_barcode.size(); i++) {
	    if (shadron_barcode[i] == parent->barcode())  isHadronLogged = true;
	  }
	  // if not counted before, add parent barcode to vector
	  if (! isHadronLogged ) { numStrange++;
	    shadron_barcode.push_back(parent->barcode());
	  }
	}
      }
    
      //--------------------------------
      // Hadronic Material Interactions
      //--------------------------------

      if (!hasHadMatInt) {
	if (truth->barcode() > 200000) {
	  if (hasConversion || hasStrange) {
	    // check in flight decay
	    const xAOD::TruthVertex * prodVtx = truth->prodVtx();
	    if (!prodVtx) {
	      ATH_MSG_WARNING("No production vertex found in truth");
	      // May only occur on some tracks. If this is missing affects DecayInFlight output but not augmentation
	      continue;
	    }
	    TLorentzVector v_out;
	    for (size_t iout = 0; iout < prodVtx->nOutgoingParticles(); iout++) {
	      auto *p = prodVtx->outgoingParticle( iout );
	      if ( ! p ) continue;
	      auto& v = p->p4();
	      v_out += v;
	    }    
	    TLorentzVector v_in;
	    for( size_t iin = 0; iin < prodVtx->nIncomingParticles(); iin++) {
	      auto *p = prodVtx->incomingParticle( iin );
	      if ( ! p ) continue;
	      auto& v = p->p4();
	      v_in += v;
	    }
	    double E_Balance = v_out.E() - v_in.E();
	    if (E_Balance < 100.) DecayInFlight = true;
	  }

	  // check if any other classification is true, if not - HadMatInt
	  if (!hasConversion && !hasStrange && !DecayInFlight) hasHadMatInt = true;
	  numHadMatInt++;
	}  
      } //HadMatInt
    } // end trackParts cont.

    // Augment jets with N particles             
    m_dec_nConversion(*jet) = numPhotonCon;    
    m_dec_nKs(*jet) = numKshort;
    m_dec_nLambda(*jet) = numLambda;
    m_dec_nshadron(*jet) = numStrange;
    // Augments jets with N tracks
    m_dec_Conversion(*jet) = numPhotonConTracks;
    m_dec_Ks(*jet) = numKshortTracks;
    m_dec_Lambda(*jet) = numLambdaTracks;
    m_dec_shadron(*jet) = numStrangeTracks;
    m_dec_HadMatInt(*jet) = numHadMatInt; 
    m_dec_FakeHS(*jet) = numFakeHS;
    m_dec_HS(*jet) = numHS;
    m_dec_NTruthLink(*jet) = numNTruthLink;

    if (msgLvl(MSG::VERBOSE)) {
      // Counters just for debug purpose 
      if (numPhotonCon != 0) {
	msg() << "No. of photon conversions: " << numPhotonCon << endmsg;    
      }
      if (numKshort != 0) {
	msg() << "No. of Kshort: " << numKshort << endmsg;
      }
      if (numLambda != 0) {
	msg() << "No. of lambda: " << numLambda << endmsg;
      }
      if (numStrange != 0) {
	msg() << "No. of s-hadrons: " << numStrange << endmsg;
      }
      if (numPhotonConTracks != 0) {
	msg() << "No. of photon conversion tracks: " << numPhotonConTracks << endmsg;
      }
      if (numKshortTracks != 0) {
	msg() << "No. of Kshort tracks: " << numKshortTracks << endmsg;
      }
      if (numLambdaTracks != 0) {
	msg() << "No. of lambda tracks: " << numLambdaTracks << endmsg;
      }
      if (numStrangeTracks != 0) {
	msg() << "No. of s-hadrons tracks: " << numStrangeTracks << endmsg;
      }
      if (numHadMatInt != 0) {
	msg() << "No. of material interaction tracks: " << numHadMatInt << endmsg;
      }
      if (numFakeHS != 0) {
	msg() << "No. of hard-scatter fake tracks: " << numFakeHS << endmsg;
      }
      if (numHS != 0) {
	msg() << "No. of hard-scatter tracks: " << numHS << endmsg;
      }
      if (numNTruthLink != 0) {
	msg() << "No. of tracks with no TruthLink (PU in some defs): " << numNTruthLink << endmsg;
      }
    }
    //clear vector
    parent_barcode.clear();
    shadron_barcode.clear();
  } //end jets loop
  
  return StatusCode::SUCCESS;
}
