///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// METElectronAssociator.cxx
// Implementation file for class METElectronAssociator
//
//  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//
// Author: P Loch, S Resconi, TJ Khoo, AS Mete
///////////////////////////////////////////////////////////////////

// METReconstruction includes
#include "METReconstruction/METElectronAssociator.h"

// Egamma EDM
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/EgammaxAODHelpers.h"

namespace met {

  using namespace xAOD;

  // Constructors
  ////////////////
  METElectronAssociator::METElectronAssociator(const std::string& name) :
    AsgTool(name),
    METAssociator(name),
    METEgammaAssociator(name)
  {}

  // Destructor
  ///////////////
  METElectronAssociator::~METElectronAssociator()
  {}

  // Athena algtool's Hooks
  ////////////////////////////
  StatusCode METElectronAssociator::initialize()
  {
    ATH_CHECK( METEgammaAssociator::initialize() );
    ATH_MSG_VERBOSE ("Initializing " << name() << "...");
    return StatusCode::SUCCESS;
  }

  StatusCode METElectronAssociator::finalize()
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
  StatusCode METElectronAssociator::executeTool(xAOD::MissingETContainer* /*metCont*/, xAOD::MissingETAssociationMap* metMap)
  {
    ATH_MSG_VERBOSE ("In execute: " << name() << "...");

    const ElectronContainer* elCont(0);
    if( evtStore()->retrieve(elCont, m_input_data_key).isFailure() ) {
      ATH_MSG_WARNING("Unable to retrieve input electron container " << m_input_data_key);
      return StatusCode::FAILURE;
    }

    ATH_MSG_DEBUG("Successfully retrieved electron collection");
    if (fillAssocMap(metMap,elCont).isFailure()) {
      ATH_MSG_WARNING("Unable to fill map with electron container " << m_input_data_key);
      return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
  }


  StatusCode METElectronAssociator::extractTracks(const xAOD::IParticle* obj,
						  std::vector<const xAOD::IParticle*>& constlist,
						  const xAOD::IParticleContainer* tcCont,
					          const xAOD::Vertex* pv) const
  {
    const Electron *el = static_cast<const Electron*>(obj);
    for(size_t iTrk=0; iTrk<el->nTrackParticles(); ++iTrk) {
      const TrackParticle* eltrk = EgammaHelpers::getOriginalTrackParticleFromGSF(el->trackParticle(iTrk));
      if(acceptTrack(eltrk,pv) && isGoodEoverP(eltrk,tcCont)) {
      // if(acceptTrack(eltrk,pv)) {
	ATH_MSG_VERBOSE("Accept electron track " << eltrk << " px, py = " << eltrk->p4().Px() << ", " << eltrk->p4().Py());
	constlist.push_back(eltrk);
      }
    }
    return StatusCode::SUCCESS;
  }

  //**********************************************************************
  // Get Egamma constituents
  StatusCode METElectronAssociator::extractPFO(const xAOD::IParticle* obj,
					       std::vector<const xAOD::IParticle*>& pfolist,
					       const xAOD::PFOContainer* pfoCont,
					       std::map<const IParticle*,MissingETBase::Types::constvec_t> &momenta,
					       const xAOD::Vertex* pv) const
  {
    const Electron *el = static_cast<const Electron*>(obj);
    // safe to assume a single SW cluster?
    // will do so for now...
    const IParticle* swclus = el->caloCluster();
    double eg_cl_e = swclus->e();

    // the matching strategy depends on how the cluster container is sorted
    // easier if it's sorted in descending pt order
    // we'll worry about optimisation later
    std::vector<const PFO*> nearbyPFO;
    nearbyPFO.reserve(10);
    for(const auto& pfo : *pfoCont) {
      std::vector<const IParticle*> cls;
      if (pfo->charge()==0) {
        if (swclus->p4().DeltaR(pfo->p4EM())<m_tcMatch_dR && pfo->eEM()>0) {
	  nearbyPFO.push_back(pfo);
	}
      } else {
	const TrackParticle* pfotrk = pfo->track(0);
	for(size_t iTrk=0; iTrk<el->nTrackParticles(); ++iTrk) {
	  const TrackParticle* eltrk = EgammaHelpers::getOriginalTrackParticleFromGSF(el->trackParticle(iTrk));
	  if(pfotrk == eltrk) {
	    if(acceptChargedPFO(eltrk,pv)) {
	      pfolist.push_back(pfo);
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
