/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRACKINGATHENAPOOL_XAODMSOSCONTAINERCNV_H
#define XAODTRACKINGATHENAPOOL_XAODMSOSCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTracking/MeasurementStateOnSurfaceContainer.h"

/// Type definition for the converter's base
typedef T_AthenaPoolCustomCnv< xAOD::MeasurementStateOnSurfaceContainer,
                               xAOD::MeasurementStateOnSurfaceContainer >
   xAODMeasurementStateOnSurfaceContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::MeasurementStateOnSurfaceContainer class
 *
 *         Simple converter class making the xAOD::MeasurementStateOnSurfaceContainer
 *         class known to POOL.
 *
 *
 */
class xAODMeasurementStateOnSurfaceContainerCnv : public xAODMeasurementStateOnSurfaceContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODMeasurementStateOnSurfaceContainerCnv >;

public:
   /// Converter constructor
   xAODMeasurementStateOnSurfaceContainerCnv( ISvcLocator* svcLoc );
   
   /// Re-implemented function in order to get access to the SG key
   virtual StatusCode createObj( IOpaqueAddress* pAddr, DataObject*& pObj );
   
   /// Function preparing the container to be written out
   virtual xAOD::MeasurementStateOnSurfaceContainer* createPersistent( xAOD::MeasurementStateOnSurfaceContainer* trans );
   
   /// Function reading in the persistent object
   virtual xAOD::MeasurementStateOnSurfaceContainer* createTransient();

private:
   /// Function preparing a vertex object for persistence
   void toPersistent( xAOD::MeasurementStateOnSurface* np ) const;

   /// StoreGate key of the container just being created
   std::string m_key;

}; // class xAODMSOSContainerCnv

#endif 
