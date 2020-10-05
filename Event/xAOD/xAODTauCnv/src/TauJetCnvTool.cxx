///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TauJetCnvTool.cxx 
// Implementation file for class TauJetCnvTool
// Author: Michel Janus janus@cern.ch
/////////////////////////////////////////////////////////////////// 


// xAODJetCnv includes
#include "TauJetCnvTool.h"

// STL includes

// FrameWork includes
#include "Gaudi/Property.h"
#include "AthenaKernel/errorcheck.h"


/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

namespace xAODMaker {
  // Constructors
  ////////////////
  TauJetCnvTool::TauJetCnvTool( const std::string& type,
				const std::string& name, 
				const IInterface* parent ) : 
    ::AthAlgTool( type, name, parent )
    , m_inDetTrackParticles("InDetTrackParticles")
    , m_jets("AntiKt4LCTopoJets")
  {
    declareInterface< ITauJetCnvTool > (this);
    //
    // Property declaration
    // 
    declareProperty( "TrackContainerName", m_inDetTrackParticles );
    declareProperty( "JetContainerName", m_jets );
  }

  // Destructor
  ///////////////
  TauJetCnvTool::~TauJetCnvTool()
  {}

  // Athena Toolorithm's Hooks
  ////////////////////////////
  StatusCode TauJetCnvTool::initialize()
  {
    ATH_MSG_INFO ("Initializing " << name() << "...");

    return StatusCode::SUCCESS;
  }

