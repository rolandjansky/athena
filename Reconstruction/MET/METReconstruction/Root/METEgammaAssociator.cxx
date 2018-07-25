///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// METEgammaAssociator.cxx 
// Implementation file for class METEgammaAssociator
//
//  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//
// Author: P Loch, S Resconi, TJ Khoo, AS Mete
/////////////////////////////////////////////////////////////////// 

// METReconstruction includes
#include "METReconstruction/METEgammaAssociator.h"

// Egamma EDM
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/EgammaxAODHelpers.h"

// Tracking EDM
#include "xAODTracking/Vertex.h"

// DeltaR calculation
#include "FourMomUtils/xAODP4Helpers.h"

#include "TRandom3.h"
#include <TLorentzVector.h>

namespace met {

  using namespace xAOD;

  // Constructors
  ////////////////
  METEgammaAssociator::METEgammaAssociator(const std::string& name) : 
    AsgTool(name),
    METAssociator(name)
  {
    // 0 is delta-R geometrical matching
    // 1 is using TopoClusterLink decoration on clusters
    declareProperty( "TCMatchMethod",     m_tcMatch_method = DeltaR );

    declareProperty( "TCMatchMaxRat",     m_tcMatch_maxRat = 1.5    );
    declareProperty( "TCMatchDeltaR",     m_tcMatch_dR     = 0.1    );

    declareProperty( "ExtraTrackMatchDeltaR", m_extraTrkMatch_dR = 0.05 );
  }

  // Destructor
  ///////////////
  METEgammaAssociator::~METEgammaAssociator()
  {}

  // Athena algtool's Hooks
  ////////////////////////////
  StatusCode METEgammaAssociator::initialize()
  {
    ATH_CHECK( METAssociator::initialize() );
    ATH_MSG_VERBOSE ("Initializing " << name() << "...");
    switch(m_tcMatch_method) {
    case DeltaR: ATH_MSG_INFO("Egamma-topocluster matching uses DeltaR check."); break;
    case ClusterLink: ATH_MSG_INFO("Egamma-topocluster matching uses topocluster links."); break;
    default:
      ATH_MSG_WARNING( "Invalid topocluster match method configured!" );
      return StatusCode::FAILURE;
    } 
    return StatusCode::SUCCESS;
  }

  StatusCode METEgammaAssociator::finalize()
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

  //**********************************************************************
  // Get Egamma constituents
  StatusCode METEgammaAssociator::extractTopoClusters(const xAOD::IParticle* obj,
						      std::vector<const xAOD::IParticle*>& tclist,
						      const met::METAssociator::ConstitHolder& constits) const
  {
    const Egamma *eg = static_cast<const Egamma*>(obj);
    // safe to assume a single SW cluster?
    // will do so for now...
    const CaloCluster* swclus = eg->caloCluster();

    // the matching strategy depends on how the cluster container is sorted
    // easier if it's sorted in descending pt order
    // we'll worry about optimisation later
    std::vector<const IParticle*> inputTC;
    inputTC.reserve(10);
    
    if(m_tcMatch_method==DeltaR) {
      for(const auto& cl : *constits.tcCont) {
	// this can probably be done more elegantly
	if(P4Helpers::isInDeltaR(*swclus,*cl,m_tcMatch_dR,m_useRapidity) && cl->e()>FLT_MIN) {
	  // could consider also requirements on the EM fraction or depth
	  inputTC.push_back(cl);
	} // match TC in a cone around SW cluster
      }
      ATH_MSG_VERBOSE("Found " << inputTC.size() << " nearby topoclusters");
      std::sort(inputTC.begin(),inputTC.end(),greaterPt);
    } else if(m_tcMatch_method==ClusterLink) {
      static const SG::AuxElement::ConstAccessor<std::vector<ElementLink<CaloClusterContainer> > > tcLinkAcc("constituentClusterLinks");
      // Fill a vector of vectors
      for(const auto& el : tcLinkAcc(*swclus)) {
	if (el.isValid()) {
          inputTC.push_back(*el);
        } else {
          ATH_MSG_VERBOSE("Found invalid topocluster link");
        }
      }
      ATH_MSG_VERBOSE("Found " << inputTC.size() << " linked topoclusters");
    } else {
      ATH_MSG_WARNING( "Invalid topocluster match method configured!" );
      return StatusCode::FAILURE;
    } 

    ATH_CHECK( selectEgammaClusters(swclus, inputTC, tclist) );

    return StatusCode::SUCCESS;
  }


