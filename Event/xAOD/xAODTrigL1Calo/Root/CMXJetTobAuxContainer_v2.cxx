/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id

// Local include(s):
#include "xAODTrigL1Calo/versions/CMXJetTobAuxContainer_v2.h"

namespace xAOD{

  CMXJetTobAuxContainer_v2::CMXJetTobAuxContainer_v2() :
    AuxContainerBase()
  {
    AUX_VARIABLE( crate );
    AUX_VARIABLE( jem );
    AUX_VARIABLE( frame );
    AUX_VARIABLE( location );
    AUX_VARIABLE( peak );
    AUX_VARIABLE( energyLargeVec );
    AUX_VARIABLE( energySmallVec );
    AUX_VARIABLE( errorVec );
    AUX_VARIABLE( presenceMapVec );
  }

} // namespace xAOD

