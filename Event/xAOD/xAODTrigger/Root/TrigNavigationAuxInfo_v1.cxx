/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigNavigationAuxInfo_v1.cxx 613820 2014-08-28 13:36:36Z krasznaa $

// Local include(s):
#include "xAODTrigger/versions/TrigNavigationAuxInfo_v1.h"

namespace xAOD {

   TrigNavigationAuxInfo_v1::TrigNavigationAuxInfo_v1()
      : AuxInfoBase() {

      AUX_VARIABLE( serialized );

   }

} // namespace xAOD
