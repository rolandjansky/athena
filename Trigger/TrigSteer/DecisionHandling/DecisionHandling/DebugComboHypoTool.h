/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DECISIONHANDLING_DEBUGCOMBOHYPOTOOL_H
#define DECISIONHANDLING_DEBUGCOMBOHYPOTOOL_H

#include "TrigCompositeUtils/HLTIdentifier.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "DecisionHandling/ComboHypoToolBase.h"

/**
 * @class DebugComboHypoTool
 * @brief Concrete implimentation of the ComboHypoToolBase used by the ComboHypoTool unit test.
 * This class will report which physics objects were supplied to it, and it will pass any combination
 * supplied where at least one physics object is supplied on each leg whose pT > 45 MeV.
 **/
class DebugComboHypoTool:  public ComboHypoToolBase {

 public:
  
  DebugComboHypoTool(const std::string& type,
                     const std::string& name,
                     const IInterface* parent);
  
  virtual ~DebugComboHypoTool() {};
  virtual StatusCode initialize() override;

  private:
  bool executeAlg(const std::vector<Combo::LegDecision>& combination) const override;

};  

#endif // DECISIONHANDLING_DEBUGCOMBOHYPOTOOL_H

