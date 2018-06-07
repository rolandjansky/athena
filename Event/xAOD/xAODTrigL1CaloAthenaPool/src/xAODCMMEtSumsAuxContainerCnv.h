/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALOATHENAPOOL_XAODCMMETSUMSAUXCONTAINERCNV_H
#define XAODTRIGL1CALOATHENAPOOL_XAODCMMETSUMSAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTrigL1Calo/CMMEtSumsAuxContainer.h"

/// Base class for the converter
typedef T_AthenaPoolCustomCnv< xAOD::CMMEtSumsAuxContainer,
                               xAOD::CMMEtSumsAuxContainer >
   xAODCMMEtSumsAuxContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::CMMEtSumsAuxContainer class
 *
 *         This is the converter doing the actual schema evolution
 *         of the package... The converter for xAOD::CMMEtSumsContainer
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
class xAODCMMEtSumsAuxContainerCnv :
   public xAODCMMEtSumsAuxContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODCMMEtSumsAuxContainerCnv >;

protected:
   /// Converter constructor
   xAODCMMEtSumsAuxContainerCnv( ISvcLocator* svcLoc );

   /// Function preparing the container to be written out
   virtual xAOD::CMMEtSumsAuxContainer*
   createPersistent( xAOD::CMMEtSumsAuxContainer* trans );
   /// Function reading in the object from the input file
   virtual xAOD::CMMEtSumsAuxContainer* createTransient();

}; // class xAODCMMEtSumsAuxContainerCnv

#endif // XAODTRIGL1CALOATHENAPOOL_XAODCMMETSUMSAUXCONTAINERCNV_H
