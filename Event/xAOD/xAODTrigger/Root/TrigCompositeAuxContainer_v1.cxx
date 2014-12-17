/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigCompositeAuxContainer_v1.cxx 631318 2014-11-26 18:32:04Z krasznaa $

// Local include(s):
#include "xAODTrigger/versions/TrigCompositeAuxContainer_v1.h"

namespace xAOD {

   TrigCompositeAuxContainer_v1::TrigCompositeAuxContainer_v1()
      : ByteStreamAuxContainer_v1(){

      AUX_VARIABLE( name );

      AUX_VARIABLE( linkColNames );
      AUX_VARIABLE( linkColKeys );
      AUX_VARIABLE( linkColIndices );
      AUX_VARIABLE( linkColClids );
   }

} // namespace xAOD
