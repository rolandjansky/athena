/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HIEVENTUTILS_HIEVENTSHAPEMAPTOOL_H
#define HIEVENTUTILS_HIEVENTSHAPEMAPTOOL_H

#include "xAODHIEvent/HIEventShape.h"
#include "HIEventUtils/HIEventShapeIndex.h"
#include "HIEventUtils/IHIEventShapeMapTool.h"
#include "xAODHIEvent/HIEventShapeContainer.h"
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
  virtual const HIEventShapeIndex* getIndex(IHIEventShapeMapTool::BinningScheme key) const override;
  virtual const HIEventShapeIndex* getIndexFromShape(const xAOD::HIEventShapeContainer* shape) const override;

  virtual bool hasKey(BinningScheme key) override;

  private:

  std::map< IHIEventShapeMapTool::BinningScheme ,HIEventShapeIndex > m_map;

};
#endif
