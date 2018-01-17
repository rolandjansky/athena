/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FORWARDTRACKER_CONDIFGDATA_H
#define FORWARDTRACKER_CONDIFGDATA_H

#include <ostream>

namespace ForwardTracker {

  class ConfigData {
  public:
    
    std::string twissFile1;
    std::string twissFile2;
    double      positionC1;
    double      positionC2;
    double      apertureC1;
    double      apertureC2;
    double      endMarker;

    ConfigData(); 

    std::string str() const;
  };
  
  std::ostream& operator<<(std::ostream&, const ConfigData&);
}

#endif
