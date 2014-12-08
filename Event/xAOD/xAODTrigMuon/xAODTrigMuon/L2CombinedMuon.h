/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// L2CombinedMuon.h giagu
#ifndef XAODTRIGMUON_L2COMBINEDMUON_H
#define XAODTRIGMUON_L2COMBINEDMUON_H

// Local include(s):
#include "xAODTrigMuon/versions/L2CombinedMuon_v1.h"

/// Namespace holding all the xAOD EDM classes
namespace xAOD {
  /// Define the latest version of the muon CB class
  typedef L2CombinedMuon_v1 L2CombinedMuon;
}

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::L2CombinedMuon, 221697629 , 1 )

#endif // XAODTRIGMUON_L2COMBINEDMUON_H
