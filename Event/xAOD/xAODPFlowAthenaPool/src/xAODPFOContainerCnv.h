/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODPFOCONTAINERCNV_H
#define XAODPFOCONTAINERCNV_H

//based on xAODMuonContainerCnv

#include "xAODPFlow/PFO.h"
#include "xAODPFlow/PFOContainer.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"



typedef T_AthenaPoolCustomCnv<xAOD::PFOContainer, xAOD::PFOContainer> xAODPFOContainerCnvBase;

class xAODPFOContainerCnv : public xAODPFOContainerCnvBase {

  /** Other example classes do this */
  friend class CnvFactory < xAODPFOContainerCnv >;

 public:
  
  /** The constructor */
  xAODPFOContainerCnv( ISvcLocator* svcLoc );
  
  StatusCode createObj( IOpaqueAddress* pAddr, DataObject*& pObj );

  /** Create persistent container */
  xAOD::PFOContainer* createPersistent( xAOD::PFOContainer* trans );

  /** Create transient object, when reading from disk */
  xAOD::PFOContainer* createTransient();
  
 private:

  /** Prepates object for persistence */
  void toPersistent(xAOD::PFO* thePFO) const;

  /** Storegate key name */
  std::string m_keyName;

};
#endif
