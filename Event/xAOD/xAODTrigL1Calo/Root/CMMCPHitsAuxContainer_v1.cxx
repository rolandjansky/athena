/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CMMCPHitsAuxContainer_v1.cxx 579924 2014-01-24 10:26:51Z krasznaa $

// Local include(s):
#include "xAODTrigL1Calo/versions/CMMCPHitsAuxContainer_v1.h"

namespace xAOD {

  CMMCPHitsAuxContainer_v1::CMMCPHitsAuxContainer_v1() :
    AuxContainerBase()
  {
    AUX_VARIABLE( crate );
    AUX_VARIABLE( dataID );
    AUX_VARIABLE( peak );
    AUX_VARIABLE( hitsVec0 );
    AUX_VARIABLE( hitsVec1 );
    AUX_VARIABLE( errorVec0 );
    AUX_VARIABLE( errorVec1 );
  }

} // namespace xAOD
