///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
#include "xAODTau/TauxAODHelpers.h"

// Tracking EDM
#include "xAODTracking/Vertex.h"

// DeltaR calculation
#include "FourMomUtils/xAODP4Helpers.h"

#include "tauRecTools/HelperFunctions.h"
#include "xAODCaloEvent/CaloVertexedTopoCluster.h"
#include "PFlowUtils/IWeightPFOTool.h"


typedef ElementLink<xAOD::TauJetContainer> TauLink_t;
typedef ElementLink<xAOD::FlowElementContainer> FELink_t;

namespace met {

  using namespace xAOD;

  // Constructors
  ////////////////
  METTauAssociator::METTauAssociator(const std::string& name) :
    AsgTool(name),
    METAssociator(name),
    m_tauContKey("")
  {
    declareProperty("tauContainer",m_tauContKey);
    declareProperty("UseFETauLinks", m_useFETauLinks = false ); 
  }

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
    ATH_CHECK( m_tauContKey.assign(m_input_data_key));
    ATH_CHECK( m_tauContKey.initialize());

    if (m_useFETauLinks || m_useFELinks) {
      if (m_neutralFEReadDecorKey.key()=="") {ATH_CHECK( m_neutralFEReadDecorKey.assign(m_input_data_key+"."+m_neutralFELinksKey));}
      if (m_chargedFEReadDecorKey.key()=="") {ATH_CHECK( m_chargedFEReadDecorKey.assign(m_input_data_key+"."+m_chargedFELinksKey));}
      ATH_CHECK( m_neutralFEReadDecorKey.initialize());
      ATH_CHECK( m_chargedFEReadDecorKey.initialize());
    }

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

    SG::ReadHandle<xAOD::TauJetContainer> tauCont(m_tauContKey);
    if (!tauCont.isValid()) {
      ATH_MSG_WARNING("Unable to retrieve input tau container " << m_input_data_key);
      return StatusCode::FAILURE;
    }

    ATH_MSG_DEBUG("Successfully retrieved tau collection");
    if (fillAssocMap(metMap,tauCont.cptr()).isFailure()) {
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
    TLorentzVector tauAxis = tauRecTools::getTauAxis(*tau);
    const xAOD::Vertex* tauVertex = tauRecTools::getTauVertex(*tau);

    auto clusterList = tau->clusters();
    for (const xAOD::IParticle* particle : clusterList) {
      const xAOD::CaloCluster* cluster = static_cast<const xAOD::CaloCluster*>(particle);
      
      TLorentzVector clusterP4 = cluster->p4();
      
      // Correct the four momentum to point at the tau vertex
      if (tauVertex) {
        xAOD::CaloVertexedTopoCluster vertexedCluster(*cluster, tauVertex->position()); 
        clusterP4 = vertexedCluster.p4();
      }
        
      if (clusterP4.DeltaR(tauAxis) > 0.2) continue;
    
      tclist.push_back(particle);
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
    for(const auto pfo : *constits.pfoCont) {
      bool match = false;
      if (!pfo->isCharged()) {
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

  StatusCode METTauAssociator::extractFE(const xAOD::IParticle* obj, 
                                            std::vector<const xAOD::IParticle*>& felist,
                                            const met::METAssociator::ConstitHolder& constits,
                                            std::map<const IParticle*,MissingETBase::Types::constvec_t> &/*momenta*/) const
  {
    const TauJet* tau = static_cast<const TauJet*>(obj);

    if (m_useFETauLinks) { 
      ATH_CHECK( extractFEsFromLinks(tau, felist,constits) );
    } 
    else {
      ATH_CHECK( extractFEs(tau, felist, constits) );
    }

    return StatusCode::SUCCESS;
  }


  StatusCode METTauAssociator::extractFEsFromLinks(const xAOD::TauJet* tau, //TODO to be tested
    				    std::vector<const xAOD::IParticle*>& felist,
				    const met::METAssociator::ConstitHolder& constits) const 
  {

    ATH_MSG_DEBUG("Extract FEs From Links for " << tau->type()  << " with pT " << tau->pt());

    std::vector<FELink_t> nFELinks;
    std::vector<FELink_t> cFELinks;

    SG::ReadDecorHandle<xAOD::TauJetContainer, std::vector<FELink_t> > neutralFEReadDecorHandle (m_neutralFEReadDecorKey);
    SG::ReadDecorHandle<xAOD::TauJetContainer, std::vector<FELink_t> > chargedFEReadDecorHandle (m_chargedFEReadDecorKey);
    nFELinks=neutralFEReadDecorHandle(*tau);
    cFELinks=chargedFEReadDecorHandle(*tau);

    // Charged FEs
    for (FELink_t feLink : cFELinks) {
      if (feLink.isValid()){
	const xAOD::FlowElement* fe_init = *feLink;
	for (const auto& fe : *constits.feCont){
	  if (fe->index() == fe_init->index() && fe->isCharged()){ //index-based match between JetETmiss and CHSFlowElements collections
	    const static SG::AuxElement::ConstAccessor<char> PVMatchedAcc("matchedToPV");
	    if(  fe->isCharged() && PVMatchedAcc(*fe)&& ( !m_cleanChargedPFO || isGoodEoverP(static_cast<const xAOD::TrackParticle*>(fe->chargedObject(0))) ) ) {
	      ATH_MSG_DEBUG("Accept cFE with pt " << fe->pt() << ", e " << fe->e() << ", eta " << fe->eta() << ", phi " << fe->phi() );
	      felist.push_back(fe); 
	    } 
	  }
	}
      }
    } // end cFE loop

    // Neutral FEs
    for (FELink_t feLink : nFELinks) {
      if (feLink.isValid()){
        const xAOD::FlowElement* fe_init = *feLink;
	for (const auto& fe : *constits.feCont){
	  if (fe->index() == fe_init->index() && !fe->isCharged()){ //index-based match between JetETmiss and CHSFlowElements collections
	    if( ( !fe->isCharged()&& fe->e() > FLT_MIN ) ){ 
	      ATH_MSG_DEBUG("Accept nFE with pt " << fe->pt() << ", e " << fe->e() << ", eta " << fe->eta() << ", phi " << fe->phi() << " in sum.");
	      felist.push_back(fe);
	    }   
	  }
	}
      }
    } // end nFE links loop


    return StatusCode::SUCCESS;

  }


  StatusCode METTauAssociator::extractFEs(const xAOD::TauJet* tau,
                                         std::vector<const xAOD::IParticle*>& felist,
                                         const met::METAssociator::ConstitHolder& constits) const
  {
    //const TauJet* tau = static_cast<const TauJet*>(obj);
    const Jet* seedjet = *tau->jetLink();
    TLorentzVector momentum;
    for(const xAOD::FlowElement* pfo : *constits.feCont) {
      bool match = false;
      if (!pfo->isCharged()) {
        if(xAOD::P4Helpers::isInDeltaR(*seedjet,*pfo,0.2,m_useRapidity) && pfo->e()>0) {
          ATH_MSG_VERBOSE("Found nPFO with dR " << seedjet->p4().DeltaR(pfo->p4()));
          match = true;
        }
      }
      else {
        const TrackParticle* pfotrk = static_cast<const xAOD::TrackParticle*>(pfo->chargedObject(0));
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
        felist.push_back(pfo);
      }
    }
    return StatusCode::SUCCESS;
  }


}
