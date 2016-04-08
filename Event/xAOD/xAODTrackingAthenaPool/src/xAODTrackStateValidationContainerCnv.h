/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRACKINGATHENAPOOL_XAODMSOSCONTAINERCNV_H
#define XAODTRACKINGATHENAPOOL_XAODMSOSCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTracking/TrackStateValidationContainer.h"

/// Type definition for the converter's base
typedef T_AthenaPoolCustomCnv< xAOD::TrackStateValidationContainer,
                               xAOD::TrackStateValidationContainer >
   xAODTrackStateValidationContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::TrackStateValidationContainer class
 *
 *         Simple converter class making the xAOD::TrackStateValidationContainer
 *         class known to POOL.
 *
 *
 */
class xAODTrackStateValidationContainerCnv : public xAODTrackStateValidationContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODTrackStateValidationContainerCnv >;

public:
   /// Converter constructor
   xAODTrackStateValidationContainerCnv( ISvcLocator* svcLoc );
   
   /// Re-implemented function in order to get access to the SG key
   virtual StatusCode createObj( IOpaqueAddress* pAddr, DataObject*& pObj );
   
   /// Function preparing the container to be written out
   virtual xAOD::TrackStateValidationContainer* createPersistent( xAOD::TrackStateValidationContainer* trans );
   
   /// Function reading in the persistent object
   virtual xAOD::TrackStateValidationContainer* createTransient();

private:

   /// StoreGate key of the container just being created
   std::string m_key;

}; // class xAODMSOSContainerCnv

#endif 
