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

// DeltaR calculation
#include "FourMomUtils/xAODP4Helpers.h"

#include "CaloClusterMatching/ICaloClusterMatchingTool.h"

namespace met {

  using namespace xAOD;

  // Constructors
  ////////////////
  METMuonAssociator::METMuonAssociator(const std::string& name) : 
    AsgTool(name),
    METAssociator(name)
  {
    declareProperty("DoClusterMatch", m_doMuonClusterMatch=true);
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
      
      static const SG::AuxElement::ConstAccessor<std::vector<ElementLink<CaloClusterContainer> > > tcLinkAcc("constituentClusterLinks");
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
      if(fabs(pfo->charge())>1e-9) {
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
      
      	  static const SG::AuxElement::ConstAccessor<std::vector<ElementLink<CaloClusterContainer> > > tcLinkAcc("constituentClusterLinks");
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


  // extractPFO for W precision-type measurements
  StatusCode METMuonAssociator::GetPFOWana(const xAOD::IParticle* obj,
             std::vector<const xAOD::IParticle*>& pfolist,
             const met::METAssociator::ConstitHolder& constits,
             std::map<const IParticle*,MissingETBase::Types::constvec_t>& /*momenta*/,
             std::vector<double>& vPhiRnd,
             unsigned int& lept_count,
             float& UEcorr) const
  {
    const xAOD::Muon *mu = static_cast<const xAOD::Muon*>(obj);
    const TrackParticle* idtrack = mu->trackParticle(xAOD::Muon::InnerDetectorTrackParticle); 
    const CaloCluster* muclus = mu->cluster();

    // Loop over all PFOs
    for(const auto& pfo : *constits.pfoCont) {      
      if( fabs(pfo->charge()) > FLT_MIN ) { // Fill list with charged PFOs (using muon tracks)
        const static SG::AuxElement::ConstAccessor<char> PVMatchedAcc("matchedToPV");
        if( idtrack && P4Helpers::isInDeltaR(*pfo, *idtrack, m_Drcone, m_useRapidity) && PVMatchedAcc(*pfo) &&
          ( !m_cleanChargedPFO || isGoodEoverP(pfo->track(0)) ) ){
          pfolist.push_back(pfo);
        }
      } 
      else{ // Fill list with neutral PFOs (using muon clusters)
        if( muclus && P4Helpers::isInDeltaR(*pfo, *muclus, m_Drcone, m_useRapidity) )
          pfolist.push_back(pfo);
      } // neutral PFO condition
    } // loop over all PFOs


    // Step 2. Calculating UE energy correction for a given lepton (using muclus only)
    if(muclus){
      TLorentzVector tv_UEcorr; // vector of UE correction       
      std::pair <double, double> eta_rndphi;
      eta_rndphi.first = muclus->eta();
      eta_rndphi.second = vPhiRnd[lept_count];
      lept_count++;
  
      for(const auto& pfo_itr : *constits.pfoCont) { // loop over PFOs
        if( pfo_itr->e() < 0)
          continue;
        double dR = P4Helpers::deltaR( pfo_itr->eta(), pfo_itr->phi(), eta_rndphi.first,  eta_rndphi.second );
        if( dR < m_Drcone ){
          float dphi_angle = std::fabs( muclus->phi() - pfo_itr->phi() );
          if( dphi_angle > TMath::Pi() ) 
            dphi_angle = 2*TMath::Pi() - dphi_angle;
          if( muclus->phi() <  pfo_itr->phi() )
            dphi_angle = -1. * dphi_angle;

          TLorentzVector tv_pfo = pfo_itr->p4();
          tv_pfo.RotateZ(dphi_angle);
          tv_UEcorr += tv_pfo;  // summing PFOs of UE for correction
        } // cone requirement        
      } // loop over PFOs
      UEcorr = tv_UEcorr.Pt();  // Pt of UE correction
    }  // available muclus requirement

    return StatusCode::SUCCESS;
  }


  StatusCode METMuonAssociator::hadrecoil_PFO(std::vector<const xAOD::IParticle*> hardObjs, 
                                              const met::METAssociator::ConstitHolder& constits, 
                                              TLorentzVector& HR,
                                              std::vector<double>& vPhiRnd) const
  {
    // 1. Summing all PFOs
    for(const auto& pfo_itr : *constits.pfoCont) {
      if( pfo_itr->pt() < 0 || pfo_itr->e() < 0 ) // sanity check
        continue;
      HR += pfo_itr->p4();
    }

    // 2. Subtracting PFOs matched to muons from HR 
    std::vector<const xAOD::Muon*> mu;
    for(const auto& obj_i : hardObjs){
      if(obj_i->pt()<5e3 && obj_i->type() != xAOD::Type::Muon) // similar to METAssociator
        continue;
      mu.push_back( static_cast<const xAOD::Muon*>(obj_i) );
    }

    std::vector<const TrackParticle*> idtrack;  
    for(const auto& mu_i : mu){
      if( mu_i->trackParticle(xAOD::Muon::InnerDetectorTrackParticle) )
        idtrack.push_back( mu_i->trackParticle(xAOD::Muon::InnerDetectorTrackParticle) );     
    }

    std::vector<const CaloCluster*> muclus;
    for(const auto& mu_i : mu){
      if( mu_i->cluster() )
        muclus.push_back( mu_i->cluster() );
    }
    
    for(const auto& pfo_i : *constits.pfoCont) {  // charged and neutral PFOs
      if( pfo_i->pt() < 0 || pfo_i->e() < 0 ) // sanity check
        continue;

      // charged PFOs 
      if( fabs(pfo_i->charge()) > FLT_MIN ) {
        for(const auto& idtrack_i : idtrack) {
          double dR = P4Helpers::deltaR( pfo_i->eta(), pfo_i->phi(), idtrack_i->eta(), idtrack_i->phi() );
          if( dR < m_Drcone ) 
            HR -= pfo_i->p4();
        } // over muon idtrack
      } // charged PFOs 
      else{ // neutral PFOs
        for(const auto& muclus_i : muclus) {
          double dR = P4Helpers::deltaR( pfo_i->eta(), pfo_i->phi(), muclus_i->eta(), muclus_i->phi() );
          if( dR < m_Drcone ) 
            HR -= pfo_i->p4();
        } // over muon muclus
      } // neutral PFOs       
    } // over all PFOs

    
    // 3. Get random phi based on muclus
    unsigned int seed = 0;
    TRandom3 hole;
    if( !muclus.empty() ) {   
      seed = floor( muclus.back()->pt() * 1.e3 );     
      hole.SetSeed(seed);
    }
  
    for(const auto& muclus_i : muclus) {
      bool isNextToPart(true);
      bool isNextToHR(true);
      double Rnd;
  
      while(isNextToPart || isNextToHR ){
        isNextToPart = false; 
        isNextToHR = true;
  
        Rnd = hole.Uniform( -TMath::Pi(), TMath::Pi() );
        double dR = P4Helpers::deltaR( HR.Eta(), HR.Phi(), muclus_i->eta(), Rnd );
  
        if(dR > m_MinDistCone) 
          isNextToHR = false;
  
        for(const auto& muclus_j : muclus) {
          dR = P4Helpers::deltaR( muclus_i->eta(), Rnd, muclus_j->eta(), muclus_j->phi() );
          if(dR < m_MinDistCone)
            isNextToPart = true;
        } // muclus_j
      } // while isNextToPart, isNextToHR
      vPhiRnd.push_back(Rnd);
    } // muclus_i 
  
    return StatusCode::SUCCESS;

  }
  

  // **********************************


} // namespace met


