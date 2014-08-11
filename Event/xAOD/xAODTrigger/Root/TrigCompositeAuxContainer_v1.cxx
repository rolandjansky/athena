/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$

// Local include(s):
#include "xAODTrigger/versions/TrigCompositeAuxContainer_v1.h"

namespace xAOD {

   TrigCompositeAuxContainer_v1::TrigCompositeAuxContainer_v1()
      : ByteStreamAuxContainer_v1(){

      AUX_VARIABLE( name );

      AUX_VARIABLE( linkColName );
      AUX_VARIABLE( linkColIndex );
   }

} // namespace xAOD
