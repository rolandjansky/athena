// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id
#ifndef XAODTRIGL1CALOATHENAPOOL_XAODCMXROIAUXCONTAINERCNV_H
#define XAODTRIGL1CALOATHENAPOOL_XAODCMXROIAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTrigL1Calo/CMXRoIAuxContainer.h"

/// Type definition for the converter's base
typedef T_AthenaPoolCustomCnv< xAOD::CMXRoIAuxContainer , 
                               xAOD::CMXRoIAuxContainer >
        xAODCMXRoIAuxContainerCnvBase;

/**
  * @short POOL converter for the xAOD::CMXRoIAuxContainer class
  *
  *         This is the converter doing the actual schema evolution
  *         of the package... The converter for xAOD::CMXRoIContainer
  *         doesn't do much, as the "interface classes" don't contain
  *         too much/any payload. Most of the payload is in the auxiliary
  *         containers like this one.
  *
  * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
  * @author John Morris <john.morris@cern.ch>
  *
  * $Revision
  * $Date 
  *
  */

class xAODCMXRoIAuxContainerCnv : public xAODCMXRoIAuxContainerCnvBase {

  // Declare the factory as our friend:
  friend class CnvFactory< xAODCMXRoIAuxContainerCnv >;

  protected:
    /// Converter constructor
    xAODCMXRoIAuxContainerCnv( ISvcLocator* svcLoc );

    /// Function preparing the container to be written out
    virtual xAOD::CMXRoIAuxContainer* createPersistent( xAOD::CMXRoIAuxContainer* trans );
    /// Function reading in the object from the input file
    virtual xAOD::CMXRoIAuxContainer* createTransient();

}; // class xAODCMXRoIAuxContainerCnv
#endif

