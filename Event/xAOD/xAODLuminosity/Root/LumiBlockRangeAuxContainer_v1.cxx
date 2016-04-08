/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: LumiBlockRangeAuxContainer_v1.cxx 647501 2015-02-17 14:22:33Z krasznaa $

// Local include(s):
#include "xAODLuminosity/versions/LumiBlockRangeAuxContainer_v1.h"

namespace xAOD {

   LumiBlockRangeAuxContainer_v1::LumiBlockRangeAuxContainer_v1()
      : AuxContainerBase() {

      AUX_VARIABLE( startRunNumber );
      AUX_VARIABLE( startLumiBlockNumber );

      AUX_VARIABLE( stopRunNumber );
      AUX_VARIABLE( stopLumiBlockNumber );

      AUX_VARIABLE( eventsExpected );
      AUX_VARIABLE( eventsSeen );
   }

} // namespace xAOD
