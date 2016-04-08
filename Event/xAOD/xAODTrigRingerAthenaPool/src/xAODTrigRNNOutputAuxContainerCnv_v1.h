// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigRNNOutputAuxContainerCnv_v1.h 707590 2015-11-12 19:09:03Z krasznaa $
#ifndef XAODTRIGRINGERATHENAPOOL_XAODTRIGRNNOUTPUTSAUXCONTAINERCNV_V1_H
#define XAODTRIGRINGERATHENAPOOL_XAODTRIGRNNOUTPUTSAUXCONTAINERCNV_V1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// EDM include(s):
#include "xAODTrigRinger/versions/TrigRNNOutputAuxContainer_v1.h"
#include "xAODTrigRinger/TrigRNNOutputAuxContainer.h"

/// Converter class used for reading xAOD::EmTauRoIAuxContainer_v1
///
class xAODTrigRNNOutputAuxContainerCnv_v1 :
   public T_AthenaPoolTPCnvBase< xAOD::TrigRNNOutputAuxContainer,
                                 xAOD::TrigRNNOutputAuxContainer_v1 > {

public:
   /// Default constructor
   xAODTrigRNNOutputAuxContainerCnv_v1();

   /// Function converting from the old type to the current one
   virtual void persToTrans( const xAOD::TrigRNNOutputAuxContainer_v1* oldObj,
                             xAOD::TrigRNNOutputAuxContainer* newObj,
                             MsgStream& log );
   /// Dummy function inherited from the base class
   virtual void transToPers( const xAOD::TrigRNNOutputAuxContainer*,
                             xAOD::TrigRNNOutputAuxContainer_v1*,
                             MsgStream& log );

}; // class xAODTrigRNNOutputAuxContainerCnv_v1

#endif // not XAODTRIGRINGERATHENAPOOL_XAODTRIGRNNOUTPUTSAUXCONTAINERCNV_V1_H
