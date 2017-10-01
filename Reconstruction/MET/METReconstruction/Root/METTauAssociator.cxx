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
#include "xAODTau/TauTrack.h"

// Tracking EDM
#include "xAODTracking/Vertex.h"

// DeltaR calculation
#include "FourMomUtils/xAODP4Helpers.h"

#include "PFlowUtils/IWeightPFOTool.h"

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
    ATH_CHECK( METAssociator::initialize() );
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
  StatusCode METTauAssociator::executeTool(xAOD::MissingETContainer* /*metCont*/, xAOD::MissingETAssociationMap* metMap) const
  {
    ATH_MSG_VERBOSE ("In execute: " << name() << "...");

    const TauJetContainer* tauCont(0);
    if( evtStore()->retrieve(tauCont, m_input_data_key).isFailure() ) {
      ATH_MSG_WARNING("Unable to retrieve input tau container " << m_input_data_key);
      return StatusCode::FAILURE;
    }

    ATH_MSG_DEBUG("Successfully retrieved tau collection");
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
				        	   const met::METAssociator::ConstitHolder& /*tcCont*/) const
  {
    const TauJet* tau = static_cast<const TauJet*>(obj);
    for( ElementLink< xAOD::IParticleContainer > cluster_link : tau->clusterLinks() ){
      const xAOD::IParticle* ipart = *cluster_link;
      if(ipart->type() != xAOD::Type::CaloCluster) {
    	ATH_MSG_WARNING("Unexpected jet constituent type " << ipart->type() << " received! Skip.");
    	continue;
      }      
      // Link set in Reconstruction/tauRecTools/src/TauAxisSetter.cxx
      // Internal defaults are m_clusterCone = 0.2, m_doCellCorrection = false, m_doAxisCorrection = True
      const CaloCluster* pClus = static_cast<const CaloCluster*>( ipart );
      tclist.push_back(pClus);
    }

    return StatusCode::SUCCESS;
  }


  StatusCode METTauAssociator::extractTracks(const xAOD::IParticle *obj,
					     std::vector<const xAOD::IParticle*>& constlist,
					     const met::METAssociator::ConstitHolder& constits) const
  {
    const TauJet* tau = static_cast<const TauJet*>(obj);
    for( const xAOD::TauTrack* tauTrk : tau->tracks(xAOD::TauJetParameters::coreTrack) ){//all tracks dR<0.2 regardless of quality
      const TrackParticle* trk = tauTrk->track();
      if(acceptTrack(trk,constits.pv) && isGoodEoverP(trk) ){
        ATH_MSG_VERBOSE("Accept tau track " << trk << " px, py = " << trk->p4().Px() << ", " << trk->p4().Py());
        constlist.push_back(trk);
      }
    }

    return StatusCode::SUCCESS;
  }
  //*********************************************************************************************************
  // Get tau constituents
  StatusCode METTauAssociator::extractPFO(const xAOD::IParticle* obj,
					  std::vector<const xAOD::IParticle*>& pfolist,
					  const met::METAssociator::ConstitHolder& constits,
					  std::map<const IParticle*,MissingETBase::Types::constvec_t> &/*momenta*/) const
  {
    const TauJet* tau = static_cast<const TauJet*>(obj);
    const Jet* seedjet = *tau->jetLink();
    TLorentzVector momentum;
    for(const auto& pfo : *constits.pfoCont) {
      bool match = false;
      if (fabs(pfo->charge())<FLT_MIN) {
	if(xAOD::P4Helpers::isInDeltaR(*seedjet,*pfo,0.2,m_useRapidity) && pfo->eEM()>0) {
	  ATH_MSG_VERBOSE("Found nPFO with dR " << seedjet->p4().DeltaR(pfo->p4EM()));
	  match = true;
	}
      }
      else {
        const TrackParticle* pfotrk = pfo->track(0);
        for( const xAOD::TauTrack* ttrk : tau->tracks(xAOD::TauJetParameters::coreTrack) ){//all tracks <0.2, no quality
          const TrackParticle* tautrk = ttrk->track();
          if(tautrk==pfotrk) {
	    ATH_MSG_VERBOSE("Found cPFO with dR " << seedjet->p4().DeltaR(ttrk->p4()));
	    // We set a small -ve pt for cPFOs that were rejected
	    // by the ChargedHadronSubtractionTool
	    const static SG::AuxElement::ConstAccessor<char> PVMatchedAcc("matchedToPV");	
	    if(PVMatchedAcc(*pfo) && ( !m_cleanChargedPFO || isGoodEoverP(pfotrk) )) match = true;
          }
        }
      }
      if(match) {
	pfolist.push_back(pfo);
      }
    }
    return StatusCode::SUCCESS;
  }

}
