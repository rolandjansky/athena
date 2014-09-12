/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JetAuxContainer_v1.cxx 596642 2014-05-12 16:54:56Z delsart $

// Local include(s):
#include "xAODJet/versions/JetAuxContainer_v1.h"

namespace xAOD {

   JetAuxContainer_v1::JetAuxContainer_v1()
      : AuxContainerBase() {

     //AUX_VARIABLE( momentum );
     AUX_VARIABLE( constituentLinks );
     AUX_VARIABLE( constituentWeights );
     
     AUX_VARIABLE( pt );
     AUX_VARIABLE( eta );
     AUX_VARIABLE( phi );
     AUX_VARIABLE( m );

   }

} // namespace xAOD
