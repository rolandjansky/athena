/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/Combinators.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "TrigEgammaFastElectronHypoTool.h"
#include <cmath>

namespace TCU = TrigCompositeUtils;

TrigEgammaFastElectronHypoTool::TrigEgammaFastElectronHypoTool( const std::string& type, 
  const std::string& name, 
  const IInterface* parent ) 
  : base_class( type, name, parent ),
    m_decisionId( HLT::Identifier::fromToolName( name ) ) 
{}

//==================================================================


StatusCode TrigEgammaFastElectronHypoTool::initialize()  {
  
  if ( !m_monTool.empty() ) ATH_CHECK( m_monTool.retrieve() );


  ATH_MSG_DEBUG( "AcceptAll            = " << ( m_acceptAll? "True" : "False" ) ); 
  ATH_MSG_DEBUG( "Do_Ringer            = " << ( m_doRinger ? "True" : "False" ) ); 
  ATH_MSG_DEBUG( "TrackPt              = " << m_trackPt              ); 
  ATH_MSG_DEBUG( "CaloTrackdETA        = " << m_caloTrackDEta        );
  ATH_MSG_DEBUG( "CaloTrackdPHI        = " << m_caloTrackDPhi        );
  ATH_MSG_DEBUG( "CaloTrackdEoverPLow  = " << m_caloTrackdEoverPLow  );
  ATH_MSG_DEBUG( "CaloTrackdEoverPHigh = " << m_caloTrackdEoverPHigh );
  ATH_MSG_DEBUG( "TRTRatio             = " << m_trtRatio             );
  ATH_MSG_DEBUG( "Do_LRT               = " << m_doLRT                );
  ATH_MSG_DEBUG( "d0Cut                = " << m_d0                   );

  ATH_MSG_DEBUG( "Tool configured for chain/id: " << m_decisionId  );
  
  ATH_MSG_DEBUG( "Initialization completed successfully:" );
  return StatusCode::SUCCESS;
}

//==================================================================

StatusCode TrigEgammaFastElectronHypoTool::decide( std::vector<ITrigEgammaFastElectronHypoTool::ElectronInfo>& input)  const {
  for ( auto& i: input ) {
    if ( TCU::passed ( m_decisionId.numeric(), i.previousDecisionIDs ) ) {
      if ( decide( i ) ) {
        TCU::addDecisionID( m_decisionId, i.decision );
      }
    }
  }
  return StatusCode::SUCCESS;
}

//==================================================================

bool TrigEgammaFastElectronHypoTool::decide( const ITrigEgammaFastElectronHypoTool::ElectronInfo& input) const
{
  return m_doRinger ? decide_ringer(input) : decide_cb(input);
}

//==================================================================

bool TrigEgammaFastElectronHypoTool::decide_cb ( const ITrigEgammaFastElectronHypoTool::ElectronInfo& input) const

{

  if ( m_acceptAll ) {
    ATH_MSG_DEBUG( "AcceptAll property is set: taking all events" );
    return true;
  } else {
     ATH_MSG_DEBUG( "AcceptAll property not set: applying selection" );
  }

  auto cutCounter = Monitored::Scalar<int>( "CutCounter", -1 );  
  auto mon_ptCalo     = Monitored::Scalar( "PtCalo", -999. );
  auto mon_ptTrack    = Monitored::Scalar( "PtTrack", -999. );
  auto mon_dEtaCalo   = Monitored::Scalar( "CaloTrackdEta", -1. );
  auto mon_dPhiCalo   = Monitored::Scalar( "CaloTrackdPhi", -1. );
  auto mon_eToverPt   = Monitored::Scalar( "CaloTrackEoverP", -1. );
  auto mon_caloEta    = Monitored::Scalar( "CaloEta", -100. );
  auto mon_caloPhi    = Monitored::Scalar( "CaloPhi", -100. );
  auto mon_trk_d0     = Monitored::Scalar( "d0Value", -1. );
  auto monitorIt  = Monitored::Group( m_monTool, 
                                      cutCounter,
                                      mon_ptCalo, mon_ptTrack,    
                                      mon_dEtaCalo, mon_dPhiCalo,   
                                      mon_eToverPt,   
                                      mon_caloEta, mon_caloPhi, mon_trk_d0);

  auto electron = input.electron;

  if(!electron){
    ATH_MSG_DEBUG("disconsider candidates without electron");
    return false;
  }

  float ptCalo(0), dEtaCalo(0), dPhiCalo(0), eToverPt(0), caloEta(0), caloPhi(0), trk_d0(0); 

  const xAOD::TrackParticle* trkIter = electron->trackParticle();

  if ( !trkIter ){  // disconsider candidates without track
    ATH_MSG_DEBUG("disconsider candidates without track");
     return false;
  }
  cutCounter++;

  // Retrieve all quantities
  dPhiCalo    = electron->trkClusDphi();
  dEtaCalo    = electron->trkClusDeta();
  ptCalo      = electron->pt();	  
  eToverPt    = electron->etOverPt();	  
  caloEta     = electron->caloEta();
  caloPhi     = electron->caloPhi();
  trk_d0      = std::abs(trkIter->d0());

  float NTRHits     = static_cast<float>( electron->nTRTHits() );
  float NStrawHits  = static_cast<float>( electron->nTRTHiThresholdHits() );
  float TRTHitRatio = NStrawHits == 0 ? 1e10 : NTRHits/NStrawHits;

  if ( ptCalo < m_trackPt ){ 
    ATH_MSG_DEBUG( "Fails pt cut" << ptCalo << " < " << m_trackPt );
    return  false;
  }
  mon_ptCalo    = ptCalo;
  mon_ptTrack   = m_trackPt;
  cutCounter++;

  if ( dEtaCalo > m_caloTrackDEta ) {
    ATH_MSG_DEBUG( "Fails dEta cut " << dEtaCalo << " < " << m_caloTrackDEta );
    return  false;
  }
  mon_dEtaCalo  = dEtaCalo;
  mon_caloEta   = caloEta; 
   
  cutCounter++;
  if ( dPhiCalo > m_caloTrackDPhi ) {
    ATH_MSG_DEBUG( "Fails dPhi cut " << dPhiCalo << " < " << m_caloTrackDPhi );
    return  false;
  }
   
  mon_dPhiCalo  = dPhiCalo;
  mon_caloPhi   = caloPhi;
  cutCounter++;
  if( eToverPt <  m_caloTrackdEoverPLow ) {
    ATH_MSG_DEBUG( "Fails eoverp low cut " << eToverPt << " < " <<  m_caloTrackdEoverPLow );
    return false;
  }


  cutCounter++;
  if ( eToverPt > m_caloTrackdEoverPHigh ) {
    ATH_MSG_DEBUG( "Fails eoverp high cut " << eToverPt << " < " << m_caloTrackdEoverPHigh );
    return false;
  }
  mon_eToverPt  = eToverPt;

  cutCounter++;
  if ( TRTHitRatio < m_trtRatio ){
    ATH_MSG_DEBUG( "Fails TRT cut " << TRTHitRatio << " < " << m_trtRatio );
    return false;
  }


  cutCounter++;
  if(m_doLRT){
    ATH_MSG_DEBUG( "doLRT: " <<m_doLRT);
    ATH_MSG_DEBUG( "Track d0: " <<trk_d0);

    if(trk_d0 < m_d0){
      ATH_MSG_DEBUG( "Fails d0 cut " <<trk_d0<< " < " <<m_d0 );
      return false;
    }
  }
  mon_trk_d0    = trk_d0;

  ATH_MSG_DEBUG( "Passed selection" );
  return  true;

}


