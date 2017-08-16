/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "DecisionHandling/HLTIdentifier.h"

#include "TrigL2ElectronHypoTool.h"

using namespace TrigCompositeUtils;

TrigL2ElectronHypoTool::TrigL2ElectronHypoTool( const std::string& type, 
		    const std::string& name, 
		    const IInterface* parent ) :
  AthAlgTool( type, name, parent ),
  m_id( name )
{}

StatusCode TrigL2ElectronHypoTool::initialize()  {
  ATH_MSG_DEBUG( "Initialization:" );
  
  ATH_MSG_DEBUG( "Initialization completed successfully:" );
  ATH_MSG_DEBUG( "AcceptAll            = " 
		<< ( m_acceptAll==true ? "True" : "False" ) ); 
  ATH_MSG_DEBUG( "TrackPt              = " << m_trackPtthr           ); 
  ATH_MSG_DEBUG( "CaloTrackdETA        = " << m_calotrackdeta        );
  ATH_MSG_DEBUG( "CaloTrackdPHI        = " << m_calotrackdphi        );
  ATH_MSG_DEBUG( "CaloTrackdEoverPLow  = " << m_calotrackdeoverp_low );
  ATH_MSG_DEBUG( "CaloTrackdEoverPHigh = " << m_calotrackdeoverp_high );
  ATH_MSG_DEBUG( "TRTRatio = " << m_trtratio );
  
  return StatusCode::SUCCESS;
}

StatusCode TrigL2ElectronHypoTool::finalize()  {
  return StatusCode::SUCCESS;
}

TrigL2ElectronHypoTool::~TrigL2ElectronHypoTool()
{}


std::pair<bool,int> TrigL2ElectronHypoTool::decideOnSingleObject( const xAOD::TrigElectron* electron ) const {
  int cutCounter=0;
  const xAOD::TrackParticle* trkIter = electron-> trackParticle();
  if ( trkIter == 0 )  // disconsider candidates without track
    return std::make_pair( false, cutCounter );
  cutCounter++;

  // Retrieve all quantities
  float dPhiCalo    = electron->trkClusDphi();
  float dEtaCalo    = electron->trkClusDeta();
  float pTcalo      = electron->pt();	  
  float eTOverPt    = electron->etOverPt();	  
  //    int   trackIndx   = electron->trackIndx();
  float NTRHits     = ( float )( electron->nTRTHits() );
  float NStrawHits  = ( float )( electron->nTRTHiThresholdHits() );
  float TRTHitRatio = NStrawHits == 0 ? 1e10 : NTRHits/NStrawHits;



  if (pTcalo < m_trackPtthr){ 
    ATH_MSG_VERBOSE("Fails pt cut");
    return std::make_pair( false, cutCounter );
  }
  cutCounter++;

  if (dEtaCalo > m_calotrackdeta) {
    ATH_MSG_VERBOSE("Fails dEta cut");
    return std::make_pair( false, cutCounter );
  }
  cutCounter++;
  if (dPhiCalo > m_calotrackdphi) {
    ATH_MSG_VERBOSE("Fails dPhi cut");
    return std::make_pair( false, cutCounter );
  }

  cutCounter++;
  if(eTOverPt <  m_calotrackdeoverp_low ) {
    ATH_MSG_VERBOSE("Fails eoverp low cut");
    return std::make_pair( false, cutCounter );
  }
  cutCounter++;
  if ( eTOverPt > m_calotrackdeoverp_high ) {
    ATH_MSG_VERBOSE("Fails eoverp high cut");
    return std::make_pair( false, cutCounter );
  }
  cutCounter++;
  if (TRTHitRatio < m_trtratio){
    ATH_MSG_VERBOSE("Fails TRT cut");
    return std::make_pair( false, cutCounter );
  }
  cutCounter++;
  return std::make_pair( true, cutCounter );

}


StatusCode TrigL2ElectronHypoTool::decide(  std::vector<Input>& input )  const{

  // handle simples and most common case in easiest possible manner
  if ( m_multiplicity == 1 ) {
    for ( auto i: input ) {
      
    }
    return StatusCode::SUCCESS;
  }


  return StatusCode::SUCCESS;
}


