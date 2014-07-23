/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 15.02.2011, MCP working group
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifndef MuidCBScaleEffFactorH
#define MuidCBScaleEffFactorH

//////////////////////////////////
// CLASS StacoCBScaleEffFactors //
//////////////////////////////////

/// \class StacoCBScaleEffFactors
///
/// This class provides efficiency scale factors and their uncertainties
/// for physics analyses using STACO combined muons only. A separate class
/// must be used if the analysis uses Muid combined and tagged muons.
///
/// For release 16 reprocessed data.
///
/// \date 15.02.2011
///
/// \author Oliver.Kortner@CERN.CH

//////////////////
// HEADER FILES //
//////////////////

// ROOT //
#include "TLorentzVector.h"

// STL //
#include <vector>

// Base class //
#include "rel17_MuonEfficiencyScaleFactor.h"

// auxiliary class //
#include "rel17_EtaPhiBinning.h"

namespace Analysis {
  class StacoCBScaleEffFactors : public MuonEfficiencyScaleFactor {
  public:
    //! Constructor
    StacoCBScaleEffFactors(void);
    //! Default constructor
    virtual ~StacoCBScaleEffFactors() {}

    // Methods //
    double scaleFactor(const TLorentzVector & tlv) const;
    ///< Get the efficiency scale factor for the given
    ///< fourmomentum.
    double scaleFactorUncertainty(const TLorentzVector & tlv) const;
    ///< Get the uncertainty of the efficiency scale
    ///< factor for the given fourmomentum.

  private:
    std::vector<double> m_scale_factor;
    std::vector<double> m_scale_factor_uncertainty;

    EtaPhiBinning m_eta_phi_binning; // auxiliary binning class
  };
}

#endif
