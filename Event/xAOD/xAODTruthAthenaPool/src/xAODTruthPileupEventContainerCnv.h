// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTruthPileupEventContainerCnv.h 637416 2014-12-24 11:43:22Z jcatmore $
#ifndef XAODTRUTHATHENAPOOL_XAODTRUTHPILEUPEVENTCONTAINERCNV_H
#define XAODTRUTHATHENAPOOL_XAODTRUTHPILEUPEVENTCONTAINERCNV_H

// Base package(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM package(s):
#include "xAODTruth/TruthPileupEventContainer.h"

/// The converter is just the specialisation of the template type
typedef T_AthenaPoolCustomCnv< xAOD::TruthPileupEventContainer,
			       xAOD::TruthPileupEventContainer >
    xAODTruthPileupEventContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::TruthPileupEventContainer class
 *
 *         Simple converter class making the xAOD::TruthPileupEventContainer
 *         class known to POOL. (based on xAOD::JetContainer)
 *
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 * @author Jovan Mitrevski <Jovan.Mitrevski@cern.ch>
 *
 * $Revision: 637416 $
 * $Date: 2014-12-24 12:43:22 +0100 (Wed, 24 Dec 2014) $
 */
class xAODTruthPileupEventContainerCnv : public xAODTruthPileupEventContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODTruthPileupEventContainerCnv >;

public:
   /// Converter constructor
   xAODTruthPileupEventContainerCnv( ISvcLocator* svcLoc );

   /// Re-implemented function in order to get access to the SG key 
   virtual StatusCode createObj( IOpaqueAddress* pAddr, DataObject*& pObj );

   /// Function preparing the container to be written out
   virtual xAOD::TruthPileupEventContainer* createPersistent( xAOD::TruthPileupEventContainer* trans );
   /// Function reading in the persistent object
   virtual xAOD::TruthPileupEventContainer* createTransient();

private:
   /// Function preparing a vertex object for persistence
   void toPersistent( xAOD::TruthPileupEvent* cluster ) const;

   /// StoreGate key of the container just being created
   std::string m_key;

}; // class xAODTruthPileupEventContainerCnv


#endif // XAODTRUTHATHENAPOOL_XAODTRUTHPILEUPEVENTCONTAINERCNV_H
