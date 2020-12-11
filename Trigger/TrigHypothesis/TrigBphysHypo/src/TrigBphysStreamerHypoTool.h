/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_TrigBphysStreamerHypoTool_H
#define TRIG_TrigBphysStreamerHypoTool_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigCompositeUtils/HLTIdentifier.h"

class TrigBphysStreamerHypoTool : public ::AthAlgTool {
 public:
  TrigBphysStreamerHypoTool(const std::string& type, const std::string& name, const IInterface* parent);
  HLT::Identifier decisionId() const { return m_decisionId; }

 private:
  HLT::Identifier m_decisionId;

};

#endif  // TRIG_TrigBphysStreamerHypoTool_H
