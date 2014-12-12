/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRACKINGATHENAPOOL_XAODPREPRAWDATAAUXCONTAINERCNV_H
#define XAODTRACKINGATHENAPOOL_XAODPREPRAWDATAAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTracking/TrackMeasurementValidationAuxContainer.h"

/// Base class for the converter
typedef T_AthenaPoolCustomCnv< xAOD::TrackMeasurementValidationAuxContainer,
                               xAOD::TrackMeasurementValidationAuxContainer >
   xAODTrackMeasurementValidationAuxContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::TrackMeasurementValidationAuxContainer class
 *
 *         This is the converter doing the actual schema evolution
 *         of the package... The converter for xAOD::TrackParticleContainer
 *         doesn't do much, as the "interface classes" don't contain
 *         too much/any payload. Most of the payload is in the auxiliary
 *         containers like this one.
 *
 * @author Edward Moyse <Edward.Moyse@cern.ch>
 */
class xAODTrackMeasurementValidationAuxContainerCnv :
   public xAODTrackMeasurementValidationAuxContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODTrackMeasurementValidationAuxContainerCnv >;

protected:
   /// Converter constructor
   xAODTrackMeasurementValidationAuxContainerCnv( ISvcLocator* svcLoc );

   /// Function preparing the container to be written out
   virtual xAOD::TrackMeasurementValidationAuxContainer*
   createPersistent( xAOD::TrackMeasurementValidationAuxContainer* trans );
   /// Function reading in the object from the input file
   virtual xAOD::TrackMeasurementValidationAuxContainer* createTransient();

}; // class xAODTrackMeasurementValidationAuxContainerCnv

#endif 
