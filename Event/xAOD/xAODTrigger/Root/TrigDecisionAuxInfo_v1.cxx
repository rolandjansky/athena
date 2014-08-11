/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigDecisionAuxInfo_v1.cxx 581968 2014-02-06 16:08:52Z krasznaa $

// Local include(s):
#include "xAODTrigger/versions/TrigDecisionAuxInfo_v1.h"

namespace xAOD {

   TrigDecisionAuxInfo_v1::TrigDecisionAuxInfo_v1()
      : AuxInfoBase() {

      AUX_VARIABLE( smk );

      AUX_VARIABLE( bgCode );
      AUX_VARIABLE( tav );
      AUX_VARIABLE( tap );
      AUX_VARIABLE( tbp );

      AUX_VARIABLE( lvl2ErrorBits );
      AUX_VARIABLE( efErrorBits );
      AUX_VARIABLE( lvl2Truncated );
      AUX_VARIABLE( efTruncated );

      AUX_VARIABLE( lvl2PassedPhysics );
      AUX_VARIABLE( efPassedPhysics );
      AUX_VARIABLE( lvl2PassedRaw );
      AUX_VARIABLE( efPassedRaw );
      AUX_VARIABLE( lvl2PassedThrough );
      AUX_VARIABLE( efPassedThrough );
      AUX_VARIABLE( lvl2Prescaled );
      AUX_VARIABLE( efPrescaled );
      AUX_VARIABLE( lvl2Resurrected );
      AUX_VARIABLE( efResurrected );
   }

} // namespace xAOD
