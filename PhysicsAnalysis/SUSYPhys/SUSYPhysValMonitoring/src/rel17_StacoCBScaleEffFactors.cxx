/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 15.02.2011, MCP working group
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//////////////////
// HEADER FILES //
//////////////////

#include "SUSYPhysValMonitoring/rel17_StacoCBScaleEffFactors.h"

//*****************************************************************************

/////////////////
// CONSTRUCTOR //
/////////////////

Analysis::StacoCBScaleEffFactors::StacoCBScaleEffFactors(void) {

////////////////////////////////////////////////////////////////
// FILL THE SCALE FACTOR AND SCALE FACTOR UNCERTAINTY VECTORS //
////////////////////////////////////////////////////////////////

    m_scale_factor = std::vector<double>(10);
    m_scale_factor_uncertainty = std::vector<double>(10);

    m_scale_factor[0] = 1.0070;
    m_scale_factor[1] = 0.9950;
    m_scale_factor[2] = 0.9880;
    m_scale_factor[3] = 0.9829;
    m_scale_factor[4] = 0.9274;
    m_scale_factor[5] = 0.9890;
    m_scale_factor[6] = 0.9930;
    m_scale_factor[7] = 0.9890;
    m_scale_factor[8] = 1.0090;
    m_scale_factor[9] = 0.9990;

    m_scale_factor_uncertainty[0] = 0.0063;
    m_scale_factor_uncertainty[1] = 0.0072;
    m_scale_factor_uncertainty[2] = 0.0081;
    m_scale_factor_uncertainty[3] = 0.0136;
    m_scale_factor_uncertainty[4] = 0.0121;
    m_scale_factor_uncertainty[5] = 0.0076;
    m_scale_factor_uncertainty[6] = 0.0076;
    m_scale_factor_uncertainty[7] = 0.0103;
    m_scale_factor_uncertainty[8] = 0.0103;
    m_scale_factor_uncertainty[9] = 0.0072;

}

//*****************************************************************************

////////////////////////
// METHOD scaleFactor //
////////////////////////

double Analysis::StacoCBScaleEffFactors::scaleFactor(const TLorentzVector & tlv) const {

    int bin(m_eta_phi_binning.symmetricBin(&tlv)-1);
    if(bin<0) return 1.; //UNKNOWN BIN
    return m_scale_factor[bin];

}

//*****************************************************************************

///////////////////////////////////
// METHOD scaleFactorUncertainty //
///////////////////////////////////

double Analysis::StacoCBScaleEffFactors::scaleFactorUncertainty(
                                            const TLorentzVector & tlv) const {

    int bin(m_eta_phi_binning.symmetricBin(&tlv)-1);
    if(bin<0) return 1.; //UNKNOWN BIN
    return m_scale_factor_uncertainty[bin];

}
