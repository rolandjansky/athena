/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: L1TopoSimResults.h 2021-10-06 @asonay $
#ifndef XAODL1TOPOSIMRESULTS_H
#define XAODL1TOPOSIMRESULTS_H

// Local include(s):
#include "xAODTrigger/versions/L1TopoSimResults_v1.h"

// Namespace holding all the xAOD EDM classes
namespace xAOD{
  /// Define the latest version of the L1TopoSimResults class
  typedef L1TopoSimResults_v1 L1TopoSimResults;
}

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::L1TopoSimResults , 14350674 , 1 )
#endif 
