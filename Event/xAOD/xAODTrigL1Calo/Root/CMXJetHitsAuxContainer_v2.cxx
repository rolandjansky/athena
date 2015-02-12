/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CMXJetHitsAuxContainer_v2.cxx 646335 2015-02-12 01:16:10Z morrisj $

// Local include(s):
#include "xAODTrigL1Calo/versions/CMXJetHitsAuxContainer_v2.h"

namespace xAOD{

  CMXJetHitsAuxContainer_v2::CMXJetHitsAuxContainer_v2() :
    AuxContainerBase()
  {
    AUX_VARIABLE( crate );
    AUX_VARIABLE( source );
    AUX_VARIABLE( peak );
    AUX_VARIABLE( hitsVec0 );
    AUX_VARIABLE( hitsVec1 );
    AUX_VARIABLE( errorVec0 );
    AUX_VARIABLE( errorVec1 );
  }

} // namespace xAOD

