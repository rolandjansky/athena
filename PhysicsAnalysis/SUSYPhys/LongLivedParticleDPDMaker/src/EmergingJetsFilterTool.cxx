/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// EmergingJetsFilterTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "LongLivedParticleDPDMaker/EmergingJetsFilterTool.h"
#include <vector>
#include <string>
#include "xAODJet/JetContainer.h"
#include "xAODTracking/TrackParticleContainer.h"

#include "TrigDecisionTool/ChainGroup.h"
#include "TrigDecisionTool/FeatureContainer.h"
#include "TrigDecisionTool/Feature.h"

// Constructor
DerivationFramework::EmergingJetsFilterTool::EmergingJetsFilterTool( const std::string& t,
								     const std::string& n,
								     const IInterface* p ) : 
  AthAlgTool(t,n,p),
  m_tdt("Trig::TrigDecisionTool/TrigDecisionTool"),
  m_ntot(0),
  m_npass(0),
  m_jetSGKey("AntiKt4EMTopoJets"),
  m_BjetSGKey("BTagging_AntiKt4EMTopo"),
  m_trackSGKey("InDetTrackParticles"),
  m_ptCut(100000.0),
  m_etaCut(2.5),
  m_sumPtTrkCut(5000.0),
  m_sumJetsPtCut(800000.0)
  {
    declareInterface<DerivationFramework::ISkimmingTool>(this);
    declareProperty("TrigDecisionTool", m_tdt, "Tool to access the trigger decision");
    declareProperty("Triggers", m_triggers = std::vector< std::string >());
    declareProperty("JetContainerKey", m_jetSGKey);
    declareProperty("BJetContainerKey", m_BjetSGKey);
    declareProperty("TrackContainerKey", m_trackSGKey);
    declareProperty("JetPtCut", m_ptCut);
    declareProperty("JetEtaCut", m_etaCut);
    declareProperty("sumPtTrkCut", m_sumPtTrkCut);
    declareProperty("sumJetsPtCut", m_sumJetsPtCut);
    declareProperty("nJetsRequired", m_nJetsRequired = 4);
  }
  
// Destructor
DerivationFramework::EmergingJetsFilterTool::~EmergingJetsFilterTool() {
}  

// Athena initialize and finalize
StatusCode DerivationFramework::EmergingJetsFilterTool::initialize()
{
     ATH_MSG_VERBOSE("initialize() ...");

     if(m_tdt.retrieve().isFailure()) {
       ATH_MSG_FATAL("Failed to retrieve tool: " << m_tdt);
       return StatusCode::FAILURE;
     }
     ATH_MSG_INFO("Retrieved tool: " << m_tdt);
     
     return StatusCode::SUCCESS;
     
}
StatusCode DerivationFramework::EmergingJetsFilterTool::finalize()
{
     ATH_MSG_VERBOSE("finalize() ...");
     ATH_MSG_INFO("Processed "<< m_ntot <<" events, "<< m_npass<<" events passed filter ");
     return StatusCode::SUCCESS;
}

