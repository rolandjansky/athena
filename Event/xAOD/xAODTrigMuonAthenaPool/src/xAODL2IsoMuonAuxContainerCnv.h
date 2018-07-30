/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// xAODL2IsoMuonAuxContainerCnv.h giagu
#ifndef XAODTRIGMUONATHENAPOOL_XAODL2ISOMUONAUXCONTAINERCNV_H
#define XAODTRIGMUONATHENAPOOL_XAODL2ISOMUONAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTrigMuon/L2IsoMuonAuxContainer.h"

/// Base class for the converter
typedef T_AthenaPoolCustomCnv< xAOD::L2IsoMuonAuxContainer,
                               xAOD::L2IsoMuonAuxContainer >
   xAODL2IsoMuonAuxContainerCnvBase;

/**
 * V1.0 2014-06-30
 */
class xAODL2IsoMuonAuxContainerCnv :
   public xAODL2IsoMuonAuxContainerCnvBase {

public:
   /// Converter constructor
   xAODL2IsoMuonAuxContainerCnv( ISvcLocator* svcLoc );

protected:
   /// Function preparing the container to be written out
   virtual xAOD::L2IsoMuonAuxContainer*
   createPersistent( xAOD::L2IsoMuonAuxContainer* trans );
   /// Function reading in the object from the input file
   virtual xAOD::L2IsoMuonAuxContainer* createTransient();

}; // class xAODL2IsoMuonAuxContainerCnv

#endif // XAODTRIGMUONATHENAPOOL_XAODL2ISOMUONAUXCONTAINERCNV_H
