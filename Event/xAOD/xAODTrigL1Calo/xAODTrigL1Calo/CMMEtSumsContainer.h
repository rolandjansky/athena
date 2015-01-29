/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALO_CMMETSUMSCONTAINER_H
#define XAODTRIGL1CALO_CMMETSUMSCONTAINER_H

// Local include(s):
#include "xAODTrigL1Calo/CMMEtSums.h"
#include "xAODTrigL1Calo/versions/CMMEtSumsContainer_v1.h"

namespace xAOD {
   /// Define the latest version of the CMMEtSums container
   typedef CMMEtSumsContainer_v1 CMMEtSumsContainer;
}

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::CMMEtSumsContainer, 1175845346 , 1 )

#endif // XAODTRIGL1CALO_CMMETSUMSCONTAINER_H
