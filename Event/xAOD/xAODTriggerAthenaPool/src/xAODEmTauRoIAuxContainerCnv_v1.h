// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
#ifndef XAODTRIGGERATHENAPOOL_XAODEMTAUROIAUXCONTAINERCNV_V1_H
#define XAODTRIGGERATHENAPOOL_XAODEMTAUROIAUXCONTAINERCNV_V1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// EDM include(s):
#include "xAODTrigger/versions/EmTauRoIAuxContainer_v1.h"
#include "xAODTrigger/EmTauRoIAuxContainer.h"

/// Converter class used for reading xAOD::EmTauRoIAuxContainer_v1
///
class xAODEmTauRoIAuxContainerCnv_v1 :
   public T_AthenaPoolTPCnvBase< xAOD::EmTauRoIAuxContainer,
                                 xAOD::EmTauRoIAuxContainer_v1 > {

public:
   /// Default constructor
   xAODEmTauRoIAuxContainerCnv_v1();

   /// Function converting from the old type to the current one
   virtual void persToTrans( const xAOD::EmTauRoIAuxContainer_v1* oldObj,
                             xAOD::EmTauRoIAuxContainer* newObj,
                             MsgStream& log );
   /// Dummy function inherited from the base class
   virtual void transToPers( const xAOD::EmTauRoIAuxContainer*,
                             xAOD::EmTauRoIAuxContainer_v1*,
                             MsgStream& log );

}; // class xAODEmTauRoIAuxContainer_v1

#endif // XAODTRIGGERATHENAPOOL_XAODEMTAUROIAUXCONTAINERCNV_V1_H
