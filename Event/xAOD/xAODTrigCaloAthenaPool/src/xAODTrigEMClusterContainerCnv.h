// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigEMClusterContainerCnv.h
#ifndef XAODTRIGCALOATHENAPOOL_XAODTRIGEMCLUSTERCONTAINERCNV_H
#define XAODTRIGCALOATHENAPOOL_XAODTRIGEMCLUSTERCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTrigCalo/TrigEMClusterContainer.h"

/// Type definition for the converter's base
typedef T_AthenaPoolCustomCnv< xAOD::TrigEMClusterContainer,
                               xAOD::TrigEMClusterContainer >
   xAODTrigEMClusterContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::TrigEMClusterContainer class
 *
 *         Simple converter class making the xAOD::TrigEMClusterContainer
 *         class known to POOL.
 *
 * @author Aranzazu Ruiz Martinez <Aranzazu.Ruiz.Martinez@cern.ch>
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 *
 * $Revision: 592328 $
 * $Date: 2014-04-10 12:58:05 +0200 (Thu, 10 Apr 2014) $
 */
class xAODTrigEMClusterContainerCnv : public xAODTrigEMClusterContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODTrigEMClusterContainerCnv >;

public:
   /// Converter constructor
   xAODTrigEMClusterContainerCnv( ISvcLocator* svcLoc );

   // Re-implemented function in order to get access to the SG key
   virtual StatusCode createObj( IOpaqueAddress* pAddr, DataObject*& pObj );

   /// Function preparing the container to be written out
   virtual xAOD::TrigEMClusterContainer*
   createPersistent( xAOD::TrigEMClusterContainer* trans );
   /// Function reading in the persistent object
   virtual xAOD::TrigEMClusterContainer* createTransient();

private:
   /// StoreGate key of the container just being created
   std::string m_key;

}; // class xAODTrigEMClusterContainerCnv

#endif // XAODTRIGCALOATHENAPOOL_XAODTRIGEMCLUSTERCONTAINERCNV_H
