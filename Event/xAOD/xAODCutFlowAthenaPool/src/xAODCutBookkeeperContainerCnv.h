/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODCUTBOOKKEEPERCONTAINERCNV_H
#define XAODCUTBOOKKEEPERCONTAINERCNV_H

#include "xAODCutFlow/CutBookkeeper.h"
#include "xAODCutFlow/CutBookkeeperContainer.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"



typedef T_AthenaPoolCustomCnv<xAOD::CutBookkeeperContainer,
                              xAOD::CutBookkeeperContainer>
          xAODCutBookkeeperContainerCnvBase;

class xAODCutBookkeeperContainerCnv : public xAODCutBookkeeperContainerCnvBase {

  /** Other example classes do this */
  friend class CnvFactory < xAODCutBookkeeperContainerCnv >;

 public:

  /** The constructor */
  xAODCutBookkeeperContainerCnv( ISvcLocator* svcLoc );

  StatusCode createObj( IOpaqueAddress* pAddr, DataObject*& pObj );

  /** Create persistent container */
  xAOD::CutBookkeeperContainer* createPersistent( xAOD::CutBookkeeperContainer* trans );

  /** Create transient object, when reading from disk */
  xAOD::CutBookkeeperContainer* createTransient();

 private:

  /** Prepates object for persistence */
  void toPersistent(xAOD::CutBookkeeper* theCutBookkeeper) const;

  /** Storegate key name */
  std::string m_keyName;

};
#endif // XAODCUTBOOKKEEPERCONTAINERCNV_H
