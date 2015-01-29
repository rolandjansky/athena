// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id
#ifndef XAODTRIGL1CALOATHENAPOOL_XAODCPMTOBROIAUXCONTAINERCNV_H
#define XAODTRIGL1CALOATHENAPOOL_XAODCPMTOBROIAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTrigL1Calo/CPMTobRoIAuxContainer.h"

/// Type definition for the converter's base
typedef T_AthenaPoolCustomCnv< xAOD::CPMTobRoIAuxContainer , 
                               xAOD::CPMTobRoIAuxContainer >
        xAODCPMTobRoIAuxContainerCnvBase;

/**
  * @short POOL converter for the xAOD::CPMTobRoIAuxContainer class
  *
  *         This is the converter doing the actual schema evolution
  *         of the package... The converter for xAOD::CPMTobRoIContainer
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

class xAODCPMTobRoIAuxContainerCnv : public xAODCPMTobRoIAuxContainerCnvBase {

  // Declare the factory as our friend:
  friend class CnvFactory< xAODCPMTobRoIAuxContainerCnv >;

  protected:
    /// Converter constructor
    xAODCPMTobRoIAuxContainerCnv( ISvcLocator* svcLoc );

    /// Function preparing the container to be written out
    virtual xAOD::CPMTobRoIAuxContainer* createPersistent( xAOD::CPMTobRoIAuxContainer* trans );
    /// Function reading in the object from the input file
    virtual xAOD::CPMTobRoIAuxContainer* createTransient();

}; // class xAODCPMTobRoIAuxContainerCnv
#endif

