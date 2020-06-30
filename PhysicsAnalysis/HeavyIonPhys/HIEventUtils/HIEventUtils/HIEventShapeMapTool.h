/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HIEVENTUTILS_HIEVENTSHAPEMAPTOOL_H
#define HIEVENTUTILS_HIEVENTSHAPEMAPTOOL_H

#include "xAODHIEvent/HIEventShape.h"
#include "HIEventUtils/HIEventShapeIndex.h"
#include "HIEventUtils/IHIEventShapeMapTool.h"
#include "AsgTools/IAsgTool.h"
#include "AsgTools/AsgTool.h"
#include <string>
#include <map>

class HIEventShapeMapTool : virtual public asg::AsgTool
{
  ASG_TOOL_CLASS(HIEventShapeMapTool,IHIEventShapeMapTool)
  public:

  HIEventShapeMapTool();
  virtual ~HIEventShapeMapTool() {};
  virtual StatusCode initialize() override;
  virtual const HIEventShapeIndex* getIndex(std::string key) override;
  virtual bool hasKey(std::string key) override;

  private:
  Gaudi::Property< string > m_containerName { this, "ContainerName", "", "use Clusters boolean switch" }  ;

};
#endif
