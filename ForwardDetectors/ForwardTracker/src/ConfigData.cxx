/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ForwardTracker/ConfigData.h"

#include <string>
#include <sstream>

namespace ForwardTracker {

  ConfigData::ConfigData() {
    
    twissFile1 = "/afs/cern.ch/atlas/offline/external/TwissFiles/v001/0090.00m/8TeV/alfaTwiss1.txt"; // LHC twiss file for beam1
    twissFile2 = "/afs/cern.ch/atlas/offline/external/TwissFiles/v001/0090.00m/8TeV/alfaTwiss2.txt"; // LHC twiss file for beam2
    positionC1 = 149;                                                                                // Z position [ m] of collimator1
    positionC2 = 184;                                                                                // Z position [ m] of collimator2
    apertureC1 = 999;                                                                                // X aperture [mm] of collimator1
    apertureC2 = 999;                                                                                // X aperture [mm] of collimator2
    endMarker  = 236.888;                                                                            // Z position [ m] of the detector
  }

  std::string ConfigData::str() const {
    
    std::ostringstream ost;
    
    ost << '\n'
	<< "twissFile1 " << twissFile1 << '\n'
 	<< "twissFile2 " << twissFile2 << '\n'
	<< "positionC1 " << positionC1 << '\n'
	<< "positionC2 " << positionC2 << '\n'
	<< "apertureC1 " << apertureC1 << '\n'
	<< "apertureC2 " << apertureC2 << '\n'
	<< "endMarker  " << endMarker  << '\n';
    
    return ost.str();
  } 

  std::ostream& operator<<(std::ostream& os, const ConfigData& cd) { os << cd.str(); return os; }
}
