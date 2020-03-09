// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
#ifndef XAODTRIGGERATHENAPOOL_XAODJETROICONTAINERCNV_V1_H
#define XAODTRIGGERATHENAPOOL_XAODJETROICONTAINERCNV_V1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// EDM include(s):
#include "xAODTrigger/versions/JetRoIContainer_v1.h"
#include "xAODTrigger/JetRoIContainer.h"

/// Converter class used for reading xAOD::JetRoIContainer_v1
///
class xAODJetRoIContainerCnv_v1 :
   public T_AthenaPoolTPCnvConstBase< xAOD::JetRoIContainer,
                                      xAOD::JetRoIContainer_v1 > {

public:
   using base_class::transToPers;
   using base_class::persToTrans;

   /// Default constructor
   xAODJetRoIContainerCnv_v1();

   /// Function converting from the old type to the current one
   virtual void persToTrans( const xAOD::JetRoIContainer_v1* oldObj,
                             xAOD::JetRoIContainer* newObj,
                             MsgStream& log ) const override;
   /// Dummy function inherited from the base class
   virtual void transToPers( const xAOD::JetRoIContainer*,
                             xAOD::JetRoIContainer_v1*,
                             MsgStream& log ) const override;

}; // class xAODJetRoIContainer_v1

#endif // XAODTRIGGERATHENAPOOL_XAODJETROICONTAINERCNV_V1_H
