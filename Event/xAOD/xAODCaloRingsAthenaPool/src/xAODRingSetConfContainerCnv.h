/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODRingSetConfContainerCnv.h 659544 2015-04-08 17:39:38Z wsfreund $
#ifndef XAODCALORINGSATHENAPOOL_XAODRINGSETCONFCONTAINERCNV_H
#define XAODCALORINGSATHENAPOOL_XAODRINGSETCONFCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODCaloRings/RingSetConfContainer.h"

// System include(s): 
#include <string> 

/// Type definition for the converter's base
typedef T_AthenaPoolCustomCnv< xAOD::RingSetConfContainer,
                               xAOD::RingSetConfContainer >
   xAODRingSetConfContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::RingSetConfContainer class
 *
 *         Simple converter class making the xAOD::RingSetConfContainer
 *         class known to POOL.
 *
 * @author Werner Freund <wsfreund@cern.ch>
 *
 * based on xAODEgammaAthenaPool
 *
 **/
class xAODRingSetConfContainerCnv : public xAODRingSetConfContainerCnvBase {
  
  // Declare the factory as our friend:
  friend class CnvFactory< xAODRingSetConfContainerCnv >;

public:
  /// Converter constructor
  xAODRingSetConfContainerCnv( ISvcLocator* svcLoc );

  /// Re-implemented function in order to get access to the SG key 
  virtual StatusCode createObj( IOpaqueAddress* pAddr, DataObject*& pObj ); 
  /// Function preparing the container to be written out
  virtual xAOD::RingSetConfContainer* createPersistent( xAOD::RingSetConfContainer* trans );
  /// Function reading in the persistent object
  virtual xAOD::RingSetConfContainer* createTransient();
  
private:

  /// StoreGate key of the container just being created 
  std::string m_key; 
  
}; // class xAODRingSetConfContainerCnv

#endif // XAODCALORINGSATHENAPOOL_XAODRINGSETCONFCONTAINERCNV_H
