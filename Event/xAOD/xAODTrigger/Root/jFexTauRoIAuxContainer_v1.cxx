/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// Local includes(s):
#include "xAODTrigger/versions/jFexTauRoIAuxContainer_v1.h"

namespace xAOD{

  jFexTauRoIAuxContainer_v1::jFexTauRoIAuxContainer_v1()
      : AuxContainerBase() {
  
      AUX_VARIABLE(tobWord);
      AUX_VARIABLE(jFexNumber);
      AUX_VARIABLE(fpgaNumber); 
      AUX_VARIABLE(tobEt);
      AUX_VARIABLE(tobIso);
      AUX_VARIABLE(tobLocalEta);
      AUX_VARIABLE(tobLocalPhi);     
      AUX_VARIABLE(tobSat);
      AUX_VARIABLE(globalEta);
      AUX_VARIABLE(globalPhi);
      AUX_VARIABLE(eta);
      AUX_VARIABLE(phi);
      AUX_VARIABLE(tobEtScale);
  }

}//end of namespace xAOD