  StatusCode METEgammaAssociator::extractTracks(const xAOD::IParticle* obj,
						std::vector<const xAOD::IParticle*>& constlist,
						const met::METAssociator::ConstitHolder& constits) const
  {
    const xAOD::Egamma *eg = static_cast<const xAOD::Egamma*>(obj);
    std::set<const xAOD::TrackParticle*> trackset; // use a set for duplicate-free retrieval
    ATH_CHECK( selectEgammaTracks(eg, constits.trkCont, trackset) );
    for(const auto& track : trackset) {
      if(acceptTrack(track,constits.pv) && isGoodEoverP(track)) {
	constlist.push_back(track);
      }
    }
    return StatusCode::SUCCESS;
  }

  StatusCode METEgammaAssociator::extractPFO(const xAOD::IParticle* obj,
					     std::vector<const xAOD::IParticle*>& pfolist,
					     const met::METAssociator::ConstitHolder& constits,
					     std::map<const IParticle*,MissingETBase::Types::constvec_t> &/*momenta*/) const
  {
    const xAOD::Egamma *eg = static_cast<const xAOD::Egamma*>(obj);
    // safe to assume a single SW cluster?
    // will do so for now...
    const xAOD::IParticle* swclus = eg->caloCluster();

    //std::cout<<"!!!!!!!!!! METEgammaAssociator::extractPFO: electrons  --- standard method"<<std::endl;

    // Preselect PFOs based on proximity: dR<0.4
    std::vector<const xAOD::PFO*> nearbyPFO;
    nearbyPFO.reserve(20);
    for(const auto& pfo : *constits.pfoCont) {
      if(P4Helpers::isInDeltaR(*pfo, *swclus, 0.4, m_useRapidity)) {
	// We set a small -ve pt for cPFOs that were rejected
	// by the ChargedHadronSubtractionTool
	const static SG::AuxElement::ConstAccessor<char> PVMatchedAcc("matchedToPV");	
	if( ( fabs(pfo->charge())<FLT_MIN && pfo->e() > FLT_MIN ) ||
	    ( fabs(pfo->charge())>FLT_MIN && PVMatchedAcc(*pfo)
	      && ( !m_cleanChargedPFO || isGoodEoverP(pfo->track(0)) ) )
	    ) {
	  nearbyPFO.push_back(pfo);
	} // retain +ve E neutral PFOs and charged PFOs passing PV association
      } // DeltaR check
    } // PFO loop
    ATH_MSG_VERBOSE("Found " << nearbyPFO.size() << " nearby pfos");

    std::set<const xAOD::TrackParticle*> trackset; // use a set for duplicate-free retrieval
    ATH_CHECK( selectEgammaTracks(eg, constits.trkCont, trackset) );
    for(const auto& track : trackset) {
      for(const auto& pfo : nearbyPFO) {
	if(fabs(pfo->charge())>FLT_MIN && pfo->track(0) == track) {
	  pfolist.push_back(pfo);
	} // PFO/track match
      } // PFO loop
    } // Track loop
    double eg_cl_e = swclus->e();

    // the matching strategy depends on how the cluster container is sorted
    // easier if it's sorted in descending pt order
    // ideally this should be done using cell matching, but we can't use the links from topoclusters reliably
    // because some PFOs don't correspond to the original TC
    bool doSum = true;
    double sumE_pfo = 0.;
    const IParticle* bestbadmatch = 0;
    std::sort(nearbyPFO.begin(),nearbyPFO.end(),greaterPtPFO);
    for(const auto& pfo : nearbyPFO) {
      // Skip charged PFOs, as we already matched them
      if(fabs(pfo->charge())>FLT_MIN || !P4Helpers::isInDeltaR(*pfo, *swclus, m_tcMatch_dR, m_useRapidity)) {continue;}
      // Handle neutral PFOs like topoclusters
      double pfo_e = pfo->eEM();
      // skip cluster if it's above our bad match threshold or outside the matching radius
      if(pfo_e>m_tcMatch_maxRat*eg_cl_e) {
        ATH_MSG_VERBOSE("Reject topocluster in sum. Ratio vs eg cluster: " << (pfo_e/eg_cl_e));
    	if( !bestbadmatch || (fabs(pfo_e/eg_cl_e-1.) < fabs(bestbadmatch->e()/eg_cl_e-1.)) ) bestbadmatch = pfo;
	continue;
      }

      ATH_MSG_VERBOSE("E match with new nPFO: " << fabs(sumE_pfo+pfo_e - eg_cl_e) / eg_cl_e);
      if( (doSum = fabs(sumE_pfo+pfo_e-eg_cl_e) < fabs(sumE_pfo - eg_cl_e)) ) {
	pfolist.push_back(pfo);
	sumE_pfo += pfo_e;
    	ATH_MSG_VERBOSE("Accept pfo with pt " << pfo->pt() << ", e " << pfo->e() << " in sum.");
    	ATH_MSG_VERBOSE("Energy ratio of nPFO to eg: " << pfo_e / eg_cl_e);
    	ATH_MSG_VERBOSE("E match with new PFO: " << fabs(sumE_pfo+pfo_e - eg_cl_e) / eg_cl_e);
      } // if we will retain the topocluster
      else {break;}
    } // loop over nearby clusters
    if(sumE_pfo<FLT_MIN && bestbadmatch) {
      ATH_MSG_VERBOSE("No better matches found -- add bad match topocluster with pt "
    		      << bestbadmatch->pt() << ", e " << bestbadmatch->e() << ".");
      pfolist.push_back(bestbadmatch);
    }

    return StatusCode::SUCCESS;
  }


