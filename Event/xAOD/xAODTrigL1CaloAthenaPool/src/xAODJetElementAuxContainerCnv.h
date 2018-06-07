/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALOATHENAPOOL_XAODJETELEMENTAUXCONTAINERCNV_H
#define XAODTRIGL1CALOATHENAPOOL_XAODJETELEMENTAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTrigL1Calo/JetElementAuxContainer.h"

/// Base class for the converter
typedef T_AthenaPoolCustomCnv< xAOD::JetElementAuxContainer,
                               xAOD::JetElementAuxContainer >
   xAODJetElementAuxContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::JetElementAuxContainer class
 *
 *         This is the converter doing the actual schema evolution
 *         of the package... The converter for xAOD::JetElementContainer
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
class xAODJetElementAuxContainerCnv :
   public xAODJetElementAuxContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODJetElementAuxContainerCnv >;

protected:
   /// Converter constructor
   xAODJetElementAuxContainerCnv( ISvcLocator* svcLoc );

   /// Function preparing the container to be written out
   virtual xAOD::JetElementAuxContainer*
   createPersistent( xAOD::JetElementAuxContainer* trans );
   /// Function reading in the object from the input file
   virtual xAOD::JetElementAuxContainer* createTransient();

}; // class xAODJetElementAuxContainerCnv

#endif // XAODTRIGL1CALOATHENAPOOL_XAODJETELEMENTAUXCONTAINERCNV_H
