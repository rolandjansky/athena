/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODPFLOWATHENAPOOL_XAODTRACKCALOCLUSTERCONTAINERCNV_H
#define XAODPFLOWATHENAPOOL_XAODTRACKCALOCLUSTERCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODPFlow/TrackCaloClusterContainer.h"

/// Type definition for the converter's base
typedef T_AthenaPoolCustomCnv< xAOD::TrackCaloClusterContainer,
                               xAOD::TrackCaloClusterContainer >
   xAODTrackCaloClusterContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::TrackCaloClusterContainer class
 *
 *         Simple converter class making the xAOD::TrackCaloClusterContainer
 *         class known to POOL.
 *
 * @author Noemi Calace <Noemi.Calace@cern.ch>
 *
 */
class xAODTrackCaloClusterContainerCnv : public xAODTrackCaloClusterContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODTrackCaloClusterContainerCnv >;

public:
   /// Converter constructor
   xAODTrackCaloClusterContainerCnv( ISvcLocator* svcLoc );

   /// Re-implemented function in order to get access to the SG key
   virtual StatusCode createObj( IOpaqueAddress* pAddr, DataObject*& pObj );
   
   /// Function preparing the container to be written out
   virtual xAOD::TrackCaloClusterContainer* createPersistent( xAOD::TrackCaloClusterContainer* trans );

   /// Function reading in the persistent object
   virtual xAOD::TrackCaloClusterContainer* createTransient();

private:

   /// StoreGate key of the container just being created
   std::string m_key;

}; // class xAODTrackCaloClusterContainerCnv

#endif // XAODPFLOWATHENAPOOL_XAODTRACKCALOCLUSTERCONTAINERCNV_H