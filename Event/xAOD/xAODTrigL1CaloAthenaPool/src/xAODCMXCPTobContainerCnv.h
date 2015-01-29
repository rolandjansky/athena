// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id
#ifndef XAODTRIGL1CALOATHENAPOOL_XAODCMXCPTOBCONTAINERCNV_H
#define XAODTRIGL1CALOATHENAPOOL_XAODCMXCPTOBCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTrigL1Calo/CMXCPTobContainer.h"

/// Type definition for the converter's base
typedef T_AthenaPoolCustomCnv< xAOD::CMXCPTobContainer , 
                               xAOD::CMXCPTobContainer >
        xAODCMXCPTobContainerCnvBase;

/**
  * @short POOL converter for the xAOD::CMXCPTobContainer class
  *
  *        Simple converter class making the xAOD::CMXCPTobContainer class known to POOL
  *
  * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
  * @author John Morris <john.morris@cern.ch>
  *
  * $Revision
  * $Date 
  *
  */

class xAODCMXCPTobContainerCnv : public xAODCMXCPTobContainerCnvBase {

  // Declare the factory as our friend:
  friend class CnvFactory< xAODCMXCPTobContainerCnv >;

  public:
    /// Converter constructor
    xAODCMXCPTobContainerCnv( ISvcLocator* svcLoc );

    /// Function preparing the container to be written out
    virtual xAOD::CMXCPTobContainer* createPersistent( xAOD::CMXCPTobContainer* trans );
    /// Function reading in the persistent object
    virtual xAOD::CMXCPTobContainer* createTransient();

  private:
    /// Function preparing a vertex object for persistence
    void toPersistent( xAOD::CMXCPTob* ) const;

}; // class xAODCMXCPTobContainerCnv
#endif

