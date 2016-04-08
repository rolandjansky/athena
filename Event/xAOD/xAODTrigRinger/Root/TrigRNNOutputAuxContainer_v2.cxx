/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Local include(s):
#include "xAODTrigRinger/versions/TrigRNNOutputAuxContainer_v2.h"

namespace xAOD {

   TrigRNNOutputAuxContainer_v2::TrigRNNOutputAuxContainer_v2():AuxContainerBase(){
	
      AUX_VARIABLE( rnnDecision );
      AUX_VARIABLE( ringerLink );
	
  }
} // namespace xAOD
