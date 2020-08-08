/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: eFexEMRoIAuxContainer_v1.cxx  2019-07-01 13:04:25Z watsona $

// Local include(s):
#include "xAODTrigger/versions/eFexEMRoIAuxContainer_v1.h"

namespace xAOD {

   eFexEMRoIAuxContainer_v1::eFexEMRoIAuxContainer_v1()
      : AuxContainerBase() {

      AUX_VARIABLE( word0);
      AUX_VARIABLE( word1);
      AUX_VARIABLE( eFexNumber);
      AUX_VARIABLE( retaEnv);
      AUX_VARIABLE( retaCore);
      AUX_VARIABLE( rhadHad);
      AUX_VARIABLE( rhadEM);
      AUX_VARIABLE( wstotNumerator);
      AUX_VARIABLE( wstotDenominator);
      AUX_VARIABLE( et);
      AUX_VARIABLE( eta);
      AUX_VARIABLE( phi);
      AUX_VARIABLE( isTOB);
      AUX_VARIABLE( thrPattern);
   }

} // namespace xAOD
