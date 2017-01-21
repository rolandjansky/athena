/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALO_CMMCPHITS_H
#define XAODTRIGL1CALO_CMMCPHITS_H

// Local include(s):
#include "xAODTrigL1Calo/versions/CMMCPHits_v1.h"

/// Namespace holding all the xAOD EDM classes
namespace xAOD {
   /// Define the latest version of the CMMCPHits class
   typedef CMMCPHits_v1 CMMCPHits;

   typedef std::map<int, const CMMCPHits*> CMMCPHitsMap_t;
}

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::CMMCPHits, 1938820 , 1 )

#endif // XAODTRIGL1CALO_CMMCPHITS_H
