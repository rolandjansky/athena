/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DECISIONHANDLING_DELTARROICOMBOHYPOTOOL_H
#define DECISIONHANDLING_DELTARROICOMBOHYPOTOOL_H


#include "TrigCompositeUtils/HLTIdentifier.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "DecisionHandling/ComboHypoToolBase.h"

class DeltaRRoIComboHypoTool:  public ComboHypoToolBase {

 public:
  
  DeltaRRoIComboHypoTool(const std::string& type,
                    const std::string& name,
                    const IInterface* parent);
  
  virtual ~DeltaRRoIComboHypoTool() {};
  virtual StatusCode initialize() override;

  private:

  bool executeAlg(std::vector<LegDecision> & thecomb)  const override;
 
  double deltaR(double eta1, double eta2, double phi1, double phi2) const;

  Gaudi::Property<float> m_DRcut{this, "DRcut", 0.1, "DR threshold" };
 


}; // DECISIONHANDLING_DR2ROICOMBOHYPOTOOL_H
#endif

