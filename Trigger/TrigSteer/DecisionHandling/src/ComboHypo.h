/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef DECISIONHANDLING_COMBOHYPO_H
#define DECISIONHANDLING_COMBOHYPO_H

// Framework includes
#include "DecisionHandling/InputMakerBase.h"
#include "DecisionHandling/TrigCompositeUtils.h"

// STL includes
#include <string>

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
class ComboHypo : public InputMakerBase {
public:
  ComboHypo(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~ComboHypo() override;

  virtual StatusCode initialize() override;
  virtual StatusCode execute_r(const EventContext& context) const override;
  virtual StatusCode finalize() override;

private:
  Gaudi::Property< std::map<std::string, std::vector<int>>> m_multiplicitiesMap{this, "MultiplicitiesMap", {}, "Map from the chain name to implicities required at each input"};

  //!< iterates over the inputs and for every object (no filtering) crates output object linked to input moving the decisions that are mentioned in the passing set
  StatusCode copyDecisions( const TrigCompositeUtils::DecisionIDContainer& passing ) const;

  //!< iterates over all inputs filling the multiplicity map for each input collection
  typedef std::map<TrigCompositeUtils::DecisionID, int> MultiplicityMap;
  void fillDecisionsMap( std::vector< MultiplicityMap >&  dmap) const;
};

#endif // DECISIONHANDLING_COMBOHYPO_H
