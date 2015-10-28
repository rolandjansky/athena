/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EnergySumRoIAuxInfo_v1.cxx 680159 2015-07-03 14:02:33Z krasznaa $

// Local includes
#include "xAODTrigger/versions/EnergySumRoIAuxInfo_v1.h"

namespace xAOD {

   EnergySumRoIAuxInfo_v1::EnergySumRoIAuxInfo_v1()
      : AuxInfoBase(),
        roiWord0( 0 ), roiWord1( 0 ), roiWord2( 0 ),
        thrNames(), energyX( 0 ), energyY( 0 ), energyT( 0 ) {

      AUX_VARIABLE( roiWord0 );
      AUX_VARIABLE( roiWord1 );
      AUX_VARIABLE( roiWord2 );

      AUX_VARIABLE( thrNames );

      AUX_VARIABLE( energyX );
      AUX_VARIABLE( energyY );
      AUX_VARIABLE( energyT );
   }

} // namespace xAOD
