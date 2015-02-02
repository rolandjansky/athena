///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// METTauAssociator.cxx 
// Implementation file for class METTauAssociator
//
//  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//
// Author: P Loch, S Resconi, TJ Khoo, AS Mete
/////////////////////////////////////////////////////////////////// 

// METReconstruction includes
#include "METReconstruction/METTauAssociator.h"

// MET EDM
#include "xAODMissingET/MissingETComposition.h"

// Tau EDM
#include "xAODTau/TauJetContainer.h"

// Tracking EDM
#include "xAODTracking/Vertex.h"

namespace met {

  using namespace xAOD;

  // Constructors
  ////////////////
  METTauAssociator::METTauAssociator(const std::string& name) : 
    AsgTool(name),
    METAssociator(name)
  {}

  // Destructor
  ///////////////
  METTauAssociator::~METTauAssociator()
  {}

  // Athena algtool's Hooks
  ////////////////////////////
  StatusCode METTauAssociator::initialize()
  {
    ATH_MSG_VERBOSE ("Initializing " << name() << "...");
    return StatusCode::SUCCESS;
  }

  StatusCode METTauAssociator::finalize()
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

  // executeTool
  ////////////////
  StatusCode METTauAssociator::executeTool(xAOD::MissingETContainer* /*metCont*/, xAOD::MissingETAssociationMap* metMap) 
  {
    ATH_MSG_VERBOSE ("In execute: " << name() << "...");

    const TauJetContainer* tauCont(0);
    if( evtStore()->retrieve(tauCont, m_input_data_key).isFailure() ) {
      ATH_MSG_WARNING("Unable to retrieve input electron container " << m_input_data_key);
      return StatusCode::FAILURE;
    }

    ATH_MSG_DEBUG("Successfully retrieved electron collection");
    if (fillAssocMap(metMap,tauCont).isFailure()) {
      ATH_MSG_WARNING("Unable to fill map with tau container " << m_input_data_key);
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  }


  //*********************************************************************************************************
  // Get tau constituents
  StatusCode METTauAssociator::extractTopoClusters(const xAOD::IParticle *obj,
						   std::vector<const xAOD::IParticle*>& tclist,
				        	   const xAOD::CaloClusterContainer* /*tcCont*/)
  {  
    const TauJet* tau = static_cast<const TauJet*>(obj);
    const Jet* seedjet = *tau->jetLink();
    JetConstituentVector constit = seedjet->getConstituents();
    ATH_MSG_VERBOSE("Current tau has " << constit.size() << " constituents.");
    // test for used topoclusters, and retrieve unused ones (ok until/unless we use PFlow taus)
    // only use clusters for computing the overlap removal relative to other objects
    for(const auto& cl : constit) {
      // TEMP: use jet seed axis
      //       taus will provide an accessor
      double dR = seedjet->p4().DeltaR(cl->rawConstituent()->p4());
      if(dR>0.2) continue;
      // skip cluster if dR>0.2
      if(cl->rawConstituent()->type() == xAOD::Type::CaloCluster) {
	const CaloCluster* pClus = static_cast<const CaloCluster*>( cl->rawConstituent() );
	tclist.push_back(pClus);
      } else {
	ATH_MSG_WARNING("Expected an object of type CaloCluster, received one of type " << cl->rawConstituent()->type());
      }
    } // loop over jet constituents
    return StatusCode::SUCCESS;
  }


  StatusCode METTauAssociator::extractTracks(const xAOD::IParticle *obj,
					     std::vector<const xAOD::IParticle*>& constlist,
					     const xAOD::CaloClusterContainer* /*tcCont*/,
					     const xAOD::Vertex* pv)
  {
    const TauJet* tau = static_cast<const TauJet*>(obj);
    const Jet* jet = *tau->jetLink();
    for(size_t iTrk=0; iTrk<tau->nTracks(); ++iTrk) {
      const TrackParticle* tautrk = tau->track(iTrk);
      // if(acceptTrack(tautrk,pv) && isGoodEoverP(tautrk,tcCont)) {
      if(acceptTrack(tautrk,pv)) {
	// bool matchedmu = false;
	// for(const auto& mutrk : mutracks) {
	//   if( (matchedmu = (tautrk == mutrk)) ) {
	//     ATH_MSG_VERBOSE("Veto track matched to muon");
	// 	break;
	//   }
	// }
	// if(!matchedmu) {
	ATH_MSG_VERBOSE("Accept tau track " << tautrk << " px, py = " << tautrk->p4().Px() << ", " << tautrk->p4().Py());
	constlist.push_back(tautrk);
	// }
      }
    }
    for(size_t iTrk=0; iTrk<tau->nOtherTracks(); ++iTrk) {
      const TrackParticle* tautrk = tau->otherTrack(iTrk);
      double dR = jet->p4().DeltaR(tautrk->p4());
      //if(dR<0.2 && acceptTrack(tautrk,pv) && isGoodEoverP(tautrk,tcCont)) {
      if(dR<0.2 && acceptTrack(tautrk,pv)) {
	// bool matchedmu = false;
	// for(const auto& mutrk : mutracks) {
	//   if( (matchedmu = (tautrk == mutrk)) ) {
	//     ATH_MSG_VERBOSE("Veto track matched to muon");
	// 	break;
	//   }
	// }
	// if(!matchedmu) {
	ATH_MSG_VERBOSE("Accept track " << tautrk << " px, py = " << tautrk->p4().Px() << ", " << tautrk->p4().Py());
	constlist.push_back(tautrk);
	// }
      }
    }
    return StatusCode::SUCCESS;
  }
  //*********************************************************************************************************
  // Get tau constituents
  StatusCode METTauAssociator::extractPFO(const xAOD::IParticle* obj,
					  std::vector<const xAOD::IParticle*>& pfolist,
					  const xAOD::PFOContainer* pfoCont,
					  std::map<const IParticle*,MissingETBase::Types::constvec_t> &momenta,
					  const xAOD::Vertex* pv)
  {  
    const TauJet* tau = static_cast<const TauJet*>(obj);
    const Jet* seedjet = *tau->jetLink();
    for(const auto& pfo : *pfoCont) {
      bool match = false;
      if (pfo->charge()==0) {
	if(seedjet->p4().DeltaR(pfo->p4EM())<0.2 && pfo->eEM()>0) {
	match = true;
	}
      } else {
	for(size_t iTrk=0; iTrk<tau->nTracks(); ++iTrk) {
	  const TrackParticle* tautrk = tau->track(iTrk);
	  if(tautrk==pfo->track(0)) {
	    if(acceptChargedPFO(tautrk,pv)) match = true; 
	  }
	}
	for(size_t iTrk=0; iTrk<tau->nOtherTracks(); ++iTrk) {
	  const TrackParticle* tautrk = tau->otherTrack(iTrk);
	  if(tautrk==pfo->track(0)) {
	    double dR = seedjet->p4().DeltaR(tautrk->p4());
	    if(dR<0.2 && acceptChargedPFO(tautrk,pv)) match = true;
	  }
	}
      }
      if(match) {
	pfolist.push_back(pfo);
	if(pfo->charge()==0) {
	  TLorentzVector momentum = pfo->GetVertexCorrectedEMFourVec(*pv);
	  momenta[pfo] = MissingETBase::Types::constvec_t(momentum.Px(),momentum.Py(),momentum.Pz(),
						     momentum.E(),momentum.Pt());
	}
      }
    }
    return StatusCode::SUCCESS;
  }

}
