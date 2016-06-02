/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRACKINGATHENAPOOL_XAODSCTRDOCONTAINERCNV_H
#define XAODTRACKINGATHENAPOOL_XAODSCTRDOCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTracking/SCTRawHitValidationContainer.h"

/// Type definition for the converter's base
typedef T_AthenaPoolCustomCnv< xAOD::SCTRawHitValidationContainer,
                               xAOD::SCTRawHitValidationContainer >
   xAODSCTRDOContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::SCTRawHitValidationContainer class
 *
 *         Simple converter class making the xAOD::SCTRawHitValidationContainer
 *         class known to POOL.
 *
 * @author Edward Moyse <Edward.Moyse@cern.ch>
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 *
 */
class xAODSCTRDOContainerCnv : public xAODSCTRDOContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODSCTRDOContainerCnv >;

public:
   /// Converter constructor
   xAODSCTRDOContainerCnv( ISvcLocator* svcLoc );
   
   /// Re-implemented function in order to get access to the SG key
   virtual StatusCode createObj( IOpaqueAddress* pAddr, DataObject*& pObj ) override;
   
   /// Function preparing the container to be written out
   virtual xAOD::SCTRawHitValidationContainer* createPersistent( xAOD::SCTRawHitValidationContainer* trans ) override;
   
   /// Function reading in the persistent object
   virtual xAOD::SCTRawHitValidationContainer* createTransient() override;

protected:
   /// StoreGate key of the container just being created
   std::string m_key;

}; // class xAODSCTRDOContainerCnv

#endif 
