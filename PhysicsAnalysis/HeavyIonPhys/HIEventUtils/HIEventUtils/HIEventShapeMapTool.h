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

class HIEventShapeMapTool : virtual public asg::AsgTool, virtual public IHIEventShapeMapTool
{
  ASG_TOOL_CLASS(HIEventShapeMapTool,IHIEventShapeMapTool)

  public:

  HIEventShapeMapTool(const std::string& n);
  virtual ~HIEventShapeMapTool() {};

  virtual StatusCode initialize() override;
  virtual StatusCode insert(std::string key, const HIEventShapeIndex& index, bool clobber=false) override;
  virtual const HIEventShapeIndex* getIndex(std::string key) const override;

  virtual bool hasKey(std::string key) override;

  private:
  //This type of tool should be able to retrieve all the configuration that set the event shape binning
  Gaudi::Property< std::string > m_containerName { this, "ContainerName", "", "Container Name" };

  Gaudi::Property< std::vector < std::string > > m_JetIterNames { this, "JetIterNames", {}, "Key for the shape passed in input to HI Event Shape Jet Iteration" };

  Gaudi::Property< bool > m_useCaloCell { this, "UseCaloCell", false , "Flag for Calo Cells" };

  Gaudi::Property< bool > m_useClusters { this, "UseClusters", true  , "Flag for Clusters" };

};
#endif
