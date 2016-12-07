/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMonitorBase/IExtMonitorTool.h"

void fillMonitored(ToolHandleArray<IExtMonitorTool>& monTools)
{
  for (auto& m : monTools) {
    m->fillHists();
  }
}

void declareMonitoredCustomVariable(const std::string& name, IGetter* g,
                                    ToolHandleArray<IExtMonitorTool>& monTools)
{
  size_t i(0);
  for (auto& m : monTools) {
    if (++i>1) g = new IGetter(*g);
    g->name(name);
    m->setProxy(name, g);
  }
}
