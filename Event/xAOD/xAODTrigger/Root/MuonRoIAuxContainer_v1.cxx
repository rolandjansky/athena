/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MuonRoIAuxContainer_v1.cxx 579914 2014-01-24 10:15:15Z krasznaa $

// Local include(s):
#include "xAODTrigger/versions/MuonRoIAuxContainer_v1.h"

namespace xAOD {

   MuonRoIAuxContainer_v1::MuonRoIAuxContainer_v1()
      : AuxContainerBase() {

      AUX_VARIABLE( eta );
      AUX_VARIABLE( phi );
      AUX_VARIABLE( roiWord );
      AUX_VARIABLE( thrName );
      AUX_VARIABLE( thrValue );
   }

} // namespace xAOD
