///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
#include "StoreGate/ReadDecorHandle.h"

// Muon EDM
#include "xAODMuon/MuonContainer.h"

// Tracking EDM
#include "xAODTracking/Vertex.h"


typedef ElementLink<xAOD::MuonContainer> MuonLink_t;
typedef ElementLink<xAOD::FlowElementContainer> FELink_t;

namespace met {

  using namespace xAOD;

  // Constructors
  ////////////////
  METMuonAssociator::METMuonAssociator(const std::string& name) : 
    AsgTool(name),
    METAssociator(name),
    m_muContKey("")

  {
    declareProperty("DoClusterMatch", m_doMuonClusterMatch=true);
    declareProperty("MuonKey",m_muContKey);
    declareProperty("UseFEMuonLinks", m_useFEMuonLinks = false ); 
  }

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
    ATH_CHECK( m_muContKey.assign(m_input_data_key));
    ATH_CHECK( m_muContKey.initialize());
    if (m_neutralFEReadDecorKey.key()=="") {ATH_CHECK( m_neutralFEReadDecorKey.assign(m_input_data_key+"."+m_neutralFELinksKey));}
    if (m_chargedFEReadDecorKey.key()=="") {ATH_CHECK( m_chargedFEReadDecorKey.assign(m_input_data_key+"."+m_chargedFELinksKey));}
    ATH_CHECK( m_neutralFEReadDecorKey.initialize());
    ATH_CHECK( m_chargedFEReadDecorKey.initialize());

