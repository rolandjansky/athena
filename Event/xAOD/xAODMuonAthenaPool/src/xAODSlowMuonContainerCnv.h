/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODSLOWMUONATHENAPOOL_XAODSLOWMUONCONTAINERCNV_H
#define XAODSLOWMUONATHENAPOOL_XAODSLOWMUONCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODMuon/SlowMuonContainer.h"

/// Type definition for the converter's base
typedef T_AthenaPoolCustomCnv< xAOD::SlowMuonContainer,
                               xAOD::SlowMuonContainer >
   xAODSlowMuonContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::SlowMuonContainer class
 *
 *         Simple converter class making the xAOD::SlowMuonContainer
 *         class known to POOL.
 *
 * @author Sue Cheatham <susan.cheatham@cern.ch>
 * copied from MuonCnv by
 * @author Edward Moyse <Edward.Moyse@cern.ch>
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 *
 */
class xAODSlowMuonContainerCnv : public xAODSlowMuonContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODSlowMuonContainerCnv >;

public:
   /// Converter constructor
   xAODSlowMuonContainerCnv( ISvcLocator* svcLoc );

   /// Re-implemented function in order to get access to the SG key
   virtual StatusCode createObj( IOpaqueAddress* pAddr, DataObject*& pObj );
   
   /// Function preparing the container to be written out
   virtual xAOD::SlowMuonContainer* createPersistent( xAOD::SlowMuonContainer* trans );
   /// Function reading in the persistent object
   virtual xAOD::SlowMuonContainer* createTransient();

private:
   /// Function preparing a vertex object for persistence
   void toPersistent( xAOD::SlowMuon* slowMuon ) const;

   /// StoreGate key of the container just being created
   std::string m_key;

}; // class xAODSlowMuonContainerCnv

#endif 
