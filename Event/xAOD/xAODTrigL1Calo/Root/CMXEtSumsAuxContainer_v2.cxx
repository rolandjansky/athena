/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id

// Local include(s):
#include "xAODTrigL1Calo/versions/CMXEtSumsAuxContainer_v2.h"

namespace xAOD{

  CMXEtSumsAuxContainer_v2::CMXEtSumsAuxContainer_v2() :
    AuxContainerBase()
  {
    AUX_VARIABLE( crate );
    AUX_VARIABLE( source );
    AUX_VARIABLE( peak );
    AUX_VARIABLE( etVec );
    AUX_VARIABLE( exVec );
    AUX_VARIABLE( eyVec );
    AUX_VARIABLE( etErrorVec );
    AUX_VARIABLE( exErrorVec );
    AUX_VARIABLE( eyErrorVec );
  }

} // namespace xAOD

