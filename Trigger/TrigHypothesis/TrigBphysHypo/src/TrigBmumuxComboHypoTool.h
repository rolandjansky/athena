/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_TrigBmumuxComboHypoTool_H
#define TRIG_TrigBmumuxComboHypoTool_H

#include <string>
#include <vector>

#include "DecisionHandling/ComboHypoToolBase.h"

#include "xAODTrigBphys/TrigBphys.h"
#include "xAODTrigBphys/TrigBphysContainer.h"

#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"

#include "AthenaMonitoringKernel/Monitored.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"

using TrigCompositeUtils::Decision;
using TrigCompositeUtils::DecisionIDContainer;

/**
 * @class TrigBmumuxComboHypoTool
 * @brief ComboHypoTool for B -> mu+, mu- X decays
 */
class TrigBmumuxComboHypoTool: public ComboHypoToolBase {
 public:
  TrigBmumuxComboHypoTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual StatusCode initialize() override;
  virtual StatusCode decideOnSingleObject(Decision*, const std::vector<const DecisionIDContainer*>&) const override;

 private:
  bool passed(const xAOD::TrigBphys*) const;
  virtual bool executeAlg(std::vector<LegDecision>&) const override { return true; }

  Gaudi::Property<int> m_decay {this, "Decay", 9999, "decay as enumerated in xAOD::TrigBphys::pType"};

  ToolHandle<GenericMonitoringTool> m_monTool {this, "MonTool", "", "Monitoring tool"};
};

#endif  // TRIG_TrigBmumuxComboHypoTool_H
