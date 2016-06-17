/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EnergySumRoIAuxInfo_v2.cxx 680159 2015-07-03 14:02:33Z krasznaa $

// Local includes
#include "xAODTrigger/versions/EnergySumRoIAuxInfo_v2.h"

namespace xAOD {

   EnergySumRoIAuxInfo_v2::EnergySumRoIAuxInfo_v2()
      : AuxInfoBase(),
        roiWord0( 0 ), roiWord1( 0 ), roiWord2( 0 ),
        roiWord3( 0 ), roiWord4( 0 ), roiWord5( 0 ),
        thrNames(), 
        energyX( 0 ), energyY( 0 ), energyT( 0 ),
        energyXRestricted( 0 ), energyYRestricted( 0 ), energyTRestricted( 0 ) {

      AUX_VARIABLE( roiWord0 );
      AUX_VARIABLE( roiWord1 );
      AUX_VARIABLE( roiWord2 );
      AUX_VARIABLE( roiWord3 );
      AUX_VARIABLE( roiWord4 );
      AUX_VARIABLE( roiWord5 );

      AUX_VARIABLE( thrNames );

      AUX_VARIABLE( energyX );
      AUX_VARIABLE( energyY );
      AUX_VARIABLE( energyT );
      AUX_VARIABLE( energyXRestricted );
      AUX_VARIABLE( energyYRestricted );
      AUX_VARIABLE( energyTRestricted );
   }

} // namespace xAOD
