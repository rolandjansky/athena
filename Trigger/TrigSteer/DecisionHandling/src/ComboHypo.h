/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef DECISIONHANDLING_COMBOHYPO_H
#define DECISIONHANDLING_COMBOHYPO_H

// Framework includes
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "DecisionHandling/TrigCompositeUtils.h"

// STL includes
#include <string>
#include <utility>  

/**
 * @class ComboHypo for combined hypotheses required only counting (multiplicity requirements)
 * @warning while configuring it the order of specified multiplicities has to mach order of input decision containers
 * i.e. if feed with:
 * electronDecisions
 * muonDecisions
 * jetDecisions
 * the multiplicity specification like this:
 * "HLT_4e10_2mu7_j100" : [ 4, 2, 1 ] will apply respectively requirement of 4, 2, 1 positive decisions in electron, muon and jet inputs
 **/
class ComboHypo : public ::AthReentrantAlgorithm {
public:
  ComboHypo(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~ComboHypo() override;

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& context) const override;
  virtual StatusCode finalize() override;


private:
  SG::ReadHandleKeyArray<TrigCompositeUtils::DecisionContainer> m_inputs { this, "HypoInputDecisions", {}, "Input Decisions" };
  SG::WriteHandleKeyArray<TrigCompositeUtils::DecisionContainer> m_outputs { this, "HypoOutputDecisions", {}, "Ouput Decisions" };

  typedef std::map<std::string, std::vector<int>> MultiplicityReqMap;
  Gaudi::Property< MultiplicityReqMap > m_multiplicitiesReqMap{this, "MultiplicitiesMap", {}, "Map from the chain name to implicities required at each input"};

  //!< iterates over the inputs and for every object (no filtering) crates output object linked to input moving the decisions that are mentioned in the passing set
  StatusCode copyDecisions( const TrigCompositeUtils::DecisionIDContainer& passing, const EventContext& context ) const;

  //!< iterates over all inputs filling the multiplicity map for each input collection
  typedef std::map<TrigCompositeUtils::DecisionID, int> MultiplicityMap;
  typedef std::map<TrigCompositeUtils::DecisionID,  std::vector<std::pair<int,int>>> CombinationMap;
  void fillDecisionsMap(  CombinationMap &  dmap, std::map<TrigCompositeUtils::DecisionID,std::map<int,int>> & featureMap, const EventContext& context) const;

};

#endif // DECISIONHANDLING_COMBOHYPO_H
