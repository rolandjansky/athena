/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALOATHENAPOOL_XAODCPMROIAUXCONTAINERCNV_H
#define XAODTRIGL1CALOATHENAPOOL_XAODCPMROIAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTrigL1Calo/CPMRoIAuxContainer.h"

/// Base class for the converter
typedef T_AthenaPoolCustomCnv< xAOD::CPMRoIAuxContainer,
                               xAOD::CPMRoIAuxContainer >
   xAODCPMRoIAuxContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::CPMRoIAuxContainer class
 *
 *         This is the converter doing the actual schema evolution
 *         of the package... The converter for xAOD::CPMRoIContainer
 *         doesn't do much, as the "interface classes" don't contain
 *         too much/any payload. Most of the payload is in the auxiliary
 *         containers like this one.
 *
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 * @author John Morris <john.morris@cern.ch>
 *
 * $Revision: 576324 $
 * $Date: 2013-12-19 17:07:53 +0100 (Thu, 19 Dec 2013) $
 */
class xAODCPMRoIAuxContainerCnv :
   public xAODCPMRoIAuxContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODCPMRoIAuxContainerCnv >;

protected:
   /// Converter constructor
   xAODCPMRoIAuxContainerCnv( ISvcLocator* svcLoc );

   /// Function preparing the container to be written out
   virtual xAOD::CPMRoIAuxContainer*
   createPersistent( xAOD::CPMRoIAuxContainer* trans );
   /// Function reading in the object from the input file
   virtual xAOD::CPMRoIAuxContainer* createTransient();

}; // class xAODCPMRoIAuxContainerCnv

#endif // XAODTRIGL1CALOATHENAPOOL_XAODCPMROIAUXCONTAINERCNV_H
