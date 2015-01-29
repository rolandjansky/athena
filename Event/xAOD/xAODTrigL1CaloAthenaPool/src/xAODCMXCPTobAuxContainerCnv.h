// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id
#ifndef XAODTRIGL1CALOATHENAPOOL_XAODCMXCPTOBAUXCONTAINERCNV_H
#define XAODTRIGL1CALOATHENAPOOL_XAODCMXCPTOBAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTrigL1Calo/CMXCPTobAuxContainer.h"

/// Type definition for the converter's base
typedef T_AthenaPoolCustomCnv< xAOD::CMXCPTobAuxContainer , 
                               xAOD::CMXCPTobAuxContainer >
        xAODCMXCPTobAuxContainerCnvBase;

/**
  * @short POOL converter for the xAOD::CMXCPTobAuxContainer class
  *
  *         This is the converter doing the actual schema evolution
  *         of the package... The converter for xAOD::CMXCPTobContainer
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

class xAODCMXCPTobAuxContainerCnv : public xAODCMXCPTobAuxContainerCnvBase {

  // Declare the factory as our friend:
  friend class CnvFactory< xAODCMXCPTobAuxContainerCnv >;

  protected:
    /// Converter constructor
    xAODCMXCPTobAuxContainerCnv( ISvcLocator* svcLoc );

    /// Function preparing the container to be written out
    virtual xAOD::CMXCPTobAuxContainer* createPersistent( xAOD::CMXCPTobAuxContainer* trans );
    /// Function reading in the object from the input file
    virtual xAOD::CMXCPTobAuxContainer* createTransient();

}; // class xAODCMXCPTobAuxContainerCnv
#endif

