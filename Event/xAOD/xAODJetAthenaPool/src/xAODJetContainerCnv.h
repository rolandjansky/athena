// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODJetContainerCnv.h 795694 2017-02-05 23:13:50Z khoo $
#ifndef XAODJETATHENAPOOL_XAODJETCONTAINERCNV_H
#define XAODJETATHENAPOOL_XAODJETCONTAINERCNV_H

// System include(s):
#include <string>

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODJet/JetContainer.h"

/// Type definition for the converter's base
typedef T_AthenaPoolCustomCnv< xAOD::JetContainer,
                               xAOD::JetContainer >
   xAODJetContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::JetContainer class
 *
 *         Simple converter class making the xAOD::JetContainer
 *         class known to POOL.
 *
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 *
 * $Revision: 795694 $
 * $Date: 2017-02-06 00:13:50 +0100 (Mon, 06 Feb 2017) $
 */
class xAODJetContainerCnv : public xAODJetContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODJetContainerCnv >;

public:
   /// Converter constructor
   xAODJetContainerCnv( ISvcLocator* svcLoc );

   /// Re-implemented function in order to get access to the SG key
   virtual StatusCode createObj( IOpaqueAddress* pAddr, DataObject*& pObj );

   /// Function preparing the container to be written out
   virtual xAOD::JetContainer* createPersistent( xAOD::JetContainer* trans );
   /// Function reading in the persistent object
   virtual xAOD::JetContainer* createTransient();

private:
   // /// Function preparing a vertex object for persistence
   // void toPersistent( xAOD::Jet* jet ) const;

   /// StoreGate key of the container just being created
   std::string m_key;

}; // class xAODJetContainerCnv

#endif // XAODJETATHENAPOOL_XAODJETCONTAINERCNV_H
