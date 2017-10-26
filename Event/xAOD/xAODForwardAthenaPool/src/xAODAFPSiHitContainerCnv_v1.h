/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODFORWARDATHENAPOOL_XAODAFPSIHITCONTAINERCNV_V1_H
#define XAODFORWARDATHENAPOOL_XAODAFPSIHITCONTAINERCNV_V1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// EDM include(s):
#include "xAODForward/versions/AFPSiHitContainer_v1.h"
#include "xAODForward/AFPSiHitContainer.h"

/// Converter class used for reading xAOD::AFPSiHitContainer_v1
///
/// This is an extremely simple class. Since the interface containers don't
/// actually hold any persistent payload, converting from one to the other
/// is extremely simple. We could've even just used a ROOT reading rule for
/// doing this.
///
/// But, since the interface changes were substantial, this is a way to
/// prevent the users from reading old xAOD files outside of Athena with the
/// latest xAODAFPSiHit  version. Since ROOT will not convert between the types
/// automatically, the error messages that the users see in that case,
/// should be easier to understand.
///
/// Implementation based on xAODTauJetContainerCnv_v1.h
class xAODAFPSiHitContainerCnv_v1 :
   public T_AthenaPoolTPCnvBase< xAOD::AFPSiHitContainer,
                                 xAOD::AFPSiHitContainer_v1 > {

public:
   xAODAFPSiHitContainerCnv_v1();

   /// Function converting from the old type to the current one
   virtual void persToTrans( const xAOD::AFPSiHitContainer_v1* oldObj,
                             xAOD::AFPSiHitContainer* newObj,
                             MsgStream& log );
   /// Dummy function inherited from the base class
   virtual void transToPers( const xAOD::AFPSiHitContainer*,
                             xAOD::AFPSiHitContainer_v1*,
                             MsgStream& log );

}; // class xAODAFPSiHitContainer_v1

#endif //  XAODFORWARDATHENAPOOL_XAODAFPSIHITCONTAINERCNV_V1_H
