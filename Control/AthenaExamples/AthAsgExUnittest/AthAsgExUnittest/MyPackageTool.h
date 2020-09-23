// -*- mode: c++ -*-
//
//  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//
#ifndef ATHASGEXUNITTEST_MYPACKAGETOOL_H
#define ATHASGEXUNITTEST_MYPACKAGETOOL_H 1

#include "AsgTools/AsgTool.h"
#include "AthAsgExUnittest/IMyPackageTool.h"

class MyPackageTool: public asg::AsgTool, public virtual IMyPackageTool {
public:

  ASG_TOOL_CLASS( MyPackageTool, IMyPackageTool )
  // Add another constructor for non-athena use cases
  MyPackageTool( const std::string& name );

  // Initialize is required by AsgTool base class
  virtual StatusCode initialize() override;

  // This tools method
  virtual double useTheProperty() override;

private:

  double m_nProperty;
  unsigned int m_enumProperty;

};

#endif //> !ATHASGEXUNITTEST_MYPACKAGETOOL_H
