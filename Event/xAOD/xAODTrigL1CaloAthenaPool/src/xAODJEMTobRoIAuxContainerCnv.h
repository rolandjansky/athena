// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id
#ifndef XAODTRIGL1CALOATHENAPOOL_XAODJEMTOBROIAUXCONTAINERCNV_H
#define XAODTRIGL1CALOATHENAPOOL_XAODJEMTOBROIAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTrigL1Calo/JEMTobRoIAuxContainer.h"

/// Type definition for the converter's base
typedef T_AthenaPoolCustomCnv< xAOD::JEMTobRoIAuxContainer , 
                               xAOD::JEMTobRoIAuxContainer >
        xAODJEMTobRoIAuxContainerCnvBase;

/**
  * @short POOL converter for the xAOD::JEMTobRoIAuxContainer class
  *
  *         This is the converter doing the actual schema evolution
  *         of the package... The converter for xAOD::JEMTobRoIContainer
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

class xAODJEMTobRoIAuxContainerCnv : public xAODJEMTobRoIAuxContainerCnvBase {

  // Declare the factory as our friend:
  friend class CnvFactory< xAODJEMTobRoIAuxContainerCnv >;

  protected:
    /// Converter constructor
    xAODJEMTobRoIAuxContainerCnv( ISvcLocator* svcLoc );

    /// Function preparing the container to be written out
    virtual xAOD::JEMTobRoIAuxContainer* createPersistent( xAOD::JEMTobRoIAuxContainer* trans );
    /// Function reading in the object from the input file
    virtual xAOD::JEMTobRoIAuxContainer* createTransient();

}; // class xAODJEMTobRoIAuxContainerCnv
#endif

