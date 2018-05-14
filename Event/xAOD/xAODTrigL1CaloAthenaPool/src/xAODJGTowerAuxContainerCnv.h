/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef XAODTRIGL1CALOATHENAPOOL_XAODJGTOWERAUXCONTAINERCNV_H
#define XAODTRIGL1CALOATHENAPOOL_XAODJGTOWERAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTrigL1Calo/JGTowerAuxContainer.h"

/// Base class for the converter
typedef T_AthenaPoolCustomCnv< xAOD::JGTowerAuxContainer,
                               xAOD::JGTowerAuxContainer >
   xAODJGTowerAuxContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::JGTowerAuxContainer class
 *
 *         This is the converter doing the actual schema evolution
 *         of the package... The converter for xAOD::JGTowerContainer
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
class xAODJGTowerAuxContainerCnv :
   public xAODJGTowerAuxContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODJGTowerAuxContainerCnv >;

protected:
   /// Converter constructor
   xAODJGTowerAuxContainerCnv( ISvcLocator* svcLoc );

   /// Function preparing the container to be written out
   virtual xAOD::JGTowerAuxContainer*
   createPersistent( xAOD::JGTowerAuxContainer* trans );
   /// Function reading in the object from the input file
   virtual xAOD::JGTowerAuxContainer* createTransient();

}; // class xAODJGTowerAuxContainerCnv

#endif // XAODTRIGL1CALOATHENAPOOL_XAODJGTOWERAUXCONTAINERCNV_H
