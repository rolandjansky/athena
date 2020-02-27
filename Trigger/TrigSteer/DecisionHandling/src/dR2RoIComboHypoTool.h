/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DECISIONHANDLING_DR2ROICOMBOHYPOTOOL_H
#define DECISIONHANDLING_DR2ROICOMBOHYPOTOOL_H


#include "DecisionHandling/HLTIdentifier.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "IComboHypoTool.h"

class dR2RoIComboHypoTool:  virtual public IComboHypoTool, public AthAlgTool {

 public:
  
  dR2RoIComboHypoTool(const std::string& type,
                    const std::string& name,
                    const IInterface* parent);
  virtual ~dR2RoIComboHypoTool();
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  virtual StatusCode decide( std::vector<TrigCompositeUtils::DecisionContainer>& decisions,
			     const CombinationMap IDCombMap,
			     std::map<TrigCompositeUtils::DecisionID,
			     std::vector< std::pair<uint32_t,
			     uint16_t> >> & passingComb ) const override;

  private:
  
  Gaudi::Property<float> m_DRcut{this, "DRcut", 0.1, "DR threshold" };
 
  // Thees two can be mmerged in one, and found common way of retrieveing the combinations--> in tha base class
  StatusCode combineOneLeg(std::vector<TrigCompositeUtils::DecisionContainer>& decisions,
			   std::vector< std::pair<uint32_t,uint16_t> > combinations,
			   std::map<TrigCompositeUtils::DecisionID,
			   std::vector< std::pair<uint32_t,uint16_t> >> & passingCombinations) const;

  StatusCode combineMoreLegs(std::vector<TrigCompositeUtils::DecisionContainer>& decisions,
			     std::vector<std::vector< std::pair<uint32_t,uint16_t> >> combinations, //vector of vector
			   std::map<TrigCompositeUtils::DecisionID,
			   std::vector< std::pair<uint32_t,uint16_t> >> & passingCombinations) const;



  double deltaR(double eta1, double eta2, double phi1, double phi2) const;


  // to be moved to Base class: 
  void recursive_combine(std::vector<std::vector< std::pair<uint32_t,uint16_t>>> &all,
			 std::vector<std::vector< std::pair<uint32_t,uint16_t> >> & tocombine,
			 std::vector<std::pair<uint32_t,uint16_t>>& local, u_int lindex)  const;


}; // DECISIONHANDLING_DR2ROICOMBOHYPOTOOL_H
#endif

