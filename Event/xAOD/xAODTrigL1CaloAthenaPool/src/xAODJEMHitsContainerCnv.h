/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALOATHENAPOOL_XAODJEMHITSCONTAINERCNV_H
#define XAODTRIGL1CALOATHENAPOOL_XAODJEMHITSCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTrigL1Calo/JEMHitsContainer.h"

/// Type definition for the converter's base
typedef T_AthenaPoolCustomCnv< xAOD::JEMHitsContainer,
                               xAOD::JEMHitsContainer >
   xAODJEMHitsContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::JEMHitsContainer class
 *
 *         Simple converter class making the xAOD::MuonContainer
 *         class known to POOL.
 *
 * @author Edward Moyse <Edward.Moyse@cern.ch>
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 * @author John Morris <john.morris@cern.ch>
 *
 */
class xAODJEMHitsContainerCnv : public xAODJEMHitsContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODJEMHitsContainerCnv >;

public:
   /// Converter constructor
   xAODJEMHitsContainerCnv( ISvcLocator* svcLoc );

   /// Re-implemented function in order to get access to the SG key
   virtual StatusCode createObj( IOpaqueAddress* pAddr, DataObject*& pObj );    
   /// Function preparing the container to be written out
   virtual xAOD::JEMHitsContainer*
   createPersistent( xAOD::JEMHitsContainer* trans );
   /// Function reading in the persistent object
   virtual xAOD::JEMHitsContainer* createTransient();

private:
   /// Function preparing a vertex object for persistence
   void toPersistent( xAOD::JEMHits* cluster ) const;
   
   /// StoreGate key of the container just being created
   std::string m_key;

}; // class xAODJEMHitsContainerCnv

#endif 
