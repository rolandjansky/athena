/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BCMRawDataAuxContainer_v1.cxx 652111 2015-03-06 10:13:22Z krasznaa $

// Local include(s):
#include "xAODLuminosity/versions/BCMRawDataAuxContainer_v1.h"

namespace xAOD {

   BCMRawDataAuxContainer_v1::BCMRawDataAuxContainer_v1()
   : AuxContainerBase() {

      AUX_VARIABLE( bcmWord1 );
      AUX_VARIABLE( bcmWord2 );
   }

} // namespace xAOD
