/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IPARSER_PARSER_H
#define IPARSER_PARSER_H

#include "AsgTools/IAsgTool.h"

//EDM include

class IParser : public virtual asg::IAsgTool
{

   ASG_TOOL_INTERFACE(IParser)
    
 public:


  /* virtual const int get_pass_number(const std::string & item_name) const = 0; */

  virtual ~IParser() {};


};

#endif
