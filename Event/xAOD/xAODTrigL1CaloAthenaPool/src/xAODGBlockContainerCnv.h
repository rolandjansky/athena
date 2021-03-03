/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALOATHENAPOOL_XAODGBLOCKCONTAINERCNV_H
#define XAODTRIGL1CALOATHENAPOOL_XAODGBLOCKCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTrigL1Calo/GBlockContainer.h"
#include "xAODTrigL1Calo/GBlock.h"

/// Type definition for the converter's base
typedef T_AthenaPoolCustomCnv<xAOD::GBlockContainer,
                              xAOD::GBlockContainer>
    xAODGBlockContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::GBlockContainer class
 *
 *         Simple converter class making the xAOD::GBlockContainer
 *         class known to POOL.
 *
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 * @author John Morris <john.morris@cern.ch>
 *
 */
class xAODGBlockContainerCnv : public xAODGBlockContainerCnvBase
{

  // Declare the factory as our friend:
  friend class CnvFactory<xAODGBlockContainerCnv>;

public:
  /// Converter constructor
  xAODGBlockContainerCnv(ISvcLocator *svcLoc);

  /// Re-implemented function in order to get access to the SG key
  virtual StatusCode createObj(IOpaqueAddress *pAddr, DataObject *&pObj);
  /// Function preparing the container to be written out
  virtual xAOD::GBlockContainer *createPersistent(xAOD::GBlockContainer *trans);
  /// Function reading in the persistent object
  virtual xAOD::GBlockContainer *createTransient();

private:
  /// Function preparing a vertex object for persistence
  void toPersistent(xAOD::GBlock *cluster) const;

  /// StoreGate key of the container just being created
  std::string m_key;

}; // class xAODGBlockContainerCnv

#endif
