/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTTAGALGS_GLOBALTRIGGERTAGBUILDER_H
#define EVENTTAGALGS_GLOBALTRIGGERTAGBUILDER_H

/**
   Tag Builder for Trigger
*/

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "EventTagAlgs/TagBuilderBase.h"
#include "EventTagUtils/GlobalTriggerTagTool.h"

#include <vector>

class GlobalTriggerTagBuilder : public AthAlgorithm, public TagBuilderBase
{
public:

  //! Standard constructor.
  GlobalTriggerTagBuilder(const std::string& name, ISvcLocator* pSvcLocator);
  
  //! Destructor.
  ~GlobalTriggerTagBuilder();
  
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
private:
  
  //! Attribute list name
  std::string m_attributeListName;

  //! global event tag tool
  ToolHandle<GlobalTriggerTagTool> m_globalTriggerTagTool;

  //! number of trigger level words
  std::vector<int> m_MAX_NUMBER;
};

#endif
