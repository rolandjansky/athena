/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// Local includes(s):
#include "xAODTrigger/versions/jFexMETRoIAuxContainer_v1.h"

namespace xAOD{

  jFexMETRoIAuxContainer_v1::jFexMETRoIAuxContainer_v1()
      : AuxContainerBase() {
  
      AUX_VARIABLE(tobWord);
      AUX_VARIABLE(jFexNumber);
      AUX_VARIABLE(fpgaNumber);
      AUX_VARIABLE(tobEx);
      AUX_VARIABLE(tobEy); 
      AUX_VARIABLE(tobRes);
      AUX_VARIABLE(tobSat);
  }

}//end of namespace xAOD
