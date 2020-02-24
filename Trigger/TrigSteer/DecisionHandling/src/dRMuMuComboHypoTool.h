/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DECISIONHANDLING_DRMUMUCOMBOHYPOTOOL_H
#define DECISIONHANDLING_DRMUMUCOMBOHYPOTOOL_H


#include "DecisionHandling/HLTIdentifier.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "IComboHypoTool.h"

class dRMuMuComboHypoTool:  virtual public IComboHypoTool, public AthAlgTool {

 public:
  
  dRMuMuComboHypoTool(const std::string& type,
                    const std::string& name,
                    const IInterface* parent);
  virtual ~dRMuMuComboHypoTool();
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  virtual StatusCode decide( std::vector<TrigCompositeUtils::DecisionContainer>& decisions,
			     const CombinationMap IDCombMap, std::map<TrigCompositeUtils::DecisionID, std::vector< std::pair<uint32_t,uint16_t> >> & passingComb ) const override;///TrigCompositeUtils::DecisionIDContainer& passingIds ) const override;

  double deltaR(double eta1, double eta2, double phi1, double phi2) const;


 private:
  
  Gaudi::Property<float> m_DRcut{this, "DRcut", 0.1, "DR threshold" };
 
 


}; // DECISIONHANDLING_DRMUMUCOMBOHYPOTOOL_H
#endif

