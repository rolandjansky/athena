///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// METEgammaAssociator.cxx 
// Implementation file for class METEgammaAssociator
//
//  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//
// Author: P Loch, S Resconi, TJ Khoo, AS Mete
/////////////////////////////////////////////////////////////////// 

// METReconstruction includes
#include "METReconstruction/METEgammaAssociator.h"

// Egamma EDM
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/EgammaxAODHelpers.h"

// Tracking EDM
#include "xAODTracking/Vertex.h"

// DeltaR calculation
#include "FourMomUtils/xAODP4Helpers.h"

namespace met {

  using namespace xAOD;

  // Constructors
  ////////////////
  METEgammaAssociator::METEgammaAssociator(const std::string& name) : 
    AsgTool(name),
    METAssociator(name)
  {
    // 0 is delta-R geometrical matching
    // 1 is using TopoClusterLink decoration on clusters
    declareProperty( "TCMatchMethod",     m_tcMatch_method = DeltaR );

    declareProperty( "TCMatchMaxRat",     m_tcMatch_maxRat = 1.5    );
    declareProperty( "TCMatchDeltaR",     m_tcMatch_dR     = 0.1    );

    declareProperty( "ExtraTrackMatchDeltaR", m_extraTrkMatch_dR = 0.05 );
  }

  // Destructor
  ///////////////
  METEgammaAssociator::~METEgammaAssociator()
  {}

  // Athena algtool's Hooks
  ////////////////////////////
  StatusCode METEgammaAssociator::initialize()
  {
    ATH_CHECK( METAssociator::initialize() );
    ATH_MSG_VERBOSE ("Initializing " << name() << "...");
    switch(m_tcMatch_method) {
    case DeltaR: ATH_MSG_INFO("Egamma-topocluster matching uses DeltaR check."); break;
    case ClusterLink: ATH_MSG_INFO("Egamma-topocluster matching uses topocluster links."); break;
    default:
      ATH_MSG_WARNING( "Invalid topocluster match method configured!" );
      return StatusCode::FAILURE;
    } 
    return StatusCode::SUCCESS;
  }

  StatusCode METEgammaAssociator::finalize()
  {
    ATH_MSG_VERBOSE ("Finalizing " << name() << "...");
    return StatusCode::SUCCESS;
  }

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /////////////////////////////////////////////////////////////////// 
  // Protected methods: 
  /////////////////////////////////////////////////////////////////// 

  //**********************************************************************
  // Get Egamma constituents
  StatusCode METEgammaAssociator::extractTopoClusters(const xAOD::IParticle* obj,
						      std::vector<const xAOD::IParticle*>& tclist,
						      const met::METAssociator::ConstitHolder& constits) const
  {
    const Egamma *eg = static_cast<const Egamma*>(obj);
    // safe to assume a single SW cluster?
    // will do so for now...
    const CaloCluster* swclus = eg->caloCluster();

    // the matching strategy depends on how the cluster container is sorted
    // easier if it's sorted in descending pt order
    // we'll worry about optimisation later
    std::vector<const IParticle*> inputTC;
    inputTC.reserve(10);
    
    if(m_tcMatch_method==DeltaR) {
      for(const auto& cl : *constits.tcCont) {
	// this can probably be done more elegantly
	if(P4Helpers::isInDeltaR(*swclus,*cl,m_tcMatch_dR,m_useRapidity) && cl->e()>FLT_MIN) {
	  // could consider also requirements on the EM fraction or depth
	  inputTC.push_back(cl);
	} // match TC in a cone around SW cluster
      }
      ATH_MSG_VERBOSE("Found " << inputTC.size() << " nearby topoclusters");
      std::sort(inputTC.begin(),inputTC.end(),greaterPt);
    } else if(m_tcMatch_method==ClusterLink) {
      static const SG::AuxElement::ConstAccessor<std::vector<ElementLink<CaloClusterContainer> > > tcLinkAcc("constituentClusterLinks");
      // Fill a vector of vectors
      for(const auto& el : tcLinkAcc(*swclus)) {
	if (el.isValid()) {
          inputTC.push_back(*el);
        } else {
          ATH_MSG_VERBOSE("Found invalid topocluster link");
        }
      }
      ATH_MSG_VERBOSE("Found " << inputTC.size() << " linked topoclusters");
    } else {
      ATH_MSG_WARNING( "Invalid topocluster match method configured!" );
      return StatusCode::FAILURE;
    } 

    ATH_CHECK( selectEgammaClusters(swclus, inputTC, tclist) );

    return StatusCode::SUCCESS;
  }


