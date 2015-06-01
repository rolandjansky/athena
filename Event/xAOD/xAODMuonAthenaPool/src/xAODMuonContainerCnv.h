/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODMUONATHENAPOOL_XAODMUONCONTAINERCNV_H
#define XAODMUONATHENAPOOL_XAODMUONCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODMuon/MuonContainer.h"

/// Type definition for the converter's base
typedef T_AthenaPoolCustomCnv< xAOD::MuonContainer,
                               xAOD::MuonContainer >
   xAODMuonContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::MuonContainer class
 *
 *         Simple converter class making the xAOD::MuonContainer
 *         class known to POOL.
 *
 * @author Edward Moyse <Edward.Moyse@cern.ch>
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 *
 */
class xAODMuonContainerCnv : public xAODMuonContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODMuonContainerCnv >;

public:
   /// Converter constructor
   xAODMuonContainerCnv( ISvcLocator* svcLoc );

   /// Re-implemented function in order to get access to the SG key
   virtual StatusCode createObj( IOpaqueAddress* pAddr, DataObject*& pObj );
   
   /// Function preparing the container to be written out
   virtual xAOD::MuonContainer* createPersistent( xAOD::MuonContainer* trans );
   /// Function reading in the persistent object
   virtual xAOD::MuonContainer* createTransient();

private:

   /// StoreGate key of the container just being created
   std::string m_key;

}; // class xAODMuonContainerCnv

#endif 
