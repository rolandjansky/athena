/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

  * Trigger Hypo Tool, that is aimed at triggering high pt isolated tracks 
  * author Ismet Siral <ismet.siral@cern.ch> - University of Oregon
*/


#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/Combinators.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "TrigIsoHPtTrackTriggerHypoTool.h"
#include <math.h>
using namespace TrigCompositeUtils;

TrigIsoHPtTrackTriggerHypoTool::TrigIsoHPtTrackTriggerHypoTool( const std::string& type, 
						const std::string& name, 
						const IInterface* parent ) 
  : AthAlgTool( type, name, parent ),
    m_decisionId( HLT::Identifier::fromToolName( name ) ) {}

StatusCode TrigIsoHPtTrackTriggerHypoTool::initialize()  {
  
  if ( !m_monTool.empty() ) CHECK( m_monTool.retrieve() );

  ATH_MSG_VERBOSE( "Initialization completed successfully:" );
  // ATH_MSG_VERBOSE( "AcceptAll            = " 
  // 		<< ( m_acceptAll==true ? "True" : "False" ) );
  //Track Trigger Kinematic Requirments
  ATH_MSG_VERBOSE( "MaxTrackPt           = " << m_TrackPt           ); 
  ATH_MSG_VERBOSE( "MaxTrackd0           = " << m_Trackd0        );
  ATH_MSG_VERBOSE( "MinTrackNPixHits     = " << m_TrackNPixHits        );

  //Track Trigger Isolation Requirments
  ATH_MSG_VERBOSE( "EnableTrackIsolation = "      << m_doIso );
  ATH_MSG_VERBOSE( "EnableCumalitiveIsolation = " << m_IsoCum );
  ATH_MSG_VERBOSE( "TrackIsoCone         = "      << m_IsoDR );
  ATH_MSG_VERBOSE( "MinIsoTrackPt        = "      << m_IsoPt );

  std::vector<size_t> sizes( {m_TrackPt.size(), m_Trackd0.size() , m_TrackNPixHits.size() , m_doIso.size() , m_IsoDR.size() , m_IsoPt.size() } );


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
  auto trackPt     = Monitored::Scalar( "tackPt", -999. );
  auto trackd0    = Monitored::Scalar( "trackD0", -999. );
  auto trackNPixHits   = Monitored::Scalar( "trackNPixHits", -999. );
  auto monitorIt  = Monitored::Group( m_monTool, 
				      cutCounter, cutIndexM,
				      trackPt, trackd0, trackNPixHits    
				      );

  // if(m_acceptAll){
  //   return true;
  // }


  //Attempt to read track summary, and save info on number of pixels
  uint8_t tmpVal=0;
  // if ( (track)->summaryValue(tmpVal,xAOD::numberOfPixelHits )) { // Cannot obtain the track summary
  //   ATH_MSG_DEBUG( "Failed to retrieve pedigree parameters");  
  //   return  false;
  // }
  // cutCounter++;

  //Store the used track objects 
  trackNPixHits=tmpVal; 
  trackPt= (track)->pt();
  trackd0= (track)->d0();


  //Checking Track pT Requirments
  if( trackPt < m_TrackPt[cutIndex] ) { // Check track pT requirments 
    ATH_MSG_DEBUG( "Fails pt cut" << trackPt << " < " << m_TrackPt[cutIndex] );
    return false;

  }
  cutCounter++;
  
  //Checking Track d0 Requirments
  if( fabs(trackd0) > m_Trackd0[cutIndex]) {
    ATH_MSG_DEBUG( "Fails d0 cut" << fabs(trackd0) << " > " << m_Trackd0[cutIndex] );
    return false;

  }
  cutCounter++;


  //Checking Track Min number of Pix Hits Requirments
  // if( trackNPixHits < m_TrackNPixHits[cutIndex] ){

  //   ATH_MSG_DEBUG( "Fails numperOfPixelHits cut" << trackNPixHits  << " < " << m_TrackNPixHits[cutIndex]  );
  //   return false;
  // }
  // cutCounter++;    
  

  
  //Definning a cumlative pT variable that adds up the momentum of all tracks
  float CumulativePT = 0.0;
  //If isolation is applied, loop over all tracks, and veto the event if there is a track above a certain threshold in the isolation cone
  if (m_doIso[cutIndex]) {
    
    for (auto trackIter = AllTracks->begin(); trackIter != AllTracks->end(); ++trackIter){

      //Skip the track that is the considered track 
      if( (*trackIter)==track) continue;

      //Use the track only if it is withing a certain dR cut 
      
      float dEta = (*trackIter)->eta() - (track)->eta();
      float dPhi = fabs((*trackIter)->phi() - (track)->phi());
      dPhi = dPhi > CLHEP::pi ? dPhi-2*CLHEP::pi : dPhi;
      
      if( sqrt( dEta*dEta+dPhi*dPhi )>m_IsoDR[cutIndex] ) continue;


      //If cumalitve, add up the momentum of the track, if it's a bove a certain threshold kill the track
      if(m_IsoCum[cutIndex]){
	CumulativePT+=(*trackIter)->pt();
	if(CumulativePT>=m_IsoPt[cutIndex]) {
	  ATH_MSG_DEBUG( "Fails Cum Isolation cut"  );    
	  return false;
	}
      }
      //If not cumlaitve, veto the track only if one track is bigger then the threshold
      else {
	if( (*trackIter)->pt()>=m_IsoPt[cutIndex] ) {
	  ATH_MSG_DEBUG( "Fails Isolation cut"  );    
	  return false;
	}
      }
     
    }

    cutCounter++;
  }


  ATH_MSG_DEBUG( "Passed selection" );
  return  true;

}

StatusCode TrigIsoHPtTrackTriggerHypoTool::inclusiveSelection( std::vector<TrackInfo>& input ) const {
    for ( auto i: input ) {

      auto objDecision = decideOnSingleObject( i.track, i.AllTracks, 0 );
      if ( objDecision == true ) {
	addDecisionID( m_decisionId.numeric(), i.decision );
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
