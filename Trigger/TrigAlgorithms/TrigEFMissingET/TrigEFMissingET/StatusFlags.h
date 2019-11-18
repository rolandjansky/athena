/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
    enum StatusFlag : int {
      // 1 << 0 free
      BSConvError             = 1 << 1,
      MuonError               = 1 << 2,
      FEBError                = 1 << 3,
      // 1 << 4 free
      ComponentBigMEtSEtRatio = 1 << 5,
      BadComponentEnergy      = 1 << 6,
      BadEnergyRatio          = 1 << 7,
      NoisyEnergyRatio        = 1 << 8,
      BadCellQuality          = 1 << 9,
      BadCellEnergy           = 1 << 10,
      BadCellTime             = 1 << 11,
      NoMuonTrack             = 1 << 12,
      // 1 << 13 free
      // 1 << 14 free
      ComponentErrors         = 1 << 15,
      // everything until 1 << 27 free
      GlobalNoisyEnergyRatio  = 1 << 27,
      BadEMFraction           = 1 << 28,
      GlobalBigMEtSEtRatio    = 1 << 29,
      ObjectInCrack           = 1 << 30,
      GlobalErrors            = 1 << 31
    }; //> end enum StatusFlag
    std::string flagIndexToString(int idx);
  } //> end namespace StatusFlag
} } //> end namespace HLT::MET
