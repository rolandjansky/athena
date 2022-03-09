/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// Local include(s):
#include "xAODTrigger/versions/gFexGlobalRoIAuxContainer_v1.h"

namespace xAOD {

   gFexGlobalRoIAuxContainer_v1::gFexGlobalRoIAuxContainer_v1()
      : AuxContainerBase() {

      AUX_VARIABLE( word);
      AUX_VARIABLE( tobEtScale);
      AUX_VARIABLE( saturated);
      AUX_VARIABLE( globalType);
      AUX_VARIABLE( statusOne);
      AUX_VARIABLE( statusTwo);
      AUX_VARIABLE( quantityOne);
      AUX_VARIABLE( quantityTwo);

   }

} // namespace xAOD
