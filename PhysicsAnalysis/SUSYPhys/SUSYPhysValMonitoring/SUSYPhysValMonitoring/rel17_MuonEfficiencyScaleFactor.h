/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 07.12.2010, MCP working group
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifndef MuonEfficiencyScaleFactorH
#define MuonEfficiencyScaleFactorH

/////////////////////////////////////
// CLASS MuonEfficiencyScaleFactor //
/////////////////////////////////////

/// \class MuonEfficiencyScaleFactor
///
/// This class is the base class for muon efficiency scale factors.
///
/// \date 07.12.2010
///
/// \author Oliver.Kortner@CERN.CH

//////////////////
// HEADER FILES //
//////////////////

// ROOT //
#include "TLorentzVector.h"

namespace Analysis {
  class MuonEfficiencyScaleFactor {
  public:
    MuonEfficiencyScaleFactor() {}
    virtual ~MuonEfficiencyScaleFactor() {}
    // Methods //
    virtual double scaleFactor(const TLorentzVector & tlv) const = 0;
    ///< Get the efficiency scale factor for the given
    ///< fourmomentum.
    virtual double scaleFactorUncertainty(const TLorentzVector & tlv) const = 0;
    ///< Get the uncertainty of the efficiency scale
    ///< factor for the given fourmomentum.
  };
}

#endif
