/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGGERATHENAPOOL_XAODTRIGCOMPOSITEAUXCONTAINER_V1_H
#define XAODTRIGGERATHENAPOOL_XAODTRIGCOMPOSITEAUXCONTAINER_V1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// EDM include(s):
#include "xAODTrigger/TrigCompositeAuxContainer.h"
#include "xAODTrigger/versions/TrigCompositeAuxContainer_v1.h"

/// Converter class used for reading TrigCompositeAuxContainer_v1
///
/// This class implements the meat of the v1->vX conversion for xAOD::TrigCompositeAuxContainer.
/// It translates the payload from the v1 object to the latest type.
/// The converter is implemented in a separate class in order to make it
/// visible to the BS reading code as well.
///
/// v1 used ByteStreamAuxContainer_v1 base, v2 uses the standard AuxContainerBase
///
class xAODTrigCompositeAuxContainerCnv_v1 :
   public T_AthenaPoolTPCnvBase< xAOD::TrigCompositeAuxContainer,
                                 xAOD::TrigCompositeAuxContainer_v1 > {

public:
   /// Default constructor
   xAODTrigCompositeAuxContainerCnv_v1();

   /// Function converting from the old type to the current one
   virtual void persToTrans( const xAOD::TrigCompositeAuxContainer_v1* oldObj,
                             xAOD::TrigCompositeAuxContainer* newObj,
                             MsgStream& log );
   /// Dummy function inherited from the base class
   virtual void transToPers( const xAOD::TrigCompositeAuxContainer*,
                             xAOD::TrigCompositeAuxContainer_v1*,
                             MsgStream& log );

}; // class TrigCompositeAuxContainer_v1

#endif // XAODTRIGGERATHENAPOOL_XAODTRIGCOMPOSITEAUXCONTAINER_V1_H
