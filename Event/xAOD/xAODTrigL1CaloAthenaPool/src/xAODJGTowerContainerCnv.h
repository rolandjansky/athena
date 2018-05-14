/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef XAODTRIGL1CALOATHENAPOOL_XAODJGTOWERCONTAINERCNV_H
#define XAODTRIGL1CALOATHENAPOOL_XAODJGTOWERCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTrigL1Calo/JGTowerContainer.h"

/// Type definition for the converter's base
typedef T_AthenaPoolCustomCnv< xAOD::JGTowerContainer,
                               xAOD::JGTowerContainer >
   xAODJGTowerContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::JGTowerContainer class
 *
 *         Simple converter class making the xAOD::JGTowerContainer
 *         class known to POOL.
 *
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 * @author John Morris <john.morris@cern.ch>
 *
 */
class xAODJGTowerContainerCnv : public xAODJGTowerContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODJGTowerContainerCnv >;

public:
   /// Converter constructor
   xAODJGTowerContainerCnv( ISvcLocator* svcLoc );

   /// Re-implemented function in order to get access to the SG key
   virtual StatusCode createObj( IOpaqueAddress* pAddr, DataObject*& pObj );   
   /// Function preparing the container to be written out
   virtual xAOD::JGTowerContainer*  createPersistent( xAOD::JGTowerContainer* trans );
   /// Function reading in the persistent object
   virtual xAOD::JGTowerContainer* createTransient();

private:
   /// Function preparing a vertex object for persistence
   void toPersistent( xAOD::JGTower* cluster ) const;
   
   /// StoreGate key of the container just being created
   std::string m_key;

}; // class xAODJGTowerContainerCnv

#endif 
