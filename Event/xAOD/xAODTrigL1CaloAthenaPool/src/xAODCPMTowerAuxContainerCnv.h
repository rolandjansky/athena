/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALOATHENAPOOL_XAODCPMTOWERAUXCONTAINERCNV_H
#define XAODTRIGL1CALOATHENAPOOL_XAODCPMTOWERAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTrigL1Calo/CPMTowerAuxContainer.h"

/// Base class for the converter
typedef T_AthenaPoolCustomCnv< xAOD::CPMTowerAuxContainer,
                               xAOD::CPMTowerAuxContainer >
   xAODCPMTowerAuxContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::CPMTowerAuxContainer class
 *
 *         This is the converter doing the actual schema evolution
 *         of the package... The converter for xAOD::CPMTowerContainer
 *         doesn't do much, as the "interface classes" don't contain
 *         too much/any payload. Most of the payload is in the auxiliary
 *         containers like this one.
 *
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 * @author John Morris <john.morris@cern.ch>
 *
 * $Revision: 576323 $
 * $Date: 2013-12-19 17:07:14 +0100 (Thu, 19 Dec 2013) $
 */
class xAODCPMTowerAuxContainerCnv :
   public xAODCPMTowerAuxContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODCPMTowerAuxContainerCnv >;

protected:
   /// Converter constructor
   xAODCPMTowerAuxContainerCnv( ISvcLocator* svcLoc );

   /// Function preparing the container to be written out
   virtual xAOD::CPMTowerAuxContainer*
   createPersistent( xAOD::CPMTowerAuxContainer* trans );
   /// Function reading in the object from the input file
   virtual xAOD::CPMTowerAuxContainer* createTransient();

}; // class xAODCPMTowerAuxContainerCnv

#endif // XAODTRIGL1CALOATHENAPOOL_XAODCPMTOWERAUXCONTAINERCNV_H
