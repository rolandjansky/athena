///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// METMuonAssociator.cxx 
// Implementation file for class METMuonAssociator
//
//  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//
// Author: P Loch, S Resconi, TJ Khoo, AS Mete
/////////////////////////////////////////////////////////////////// 

// METReconstruction includes
#include "METReconstruction/METMuonAssociator.h"

// Muon EDM
#include "xAODMuon/MuonContainer.h"

// Tracking EDM
#include "xAODTracking/Vertex.h"

#include "CaloClusterMatching/ICaloClusterMatchingTool.h"

namespace met {

  using namespace xAOD;

  // Constructors
  ////////////////
  METMuonAssociator::METMuonAssociator(const std::string& name) : 
    AsgTool(name),
    METAssociator(name)
  {}

  // Destructor
  ///////////////
  METMuonAssociator::~METMuonAssociator()
  {}

  // Athena algtool's Hooks
  ////////////////////////////
  StatusCode METMuonAssociator::initialize()
  {
    ATH_CHECK( METAssociator::initialize() );
    ATH_MSG_VERBOSE ("Initializing " << name() << "...");
    return StatusCode::SUCCESS;
  }

  StatusCode METMuonAssociator::finalize()
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
  StatusCode METMuonAssociator::executeTool(xAOD::MissingETContainer* /*metCont*/, xAOD::MissingETAssociationMap* metMap)
  {
    ATH_MSG_VERBOSE ("In execute: " << name() << "...");

    const MuonContainer* muonCont(0);
    if( evtStore()->retrieve(muonCont, m_input_data_key).isFailure() ) {
      ATH_MSG_WARNING("Unable to retrieve input muon container " << m_input_data_key);
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG("Successfully retrieved muon collection");
    if (fillAssocMap(metMap,muonCont).isFailure()) {
      ATH_MSG_WARNING("Unable to fill map with muon container " << m_input_data_key);
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  }

  //*********************************************************************************************************
  // Get constituents
  StatusCode METMuonAssociator::extractTopoClusters(const xAOD::IParticle* obj,
						    std::vector<const xAOD::IParticle*>& tclist,
						    const xAOD::IParticleContainer* /*tcCont*/) const
  {
    const xAOD::Muon *mu = static_cast<const xAOD::Muon*>(obj);
    const CaloCluster* muclus = mu->cluster();
    if(muclus) {
      ATH_MSG_VERBOSE("Muon " << mu->index() << " with pt " << mu->pt()
		   << ", eta "   << mu->eta()
		   << ", phi " << mu->phi()
		   << " has cluster with "
		   << "eta "   << muclus->calEta()
		   << ", phi " << muclus->calPhi()
		   << ", E "   << muclus->calE()
		   << " formed of " << muclus->size() << " cells.");
      ATH_MSG_VERBOSE("Muon Eloss type: " << mu->energyLossType()
		   << " Eloss: " << mu->floatParameter(xAOD::Muon::EnergyLoss)
		   << " MeasuredEloss: " << mu->floatParameter(xAOD::Muon::MeasEnergyLoss)
		   << " FSR E: " << mu->floatParameter(xAOD::Muon::FSR_CandidateEnergy) );
      
      static const SG::AuxElement::ConstAccessor<std::vector<ElementLink<CaloClusterContainer> > > tcLinkAcc("constituentClusterLinks");
      for(const auto& matchel : tcLinkAcc(*muclus)) {
	ATH_MSG_VERBOSE("Tool found cluster " << (*matchel)->index() << " with pt " << (*matchel)->pt() );
	if((*matchel)->e()>1e-9) { // +ve E
	  tclist.push_back(*matchel);
	}
      }
    } // muon has linked cluster
    
    return StatusCode::SUCCESS;
  }

  StatusCode METMuonAssociator::extractTracks(const xAOD::IParticle *obj,
					      std::vector<const xAOD::IParticle*>& constlist,
					      const xAOD::IParticleContainer* tcCont,
					      const xAOD::Vertex* pv) const
  {
    const xAOD::Muon *mu = static_cast<const xAOD::Muon*>(obj);
    const TrackParticle* idtrack = mu->trackParticle(xAOD::Muon::InnerDetectorTrackParticle);
    if(idtrack && acceptTrack(idtrack,pv) && isGoodEoverP(idtrack,tcCont)) {
    // if(idtrack && acceptTrack(idtrack,pv)) {
      ATH_MSG_VERBOSE("Accept muon track " << idtrack << " px, py = " << idtrack->p4().Px() << ", " << idtrack->p4().Py());
      ATH_MSG_VERBOSE("Muon ID track ptr: " << idtrack);
      constlist.push_back(idtrack);
      // if(mu->pt()>10e3 && (mu->muonType()==xAOD::Muon::Combined || mu->muonType()==xAOD::Muon::SegmentTagged)) {
      //   mutracks.push_back(idtrack);
      // }
    }
    return StatusCode::SUCCESS;
  }

  //*********************************************************************************************************
  // Get constituents
  StatusCode METMuonAssociator::extractPFO(const xAOD::IParticle* obj,
					   std::vector<const xAOD::IParticle*>& pfolist,
					   const xAOD::PFOContainer* pfoCont,
					   std::map<const IParticle*,MissingETBase::Types::constvec_t>&,
					   const xAOD::Vertex* pv) const
  {  
    const xAOD::Muon *mu = static_cast<const xAOD::Muon*>(obj);
    const TrackParticle* idtrack = mu->trackParticle(xAOD::Muon::InnerDetectorTrackParticle);
    if(idtrack && acceptChargedPFO(idtrack,pv)) {
      for(const auto& pfo : *pfoCont) {
	if (pfo->charge()!=0 && pfo->track(0) == idtrack) {
	  pfolist.push_back(pfo);
	  break;
	}
      }
    }
    return StatusCode::SUCCESS;
  }



}
