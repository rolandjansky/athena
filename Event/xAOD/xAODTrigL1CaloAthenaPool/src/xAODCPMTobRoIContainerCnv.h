// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id
#ifndef XAODTRIGL1CALOATHENAPOOL_XAODCPMTOBROICONTAINERCNV_H
#define XAODTRIGL1CALOATHENAPOOL_XAODCPMTOBROICONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTrigL1Calo/CPMTobRoIContainer.h"

/// Type definition for the converter's base
typedef T_AthenaPoolCustomCnv< xAOD::CPMTobRoIContainer , 
                               xAOD::CPMTobRoIContainer >
        xAODCPMTobRoIContainerCnvBase;

/**
  * @short POOL converter for the xAOD::CPMTobRoIContainer class
  *
  *        Simple converter class making the xAOD::CPMTobRoIContainer class known to POOL
  *
  * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
  * @author John Morris <john.morris@cern.ch>
  *
  * $Revision
  * $Date 
  *
  */

class xAODCPMTobRoIContainerCnv : public xAODCPMTobRoIContainerCnvBase {

  // Declare the factory as our friend:
  friend class CnvFactory< xAODCPMTobRoIContainerCnv >;

  public:
    /// Converter constructor
    xAODCPMTobRoIContainerCnv( ISvcLocator* svcLoc );

    /// Re-implemented function in order to get access to the SG key
    virtual StatusCode createObj( IOpaqueAddress* pAddr, DataObject*& pObj );     
    /// Function preparing the container to be written out
    virtual xAOD::CPMTobRoIContainer* createPersistent( xAOD::CPMTobRoIContainer* trans );
    /// Function reading in the persistent object
    virtual xAOD::CPMTobRoIContainer* createTransient();

  private:
    /// Function preparing a vertex object for persistence
    void toPersistent( xAOD::CPMTobRoI* ) const;
   
   /// StoreGate key of the container just being created
   std::string m_key;

}; // class xAODCPMTobRoIContainerCnv
#endif

