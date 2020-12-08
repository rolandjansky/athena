/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigBphysStreamerHypoTool.h"


TrigBphysStreamerHypoTool::TrigBphysStreamerHypoTool(const std::string& type, const std::string& name, const IInterface* parent)
    : AthAlgTool(type, name, parent),
      m_decisionId(HLT::Identifier::fromToolName(name)) {}
