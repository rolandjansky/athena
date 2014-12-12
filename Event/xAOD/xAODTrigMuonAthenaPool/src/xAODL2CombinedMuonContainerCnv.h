/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// xAODL2CombinedMuonContainerCnv.h giagu
#ifndef XAODTRIGMUONATHENAPOOL_XAODL2COMBINEDMUONCONTAINERCNV_H
#define XAODTRIGMUONATHENAPOOL_XAODL2COMBINEDMUONCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTrigMuon/L2CombinedMuonContainer.h"

/// Type definition for the converter's base
typedef T_AthenaPoolCustomCnv< xAOD::L2CombinedMuonContainer,
                               xAOD::L2CombinedMuonContainer >
   xAODL2CombinedMuonContainerCnvBase;

/**
 * 
 * V1.0 2014-06-30
 */
class xAODL2CombinedMuonContainerCnv :
   public xAODL2CombinedMuonContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODL2CombinedMuonContainerCnv >;

public:
   /// Converter constructor
   xAODL2CombinedMuonContainerCnv( ISvcLocator* svcLoc );

   /// Re-implemented function in order to get access to the SG key
   virtual StatusCode createObj( IOpaqueAddress* pAddr, DataObject*& pObj );

   /// Function preparing the container to be written out
   virtual xAOD::L2CombinedMuonContainer*
   createPersistent( xAOD::L2CombinedMuonContainer* trans );
   /// Function reading in the persistent object
   virtual xAOD::L2CombinedMuonContainer* createTransient();

private:
   /// StoreGate key of the container just being created
   std::string m_key;

}; // class xAODL2CombinedMuonContainerCnv

#endif // XAODTRIGMUONATHENAPOOL_XAODL2COMBINEDMUONCONTAINERCNV_H
