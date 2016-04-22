// emacs: this is -*- c++ -*-
//
//   @file    IRoiReader.h        
//
//                   
//  
//   Copyright (C) 2016 M.Sutton (sutt@cern.ch)    
//
//   $Id: IRoiReader.h, v0.0   Sun 24 Jan 2016 16:40:31 CET sutt $


#ifndef  IROIREADER_H
#define  IROIREADER_H

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


#endif  // IROIREADER_H 
