// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id
#ifndef XAODTRIGL1CALOATHENAPOOL_XAODCMXJETTOBCONTAINERCNV_H
#define XAODTRIGL1CALOATHENAPOOL_XAODCMXJETTOBCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTrigL1Calo/CMXJetTobContainer.h"

/// Type definition for the converter's base
typedef T_AthenaPoolCustomCnv< xAOD::CMXJetTobContainer , 
                               xAOD::CMXJetTobContainer >
        xAODCMXJetTobContainerCnvBase;

/**
  * @short POOL converter for the xAOD::CMXJetTobContainer class
  *
  *        Simple converter class making the xAOD::CMXJetTobContainer class known to POOL
  *
  * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
  * @author John Morris <john.morris@cern.ch>
  *
  * $Revision
  * $Date 
  *
  */

class xAODCMXJetTobContainerCnv : public xAODCMXJetTobContainerCnvBase {

  // Declare the factory as our friend:
  friend class CnvFactory< xAODCMXJetTobContainerCnv >;

  public:
    /// Converter constructor
    xAODCMXJetTobContainerCnv( ISvcLocator* svcLoc );

    /// Re-implemented function in order to get access to the SG key
    virtual StatusCode createObj( IOpaqueAddress* pAddr, DataObject*& pObj );     
    /// Function preparing the container to be written out
    virtual xAOD::CMXJetTobContainer* createPersistent( xAOD::CMXJetTobContainer* trans );
    /// Function reading in the persistent object
    virtual xAOD::CMXJetTobContainer* createTransient();

  private:
    /// Function preparing a vertex object for persistence
    void toPersistent( xAOD::CMXJetTob* ) const;
   
   /// StoreGate key of the container just being created
   std::string m_key;

}; // class xAODCMXJetTobContainerCnv
#endif

