/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CMXCPTobAuxContainer_v1.cxx 652811 2015-03-09 22:35:53Z morrisj $

// Local include(s):
#include "xAODTrigL1Calo/versions/CMXCPTobAuxContainer_v1.h"

namespace xAOD{

  CMXCPTobAuxContainer_v1::CMXCPTobAuxContainer_v1() :
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

