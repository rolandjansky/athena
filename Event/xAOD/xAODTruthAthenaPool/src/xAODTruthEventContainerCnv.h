// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
#ifndef XAODTRUTHATHENAPOOL_XAODTRUTHEVENTCONTAINERCNV_H
#define XAODTRUTHATHENAPOOL_XAODTRUTHEVENTCONTAINERCNV_H

// Base package(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM package(s):
#include "xAODTruth/TruthEventContainer.h"

/// The converter is just the specialisation of the template type
typedef T_AthenaPoolCustomCnv< xAOD::TruthEventContainer,
			       xAOD::TruthEventContainer >
    xAODTruthEventContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::TruthEventContainer class
 *
 *         Simple converter class making the xAOD::TruthEventContainer
 *         class known to POOL. (based on xAOD::JetContainer)
 *
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 * @author Jovan Mitrevski <Jovan.Mitrevski@cern.ch>
 *
 * $Revision$
 * $Date$
 */
class xAODTruthEventContainerCnv : public xAODTruthEventContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODTruthEventContainerCnv >;

public:
   /// Converter constructor
   xAODTruthEventContainerCnv( ISvcLocator* svcLoc );

   /// Re-implemented function in order to get access to the SG key 
   virtual StatusCode createObj( IOpaqueAddress* pAddr, DataObject*& pObj );

   /// Function preparing the container to be written out
   virtual xAOD::TruthEventContainer* createPersistent( xAOD::TruthEventContainer* trans );
   /// Function reading in the persistent object
   virtual xAOD::TruthEventContainer* createTransient();

private:
   /// Function preparing a vertex object for persistence
   void toPersistent( xAOD::TruthEvent* cluster ) const;

   /// StoreGate key of the container just being created
   std::string m_key;

}; // class xAODTruthEventContainerCnv


#endif // XAODTRUTHATHENAPOOL_XAODTRUTHEVENTCONTAINERCNV_H
