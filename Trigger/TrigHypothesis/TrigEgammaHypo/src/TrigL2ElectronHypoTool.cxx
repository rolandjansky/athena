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
  ATH_MSG_DEBUG( "TrackPt              = " << m_trackPt           ); 
  ATH_MSG_DEBUG( "CaloTrackdETA        = " << m_caloTrackDEta        );
  ATH_MSG_DEBUG( "CaloTrackdPHI        = " << m_caloTrackDPhi        );
  ATH_MSG_DEBUG( "CaloTrackdEoverPLow  = " << m_caloTrackdEoverPLow );
  ATH_MSG_DEBUG( "CaloTrackdEoverPHigh = " << m_caloTrackdEoverPHigh );
  ATH_MSG_DEBUG( "TRTRatio = " << m_trtRatio );

  std::vector<size_t> sizes( {m_trackPt.size(), m_caloTrackDEta.size(), m_caloTrackDPhi.size(), m_caloTrackdEoverPLow.size(), m_caloTrackdEoverPHigh.size(), m_trtRatio.size() });

  if ( min( sizes.begin(), sizes.end() ) != std::max( sizes.begin(), sizes.end() )  ) {
    ATH_MSG_ERROR( "Missconfiguration, cut properties listed above (when DEBUG) have different dimensions" );
  }

  m_multiplicity = m_trackPt.size();

  return StatusCode::SUCCESS;
}

StatusCode TrigL2ElectronHypoTool::finalize()  {
  return StatusCode::SUCCESS;
}

TrigL2ElectronHypoTool::~TrigL2ElectronHypoTool()
{}

bool TrigL2ElectronHypoTool::decideOnSingleObject( const xAOD::TrigElectron* electron, 
						   size_t cutIndex ) const {
  using namespace Monitored;
  auto cutCounter = MonitoredScalar::declare<int>( "CutCounter", -1 );  
  auto cutIndex   = MonitoredScalar::declare<int>( "CutIndex", cutIndex );  // one can do 2D plots for each cut independently
  auto ptCalo     = MonitoredScalar::declare( "PtCalo", -999. );
  auto ptTrack    = MonitoredScalar::declare( "PtTrack", -999. );
  auto dEtaCalo   = MonitoredScalar::declare( "CaloTrackEta", -1. );
  auto dPhiCalo   = MonitoredScalar::declare( "CaloTrackPhi", -1. );
  auto eToverPt   = MonitoredScalar::declare( "CaloTrackEoverP", -1. );
  auto caloEta    = MonitoredScalar::declare( "CaloEta", -100. );
  auto caloPhi    = MonitoredScalar::declare( "CaloEta", -100. );
  auto monitorIt  = MonitoredScope::declare( m_monTool, 
					     cutCounter, cutIndex,
					     ptCalo, ptTrack,    
					     dEtaCalo, dPhiCalo,   
					     eToverPt,   
					     caloEta, caloPhi );

  const xAOD::TrackParticle* trkIter = electron-> trackParticle();
  if ( trkIter == 0 )  // disconsider candidates without track
    return  false;
  cutCounter++;

  // Retrieve all quantities
  dPhiCalo    = electron->trkClusDphi();
  dEtaCalo    = electron->trkClusDeta();
  ptCalo      = electron->pt();	  
  eToverPt    = electron->etOverPt();	  

  caloEta     = electron->caloEta();
  caloPhi     = electron->caloPhi();

  float NTRHits     = ( float )( electron->nTRTHits() );
  float NStrawHits  = ( float )( electron->nTRTHiThresholdHits() );
  float TRTHitRatio = NStrawHits == 0 ? 1e10 : NTRHits/NStrawHits;


  if ( ptCalo < m_trackPt[cutIndex] ){ 
    ATH_MSG_VERBOSE( "Fails pt cut" );
    return  false;
  }
  cutCounter++;

  if ( dEtaCalo > m_caloTrackDEta[cutIndex] ) {
    ATH_MSG_VERBOSE( "Fails dEta cut" );
    return  false;
  }
  cutCounter++;
  if ( dPhiCalo > m_caloTrackDPhi[cutIndex] ) {
    ATH_MSG_VERBOSE( "Fails dPhi cut" );
    return  false;
  }

  cutCounter++;
  if( eToverPt <  m_caloTrackdEoverPLow[cutIndex] ) {
    ATH_MSG_VERBOSE( "Fails eoverp low cut" );
    return  false;
  }
  cutCounter++;
  if ( eToverPt > m_caloTrackdEoverPHigh[cutIndex] ) {
    ATH_MSG_VERBOSE( "Fails eoverp high cut" );
    return  false;
  }
  cutCounter++;
  if ( TRTHitRatio < m_trtRatio[cutIndex] ){
    ATH_MSG_VERBOSE( "Fails TRT cut" );
    return  false;
  }
  cutCounter++;
  return  true;

}

