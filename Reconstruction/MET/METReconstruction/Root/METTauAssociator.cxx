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
				        	   const xAOD::IParticleContainer* /*tcCont*/) const
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
      if(!xAOD::P4Helpers::isInDeltaR(*seedjet,*cl->rawConstituent(),0.2,m_useRapidity)) continue;
      // skip cluster if dR>0.2
      if(cl->rawConstituent()->type() == xAOD::Type::CaloCluster) {
        const CaloCluster* pClus = static_cast<const CaloCluster*>( cl->rawConstituent() );
        tclist.push_back(pClus);
      } else {
        ATH_MSG_WARNING("Expected an object of type CaloCluster, received one of type " << cl->rawConstituent()->type());
      }
    } // loop over jet constituents

    ///////////////////////////////////////////////////////////////////////////
    //// PROPOSED (BUT NOT FINALIZED) NEW TAU EDM 21/3/2016 
    //// FROM : https://its.cern.ch/jira/browse/ATLTAU-976
    //const TauJet* tau = static_cast<const TauJet*>(obj);
    //std::vector< ElementLink< CaloClusterContainer > > cluLinVec = tau->clusterLinks();
    //ATH_MSG_VERBOSE("Current tau has " << cluLinVec.size() << " constituents.");
    //unsigned int ncc = tau-> nClustersCore();
    //ATH_MSG_VERBOSE("Current tau has " << ncc << " core constituents.");
    //for( unsigned int tc = 0 ; tc < ncc /*cluLinVec.size()*/ ; tc++ ) { 
    //  if ( !cluLinVec[tc].isValid() ) continue ;
    //  const CaloCluster* pClus = *( cluLinVec[tc] ) ;
    //  tclist.push_back(pClus);
    //}
    ///////////////////////////////////////////////////////////////////////////

    return StatusCode::SUCCESS;
  }


  StatusCode METTauAssociator::extractTracks(const xAOD::IParticle *obj,
					     std::vector<const xAOD::IParticle*>& constlist,
					     const xAOD::IParticleContainer* tcCont,
					     const xAOD::Vertex* pv) const
  {
    const TauJet* tau = static_cast<const TauJet*>(obj);
    const Jet* jet = *tau->jetLink();
    for(size_t iTrk=0; iTrk<tau->nTracks(); ++iTrk) {
      const TrackParticle* tautrk = tau->track(iTrk);
      if(acceptTrack(tautrk,pv) && isGoodEoverP(tautrk,tcCont)) {
      // if(acceptTrack(tautrk,pv)) {
	ATH_MSG_VERBOSE("Accept tau track " << tautrk << " px, py = " << tautrk->p4().Px() << ", " << tautrk->p4().Py());
	constlist.push_back(tautrk);
      }
    }
    for(size_t iTrk=0; iTrk<tau->nOtherTracks(); ++iTrk) {
      const TrackParticle* tautrk = tau->otherTrack(iTrk);
      if(xAOD::P4Helpers::isInDeltaR(*jet,*tautrk,0.2,m_useRapidity) && acceptTrack(tautrk,pv) && isGoodEoverP(tautrk,tcCont)) {
      // if(dR<0.2 && acceptTrack(tautrk,pv)) {
	ATH_MSG_VERBOSE("Accept track " << tautrk << " px, py = " << tautrk->p4().Px() << ", " << tautrk->p4().Py());
	constlist.push_back(tautrk);
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
					  const xAOD::Vertex* pv) const
  {
    const TauJet* tau = static_cast<const TauJet*>(obj);
    const Jet* seedjet = *tau->jetLink();
    for(const auto& pfo : *pfoCont) {
      bool match = false;
      if (pfo->charge()==0) {
	if(seedjet->p4().DeltaR(pfo->p4EM())<0.2 && pfo->eEM()>0) {match = true;}
      }
      else {
	const TrackParticle* pfotrk = pfo->track(0);
	for(size_t iTrk=0; iTrk<tau->nTracks(); ++iTrk) {
	  const TrackParticle* tautrk = tau->track(iTrk);
	  if(tautrk==pfotrk) {
	    if(acceptChargedPFO(tautrk,pv)) match = true;
	  }
	}
	for(size_t iTrk=0; iTrk<tau->nOtherTracks(); ++iTrk) {
	  const TrackParticle* tautrk = tau->otherTrack(iTrk);
          if(tautrk==pfotrk && xAOD::P4Helpers::isInDeltaR(*seedjet,*tautrk,0.2,m_useRapidity) && acceptChargedPFO(tautrk,pv)) match = true;
	}
      }
      if(match) {
	pfolist.push_back(pfo);
	if(pfo->charge()==0) {
	  TLorentzVector momentum = pv ? pfo->GetVertexCorrectedEMFourVec(*pv) : pfo->p4EM();
	  momenta[pfo] = MissingETBase::Types::constvec_t(momentum.Px(),momentum.Py(),momentum.Pz(),
						     momentum.E(),momentum.Pt());
	}
      }
    }
    return StatusCode::SUCCESS;
  }

}
