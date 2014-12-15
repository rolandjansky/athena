// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
#ifndef XAODTRIGGERATHENAPOOL_XAODEMTAUROICONTAINERCNV_V1_H
#define XAODTRIGGERATHENAPOOL_XAODEMTAUROICONTAINERCNV_V1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// EDM include(s):
#include "xAODTrigger/versions/EmTauRoIContainer_v1.h"
#include "xAODTrigger/EmTauRoIContainer.h"

/// Converter class used for reading xAOD::EmTauRoIContainer_v1
///
class xAODEmTauRoIContainerCnv_v1 :
   public T_AthenaPoolTPCnvBase< xAOD::EmTauRoIContainer,
                                 xAOD::EmTauRoIContainer_v1 > {

public:
   /// Default constructor
   xAODEmTauRoIContainerCnv_v1();

   /// Function converting from the old type to the current one
   virtual void persToTrans( const xAOD::EmTauRoIContainer_v1* oldObj,
                             xAOD::EmTauRoIContainer* newObj,
                             MsgStream& log );
   /// Dummy function inherited from the base class
   virtual void transToPers( const xAOD::EmTauRoIContainer*,
                             xAOD::EmTauRoIContainer_v1*,
                             MsgStream& log );

}; // class xAODEmTauRoIContainer_v1

#endif // XAODTRIGGERATHENAPOOL_XAODEMTAUROICONTAINERCNV_V1_H
