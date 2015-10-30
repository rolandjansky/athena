//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <array>
#include <cstdint>
#include "eformat/SourceIdentifier.h"

namespace
{
  // ROBs that should go into the L1R
  constexpr std::array<uint32_t, 12> L1R_ROBS =
    {{
      0x7300a8, 0x7300a9, 0x7300aa, 0x7300ab, // TDAQ_CALO_CLUSTER_PROC_ROI ROBs
      0x7500ac, 0x7500ad,                     // TDAQ_CALO_JET_PROC_ROI ROBs
      0x760001,                               // TDAQ_MUON_CTP_INTERFACE ROB
      0x770001,                               // TDAQ_CTP ROB
      0x910081, 0x910091, 0x910082, 0x910092  // TDAQ_CALO_TOPO_PROC ROBs
    }};

  constexpr std::array<eformat::SubDetector, 1> L1R_SDS =
    {{eformat::TDAQ_CALO_FEAT_EXTRACT_ROI}};
}
