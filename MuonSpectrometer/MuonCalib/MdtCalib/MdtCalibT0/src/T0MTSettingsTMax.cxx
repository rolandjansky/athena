/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "MdtCalibT0/T0MTSettingsTMax.h"

namespace MuonCalib {

T0MTSettingsTMax :: T0MTSettingsTMax() :	m_vbh_bin_content_rel(4.0),
						m_max_bin_width(40.0),
						m_vbh_low(200.0),
						m_end_min(500.0),
						m_end_max(1000.0),
						m_dist_background(80.0),
						m_min_background_bins(10),
						m_dist_ab(50.0),
						m_width_ab(300.0)
	{
	}

}