  StatusCode TauJetCnvTool::convert(const Analysis::TauJetContainer* inputTaus, xAOD::TauJetContainer* xaodTauJets) const
  {  
    Analysis::TauJetContainer::const_iterator it  = inputTaus->begin();
    Analysis::TauJetContainer::const_iterator itE = inputTaus->end();

    for( ; it!= itE; ++it)
      {
	const Analysis::TauJet * tau = *it;

	ATH_MSG_DEBUG( "trying to convert tau with  pt="<< tau->pt() << " eta=" << tau->eta() << " phi=" << tau->phi() << " m=" << tau->m() );    
      
	xAOD::TauJet* xtaujet = new xAOD::TauJet();
	xaodTauJets->push_back( xtaujet );
    
	xtaujet->setP4( tau->pt(), tau->eta(), tau->phi(), tau->m() );

	ATH_MSG_DEBUG( "converted xtau with  pt="<< xtaujet->pt() << " eta=" << xtaujet->eta() << " phi=" << xtaujet->phi() << " m=" << xtaujet->m() );    

	ATH_MSG_DEBUG( "trying to convert tau jetseed 4-vector with  pt="<< tau->getHLV(TauJetParameters::JetSeed).perp() << " eta=" << tau->getHLV(TauJetParameters::JetSeed).eta() << " phi=" << tau->getHLV(TauJetParameters::JetSeed).phi() << " m=" << tau->getHLV(TauJetParameters::JetSeed).m() );    
     

	xtaujet->setP4(xAOD::TauJetParameters::JetSeed, tau->getHLV(TauJetParameters::JetSeed).perp(), tau->getHLV(TauJetParameters::JetSeed).eta(), tau->getHLV(TauJetParameters::JetSeed).phi(), tau->getHLV(TauJetParameters::JetSeed).m());

	ATH_MSG_DEBUG( "converted xtau jetseed 4-vector with  pt="<< xtaujet->ptJetSeed() << " eta=" << xtaujet->etaJetSeed() << " phi=" << xtaujet->phiJetSeed() << " m=" << xtaujet->mJetSeed() );  

	ATH_MSG_DEBUG( "trying to convert tau detectoraxis 4-vector with  pt="<< tau->getHLV(TauJetParameters::DetectorAxis).perp() << " eta=" << tau->getHLV(TauJetParameters::DetectorAxis).eta() << " phi=" << tau->getHLV(TauJetParameters::DetectorAxis).phi() << " m=" << tau->getHLV(TauJetParameters::DetectorAxis).m() );    
     
	xtaujet->setP4(xAOD::TauJetParameters::DetectorAxis, tau->getHLV(TauJetParameters::DetectorAxis).perp(), tau->getHLV(TauJetParameters::DetectorAxis).eta(), tau->getHLV(TauJetParameters::DetectorAxis).phi(), tau->getHLV(TauJetParameters::DetectorAxis).m());

	ATH_MSG_DEBUG( "converted xtau detectoraxis 4-vector with  pt="<< xtaujet->ptDetectorAxis() << " eta=" << xtaujet->etaDetectorAxis() << " phi=" << xtaujet->phiDetectorAxis() << " m=" << xtaujet->mDetectorAxis() );    


	ATH_MSG_DEBUG( "trying to convert tau intermediateaxis 4-vector with  pt="<< tau->getHLV(TauJetParameters::IntermediateAxis).perp() << " eta=" << tau->getHLV(TauJetParameters::IntermediateAxis).eta() << " phi=" << tau->getHLV(TauJetParameters::IntermediateAxis).phi() << " m=" << tau->getHLV(TauJetParameters::IntermediateAxis).m() );    
     
	xtaujet->setP4(xAOD::TauJetParameters::IntermediateAxis, tau->getHLV(TauJetParameters::IntermediateAxis).perp(), tau->getHLV(TauJetParameters::IntermediateAxis).eta(), tau->getHLV(TauJetParameters::IntermediateAxis).phi(), tau->getHLV(TauJetParameters::IntermediateAxis).m());

	ATH_MSG_DEBUG( "converted xtau intermediateaxis 4-vector with  pt="<< xtaujet->ptIntermediateAxis() << " eta=" << xtaujet->etaIntermediateAxis() << " phi=" << xtaujet->phiIntermediateAxis() << " m=" << xtaujet->mIntermediateAxis() );    


	ATH_MSG_DEBUG( "trying to convert tau tauenergyscale 4-vector with  pt="<< tau->getHLV(TauJetParameters::TauEnergyScale).perp() << " eta=" << tau->getHLV(TauJetParameters::TauEnergyScale).eta() << " phi=" << tau->getHLV(TauJetParameters::TauEnergyScale).phi() << " m=" << tau->getHLV(TauJetParameters::TauEnergyScale).m() );    
     
	xtaujet->setP4(xAOD::TauJetParameters::TauEnergyScale, tau->getHLV(TauJetParameters::TauEnergyScale).perp(), tau->getHLV(TauJetParameters::TauEnergyScale).eta(), tau->getHLV(TauJetParameters::TauEnergyScale).phi(), tau->getHLV(TauJetParameters::TauEnergyScale).m());

	ATH_MSG_DEBUG( "converted xtau tauenergyscale 4-vector with  pt="<< xtaujet->ptTauEnergyScale() << " eta=" << xtaujet->etaTauEnergyScale() << " phi=" << xtaujet->phiTauEnergyScale() << " m=" << xtaujet->mTauEnergyScale() );    

	ATH_MSG_DEBUG( "trying to convert tau intermediateaxis 4-vector with  pt="<< tau->getHLV(TauJetParameters::TauEtaCalib).perp() << " eta=" << tau->getHLV(TauJetParameters::TauEtaCalib).eta() << " phi=" << tau->getHLV(TauJetParameters::TauEtaCalib).phi() << " m=" << tau->getHLV(TauJetParameters::TauEtaCalib).m() );    
     
	xtaujet->setP4(xAOD::TauJetParameters::TauEtaCalib, tau->getHLV(TauJetParameters::TauEtaCalib).perp(), tau->getHLV(TauJetParameters::TauEtaCalib).eta(), tau->getHLV(TauJetParameters::TauEtaCalib).phi(), tau->getHLV(TauJetParameters::TauEtaCalib).m());

	ATH_MSG_DEBUG( "converted xtau intermediateaxis 4-vector with  pt="<< xtaujet->ptTauEtaCalib() << " eta=" << xtaujet->etaTauEtaCalib() << " phi=" << xtaujet->phiTauEtaCalib() << " m=" << xtaujet->mTauEtaCalib() );    

	//trying to set element links
	ATH_MSG_DEBUG( "trying to set element links for tau with  numTrack=" << tau->numTrack() );    

	// this method leads to a segfault when converting Run 1 BS to xAOD.
	// It was decided that the links are not needed in the reprocessed files
	// (for data analysis), therefore is disabled (ATR-13562)
	// 
	// setLinks((*tau), (*xtaujet));

	ATH_MSG_DEBUG( "converted xaod tau with numTrack=" << xtaujet->nTracks() );    


	ATH_MSG_DEBUG( "trying to convert tau with  ROIWord=" << tau->ROIWord() );    
	xtaujet->setROIWord( tau->ROIWord());
	ATH_MSG_DEBUG( "converted xaod tau with ROIWord=" << xtaujet->ROIWord() );    

	ATH_MSG_DEBUG( "trying to convert tau with  charge=" << tau->charge() );    
	xtaujet->setCharge( tau->charge() );
	ATH_MSG_DEBUG( "converted xaod tau with charge=" << xtaujet->charge() );    
      
	//set PID variables

	ATH_MSG_DEBUG( "trying to convert tau with  BDTElescore=" << tau->tauID()->discriminant(TauJetParameters::BDTEleScore)   );
	xtaujet->setDiscriminant(xAOD::TauJetParameters::BDTEleScore    , tau->tauID()->discriminant(TauJetParameters::BDTEleScore) );
	ATH_MSG_DEBUG( "converted xaod tau with BDTEleScore=" <<  xtaujet->discriminant(xAOD::TauJetParameters::BDTEleScore ) );

	xtaujet->setDiscriminant(xAOD::TauJetParameters::BDTJetScoreSigTrans    , tau->tauID()->discriminant(TauJetParameters::BDTJetScoreSigTrans) );
	//	xtaujet->setDiscriminant(xAOD::TauJetParameters::BDTJetScoreBkgTrans    , tau->tauID()->discriminant(TauJetParameters::BDTJetScoreBkgTrans) );
	xtaujet->setDiscriminant(xAOD::TauJetParameters::BDTJetScore    , tau->tauID()->discriminant(TauJetParameters::BDTJetScore) );
	// xtaujet->setDiscriminant(xAOD::TauJetParameters::Likelihood     , tau->tauID()->discriminant(TauJetParameters::Likelihood) );
	// xtaujet->setDiscriminant(xAOD::TauJetParameters::SafeLikelihood , tau->tauID()->discriminant(TauJetParameters::SafeLikelihood) );

	ATH_MSG_DEBUG( "trying to convert tau with  MuonVeto=" << tau->tauID()->isTau(TauJetParameters::MuonVeto)   );
	xtaujet->setIsTau(xAOD::TauJetParameters::MuonVeto           ,   tau->tauID()->isTau(TauJetParameters::MuonVeto) );

	xtaujet->setIsTau(xAOD::TauJetParameters::JetBDTSigLoose     ,   tau->tauID()->isTau(TauJetParameters::JetBDTSigLoose) );
	xtaujet->setIsTau(xAOD::TauJetParameters::JetBDTSigMedium    ,   tau->tauID()->isTau(TauJetParameters::JetBDTSigMedium) );
	xtaujet->setIsTau(xAOD::TauJetParameters::JetBDTSigTight     ,   tau->tauID()->isTau(TauJetParameters::JetBDTSigTight) );
	xtaujet->setIsTau(xAOD::TauJetParameters::EleBDTLoose	   ,   tau->tauID()->isTau(TauJetParameters::EleBDTLoose) );
	xtaujet->setIsTau(xAOD::TauJetParameters::EleBDTMedium       ,   tau->tauID()->isTau(TauJetParameters::EleBDTMedium) );
	xtaujet->setIsTau(xAOD::TauJetParameters::EleBDTTight        ,   tau->tauID()->isTau(TauJetParameters::EleBDTTight) );         

	ATH_MSG_DEBUG( "set individual details variables"  );

	//set individual details variables

	const Analysis::TauCommonDetails* commonDetails(tau->details<Analysis::TauCommonDetails>());
	if(commonDetails != NULL)
	  {
	    int tempint = 0;

	    ATH_MSG_DEBUG( "tau with nCharged " << tau->numTrack() );
	    xtaujet->setDetail(xAOD::TauJetParameters::nCharged , static_cast<int>( tau->numTrack() ) );			  
	    if( xtaujet->detail( xAOD::TauJetParameters::nCharged, tempint ) )
	      ATH_MSG_DEBUG( "converted xaod tau with nPi0 " << tempint );


	    ATH_MSG_DEBUG( "tau with pi0ConeDR " << commonDetails->Pi0ConeDR() );
	    xtaujet->setPi0ConeDR(      commonDetails->Pi0ConeDR() );			  
	    ATH_MSG_DEBUG( "converted xaod tau with pi0ConeDR " << xtaujet->pi0ConeDR() );

	    ATH_MSG_DEBUG( "tau with trackFilterProngs " << commonDetails->TrackFilterProngs() );
	    xtaujet->setTrackFilterProngs(      commonDetails->TrackFilterProngs() );			  
	    ATH_MSG_DEBUG( "converted xaod tau with trackFilterProngs " << xtaujet->trackFilterProngs() );

	    ATH_MSG_DEBUG( "tau with trackFilterQuality " << commonDetails->TrackFilterQuality() );
	    xtaujet->setTrackFilterQuality(      commonDetails->TrackFilterQuality() );			  
	    ATH_MSG_DEBUG( "converted xaod tau with trackFilterQuality " << xtaujet->trackFilterQuality() );


	    // for (unsigned int i = 0; i < commonDetails->TrackFilterPass().size(); ++i) 
	    //   {

	    // 	ATH_MSG_DEBUG( " tau with trackFilterPass " << commonDetails->TrackFilterPass().at(i) << " for track " << i );

	    // 	xtaujet->setTrackFlag(*getNewTrackLink(tau->conversionTrackLinkVector().at(i), m_inDetTrackParticles), xAOD::TauJetParameters::failTrackFilter, commonDetails->TrackFilterPass().at(i) );
	    // 	// xtaujet->setTrackFilterPass(  i,     static_cast<int> (commonDetails->TrackFilterPass().at(i)) );			  

	    // 	// ATH_MSG_DEBUG( "converted xaod tau with trackFilterPass " << xtaujet->trackFlag(tau->track(i), xAOD::TauJetParameters::failTrackFilter )  << " for track " << i  );

	    //   }



	    ATH_MSG_DEBUG( "found details container for this tau with SeedCalo_EMRadius " << commonDetails->seedCalo_EMRadius() );

	    xtaujet->setDetail(xAOD::TauJetParameters::ipZ0SinThetaSigLeadTrk ,      static_cast<float>( commonDetails->ipZ0SinThetaSigLeadTrk() ) );			  
	    xtaujet->setDetail(xAOD::TauJetParameters::etOverPtLeadTrk        ,      static_cast<float>( commonDetails->etOverPtLeadTrk() ) );				  
	    xtaujet->setDetail(xAOD::TauJetParameters::leadTrkPt              ,      static_cast<float>( commonDetails->leadTrkPt() ) );				  
	    xtaujet->setDetail(xAOD::TauJetParameters::ipSigLeadTrk           ,      static_cast<float>( commonDetails->ipSigLeadTrk() ) );				  
	    // xtaujet->setDetail(xAOD::TauJetParameters::ipSigLeadLooseTrk      ,      static_cast<float>( commonDetails->ipSigLeadLooseTrk() ) );			  
	    // xtaujet->setDetail(xAOD::TauJetParameters::etOverPtLeadLooseTrk   ,      static_cast<float>( commonDetails->etOverPtLeadLooseTrk() ) );			  
	    // xtaujet->setDetail(xAOD::TauJetParameters::leadLooseTrkPt         ,      static_cast<float>( commonDetails->leadLooseTrkPt() ) );				  
	    // xtaujet->setDetail(xAOD::TauJetParameters::chrgLooseTrk           ,      static_cast<float>( commonDetails->chrgLooseTrk() ) );				  
	    xtaujet->setDetail(xAOD::TauJetParameters::massTrkSys             ,      static_cast<float>( commonDetails->massTrkSys() ) );				  
	    xtaujet->setDetail(xAOD::TauJetParameters::trkWidth2              ,      static_cast<float>( commonDetails->trkWidth2() ) );				  
	    xtaujet->setDetail(xAOD::TauJetParameters::trFlightPathSig        ,      static_cast<float>( commonDetails->trFlightPathSig() ) );				  
	    // xtaujet->setDetail(xAOD::TauJetParameters::etEflow                ,      static_cast<float>( commonDetails->etEflow() ) );					  
	    // xtaujet->setDetail(xAOD::TauJetParameters::mEflow		      ,      static_cast<float>( commonDetails->mEflow() ) );					  
	    // xtaujet->setDetail(xAOD::TauJetParameters::ele_E237E277           ,      static_cast<float>( commonDetails->ele_E237E277() ) );				  
	    // xtaujet->setDetail(xAOD::TauJetParameters::ele_PresamplerFraction ,      static_cast<float>( commonDetails->ele_PresamplerFraction() ) );			  
	    // xtaujet->setDetail(xAOD::TauJetParameters::ele_ECALFirstFraction  ,      static_cast<float>( commonDetails->ele_ECALFirstFraction() ) );			  
	    xtaujet->setDetail(xAOD::TauJetParameters::numCells        ,      static_cast<int>( commonDetails->numCells() ) );				  
	    xtaujet->setDetail(xAOD::TauJetParameters::numTopoClusters        ,      static_cast<int>( commonDetails->numTopoClusters() ) );			  
	    xtaujet->setDetail(xAOD::TauJetParameters::numEffTopoClusters     ,      static_cast<float>( commonDetails->numEffTopoClusters() ) );			  
	    xtaujet->setDetail(xAOD::TauJetParameters::topoInvMass            ,      static_cast<float>( commonDetails->topoInvMass() ) );				  
	    xtaujet->setDetail(xAOD::TauJetParameters::effTopoInvMass         ,      static_cast<float>( commonDetails->effTopoInvMass() ) );				  
	    xtaujet->setDetail(xAOD::TauJetParameters::topoMeanDeltaR         ,      static_cast<float>( commonDetails->topoMeanDeltaR() ) );				  
	    xtaujet->setDetail(xAOD::TauJetParameters::effTopoMeanDeltaR      ,      static_cast<float>( commonDetails->effTopoMeanDeltaR() ) );			  
	    xtaujet->setDetail(xAOD::TauJetParameters::EMRadius      ,      static_cast<float>( commonDetails->seedCalo_EMRadius() ) );			  

	    float tempfloat = 0;
	    if (   xtaujet->detail(xAOD::TauJetParameters::EMRadius, tempfloat) )
	      ATH_MSG_DEBUG( "converted details for this xaod tau with SeedCalo_EMRadius " << tempfloat );
	    else
	      ATH_MSG_DEBUG( "error getting SeedCalo_EMRadius from xaod tau " );

	    xtaujet->setDetail(xAOD::TauJetParameters::hadRadius ,	      static_cast<float>( commonDetails->seedCalo_hadRadius() ) );			  
	    xtaujet->setDetail(xAOD::TauJetParameters::etEMAtEMScale ,      static_cast<float>( commonDetails->seedCalo_etEMAtEMScale() ) );			  
	    xtaujet->setDetail(xAOD::TauJetParameters::etHadAtEMScale ,     static_cast<float>( commonDetails->seedCalo_etHadAtEMScale() ) );			  
	    xtaujet->setDetail(xAOD::TauJetParameters::isolFrac ,	      static_cast<float>( commonDetails->seedCalo_isolFrac() ) );			  
	    xtaujet->setDetail(xAOD::TauJetParameters::centFrac ,	      static_cast<float>( commonDetails->seedCalo_centFrac() ) );			  
	    xtaujet->setDetail(xAOD::TauJetParameters::stripWidth2 ,	      static_cast<float>( commonDetails->seedCalo_stripWidth2() ) );			  
	    xtaujet->setDetail(xAOD::TauJetParameters::nStrip ,	      static_cast<int>( commonDetails->seedCalo_nStrip() ) );				  
	    // xtaujet->setDetail(xAOD::TauJetParameters::etEMCalib ,	      static_cast<float>( commonDetails->seedCalo_etEMCalib() ) );			  
	    // xtaujet->setDetail(xAOD::TauJetParameters::etHadCalib ,	      static_cast<float>( commonDetails->seedCalo_etHadCalib() ) );			  
	    // xtaujet->setDetail(xAOD::TauJetParameters::seedCalo_eta ,		      static_cast<float>( commonDetails->seedCalo_eta() ) );				  
	    // xtaujet->setDetail(xAOD::TauJetParameters::seedCalo_phi ,		      static_cast<float>( commonDetails->seedCalo_phi() ) );				  
	    // xtaujet->setDetail(xAOD::TauJetParameters::nIsolLooseTrk ,      static_cast<float>( commonDetails->seedCalo_nIsolLooseTrk() ) );			  
	    xtaujet->setDetail(xAOD::TauJetParameters::trkAvgDist ,	      static_cast<float>( commonDetails->seedCalo_trkAvgDist() ) );			  
	    xtaujet->setDetail(xAOD::TauJetParameters::trkRmsDist ,	      static_cast<float>( commonDetails->seedCalo_trkRmsDist() ) );			  
	    xtaujet->setDetail(xAOD::TauJetParameters::lead2ClusterEOverAllClusterE , static_cast<float>( commonDetails->seedCalo_lead2ClusterEOverAllClusterE() ) );	  
	    xtaujet->setDetail(xAOD::TauJetParameters::lead3ClusterEOverAllClusterE , static_cast<float>( commonDetails->seedCalo_lead3ClusterEOverAllClusterE() ) );	  
	    xtaujet->setDetail(xAOD::TauJetParameters::caloIso ,	      static_cast<float>( commonDetails->seedCalo_caloIso() ) );			  
	    xtaujet->setDetail(xAOD::TauJetParameters::caloIsoCorrected ,   static_cast<float>( commonDetails->seedCalo_caloIsoCorrected() ) );		  
	    xtaujet->setDetail(xAOD::TauJetParameters::dRmax ,	      static_cast<float>( commonDetails->seedCalo_dRmax() ) );				  
	    xtaujet->setDetail(xAOD::TauJetParameters::secMaxStripEt  ,	      static_cast<float>( commonDetails->seedTrk_secMaxStripEt() ) );				  
	    xtaujet->setDetail(xAOD::TauJetParameters::sumEMCellEtOverLeadTrkPt  ,   static_cast<float>( commonDetails->seedTrk_sumEMCellEtOverLeadTrkPt() ) );				  
	    xtaujet->setDetail(xAOD::TauJetParameters::hadLeakEt  ,	              static_cast<float>( commonDetails->seedTrk_hadLeakEt() ) );				  

	    // xtaujet->setDetail(xAOD::TauJetParameters::EM_TES_scale ,		      static_cast<float>( commonDetails->EM_TES_scale() ) );				  
	    //	    xtaujet->setDetail(xAOD::TauJetParameters::LC_TES_precalib ,	      static_cast<float>( commonDetails->LC_TES_precalib() ) );				  
	    xtaujet->setDetail(xAOD::TauJetParameters::cellBasedEnergyRing1 ,	      static_cast<float>( commonDetails->cellBasedEnergyRing1() ) );			  
	    xtaujet->setDetail(xAOD::TauJetParameters::cellBasedEnergyRing2 ,	      static_cast<float>( commonDetails->cellBasedEnergyRing2() ) );			  
	    xtaujet->setDetail(xAOD::TauJetParameters::cellBasedEnergyRing3 ,	      static_cast<float>( commonDetails->cellBasedEnergyRing3() ) );			  
	    xtaujet->setDetail(xAOD::TauJetParameters::cellBasedEnergyRing4 ,	      static_cast<float>( commonDetails->cellBasedEnergyRing4() ) );			  
	    xtaujet->setDetail(xAOD::TauJetParameters::cellBasedEnergyRing5 ,	      static_cast<float>( commonDetails->cellBasedEnergyRing5() ) );			  
	    xtaujet->setDetail(xAOD::TauJetParameters::cellBasedEnergyRing6 ,	      static_cast<float>( commonDetails->cellBasedEnergyRing6() ) );			  
	    xtaujet->setDetail(xAOD::TauJetParameters::cellBasedEnergyRing7 ,	      static_cast<float>( commonDetails->cellBasedEnergyRing7() ) );			  
	    xtaujet->setDetail(xAOD::TauJetParameters::TRT_NHT_OVER_NLT ,	      static_cast<float>( commonDetails->TRT_NHT_OVER_NLT() ) );			  
	    // xtaujet->setTauJetVtxFraction(           commonDetails->tauJetVtxFraction() );                         
	  } else {
	  ATH_MSG_WARNING( "there was no TauDetails container found" );
	}

	//copy pantau details
	ATH_MSG_DEBUG( "go looking for pantau details container " );

	copyPanTauDetails((*tau), (*xtaujet));

      }
  

    return StatusCode::SUCCESS;
  }

