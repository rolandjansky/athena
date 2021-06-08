/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// Local includes(s):
#include "xAODTrigger/versions/jFexSumETRoIAuxContainer_v1.h"

namespace xAOD{

  jFexSumETRoIAuxContainer_v1::jFexSumETRoIAuxContainer_v1()
      : AuxContainerBase() {
  
      AUX_VARIABLE(word0);
      AUX_VARIABLE(fpgaNumber);
      AUX_VARIABLE(jFexNumber);
      AUX_VARIABLE(Et_upper);
      AUX_VARIABLE(Et_lower);
      AUX_VARIABLE(Sat_upper);
      AUX_VARIABLE(Sat_lower);
       
  }

}//end of namespace xAOD
