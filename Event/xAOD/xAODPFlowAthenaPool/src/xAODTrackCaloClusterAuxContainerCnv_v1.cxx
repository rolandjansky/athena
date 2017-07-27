// $Id$
       
// System include(s):
#include <stdexcept>
       
// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"
       
// Core EDM include(s):
#include "AthContainers/tools/copyAuxStoreThinned.h"
       
// Local include(s):
#include "xAODTrackCaloClusterAuxContainerCnv_v1.h"
#include "xAODPFlow/TrackCaloClusterContainer.h"
#include "xAODPFlow/versions/TrackCaloClusterContainer_v1.h"
       
/// Convenience macro for setting the level of output messages
#define MSGLVL MSG::DEBUG
       
/// Another convenience macro for printing messages in the converter
#define ATH_MSG( MSG )                       \
do {                                         \
  if( log.level() <= MSGLVL ) {              \
    log << MSGLVL << MSG << endmsg;          \
  }                                          \
} while( 0 )
       
xAODTrackCaloClusterAuxContainerCnv_v1::xAODTrackCaloClusterAuxContainerCnv_v1()
    : T_AthenaPoolTPCnvBase< xAOD::TrackCaloClusterAuxContainer, xAOD::TrackCaloClusterAuxContainer_v1 >()
{
}
       
void xAODTrackCaloClusterAuxContainerCnv_v1::
persToTrans(  const xAOD::TrackCaloClusterAuxContainer_v1* oldObj,
              xAOD::TrackCaloClusterAuxContainer* newObj,
              MsgStream& log ) {
       
  // Greet the user:
  ATH_MSG( "Converting xAOD::TrackCaloClusterAuxContainer_v1 to current version..." );
       
  // Clear the transient object:
  newObj->resize( 0 );
       
  // Copy the payload of the v1 object into the latest one by misusing
  // the thinning code a bit...
  SG::copyAuxStoreThinned( *oldObj, *newObj, 0 );

  // Set up interface containers on top of them:
       
  //The old  uses v_
  xAOD::TrackCaloClusterContainer_v1 oldInt;
  for( size_t i = 0; i < oldObj->size(); ++i ) {
    oldInt.push_back( new xAOD::TrackCaloCluster_v1() );
  }
  oldInt.setStore( oldObj );
           
  xAOD::TrackCaloClusterContainer newInt;
  for( size_t i = 0; i < newObj->size(); ++i ) {
    newInt.push_back( new xAOD::TrackCaloCluster() );
  }
  newInt.setStore( newObj );
    
         
  // Print what happened:
  ATH_MSG( "Converting xAOD::TrackCaloClusterAuxContainer_v1 to current version [OK]" );
       
  return;
}
       
/// This function should never be called, as we are not supposed to convert
/// object before writing.
///
void xAODTrackCaloClusterAuxContainerCnv_v1::transToPers( const xAOD::TrackCaloClusterAuxContainer*,
                                                       xAOD::TrackCaloClusterAuxContainer_v1*,
                                                       MsgStream& log ) {
       
  log << MSG::ERROR
      << "Somebody called xAODTrackCaloClusterAuxContainerCnv_v1::transToPers"
      << endmsg;
  throw std::runtime_error( "Somebody called xAODTrackCaloClusterAuxContainerCnv_v1::"
    "transToPers" );
  return;
}