/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "MdtCalibT0/T0MTSettings.h"

namespace MuonCalib {

  T0MTSettings :: T0MTSettings():	
    m_n_bins_adc(1000),
    m_adc_min(-0.5),
    m_adc_max(999.5),
    m_n_bins_time(1722),
    m_time_min(-15.625),
    m_time_max(1330.08),
    m_draw_debug_graphs(false),
    m_add_fitfun(false),
    m_verbose_level(0),
    m_fit_time(true),
    m_min_entries_time(10000),
    m_min_entries_adc(1000),
    m_t0_settings(),
    m_tmax_settings() {
  }
}