// The filter itself
bool DerivationFramework::EmergingJetsFilterTool::eventPassesFilter() const
{

  ++m_ntot;
  bool thisEventPasses = false;

  bool passedTrigger = false;
  /*
  int decision = m_tdt->isPassed("HLT_4j100");
  int decision2 = m_tdt->isPassed("HLT_4j110");

  if( decision == 1 || decision2 == 1 ){
    passedTrigger = true;
  }
  */
  if( m_triggers.size() > 0 ){

    passedTrigger = false;

    for( unsigned int i = 0; i < m_triggers.size(); i++ ){
      int decision = m_tdt->isPassed( m_triggers.at(i) );
      if( decision == 1 ){
	passedTrigger = true;
	break;
      }
    }

  }

  if( !passedTrigger ) return thisEventPasses;

  int nJetsPassed = 0;

  //Int_t nJets25SumTkPt5 = 0;
  //Int_t nJets100SumTkPt5 = 0;

  std::vector< double > sumJetsPt;
  
  const xAOD::JetContainer* jets(0);
  StatusCode sc = evtStore()->retrieve(jets,m_jetSGKey);
  if( sc.isFailure() || !jets ){
    msg(MSG::WARNING) << "No Jet container found, will skip this event" << endmsg;
    return StatusCode::FAILURE;
  } 
  msg(MSG::DEBUG) << "size of Jet container is " << jets->size() << endmsg;

  
  for( unsigned int i = 0; i < jets->size(); ++i ){ 

    const xAOD::Jet* jet = jets->at(i);
    if( (jet->pt() < m_ptCut) || (fabs(jet->eta())>m_etaCut) ) continue;
    sumJetsPt.push_back(jet->pt());
    nJetsPassed += 1;
    /*
    std::vector<float> sumPtTrkPt500vec;
    jet->getAttribute(xAOD::JetAttribute::SumPtTrkPt500, sumPtTrkPt500vec);
    if( sumPtTrkPt500vec.size() > 0 ){
      msg(MSG::DEBUG) << "sumptTrk is " << sumPtTrkPt500vec.at(0) << endmsg;
      if( sumPtTrkPt500vec.at(0) < m_sumPtTrkCut ){
	if( jet->pt() > 25e3 ) ++nJets25SumTkPt5;
	if( jet->pt() > 100e3 ) ++nJets100SumTkPt5;
      }
    }
    */
  }
  /*
  // Look at track d0-significance and track pT for each jet

  Int_t nJets100 = 0;
  Int_t nJets100_sumSigD0WpT3000 = 0;

  std::vector< std::pair<float,int> > myJets_100;

  // TLorentzVectors for the highest-pT jets with pT > 100 GeV                        
  TLorentzVector* jet100_1 = new TLorentzVector(0,0,0,0);
  TLorentzVector* jet100_2 = new TLorentzVector(0,0,0,0);
  TLorentzVector* jet100_3 = new TLorentzVector(0,0,0,0);
  TLorentzVector* jet100_4 = new TLorentzVector(0,0,0,0);
  TLorentzVector* jet100_5 = new TLorentzVector(0,0,0,0);
  TLorentzVector* jet100_6 = new TLorentzVector(0,0,0,0);
  TLorentzVector* jet100_7 = new TLorentzVector(0,0,0,0);
  TLorentzVector* jet100_8 = new TLorentzVector(0,0,0,0);
  TLorentzVector* jet100_9 = new TLorentzVector(0,0,0,0);
  TLorentzVector* jet100_10 = new TLorentzVector(0,0,0,0);

  std::vector< TLorentzVector* > jets_100;
  jets_100.push_back(jet100_1);
  jets_100.push_back(jet100_2);
  jets_100.push_back(jet100_3);
  jets_100.push_back(jet100_4);
  jets_100.push_back(jet100_5);
  jets_100.push_back(jet100_6);
  jets_100.push_back(jet100_7);
  jets_100.push_back(jet100_8);
  jets_100.push_back(jet100_9);
  jets_100.push_back(jet100_10);

  // Get track particles, to check tracks near jets                                   

  const xAOD::TrackParticleContainer* recoTracks = 0;
  if( !evtStore()->retrieve( recoTracks, m_trackSGKey ).isSuccess() ){
    Error("execute()", "Failed to retrieve Reconstructed Track container. Exiting." );
  }


  xAOD::JetContainer::const_iterator jet_itr = jets->begin();
  xAOD::JetContainer::const_iterator jet_end = jets->end();

  Int_t jetInd = 0;

  for( ; jet_itr != jet_end; ++jet_itr ) {

    Double_t jetPTCut = 100.0;
    Double_t jetEtaCut = 2.5;

    std::pair<float,int> ptAndInd( ( (*jet_itr)->pt() )/1e3, jetInd );

    if( ( (*jet_itr)->pt() )/1e3 > jetPTCut && fabs( (*jet_itr)->eta() ) < jetEtaCut ){

      myJets_100.push_back( ptAndInd );

      jets_100[nJets100]->SetPtEtaPhiM( (*jet_itr)->pt(),
					(*jet_itr)->eta(),
					(*jet_itr)->phi(),
					(*jet_itr)->m()
					);

      // Check tracks near this jet                                                    
      TLorentzVector* thisTrack = new TLorentzVector(0,0,0,0);

      std::vector< double > dRs;
      dRs.push_back(0.5);

      for( UInt_t dRInd = 0; dRInd < dRs.size(); ++dRInd ){


	Int_t nTrksAbovePt500 = 0;
	Double_t sumSigD0WpT_500 = 0.0;
	Int_t trkCtr = 0;

	for( const auto& trk : *recoTracks ){
	  //std::cout << "Track pT = " << trk->pt() << std::endl;                              
	  thisTrack->SetPtEtaPhiM( trk->pt(),
				   trk->eta(),
				   trk->phi(),
				   trk->m()
				   );

	  Double_t d0sig = fabs( trk->d0() ) / sqrt( trk->definingParametersCovMatrix()(0,0) );

	  if( thisTrack->Pt() > 500.0 && thisTrack->DeltaR(*jets_100[nJets100]) < dRs[dRInd] ){
	    ++nTrksAbovePt500;
	    sumSigD0WpT_500 += d0sig*(trk->pt());
	  }

	  ++trkCtr;

	}

	if( sumSigD0WpT_500/1e3 > 3000.0 ) ++nJets100_sumSigD0WpT3000;

      }

      ++nJets100;

    }

    ++jetInd;

  }


  // Look at b-jets

  const xAOD::BTaggingContainer* Bjets = 0;
  evtStore()->retrieve( Bjets, m_BjetSGKey );

  xAOD::BTaggingContainer::const_iterator bjet_itr = Bjets->begin();
  xAOD::BTaggingContainer::const_iterator bjet_end = Bjets->end();

  Int_t bJetIndex = -1;
  Int_t nBjetsPt100 = 0;

  for( ; bjet_itr != bjet_end; ++bjet_itr ) {

    ++bJetIndex;

    Double_t mv2c10Val = (*bjet_itr)->auxdataConst< double >("MV2c10_discriminant");
    if( mv2c10Val > 0.6459 ){
      if( bJetIndex < int(jets->size()) ){
	if( jets->at(bJetIndex)->pt()/1e3 > 100.0 ){
	  ++nBjetsPt100;
	}
      }
    }
  }
  */
  // Finally put it all together to see if the event passes

  if( nJetsPassed >= 4 ){
    ++m_npass;
    thisEventPasses = true;
  }
  /*
  if( nJetsPassed >= m_nJetsRequired ){

    double sumJetsPtLocal = 0.0;

    for( UInt_t jj = 0; jj < 4; ++jj ) sumJetsPtLocal += sumJetsPt[jj];

    if( sumJetsPtLocal > m_sumJetsPtCut ){

      if( nJets25SumTkPt5 > 1 || nJets100SumTkPt5 > 0 ){
	++m_npass;
	thisEventPasses = true;
      }else if( nJets100_sumSigD0WpT3000 > 1 || nBjetsPt100 > 1 ){
	++m_npass;
	thisEventPasses = true;
      }

    }

  }
  */
  return thisEventPasses;
  
}  
  
