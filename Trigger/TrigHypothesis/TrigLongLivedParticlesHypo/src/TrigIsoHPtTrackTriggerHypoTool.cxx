/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

  * Trigger Hypo Tool, that is aimed at triggering high pt isolated tracks 
  * author Ismet Siral <ismet.siral@cern.ch> - University of Oregon
*/


#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/Combinators.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "TrigIsoHPtTrackTriggerHypoTool.h"
#include "GaudiKernel/PhysicalConstants.h"

using namespace TrigCompositeUtils;

TrigIsoHPtTrackTriggerHypoTool::TrigIsoHPtTrackTriggerHypoTool( const std::string& type, 
						const std::string& name, 
						const IInterface* parent ) 
  : AthAlgTool( type, name, parent ),
    m_decisionId( HLT::Identifier::fromToolName( name ) ) {}

StatusCode TrigIsoHPtTrackTriggerHypoTool::initialize()  {
  
  if ( !m_monTool.empty() ) CHECK( m_monTool.retrieve() );

  ATH_MSG_VERBOSE( "Initialization completed successfully:" );

  //Track Trigger Kinematic Requirments
  ATH_MSG_VERBOSE( "MaxTrackPt           = " << m_TrackPt ); 
  ATH_MSG_VERBOSE( "MaxTrackEta          = " << m_TrackEta ); 
  ATH_MSG_VERBOSE( "MaxTrackd0           = " << m_Trackd0 );
  ATH_MSG_VERBOSE( "MaxTrackd0Sig        = " << m_Trackd0Sig );
  ATH_MSG_VERBOSE( "MinTrackNPixHits     = " << m_TrackNPixHits );
  ATH_MSG_VERBOSE( "MinTrackNSCTHits     = " << m_TrackNSCTHits );
  //Track Trigger Isolation Requirments
  ATH_MSG_VERBOSE( "TrackIsoCone         = " << m_IsoDR );
  ATH_MSG_VERBOSE( "MinIsoTrackPt        = " << m_IsoPt );
  ATH_MSG_VERBOSE( "EnableTrackIsolation = " << m_doIso );
  ATH_MSG_VERBOSE( "EnableCumulIsolation = " << m_IsoCum );

  std::vector<size_t> sizes( {m_TrackPt.size(), m_TrackEta.size(), m_Trackd0.size( ), m_Trackd0Sig.size(), m_TrackNPixHits.size() ,  m_TrackNSCTHits.size() , m_doIso.size() , m_IsoDR.size() , m_IsoPt.size() } );


  if ( *std::min_element( sizes.begin(), sizes.end() ) != *std::max_element( sizes.begin(), sizes.end() )  ) {
    ATH_MSG_ERROR( "Missconfiguration, cut properties listed above ( when INFO ) have different dimensions shortest: " <<  *std::min_element( sizes.begin(), sizes.end() ) << " longest " << *std::max_element( sizes.begin(), sizes.end() ) );
    return StatusCode::FAILURE;
  }

  ATH_MSG_VERBOSE( "Tool configured for chain/id: " << m_decisionId  );

  return StatusCode::SUCCESS;
}


TrigIsoHPtTrackTriggerHypoTool::~TrigIsoHPtTrackTriggerHypoTool() {}

