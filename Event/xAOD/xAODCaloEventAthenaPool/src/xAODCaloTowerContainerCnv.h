// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODCaloTowerContainerCnv.h 700671 2015-10-15 08:33:38Z wlampl $
#ifndef XAODCALOEVENTATHENAPOOL_XAODCALOTOWERCONTAINERCNV_H
#define XAODCALOEVENTATHENAPOOL_XAODCALOTOWERCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "GaudiKernel/ToolHandle.h"

// EDM include(s):
#include "xAODCaloEvent/CaloTowerContainer.h"

/// Type definition for the converter's base
typedef T_AthenaPoolCustomCnv< xAOD::CaloTowerContainer,
                               xAOD::CaloTowerContainer >
   xAODCaloTowerContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::CaloTowerContainer class
 *
 *         Simple converter class making the xAOD::CaloTowerContainer
 *         class known to POOL.
 *
 * $Revision: 700671 $
 * $Date: 2015-10-15 10:33:38 +0200 (Thu, 15 Oct 2015) $
 */
class xAODCaloTowerContainerCnv : public xAODCaloTowerContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODCaloTowerContainerCnv >;

public:
   /// Converter constructor
   xAODCaloTowerContainerCnv( ISvcLocator* svcLoc );

   /// Re-implemented function in order to get access to the SG key
   virtual StatusCode createObj( IOpaqueAddress* pAddr, DataObject*& pObj );

   /// Function preparing the container to be written out
   virtual xAOD::CaloTowerContainer*
   createPersistent( xAOD::CaloTowerContainer* trans );
   /// Function reading in the persistent object
   virtual xAOD::CaloTowerContainer* createTransient();

private:
   /// StoreGate key of the container just being created
   std::string m_key;

}; // class xAODCaloTowerContainerCnv

#endif // XAODCALOEVENTATHENAPOOL_XAODCALOCLUSTERCONTAINERCNV_H
