// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODCaloClusterContainerCnv.h 581034 2014-01-31 13:22:57Z krasznaa $
#ifndef XAODCALOEVENTATHENAPOOL_XAODCALOCLUSTERCONTAINERCNV_H
#define XAODCALOEVENTATHENAPOOL_XAODCALOCLUSTERCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODCaloEvent/CaloClusterContainer.h"

/// Type definition for the converter's base
typedef T_AthenaPoolCustomCnv< xAOD::CaloClusterContainer,
                               xAOD::CaloClusterContainer >
   xAODCaloClusterContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::CaloClusterContainer class
 *
 *         Simple converter class making the xAOD::CaloClusterContainer
 *         class known to POOL.
 *
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 *
 * $Revision: 581034 $
 * $Date: 2014-01-31 14:22:57 +0100 (Fri, 31 Jan 2014) $
 */
class xAODCaloClusterContainerCnv : public xAODCaloClusterContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODCaloClusterContainerCnv >;

public:
   /// Converter constructor
   xAODCaloClusterContainerCnv( ISvcLocator* svcLoc );

   /// Re-implemented function in order to get access to the SG key
   virtual StatusCode createObj( IOpaqueAddress* pAddr, DataObject*& pObj );

   /// Function preparing the container to be written out
   virtual xAOD::CaloClusterContainer*
   createPersistent( xAOD::CaloClusterContainer* trans );
   /// Function reading in the persistent object
   virtual xAOD::CaloClusterContainer* createTransient();

private:
   /// Function preparing a vertex object for persistence
   void toPersistent( xAOD::CaloCluster* cluster ) const;

   /// StoreGate key of the container just being created
   std::string m_key;

}; // class xAODCaloClusterContainerCnv

#endif // XAODCALOEVENTATHENAPOOL_XAODCALOCLUSTERCONTAINERCNV_H
