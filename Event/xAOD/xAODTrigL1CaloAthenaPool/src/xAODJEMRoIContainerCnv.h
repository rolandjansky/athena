/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALOATHENAPOOL_XAODJEMROICONTAINERCNV_H
#define XAODTRIGL1CALOATHENAPOOL_XAODJEMROICONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTrigL1Calo/JEMRoIContainer.h"

/// Type definition for the converter's base
typedef T_AthenaPoolCustomCnv< xAOD::JEMRoIContainer,
                               xAOD::JEMRoIContainer >
   xAODJEMRoIContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::JEMRoIContainer class
 *
 *         Simple converter class making the xAOD::JEMRoI
 *         class known to POOL.
 *
 * @author Edward Moyse <Edward.Moyse@cern.ch>
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 * @author John Morris <john.morris@cern.ch>
 *
 */
class xAODJEMRoIContainerCnv : public xAODJEMRoIContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODJEMRoIContainerCnv >;

public:
   /// Converter constructor
   xAODJEMRoIContainerCnv( ISvcLocator* svcLoc );

   /// Re-implemented function in order to get access to the SG key
   virtual StatusCode createObj( IOpaqueAddress* pAddr, DataObject*& pObj );    
   /// Function preparing the container to be written out
   virtual xAOD::JEMRoIContainer*
   createPersistent( xAOD::JEMRoIContainer* trans );
   /// Function reading in the persistent object
   virtual xAOD::JEMRoIContainer* createTransient();

private:
   /// Function preparing a vertex object for persistence
   void toPersistent( xAOD::JEMRoI* cluster ) const;
   
   /// StoreGate key of the container just being created
   std::string m_key;

}; // class xAODJEMRoIContainerCnv

#endif 
