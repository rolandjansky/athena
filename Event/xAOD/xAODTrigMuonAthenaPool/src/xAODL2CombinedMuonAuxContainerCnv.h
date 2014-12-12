/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// xAODL2CombinedMuonAuxContainerCnv.h giagu
#ifndef XAODTRIGMUONATHENAPOOL_XAODL2COMBINEDMUONAUXCONTAINERCNV_H
#define XAODTRIGMUONATHENAPOOL_XAODL2COMBINEDMUONAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTrigMuon/L2CombinedMuonAuxContainer.h"

/// Base class for the converter
typedef T_AthenaPoolCustomCnv< xAOD::L2CombinedMuonAuxContainer,
                               xAOD::L2CombinedMuonAuxContainer >
   xAODL2CombinedMuonAuxContainerCnvBase;

/**
 * V1.0 2014-06-30
 */
class xAODL2CombinedMuonAuxContainerCnv :
   public xAODL2CombinedMuonAuxContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODL2CombinedMuonAuxContainerCnv >;

protected:
   /// Converter constructor
   xAODL2CombinedMuonAuxContainerCnv( ISvcLocator* svcLoc );

   /// Function preparing the container to be written out
   virtual xAOD::L2CombinedMuonAuxContainer*
   createPersistent( xAOD::L2CombinedMuonAuxContainer* trans );
   /// Function reading in the object from the input file
   virtual xAOD::L2CombinedMuonAuxContainer* createTransient();

}; // class xAODL2CombinedMuonAuxContainerCnv

#endif // XAODTRIGMUONATHENAPOOL_XAODL2COMBINEDMUONAUXCONTAINERCNV_H
