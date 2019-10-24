// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTAUATHENAPOOL_XAODTAUJETCONTAINERCNV_V2_H
#define XAODTAUATHENAPOOL_XAODTAUJETCONTAINERCNV_V2_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// EDM include(s):
#include "xAODTau/versions/TauJetContainer_v2.h"
#include "xAODTau/TauJetContainer.h"

/// Converter class used for reading xAOD::TauJetContainer_v2
///
/// This is an extremely simple class. Since the interface containers don't
/// actually hold any persistent payload, converting from one to the other
/// is extremely simple. We could've even just used a ROOT reading rule for
/// doing this.
///
/// But, since the interface changes were substantial, this is a way to
/// prevent the users from reading old xAOD files outside of Athena with the
/// latest xAODTau version. Since ROOT will not convert between the types
/// automatically, the error messages that the users see in that case,
/// should be easier to understand.
///
class xAODTauJetContainerCnv_v2 :
   public T_AthenaPoolTPCnvBase< xAOD::TauJetContainer,
                                 xAOD::TauJetContainer_v2 > {

public:
   /// Default constructor
   xAODTauJetContainerCnv_v2();

   /// Function converting from the old type to the current one
   virtual void persToTrans( const xAOD::TauJetContainer_v2* oldObj,
                             xAOD::TauJetContainer* newObj,
                             MsgStream& log );
   /// Dummy function inherited from the base class
   virtual void transToPers( const xAOD::TauJetContainer*,
                             xAOD::TauJetContainer_v2*,
                             MsgStream& log );

}; // class xAODTauJetContainer_v2

#endif // XAODTAUATHENAPOOL_XAODTAUJETCONTAINERCNV_V2_H
