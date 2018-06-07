/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALOATHENAPOOL_XAODTRIGGERTOWERCONTAINERCNV_H
#define XAODTRIGL1CALOATHENAPOOL_XAODTRIGGERTOWERCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTrigL1Calo/TriggerTowerContainer.h"

/// Type definition for the converter's base
typedef T_AthenaPoolCustomCnv< xAOD::TriggerTowerContainer,
                               xAOD::TriggerTowerContainer >
   xAODTriggerTowerContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::TriggerTowerContainer class
 *
 *         Simple converter class making the xAOD::TriggerTowerContainer
 *         class known to POOL.
 *
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 * @author John Morris <john.morris@cern.ch>
 *
 */
class xAODTriggerTowerContainerCnv : public xAODTriggerTowerContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODTriggerTowerContainerCnv >;

public:
   /// Converter constructor
   xAODTriggerTowerContainerCnv( ISvcLocator* svcLoc );

   /// Re-implemented function in order to get access to the SG key
   virtual StatusCode createObj( IOpaqueAddress* pAddr, DataObject*& pObj );   
   /// Function preparing the container to be written out
   virtual xAOD::TriggerTowerContainer*  createPersistent( xAOD::TriggerTowerContainer* trans );
   /// Function reading in the persistent object
   virtual xAOD::TriggerTowerContainer* createTransient();

private:
   /// Function preparing a vertex object for persistence
   void toPersistent( xAOD::TriggerTower* cluster ) const;
   
   /// StoreGate key of the container just being created
   std::string m_key;

}; // class xAODTriggerTowerContainerCnv

#endif 
