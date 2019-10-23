// for text editors: this file is -*- C++ -*-
/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AODCOUNTS_TOOL_H
#define AODCOUNTS_TOOL_H

#include "CutBookkeeperUtils/OriginalAodCounts.h"

#include "AsgTools/AsgTool.h"

namespace xAOD {
  class TEvent;
}
class StatusCode;

class OriginalAodCountsTool : public asg::AsgTool
{
  ASG_TOOL_CLASS0(OriginalAodCountsTool)
  public:
  OriginalAodCountsTool(const std::string& name);
  ~OriginalAodCountsTool();

  virtual StatusCode initialize() override;

  StatusCode incrementAodCounts(xAOD::TEvent& event, OriginalAodCounts&) const;
private:
  AodCountsConfig m_config;
};

#endif
