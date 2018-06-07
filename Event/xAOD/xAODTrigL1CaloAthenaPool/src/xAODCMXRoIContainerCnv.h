// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id
#ifndef XAODTRIGL1CALOATHENAPOOL_XAODCMXROICONTAINERCNV_H
#define XAODTRIGL1CALOATHENAPOOL_XAODCMXROICONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTrigL1Calo/CMXRoIContainer.h"

/// Type definition for the converter's base
typedef T_AthenaPoolCustomCnv< xAOD::CMXRoIContainer , 
                               xAOD::CMXRoIContainer >
        xAODCMXRoIContainerCnvBase;

/**
  * @short POOL converter for the xAOD::CMXRoIContainer class
  *
  *        Simple converter class making the xAOD::CMXRoIContainer class known to POOL
  *
  * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
  * @author John Morris <john.morris@cern.ch>
  *
  * $Revision
  * $Date 
  *
  */

class xAODCMXRoIContainerCnv : public xAODCMXRoIContainerCnvBase {

  // Declare the factory as our friend:
  friend class CnvFactory< xAODCMXRoIContainerCnv >;

  public:
    /// Converter constructor
    xAODCMXRoIContainerCnv( ISvcLocator* svcLoc );

    /// Re-implemented function in order to get access to the SG key
    virtual StatusCode createObj( IOpaqueAddress* pAddr, DataObject*& pObj );     
    /// Function preparing the container to be written out
    virtual xAOD::CMXRoIContainer* createPersistent( xAOD::CMXRoIContainer* trans );
    /// Function reading in the persistent object
    virtual xAOD::CMXRoIContainer* createTransient();

  private:
    /// Function preparing a vertex object for persistence
    void toPersistent( xAOD::CMXRoI* ) const;
   
   /// StoreGate key of the container just being created
   std::string m_key;

}; // class xAODCMXRoIContainerCnv
#endif

