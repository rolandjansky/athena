/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: eFexEMRoIAuxContainer_v1.cxx  2019-07-01 13:04:25Z watsona $

// Local include(s):
#include "xAODTrigger/versions/eFexEMRoIAuxContainer_v1.h"

namespace xAOD {

   eFexEMRoIAuxContainer_v1::eFexEMRoIAuxContainer_v1()
      : AuxContainerBase() {

      AUX_VARIABLE( Word0);
      AUX_VARIABLE( Word1);
      AUX_VARIABLE( eFexNumber);
      AUX_VARIABLE( shelfNumber);
      AUX_VARIABLE( RetaEnv);
      AUX_VARIABLE( RetaCore);
      AUX_VARIABLE( RhadHad);
      AUX_VARIABLE( RhadEM);
      AUX_VARIABLE( WstotNumerator);
      AUX_VARIABLE( WstotDenominator);
      AUX_VARIABLE( et);
      AUX_VARIABLE( eta);
      AUX_VARIABLE( phi);
      AUX_VARIABLE( isTOB);
   }

} // namespace xAOD
