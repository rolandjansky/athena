/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @file AFPSiHitContainerCnv.h
 * @brief Generated header file which defines a typedef for templated converter class
 * @author RD Schaffer <R.D. Schaffer@cern.ch>
 */

#ifndef AFPSiHitContainerCnv_H
#define AFPSiHitContainerCnv_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "xAODForward/AFPSiHitContainer.h"
#include "xAODForward/AFPSiHit.h"


typedef T_AthenaPoolCustomCnv<xAOD::AFPSiHitContainer, xAOD::AFPSiHitContainer> xAODAFPSiHitContainerCnvBase;

class xAODAFPSiHitContainerCnv : public xAODAFPSiHitContainerCnvBase {
 
  // Declare the factory as our friend:
  friend class CnvFactory< xAODAFPSiHitContainerCnv >;
 
public:
  /// Converter constructor
  xAODAFPSiHitContainerCnv( ISvcLocator* svcLoc );
 
  /// Re-implemented function in order to get access to the SG key 
  virtual StatusCode createObj( IOpaqueAddress* pAddr, DataObject*& pObj );
 
  /// Function preparing the container to be written out
  virtual xAOD::AFPSiHitContainer* createPersistent( xAOD::AFPSiHitContainer* trans );
  
  /// Function reading in the persistent object
  virtual xAOD::AFPSiHitContainer* createTransient();
 
private:
  /// Function preparing a vertex object for persistence
  void toPersistent( xAOD::AFPSiHit* hit ) const;
 
  /// StoreGate key of the container just being created
  std::string m_key;
 
};


#endif