//==================================================================


bool TrigEgammaFastElectronHypoTool::decide_ringer ( const ITrigEgammaFastElectronHypoTool::ElectronInfo& input) const
{

  if ( m_acceptAll ) {
    ATH_MSG_DEBUG( "AcceptAll property is set: taking all events" );
    return true;
  } else {
     ATH_MSG_DEBUG( "AcceptAll property not set: applying selection" );
  }
  auto cutCounter     = Monitored::Scalar<int>( "CutCounter", -1 );  
  auto mon_ptCalo     = Monitored::Scalar( "PtCalo", -999. );
  auto mon_ptTrack    = Monitored::Scalar( "PtTrack", -999. );
  auto mon_dEtaCalo   = Monitored::Scalar( "CaloTrackdEta", -1. );
  auto mon_dPhiCalo   = Monitored::Scalar( "CaloTrackdPhi", -1. );
  auto mon_eToverPt   = Monitored::Scalar( "CaloTrackEoverP", -1. );
  auto mon_caloEta    = Monitored::Scalar( "CaloEta", -100. );
  auto mon_caloPhi    = Monitored::Scalar( "CaloPhi", -100. );
  auto mon_trk_d0     = Monitored::Scalar( "d0Value", -1. );
  auto mon_nnOutput   = Monitored::Scalar("NNOutput",-100);

  auto mon        = Monitored::Group( m_monTool, 
                                      cutCounter,
                                      mon_ptCalo, 
                                      mon_ptTrack,    
                                      mon_dEtaCalo, 
                                      mon_dPhiCalo,   
                                      mon_eToverPt,   
                                      mon_caloEta, 
                                      mon_caloPhi, 
                                      mon_trk_d0,
                                      mon_nnOutput);

  auto el = input.electron;

  if(!el){
    return false;
  }

  float ptCalo(0), dEtaCalo(0), dPhiCalo(0), eToverPt(0), caloEta(0), caloPhi(0), trk_d0(0), nnOutput(0); 

  cutCounter++;

  const xAOD::TrackParticle* trk = el->trackParticle();
  if ( !trk ){  
    ATH_MSG_DEBUG("disconsider candidates without track");
    return false;
  }
  cutCounter++;

  // Retrieve all quantities
  dPhiCalo    = el->trkClusDphi();
  dEtaCalo    = el->trkClusDeta();
  ptCalo      = el->pt();	  
  eToverPt    = el->etOverPt();	  
  caloEta     = el->caloEta();
  caloPhi     = el->caloPhi();
  trk_d0      = std::abs(trk->d0());

 
  if ( ptCalo < m_trackPt ){ 
    ATH_MSG_DEBUG( "Fails pt cut" << ptCalo << " < " << m_trackPt );
    return  false;
  }
  mon_ptCalo    = ptCalo;
  cutCounter++;

  auto rings = input.rings;
  if(!rings){
    ATH_MSG_DEBUG("disconsider candidates without rings");
    return false;
  }
  cutCounter++;

  auto clus = input.cluster;
  if(!clus){
    ATH_MSG_DEBUG("disconsider candidates without cluster");
    return false;
  }
  cutCounter++;


  bool pass = false;
  if( input.pidDecorator.count(m_pidName)){
    nnOutput = input.valueDecorator.at(m_pidName+"NNOutput");
    pass = input.pidDecorator.at(m_pidName);
    ATH_MSG_DEBUG( "Get the decision for " << m_pidName << ": " << (pass?"Yes":"No") );
  }

  
  mon_ptTrack   = m_trackPt;
  mon_dEtaCalo  = dEtaCalo; 
  mon_dPhiCalo  = dPhiCalo;
  mon_eToverPt  = eToverPt;
  mon_caloEta   = caloEta; 
  mon_caloPhi   = caloPhi; 
  mon_trk_d0    = trk_d0;
  mon_nnOutput  = nnOutput;


  return pass;
}


