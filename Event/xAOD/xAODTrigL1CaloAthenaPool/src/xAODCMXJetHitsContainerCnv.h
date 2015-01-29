// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id
#ifndef XAODTRIGL1CALOATHENAPOOL_XAODCMXJETHITSCONTAINERCNV_H
#define XAODTRIGL1CALOATHENAPOOL_XAODCMXJETHITSCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTrigL1Calo/CMXJetHitsContainer.h"

/// Type definition for the converter's base
typedef T_AthenaPoolCustomCnv< xAOD::CMXJetHitsContainer , 
                               xAOD::CMXJetHitsContainer >
        xAODCMXJetHitsContainerCnvBase;

/**
  * @short POOL converter for the xAOD::CMXJetHitsContainer class
  *
  *        Simple converter class making the xAOD::CMXJetHitsContainer class known to POOL
  *
  * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
  * @author John Morris <john.morris@cern.ch>
  *
  * $Revision
  * $Date 
  *
  */

class xAODCMXJetHitsContainerCnv : public xAODCMXJetHitsContainerCnvBase {

  // Declare the factory as our friend:
  friend class CnvFactory< xAODCMXJetHitsContainerCnv >;

  public:
    /// Converter constructor
    xAODCMXJetHitsContainerCnv( ISvcLocator* svcLoc );

    /// Function preparing the container to be written out
    virtual xAOD::CMXJetHitsContainer* createPersistent( xAOD::CMXJetHitsContainer* trans );
    /// Function reading in the persistent object
    virtual xAOD::CMXJetHitsContainer* createTransient();

  private:
    /// Function preparing a vertex object for persistence
    void toPersistent( xAOD::CMXJetHits* ) const;

}; // class xAODCMXJetHitsContainerCnv
#endif

