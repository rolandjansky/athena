/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: L2CombinedMuonAuxContainer_v1.cxx 605591 2014-07-09 12:07:31Z krasznaa $

// Local include(s):
#include "xAODTrigMuon/versions/L2CombinedMuonAuxContainer_v1.h"

namespace xAOD {

   L2CombinedMuonAuxContainer_v1::L2CombinedMuonAuxContainer_v1()
      : AuxContainerBase() {

      AUX_VARIABLE( pt );
      AUX_VARIABLE( phi );
      AUX_VARIABLE( eta );

      AUX_VARIABLE( charge );
      AUX_VARIABLE( strategy );
      AUX_VARIABLE( errorFlag );
      AUX_VARIABLE( matchFlag );
      AUX_VARIABLE( sigmaPt );

      AUX_VARIABLE( idTrackLink );
      AUX_VARIABLE( muSATrackLink );
   }

} // namespace xAOD
