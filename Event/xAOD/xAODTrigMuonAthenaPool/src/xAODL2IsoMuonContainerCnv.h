/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// xAODL2IsoMuonContainerCnv.h giagu
#ifndef XAODTRIGMUONATHENAPOOL_XAODL2ISOMUONCONTAINERCNV_H
#define XAODTRIGMUONATHENAPOOL_XAODL2ISOMUONCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTrigMuon/L2IsoMuonContainer.h"

/// Type definition for the converter's base
typedef T_AthenaPoolCustomCnv< xAOD::L2IsoMuonContainer,
                               xAOD::L2IsoMuonContainer >
   xAODL2IsoMuonContainerCnvBase;

/**
 * 
 * V1.0 2014-06-30
 */
class xAODL2IsoMuonContainerCnv :
   public xAODL2IsoMuonContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODL2IsoMuonContainerCnv >;

public:
   /// Converter constructor
   xAODL2IsoMuonContainerCnv( ISvcLocator* svcLoc );

   /// Re-implemented function in order to get access to the SG key
   virtual StatusCode createObj( IOpaqueAddress* pAddr, DataObject*& pObj );

   /// Function preparing the container to be written out
   virtual xAOD::L2IsoMuonContainer*
   createPersistent( xAOD::L2IsoMuonContainer* trans );
   /// Function reading in the persistent object
   virtual xAOD::L2IsoMuonContainer* createTransient();

private:
   /// StoreGate key of the container just being created
   std::string m_key;

}; // class xAODL2IsoMuonContainerCnv

#endif // XAODTRIGMUONATHENAPOOL_XAODL2ISOMUONCONTAINERCNV_H
