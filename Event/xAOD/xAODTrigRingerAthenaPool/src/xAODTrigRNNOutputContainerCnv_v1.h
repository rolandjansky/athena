// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigRNNOutputContainerCnv_v1.h 707590 2015-11-12 19:09:03Z krasznaa $
#ifndef XAODTRIGRINGERATHENAPOOL_XAODTRIGRNNOUTPUTCONTAINERCNV_V1_H
#define XAODTRIGRINGERATHENAPOOL_XAODTRIGRNNOUTPUTCONTAINERCNV_V1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// EDM include(s):
#include "xAODTrigRinger/versions/TrigRNNOutputContainer_v1.h"
#include "xAODTrigRinger/TrigRNNOutputContainer.h"

/// Converter class used for reading xAOD::TrigRNNOutputContainer_v1
///
class xAODTrigRNNOutputContainerCnv_v1 :
   public T_AthenaPoolTPCnvConstBase< xAOD::TrigRNNOutputContainer,
                                      xAOD::TrigRNNOutputContainer_v1 > {

public:
   using base_class::transToPers;
   using base_class::persToTrans;


   /// Default constructor
   xAODTrigRNNOutputContainerCnv_v1();

   /// Function converting from the old type to the current one
   virtual void persToTrans( const xAOD::TrigRNNOutputContainer_v1* oldObj,
                             xAOD::TrigRNNOutputContainer* newObj,
                             MsgStream& log ) const override;
   /// Dummy function inherited from the base class
   virtual void transToPers( const xAOD::TrigRNNOutputContainer*,
                             xAOD::TrigRNNOutputContainer_v1*,
                             MsgStream& log ) const override;

}; // class xAODTrigRNNOutputContainerCnv_v1

#endif // XAODTRIGRINGERATHENAPOOL_XAODTRIGRNNOUTPUTCONTAINERCNV_V1_H
