// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODDiTauJetContainerCnv.h 631921 2015-09-23 23:30:59Z dkirchme $
#ifndef XAODDITAUEVENTATHENAPOOL_XAODDITAUJETCONTAINERCNV_H
#define XAODDITAUEVENTATHENAPOOL_XAODDITAUJETCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTau/DiTauJetContainer.h"

/// Type definition for the converter's base
typedef T_AthenaPoolCustomCnv< xAOD::DiTauJetContainer,
                               xAOD::DiTauJetContainer >
   xAODDiTauJetContainerCnvBase;

class xAODDiTauJetContainerCnv : public xAODDiTauJetContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODDiTauJetContainerCnv >;

public:
   /// Converter constructor
   xAODDiTauJetContainerCnv( ISvcLocator* svcLoc );

   /// Re-implemented function in order to get access to the SG key
   virtual StatusCode createObj( IOpaqueAddress* pAddr, DataObject*& pObj );

   /// Function preparing the container to be written out
   virtual xAOD::DiTauJetContainer*
   createPersistent( xAOD::DiTauJetContainer* trans );
   /// Function reading in the persistent object
   virtual xAOD::DiTauJetContainer* createTransient();

private:
   /// StoreGate key of the container just being created
   std::string m_key;

}; // class xAODDiTauJetContainerCnv

#endif // XAODDITAUEVENTATHENAPOOL_XAODDITAUJETCONTAINERCNV_H