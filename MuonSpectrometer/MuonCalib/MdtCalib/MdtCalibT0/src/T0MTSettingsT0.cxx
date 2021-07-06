/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MdtCalibT0/T0MTSettingsT0.h"

namespace MuonCalib {

    T0MTSettingsT0 ::T0MTSettingsT0() :
        m_vbh_bin_content_rel(2.0),
        m_max_bin_width(10.0),
        m_min_background_bins(50),
        m_T_start(3.0),
        m_use_top_chi2(true),
        m_scramble_threshold(-1.0),
        m_slicing_chi2(-1.0),
        m_correct_rel_t0s(false) {}
}  // namespace MuonCalib
