/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JetRoIAuxContainer_v2.cxx 579914 2014-01-24 10:15:15Z krasznaa $

// Local include(s):
#include "xAODTrigger/versions/JetRoIAuxContainer_v2.h"

namespace xAOD {

   JetRoIAuxContainer_v2::JetRoIAuxContainer_v2()
      : AuxContainerBase() {

      AUX_VARIABLE( eta );
      AUX_VARIABLE( phi );
      AUX_VARIABLE( roiWord );

      AUX_VARIABLE( etScale );

      AUX_VARIABLE( thrPattern );
      AUX_VARIABLE( thrValues );
      AUX_VARIABLE( thrNames );

      AUX_VARIABLE( et4x4 );
      AUX_VARIABLE( et6x6 );
      AUX_VARIABLE( et8x8 );
   }

} // namespace xAOD