bool TrigIsoHPtTrackTriggerHypoTool::decideOnSingleObject( const xAOD::TrackParticle_v1* track, const xAOD::TrackParticleContainer* AllTracks, size_t cutIndex ) const {
  auto cutCounter = Monitored::Scalar<int>( "CutCounter", -1 );  
  auto cutIndexM  = Monitored::Scalar<int>( "CutIndex", cutIndex );  // one can do 2D plots for each cut independently

  auto MONtrackPt     = Monitored::Scalar( "trackPt", -999. );
  auto MONtrackd0    = Monitored::Scalar( "trackd0", -999. );
  auto MONtrackNPixHits   = Monitored::Scalar( "trackNPixHits", -999. );
  auto MONtrackNSCTHits   = Monitored::Scalar( "trackNSCTHits", -999. );
  auto MONtrackd0Sig   = Monitored::Scalar( "trackd0Sig", -999. );
  auto MONtrackEta     = Monitored::Scalar( "trackEta", -999. );
  auto MONtrackIsoPt   = Monitored::Scalar( "trackIsoPt", -999. );
  auto MONtrackAggrIsoPt   = Monitored::Scalar( "trackAggrIsoPt", -999. );
  auto monitorIt  = Monitored::Group( m_monTool, cutCounter, cutIndexM,
				      MONtrackPt, MONtrackEta, 
				      MONtrackd0, MONtrackd0Sig,
				      MONtrackNPixHits, MONtrackNSCTHits, 
				      MONtrackIsoPt, MONtrackAggrIsoPt   
				      );




  //Checking Track pT Requirments
  auto trackPt= (track)->pt();
  if( trackPt < m_TrackPt[cutIndex] ) { // Check track pT requirments 
    ATH_MSG_DEBUG( "Fails pt cut" << trackPt << " < " << m_TrackPt[cutIndex] );
    return false;

  }
  cutCounter++;


  //Checking Track Eta Requirments
  auto trackEta=std::abs( (track)->p4().Eta() );
  if(  trackEta > m_TrackEta[cutIndex] ) { // Check track pT requirments 
    ATH_MSG_DEBUG( "Fails Eta cut" << trackEta << " > " << m_TrackEta[cutIndex] );
    return false;

  }
  cutCounter++;


  
  //Checking Track d0 Requirments
  auto trackd0= std::abs((track)->d0());
  if( trackd0 > m_Trackd0[cutIndex]) {
    ATH_MSG_DEBUG( "Fails d0 cut" << trackd0 << " > " << m_Trackd0[cutIndex] );
    return false;

  }
  cutCounter++;


  //Checking Track d0 Sig Requirments
  auto trackd0Sig= std::abs(xAOD::TrackingHelpers::d0significance(track));
  if( trackd0Sig > m_Trackd0Sig[cutIndex]) {
    ATH_MSG_DEBUG( "Fails d0 Sig cut" << trackd0Sig << " > " << m_Trackd0Sig[cutIndex] );
    return false;

  }
  cutCounter++;


  //Attempt to read track summary, and save info on number of pixels
  uint8_t trackNPixHits=0;
  if ( ! (track)->summaryValue(trackNPixHits,xAOD::numberOfPixelHits )) { // Cannot obtain the track summary
    ATH_MSG_DEBUG( "Failed to retrieve pedigree parameters");  
    return  false;
  }
  ATH_MSG_DEBUG( "Succesfully retrieved pedigree parameters");  
  cutCounter++;
 //Attempt to read track summary, and save info on number of SCT hits
  uint8_t trackNSCTHits=0;
  if ( ! (track)->summaryValue(trackNSCTHits,xAOD::numberOfSCTHits )) { // Cannot obtain the track summary
    ATH_MSG_DEBUG( "Failed to retrieve pedigree parameters");  
    return  false;
  }
  ATH_MSG_DEBUG( "Succesfully retrieved pedigree parameters");  
  cutCounter++;



  //Checking Track Min number of Pix Hits Requirments
  if( trackNPixHits <= m_TrackNPixHits[cutIndex] ){

    ATH_MSG_DEBUG( "Fails numperOfPixelHits cut" << trackNPixHits  << " <= " << m_TrackNPixHits[cutIndex]  );
    return false;
  }
  cutCounter++;    


  //Checking Track Min number of SCT Hits Requirments
  if( trackNSCTHits <= m_TrackNSCTHits[cutIndex] ){

    ATH_MSG_DEBUG( "Fails numperOfSCTHits cut" << trackNSCTHits  << " <= " << m_TrackNSCTHits[cutIndex]  );
    return false;
  }
  cutCounter++;    


  //Definning a cumlative pT variable that adds up the momentum of all tracks

  //If isolation is applied, loop over all tracks, and veto the event if there is a track above a certain threshold in the isolation cone
  if (m_doIso[cutIndex]) {
    float CumulativePT = 0.0;
    
    for (auto trackIter = AllTracks->begin(); trackIter != AllTracks->end(); ++trackIter){

      //Skip the track that is out of DR
      if( (*trackIter)==track) continue;
      if (track->p4().DeltaR((*trackIter)->p4()) > m_IsoDR[cutIndex] ) continue;

      //Skip the track that doens't have many Pixel and SCT Hits (Quality Check)
      uint8_t iterPix=0;
      if ( ! (*trackIter)->summaryValue(iterPix,xAOD::numberOfPixelHits ))  
	continue;
      uint8_t iterSCT=0;
      if ( ! (*trackIter)->summaryValue(iterSCT,xAOD::numberOfSCTHits )) 
	continue;

      if (iterPix<=m_TrackNPixHits[cutIndex] ) continue;
      if (iterSCT<=2 ) continue;


      //If cumalitve, add up the momentum of the track, if it's a bove a certain threshold kill the trac

      if(m_IsoCum[cutIndex]){
	CumulativePT+=(*trackIter)->pt();
	if(CumulativePT>=m_IsoPt[cutIndex]) {
	  MONtrackAggrIsoPt=CumulativePT;
	  ATH_MSG_DEBUG( "Fails Cum Isolation cut"  );    
	  return false;
	}
      }
      //If not cumlaitve, veto the track only if one track is bigger then the threshold
      else {
	if ((*trackIter)->pt() > MONtrackIsoPt ) {
	  MONtrackIsoPt=(*trackIter)->pt();
	}

	if( (*trackIter)->pt()>=m_IsoPt[cutIndex] ) {
	  ATH_MSG_DEBUG( "Fails Isolation cut"  );    
	  return false;
	}
      }


     
    }

    MONtrackAggrIsoPt=CumulativePT;
    cutCounter++;
  }

  //Monitorung histograms are filled at the end, only if they pass the selection. (For optimisation studies)
  MONtrackNPixHits=trackNPixHits;
  MONtrackNSCTHits=trackNSCTHits; 
  MONtrackPt= trackPt;
  MONtrackEta= trackEta;
  MONtrackd0= trackd0;
  MONtrackd0Sig= trackd0Sig;



  ATH_MSG_DEBUG( "Passed selection" );
  return  true;

}

