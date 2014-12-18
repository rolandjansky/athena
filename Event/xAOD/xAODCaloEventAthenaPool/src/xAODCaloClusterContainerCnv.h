// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODCaloClusterContainerCnv.h 636751 2014-12-18 14:50:06Z will $
#ifndef XAODCALOEVENTATHENAPOOL_XAODCALOCLUSTERCONTAINERCNV_H
#define XAODCALOEVENTATHENAPOOL_XAODCALOCLUSTERCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "GaudiKernel/ToolHandle.h"

// EDM include(s):
#include "xAODCaloEvent/CaloClusterContainer.h"

#ifndef XAOD_ANALYSIS
class IxAODClusterCompressor;
#endif

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
 * $Revision: 636751 $
 * $Date: 2014-12-18 15:50:06 +0100 (Thu, 18 Dec 2014) $
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

#ifndef XAOD_ANALYSIS
  /// AlgTool compressing the cluster for storage on disk
  ToolHandle<IxAODClusterCompressor> m_compressor;
#endif

  /// Flag set to false if the retrieval of the compression tool failed
  bool m_doCompression;

}; // class xAODCaloClusterContainerCnv

#endif // XAODCALOEVENTATHENAPOOL_XAODCALOCLUSTERCONTAINERCNV_H
