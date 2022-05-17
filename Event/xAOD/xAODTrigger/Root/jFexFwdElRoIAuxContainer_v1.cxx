/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//TODO: add FwdEl specific variables 

// Local includes(s):
#include "xAODTrigger/versions/jFexFwdElRoIAuxContainer_v1.h"

namespace xAOD{

  jFexFwdElRoIAuxContainer_v1::jFexFwdElRoIAuxContainer_v1()
      : AuxContainerBase() {
  
      AUX_VARIABLE(tobWord);
      AUX_VARIABLE(jFexNumber);
      AUX_VARIABLE(fpgaNumber); 
      AUX_VARIABLE(tobEt);
      AUX_VARIABLE(tobEMIso);
      AUX_VARIABLE(tobEMf1);
      AUX_VARIABLE(tobEMf2);
      AUX_VARIABLE(tobLocalEta);
      AUX_VARIABLE(tobLocalPhi);     
      AUX_VARIABLE(tobSat);
      AUX_VARIABLE(globalEta);
      AUX_VARIABLE(globalPhi);
      AUX_VARIABLE(eta);
      AUX_VARIABLE(phi);
      AUX_VARIABLE(tobEtScale);
      AUX_VARIABLE(isTOB);
      AUX_VARIABLE(EtEMiso);
      AUX_VARIABLE(EtEM);
      AUX_VARIABLE(EtHad1);
      AUX_VARIABLE(EtHad2);
  }

}//end of namespace xAOD
