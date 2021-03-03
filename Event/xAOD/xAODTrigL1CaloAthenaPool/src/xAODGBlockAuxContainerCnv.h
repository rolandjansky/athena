/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALOATHENAPOOL_XAODGBLOCKAUXCONTAINERCNV_H
#define XAODTRIGL1CALOATHENAPOOL_XAODGBLOCKAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTrigL1Calo/GBlockAuxContainer.h"

/// Base class for the converter
typedef T_AthenaPoolCustomCnv<xAOD::GBlockAuxContainer,
                              xAOD::GBlockAuxContainer>
    xAODGBlockAuxContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::GBlockAuxContainer class
 *
 *         This is the converter doing the actual schema evolution
 *         of the package... The converter for xAOD::GBlockContainer
 *         doesn't do much, as the "interface classes" don't contain
 *         too much/any payload. Most of the payload is in the auxiliary
 *         containers like this one.
 */
class xAODGBlockAuxContainerCnv : public xAODGBlockAuxContainerCnvBase
{

  // Declare the factory as our friend:
  friend class CnvFactory<xAODGBlockAuxContainerCnv>;

protected:
  /// Converter constructor
  xAODGBlockAuxContainerCnv(ISvcLocator *svcLoc);

  /// Function preparing the container to be written out
  virtual xAOD::GBlockAuxContainer *
  createPersistent(xAOD::GBlockAuxContainer *trans);
  /// Function reading in the object from the input file
  virtual xAOD::GBlockAuxContainer *createTransient();

}; // class xAODGBlockAuxContainerCnv

#endif // XAODTRIGL1CALOATHENAPOOL_XAODGBLOCKAUXCONTAINERCNV_H
