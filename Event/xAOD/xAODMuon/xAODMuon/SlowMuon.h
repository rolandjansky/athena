/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODMUON_SLOWMUON_H
#define XAODMUON_SLOWMUON_H

// Local include(s):
#include "versions/SlowMuon_v1.h"

namespace xAOD {
  /// Reference the current persistent version:
  typedef SlowMuon_v1 SlowMuon;
}

// Set up a CLID for the class:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::SlowMuon, 182231508, 1 )

#endif // XAODMUON_SLOWMUON_H
