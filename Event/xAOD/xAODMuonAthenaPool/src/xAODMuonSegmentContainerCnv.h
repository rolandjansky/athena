/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODMUONATHENAPOOL_XAODMUONSEGMENTCONTAINERCNV_H
#define XAODMUONATHENAPOOL_XAODMUONSEGMENTCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODMuon/MuonSegmentContainer.h"

/// Type definition for the converter's base
typedef T_AthenaPoolCustomCnv< xAOD::MuonSegmentContainer,
                               xAOD::MuonSegmentContainer >
   xAODMuonSegmentContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::MuonSegmentContainer class
 *
 *         Simple converter class making the xAOD::MuonSegmentContainer
 *         class known to POOL.
 *
 * @author Edward Moyse <Edward.Moyse@cern.ch>
 *
 */
class xAODMuonSegmentContainerCnv : public xAODMuonSegmentContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODMuonSegmentContainerCnv >;

public:
   /// Converter constructor
   xAODMuonSegmentContainerCnv( ISvcLocator* svcLoc );

   /// Re-implemented function in order to get access to the SG key
   virtual StatusCode createObj( IOpaqueAddress* pAddr, DataObject*& pObj );
   
   /// Function preparing the container to be written out
   virtual xAOD::MuonSegmentContainer* createPersistent( xAOD::MuonSegmentContainer* trans );
   /// Function reading in the persistent object
   virtual xAOD::MuonSegmentContainer* createTransient();

private:
   /// Function preparing a vertex object for persistence
   void toPersistent( xAOD::MuonSegment* cluster ) const;

   /// StoreGate key of the container just being created
   std::string m_key;

}; // class xAODMuonSegmentContainerCnv

#endif 
