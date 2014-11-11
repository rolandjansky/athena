/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRACKINGATHENAPOOL_XAODVERTEXCONTAINERCNV_H
#define XAODTRACKINGATHENAPOOL_XAODVERTEXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTracking/VertexContainer.h"

/// Type definition for the converter's base
typedef T_AthenaPoolCustomCnv< xAOD::VertexContainer,
                               xAOD::VertexContainer >
   xAODVertexContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::VertexContainer class
 *
 *         Simple converter class making the xAOD::VertexContainer
 *         class known to POOL.
 *
 * @author Edward Moyse <Edward.Moyse@cern.ch>
 */
class xAODVertexContainerCnv : public xAODVertexContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODVertexContainerCnv >;

public:
   /// Converter constructor
   xAODVertexContainerCnv( ISvcLocator* svcLoc );

   /// Re-implemented function in order to get access to the SG key
   virtual StatusCode createObj( IOpaqueAddress* pAddr, DataObject*& pObj );

   /// Function preparing the container to be written out
   virtual xAOD::VertexContainer* createPersistent( xAOD::VertexContainer* trans );

   /// Function reading in the persistent object
   virtual xAOD::VertexContainer* createTransient();

private:

   /// StoreGate key of the container just being created
   std::string m_key;

}; // class xAODVertexContainerCnv

#endif 
