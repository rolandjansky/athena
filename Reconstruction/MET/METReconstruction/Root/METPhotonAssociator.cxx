///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// METPhotonAssociator.cxx
// Implementation file for class METPhotonAssociator
//
//  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//
// Author: P Loch, S Resconi, TJ Khoo, AS Mete
///////////////////////////////////////////////////////////////////

// METReconstruction includes
#include "METReconstruction/METPhotonAssociator.h"

// Egamma EDM
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/EgammaxAODHelpers.h"

namespace met {

  using namespace xAOD;

  // Constructors
  ////////////////
  METPhotonAssociator::METPhotonAssociator(const std::string& name) :
    AsgTool(name),
    METAssociator(name),
    METEgammaAssociator(name)
  {}

  // Destructor
  ///////////////
  METPhotonAssociator::~METPhotonAssociator()
  {}

  // Athena algtool's Hooks
  ////////////////////////////
  StatusCode METPhotonAssociator::initialize()
  {
    ATH_CHECK( METEgammaAssociator::initialize() );
    ATH_MSG_VERBOSE ("Initializing " << name() << "...");
    return StatusCode::SUCCESS;
  }

  StatusCode METPhotonAssociator::finalize()
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
  StatusCode METPhotonAssociator::executeTool(xAOD::MissingETContainer* /*metCont*/, xAOD::MissingETAssociationMap* metMap)
  {
    ATH_MSG_VERBOSE ("In execute: " << name() << "...");

    const xAOD::PhotonContainer* phCont(0);
    if( evtStore()->retrieve(phCont, m_input_data_key).isFailure() ) {
      ATH_MSG_WARNING("Unable to retrieve input photon container " << m_input_data_key);
      return StatusCode::FAILURE;
    }

    ATH_MSG_DEBUG("Successfully retrieved photon collection");

    if (fillAssocMap(metMap,phCont).isFailure()) {
      ATH_MSG_WARNING("Unable to fill map with photon container " << m_input_data_key);
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  }

  StatusCode METPhotonAssociator::extractTracks(const xAOD::IParticle* obj,
						std::vector<const xAOD::IParticle*>& constlist,
						const xAOD::IParticleContainer* tcCont,
					        const xAOD::Vertex* pv) const
  {
    const xAOD::Photon *ph = static_cast<const xAOD::Photon*>(obj);
    std::vector<const xAOD::TrackParticle*> phtrks;
    for(size_t iVtx=0; iVtx<ph->nVertices(); ++iVtx) {
      const xAOD::Vertex* phvx = ph->vertex(iVtx);
      for(size_t iTrk=0; iTrk<phvx->nTrackParticles(); ++iTrk) {
	// if(!phvx->trackParticle(iTrk)) {
	//   ATH_MSG_VERBOSE("Invalid photon trackparticle pointer");
	// }
	const xAOD::TrackParticle* phtrk = xAOD::EgammaHelpers::getOriginalTrackParticleFromGSF(phvx->trackParticle(iTrk));
	if(acceptTrack(phtrk,pv) && isGoodEoverP(phtrk,tcCont)) {
	// if(acceptTrack(phtrk,pv)) {
	  bool duplicate = false;
	  for(const auto& gamtrk : phtrks) {
	    if( (duplicate = (phtrk == gamtrk)) ) {
	      ATH_MSG_VERBOSE("Veto duplicate track");
	      break;
	    }
	  }
	  if(!duplicate) {
	    ATH_MSG_VERBOSE("Accept photon track " << phtrk << " px, py = " << phtrk->p4().Px() << ", " << phtrk->p4().Py());
	    ATH_MSG_VERBOSE("              track eta, phi = " << phtrk->p4().Eta() << ", " << phtrk->p4().Phi());
	    constlist.push_back(phtrk);
	    phtrks.push_back(phtrk);
	  }
	}
      }
    }
    return StatusCode::SUCCESS;
  }

  //**********************************************************************
  // Get Egamma constituents
  StatusCode METPhotonAssociator::extractPFO(const xAOD::IParticle* obj,
					     std::vector<const xAOD::IParticle*>& pfolist,
					     const xAOD::PFOContainer* pfoCont,
					     std::map<const IParticle*,MissingETBase::Types::constvec_t> &momenta,
					     const xAOD::Vertex* pv) const
  {
    const xAOD::Photon *ph = static_cast<const xAOD::Photon*>(obj);
    // safe to assume a single SW cluster?
    // will do so for now...
    const xAOD::IParticle* swclus = ph->caloCluster();
    double eg_cl_e = swclus->e();

    // the matching strategy depends on how the cluster container is sorted
    // easier if it's sorted in descending pt order
    // we'll worry about optimisation later
    std::vector<const xAOD::PFO*> nearbyPFO;
    nearbyPFO.reserve(10);
    for(const auto& pfo : *pfoCont) {
      std::vector<const IParticle*> cls;
      if (pfo->charge()==0) {
        if (swclus->p4().DeltaR(pfo->p4EM())<0.1 && pfo->eEM()>0) {
	  nearbyPFO.push_back(pfo);
	}
      } else {
	for(size_t iVtx=0; iVtx<ph->nVertices(); ++iVtx) {
	  const xAOD::Vertex* phvx = ph->vertex(iVtx);
	  for(size_t iTrk=0; iTrk<phvx->nTrackParticles(); ++iTrk) {
	    const xAOD::TrackParticle* phtrk = xAOD::EgammaHelpers::getOriginalTrackParticleFromGSF(phvx->trackParticle(iTrk));
	    if(pfo->track(0) == phtrk) {
	      if(acceptChargedPFO(phtrk,pv)) {
		pfolist.push_back(pfo);
	      }
	    }
	  }
	}
      }
    }
    ATH_MSG_VERBOSE("Found " << nearbyPFO.size() << " nearby pfos");

    bool doSum = true;
    double sumE_pfo = 0.;
    std::sort(nearbyPFO.begin(),nearbyPFO.end(),greaterPtPFO);
    for(const auto& pfo : nearbyPFO) {
      // Handle neutral PFOs like topoclusters
      double pfo_e = pfo->eEM();
      // skip cluster if it's above our bad match threshold
      if(pfo_e>m_tcMatch_maxRat*eg_cl_e) {
        ATH_MSG_VERBOSE("Reject topocluster in sum. Ratio vs eg cluster: " << (pfo_e/eg_cl_e));
	continue;
      }

      if( (doSum = fabs(sumE_pfo+pfo_e-eg_cl_e) < fabs(sumE_pfo - eg_cl_e)) ) {
	pfolist.push_back(pfo);
	sumE_pfo += pfo_e;

        TLorentzVector momentum = pv ? pfo->GetVertexCorrectedEMFourVec(*pv) : pfo->p4EM();
	momenta[pfo] = MissingETBase::Types::constvec_t(momentum.Px(),momentum.Py(),momentum.Pz(),
						   momentum.E(),momentum.Pt());
      } // if we will retain the topocluster
      else {break;}
    } // loop over nearby clusters
    return StatusCode::SUCCESS;
  }

}
