/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALOATHENAPOOL_XAODTRIGGERTOWERAUXCONTAINERCNV_H
#define XAODTRIGL1CALOATHENAPOOL_XAODTRIGGERTOWERAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTrigL1Calo/TriggerTowerAuxContainer.h"

/// Base class for the converter
typedef T_AthenaPoolCustomCnv< xAOD::TriggerTowerAuxContainer,
                               xAOD::TriggerTowerAuxContainer >
   xAODTriggerTowerAuxContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::TriggerTowerAuxContainer class
 *
 *         This is the converter doing the actual schema evolution
 *         of the package... The converter for xAOD::TriggerTowerContainer
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
class xAODTriggerTowerAuxContainerCnv :
   public xAODTriggerTowerAuxContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODTriggerTowerAuxContainerCnv >;

protected:
   /// Converter constructor
   xAODTriggerTowerAuxContainerCnv( ISvcLocator* svcLoc );

   /// Function preparing the container to be written out
   virtual xAOD::TriggerTowerAuxContainer*
   createPersistent( xAOD::TriggerTowerAuxContainer* trans );
   /// Function reading in the object from the input file
   virtual xAOD::TriggerTowerAuxContainer* createTransient();

}; // class xAODTriggerTowerAuxContainerCnv

#endif // XAODTRIGL1CALOATHENAPOOL_XAODTRIGGERTOWERAUXCONTAINERCNV_H
