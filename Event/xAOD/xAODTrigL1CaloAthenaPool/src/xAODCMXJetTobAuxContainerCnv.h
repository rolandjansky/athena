// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id
#ifndef XAODTRIGL1CALOATHENAPOOL_XAODCMXJETTOBAUXCONTAINERCNV_H
#define XAODTRIGL1CALOATHENAPOOL_XAODCMXJETTOBAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTrigL1Calo/CMXJetTobAuxContainer.h"

/// Type definition for the converter's base
typedef T_AthenaPoolCustomCnv< xAOD::CMXJetTobAuxContainer , 
                               xAOD::CMXJetTobAuxContainer >
        xAODCMXJetTobAuxContainerCnvBase;

/**
  * @short POOL converter for the xAOD::CMXJetTobAuxContainer class
  *
  *         This is the converter doing the actual schema evolution
  *         of the package... The converter for xAOD::CMXJetTobContainer
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

class xAODCMXJetTobAuxContainerCnv : public xAODCMXJetTobAuxContainerCnvBase {

  // Declare the factory as our friend:
  friend class CnvFactory< xAODCMXJetTobAuxContainerCnv >;

  protected:
    /// Converter constructor
    xAODCMXJetTobAuxContainerCnv( ISvcLocator* svcLoc );

    /// Function preparing the container to be written out
    virtual xAOD::CMXJetTobAuxContainer* createPersistent( xAOD::CMXJetTobAuxContainer* trans );
    /// Function reading in the object from the input file
    virtual xAOD::CMXJetTobAuxContainer* createTransient();

}; // class xAODCMXJetTobAuxContainerCnv
#endif

