/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
	
// System include(s):
#include <stdexcept>
	
// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"
	
// Core EDM include(s):
#include "AthContainers/tools/copyAuxStoreThinned.h"
	
// Local include(s):
#include "xAODTrackParticleAuxContainerCnv_v1.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/versions/TrackParticleContainer_v1.h"
	
/// Convenience macro for setting the level of output messages
#define MSGLVL MSG::DEBUG
	
/// Another convenience macro for printing messages in the converter
#define ATH_MSG( MSG )                          \
do {                                         \
  if( log.level() <= MSGLVL ) {             \
    log << MSGLVL << MSG << endmsg;        \
  }                                         \
} while( 0 )
	
xAODTrackParticleAuxContainerCnv_v1::xAODTrackParticleAuxContainerCnv_v1()
    : T_AthenaPoolTPCnvBase< xAOD::TrackParticleAuxContainer, xAOD::TrackParticleAuxContainer_v1 >() 
{
}
	
void xAODTrackParticleAuxContainerCnv_v1::
persToTrans(  const xAOD::TrackParticleAuxContainer_v1* oldObj, 
              xAOD::TrackParticleAuxContainer* newObj,
              MsgStream& log ) {
	
  // Greet the user:
  ATH_MSG( "Converting xAOD::TrackParticleAuxContainer_v1 to current version..." );
	
  // Clear the transient object:
  newObj->resize( 0 );
	
  // Copy the payload of the v1 object into the latest one by misusing
  // the thinning code a bit...
  SG::copyAuxStoreThinned( *oldObj, *newObj, 0 );

  // Set up interface containers on top of them:
	
  //The old  uses v_
  xAOD::TrackParticleContainer_v1 oldInt;
  for( size_t i = 0; i < oldObj->size(); ++i ) {
    oldInt.push_back( new xAOD::TrackParticle_v1() );
  }
  oldInt.setStore( oldObj );
	   
  xAOD::TrackParticleContainer newInt;
  for( size_t i = 0; i < newObj->size(); ++i ) {
    newInt.push_back( new xAOD::TrackParticle() );
  }
  newInt.setStore( newObj );
	
  unsigned int index=0;
  float x,y;
  uint8_t numberOfBLayerHits=0,numberOfBLayerSharedHits=0,numberOfBLayerOutliers=0,numberOfBLayerSplitHits=0,expectBLayerHit=0;
  for( size_t i = 0; i < oldInt.size(); ++i ) {
    index=0;
    if (oldInt[ i ]->indexOfParameterAtPosition (index, xAOD::FirstMeasurement)){
      x = oldInt[ i ]->parameterX(index);
      y = oldInt[ i ]->parameterY(index);
      newInt[ i ]->setRadiusOfFirstHit(std::sqrt(x*x + y*y));
    } else {
        ATH_MSG( "Can't find first hit!" );
    }
    

    static SG::AuxElement::ConstAccessor< uint8_t > numberOfBLayerHitsAcc( "numberOfBLayerHits" );
    static SG::AuxElement::ConstAccessor< uint8_t > numberOfBLayerSharedHitsAcc( "numberOfBLayerSharedHits" );
    static SG::AuxElement::ConstAccessor< uint8_t > numberOfBLayerOutliersAcc( "numberOfBLayerOutliers" );
    static SG::AuxElement::ConstAccessor< uint8_t > numberOfBLayerSplitHitsAcc( "numberOfBLayerSplitHits" );
    static SG::AuxElement::ConstAccessor< uint8_t > expectBLayerHitAcc( "expectBLayerHit" );

    if( numberOfBLayerHitsAcc.isAvailable( *( oldInt[ i ] ) ) ) {

      numberOfBLayerHits = numberOfBLayerHitsAcc( *( oldInt[ i ] ) );
      newInt[ i ]->setSummaryValue( numberOfBLayerHits ,xAOD::numberOfInnermostPixelLayerHits);
      
    }
    
    if( numberOfBLayerSharedHitsAcc.isAvailable( *( oldInt[ i ] ) ) ) {
      
      numberOfBLayerSharedHits = numberOfBLayerSharedHitsAcc( *( oldInt[ i ] ) );
      newInt[ i ]->setSummaryValue( numberOfBLayerSharedHits ,xAOD::numberOfInnermostPixelLayerSharedHits);
      
    }
    
    if( numberOfBLayerOutliersAcc.isAvailable( *( oldInt[ i ] ) ) ) {

      numberOfBLayerOutliers = numberOfBLayerOutliersAcc( *( oldInt[ i ] ) );
      newInt[ i ]->setSummaryValue( numberOfBLayerOutliers ,xAOD::numberOfInnermostPixelLayerOutliers);
      
    }

    if( numberOfBLayerSplitHitsAcc.isAvailable( *( oldInt[ i ] ) ) ) {
      
      numberOfBLayerSplitHits = numberOfBLayerSplitHitsAcc( *( oldInt[ i ] ) );
      newInt[ i ]->setSummaryValue( numberOfBLayerSplitHits ,xAOD::numberOfInnermostPixelLayerSplitHits);
      
    }

    if( expectBLayerHitAcc.isAvailable( *( oldInt[ i ] ) ) ) {
      expectBLayerHit = expectBLayerHitAcc( *( oldInt[ i ] ) );
      newInt[ i ]->setSummaryValue( expectBLayerHit ,xAOD::expectInnermostPixelLayerHit);
      
    }
  }
  
  // FIXME - what do we do about the identifier?
	
  // Print what happened:
  ATH_MSG( "Converting xAOD::TrackParticleAuxContainer_v1 to current version "
    "[OK]" );
	
  return;
}
	
/// This function should never be called, as we are not supposed to convert
/// object before writing.
///
void xAODTrackParticleAuxContainerCnv_v1::transToPers( const xAOD::TrackParticleAuxContainer*,
                                                       xAOD::TrackParticleAuxContainer_v1*,
                                                       MsgStream& log ) {
	
  log << MSG::ERROR
      << "Somebody called xAODTrackParticleAuxContainerCnv_v1::transToPers"
      << endmsg;
  throw std::runtime_error( "Somebody called xAODTrackParticleAuxContainerCnv_v1::"
    "transToPers" );
  return;
}
