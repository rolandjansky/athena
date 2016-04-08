/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODCUTBOOKKEEPERAUXCONTAINERCNV_H
#define XAODCUTBOOKKEEPERAUXCONTAINERCNV_H

#include "xAODCutFlow/CutBookkeeperAuxContainer.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"


typedef T_AthenaPoolCustomCnv<xAOD::CutBookkeeperAuxContainer,
                              xAOD::CutBookkeeperAuxContainer>
          xAODCutBookkeeperAuxContainerCnvBase;


class xAODCutBookkeeperAuxContainerCnv : public xAODCutBookkeeperAuxContainerCnvBase {

  /** Other example classes do this */
  friend class CnvFactory < xAODCutBookkeeperAuxContainerCnv >;

 protected:

  /** The constructor */
  xAODCutBookkeeperAuxContainerCnv( ISvcLocator* svcLoc );

  /** Create persistent container */
  xAOD::CutBookkeeperAuxContainer* createPersistent( xAOD::CutBookkeeperAuxContainer* trans );

  /** Create transient object, when reading from disk */
  xAOD::CutBookkeeperAuxContainer* createTransient();

};
#endif //XAODCUTBOOKKEEPERAUXCONTAINERCNV_H
