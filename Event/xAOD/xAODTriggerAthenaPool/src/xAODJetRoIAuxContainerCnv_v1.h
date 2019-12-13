// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
#ifndef XAODTRIGGERATHENAPOOL_XAODJETROIAUXCONTAINERCNV_V1_H
#define XAODTRIGGERATHENAPOOL_XAODJETROIAUXCONTAINERCNV_V1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// EDM include(s):
#include "xAODTrigger/versions/JetRoIAuxContainer_v1.h"
#include "xAODTrigger/JetRoIAuxContainer.h"

/// Converter class used for reading xAOD::JetRoIAuxContainer_v1
///
class xAODJetRoIAuxContainerCnv_v1 :
   public T_AthenaPoolTPCnvConstBase< xAOD::JetRoIAuxContainer,
                                      xAOD::JetRoIAuxContainer_v1 > {

public:
   using base_class::transToPers;
   using base_class::persToTrans;

   /// Default constructor
   xAODJetRoIAuxContainerCnv_v1();

   /// Function converting from the old type to the current one
   virtual void persToTrans( const xAOD::JetRoIAuxContainer_v1* oldObj,
                             xAOD::JetRoIAuxContainer* newObj,
                             MsgStream& log ) const override;
   /// Dummy function inherited from the base class
   virtual void transToPers( const xAOD::JetRoIAuxContainer*,
                             xAOD::JetRoIAuxContainer_v1*,
                             MsgStream& log ) const override;

}; // class xAODJetRoIAuxContainer_v1

#endif // XAODTRIGGERATHENAPOOL_XAODJETROIAUXCONTAINERCNV_V1_H
