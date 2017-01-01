/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALO_CPMHITS_H
#define XAODTRIGL1CALO_CPMHITS_H

// Local include(s):
#include "xAODTrigL1Calo/versions/CPMHits_v1.h"

/// Namespace holding all the xAOD EDM classes
namespace xAOD {
   /// Define the latest version of the CPMHits class
   typedef CPMHits_v1 CPMHits;

   typedef std::map<int, const CPMHits*> CPMHitsMap_t;
}

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::CPMHits , 39223308 , 1 )

#endif // XAODTRIGL1CALO_CPMHITS_H
