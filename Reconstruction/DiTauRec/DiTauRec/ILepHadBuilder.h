/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DITAUREC_ILEPHADBUILDER_H
#define DITAUREC_ILEPHADBUILDER_H

#include "AsgTools/IAsgTool.h"
namespace DiTauRec
{
  class ILepHadBuilder :
    public virtual asg::IAsgTool
  {
  
    // Declare the interface that the class provides
    ASG_TOOL_INTERFACE( DiTauRec::ILepHadBuilder )
  
    public:
    // initialize the tool
    virtual StatusCode initialize() = 0;

    // set pointer to event
    virtual StatusCode initializeEvent() = 0;

    virtual StatusCode execute() = 0;
  
  }; //class ILepHadBuilder
} // namespace DiTauRec


#endif // DITAUREC_ILEPHADBUILDER_H