  // extractPFO modified for W measurements
  StatusCode METEgammaAssociator::GetPFOWana(const xAOD::IParticle* obj,
               std::vector<const xAOD::IParticle*>& pfolist,
               const met::METAssociator::ConstitHolder& constits,
               std::map<const IParticle*,MissingETBase::Types::constvec_t> & momenta,
               std::vector<double>& vPhiRnd,
               unsigned int& lept_count,
               float& UEcorr) const
  {
    //std::cout<<"!!!!!!!!!! METEgammaAssociator::GetPFOWana: electrons --- new method"<<std::endl;
    //std::cout<<"!!!!!!!!!! METEgammaAssociator::GetPFOWana: m_Drcone = "<<m_Drcone<<std::endl;
    

    // Step 1. Cnstructing association electron-PFO map
    const xAOD::Egamma *eg = static_cast<const xAOD::Egamma*>(obj);
    const xAOD::IParticle* swclus = eg->caloCluster();

    std::vector<const xAOD::PFO*> nearbyPFO;
    nearbyPFO.reserve(20);

    // Preselect charged and neutral PFOs, based on proximity: dR < m_Drcone
    for(const auto& pfo : *constits.pfoCont) { 
      if(P4Helpers::isInDeltaR(*pfo, *swclus, 0.2, m_useRapidity)) {
        const static SG::AuxElement::ConstAccessor<char> PVMatchedAcc("matchedToPV");
        if( ( fabs(pfo->charge())<FLT_MIN && pfo->e() > FLT_MIN ) ||
        ( fabs(pfo->charge())>FLT_MIN && PVMatchedAcc(*pfo)  && ( !m_cleanChargedPFO || isGoodEoverP(pfo->track(0)) ) ) ) {
          nearbyPFO.push_back(pfo); 
        } // quality cuts        
      } // DeltaR check
    } // PFO loop

    // Fill charged PFOs from nearbyPFO to pfolist
    for(const auto& pfo : nearbyPFO) {
      if( fabs(pfo->charge()) > FLT_MIN ) {
        pfolist.push_back(pfo);
      }
    }

    // Sort elements in nearbyPFO
    std::sort(nearbyPFO.begin(),nearbyPFO.end(),greaterPtPFO);

    // Fill neutral PFOs from nearbyPFO to pfolist
    for(const auto& pfo : nearbyPFO) {
      if( fabs(pfo->charge()) > FLT_MIN )
        continue;
      pfolist.push_back(pfo);
    }

    // Step 2. Calculating UE energy correction for given lepton
    if(swclus){
      TLorentzVector tv_UEcorr;  // vector of UE correction
      TLorentzVector tv_swclus; // make TLorentzVector from swclus to simplify operations
      tv_swclus.SetPtEtaPhiE( swclus->pt(), swclus->eta(), swclus->phi(), swclus->e() );
  
      std::pair <double, double> eta_rndphi;
      eta_rndphi.first = tv_swclus.Eta();
      eta_rndphi.second = vPhiRnd[lept_count];
      lept_count++;
  
      //std::cout<<"start looping over PFO to calculate correction"<<std::endl;
      for(const auto& pfo_itr : *constits.pfoCont) { // loop over PFOs
        if( pfo_itr->e() < 0)
          continue;
        //std::cout<<"passed basic cut pfo_itr->e() < 0 "<<std::endl;
        TLorentzVector tv_pfo;
        tv_pfo.SetPtEtaPhiE( pfo_itr->pt(), pfo_itr->eta(), pfo_itr->phi(), pfo_itr->e() );
        float dR = 0.;
        deltaR( tv_pfo.Eta(), tv_pfo.Phi(), eta_rndphi.first,  eta_rndphi.second, dR);
        //std::cout<<"dR < m_Drcone requirement:"<<std::endl;
        if( dR < m_Drcone ){
          //std::cout<<"passed"<<std::endl;
          float angle;
          METEgammaAssociator::deltaPhi(tv_swclus.Phi(), tv_pfo.Phi(), angle);
          if( tv_swclus.Phi() <  tv_pfo.Phi() )
            angle = -1. * angle;
          tv_pfo.RotateZ(angle);
          tv_UEcorr += tv_pfo;  // summing PFOs of UE for correction
        } // m_Drcone requirement   
      } // loop over PFOs
      UEcorr = tv_UEcorr.Pt();  // Pt of UE correction
  } // swclus existance requirement

    return StatusCode::SUCCESS;
  }