  StatusCode METEgammaAssociator::extractTracks(const xAOD::IParticle* obj,
						std::vector<const xAOD::IParticle*>& constlist,
						const met::METAssociator::ConstitHolder& constits) const
  {
    const xAOD::Egamma *eg = static_cast<const xAOD::Egamma*>(obj);
    std::set<const xAOD::TrackParticle*> trackset; // use a set for duplicate-free retrieval
    ATH_CHECK( selectEgammaTracks(eg, constits.trkCont, trackset) );
    for(const auto& track : trackset) {
      if(acceptTrack(track,constits.pv) && isGoodEoverP(track)) {
	constlist.push_back(track);
      }
    }
    return StatusCode::SUCCESS;
  }

  StatusCode METEgammaAssociator::extractPFO(const xAOD::IParticle* obj,
					     std::vector<const xAOD::IParticle*>& pfolist,
					     const met::METAssociator::ConstitHolder& constits,
					     std::map<const IParticle*,MissingETBase::Types::constvec_t> &/*momenta*/) const
  {
    const xAOD::Egamma *eg = static_cast<const xAOD::Egamma*>(obj);
    // safe to assume a single SW cluster?
    // will do so for now...
    const xAOD::IParticle* swclus = eg->caloCluster();

    // Preselect PFOs based on proximity: dR<0.4
    std::vector<const xAOD::PFO*> nearbyPFO;
    nearbyPFO.reserve(20);
    for(const auto& pfo : *constits.pfoCont) {
      // Reject pfo's with negative or 0 energy
      // -- the latter includes cPFOs removed by charged hadron subtraction
      // and/or nPFOs removed by other constituent-based pileup-subtraction methods
      if(pfo->e() > FLT_MIN && P4Helpers::isInDeltaR(*pfo, *swclus, 0.4, m_useRapidity)) {
	if(fabs(pfo->charge())<FLT_MIN || ( !m_cleanChargedPFO || isGoodEoverP(pfo->track(0)) ) ) {
	  nearbyPFO.push_back(pfo);
	} // retain neutral PFOs and charged PFOs passing PV association
      } // DeltaR check
    } // PFO loop
    ATH_MSG_VERBOSE("Found " << nearbyPFO.size() << " nearby pfos");

    std::set<const xAOD::TrackParticle*> trackset; // use a set for duplicate-free retrieval
    ATH_CHECK( selectEgammaTracks(eg, constits.trkCont, trackset) );
    for(const auto& track : trackset) {
      for(const auto& pfo : nearbyPFO) {
	if(fabs(pfo->charge())>FLT_MIN && pfo->track(0) == track) {
	  pfolist.push_back(pfo);
	} // PFO/track match
      } // PFO loop
    } // Track loop
    double eg_cl_e = swclus->e();

    // the matching strategy depends on how the cluster container is sorted
    // easier if it's sorted in descending pt order
    // ideally this should be done using cell matching, but we can't use the links from topoclusters reliably
    // because some PFOs don't correspond to the original TC
    bool doSum = true;
    double sumE_pfo = 0.;
    const IParticle* bestbadmatch = 0;
    std::sort(nearbyPFO.begin(),nearbyPFO.end(),greaterPtPFO);
    TLorentzVector momentum;
    for(const auto& pfo : nearbyPFO) {
      // Skip charged PFOs, as we already matched them
      if(fabs(pfo->charge())>FLT_MIN || !P4Helpers::isInDeltaR(*pfo, *swclus, m_tcMatch_dR, m_useRapidity)) {continue;}
      // Handle neutral PFOs like topoclusters
      double pfo_e = pfo->eEM();
      // skip cluster if it's above our bad match threshold or outside the matching radius
      if(pfo_e>m_tcMatch_maxRat*eg_cl_e) {
        ATH_MSG_VERBOSE("Reject topocluster in sum. Ratio vs eg cluster: " << (pfo_e/eg_cl_e));
    	if( !bestbadmatch || (fabs(pfo_e/eg_cl_e-1.) < fabs(bestbadmatch->e()/eg_cl_e-1.)) ) bestbadmatch = pfo;
	continue;
      }

      ATH_MSG_VERBOSE("E match with new nPFO: " << fabs(sumE_pfo+pfo_e - eg_cl_e) / eg_cl_e);
      if( (doSum = fabs(sumE_pfo+pfo_e-eg_cl_e) < fabs(sumE_pfo - eg_cl_e)) ) {
	pfolist.push_back(pfo);
	sumE_pfo += pfo_e;
    	ATH_MSG_VERBOSE("Accept pfo with pt " << pfo->pt() << ", e " << pfo->e() << " in sum.");
    	ATH_MSG_VERBOSE("E match with new PFO: " << fabs(sumE_pfo+pfo_e - eg_cl_e) / eg_cl_e);
    	ATH_MSG_VERBOSE("Energy ratio of TC to eg: " << pfo_e / eg_cl_e);
      } // if we will retain the topocluster
      else {break;}
    } // loop over nearby clusters
    if(sumE_pfo<FLT_MIN && bestbadmatch) {
      ATH_MSG_VERBOSE("No better matches found -- add bad match topocluster with pt "
    		      << bestbadmatch->pt() << ", e " << bestbadmatch->e() << ".");
      pfolist.push_back(bestbadmatch);
    }

    return StatusCode::SUCCESS;
  }

