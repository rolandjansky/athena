/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DECISIONHANDLING_DELTARROICOMBOHYPOTOOL_H
#define DECISIONHANDLING_DELTARROICOMBOHYPOTOOL_H


#include "DecisionHandling/HLTIdentifier.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "IComboHypoTool.h"

class DeltaRRoIComboHypoTool:  virtual public IComboHypoTool, public AthAlgTool {

 public:
  
  DeltaRRoIComboHypoTool(const std::string& type,
                    const std::string& name,
                    const IInterface* parent);
  virtual ~DeltaRRoIComboHypoTool();
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  virtual StatusCode decide( const std::vector<TrigCompositeUtils::DecisionContainer>& decisions,
			     const CombinationMap & IDCombMap,
			     std::map<TrigCompositeUtils::DecisionID, std::vector< std::pair<uint32_t, uint16_t> >> & passingComb ) const override;

  private:
  
  Gaudi::Property<float> m_DRcut{this, "DRcut", 0.1, "DR threshold" };
 
  double deltaR(double eta1, double eta2, double phi1, double phi2) const;


 


}; // DECISIONHANDLING_DR2ROICOMBOHYPOTOOL_H
#endif

