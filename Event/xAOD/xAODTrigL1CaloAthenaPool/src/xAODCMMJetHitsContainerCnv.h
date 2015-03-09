/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALOATHENAPOOL_XAODCMMJETHITSCONTAINERCNV_H
#define XAODTRIGL1CALOATHENAPOOL_XAODCMMJETHITSCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTrigL1Calo/CMMJetHitsContainer.h"

/// Type definition for the converter's base
typedef T_AthenaPoolCustomCnv< xAOD::CMMJetHitsContainer,
                               xAOD::CMMJetHitsContainer >
   xAODCMMJetHitsContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::CMMJetHitsContainer class
 *
 *         Simple converter class making the xAOD::CMMJetHitsContainer
 *         class known to POOL.
 *
 * @author Edward Moyse <Edward.Moyse@cern.ch>
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 * @author John Morris <john.morris@cern.ch>
 *
 */
class xAODCMMJetHitsContainerCnv : public xAODCMMJetHitsContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODCMMJetHitsContainerCnv >;

public:
   /// Converter constructor
   xAODCMMJetHitsContainerCnv( ISvcLocator* svcLoc );

   /// Re-implemented function in order to get access to the SG key
   virtual StatusCode createObj( IOpaqueAddress* pAddr, DataObject*& pObj );    
   /// Function preparing the container to be written out
   virtual xAOD::CMMJetHitsContainer*
   createPersistent( xAOD::CMMJetHitsContainer* trans );
   /// Function reading in the persistent object
   virtual xAOD::CMMJetHitsContainer* createTransient();

private:
   /// Function preparing a vertex object for persistence
   void toPersistent( xAOD::CMMJetHits* cluster ) const;
   
   /// StoreGate key of the container just being created
   std::string m_key;

}; // class xAODCMMJetHitsContainerCnv

#endif 
