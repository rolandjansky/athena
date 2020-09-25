// -*- mode: c++ -*-

//
//  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
//

#ifndef ATHASGEXUNITTEST_IMYPACKAGETOOL_H
#define ATHASGEXUNITTEST_IMYPACKAGETOOL_H 1

#include "AsgTools/IAsgTool.h"

class IMyPackageTool : public virtual asg::IAsgTool {
public:

  ASG_TOOL_INTERFACE( IMyPackageTool )
  
  //declare enums for properties here too, so that they are accessible through just the interface header
  //example of an enum you might end up using for your properties .. please put enums in a class! (not just a namespace)
  enum MyEnumProperty { Val1=1 , Val2=2 };

  virtual double useTheProperty() = 0;
  
};

#endif //> !ATHASGEXUNITTEST_IMYPACKAGETOOL_H
