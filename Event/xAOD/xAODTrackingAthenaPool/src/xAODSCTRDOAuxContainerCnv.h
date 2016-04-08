/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRACKINGATHENAPOOL_XAODSCTRDOAUXCONTAINERCNV_H
#define XAODTRACKINGATHENAPOOL_XAODSCTRDOAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTracking/SCTRawHitValidationAuxContainer.h"

/// Base class for the converter
typedef T_AthenaPoolCustomCnv< xAOD::SCTRawHitValidationAuxContainer,
                               xAOD::SCTRawHitValidationAuxContainer >
  xAODSCTRDOAuxContainerCnvBase;

/**
 *  POOL converter for the xAOD::SCTRawHitValidationAuxContainer class
 */
class xAODSCTRDOAuxContainerCnv : public xAODSCTRDOAuxContainerCnvBase {

  // Declare the factory as our friend:
  friend class CnvFactory< xAODSCTRDOAuxContainerCnv >;

protected:
  /// Converter constructor
  xAODSCTRDOAuxContainerCnv( ISvcLocator* svcLoc );

  /// Function preparing the container to be written out
  virtual xAOD::SCTRawHitValidationAuxContainer* createPersistent( xAOD::SCTRawHitValidationAuxContainer* trans ) override;
  /// Function reading in the object from the input file
  virtual xAOD::SCTRawHitValidationAuxContainer* createTransient() override;

}; // class xAODSCTRDOAuxContainerCnv

#endif 
