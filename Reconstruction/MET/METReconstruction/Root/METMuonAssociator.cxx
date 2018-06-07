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


  // extractPFO modified for W measurements
  StatusCode METMuonAssociator::GetPFOWana(const xAOD::IParticle* obj,
             std::vector<const xAOD::IParticle*>& pfolist,
             const met::METAssociator::ConstitHolder& constits,
             std::map<const IParticle*,MissingETBase::Types::constvec_t>& momenta,
             std::vector<double>& vPhiRnd,
             unsigned int& lept_count,
             float& UEcorr) const
  {
    std::cout<<"METMuonAssociator::extractPFO_Wana:  muons"<<std::endl;
    const xAOD::Muon *mu = static_cast<const xAOD::Muon*>(obj);
    const TrackParticle* idtrack = mu->trackParticle(xAOD::Muon::InnerDetectorTrackParticle); // not used currently
    const CaloCluster* muclus = mu->cluster();

    // Loop over all PFOs
    for(const auto& pfo : *constits.pfoCont) {
      // Fill list with charged PFOs (using muon tracks)
      if( fabs(pfo->charge()) > FLT_MIN ) {
        const static SG::AuxElement::ConstAccessor<char> PVMatchedAcc("matchedToPV");
        if(idtrack && P4Helpers::isInDeltaR(*pfo, *idtrack, m_Drcone, m_useRapidity) && PVMatchedAcc(*pfo) )
          pfolist.push_back(pfo);
      } // Fill list with neutral PFOs (using muon clusters)
      else{
        if( muclus && P4Helpers::isInDeltaR(*pfo, *muclus, m_Drcone, m_useRapidity) )
          pfolist.push_back(pfo);
      } // neutral PFO condition
    } // loop over all PFOs


    // Step 2. Calculating UE energy correction for given lepton (using muclus only)
    if(muclus){
      TLorentzVector tv_UEcorr; // vector of UE correction
      TLorentzVector tv_muclus; // make TLorentzVector from muclus to simplify operations
      tv_muclus.SetPtEtaPhiE( muclus->pt(), muclus->eta(), muclus->phi(), muclus->e() );
  
      std::pair <double, double> eta_rndphi;
      eta_rndphi.first = tv_muclus.Eta();
      eta_rndphi.second = vPhiRnd[lept_count];
      lept_count++;
  
      std::cout<<"start looping over PFO to calculate correction"<<std::endl;
      for(const auto& pfo_itr : *constits.pfoCont) { // loop over PFOs
        if( pfo_itr->e() < 0)
          continue;
        TLorentzVector tv_pfo;
        tv_pfo.SetPtEtaPhiE( pfo_itr->pt(), pfo_itr->eta(), pfo_itr->phi(), pfo_itr->e() );
  
        float dR = 0.;
        deltaR( tv_pfo.Eta(), tv_pfo.Phi(), eta_rndphi.first,  eta_rndphi.second, dR);
        if( dR < m_Drcone ){
          float angle;
          METMuonAssociator::deltaPhi(tv_muclus.Phi(), tv_pfo.Phi(), angle);
          if( tv_muclus.Phi() <  tv_pfo.Phi() )
            angle = -1. * angle;
          tv_pfo.RotateZ(angle);
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
    std::cout << "METMuonAssociator::hadrecoil_PFO " << std::endl;
     // 1. Summing all PFOs
    for(const auto& pfo_itr : *constits.pfoCont) {
      if( pfo_itr->pt() < 0 || pfo_itr->e() < 0 ) // sanity check
        continue;
      TLorentzVector pfo_tmp;
      pfo_tmp.SetPtEtaPhiE( pfo_itr->pt(), pfo_itr->eta(), pfo_itr->phi(), pfo_itr->e() );
      HR += pfo_tmp;
    }
    std::cout << "HR->pt() HR->eta() HR->phi() HR->e()          : " << HR.Pt() << "  " << HR.Eta() << "  " << HR.Phi() << "  " << HR.E() << std::endl;


    // 2. Subtracting PFOs mathed to electrons from HR 
    std::vector<const xAOD::Muon*> mu;
    for(const auto& obj_i : hardObjs){
      if(obj_i->pt()<5e3 && obj_i->type() != xAOD::Type::Muon) // similar to METAssociator
        continue;
      mu.push_back( static_cast<const xAOD::Muon*>(obj_i) );
    }
    std::cout << "mu.size()  = " << mu.size() << std::endl;

    std::vector<const TrackParticle*> idtrack_orig;  
    for(const auto& mu_i : mu){
      if( mu_i->trackParticle(xAOD::Muon::InnerDetectorTrackParticle) )
        idtrack_orig.push_back( mu_i->trackParticle(xAOD::Muon::InnerDetectorTrackParticle) );     
    }

    // convert to TLorentzVector
    std::vector<TLorentzVector> idtrack;
    for(const auto& idtrack_orig_i : idtrack_orig){
      TLorentzVector idtrack_curr;

      idtrack_curr.SetPtEtaPhiE( idtrack_orig_i->pt(), idtrack_orig_i->eta(), idtrack_orig_i->phi(), idtrack_orig_i->e() );
      idtrack.push_back(idtrack_curr);
    }
    std::cout << "idtrack.size()  = " << idtrack.size() << std::endl;


    std::vector<const CaloCluster*> muclus_orig;
    for(const auto& mu_i : mu){
      if( mu_i->cluster() )
        muclus_orig.push_back( mu_i->cluster() );
    }

    // convert to TLorentzVector
    std::vector<TLorentzVector> muclus;
    for(const auto& muclus_orig_i : muclus_orig){
      TLorentzVector muclus_curr;
      muclus_curr.SetPtEtaPhiE( muclus_orig_i->pt(), muclus_orig_i->eta(), muclus_orig_i->phi(), muclus_orig_i->e() );
      muclus.push_back(muclus_curr);
    }
    std::cout << "muclus.size()  = " << muclus.size() << std::endl;


   for(const auto& pfo_i : *constits.pfoCont) {  // charged and neutral PFOs
      if( pfo_i->pt() < 0 || pfo_i->e() < 0 ) // sanity check
        continue;
  
      TLorentzVector pfo_curr;
      pfo_curr.SetPtEtaPhiE( pfo_i->pt(), pfo_i->eta(), pfo_i->phi(), pfo_i->e() );

      // charged PFOs 
      if( fabs(pfo_i->charge()) > FLT_MIN ) {
        for(const auto& idtrack_i : idtrack) {
          float dR = 0.;
          deltaR( pfo_curr.Eta(), pfo_curr.Phi(), idtrack_i.Eta(), idtrack_i.Phi(), dR);
          if( dR < m_Drcone ) 
            HR -= pfo_curr;
        } // over muon idtrack
      } // charged PFOs 
      else{ // neutral PFOs
        for(const auto& muclus_i : muclus) {
          float dR = 0.;
          deltaR( pfo_curr.Eta(), pfo_curr.Phi(), muclus_i.Eta(), muclus_i.Phi(), dR);
          if( dR < m_Drcone ) 
            HR -= pfo_curr;
        } // over muon muclus
      } // neutral PFOs       
    } // over all PFOs
    std::cout << "HR->pt() HR->eta() HR->phi() HR->e() corrected: " << HR.Pt() << "  " << HR.Eta() << "  " << HR.Phi() << "  " << HR.E() << std::endl;    


    // 3. Get random phi based on muclus
    unsigned int seed;
    TRandom3 hole;
  
    for(const auto& muclus_i : muclus) {
      seed = floor( muclus_i.Pt() * 1.e3 );      
      hole.SetSeed(seed);
      std::cout << "seed = " << seed << std::endl;
    }
  
    for(const auto& muclus_i : muclus) {
      bool isNextToPart(true);
      bool isNextToHR(true);
      double Rnd;
  
      while(isNextToPart || isNextToHR ){
        isNextToPart = false; 
        isNextToHR = true;
  
        Rnd = hole.Uniform( -TMath::Pi(), TMath::Pi() );
        float dR = 0.;
        this->METMuonAssociator::deltaR(HR.Eta(), HR.Phi(), muclus_i.Eta(), Rnd, dR);
  
        if(dR > m_MinDistCone) 
          isNextToHR = false;
  
        for(const auto& muclus_j : muclus) {
          dR = 0.;
          this->METMuonAssociator::deltaR( muclus_i.Eta(), Rnd, muclus_j.Eta(), muclus_j.Phi(), dR );
          if(dR < m_MinDistCone)
            isNextToPart = true;
        } // muclus_j
      } // while isNextToPart, isNextToHR
      std::cout << "aaa pushback random : " << Rnd << std::endl;
      vPhiRnd.push_back(Rnd);
    } // muclus_i 
  
    return StatusCode::SUCCESS;

  }
  
  void METMuonAssociator::deltaR(float eta1, float phi1, float eta2, float phi2, float& result) const {
    float deta = eta1 - eta2;  
    float dphi = 0;
    METMuonAssociator::deltaPhi(phi1, phi2, dphi);    
    
    result = sqrt(deta*deta + dphi*dphi);  
  
    return;
  } 
  
  
  void METMuonAssociator::deltaPhi(float phi1, float phi2, float& result) const {
    float dphi = std::fabs(phi1 - phi2);
    if (dphi > TMath::Pi()) dphi = 2*TMath::Pi() - dphi;
  
    result = dphi;
  
    return;
  }
  // **********************************


} // namespace met


