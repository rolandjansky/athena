/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALOATHENAPOOL_XAODL1TOPORAWDATACONTAINERCNV_H
#define XAODTRIGL1CALOATHENAPOOL_XAODL1TOPORAWDATACONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTrigL1Calo/L1TopoRawDataContainer.h"

/// Type definition for the converter's base
typedef T_AthenaPoolCustomCnv< xAOD::L1TopoRawDataContainer,
                               xAOD::L1TopoRawDataContainer >
   xAODL1TopoRawDataContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::L1TopoRawDataContainer class
 *
 *         Simple converter class making the xAOD::MuonContainer
 *         class known to POOL.
 *
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 * @author John Morris <john.morris@cern.ch>
 * @author Simon George <s.george@rhul.ac.uk>
 *
 */
class xAODL1TopoRawDataContainerCnv : public xAODL1TopoRawDataContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODL1TopoRawDataContainerCnv >;

public:
   /// Converter constructor
   xAODL1TopoRawDataContainerCnv( ISvcLocator* svcLoc );

   /// Re-implemented function in order to get access to the SG key
   virtual StatusCode createObj( IOpaqueAddress* pAddr, DataObject*& pObj );    
   /// Function preparing the container to be written out
   virtual xAOD::L1TopoRawDataContainer*
   createPersistent( xAOD::L1TopoRawDataContainer* trans );
   /// Function reading in the persistent object
   virtual xAOD::L1TopoRawDataContainer* createTransient();

private:
   /// Function preparing a vertex object for persistence
   void toPersistent( xAOD::L1TopoRawData* cluster ) const;
   
   /// StoreGate key of the container just being created
   std::string m_key;

}; // class xAODL1TopoRawDataContainerCnv

#endif 
