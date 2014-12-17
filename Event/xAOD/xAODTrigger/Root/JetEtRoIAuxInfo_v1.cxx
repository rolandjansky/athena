/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JetEtRoIAuxInfo_v1.cxx 634618 2014-12-09 02:37:22Z ssnyder $

// Local include(s):
#include "xAODTrigger/versions/JetEtRoIAuxInfo_v1.h"

namespace xAOD {

   JetEtRoIAuxInfo_v1::JetEtRoIAuxInfo_v1()
      : AuxInfoBase() {

      roiWord = 0;
      AUX_VARIABLE( roiWord );
      AUX_VARIABLE( thrNames );
   }

} // namespace xAOD
