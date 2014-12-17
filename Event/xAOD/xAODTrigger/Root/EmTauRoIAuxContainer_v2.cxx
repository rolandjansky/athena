/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EmTauRoIAuxContainer_v2.cxx 579914 2014-01-24 10:15:15Z krasznaa $

// Local include(s):
#include "xAODTrigger/versions/EmTauRoIAuxContainer_v2.h"

namespace xAOD {

   EmTauRoIAuxContainer_v2::EmTauRoIAuxContainer_v2()
      : AuxContainerBase() {

      AUX_VARIABLE( eta);
      AUX_VARIABLE( phi);
      AUX_VARIABLE( roiWord);

      AUX_VARIABLE( etScale);

      AUX_VARIABLE( thrPattern);
      AUX_VARIABLE( thrNames);
      AUX_VARIABLE( thrValues);

      AUX_VARIABLE( core);
      AUX_VARIABLE( emClus);
      AUX_VARIABLE( tauClus);
      AUX_VARIABLE( emIsol);
      AUX_VARIABLE( hadIsol);
      AUX_VARIABLE( hadCore);
   }

} // namespace xAOD
