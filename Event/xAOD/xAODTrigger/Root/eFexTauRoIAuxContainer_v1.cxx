/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// Local include(s):
#include "xAODTrigger/versions/eFexTauRoIAuxContainer_v1.h"

namespace xAOD {

   eFexTauRoIAuxContainer_v1::eFexTauRoIAuxContainer_v1()
      : AuxContainerBase() {

      AUX_VARIABLE( word0);
      AUX_VARIABLE( word1);
      AUX_VARIABLE( fCoreNumerator);
      AUX_VARIABLE( fCoreDenominator);
      AUX_VARIABLE( fHadNumerator);
      AUX_VARIABLE( fHadDenominator);
      AUX_VARIABLE( et);
      AUX_VARIABLE( eta);
      AUX_VARIABLE( phi);
      AUX_VARIABLE( isTOB);
   }

} // namespace xAOD
