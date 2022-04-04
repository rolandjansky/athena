/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 * @package Trigger/TrigAlgorithms/TrigEFMissingET
 * 
 * @brief Enum for EDM status flags
 * @author Jon Burr
 ********************************************************************/

#include <string>

namespace HLT { namespace MET {
  namespace StatusFlag {
    // Right now just taking these from the values in the old code for
    // consistency
    //
    // Use bitshift notation as these represent different bits in the status
    // word. 1 << x is equivalent to 2^x. 
    enum StatusFlag {
      // 1 << 0 free
      BSConvError             = 1u << 1,
      MuonError               = 1u << 2,
      FEBError                = 1u << 3,
      // 1 << 4 free
      ComponentBigMEtSEtRatio = 1u << 5,
      BadComponentEnergy      = 1u << 6,
      BadEnergyRatio          = 1u << 7,
      NoisyEnergyRatio        = 1u << 8,
      BadCellQuality          = 1u << 9,
      BadCellEnergy           = 1u << 10,
      BadCellTime             = 1u << 11,
      NoMuonTrack             = 1u << 12,
      // 1 << 13 free
      // 1 << 14 free
      ComponentErrors         = 1u << 15,
      // everything until 1 << 27 free
      GlobalNoisyEnergyRatio  = 1u << 27,
      BadEMFraction           = 1u << 28,
      GlobalBigMEtSEtRatio    = 1u << 29,
      ObjectInCrack           = 1u << 30,
      GlobalErrors            = 1u << 31
    }; //> end enum StatusFlag
    std::string flagIndexToString(int idx);
  } //> end namespace StatusFlag
} } //> end namespace HLT::MET
