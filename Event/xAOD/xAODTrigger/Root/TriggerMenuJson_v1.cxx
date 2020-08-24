/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


// System include(s):
#include <iostream>

// EDM include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrigger/versions/TriggerMenuJson_v1.h"

namespace xAOD {

   TriggerMenuJson_v1::TriggerMenuJson_v1()
      : SG::AuxElement() {

   }

   /////////////////////////////////////////////////////////////////////////////
   //
   //   Implementation of the configuration identifier accessor functions
   //

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TriggerMenuJson_v1, uint32_t, key,
                                         setKey )
   AUXSTORE_OBJECT_SETTER_AND_GETTER( TriggerMenuJson_v1, std::string, name,
                                      setName )


   //
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   //
   //       Implementation of the payload accessor functions
   //


   AUXSTORE_OBJECT_SETTER_AND_GETTER( TriggerMenuJson_v1, std::string,
                                      payload, setPayload )

   //
   /////////////////////////////////////////////////////////////////////////////

} // namespace xAOD
