/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "DecisionHandling/HLTIdentifier.h"
#include "DecisionHandling/Combinators.h"
#include "AthenaMonitoring/MonitoredScope.h"

#include "TrigL2TauHypoTool.h"

using namespace TrigCompositeUtils;

TrigL2TauHypoTool::TrigL2TauHypoTool( const std::string& type,
						const std::string& name, 
						const IInterface* parent ) 
  : base_class( type, name, parent ),
    m_decisionId( HLT::Identifier::fromToolName( name ) ) {}

StatusCode TrigL2TauHypoTool::initialize()  {
  
  if ( !m_monTool.empty() ) CHECK( m_monTool.retrieve() );

  ATH_MSG_DEBUG( "Initialization completed successfully:" );
  ATH_MSG_DEBUG( "AcceptAll            = " 
		<< ( m_acceptAll==true ? "True" : "False" ) ); 
  //ATH_MSG_DEBUG( "TrackPt              = " << m_trackPt           );
  // Cluster-based pT cut
  ATH_MSG_DEBUG( "LowerPtCut = " << m_lowerPtCut );
  ATH_MSG_DEBUG( "ClusterEnergySumCone = " << m_clusterCone );

  // Cell-based shower requirement
  ATH_MSG_DEBUG( "UseCellCut = " << m_cellCut );
  ATH_MSG_DEBUG( "CellVariableCore = " << m_cellCore );
  ATH_MSG_DEBUG( "CellVariableOuter = "  << m_cellOuter );
  ATH_MSG_DEBUG( "CoreFractionCut = " << m_coreFractionCut );
  ATH_MSG_DEBUG( "HadRadiusCut = " << m_hadRadiusCut );

  /*declareMonitoredVariable("CutCounter"        , m_PassedCuts);
  declareMonitoredVariable("seedPt"            , m_seedPt);
  declareMonitoredVariable("fCore"             , m_fCore);
  declareMonitoredVariable("hadRadius"         , m_hadRadius);
  declareMonitoredVariable("dRBaryCenterRoI"   , m_dRBaryCenterRoI);*/



  /*ATH_MSG_DEBUG( "CaloTrackdETA        = " << m_caloTrackDEta        );
  ATH_MSG_DEBUG( "CaloTrackdPHI        = " << m_caloTrackDPhi        );
  ATH_MSG_DEBUG( "CaloTrackdEoverPLow  = " << m_caloTrackdEoverPLow );
  ATH_MSG_DEBUG( "CaloTrackdEoverPHigh = " << m_caloTrackdEoverPHigh );
  ATH_MSG_DEBUG( "TRTRatio = " << m_trtRatio );

  std::vector<size_t> sizes( {m_trackPt.size(), m_caloTrackDEta.size(), m_caloTrackDPhi.size(), m_caloTrackdEoverPLow.size(), m_caloTrackdEoverPHigh.size(), m_trtRatio.size() } );


  if ( *std::min_element( sizes.begin(), sizes.end() ) != *std::max_element( sizes.begin(), sizes.end() )  ) {
    ATH_MSG_ERROR( "Missconfiguration, cut properties listed above ( when DEBUG ) have different dimensions shortest: " <<  *std::min_element( sizes.begin(), sizes.end() ) << " longest " << *std::max_element( sizes.begin(), sizes.end() ) );
    return StatusCode::FAILURE;
  }*/

  m_multiplicity = m_lowerPtCut.size();
  ATH_MSG_DEBUG( "Tool configured for chain/id: " << m_decisionId  );

  return StatusCode::SUCCESS;
}


TrigL2TauHypoTool::~TrigL2TauHypoTool() {}

