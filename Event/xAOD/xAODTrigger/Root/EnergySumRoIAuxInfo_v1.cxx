/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EnergySumRoIAuxInfo_v1.cxx 579914 2014-01-24 10:15:15Z krasznaa $

// Local includes
#include "xAODTrigger/versions/EnergySumRoIAuxInfo_v1.h"

namespace xAOD {

   EnergySumRoIAuxInfo_v1::EnergySumRoIAuxInfo_v1()
      : AuxInfoBase() {

      AUX_VARIABLE( roiWord0 );
      AUX_VARIABLE( roiWord1 );
      AUX_VARIABLE( roiWord2 );

      AUX_VARIABLE( thrNames );

      AUX_VARIABLE( energyX );
      AUX_VARIABLE( energyY );
      AUX_VARIABLE( energyT );
   }

} // namespace xAOD
