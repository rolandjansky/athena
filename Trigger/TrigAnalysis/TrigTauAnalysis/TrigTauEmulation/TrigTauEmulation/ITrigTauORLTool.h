/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ITRIGTAUORLTOOL_TRIGTAUORLTOOL_H
#define ITRIGTAUORLTOOL_TRIGTAUORLTOOL_H

#include "AsgTools/IAsgTool.h"

//edm includes
#include "xAODTrigger/EmTauRoIContainer.h"
#include "xAODTrigger/JetRoIContainer.h"


class ITrigTauORLTool : public virtual asg::IAsgTool
{

   ASG_TOOL_INTERFACE(ITrigTauORLTool)
    
 public:

  virtual StatusCode execute(const xAOD::EmTauRoIContainer* c1, 
			     const xAOD::EmTauRoIContainer* c2,
			     const std::string & sel1 = "",
			     const std::string & sel2 = "") = 0;

  virtual StatusCode execute(const xAOD::EmTauRoIContainer* c1, 
			     const xAOD::JetRoIContainer* c2,
			     const std::string & sel1 = "",
			     const std::string & sel2 = "") = 0;

  virtual StatusCode execute(const xAOD::EmTauRoIContainer* c1, 
			     const xAOD::EmTauRoIContainer* c2, 
			     const xAOD::JetRoIContainer* c3,
			     const std::string & sel1 = "",
			     const std::string & sel2 = "",
			     const std::string & sel3 = "") = 0;

  virtual bool decision(const std::string & item) = 0;

  virtual ~ITrigTauORLTool() {};


};

#endif