StatusCode TrigIsoHPtTrackTriggerHypoTool::inclusiveSelection( std::vector<TrackInfo>& input ) const {
    for ( auto i: input ) {

      auto objDecision = decideOnSingleObject( i.track, i.AllTracks, 0 );
      if ( objDecision == true ) {

	addDecisionID( m_decisionId.numeric(), i.decision  );
      }
    }
    return StatusCode::SUCCESS;
}


StatusCode TrigIsoHPtTrackTriggerHypoTool::markPassing( std::vector<TrackInfo>& input, const std::set<size_t>& passing ) const {

  for ( auto idx: passing ) 
    addDecisionID( m_decisionId.numeric(), input[idx].decision );
  return StatusCode::SUCCESS;

}


StatusCode TrigIsoHPtTrackTriggerHypoTool::multiplicitySelection( std::vector<TrackInfo>& input ) const {
  HLT::Index2DVec passingSelection( m_multiplicity );
  
  for ( size_t cutIndex = 0; cutIndex < m_multiplicity; ++ cutIndex ) {
    size_t trkIndex{ 0 };
    for ( auto trkIter =  input.begin(); trkIter != input.end(); ++trkIter, ++trkIndex ) {
      
      if ( decideOnSingleObject( trkIter->track, trkIter->AllTracks, cutIndex ) ) {
	  passingSelection[cutIndex].push_back( trkIndex );

      }
    }
    // checking if by chance none of the objects passed the single obj selection, if so there will be no valid combination and we can skip
    if ( passingSelection[cutIndex].empty() ) {
      ATH_MSG_DEBUG( "No object passed selection " << cutIndex << " rejecting" );
      return StatusCode::SUCCESS;
    }
  }

  std::set<size_t> passingIndices;
  HLT::elementsInUniqueCombinations( passingSelection, passingIndices );
  return markPassing( input, passingIndices );
  

}

StatusCode TrigIsoHPtTrackTriggerHypoTool::decide(  std::vector<TrackInfo>& input )  const {
  // handle the simplest and most common case ( multiplicity == 1 ) in easiest possible manner
  if ( m_TrackPt.size() == 1 ) {
    return inclusiveSelection( input );

  } else {    
    return multiplicitySelection( input );    
  }

  return StatusCode::SUCCESS;
}
