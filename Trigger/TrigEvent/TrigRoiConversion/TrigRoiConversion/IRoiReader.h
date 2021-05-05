/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGROICONVERSION_IROIREADER_H
#define TRIGROICONVERSION_IROIREADER_H

#include <string>
#include <vector>
#include <iostream>

#include "AsgTools/IAsgTool.h"


class IRoiReader : public virtual asg::IAsgTool {

  ASG_TOOL_INTERFACE( IRoiReader )

public:

  virtual void execute( std::vector<std::string>& keys ) = 0;

};

inline std::ostream& operator<<( std::ostream& s, const IRoiReader& ) { 
  return s;
}


#endif
