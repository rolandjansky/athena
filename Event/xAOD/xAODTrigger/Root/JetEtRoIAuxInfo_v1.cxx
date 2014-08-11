/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JetEtRoIAuxInfo_v1.cxx 579914 2014-01-24 10:15:15Z krasznaa $

// Local include(s):
#include "xAODTrigger/versions/JetEtRoIAuxInfo_v1.h"

namespace xAOD {

   JetEtRoIAuxInfo_v1::JetEtRoIAuxInfo_v1()
      : AuxInfoBase() {

      AUX_VARIABLE( roiWord );
      AUX_VARIABLE( thrNames );
   }

} // namespace xAOD
