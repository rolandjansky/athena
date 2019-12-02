/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODCOREATHENAPOOL_XAODSHALLOWAUXCONTAINERCNV_H
#define XAODCOREATHENAPOOL_XAODSHALLOWAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODCore/ShallowAuxContainer.h"

/// Base class for the converter
typedef T_AthenaPoolCustomCnvWithKey< xAOD::ShallowAuxContainer,
                                      xAOD::ShallowAuxContainer >
   xAODShallowAuxContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::ShallowAuxContainer class
 *
 *         This is the converter doing the actual schema evolution
 *         of the package... 
 *
 * @author Will Buttinger <will@cern.ch>
 *
 */
class xAODShallowAuxContainerCnv :
   public xAODShallowAuxContainerCnvBase {

public:
   /// Converter constructor
   xAODShallowAuxContainerCnv( ISvcLocator* svcLoc );

protected:
   /// Function preparing the container to be written out
   virtual
   xAOD::ShallowAuxContainer* createPersistentWithKey( xAOD::ShallowAuxContainer* trans,
                                                       const std::string& key) override;
   /// Function reading in the object from the input file
   virtual
   xAOD::ShallowAuxContainer* createTransientWithKey (const std::string& key) override;


}; // class xAODShallowAuxContainerCnv

#endif // XAODCOREATHENAPOOL_XAODSHALLOWAUXCONTAINERCNV_H

