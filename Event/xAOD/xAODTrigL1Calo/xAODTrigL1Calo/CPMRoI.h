/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALO_CPMROI_H
#define XAODTRIGL1CALO_CPMROI_H

// Local include(s):
#include "xAODTrigL1Calo/versions/CPMRoI_v1.h"

/// Namespace holding all the xAOD EDM classes
namespace xAOD {
   /// Define the latest version of the CPMRoI class
   typedef CPMRoI_v1 CPMRoI;

   typedef std::map<int, const CPMRoI*> CPMRoIMap_t;
}

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::CPMRoI, 5414895 , 1 )

#endif // XAODTRIGL1CALO_CPMROI_H
