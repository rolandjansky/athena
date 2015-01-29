// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id
#ifndef XAODTRIGL1CALOATHENAPOOL_XAODCMXETSUMSCONTAINERCNV_H
#define XAODTRIGL1CALOATHENAPOOL_XAODCMXETSUMSCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTrigL1Calo/CMXEtSumsContainer.h"

/// Type definition for the converter's base
typedef T_AthenaPoolCustomCnv< xAOD::CMXEtSumsContainer , 
                               xAOD::CMXEtSumsContainer >
        xAODCMXEtSumsContainerCnvBase;

/**
  * @short POOL converter for the xAOD::CMXEtSumsContainer class
  *
  *        Simple converter class making the xAOD::CMXEtSumsContainer class known to POOL
  *
  * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
  * @author John Morris <john.morris@cern.ch>
  *
  * $Revision
  * $Date 
  *
  */

class xAODCMXEtSumsContainerCnv : public xAODCMXEtSumsContainerCnvBase {

  // Declare the factory as our friend:
  friend class CnvFactory< xAODCMXEtSumsContainerCnv >;

  public:
    /// Converter constructor
    xAODCMXEtSumsContainerCnv( ISvcLocator* svcLoc );

    /// Function preparing the container to be written out
    virtual xAOD::CMXEtSumsContainer* createPersistent( xAOD::CMXEtSumsContainer* trans );
    /// Function reading in the persistent object
    virtual xAOD::CMXEtSumsContainer* createTransient();

  private:
    /// Function preparing a vertex object for persistence
    void toPersistent( xAOD::CMXEtSums* ) const;

}; // class xAODCMXEtSumsContainerCnv
#endif

