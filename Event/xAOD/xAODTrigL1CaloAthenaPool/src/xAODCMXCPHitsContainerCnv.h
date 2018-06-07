// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id
#ifndef XAODTRIGL1CALOATHENAPOOL_XAODCMXCPHITSCONTAINERCNV_H
#define XAODTRIGL1CALOATHENAPOOL_XAODCMXCPHITSCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTrigL1Calo/CMXCPHitsContainer.h"

/// Type definition for the converter's base
typedef T_AthenaPoolCustomCnv< xAOD::CMXCPHitsContainer , 
                               xAOD::CMXCPHitsContainer >
        xAODCMXCPHitsContainerCnvBase;

/**
  * @short POOL converter for the xAOD::CMXCPHitsContainer class
  *
  *        Simple converter class making the xAOD::CMXCPHitsContainer class known to POOL
  *
  * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
  * @author John Morris <john.morris@cern.ch>
  *
  * $Revision
  * $Date 
  *
  */

class xAODCMXCPHitsContainerCnv : public xAODCMXCPHitsContainerCnvBase {

  // Declare the factory as our friend:
  friend class CnvFactory< xAODCMXCPHitsContainerCnv >;

  public:
    /// Converter constructor
    xAODCMXCPHitsContainerCnv( ISvcLocator* svcLoc );

    /// Re-implemented function in order to get access to the SG key
    virtual StatusCode createObj( IOpaqueAddress* pAddr, DataObject*& pObj );     
    /// Function preparing the container to be written out
    virtual xAOD::CMXCPHitsContainer* createPersistent( xAOD::CMXCPHitsContainer* trans );
    /// Function reading in the persistent object
    virtual xAOD::CMXCPHitsContainer* createTransient();

  private:
    /// Function preparing a vertex object for persistence
    void toPersistent( xAOD::CMXCPHits* ) const;
   
   /// StoreGate key of the container just being created
   std::string m_key;

}; // class xAODCMXCPHitsContainerCnv
#endif