    if (m_doMuonClusterMatch) {
      ATH_CHECK(m_elementLinkName.initialize());
    }
    else {
      m_elementLinkName="";
    }
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
  StatusCode METMuonAssociator::executeTool(xAOD::MissingETContainer* /*metCont*/, xAOD::MissingETAssociationMap* metMap) const
  {
    ATH_MSG_VERBOSE ("In execute: " << name() << "...");

    SG::ReadHandle<xAOD::MuonContainer> muonCont(m_muContKey);
    if (!muonCont.isValid()) {
      ATH_MSG_WARNING("Unable to retrieve input muon container " << m_input_data_key);
      return StatusCode::FAILURE;
    }

    ATH_MSG_DEBUG("Successfully retrieved muon collection");
    if (fillAssocMap(metMap,muonCont.cptr()).isFailure()) {
      ATH_MSG_WARNING("Unable to fill map with muon container " << m_input_data_key);
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  }

  //*********************************************************************************************************
  // Get constituents
  StatusCode METMuonAssociator::extractTopoClusters(const xAOD::IParticle* obj,
                                                    std::vector<const xAOD::IParticle*>& tclist,
                                                    const met::METAssociator::ConstitHolder& /*constits*/) const
  {
    const xAOD::Muon *mu = static_cast<const xAOD::Muon*>(obj);
    const CaloCluster* muclus = mu->cluster();
    if(muclus && m_doMuonClusterMatch) {
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
      
      SG::ReadDecorHandle<CaloClusterContainer, std::vector<ElementLink<CaloClusterContainer> > > tcLinkAcc(m_elementLinkName); 
      for(const auto& matchel : tcLinkAcc(*muclus)) {
        if(!matchel.isValid()) {continue;} // In case of thinned cluster collection
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
                                              const met::METAssociator::ConstitHolder& constits) const
  {
    const xAOD::Muon *mu = static_cast<const xAOD::Muon*>(obj);
    const TrackParticle* idtrack = mu->trackParticle(xAOD::Muon::InnerDetectorTrackParticle);
    if(idtrack && acceptTrack(idtrack,constits.pv) && isGoodEoverP(idtrack)) {
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
                                           const met::METAssociator::ConstitHolder& constits,
                                           std::map<const IParticle*,MissingETBase::Types::constvec_t>& /*momenta*/) const
  {  
    const xAOD::Muon *mu = static_cast<const xAOD::Muon*>(obj);
    const TrackParticle* idtrack = mu->trackParticle(xAOD::Muon::InnerDetectorTrackParticle);
    const CaloCluster* muclus = mu->cluster();

    ATH_MSG_VERBOSE("Muon " << mu->index() << " with pt " << mu->pt()
                    << ", eta "   << mu->eta()
                    << ", phi " << mu->phi());
    if(muclus) {
      ATH_MSG_VERBOSE(" has cluster with "
                      << "eta "   << muclus->calEta()
                      << ", phi " << muclus->calPhi()
                      << ", E "   << muclus->calE()
                      << " formed of " << muclus->size() << " cells.");
    }
    ATH_MSG_VERBOSE("Muon Eloss type: " << mu->energyLossType()
                    << " Eloss: " << mu->floatParameter(xAOD::Muon::EnergyLoss)
                    << " MeasuredEloss: " << mu->floatParameter(xAOD::Muon::MeasEnergyLoss)
                    << " FSR E: " << mu->floatParameter(xAOD::Muon::FSR_CandidateEnergy) );

    // One loop over PFOs
    for(const auto& pfo : *constits.pfoCont) {
      if(pfo->isCharged()) {
        // get charged PFOs by matching the muon ID track
        // We set a small -ve pt for cPFOs that were rejected
        // by the ChargedHadronSubtractionTool
        const static SG::AuxElement::ConstAccessor<char> PVMatchedAcc("matchedToPV");
        if(idtrack && pfo->track(0) == idtrack && PVMatchedAcc(*pfo) &&
           ( !m_cleanChargedPFO || isGoodEoverP(pfo->track(0)) )
           ) {
          ATH_MSG_VERBOSE("Accept muon PFO " << pfo << " px, py = " << pfo->p4().Px() << ", " << pfo->p4().Py());
          ATH_MSG_VERBOSE("Muon PFO index: " << pfo->index() << ", pt: " << pfo->pt() << ", eta: " << pfo->eta() << ", phi: " << pfo->phi() );
          ATH_MSG_VERBOSE("Muon ID Track index: " << idtrack->index() << ", pt: " << idtrack->pt() << ", eta: " << idtrack->eta() << ", phi: " << idtrack->phi() );
          pfolist.push_back(pfo);
          break;
        } // track match
      } else {
        // get neutral PFOs by matching the muon cluster
        if(muclus && m_doMuonClusterMatch) {

          SG::ReadDecorHandle<CaloClusterContainer, std::vector<ElementLink<CaloClusterContainer> > > tcLinkAcc(m_elementLinkName); 
                for(const auto& matchel : tcLinkAcc(*muclus)) {
            if(!matchel.isValid()) {
              ATH_MSG_DEBUG("Invalid muon-cluster elementLink");
            } else {
              if((*matchel)->e()>FLT_MIN && pfo->cluster(0) == *matchel) { // +ve E && matches cluster
                ATH_MSG_VERBOSE("Tool found cluster " << (*matchel)->index() << " with pt " << (*matchel)->pt() );
                pfolist.push_back(pfo);
              }
            }
          }
        } // muon has linked cluster
      } 
    } // end of cluster loop

    return StatusCode::SUCCESS;
  }

  // TODO: split in extractFEsFromLinks and extractFEs, similarly to extractPFO in METEgammaAssociator, to use links

  StatusCode METMuonAssociator::extractFE(const xAOD::IParticle* obj, //testFELinks
                                            std::vector<const xAOD::IParticle*>& felist,
                                            const met::METAssociator::ConstitHolder& constits,
                                            std::map<const IParticle*,MissingETBase::Types::constvec_t> &/*momenta*/) const
  {
    const xAOD::Muon *mu = static_cast<const xAOD::Muon*>(obj);

    if (m_useFEMuonLinks) { 
      ATH_CHECK( extractFEsFromLinks(mu, felist,constits) );
    } 
    else {
      ATH_CHECK( extractFEs(mu, felist, constits) );
    }

    return StatusCode::SUCCESS;
  }

  StatusCode METMuonAssociator::extractFEsFromLinks(const xAOD::Muon* mu,
						       std::vector<const xAOD::IParticle*>& felist,
						       const met::METAssociator::ConstitHolder& constits) const
  {
    //TODO
    return StatusCode::SUCCESS;
  }

  StatusCode METMuonAssociator::extractFEs(const xAOD::Muon* mu, //testFELinks
				 std::vector<const xAOD::IParticle*>& felist,
				 const met::METAssociator::ConstitHolder& constits) const
  {  
    const TrackParticle* idtrack = mu->trackParticle(xAOD::Muon::InnerDetectorTrackParticle);
    const CaloCluster* muclus = mu->cluster();
    ATH_MSG_VERBOSE("Muon " << mu->index() << " with pt " << mu->pt()
                    << ", eta "   << mu->eta()
                    << ", phi " << mu->phi());
    if(muclus) {
      ATH_MSG_VERBOSE(" has cluster with "
                      << "eta "   << muclus->calEta()
                      << ", phi " << muclus->calPhi()
                      << ", E "   << muclus->calE()
                      << " formed of " << muclus->size() << " cells.");
    }
    ATH_MSG_VERBOSE("Muon Eloss type: " << mu->energyLossType()
                    << " Eloss: " << mu->floatParameter(xAOD::Muon::EnergyLoss)
                    << " MeasuredEloss: " << mu->floatParameter(xAOD::Muon::MeasEnergyLoss)
                    << " FSR E: " << mu->floatParameter(xAOD::Muon::FSR_CandidateEnergy) );

    // One loop over PFOs
    for(const xAOD::FlowElement* fe : *constits.feCont) {
      if(fe->isCharged()) {
        // get charged FEs by matching the muon ID track
        // We set a small -ve pt for cPFOs that were rejected
        // by the ChargedHadronSubtractionTool
        const static SG::AuxElement::ConstAccessor<char> PVMatchedAcc("matchedToPV");
        if(idtrack && fe->chargedObject(0) == idtrack && PVMatchedAcc(*fe) &&
           ( !m_cleanChargedPFO || isGoodEoverP(static_cast<const xAOD::TrackParticle*>(fe->chargedObject(0))) )
           ) {
          ATH_MSG_VERBOSE("Accept muon PFO (FE) " << fe << " px, py = " << fe->p4().Px() << ", " << fe->p4().Py());
          ATH_MSG_VERBOSE("Muon PFO index: " << fe->index() << ", pt: " << fe->pt() << ", eta: " << fe->eta() << ", phi: " << fe->phi() );
          ATH_MSG_VERBOSE("Muon ID Track index: " << idtrack->index() << ", pt: " << idtrack->pt() << ", eta: " << idtrack->eta() << ", phi: " << idtrack->phi() );
          felist.push_back(fe);
          break;
        } // track match
      } else {
        // get neutral PFOs by matching the muon cluster
        if(muclus && m_doMuonClusterMatch) {

          SG::ReadDecorHandle<CaloClusterContainer, std::vector<ElementLink<CaloClusterContainer> > > tcLinkAcc(m_elementLinkName); 
          for(const auto& matchel : tcLinkAcc(*muclus)) {
            if(!matchel.isValid()) {
              ATH_MSG_DEBUG("Invalid muon-cluster elementLink");
            } else {
              if((*matchel)->e()>FLT_MIN && fe->otherObject(0) == *matchel) { // +ve E && matches cluster
                ATH_MSG_VERBOSE("Tool found cluster " << (*matchel)->index() << " with pt " << (*matchel)->pt() );
                felist.push_back(fe);
              }
            }
          }
        } // muon has linked cluster
      } 
    } // end of cluster loop

    return StatusCode::SUCCESS;
  }

}