  void TauJetCnvTool::setLinks(const Analysis::TauJet& aodtau, xAOD::TauJet& xaodtau) const {

    // Need to reset links from old TrackParticle to xAOD::TrackParticles
  
    for (unsigned int i = 0; i != aodtau.numTrack(); ++i) 
      {
	ATH_MSG_DEBUG( "track number : " << i << " has pt: " << (aodtau.track(i) ? aodtau.track(i)->pt() : -1111.) );    
      }
  
    //set track links
    //ElementLinkVector<Rec::TrackParticleContainer>::const_iterator trackit  = aodtau.trackLinkVector().begin();
    //ElementLinkVector<Rec::TrackParticleContainer>::const_iterator trackitE = aodtau.trackLinkVector().end();


    // for( ; trackit!= trackitE; ++trackit)
    //   {
    // 	xaodtau.addTrackLink(getNewTrackLink((*trackit), m_inDetTrackParticles) );
    //   }


    // for (unsigned int i = 0; i != xaodtau.nTracks(); ++i) 
    //   {
    // 	ATH_MSG_DEBUG( "xtau track number : " << i << " has pt: " << xaodtau.track(i)->pt() );    
    //   }


    // for (unsigned int i = 0; i != aodtau.numConversionTrack(); ++i) 
    //   {
    // 	ATH_MSG_DEBUG( "conversion track number : " << i << " has pt: " << aodtau.conversionTrack(i)->pt() );    
    //   }

    // //set conversion track links
    // ElementLinkVector<Rec::TrackParticleContainer>::const_iterator convTrackit  = aodtau.conversionTrackLinkVector().begin();
    // ElementLinkVector<Rec::TrackParticleContainer>::const_iterator convTrackitE = aodtau.conversionTrackLinkVector().end();

    // for( ; convTrackit!= convTrackitE; ++convTrackit)
    //   {
    // 	xaodtau.setTrackFlag( *getNewTrackLink((*convTrackit), m_inDetTrackParticles), xAOD::TauJetParameters::isConversion, true);
    // 	// xaodtau.addConversionTrackLink(getNewTrackLink((*convTrackit), m_inDetTrackParticles) );
    //   }

    //Blake, fix me!!!
    // for (unsigned int i = 0; i != xaodtau.nConversionTracks(); ++i) {
    //   // ATH_MSG_DEBUG( "xtau conversion track number : " << i << " has pt: " << xaodtau.conversionTrack(i)->pt() );    
    // }



    //get common details member, because wide and other tracks are stored there
    const Analysis::TauCommonDetails* commonDetails(aodtau.details<Analysis::TauCommonDetails>());


    for (unsigned int i = 0; i != commonDetails->seedCalo_nWideTrk(); ++i) 
      {
	ATH_MSG_DEBUG( "wide track number : " << i << " has pt: " << commonDetails->seedCalo_wideTrk(i)->pt() );    
      }

    // //set wide track links
    // ElementLinkVector<Rec::TrackParticleContainer>::const_iterator wideTrackit  = commonDetails->seedCalo_wideTrk().begin();
    // ElementLinkVector<Rec::TrackParticleContainer>::const_iterator wideTrackitE = commonDetails->seedCalo_wideTrk().end();

    // for( ; wideTrackit!= wideTrackitE; ++wideTrackit)
    //   {
    // 	xaodtau.addWideTrackLink(getNewTrackLink((*wideTrackit), m_inDetTrackParticles) );
    //   }

    // for (unsigned int i = 0; i != xaodtau.nWideTracks(); ++i) 
    //   {
    // 	ATH_MSG_DEBUG( "xtau wide track number : " << i << " has pt: " << xaodtau.wideTrack(i)->pt() );    
    //   }


    // for (unsigned int i = 0; i != commonDetails->nOtherTrk(); ++i) 
    //   {
    // 	ATH_MSG_DEBUG( "other track number : " << i << " has pt: " << commonDetails->otherTrk(i)->pt() );    
    //   }

    // //set other track links
    // ElementLinkVector<Rec::TrackParticleContainer>::const_iterator otherTrackit  = commonDetails->otherTrk().begin();
    // ElementLinkVector<Rec::TrackParticleContainer>::const_iterator otherTrackitE = commonDetails->otherTrk().end();

    // for( ; otherTrackit!= otherTrackitE; ++otherTrackit)
    //   {
    // 	xaodtau.addOtherTrackLink(getNewTrackLink((*otherTrackit), m_inDetTrackParticles) );
    //   }


    // for (unsigned int i = 0; i != xaodtau.nOtherTracks(); ++i) {
    //   ATH_MSG_DEBUG( "xtau other track number : " << i << " has pt: " << xaodtau.otherTrack(i)->pt() );    
    // }
  

    ATH_MSG_DEBUG( "trying to set jet link " );
  
    ATH_MSG_DEBUG( "tau jet seed has pt : " <<  aodtau.jet()->pt() );    

    xaodtau.setJetLink(getNewJetLink(aodtau.jetLink(), m_jets) );

    ATH_MSG_DEBUG( "tau jet seed has pt : " <<  (*xaodtau.jetLink())->pt() );    


  }

