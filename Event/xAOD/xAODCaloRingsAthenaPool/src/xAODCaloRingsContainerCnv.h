/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODCaloRingsContainerCnv.h 659544 2015-04-08 17:39:38Z wsfreund $
#ifndef XAODCALORINGSATHENAPOOL_XAODCALORINGSCONTAINERCNV_H
#define XAODCALORINGSATHENAPOOL_XAODCALORINGSCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODCaloRings/CaloRingsContainer.h"

// System include(s): 
#include <string> 

/// Type definition for the converter's base
typedef T_AthenaPoolCustomCnv< xAOD::CaloRingsContainer,
                               xAOD::CaloRingsContainer >
   xAODCaloRingsContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::CaloRingsContainer class
 *
 *         Simple converter class making the xAOD::CaloRingsContainer
 *         class known to POOL.
 *
 * @author Werner Freund <wsfreund@cern.ch>
 *
 * based on xAODEgammaAthenaPool
 *
 **/
class xAODCaloRingsContainerCnv : public xAODCaloRingsContainerCnvBase {
  
  // Declare the factory as our friend:
  friend class CnvFactory< xAODCaloRingsContainerCnv >;

public:
  /// Converter constructor
  xAODCaloRingsContainerCnv( ISvcLocator* svcLoc );

  /// Re-implemented function in order to get access to the SG key 
  virtual StatusCode createObj( IOpaqueAddress* pAddr, DataObject*& pObj ); 
  /// Function preparing the container to be written out
  virtual xAOD::CaloRingsContainer* createPersistent( xAOD::CaloRingsContainer* trans );
  /// Function reading in the persistent object
  virtual xAOD::CaloRingsContainer* createTransient();
  
private:

  /// StoreGate key of the container just being created 
  std::string m_key; 
  
}; // class xAODCaloRingsContainerCnv

#endif // XAODCALORINGSATHENAPOOL_XAODCALORINGSCONTAINERCNV_H
