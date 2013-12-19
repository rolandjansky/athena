/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALOATHENAPOOL_XAODCMMJETHITSAUXCONTAINERCNV_H
#define XAODTRIGL1CALOATHENAPOOL_XAODCMMJETHITSAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTrigL1Calo/CMMJetHitsAuxContainer.h"

/// Base class for the converter
typedef T_AthenaPoolCustomCnv< xAOD::CMMJetHitsAuxContainer,
                               xAOD::CMMJetHitsAuxContainer >
   xAODCMMJetHitsAuxContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::CMMJetHitsAuxContainer class
 *
 *         This is the converter doing the actual schema evolution
 *         of the package... The converter for xAOD::CMMJetHitsContainer
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
class xAODCMMJetHitsAuxContainerCnv :
   public xAODCMMJetHitsAuxContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODCMMJetHitsAuxContainerCnv >;

protected:
   /// Converter constructor
   xAODCMMJetHitsAuxContainerCnv( ISvcLocator* svcLoc );

   /// Function preparing the container to be written out
   virtual xAOD::CMMJetHitsAuxContainer*
   createPersistent( xAOD::CMMJetHitsAuxContainer* trans );
   /// Function reading in the object from the input file
   virtual xAOD::CMMJetHitsAuxContainer* createTransient();

}; // class xAODCMMJetHitsAuxContainerCnv

#endif // XAODTRIGL1CALOATHENAPOOL_XAODCMMJETHITSAUXCONTAINERCNV_H
