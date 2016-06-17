/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JetEtRoIAuxInfo_v1.cxx 680159 2015-07-03 14:02:33Z krasznaa $

// Local include(s):
#include "xAODTrigger/versions/JetEtRoIAuxInfo_v1.h"

namespace xAOD {

   JetEtRoIAuxInfo_v1::JetEtRoIAuxInfo_v1()
      : AuxInfoBase(),
        roiWord( 0 ), thrNames() {

      AUX_VARIABLE( roiWord );
      AUX_VARIABLE( thrNames );
   }

} // namespace xAOD