StatusCode TrigL2ElectronHypoTool::inclusiveSelection( std::vector<Input>& input ) const {
    for ( auto i: input ) {
      if ( i.previousDecisionIDs.count( decisionId() ) == 0 ) continue; // the decision was negative or not even made in previous stage
      auto objDecision = decideOnSingleObject( i.electron, 0);
      if ( objDecision == true ) {
	addDecisionID( decisionId(), i.decision );
      }
    }
    return StatusCode::SUCCESS;
}



size_t TrigL2ElectronHypoTool::countBits( const std::vector< std::vector<bool> >& passingSelection ) const {
  // the logic is as follows, 
  // if count of all the decision bits anded is smaller than the required multiplicity we do not have sufficient number of objects
  // else we do
  std::vector<bool> compressed( passingSelection[0].size(), false );
  std::vector< std::vector<bool>::const_iterator > bIters;


  for ( auto& b: passingSelection ) 
    bIters = b.begin();
  auto itersMove = [&](){ for ( auto& i: bIters ) ++i; }
  auto itersEnd  = [&](){ return bIters[0] == passingSelection[0].end(); } // assumptrion here, passingSelection has all member vectors of same size

  for ( ; not itersEnd(); itersMove() ) {
    for ( auto b: bIters ) {
      compressed[i] = compressed[i] or *b;
    }
  }
  retrun std::count_if( compressed.begin(), compressed.end(), true );
}



StatusCode TrigL2ElectronHypoTool::markPassing( std::vector<Input>& input, const std::vector< std::vector<bool> >& passingSelection ) const {

  size_t bitIndex = 0;
  for ( auto& i: input ) {
    for ( size_t sel = 0;  sel < m_multiplicity; ++sel ) {
      if ( passingSelection[sel][bitIndex] == true )  {
	addDecisionID( decisionId(), i.decision );
	break;
      }
    }
    bitIndex++;
  }
}


StatusCode TrigL2ElectronHypoTool::multiplicitySelection( std::vector<Input>& input ) const {
  std::vector< std::vector<bool> > passingSelection( m_multiplicity );
  bool decision = false;
  for ( size_t cutIndex = 0; cutIndex < m_multiplicity; ++ cutIndex ) {
    
    for ( auto elIter =  input.begin(); elIter != input.end(); ++elIter ) {
      passingSelection.back().push_back( decideOnSingleObject( elIter, cutIndex ) );
    }
    // counting to see if the cut failed entirely - this means fast rejection w/o checking combinations
    if ( all_of( passingSelection.back().begin(), passingSelection.back().end(), 
		 []( const std::vector<bool>& bits) { return bits.empty(); } ) )
      retrun StatusCode::SUCCESS;
  }
  
  // go to the tedious counting
  // compress the bits to "one" per RoI == OR of all decsions per RoI
  if ( m_decisionPerCluster ) {
    std::vector< std::vector<bool> > compressedPassingSelection;
    size_t bitIndex = 0;
    const xAOD::TrigEMCluster* cluster = 0;
    for ( auto& i: input ) {
      if ( i.cluster != cluster ) { // we have moved to a new cluster, need to OR with per RoI bits
	cluster = i.cluster;
	for ( size_t sel = 0; sel < m_multiplicity; ++sel ) {
	  compressedPassingSelection[sel].back() =  compressedPassingSelection[sel].back() or passingSelection[sel][bitIndex];
	}
      } else { // just copy bits
	compressedPassingSelection[sel].push_back( passingSelection[sel][bitIndex] );
      }
      bitIndex ++;
    }
    decision = countBits( compressedPassingSelection, m_multiplicity ) >= m_multiplicity;
  } else {
    // respect decisions per object
    decision = countBits( passingSelection, m_multiplicity ) >= m_multiplicity;
  }
  
  if ( decision == false )    
    retrun StatusCode::SUCCESS;

  // go back to the input and mark electrons as passing
  return markPassing( input, passingSelection );
}


StatusCode TrigL2ElectronHypoTool::decide(  std::vector<Input>& input )  const{

  // handle simples and most common case ( multiplicity == 1 ) in easiest possible manner
  if ( m_trackPt.size() == 1 ) {
    return inclusiveSelection( input );

  } else {    
    return multiplicitySelection( input );    
  }


  return StatusCode::SUCCESS;
}


