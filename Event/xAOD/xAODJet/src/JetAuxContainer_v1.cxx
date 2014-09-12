/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JetAuxContainer_v1.cxx 571994 2013-11-22 13:06:21Z krasznaa $

// Local include(s):
#include "xAODJet/versions/JetAuxContainer_v1.h"

namespace xAOD {

   JetAuxContainer_v1::JetAuxContainer_v1()
      : AuxContainerBase() {

      AUX_VARIABLE( momentum );
      AUX_VARIABLE( constituents );
      AUX_VARIABLE( constituentWeights );
   }

} // namespace xAOD
