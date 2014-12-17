/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigNavigation_v1.cxx 613820 2014-08-28 13:36:36Z krasznaa $

// xAOD include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrigger/versions/TrigNavigation_v1.h"

namespace xAOD {

   TrigNavigation_v1::TrigNavigation_v1()
      : SG::AuxElement() {

   }

   AUXSTORE_OBJECT_SETTER_AND_GETTER( TrigNavigation_v1,
                                      std::vector< uint32_t >,
                                      serialized, setSerialized )

} // namespace xAOD
