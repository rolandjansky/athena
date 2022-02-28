/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// Local includes(s):
#include "xAODTrigger/versions/jFexSRJetRoIAuxContainer_v1.h"

namespace xAOD{

  jFexSRJetRoIAuxContainer_v1::jFexSRJetRoIAuxContainer_v1()
      : AuxContainerBase() {
  
      AUX_VARIABLE(tobWord);
      AUX_VARIABLE(jFexNumber);
      AUX_VARIABLE(fpgaNumber);
      AUX_VARIABLE(tobEt);
      AUX_VARIABLE(tobLocalEta);
      AUX_VARIABLE(tobLocalPhi);     
      AUX_VARIABLE(tobSat);
      AUX_VARIABLE(globalEta);
      AUX_VARIABLE(eta);
      AUX_VARIABLE(globalPhi);
      AUX_VARIABLE(phi);
      AUX_VARIABLE(tobEtScale);
  }

}//end of namespace xAOD
