// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BackgroundWordNames.h 596570 2014-05-12 12:32:04Z wlampl $
/**
 * @file BackgroundD3PDMaker/src/BackgroundWordNames.h
 * @author Mark Tibbetts <mark.james.tibbetts@cern.ch>
 * @date Mar, 2011
 * @brief Assign names to background word bits.
 */


#ifndef BACKGROUNDD3PDMAKER_BACKGROUNDWORDNAMES_H
#define BACKGROUNDD3PDMAKER_BACKGROUNDWORDNAMES_H


#include "xAODEventInfo/EventInfo.h"


namespace D3PD {
  
  
  struct BackgroundWordNames
  {
    xAOD::EventInfo::BackgroundEventFlag m_flag;
    const char* m_name;
  };
  


  static BackgroundWordNames backgroundWordNames[] = {
    { xAOD::EventInfo::MBTSTimeDiffHalo,       "MBTSTimeHaloBit" },
    { xAOD::EventInfo::MBTSTimeDiffCol,        "MBTSTimeColBit" },
    { xAOD::EventInfo::LArECTimeDiffHalo,      "LArECTimeHaloBit" },
    { xAOD::EventInfo::LArECTimeDiffCol,       "LArECTimeColBit" },
    { xAOD::EventInfo::PixMultiplicityHuge,    "PixMultiHugeBit" },
    { xAOD::EventInfo::PixSPNonEmpty,          "PixSPNonEmptyBit" },
    { xAOD::EventInfo::SCTMultiplicityHuge,    "SCTMultiHugeBit" },
    { xAOD::EventInfo::SCTSPNonEmpty,          "SCTSPNonEmptyBit" },
    { xAOD::EventInfo::CSCTimeDiffHalo,        "CSCTimeHaloBit" },
    { xAOD::EventInfo::CSCTimeDiffCol,         "CSCTimeColBit" },
    { xAOD::EventInfo::BCMTimeDiffHalo,        "BCMTimeHaloBit" },
    { xAOD::EventInfo::BCMTimeDiffCol,         "BCMTimeColBit" },
    { xAOD::EventInfo::MuonTimingCol,          "MuonTimingColBit" },
    { xAOD::EventInfo::MuonTimingCosmic,       "MuonTimingCosmicBit" },
    { xAOD::EventInfo::MBTSBeamVeto,           "MBTSBeamVetoBit" },
    { xAOD::EventInfo::BCMBeamVeto,            "BCMBeamVetoBit" },
    { xAOD::EventInfo::LUCIDBeamVeto,          "LUCIDBeamVetoBit" },
    { xAOD::EventInfo::HaloMuonSegment,        "HaloMuonSegment" },
    { xAOD::EventInfo::HaloClusterShape,       "HaloClusterShape" },
    { xAOD::EventInfo::HaloMuonOneSided,       "HaloMuonOneSided" },
    { xAOD::EventInfo::HaloMuonTwoSided,       "HaloMuonTwoSided" },
    { xAOD::EventInfo::HaloTileClusterPattern, "HaloTileClusterPattern" },
    { xAOD::EventInfo::BeamGasPixel,           "BeamGasPixel" },
    { xAOD::EventInfo::CosmicStandAlone,       "CosmicStandAlone" },
    { xAOD::EventInfo::CosmicStandAloneTight,  "CosmicStandAloneTight" },
    { xAOD::EventInfo::CosmicCombined,         "CosmicCombined" },
    { xAOD::EventInfo::CosmicCombinedTight,    "CosmicCombinedTight" },
    { xAOD::EventInfo::BkgdResvBit1,           "BkgdResvBit1" },
    { xAOD::EventInfo::BkgdResvBit2,           "BkgdResvBit2"},
    { xAOD::EventInfo::BkgdResvBit3,           "BkgdResvBit3"},
    { xAOD::EventInfo::BkgdResvBit4,           "BkgdResvBit4"},
    { xAOD::EventInfo::BkgdResvBit5,           "BkgdResvBit5"}

  };

  static const size_t nBackgroundWordNames =
    sizeof(backgroundWordNames) / sizeof(backgroundWordNames[0]);
  
  
} // namespace D3PD


#endif // not BACKGROUNDD3PDMAKER_BACKGROUNDWORDNAMES_H
