/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CMXEtSumsAuxContainer_v1.cxx 663426 2015-04-28 11:45:23Z amazurov $

// Local include(s):
#include "xAODTrigL1Calo/versions/CMXEtSumsAuxContainer_v1.h"

namespace xAOD{

  CMXEtSumsAuxContainer_v1::CMXEtSumsAuxContainer_v1() :
    AuxContainerBase()
  {
    AUX_VARIABLE( crate );
    AUX_VARIABLE( sourceComponent );
    AUX_VARIABLE( peak );
    AUX_VARIABLE( etVec );
    AUX_VARIABLE( exVec );
    AUX_VARIABLE( eyVec );
    AUX_VARIABLE( etErrorVec );
    AUX_VARIABLE( exErrorVec );
    AUX_VARIABLE( eyErrorVec );
  }

} // namespace xAOD

