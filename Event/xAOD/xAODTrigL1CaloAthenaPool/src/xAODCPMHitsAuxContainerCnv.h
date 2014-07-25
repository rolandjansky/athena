/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALOATHENAPOOL_XAODCPMHITSAUXCONTAINERCNV_H
#define XAODTRIGL1CALOATHENAPOOL_XAODCPMHITSAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTrigL1Calo/CPMHitsAuxContainer.h"

/// Base class for the converter
typedef T_AthenaPoolCustomCnv< xAOD::CPMHitsAuxContainer,
                               xAOD::CPMHitsAuxContainer >
   xAODCPMHitsAuxContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::CPMHitsAuxContainer class
 *
 *         This is the converter doing the actual schema evolution
 *         of the package... The converter for xAOD::CPMHitsContainer
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
class xAODCPMHitsAuxContainerCnv :
   public xAODCPMHitsAuxContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODCPMHitsAuxContainerCnv >;

protected:
   /// Converter constructor
   xAODCPMHitsAuxContainerCnv( ISvcLocator* svcLoc );

   /// Function preparing the container to be written out
   virtual xAOD::CPMHitsAuxContainer*
   createPersistent( xAOD::CPMHitsAuxContainer* trans );
   /// Function reading in the object from the input file
   virtual xAOD::CPMHitsAuxContainer* createTransient();

}; // class xAODCPMHitsAuxContainerCnv

#endif // XAODTRIGL1CALOATHENAPOOL_XAODCPMHITSAUXCONTAINERCNV_H
