/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRACKINGATHENAPOOL_XAODTrackStateValidationAUXCONTAINERCNV_H
#define XAODTRACKINGATHENAPOOL_XAODTrackStateValidationAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTracking/TrackStateValidationAuxContainer.h"

/// Base class for the converter
typedef T_AthenaPoolCustomCnv< xAOD::TrackStateValidationAuxContainer,
                               xAOD::TrackStateValidationAuxContainer >
   xAODTrackStateValidationAuxContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::TrackStateValidationAuxContainer class
 *
 *         This is the converter doing the actual schema evolution
 *         of the package... The converter for xAOD::TrackParticleContainer
 *         doesn't do much, as the "interface classes" don't contain
 *         too much/any payload. Most of the payload is in the auxiliary
 *         containers like this one.
 *
 */
class xAODTrackStateValidationAuxContainerCnv :
   public xAODTrackStateValidationAuxContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODTrackStateValidationAuxContainerCnv >;

protected:
   /// Converter constructor
   xAODTrackStateValidationAuxContainerCnv( ISvcLocator* svcLoc );

   /// Function preparing the container to be written out
   virtual xAOD::TrackStateValidationAuxContainer*
   createPersistent( xAOD::TrackStateValidationAuxContainer* trans );
   /// Function reading in the object from the input file
   virtual xAOD::TrackStateValidationAuxContainer* createTransient();

}; // class xAODTrackStateValidationAuxContainerCnv

#endif 
