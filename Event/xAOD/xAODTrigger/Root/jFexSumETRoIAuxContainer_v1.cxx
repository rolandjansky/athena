/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// Local includes(s):
#include "xAODTrigger/versions/jFexSumETRoIAuxContainer_v1.h"

namespace xAOD{

  jFexSumETRoIAuxContainer_v1::jFexSumETRoIAuxContainer_v1()
      : AuxContainerBase() {
  
      AUX_VARIABLE(tobWord);
      AUX_VARIABLE(fpgaNumber);
      AUX_VARIABLE(jFexNumber);
      AUX_VARIABLE(tobEt_lower);
      AUX_VARIABLE(tobSat_lower);
      AUX_VARIABLE(tobEt_upper);
      AUX_VARIABLE(tobSat_upper);
      AUX_VARIABLE(tobEtScale);
       
  }

}//end of namespace xAOD
