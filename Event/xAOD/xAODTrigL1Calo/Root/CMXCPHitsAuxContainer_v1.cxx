/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CMXCPHitsAuxContainer_v1.cxx 663426 2015-04-28 11:45:23Z amazurov $

// Local include(s):
#include "xAODTrigL1Calo/versions/CMXCPHitsAuxContainer_v1.h"

namespace xAOD{

  CMXCPHitsAuxContainer_v1::CMXCPHitsAuxContainer_v1() :
    AuxContainerBase()
  {
    AUX_VARIABLE( crate );
    AUX_VARIABLE( cmx );
    AUX_VARIABLE( sourceComponent );
    AUX_VARIABLE( peak );
    AUX_VARIABLE( hitsVec0 );
    AUX_VARIABLE( hitsVec1 );
    AUX_VARIABLE( errorVec0 );
    AUX_VARIABLE( errorVec1 );
  }

} // namespace xAOD