  StatusCode METEgammaAssociator::hadrecoil_PFO(std::vector<const xAOD::IParticle*> hardObjs, 
                                                const met::METAssociator::ConstitHolder& constits, 
                                                TLorentzVector& HR,
                                                std::vector<double>& vPhiRnd) const
  {
    //std::cout << "METEgammaAssociator::hadrecoil_PFO " << std::endl;

    // 1. Summing all PFOs
    for(const auto& pfo_itr : *constits.pfoCont) {
      if( pfo_itr->pt() < 0 || pfo_itr->e() < 0 ) // sanity check
        continue;
      TLorentzVector pfo_tmp;
      pfo_tmp.SetPtEtaPhiE( pfo_itr->pt(), pfo_itr->eta(), pfo_itr->phi(), pfo_itr->e() );
      HR += pfo_tmp;
    }
    //std::cout << "HR->pt() HR->eta() HR->phi() HR->e()          : " << HR.Pt() << "  " << HR.Eta() << "  " << HR.Phi() << "  " << HR.E() << std::endl;


    // 2. Subtracting PFOs mathed to electrons from HR 
    // std::vector<const xAOD::Egamma*> eg = static_cast<std::vector<const xAOD::Egamma*>>(hardObjs);
    std::vector<const xAOD::Egamma*> eg;
    for(const auto& obj_i : hardObjs){
      if(obj_i->pt()<5e3 && obj_i->type() != xAOD::Type::Muon) // similar to METAssociator.cxx, probaly xAOD::Type::Muon part should be removed
        continue;
      eg.push_back( static_cast<const xAOD::Egamma*>(obj_i) );
    }
    //std::cout << "eg.size()  = " << eg.size() << std::endl;

 
    std::vector<const xAOD::IParticle*> swclus_orig;
    for(const auto& eg_i : eg)
      if( eg_i->caloCluster() )
        swclus_orig.push_back( eg_i->caloCluster() );
    //std::cout << "swclus_orig.size()  = " << swclus_orig.size() << std::endl;


    std::vector<TLorentzVector> swclus;
    for(const auto& swclus_orig_i : swclus_orig){
      TLorentzVector sw_curr;
      sw_curr.SetPtEtaPhiE( swclus_orig_i->pt(), swclus_orig_i->eta(), swclus_orig_i->phi(), swclus_orig_i->e() );
      swclus.push_back(sw_curr);
    }
    //std::cout << "swclus.size()  = " << swclus.size() << std::endl;


    /*
    std::cout << "METEgammaAssociator::hadrecoil_PFO debugging : "<< std::endl;

    for(const auto& eg_i : eg)
      std::cout << "eg_i->pt() = " << eg_i->pt() << std::endl;  

    for(const auto& swclus_i : swclus)
      std::cout << "swclus_i.pt() = " << swclus_i.Pt() << std::endl;
    */  

    for(const auto& pfo_i : *constits.pfoCont) {  // charged and neutral PFOs
      //std::cout << "in the pfoCont loop" << std::endl;
      if( pfo_i->pt() < 0 || pfo_i->e() < 0 ) // sanity check
        continue;
  
      TLorentzVector pfo_curr;
      pfo_curr.SetPtEtaPhiE( pfo_i->pt(), pfo_i->eta(), pfo_i->phi(), pfo_i->e() );
      //std::cout << "created pfo_curr" << std::endl;
      for(const auto& swclus_i : swclus) {
        float dR = 0.;
        //std::cout << "swclus_i->eta() swclus_i->phi() swclus_i->pt(): " << swclus_i.Eta() << "  " << swclus_i.Phi() << "  " << swclus_i.Pt() << std::endl;
        deltaR( pfo_curr.Eta(), pfo_curr.Phi(), swclus_i.Eta(), swclus_i.Phi(), dR);
        if( dR < m_Drcone )
          HR -= pfo_curr;
      } // over swclus
    } // over PFOs
    //std::cout << "HR->pt() HR->eta() HR->phi() HR->e() corrected: " << HR.Pt() << "  " << HR.Eta() << "  " << HR.Phi() << "  " << HR.E() << std::endl;

    // 3. Get random phi
    //std::vector<double> vPhiRnd;
    unsigned int seed;
    TRandom3 hole;


    //std::cout << "before swclus loop" << std::endl;
    for(const auto& swclus_i : swclus) {
      seed = floor( swclus_i.Pt() * 1.e3 );      
      hole.SetSeed(seed);
      //std::cout << "seed = " << seed << std::endl;
    }
  
    for(const auto& swclus_i : swclus) {
      bool isNextToPart(true);
      bool isNextToHR(true);
      double Rnd;
      //std::cout<<"in the lepton loop"<<std::endl;
  
      while(isNextToPart || isNextToHR ){
        isNextToPart = false; 
        isNextToHR = true;
  
        Rnd = hole.Uniform( -TMath::Pi(), TMath::Pi() );
        float dR = 0.;
        this->METEgammaAssociator::deltaR(HR.Eta(), HR.Phi(), swclus_i.Eta(), Rnd, dR);
  
        if(dR > m_MinDistCone) 
          isNextToHR = false;
  
        for(const auto& swclus_j : swclus) {
          dR = 0.;
          this->METEgammaAssociator::deltaR( swclus_i.Eta(), Rnd, swclus_j.Eta(), swclus_j.Phi(), dR );
          if(dR < m_MinDistCone)
            isNextToPart = true;
        } // swclus_j
      } // while isNextToPart, isNextToHR
      //std::cout << "pushback random : " << Rnd << std::endl;

      vPhiRnd.push_back(Rnd);
    } // swclus_i

    return StatusCode::SUCCESS;
  }

  
  void METEgammaAssociator::deltaR(float eta1, float phi1, float eta2, float phi2, float& result) const {
    float deta = eta1 - eta2;  
    float dphi = 0;
    METEgammaAssociator::deltaPhi(phi1, phi2, dphi);    
    
    result = sqrt(deta*deta + dphi*dphi);  
  
    return;
  } 
  
  
  void METEgammaAssociator::deltaPhi(float phi1, float phi2, float& result) const {
    float dphi = std::fabs(phi1 - phi2);
    if (dphi > TMath::Pi()) dphi = 2*TMath::Pi() - dphi;
  
    result = dphi;
    
    return;
  }


