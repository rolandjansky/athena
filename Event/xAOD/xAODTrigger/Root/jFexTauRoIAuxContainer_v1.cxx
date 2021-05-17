/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// Local includes(s):
#include "xAODTrigger/versions/jFexTauRoIAuxContainer_v1.h"

namespace xAOD{

  jFexTauRoIAuxContainer_v1::jFexTauRoIAuxContainer_v1()
      : AuxContainerBase() {
  
      AUX_VARIABLE(word0);
      AUX_VARIABLE(jFexNumber);
      AUX_VARIABLE(fpgaNumber); 
      AUX_VARIABLE(tobEt);
      AUX_VARIABLE(iEta);
      AUX_VARIABLE(iPhi);     
      AUX_VARIABLE(satFlag);
      AUX_VARIABLE(globalEta);
      AUX_VARIABLE(globalPhi);
  }

}//end of namespace xAOD
