/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODCOREATHENAPOOL_XAODSHALLOWAUXCONTAINERCNV_H
#define XAODCOREATHENAPOOL_XAODSHALLOWAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#define private public
#include "xAODCore/ShallowAuxContainer.h"

/// Base class for the converter
typedef T_AthenaPoolCustomCnv< xAOD::ShallowAuxContainer,
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

   // Declare the factory as our friend:
   friend class CnvFactory< xAODShallowAuxContainerCnv >;

protected:
   /// Converter constructor
   xAODShallowAuxContainerCnv( ISvcLocator* svcLoc );

   /// Function preparing the container to be written out
   virtual xAOD::ShallowAuxContainer* createPersistent( xAOD::ShallowAuxContainer* trans );
   /// Function reading in the object from the input file
   virtual xAOD::ShallowAuxContainer* createTransient();


}; // class xAODShallowAuxContainerCnv

#endif // XAODCOREATHENAPOOL_XAODSHALLOWAUXCONTAINERCNV_H

