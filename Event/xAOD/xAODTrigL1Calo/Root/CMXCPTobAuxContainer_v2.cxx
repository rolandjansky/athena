/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CMXCPTobAuxContainer_v2.cxx 646335 2015-02-12 01:16:10Z morrisj $

// Local include(s):
#include "xAODTrigL1Calo/versions/CMXCPTobAuxContainer_v2.h"

namespace xAOD{

  CMXCPTobAuxContainer_v2::CMXCPTobAuxContainer_v2() :
    AuxContainerBase()
  {
    AUX_VARIABLE( crate );
    AUX_VARIABLE( cmx );
    AUX_VARIABLE( cpm );
    AUX_VARIABLE( chip );
    AUX_VARIABLE( location );
    AUX_VARIABLE( peak );
    AUX_VARIABLE( energyVec );
    AUX_VARIABLE( isolationVec );
    AUX_VARIABLE( errorVec );
    AUX_VARIABLE( presenceMapVec );
  }

} // namespace xAOD

