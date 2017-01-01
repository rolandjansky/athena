/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALO_JEMETSUMS_H
#define XAODTRIGL1CALO_JEMETSUMS_H

// Local include(s):
#include "xAODTrigL1Calo/versions/JEMEtSums_v1.h"
#include "xAODTrigL1Calo/versions/JEMEtSums_v2.h"

/// Namespace holding all the xAOD EDM classes
namespace xAOD {
   /// Define the latest version of the JEMEtSums class
   typedef JEMEtSums_v2 JEMEtSums;

   typedef std::map<int, const JEMEtSums*> JEMEtSumsMap_t;
}

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::JEMEtSums, 168396611 , 1 )

#endif // XAODTRIGL1CALO_JEMETSUMS_H
