/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFPToFHITCONTAINERCNV_H
#define AFPToFHITCONTAINERCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "xAODForward/AFPTrackContainer.h"
#include "xAODForward/AFPToFHitContainer.h"
#include "xAODForward/AFPToFHit.h"


typedef T_AthenaPoolCustomCnv<xAOD::AFPToFHitContainer, xAOD::AFPToFHitContainer> xAODAFPToFHitContainerCnvBase;

class xAODAFPToFHitContainerCnv : public xAODAFPToFHitContainerCnvBase {
 
  // Declare the factory as our friend:
  friend class CnvFactory< xAODAFPToFHitContainerCnv >;
 
public:
  /// Converter constructor
  xAODAFPToFHitContainerCnv( ISvcLocator* svcLoc );
 
  /// Re-implemented function in order to get access to the SG key 
  virtual StatusCode createObj( IOpaqueAddress* pAddr, DataObject*& pObj );
 
  /// Function preparing the container to be written out
  virtual xAOD::AFPToFHitContainer* createPersistent( xAOD::AFPToFHitContainer* trans );
  
  /// Function reading in the persistent object
  virtual xAOD::AFPToFHitContainer* createTransient();
 
private:
  /// Function preparing a vertex object for persistence
  void toPersistent( xAOD::AFPToFHit* hit ) const;
 
  /// StoreGate key of the container just being created
  std::string m_key;
 
};


#endif