  //**********************************************************************
  // Select Egamma tracks & clusters

  StatusCode METEgammaAssociator::selectEgammaClusters(const xAOD::CaloCluster* swclus,
						       const std::vector<const IParticle*>& inputTC,
						       std::vector<const xAOD::IParticle*>& tclist) const
  {    
    double eg_cl_e = swclus->e();

    bool doSum = true;
    double sumE_tc = 0.;
    const IParticle* bestbadmatch = 0;
    for(const auto& cl : inputTC) {
      double tcl_e = cl->e();
      // skip cluster if it's above our bad match threshold
      // retain pointer of the closest matching cluster in case no better is found
      if(tcl_e>m_tcMatch_maxRat*eg_cl_e) {
    	ATH_MSG_VERBOSE("Reject topocluster in sum. Ratio vs eg cluster: " << (tcl_e/eg_cl_e));
    	if( !bestbadmatch || (fabs(tcl_e/eg_cl_e-1.) < fabs(bestbadmatch->e()/eg_cl_e-1.)) ) bestbadmatch = cl;
    	continue;
      }

      ATH_MSG_VERBOSE("E match with new cluster: " << fabs(sumE_tc+tcl_e - eg_cl_e) / eg_cl_e);
      if( (doSum = (fabs(sumE_tc+tcl_e - eg_cl_e) < fabs(sumE_tc - eg_cl_e))) ) {
    	ATH_MSG_VERBOSE("Accept topocluster with pt " << cl->pt() << ", e " << cl->e() << " in sum.");
    	ATH_MSG_VERBOSE("Energy ratio of nPFO to eg: " << tcl_e / eg_cl_e);
    	ATH_MSG_VERBOSE("E match with new cluster: " << fabs(sumE_tc+tcl_e - eg_cl_e) / eg_cl_e);
    	tclist.push_back(cl);
    	sumE_tc += tcl_e;
      } // if we will retain the topocluster
    } // loop over nearby clusters
    if(sumE_tc<FLT_MIN && bestbadmatch) {
      ATH_MSG_VERBOSE("No better matches found -- add bad match topocluster with pt "
    		      << bestbadmatch->pt() << ", e " << bestbadmatch->e() << ".");
      tclist.push_back(bestbadmatch);
    }
    return StatusCode::SUCCESS;
  }

