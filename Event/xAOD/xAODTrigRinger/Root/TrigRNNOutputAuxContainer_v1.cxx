/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Local include(s):
#include "xAODTrigRinger/versions/TrigRNNOutputAuxContainer_v1.h"

namespace xAOD {

   TrigRNNOutputAuxContainer_v1::TrigRNNOutputAuxContainer_v1():AuxContainerBase(){
	
      AUX_VARIABLE( decision );
      AUX_VARIABLE( RoIword );
      AUX_VARIABLE( et );
	
  }
} // namespace xAOD
