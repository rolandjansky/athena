// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigCaloClusterContainerCnv.h 592328 2014-04-10 10:58:05Z krasznaa $
#ifndef XAODTRIGCALOATHENAPOOL_XAODTRIGCALOCLUSTERCONTAINERCNV_H
#define XAODTRIGCALOATHENAPOOL_XAODTRIGCALOCLUSTERCONTAINERCNV_H

// System include(s):
#include <string>

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTrigCalo/TrigCaloClusterContainer.h"

/// Type definition for the converter's base
typedef T_AthenaPoolCustomCnv< xAOD::TrigCaloClusterContainer,
                               xAOD::TrigCaloClusterContainer >
   xAODTrigCaloClusterContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::TrigCaloClusterContainer class
 *
 *         Simple converter class making the xAOD::TrigCaloClusterContainer
 *         class known to POOL.
 *
 * @author Aranzazu Ruiz Martinez <Aranzazu.Ruiz.Martinez@cern.ch>
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 *
 * $Revision: 592328 $
 * $Date: 2014-04-10 12:58:05 +0200 (Thu, 10 Apr 2014) $
 */
class xAODTrigCaloClusterContainerCnv :
   public xAODTrigCaloClusterContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODTrigCaloClusterContainerCnv >;

public:
   /// Converter constructor
   xAODTrigCaloClusterContainerCnv( ISvcLocator* svcLoc );

   // Re-implemented function in order to get access to the SG key
   virtual StatusCode createObj( IOpaqueAddress* pAddr, DataObject*& pObj );

   /// Function preparing the container to be written out
   virtual xAOD::TrigCaloClusterContainer*
   createPersistent( xAOD::TrigCaloClusterContainer* trans );
   /// Function reading in the persistent object
   virtual xAOD::TrigCaloClusterContainer* createTransient();

private:
   /// StoreGate key of the container just being created
   std::string m_key;

}; // class xAODTrigCaloClusterContainerCnv

#endif // XAODTRIGCALOATHENAPOOL_XAODTRIGCALOCLUSTERCONTAINERCNV_H