  StatusCode METEgammaAssociator::selectEgammaTracks(const xAOD::Egamma* eg,
						     const xAOD::TrackParticleContainer* trkCont,
						     std::set<const xAOD::TrackParticle*>& tracklist) const
  {
    // switch to using egamma helpers for track extraction
    // set ensures that there's no duplication
    const std::set<const xAOD::TrackParticle*> egtracks = EgammaHelpers::getTrackParticles(eg);
    for(const auto& track : egtracks) {
	ATH_MSG_VERBOSE("Accept " << eg->type() << " track " << track << " px, py = " << track->p4().Px() << ", " << track->p4().Py());
	tracklist.insert(track);
    } // end initial egamma track loop

    // for objects with ambiguous author, grab the tracks matched to the counterpart ambiguous object too
    // set ensures that there's no duplication
    if (eg->author() & xAOD::EgammaParameters::AuthorAmbiguous && eg->ambiguousObject()) {
      const std::set<const xAOD::TrackParticle*> ambitracks = EgammaHelpers::getTrackParticles(eg->ambiguousObject());
      for(const auto& track : egtracks) {
	ATH_MSG_VERBOSE("Accept ambiguous " << eg->type() << " track " << track << " px, py = " << track->p4().Px() << ", " << track->p4().Py());
	tracklist.insert(track);
      }
    } // end ambiguous track case

    // in a small dR window, also accept tracks without an IBL hit
    for(const auto& track : *trkCont) {
      if(P4Helpers::isInDeltaR(*track, *eg, m_extraTrkMatch_dR, m_useRapidity)) {
	// dR check should be faster than track summary retrieval
	uint8_t expect_innermostHit(false);
	uint8_t N_innermostHit(false);
	uint8_t expect_nextToInnermostHit(false);
	uint8_t N_nextToInnermostHit(false);
	if( !track->summaryValue(expect_innermostHit, expectInnermostPixelLayerHit)
	    || !track->summaryValue(expect_nextToInnermostHit, expectNextToInnermostPixelLayerHit)) {
	  ATH_MSG_WARNING("Track summary retrieval failed for 'expect(NextTo)InnermostPixelLayerHit'");
	  return StatusCode::FAILURE;
	}
	if(expect_innermostHit) {
	  if( !track->summaryValue(N_innermostHit, numberOfInnermostPixelLayerHits) ) {
	    ATH_MSG_WARNING("Track summary retrieval failed for 'numberOfInnermostPixelLayerHits'");
	    return StatusCode::FAILURE;
	    if(N_innermostHit==0 ) {
	      ATH_MSG_VERBOSE("Accept nearby track w/o innermost hit");
	      tracklist.insert(track);
	    }
	  }
	} else if(expect_nextToInnermostHit) {
	  if( !track->summaryValue(N_nextToInnermostHit, numberOfNextToInnermostPixelLayerHits) ) {
	    ATH_MSG_WARNING("Track summary retrieval failed for 'numberOfNextToInnermostPixelLayerHits'");
	    return StatusCode::FAILURE;
	    if(N_nextToInnermostHit==0 ) {
	      ATH_MSG_VERBOSE("Accept nearby track w/o next-to-innermost hit");
	      tracklist.insert(track);
	    }
	  }
	}
	
      } // end dR check
    } // end extra track loop
    return StatusCode::SUCCESS;
  }

}
