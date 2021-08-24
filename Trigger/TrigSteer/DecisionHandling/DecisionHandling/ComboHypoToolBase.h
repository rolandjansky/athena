/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef DECISIONHANDLING_COMBOHYPOTOOLBASE_H
#define DECISIONHANDLING_COMBOHYPOTOOLBASE_H

// Package includes
#include "DecisionHandling/IComboHypoTool.h"

// Framework includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"

// STL includes
#include <string>
#include <vector>

/**
 * @class ComboHypoToolBase
 * @brief Base class for tools which cut on properties of multi-object or multi-leg chains.
 * User should derive from this class and implement the executeAlg function.
 * This will be called once per combination of objects in the event which reach the ComboHypo alg which hosts this tool.
 **/

class ComboHypoToolBase : public extends<AthAlgTool, IComboHypoTool> {

public:
  ComboHypoToolBase(const std::string& type, const std::string& name, const IInterface* parent);
  
  /**
   * @brief retrieves the decisions associated to this decId, make their combinations and apply the algorithm
   * @param[in]  LegDecisionsMap that lists all the passing decisions, to be updated by the tool depending on the outcome of executeAlg 
   * @param[in]  Event Context, currently unused
   **/  
  virtual StatusCode decide(Combo::LegDecisionsMap& passingLegs, const EventContext& /*ctx*/) const override;
    
  /**
   * @brief retrieves this ComboHypoTool's chain's decision ID
   **/
  virtual HLT::Identifier decisionId() const { return m_decisionId; }

  /**
  * @brief Sets the number of legs and the multiplicity required on each leg.
  * This should be called when the Tool is retrieved by its parent ComboHypo alg.
  * This also sets the leg Decision IDs at the same time
  * param[in] multiplicityRequiredMap: Mapping of chains to required multiplicity per leg.
  **/
  StatusCode setLegMultiplicity(const Combo::MultiplicityReqMap& multiplicityRequiredMap);

  /**
  * @brief Gets the number of legs and the multiplicity required on each leg.
  **/
  const std::vector<int>& legMultiplicity() const { return m_legMultiplicities; }

  /**
   * @brief Retrieves this ComboHypoTool's chain's decision ID for a given leg.
   * Only populated for chains with more than one leg. For chains with one leg, use decisionId()
   **/
  HLT::Identifier legDecisionId(size_t i) const { return m_legDecisionIds.at(i); }

  /**
   * @brief Retrieves this ComboHypoTool's chain's decision IDs for all legs.
   * Only populated for chains with more than one leg. For chains with one leg, use decisionId()
   **/
  const std::vector<HLT::Identifier>& legDecisionIds() const { return m_legDecisionIds; }

  /**
  * @brief Alternate method called by BPhysics ComboHypoAlgs _instead_ of the base method decide(...).
  * This function should be considered a specialist use-case only. It must be over-ridden to do anything useful.
  **/
  virtual StatusCode decideOnSingleObject(TrigCompositeUtils::Decision*, const std::vector<const TrigCompositeUtils::DecisionIDContainer*>&) const;
    
 protected:

  /**
  * @brief Only a dummy implementation exists in ComboHypoToolBase. This should be over-ridden by a derived class.
  * The derived class should return a boolean pass/fail for each possible combination in the event.
  * param[in] combination A single combination of objects to be discriminated against. Vector contains the required number of objects over
  * all legs. Use the pair.first to tell which leg a given pair.second decision object belongs to in the current combination.
  **/
  virtual bool executeAlg(const std::vector<Combo::LegDecision>& combination) const;

  /**
  * @brief Creates the per-leg vectors of Decision objects starting from the initial LegDecision map, storing only those concerning this HypoTool's chain
  * Pack the Decision objects in std::pair<DecisionID, ElementLink<Decision>> so the derived class' executeAlg function knows which leg each object is on.
  **/
  StatusCode selectLegs(const Combo::LegDecisionsMap& IDCombMap, std::vector<std::vector<Combo::LegDecision>>& leg_decisions) const;

 /**
  * @brief For when the tool accepts some/all combinations. Remove Decision Objects from legs of this HypoTool's chain
  * which participated in NONE of combinations which were flagged as accepting the event.
  **/
  void updateLegDecisionsMap(const std::vector<std::vector<Combo::LegDecision>>& passing_comb, Combo::LegDecisionsMap & passingLegs) const;

 /**
  * @brief For when the tool rejects all combinations. Remove all Decision Objects from all the legs of this HypoTool's chain.
  **/
  void eraseFromLegDecisionsMap(Combo::LegDecisionsMap &passingLegs) const;

  /**
  * @brief Print the output of the tool, after having removed failed Decision Objects. Restricted to the ComboHypoTool's chain's legs.
  **/
  StatusCode printDebugInformation(const Combo::LegDecisionsMap & passingLegs) const;

  Gaudi::Property<size_t> m_combinationsThresholdWarn {this, "CombinationsThresholdWarn", 1000,
    "Events processing this many combinations will generate a WARNING message."};

  Gaudi::Property<size_t> m_combinationsThresholdBreak {this, "CombinationsThresholdBreak", 10000,
    "Events processing this many combinations will generate a second WARNING message, and the loop over combinations will be terminated at this point."};  

  Gaudi::Property<bool> m_modeOR {this, "ModeOR", true,
    "Accepts based on the logical OR over all calls to executeAlg. If this flag is set to false then the logical AND is required instead."}; 

  Gaudi::Property<bool> m_enableOverride {this, "EnableOverride", false,
    "Stops processing combinations as soon as a valid combination is found in OR mode, or as soon as an invalid combination is found in AND mode. This is to save CPU."}; 
 
  // TODO - add optional write out of the data stored in passingCombinations in the decide function.
  
private:

  HLT::Identifier m_decisionId; //!< The DecisionID of the chain, obtained from the Tool's name
  std::vector<HLT::Identifier> m_legDecisionIds; //!< The DecisionIDs of the individual legs, derived from both m_decisionId and m_legMultiplicities.
  std::vector<int> m_legMultiplicities; //!< The number of legs, and the required multiplicity on each leg.

};

#endif // DECISIONHANDLING_COMBOHYPOTOOLBASE_H
