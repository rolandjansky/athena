/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigCompositeAuxContainer_v2.cxx $

// Local include(s):
#include "xAODTrigger/versions/TrigCompositeAuxContainer_v2.h"

namespace xAOD {

   TrigCompositeAuxContainer_v2::TrigCompositeAuxContainer_v2()
      : AuxContainerBase(){

      AUX_VARIABLE( name );

      AUX_VARIABLE( linkColNames );
      AUX_VARIABLE( linkColKeys );
      AUX_VARIABLE( linkColIndices );
      AUX_VARIABLE( linkColClids );

      AUX_VARIABLE( decisions );
   }

} // namespace xAOD
