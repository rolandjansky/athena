/*
Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGDJEDHYPOALG_H
#define TRIGDJEDHYPOALG_H

// Framework includes
#include "DecisionHandling/HypoBase.h"
#include "xAODTrigger/TrigCompositeContainer.h"
#include "DisplacedJetEventDecisionHypoTool.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "xAODJet/JetContainer.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include <map>
#include "Gaudi/Property.h"
#include "AthViews/View.h"
#include "AthContainers/AuxElement.h"

// STL includes
#include <string>
class DisplacedJetEventDecisionHypoAlg : public ::HypoBase  {
public:
  DisplacedJetEventDecisionHypoAlg(const std::string& name, ISvcLocator* pSvcLocator);

  virtual ~DisplacedJetEventDecisionHypoAlg() = default;
  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& context) const override;

private:
  DisplacedJetEventDecisionHypoAlg();
  ToolHandleArray< DisplacedJetEventDecisionHypoTool > m_hypoTools {this, "HypoTools", {},"Tools that perform actual selection"};
};
#endif