  //**********************************************************************
  // Select Egamma tracks & clusters

  StatusCode METEgammaAssociator::selectEgammaClusters(const xAOD::CaloCluster* swclus,
						       const std::vector<const IParticle*>& inputTC,
						       std::vector<const xAOD::IParticle*>& tclist) const
  {    
    double eg_cl_e = swclus->e();

    bool doSum = true;
    double sumE_tc = 0.;
    const IParticle* bestbadmatch = 0;
    for(const auto& cl : inputTC) {
      double tcl_e = cl->e();
      // skip cluster if it's above our bad match threshold
      // retain pointer of the closest matching cluster in case no better is found
      if(tcl_e>m_tcMatch_maxRat*eg_cl_e) {
    	ATH_MSG_VERBOSE("Reject topocluster in sum. Ratio vs eg cluster: " << (tcl_e/eg_cl_e));
    	if( !bestbadmatch || (fabs(tcl_e/eg_cl_e-1.) < fabs(bestbadmatch->e()/eg_cl_e-1.)) ) bestbadmatch = cl;
    	continue;
      }

      ATH_MSG_VERBOSE("E match with new cluster: " << fabs(sumE_tc+tcl_e - eg_cl_e) / eg_cl_e);
      if( (doSum = (fabs(sumE_tc+tcl_e - eg_cl_e) < fabs(sumE_tc - eg_cl_e))) ) {
    	ATH_MSG_VERBOSE("Accept topocluster with pt " << cl->pt() << ", e " << cl->e() << " in sum.");
    	ATH_MSG_VERBOSE("E match with new cluster: " << fabs(sumE_tc+tcl_e - eg_cl_e) / eg_cl_e);
    	ATH_MSG_VERBOSE("Energy ratio of TC to eg: " << tcl_e / eg_cl_e);
    	tclist.push_back(cl);
    	sumE_tc += tcl_e;
      } // if we will retain the topocluster
    } // loop over nearby clusters
    if(sumE_tc<FLT_MIN && bestbadmatch) {
      ATH_MSG_VERBOSE("No better matches found -- add bad match topocluster with pt "
    		      << bestbadmatch->pt() << ", e " << bestbadmatch->e() << ".");
      tclist.push_back(bestbadmatch);
    }
    return StatusCode::SUCCESS;
  }

