// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTauJetContainerCnv.h 581660 2014-02-05 15:52:29Z janus $
#ifndef XAODTAUEVENTATHENAPOOL_XAODTAUJETCONTAINERCNV_H
#define XAODTAUEVENTATHENAPOOL_XAODTAUJETCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTau/TauJetContainer.h"

/// Type definition for the converter's base
typedef T_AthenaPoolCustomCnv< xAOD::TauJetContainer,
                               xAOD::TauJetContainer >
   xAODTauJetContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::TauJetContainer class
 *
 *         Simple converter class making the xAOD::TauJetContainer
 *         class known to POOL.
 *
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 * @author Michel Janus <janus@cern.ch>
 *
 * $Revision: 581660 $
 * $Date: 2014-02-05 16:52:29 +0100 (Wed, 05 Feb 2014) $
 */
class xAODTauJetContainerCnv : public xAODTauJetContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODTauJetContainerCnv >;

public:
   /// Converter constructor
   xAODTauJetContainerCnv( ISvcLocator* svcLoc );

   /// Re-implemented function in order to get access to the SG key
   virtual StatusCode createObj( IOpaqueAddress* pAddr, DataObject*& pObj );

   /// Function preparing the container to be written out
   virtual xAOD::TauJetContainer*
   createPersistent( xAOD::TauJetContainer* trans );
   /// Function reading in the persistent object
   virtual xAOD::TauJetContainer* createTransient();

private:
   /// Function preparing a vertex object for persistence
   void toPersistent( xAOD::TauJet* taujet ) const;

   /// StoreGate key of the container just being created
   std::string m_key;

}; // class xAODTauJetContainerCnv

#endif // XAODTAUEVENTATHENAPOOL_XAODTAUJETCONTAINERCNV_H
