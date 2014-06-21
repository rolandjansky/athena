// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODJetContainerCnv.h 581167 2014-02-03 10:45:56Z krasznaa $
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
 * $Revision: 581167 $
 * $Date: 2014-02-03 11:45:56 +0100 (Mon, 03 Feb 2014) $
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
   /// Function preparing a vertex object for persistence
   void toPersistent( xAOD::Jet* jet ) const;

   /// StoreGate key of the container just being created
   std::string m_key;

}; // class xAODJetContainerCnv

#endif // XAODJETATHENAPOOL_XAODJETCONTAINERCNV_H
