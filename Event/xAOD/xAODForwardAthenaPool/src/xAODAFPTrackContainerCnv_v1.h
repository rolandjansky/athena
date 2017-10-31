/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODFORWARDATHENAPOOL_XAODAFPTRACKCONTAINERCNV_V1_H
#define XAODFORWARDATHENAPOOL_XAODAFPTRACKCONTAINERCNV_V1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// EDM include(s):
#include "xAODForward/versions/AFPTrackContainer_v1.h"
#include "xAODForward/AFPTrackContainer.h"

/// Converter class used for reading xAOD::AFPTrackContainer_v1
///
/// This is an extremely simple class. Since the interface containers don't
/// actually hold any persistent payload, converting from one to the other
/// is extremely simple. We could've even just used a ROOT reading rule for
/// doing this.
///
/// But, since the interface changes were substantial, this is a way to
/// prevent the users from reading old xAOD files outside of Athena with the
/// latest xAODAFPTrack version. Since ROOT will not convert between the types
/// automatically, the error messages that the users see in that case,
/// should be easier to understand.
///
/// Implementation based on xAODTauJetContainerCnv_v1.h
class xAODAFPTrackContainerCnv_v1 :
   public T_AthenaPoolTPCnvBase< xAOD::AFPTrackContainer,
                                 xAOD::AFPTrackContainer_v1 > {

public:
   xAODAFPTrackContainerCnv_v1();

   /// Function converting from the old type to the current one
   virtual void persToTrans( const xAOD::AFPTrackContainer_v1* oldObj,
                             xAOD::AFPTrackContainer* newObj,
                             MsgStream& log );
   /// Dummy function inherited from the base class
   virtual void transToPers( const xAOD::AFPTrackContainer*,
                             xAOD::AFPTrackContainer_v1*,
                             MsgStream& log );

}; // class xAODAFPTrackContainer_v1

#endif //   XAODFORWARDATHENAPOOL_XAODAFPTRACKCONTAINERCNV_V1_H
