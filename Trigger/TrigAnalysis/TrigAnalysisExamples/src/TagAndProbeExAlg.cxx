/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TagAndProbeExAlg.cxx 723125 2016-02-10 15:37:04Z ssnyder $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"
#include "AthenaKernel/Units.h"

// Offline EDM object(s):
#include "muonEvent/MuonContainer.h"

// Trigger EDM object(s):
#include "TrigMuonEvent/MuonFeature.h"
#include "TrigMuonEvent/MuonFeatureContainer.h"
#include "TrigMuonEvent/TrigMuonEFContainer.h"

// Local include(s):
#include "TagAndProbeExAlg.h"

using Athena::Units::GeV;

TagAndProbeExAlg::TagAndProbeExAlg( const std::string& name, ISvcLocator *pSvcLocator )
   : AthAlgorithm( name, pSvcLocator ),
     m_matchTool( "TrigMatchTool/TrigMatchTool" ),
     m_numSelectedEvents( 0 ),
     m_numTaggedEvents( 0 ),
     m_numL1PassedEvents( 0 ),
     m_numL2PassedEvents( 0 ),
     m_numEFPassedEvents( 0 ) {

   // job option configurable properties
   declareProperty( "TrigMatchTool", m_matchTool );
   declareProperty( "MuonContainerKey", m_muonContainerKey = "StacoMuonCollection" );
   declareProperty( "L1ChainName", m_l1chainName = "L1_MU10" );
   declareProperty( "L2ChainName", m_l2chainName = "L2_mu10" );
   declareProperty( "EFChainName", m_efchainName = "EF_mu10" );
   declareProperty( "PtCut", m_ptCut = 20. );
   declareProperty( "Chi2Cut", m_chi2Cut = 100. );
   declareProperty( "MassMin", m_massMin = 70. );
   declareProperty( "MassMax", m_massMax = 110. );
   declareProperty( "EtaMax", m_etaMax = 2.4 );
}

StatusCode TagAndProbeExAlg::initialize() {

   // load the matching tool
   if( ! m_matchTool.empty() ) {
      CHECK( m_matchTool.retrieve() );
      ATH_MSG_INFO( "Successfully retrived the TrigMatchTool!" );
   } else {
      REPORT_ERROR( StatusCode::FAILURE ) << "Could not retrive the TrigMatchTool as it "
                                          << "was not specified!";
      return StatusCode::FAILURE;
   }

   return StatusCode::SUCCESS;
}

StatusCode TagAndProbeExAlg::execute() {

   REPORT_MESSAGE( MSG::DEBUG ) << "In TagAndProbeExAlg::Execute()";

   // load the muon container from store gate
   const Analysis::MuonContainer *muonContainer = 0;
   CHECK( evtStore()->retrieve( muonContainer, m_muonContainerKey ) );

   // collect the first two muons which pass our muon selection
   std::vector< const Analysis::Muon* > selectedMuons;
   for( Analysis::MuonContainer::const_iterator iter = muonContainer->begin();
        iter != muonContainer->end(); ++iter ) {
      if( this->passMuon( *iter ) ) {
         selectedMuons.push_back( *iter );
      }
      if( selectedMuons.size() >= 2 ) break;
   }

   // check that the event meets the requirements for selection
   if( ! passEvent( selectedMuons ) ) return StatusCode::SUCCESS;

   ++m_numSelectedEvents;

   const Analysis::Muon *m1 = selectedMuons[ 0 ];
   const Analysis::Muon *m2 = selectedMuons[ 1 ];

   // Note that we need to be careful in the case that
   // both muons pass the tag requirements.  In that case
   // we need to consider tag = m1, probe = m2 and
   // tag = m2, probe = m1, or we will bias our measurement

   // check if muon 1 can be the tag
   if( this->tagMuon( m1 ) ) {
      ++m_numTaggedEvents;
      this->recordProbe( m2 );
   }

   // check if muon 2 can be the tag
   if( this->tagMuon( m2 ) ) {
      ++m_numTaggedEvents;
      this->recordProbe( m1 );
   }

   return StatusCode::SUCCESS;
}

