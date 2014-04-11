// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODL2StandAloneMuonContainerCnv.h 592615 2014-04-11 14:36:58Z krasznaa $
#ifndef XAODTRIGMUONATHENAPOOL_XAODL2STANDALONEMUONCONTAINERCNV_H
#define XAODTRIGMUONATHENAPOOL_XAODL2STANDALONEMUONCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTrigMuon/L2StandAloneMuonContainer.h"

/// Type definition for the converter's base
typedef T_AthenaPoolCustomCnv< xAOD::L2StandAloneMuonContainer,
                               xAOD::L2StandAloneMuonContainer >
   xAODL2StandAloneMuonContainerCnvBase;

/**
 * $Revision: 592615 $
 * $Date: 2014-04-11 16:36:58 +0200 (Fri, 11 Apr 2014) $
 */
class xAODL2StandAloneMuonContainerCnv :
   public xAODL2StandAloneMuonContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODL2StandAloneMuonContainerCnv >;

public:
   /// Converter constructor
   xAODL2StandAloneMuonContainerCnv( ISvcLocator* svcLoc );

   /// Re-implemented function in order to get access to the SG key
   virtual StatusCode createObj( IOpaqueAddress* pAddr, DataObject*& pObj );

   /// Function preparing the container to be written out
   virtual xAOD::L2StandAloneMuonContainer*
   createPersistent( xAOD::L2StandAloneMuonContainer* trans );
   /// Function reading in the persistent object
   virtual xAOD::L2StandAloneMuonContainer* createTransient();

private:
   /// StoreGate key of the container just being created
   std::string m_key;

}; // class xAODL2StandAloneMuonContainerCnv

#endif // XAODTRIGMUONATHENAPOOL_XAODL2STANDALONEMUONCONTAINERCNV_H
