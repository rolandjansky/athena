/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <QuickAna/IToolScheduler.h>

#include <QuickAna/DefinitionArgs.h>
#include <QuickAna/DefinitionConf.h>
#include <QuickAna/DefinitionMaker.h>
#include <QuickAna/IAnaTool.h>
#include <QuickAna/MessageCheck.h>

//
// method implementations
//

namespace ana
{
  StatusCode IToolScheduler ::
  addTools (const Configuration& conf, InternalConfiguration& internal,
            const std::string& prefix)
  {
    using namespace msgScheduler;

    AnaToolSequence tools;
    {
      std::vector<DefinitionConf> confList;
      ANA_CHECK (DefinitionConf::makeConfigList (confList, conf));

      DefinitionArgs masterArgs (prefix, &internal);
      ANA_CHECK (makeToolSequence (confList, masterArgs, tools));
    }
    for (auto& tool : tools)
    {
      if (tool.first)
        ANA_CHECK (tool.first->getInitialConfiguration(internal));
    }
    for (auto& tool : tools)
    {
      if (tool.first)
        ANA_CHECK (tool.first->useInitialConfiguration(internal));
    }
    for (auto& tool : tools)
    {
      if (tool.first)
      {
	ANA_CHECK (tool.first->initialize());
	ANA_CHECK (tool.first->useConfiguration (internal));
	ANA_CHECK (addTool (std::move (tool.first)));
      }
    }
    return StatusCode::SUCCESS;
  }
}