StatusCode TagAndProbeExAlg::finalize() {

   // Here we can print out all the information we've learned
   // We need to be a little clever in how we interpret the information
   // presented here.  In particular, remember that each muon has a chance
   // to act as the tag muon, so for chains with very high efficiency
   // its not unreasonable to get a number of tag muons greater than the
   // number of selected events.
   REPORT_MESSAGE( MSG::DEBUG ) << "TagAndProbeExAlg::finalize()";

   ATH_MSG_INFO( "" );
   ATH_MSG_INFO( "TagAndProbeExAlg results" );
   ATH_MSG_INFO( "L1 Chain name: " << m_l1chainName );
   ATH_MSG_INFO( "L2 Chain name: " << m_l2chainName );
   ATH_MSG_INFO( "EF Chain name: " << m_efchainName );
   ATH_MSG_INFO( "Selection cuts: " );
   ATH_MSG_INFO( "muon pT > " << m_ptCut << " GeV" );
   ATH_MSG_INFO( "muon combined chi2 < " << m_chi2Cut );
   ATH_MSG_INFO( "muon |eta| < " << m_etaMax );
   ATH_MSG_INFO( m_massMin << " GeV < mZ < " << m_massMax << " GeV" );
   ATH_MSG_INFO( "Number of selected events: " << m_numSelectedEvents );
   ATH_MSG_INFO( "Number of events with a tag muon: " << m_numTaggedEvents );
   ATH_MSG_INFO( "Number of events with a L1 passed probe muon: " << m_numL1PassedEvents );
   ATH_MSG_INFO( "Number of events with a L2 passed probe muon: " << m_numL2PassedEvents );
   ATH_MSG_INFO( "Number of events with a EF passed probe muon: " << m_numEFPassedEvents );
   ATH_MSG_INFO( m_l1chainName << " trigger efficiency (relative to offline): " 
                 << (float)m_numL1PassedEvents/(float)m_numTaggedEvents * 100. << "%" );
   ATH_MSG_INFO( m_l2chainName << " trigger efficiency (relative to Level 1): " 
                 << (float)m_numL2PassedEvents/(float)m_numL1PassedEvents * 100. << "%" );
   ATH_MSG_INFO( m_efchainName << " trigger efficiency (relative to Level 2): " 
                 << (float)m_numEFPassedEvents/(float)m_numL2PassedEvents * 100. << "%" );
   ATH_MSG_INFO( "" );

   return StatusCode::SUCCESS;
}

// The jist of the algorithm is asking the three following questions:
// (1) Does a given reco'd muon pass our selection cuts?
//     This is answered in passMuon
// (2) Given our selected muons, does the event pass our cuts?
//     This is answered in passEvent
// (3) Does a given reco'd muon pass our trigger?
//     This is answered in passL1, passL2, and passEF


//
// Trigger - Reco matching
//
// The basic question here is, does a given reco'd muon pass a specified
// trigger?
// We answer this by asking, does the muon match to a trigger object
// in the chain of interest, and if so, did that trigger object cause
// the chain to be marked as passed?
// 
// There are a few subtleties in this process:
// (a) What does it mean for a trigger and reco object to match?  If we
//     don't specify anything, this is done by deltaR matching.  However,
//     we could be more creative if we like.  See DistanceFunctor.h for
//     examples of how to write a custom distance measure
// (b) What objects do we match to?  The best way to figure this out is
//     to look at the typedefs in TrigMatchDefs.h in Trigger/TrigAnalysis/
//     TrigObjectMatching package

// Does the reco'd muon pass L1 of the chain we're interested in?
bool TagAndProbeExAlg::passL1( const Analysis::Muon *m ) {
  
   return m_matchTool->chainPassedByObject< TrigMatch::TrigMuonL1 >( m, m_l1chainName );
}

// Does the reco'd muon pass L2 of the chain we're interested in?
bool TagAndProbeExAlg::passL2( const Analysis::Muon *m ) {
 
   return m_matchTool->chainPassedByObject< TrigMatch::TrigMuonL2 >( m, m_l2chainName );
}

// Does the reco'd muon pass EF of the chain we're interested in?
bool TagAndProbeExAlg::passEF( const Analysis::Muon *m ) {
 
   return m_matchTool->chainPassedByObject< TrigMatch::TrigMuonEF >( m, m_efchainName );
}

// Does the muon pass the reconstruction selection cuts?
// This is just a simple example of selction based on pT,
// match X^2 and eta
bool TagAndProbeExAlg::passMuon( const Analysis::Muon *m ) {

   if( m->pt() / GeV > m_ptCut && m->isCombinedMuon() && m->matchChi2() < m_chi2Cut &&
       fabs( m->eta() ) < m_etaMax )
      return true;

   return false;
}

// Does the event pass the reconstruction selection cuts?
// This is just a simple example of reconstructing the Z
// boson invariant mass.
bool TagAndProbeExAlg::passEvent( std::vector< const Analysis::Muon* > &muons ) {

   if( muons.size() != 2 )
      return false;

   float mass = ( muons[ 0 ]->hlv() + muons[ 1 ]->hlv() ).m() / GeV;

   if( mass > m_massMin && mass < m_massMax )
      return true;

   return false;
}

// Is the muon a tag muon?
// A tag is just defined as passing all levels of the trigger
bool TagAndProbeExAlg::tagMuon( const Analysis::Muon *m ) {

   if( m && this->passL1( m ) && this->passL2( m ) && this->passEF( m ) )
      return true;

   return false;
}

// Record the information for the probe muon.
// In this case, we just record the number of times
// it passes L1, L2, and EF.
void TagAndProbeExAlg::recordProbe( const Analysis::Muon *m ) {

   if( !m )
      return;

   if( this->passL1( m ) )
      ++m_numL1PassedEvents;
   if( this->passL2( m ) )
      ++m_numL2PassedEvents;
   if( this->passEF( m ) )
      ++m_numEFPassedEvents;

   return;
}
