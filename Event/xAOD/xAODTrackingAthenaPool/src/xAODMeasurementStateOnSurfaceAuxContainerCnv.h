/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRACKINGATHENAPOOL_XAODMeasurementStateOnSurfaceAUXCONTAINERCNV_H
#define XAODTRACKINGATHENAPOOL_XAODMeasurementStateOnSurfaceAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTracking/MeasurementStateOnSurfaceAuxContainer.h"

/// Base class for the converter
typedef T_AthenaPoolCustomCnv< xAOD::MeasurementStateOnSurfaceAuxContainer,
                               xAOD::MeasurementStateOnSurfaceAuxContainer >
   xAODMeasurementStateOnSurfaceAuxContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::MeasurementStateOnSurfaceAuxContainer class
 *
 *         This is the converter doing the actual schema evolution
 *         of the package... The converter for xAOD::TrackParticleContainer
 *         doesn't do much, as the "interface classes" don't contain
 *         too much/any payload. Most of the payload is in the auxiliary
 *         containers like this one.
 *
 */
class xAODMeasurementStateOnSurfaceAuxContainerCnv :
   public xAODMeasurementStateOnSurfaceAuxContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODMeasurementStateOnSurfaceAuxContainerCnv >;

protected:
   /// Converter constructor
   xAODMeasurementStateOnSurfaceAuxContainerCnv( ISvcLocator* svcLoc );

   /// Function preparing the container to be written out
   virtual xAOD::MeasurementStateOnSurfaceAuxContainer*
   createPersistent( xAOD::MeasurementStateOnSurfaceAuxContainer* trans );
   /// Function reading in the object from the input file
   virtual xAOD::MeasurementStateOnSurfaceAuxContainer* createTransient();

}; // class xAODMeasurementStateOnSurfaceAuxContainerCnv

#endif 
