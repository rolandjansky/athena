/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALOATHENAPOOL_XAODJEMHITSAUXCONTAINERCNV_H
#define XAODTRIGL1CALOATHENAPOOL_XAODJEMHITSAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTrigL1Calo/JEMHitsAuxContainer.h"

/// Base class for the converter
typedef T_AthenaPoolCustomCnv< xAOD::JEMHitsAuxContainer,
                               xAOD::JEMHitsAuxContainer >
   xAODJEMHitsAuxContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::JEMHitsAuxContainer class
 *
 *         This is the converter doing the actual schema evolution
 *         of the package... The converter for xAOD::JEMHitsContainer
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
class xAODJEMHitsAuxContainerCnv :
   public xAODJEMHitsAuxContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODJEMHitsAuxContainerCnv >;

protected:
   /// Converter constructor
   xAODJEMHitsAuxContainerCnv( ISvcLocator* svcLoc );

   /// Function preparing the container to be written out
   virtual xAOD::JEMHitsAuxContainer*
   createPersistent( xAOD::JEMHitsAuxContainer* trans );
   /// Function reading in the object from the input file
   virtual xAOD::JEMHitsAuxContainer* createTransient();

}; // class xAODJEMHitsAuxContainerCnv

#endif // XAODTRIGL1CALOATHENAPOOL_XAODJEMHITSAUXCONTAINERCNV_H
