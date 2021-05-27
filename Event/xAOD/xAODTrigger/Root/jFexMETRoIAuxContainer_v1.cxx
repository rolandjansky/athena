/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// Local includes(s):
#include "xAODTrigger/versions/jFexMETRoIAuxContainer_v1.h"

namespace xAOD{

  jFexMETRoIAuxContainer_v1::jFexMETRoIAuxContainer_v1()
      : AuxContainerBase() {
  
      AUX_VARIABLE(word0);
      AUX_VARIABLE(jFexNumber);
      AUX_VARIABLE(fpgaNumber);
      AUX_VARIABLE(E_x);
      AUX_VARIABLE(E_y); 
      AUX_VARIABLE(Res);
      AUX_VARIABLE(Sat);
  }

}//end of namespace xAOD
