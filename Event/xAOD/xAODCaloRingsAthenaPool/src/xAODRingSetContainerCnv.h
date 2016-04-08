/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODRingSetContainerCnv.h 659544 2015-04-08 17:39:38Z wsfreund $
#ifndef XAODCALORINGSATHENAPOOL_XAODRINGSETCONTAINERCNV_H
#define XAODCALORINGSATHENAPOOL_XAODRINGSETCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODCaloRings/RingSetContainer.h"

// System include(s): 
#include <string> 

/// Type definition for the converter's base
typedef T_AthenaPoolCustomCnv< xAOD::RingSetContainer,
                               xAOD::RingSetContainer >
   xAODRingSetContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::RingSetContainer class
 *
 *         Simple converter class making the xAOD::RingSetContainer
 *         class known to POOL.
 *
 * @author Werner Freund <wsfreund@cern.ch>
 *
 * based on xAODEgammaAthenaPool
 *
 **/
class xAODRingSetContainerCnv : public xAODRingSetContainerCnvBase {
  
  // Declare the factory as our friend:
  friend class CnvFactory< xAODRingSetContainerCnv >;

public:
  /// Converter constructor
  xAODRingSetContainerCnv( ISvcLocator* svcLoc );

  /// Re-implemented function in order to get access to the SG key 
  virtual StatusCode createObj( IOpaqueAddress* pAddr, DataObject*& pObj ); 
  /// Function preparing the container to be written out
  virtual xAOD::RingSetContainer* createPersistent( xAOD::RingSetContainer* trans );
  /// Function reading in the persistent object
  virtual xAOD::RingSetContainer* createTransient();
  
private:

  /// StoreGate key of the container just being created 
  std::string m_key; 
  
}; // class xAODRingSetContainerCnv

#endif // XAODCALORINGSATHENAPOOL_XAODRINGSETCONTAINERCNV_H