  ElementLink<xAOD::TrackParticleContainer> TauJetCnvTool::getNewTrackLink(const ElementLink<Rec::TrackParticleContainer>& oldLink, const std::string& name) const{
    ElementLink<xAOD::TrackParticleContainer> newLink;
    newLink.resetWithKeyAndIndex( name, oldLink.index() );
    return newLink;
  }

  ElementLink<xAOD::JetContainer> TauJetCnvTool::getNewJetLink(const ElementLink<JetCollection>& oldLink, const std::string& name) const{
    ElementLink<xAOD::JetContainer> newLink;
    newLink.resetWithKeyAndIndex( name, oldLink.index() );
    return newLink;
  }

  void TauJetCnvTool::copyPanTauDetails(const Analysis::TauJet& /*aodtau*/, xAOD::TauJet& /*xaodtau*/) const {
  
    // const PanTau::PanTauDetails* cellBasedDetails_PanTau(0);
    // cellBasedDetails_PanTau  = aodtau.details<const PanTau::PanTauDetails>("PanTau_SeedDetailsCellBased");
    // //with details_SGKey being
    // //   PanTau_SeedDetailsCellBased
    // //or PanTau_SeedDetailseflowRec 
  
    // int tempint = 0;
    // float tempfloat = 0;
  
    // if (!cellBasedDetails_PanTau)
    //   {
    // 	ATH_MSG_DEBUG( "TauJet has no associated cellBased PanTauDetails --- This must not be bad - if PanTau rejected a tauRec seed, that tauRec seed will not have PanTauDetails");
    //   }
    // else
    //   {
    // 	ATH_MSG_DEBUG( "trying to convert tau with cellBased isPanTauCandidate =" << cellBasedDetails_PanTau->isPanTauCandidate() );
    // 	xaodtau.setPanTauDetail( xAOD::TauJetParameters::PanTau_isPanTauCandidate, cellBasedDetails_PanTau->isPanTauCandidate() ) ;
    // 	if( xaodtau.panTauDetail(xAOD::TauJetParameters::PanTau_isPanTauCandidate, tempint ) )
    // 	  {
    // 	    ATH_MSG_DEBUG( "converted xaod tau with cellBased isPanTauCandidate =" <<  tempint);
    // 	  }
    // 	else
    // 	  {
    // 	    ATH_MSG_DEBUG( "something went wrong in setting xAOD tau's cellBased isPanTauCandidate ");
    // 	  }
      
    // 	xaodtau.setPanTauDetail( xAOD::TauJetParameters::PanTau_DecayModeProto,    cellBasedDetails_PanTau->RecoModeSubAlg() ) ;	
    // 	xaodtau.setPanTauDetail( xAOD::TauJetParameters::PanTau_DecayMode, 	cellBasedDetails_PanTau->RecoModePanTau() ) ;		 
      
    // 	ATH_MSG_DEBUG( "trying to convert tau with cellBased BDTValue_1p0n_vs_1p1n =" << cellBasedDetails_PanTau->BDTValue_1p0n_vs_1p1n() );
    // 	xaodtau.setPanTauDetail( xAOD::TauJetParameters::PanTau_BDTValue_1p0n_vs_1p1n, static_cast<float>(cellBasedDetails_PanTau->BDTValue_1p0n_vs_1p1n() ) ) ;		  
      
    // 	if( xaodtau.panTauDetail(xAOD::TauJetParameters::PanTau_BDTValue_1p0n_vs_1p1n, tempfloat ) )
    // 	  {
    // 	    ATH_MSG_DEBUG( "converted xaod tau with cellBased BDTValue_1p0n_vs_1p1n =" <<  tempfloat);
    // 	  }
    // 	else
    // 	  {
    // 	    ATH_MSG_DEBUG( "something went wrong in setting xAOD tau's cellBased BDTValue_1p0n_vs_1p1n ");
    // 	  }
      
      
    // 	xaodtau.setPanTauDetail( xAOD::TauJetParameters::PanTau_BDTValue_1p1n_vs_1pXn, 			 static_cast<float>(cellBasedDetails_PanTau->BDTValue_1p1n_vs_1pXn() ) ) ;	  
    // 	xaodtau.setPanTauDetail( xAOD::TauJetParameters::PanTau_BDTValue_3p0n_vs_3pXn, 			 static_cast<float>(cellBasedDetails_PanTau->BDTValue_3p0n_vs_3pXn() ) ) ;		  
    // 	xaodtau.setPanTauDetail( xAOD::TauJetParameters::PanTau_BDTVar_Basic_NNeutralConsts, 		 cellBasedDetails_PanTau->CellBased_Basic_NNeutralConsts() ) ;		  
    // 	xaodtau.setPanTauDetail( xAOD::TauJetParameters::PanTau_BDTVar_Charged_JetMoment_EtDRxTotalEt, 	 static_cast<float>(cellBasedDetails_PanTau->CellBased_Charged_JetMoment_EtDRxTotalEt() ) ) ; 
    // 	xaodtau.setPanTauDetail( xAOD::TauJetParameters::PanTau_BDTVar_Charged_StdDev_Et_WrtEtAllConsts, 	 static_cast<float>(cellBasedDetails_PanTau->CellBased_Charged_StdDev_Et_WrtEtAllConsts() ) ) ; 
    // 	xaodtau.setPanTauDetail( xAOD::TauJetParameters::PanTau_BDTVar_Neutral_HLV_SumM, 			 static_cast<float>(cellBasedDetails_PanTau->CellBased_Neutral_HLV_SumM() ) ) ;	  
    // 	xaodtau.setPanTauDetail( xAOD::TauJetParameters::PanTau_BDTVar_Neutral_PID_BDTValues_BDTSort_1, 	 static_cast<float>(cellBasedDetails_PanTau->CellBased_Neutral_PID_BDTValues_BDTSort_1() ) ) ;  
    // 	xaodtau.setPanTauDetail( xAOD::TauJetParameters::PanTau_BDTVar_Neutral_PID_BDTValues_BDTSort_2, 	 static_cast<float>(cellBasedDetails_PanTau->CellBased_Neutral_PID_BDTValues_BDTSort_2() ) ) ;  
    // 	xaodtau.setPanTauDetail( xAOD::TauJetParameters::PanTau_BDTVar_Neutral_Ratio_1stBDTEtOverEtAllConsts, static_cast<float>(cellBasedDetails_PanTau->CellBased_Neutral_Ratio_1stBDTEtOverEtAllConsts() ) ) ;  
    // 	xaodtau.setPanTauDetail( xAOD::TauJetParameters::PanTau_BDTVar_Neutral_Ratio_EtOverEtAllConsts, 	 static_cast<float>(cellBasedDetails_PanTau->CellBased_Neutral_Ratio_EtOverEtAllConsts() ) ) ;  
    // 	xaodtau.setPanTauDetail( xAOD::TauJetParameters::PanTau_BDTVar_Neutral_Shots_NPhotonsInSeed, 	 static_cast<float>(cellBasedDetails_PanTau->CellBased_Neutral_Shots_NPhotonsInSeed() ) ) ;	  
    // 	xaodtau.setPanTauDetail( xAOD::TauJetParameters::PanTau_BDTVar_Combined_DeltaR1stNeutralTo1stCharged, static_cast<float>(cellBasedDetails_PanTau->CellBased_Combined_DeltaR1stNeutralTo1stCharged() ) ) ;  
      
    //   }
  
  
    
  
  }
  


}


//  LocalWords:  tempfloat
