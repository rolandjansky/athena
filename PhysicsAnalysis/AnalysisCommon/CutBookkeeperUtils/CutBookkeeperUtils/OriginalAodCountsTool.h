// for text editors: this file is -*- C++ -*-
/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AODCOUNTS_TOOL_H
#define AODCOUNTS_TOOL_H

#include "AsgTools/AsgTool.h"

namespace xAOD {
  class TEvent;
}
class StatusCode;
struct OriginalAodCounts;

class OriginalAodCountsTool : public asg::AsgTool
{
  ASG_TOOL_CLASS0(OriginalAodCountsTool)
  public:
  OriginalAodCountsTool(const std::string& name);
  ~OriginalAodCountsTool();

  StatusCode incrementAodCounts(xAOD::TEvent& event, OriginalAodCounts&);
private:
};

#endif
