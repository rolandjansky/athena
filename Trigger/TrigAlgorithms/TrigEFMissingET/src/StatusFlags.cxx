/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#include "TrigEFMissingET/StatusFlags.h"
#include <stdexcept>

namespace HLT { namespace MET { namespace StatusFlag {
  std::string flagIndexToString(int idx) {
    switch(idx) {
      case 0:
      case 4:
      case 13:
      case 14:
      case 16:
      case 17:
      case 18:
      case 19:
      case 20:
      case 21:
      case 22:
      case 23:
      case 24:
      case 25:
      case 26:
        return "UNUSED";
      case 1:
        return "BSConvError";
      case 2:
        return "MuonError";
      case 3:
        return "FEBError";
      case 5:
        return "ComponentBigMEtSEtRatio";
      case 6:
        return "BadComponentEnergy";
      case 7:
        return "BadEnergyRatio";
      case 8:
        return "NoisyEnergyRatio";
      case 9:
        return "BadCellQuality";
      case 10:
        return "BadCellEnergy";
      case 11:
        return "BadCellTime";
      case 12:
        return "NoMuonTrack";
      case 15:
        return "ComponentErrors";
      case 27:
        return "GlobalNoisyEnergyRatio";
      case 28:
        return "BadEMFraction";
      case 29:
        return "GlobalNoisyEnerygRatio";
      case 30:
        return "ObjectInCrack";
      case 31:
        return "GlobalErrors";
      default:
        throw std::out_of_range("Invalid index provided " + std::to_string(idx) );
        // Make gcc happy
        return "";
    }
  }
} } } //> end namespace HLT::MET::StatusFlag
