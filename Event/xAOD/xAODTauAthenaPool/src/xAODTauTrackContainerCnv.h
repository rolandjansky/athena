// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTAUEVENTATHENAPOOL_XAODTAUTRACKCONTAINERCNV_H
#define XAODTAUEVENTATHENAPOOL_XAODTAUTRACKCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTau/TauTrackContainer.h"

/// Type definition for the converter's base
typedef T_AthenaPoolCustomCnv< xAOD::TauTrackContainer,
                               xAOD::TauTrackContainer >
   xAODTauTrackContainerCnvBase;

class xAODTauTrackContainerCnv : public xAODTauTrackContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODTauTrackContainerCnv >;

public:
   /// Converter constructor
   xAODTauTrackContainerCnv( ISvcLocator* svcLoc );

   /// Re-implemented function in order to get access to the SG key
   virtual StatusCode createObj( IOpaqueAddress* pAddr, DataObject*& pObj );

   /// Function preparing the container to be written out
   virtual xAOD::TauTrackContainer*
   createPersistent( xAOD::TauTrackContainer* trans );
   /// Function reading in the persistent object
   virtual xAOD::TauTrackContainer* createTransient();

private:
   /// StoreGate key of the container just being created
   std::string m_key;

}; // class xAODTauTrackContainerCnv

#endif // XAODTAUEVENTATHENAPOOL_XAODTAUTRACKCONTAINERCNV_H
