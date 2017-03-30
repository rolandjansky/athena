// -*- mode: c++ -*-
#ifndef ATHASGEXUNITTEST_MYPACKAGETOOL_H
#define ATHASGEXUNITTEST_MYPACKAGETOOL_H 1

#include "AsgTools/AsgTool.h"
#include "AthAsgExUnittest/IMyPackageTool.h"

class MyPackageTool: public asg::AsgTool, public virtual IMyPackageTool { 
public: 

  ASG_TOOL_CLASS( MyPackageTool, IMyPackageTool )
  // Add another constructor for non-athena use cases
  MyPackageTool( const std::string& name, bool unittest=false );
  
  // Initialize is required by AsgTool base class
  virtual StatusCode initialize() override;
  virtual double useTheProperty() override;

private: 

  double m_nProperty;
  unsigned int m_enumProperty; 
  bool m_unittest;

}; 

#endif //> !ATHASGEXUNITTEST_MYPACKAGETOOL_H
