/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MuonSelectionTool.cxx 299883 2014-03-28 17:34:16Z krasznaa $

// Local include(s):
#include "MuonSelectorTools/MuonSelectionTool.h"
#include "xAODTracking/TrackingPrimitives.h"

namespace CP {

   MuonSelectionTool::MuonSelectionTool( const std::string& name )
      : asg::AsgTool( name ),
        m_accept( "MuonSelection" ) {

      declareProperty( "MaxEta", m_maxEta = 2.5 );
   }

   StatusCode MuonSelectionTool::initialize() {

      // Greet the user:
      ATH_MSG_INFO( "Initialising..." );
      ATH_MSG_DEBUG( "Maximum eta: " << m_maxEta );

      // Set up the TAccept object:
      m_accept.addCut( "Eta",
                       "Selection of muons according to their pseudorapidity" );
      m_accept.addCut( "Quality",
                       "Selection of muons according to their tightness" );
      m_accept.addCut( "IDHits",
                       "Selection of muons according to whether they passed the MCP ID Hit cuts" );
      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   const Root::TAccept& MuonSelectionTool::getTAccept() const {

      return m_accept;
   }

   const Root::TAccept&
   MuonSelectionTool::accept( const xAOD::IParticle* p ) const {

      // Reset the result:
      m_accept.clear();

      // Check if this is a muon:
      if( p->type() != xAOD::Type::Muon ) {
         ATH_MSG_ERROR( "accept(...) Function received a non-muon" );
         return m_accept;
      }

      // Cast it to a muon:
      const xAOD::Muon* mu = dynamic_cast< const xAOD::Muon* >( p );
      if( ! mu ) {
         ATH_MSG_FATAL( "accept(...) Failed to cast particle to muon" );
         return m_accept;
      }

      // Let the specific function do the work:
      return accept( *mu );
   }

   const Root::TAccept& MuonSelectionTool::accept( const xAOD::Muon& mu ) const {

      // Reset the result:
      m_accept.clear();

      // Do the eta cut:
      ATH_MSG_VERBOSE( "Muon eta: " << mu.eta() );
      if( std::abs( mu.eta() ) > m_maxEta ) {
         return m_accept;
      }
      m_accept.setCutResult( "Eta", true );

      // Medium quality doesn't exist yet, hack for now with getQuality method
      // Do the quality cut(s):
      // ATH_MSG_VERBOSE( "Muon quality: " << mu.quality() );
      // if( mu.quality()!=xAOD::Muon::Medium ) {
      //    return m_accept;
      // }

      ATH_MSG_VERBOSE( "Muon quality: " << getQuality(mu));
      if(getQuality(mu) != xAOD::Muon::Medium){
      	return m_accept;
      }
      
      m_accept.setCutResult( "Muon quality", true );
      
      // Check not standalone 
      ATH_MSG_VERBOSE( "Type: " << mu.muonType () );
      if( mu.muonType ()==xAOD::Muon::MuonStandAlone ) {
         return m_accept;
      }
      m_accept.setCutResult( "Type", true );

      // Passes ID hit cuts
      // ATH_MSG_VERBOSE( "Passes ID Hit cuts " << mu.passesIDCuts () );
      // if( !mu.passesIDCuts () ) {
      //   return m_accept;
      // }

      ATH_MSG_VERBOSE( "Passes ID Hit cuts " << passedIDCuts(mu) );
      if( !passedIDCuts (mu) ) {
        return m_accept;
      }
  
      m_accept.setCutResult( "IDHits", true );
      
      // Return the result:
      return m_accept;
   }
   
   void MuonSelectionTool::setQuality( xAOD::Muon& mu ) const {
     mu.setQuality(getQuality(mu)); 
     return;
   }

   xAOD::Muon::Quality MuonSelectionTool::getQuality(const xAOD::Muon& mu ) const {
     using namespace xAOD;

     // ATH_MSG_VERBOSE( "Type: " << mu.muonType () );
     // ATH_MSG_VERBOSE( "Quality: " << mu.quality () );

     // Combined muons
     if (mu.muonType()==Muon_v1::Combined){

       uint8_t nprecisionLayers,nprecisionHoleLayers;

       if (!mu.primaryTrackParticle()->summaryValue(nprecisionLayers, numberOfPrecisionLayers) || 
         !mu.primaryTrackParticle()->summaryValue(nprecisionHoleLayers, numberOfPrecisionHoleLayers)){
         ATH_MSG_VERBOSE("getQuality - #precision layers missing in combined muon! Aborting.");
         return Muon_v1::Loose;
       }

       float momBalanceSig=0.0;

       // if (!mu.parameter(momBalanceSig, Muon_v1::momentumBalanceSignificance)){
       // 	ATH_MSG_VERBOSE("getQuality - momentumBalanceSignificance missing in combined muon! Don't abort yet since these variables are not filled!");
       // 	//momentum balance signficance variable is missing, proceed with a check of the precision layers
       // 	//return Muon_v1::Medium;
       // }
      
       if( (momBalanceSig<4.0)
         && (nprecisionLayers > 0 || ( nprecisionLayers == 1 && nprecisionHoleLayers < 2 ) ) ) {
         return Muon_v1::Medium;
       }
      
       //didn't pass the set of requirements for a medium combined muon
       return Muon_v1::Loose;
     }

     if (mu.author()==Muon_v1::STACO || mu.author()==Muon_v1::MuTag){
       // What about MuTagIMO? CHECK!
       return Muon_v1::Medium;
     }
    
     // SA muons
     if (mu.muonType()==Muon_v1::MuonStandAlone && fabs(mu.eta())>2.5){
       uint8_t nprecisionLayers;
       uint8_t ninnerSmallHits;
       if (!mu.primaryTrackParticle()->summaryValue(nprecisionLayers, numberOfPrecisionLayers)){
         ATH_MSG_VERBOSE("getQuality - numberOfPrecisionLayers missing in SA muon! Aborting.");
         return Muon_v1::Loose;
       }
       if (!mu.summaryValue(ninnerSmallHits, innerSmallHits)){
         ATH_MSG_VERBOSE("getQuality - innerSmallHits missing in SA muon! Aborting.");
         return Muon_v1::Loose;
       }
      
       if( nprecisionLayers>2 || ninnerSmallHits>3 ) {
         return Muon_v1::Medium; // Can't currently get tighter than medium... will need to change logic here if this is no longer true.
       }
     }

     // CaloTagged and SegmentTagged muons
     if (mu.muonType()==Muon_v1::SegmentTagged || mu.muonType()==Muon_v1::CaloTagged ){
       return Muon_v1::Loose;
     }
    
     //mu.setQuality(Muon_v1::Loose); // Is this really what we want to do? Having all other muons 'loose' with no selection, wasting information.
      return Muon_v1::Loose;
   }


   
  void MuonSelectionTool::setPassesIDCuts( xAOD::Muon& mu ) const {
     // Any time a cut is failed, we just abort - the bit is set to true only at the very end
     // This is a bit contrary to the concept of using TAccept, but we want to be able to call this in standard reco tools, without lots of output.
     // Might add DEBUG/VERBOSE level output, later.
   //      mu.setPassesIDCuts(false);// Explicitly set this here, in case this is being called on an updated muon (i.e. might be true)
   //      using namespace xAOD;
   //      uint8_t value1=0;
   //      uint8_t value2=0;
   //      if (mu.primaryTrackParticle()->summaryValue(value1, numberOfPixelHits) &&
   // mu.primaryTrackParticle()->summaryValue(value2, numberOfPixelDeadSensors) ) {
   //        if (value1+value2==0) return;
   //      } else {
   //        ATH_MSG_WARNING("setPassesIDCuts - pixel values are missing! Aborting.");
   //        return;
   //      }
   //
   //      if (mu.primaryTrackParticle()->summaryValue(value1, numberOfSCTHits) &&
   // mu.primaryTrackParticle()->summaryValue(value2, numberOfSCTDeadSensors)){
   //        if (value1+value2<=4) return;
   //      } else {
   //        ATH_MSG_WARNING("setPassesIDCuts - SCT values are missing! Aborting.");
   //        return;
   //      }
   //
   //      if (mu.primaryTrackParticle()->summaryValue(value1, numberOfPixelHoles) &&
   // mu.primaryTrackParticle()->summaryValue(value2, numberOfSCTHoles)){
   //        if (value1+value2>=3) return;
   //      } else {
   //        ATH_MSG_WARNING("setPassesIDCuts - Si hole values are missing! Aborting.");
   //        return;
   //      }
   //
   //      float abseta = std::abs(mu.eta());
   //      if (mu.primaryTrackParticle()->summaryValue(value1, numberOfTRTHits) &&
   // mu.primaryTrackParticle()->summaryValue(value2, numberOfTRTOutliers)){
   //        // Okay, so have values available - now check if TRT cuts met...
   //        uint8_t totTRThits=value1+value2;
   //        if (!((0.1<abseta && abseta<=1.9 && totTRThits>5 && value2<(0.9 * totTRThits)) || (abseta <= 0.1 || abseta > 1.9)))
   //          return;
   //      } else {
   //        ATH_MSG_WARNING("setPassesIDCuts - TRT values are missing! Aborting.");
   //        return;
   //      }
   //      // Reached end - all ID hit cuts are passed.
   //      
   mu.setPassesIDCuts(passedIDCuts(mu));
  }
  
  bool MuonSelectionTool::passedIDCuts( const xAOD::Muon& mu ) const {
     using namespace xAOD;
     uint8_t value1=0;
     uint8_t value2=0;
     
     if (mu.primaryTrackParticle()->summaryValue(value1, numberOfPixelHits) && 
         mu.primaryTrackParticle()->summaryValue(value2, numberOfPixelDeadSensors) ) {
       if (value1+value2==0) return false;
     } else {
       ATH_MSG_WARNING("isPassedIDCuts - pixel values are missing! Aborting.");
       return false;
     }

     if (mu.primaryTrackParticle()->summaryValue(value1, numberOfSCTHits) && 
         mu.primaryTrackParticle()->summaryValue(value2, numberOfSCTDeadSensors)){
       if (value1+value2<=4) return false;
     } else {
       ATH_MSG_WARNING("isPassedIDCuts - SCT values are missing! Aborting.");
       return false;
     }

     if (mu.primaryTrackParticle()->summaryValue(value1, numberOfPixelHoles) && 
         mu.primaryTrackParticle()->summaryValue(value2, numberOfSCTHoles)){
       if (value1+value2>=3) return false;
     } else {
       ATH_MSG_WARNING("isPassedIDCuts - Si hole values are missing! Aborting.");
       return false;
     }

     float abseta = std::abs(mu.eta());
     if (mu.primaryTrackParticle()->summaryValue(value1, numberOfTRTHits) && 
         mu.primaryTrackParticle()->summaryValue(value2, numberOfTRTOutliers)){
       // Okay, so have values available - now check if the TRT cuts are met...
       uint8_t totTRThits=value1+value2;
       if (!((0.1<abseta && abseta<=1.9 && totTRThits>5 && value2<(0.9 * totTRThits)) || (abseta <= 0.1 || abseta > 1.9)))
         return false;
     } else {

       ATH_MSG_WARNING("isPassedIDCuts - TRT values are missing! Aborting.");
       return false;
     }
     // Reached end - all ID hit cuts are passed.
     return true;
  } // passedIDCuts
} // namespace CP
