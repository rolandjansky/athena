/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: gFexJetRoIAuxContainer_v1.cxx  2020-12-15  ctosciri $

// Local include(s):
#include "xAODTrigger/versions/gFexJetRoIAuxContainer_v1.h"

namespace xAOD {

   gFexJetRoIAuxContainer_v1::gFexJetRoIAuxContainer_v1()
      : AuxContainerBase() {

      AUX_VARIABLE( word);
      AUX_VARIABLE( gFexType);
      AUX_VARIABLE( tobEt);
      AUX_VARIABLE( iEta);
      AUX_VARIABLE( iPhi);
      AUX_VARIABLE( gFexJetStatus);
      AUX_VARIABLE( saturated);
   }

} // namespace xAOD
