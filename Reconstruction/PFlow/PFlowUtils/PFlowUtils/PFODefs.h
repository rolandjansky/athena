/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PFOUTILS_PFODEFS_H
#define PFOUTILS_PFODEFS_H

namespace CP{

  enum PFO_JetMETConfig_inputScale { EM = 0, LC };
  enum PFO_JetMETConfig_charge { neutral = 0, charged = 1, all = 2 };

  struct inputScaleMapper {
    bool getValue(const std::string& nameToMapFrom, PFO_JetMETConfig_inputScale& inputScaleToMapTo){
      if ("EM" == nameToMapFrom) {
	inputScaleToMapTo = PFO_JetMETConfig_inputScale::EM;
	return true;
      }
      else if ("LC" == nameToMapFrom) {
	inputScaleToMapTo = PFO_JetMETConfig_inputScale::LC;
	return true;
      }
      return false;
    }
    
  };
  
}

#endif
