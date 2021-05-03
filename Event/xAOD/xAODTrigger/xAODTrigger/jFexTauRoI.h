/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODJFEXTAUROI_H
#define XAODJFEXTAUROI_H

// Local include(s):
#include "xAODTrigger/versions/jFexTauRoI_v1.h"
// Namespace holding all the xAOD EDM classes
namespace xAOD{
  /// Define the latest version of the jFexSRJetRoI class  
  typedef jFexTauRoI_v1 jFexTauRoI;
}

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF(xAOD::jFexTauRoI , 145970809 , 1 )
#endif //XAODJFEXTAUROI_H