/*bool TrigL2TauHypoTool::decideOnSingleObject( const xAOD::CaloCluster* caloclus, 
						   size_t cutIndex ) const {
  using namespace Monitored;
  auto cutCounter = MonitoredScalar::declare<int>( "CutCounter", -1 );  
  auto cutIndexM  = MonitoredScalar::declare<int>( "CutIndex", cutIndex );  // one can do 2D plots for each cut independently
  auto ptCalo     = MonitoredScalar::declare( "PtCalo", -999. );
  auto ptTrack    = MonitoredScalar::declare( "PtTrack", -999. );
  auto dEtaCalo   = MonitoredScalar::declare( "CaloTrackEta", -1. );
  auto dPhiCalo   = MonitoredScalar::declare( "CaloTrackPhi", -1. );
  auto eToverPt   = MonitoredScalar::declare( "CaloTrackEoverP", -1. );
  auto caloEta    = MonitoredScalar::declare( "CaloEta", -100. );
  auto caloPhi    = MonitoredScalar::declare( "CaloEta", -100. );
  auto monitorIt  = MonitoredScope::declare( m_monTool, 
					     cutCounter, cutIndexM,
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

  ATH_MSG_VERBOSE( "Cut index " << cutIndex );
  if ( ptCalo < m_trackPt[cutIndex] ){ 
    ATH_MSG_VERBOSE( "Fails pt cut" << ptCalo << " < " << m_trackPt[cutIndex] );
    return  false;
  }
  cutCounter++;

  if ( dEtaCalo > m_caloTrackDEta[cutIndex] ) {
    ATH_MSG_VERBOSE( "Fails dEta cut " << dEtaCalo << " < " << m_caloTrackDEta[cutIndex] );
    return  false;
  }
  cutCounter++;
  if ( dPhiCalo > m_caloTrackDPhi[cutIndex] ) {
    ATH_MSG_VERBOSE( "Fails dPhi cut " << dPhiCalo << " < " << m_caloTrackDPhi[cutIndex] );
    return  false;
  }

  cutCounter++;
  if( eToverPt <  m_caloTrackdEoverPLow[cutIndex] ) {
    ATH_MSG_VERBOSE( "Fails eoverp low cut " << eToverPt << " < " <<  m_caloTrackdEoverPLow[cutIndex] );
    return  false;
  }
  cutCounter++;
  if ( eToverPt > m_caloTrackdEoverPHigh[cutIndex] ) {
    ATH_MSG_VERBOSE( "Fails eoverp high cut " << eToverPt << " < " << m_caloTrackdEoverPHigh[cutIndex] );
    return  false;
  }
  cutCounter++;
  if ( TRTHitRatio < m_trtRatio[cutIndex] ){
    ATH_MSG_VERBOSE( "Fails TRT cut " << TRTHitRatio << " < " << m_trtRatio[cutIndex] );
    return  false;
  }
  cutCounter++;
  ATH_MSG_DEBUG( "Passed selection" );
  return  true;

}



StatusCode TrigL2TauHypoTool::inclusiveSelection( std::vector<TauInfo>& input ) const {
    for ( auto i: input ) {
      if ( m_respectPreviousDecision 
	   and ( i.previousDecisionIDs.count( m_decisionId.numeric() ) == 0 ) ) continue; // the decision was negative or not even made in previous stage

      auto objDecision = decideOnSingleObject( i.cluster, 0 );
      if ( objDecision == true ) {
	addDecisionID( m_decisionId.numeric(), i.decision );
      }
    }
    return StatusCode::SUCCESS;
}


StatusCode TrigL2TauHypoTool::markPassing( std::vector<TauInfo>& input, const std::set<size_t>& passing ) const {

  for ( auto idx: passing ) 
    addDecisionID( m_decisionId.numeric(), input[idx].decision );
  return StatusCode::SUCCESS;
}


StatusCode TrigL2TauHypoTool::multiplicitySelection( std::vector<TauInfo>& input ) const {
  HLT::Index2DVec passingSelection( m_multiplicity );
  
  for ( size_t cutIndex = 0; cutIndex < m_multiplicity; ++ cutIndex ) {
    size_t elIndex{ 0 };
    for ( auto elIter =  input.begin(); elIter != input.end(); ++elIter, ++elIndex ) {
      if ( passed( m_decisionId.numeric(), elIter->previousDecisionIDs ) ) {	
	if ( decideOnSingleObject( elIter->cluster, cutIndex ) ) {
	  passingSelection[cutIndex].push_back( elIndex );
	}
      }
    }
    // checking if by chance none of the objects passed the single obj selection, if so there will be no valid combination and we can skip
    if ( passingSelection[cutIndex].empty() ) {
      ATH_MSG_DEBUG( "No object passed selection " << cutIndex << " rejecting" );
      return StatusCode::SUCCESS;
    }
  }
  
  // go to the tedious counting
  std::set<size_t> passingIndices;
  if ( m_decisionPerCluster ) {            
    // additional constrain has to be applied for each combination
    // from each combination we extract set of clustusters associated to it
    // if all are distinct then size of the set should be == size of combination, 
    // if size of clusters is smaller then the combination consists of electrons from the same RoI
    // and ought to be ignored
    auto notFromSameRoI = [&]( const HLT::Index1DVec& comb ) {
      std::set<const xAOD::CaloCluster*> setOfClusters;
      for ( auto index: comb ) {
	setOfClusters.insert( input[index].cluster );
      }
      return setOfClusters.size() == comb.size();
    };

    HLT::elementsInUniqueCombinations( passingSelection, passingIndices, notFromSameRoI );    
  } else {
    HLT::elementsInUniqueCombinations( passingSelection, passingIndices );
  }
  
  return markPassing( input, passingIndices );
}
*/

bool TrigL2TauHypoTool::decide( const ITrigL2TauCaloHypoTool::ClusterInfo& input ) const {

  bool pass = false;

  using namespace Monitored;

  if ( m_acceptAll ) {
    pass = true;
    ATH_MSG_DEBUG( "AcceptAll property is set: taking all events" );
  } else {
    pass = false;
    ATH_MSG_DEBUG( "AcceptAll property not set: applying selection" );
  }
  auto roiDescriptor = input.roi;

  if ( fabs( roiDescriptor->eta() ) > 2.6 ) {
      ATH_MSG_DEBUG( "REJECT The cluster had eta coordinates beyond the EM fiducial volume : " << roiDescriptor->eta() << "; stop the chain now" );
      pass=false; // special case       
      return pass;
  } 

  ATH_MSG_DEBUG( "; RoI ID = " << roiDescriptor->roiId()
       << ": Eta = " << roiDescriptor->eta()
       << ", Phi = " << roiDescriptor->phi() );

  // fill local variables for RoI reference position
  double etaRef = roiDescriptor->eta();
  double phiRef = roiDescriptor->phi();
  // correct phi the to right range ( probably not needed anymore )   
  if ( fabs( phiRef ) > M_PI ) phiRef -= 2*M_PI; // correct phi if outside range

  // got this far => passed!
  pass = true;

  // Reach this point successfully  
  ATH_MSG_DEBUG( "pass = " << pass );

  return pass;
}






StatusCode TrigL2TauHypoTool::decide(  std::vector<ClusterInfo>& input )  const {

  for ( auto& i: input ) {
    if ( passed ( m_decisionId.numeric(), i.previousDecisionIDs ) ) {
      if ( decide( i ) ) {
   addDecisionID( m_decisionId, i.decision );
      }
    }
  }
  return StatusCode::SUCCESS;
}