  StatusCode METEgammaAssociator::selectEgammaTracks(const xAOD::Egamma* eg,
						     const xAOD::TrackParticleContainer* trkCont,
						     std::set<const xAOD::TrackParticle*>& tracklist) const
  {
    // switch to using egamma helpers for track extraction
    // set ensures that there's no duplication
    const std::set<const xAOD::TrackParticle*> egtracks = EgammaHelpers::getTrackParticles(eg);
    for(const auto& track : egtracks) {
	ATH_MSG_VERBOSE("Accept " << eg->type() << " track " << track << " px, py = " << track->p4().Px() << ", " << track->p4().Py());
	tracklist.insert(track);
    } // end initial egamma track loop

    // for objects with ambiguous author, grab the tracks matched to the counterpart ambiguous object too
    // set ensures that there's no duplication
    if (eg->author() & xAOD::EgammaParameters::AuthorAmbiguous && eg->ambiguousObject()) {
      const std::set<const xAOD::TrackParticle*> ambitracks = EgammaHelpers::getTrackParticles(eg->ambiguousObject());
      for(const auto& track : egtracks) {
	ATH_MSG_VERBOSE("Accept ambiguous " << eg->type() << " track " << track << " px, py = " << track->p4().Px() << ", " << track->p4().Py());
	tracklist.insert(track);
      }
    } // end ambiguous track case

    // in a small dR window, also accept tracks without an IBL hit
    for(const auto& track : *trkCont) {
      if(P4Helpers::isInDeltaR(*track, *eg, m_extraTrkMatch_dR, m_useRapidity)) {
	// dR check should be faster than track summary retrieval
	uint8_t expect_innermostHit(false);
	uint8_t N_innermostHit(false);
	uint8_t expect_nextToInnermostHit(false);
	uint8_t N_nextToInnermostHit(false);
	if( !track->summaryValue(expect_innermostHit, expectInnermostPixelLayerHit)
	    || !track->summaryValue(expect_nextToInnermostHit, expectNextToInnermostPixelLayerHit)) {
	  ATH_MSG_WARNING("Track summary retrieval failed for 'expect(NextTo)InnermostPixelLayerHit'");
	  return StatusCode::FAILURE;
	}
	if(expect_innermostHit) {
	  if( !track->summaryValue(N_innermostHit, numberOfInnermostPixelLayerHits) ) {
	    ATH_MSG_WARNING("Track summary retrieval failed for 'numberOfInnermostPixelLayerHits'");
	    return StatusCode::FAILURE;
	    if(N_innermostHit==0 ) {
	      ATH_MSG_VERBOSE("Accept nearby track w/o innermost hit");
	      tracklist.insert(track);
	    }
	  }
	} else if(expect_nextToInnermostHit) {
	  if( !track->summaryValue(N_nextToInnermostHit, numberOfNextToInnermostPixelLayerHits) ) {
	    ATH_MSG_WARNING("Track summary retrieval failed for 'numberOfNextToInnermostPixelLayerHits'");
	    return StatusCode::FAILURE;
	    if(N_nextToInnermostHit==0 ) {
	      ATH_MSG_VERBOSE("Accept nearby track w/o next-to-innermost hit");
	      tracklist.insert(track);
	    }
	  }
	}
	
      } // end dR check
    } // end extra track loop
    return StatusCode::SUCCESS;
  }

}
