/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef DECISIONHANDLING_COMBOHYPOTOOLBASE_H
#define DECISIONHANDLING_COMBOHYPOTOOLBASE_H

// Package includes
#include "IComboHypoTool.h"

// Framework includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "DecisionHandling/HLTIdentifier.h"
#include "DecisionHandling/TrigCompositeUtils.h"

// STL includes
#include <string>

/**
 * @class ComboHypoToolBase
 * @brief
 **/


class ComboHypoToolBase : public extends<AthAlgTool, IComboHypoTool> {

public:
  ComboHypoToolBase(const std::string& type, const std::string& name, const IInterface* parent);

  
  /**
   * @brief retreives the decisions associated to this decId, make their combinations and apply the algorithm
   @param[in]  InputDecisions
   @param[in]  Cobminaiton map that lists all the decisions passing the multiplicity map of the ComboHypo
   @param[out] Combination map that lists all the decisions passing the HypoTool algorithm
  **/  
  virtual StatusCode decide(const LegDecisionsMap & IDCombMap,LegDecisionsMap & passingCombinations ) const override;
  
  
  /**
   * @brief retrieves this decision Id
   **/
  virtual HLT::Identifier decisionId() const { return m_decisionId; } 

    
 protected:

  /**
  * @brief creates the decision legs starting from the initial LegDecision map, storing only those concerning this decisionID
  **/
  StatusCode selectLegs(const LegDecisionsMap & IDCombMap, std::vector<ElementLinkVector<TrigCompositeUtils::DecisionContainer>>& leg_decisions) const;
    
   /**
  * @brief creates combinations of decisions starting from the legs vector of pairs, given the number of legs and the number of elements to combine
    @param[in] v_legs: vector of legs (vector), each containing the corresponding decision pairs
    @param[in] nLegs: number of legs to combine
    @param[in] nToGroup: number of elements to group in a combination, in case one leg is used
    @param[out] combinations: vector of combinations (vectors) of decision pairs
  **/
  void createCombinations(const std::vector<ElementLinkVector<TrigCompositeUtils::DecisionContainer>> & v_legs,
			  std::vector<ElementLinkVector<TrigCompositeUtils::DecisionContainer>> & combinations, int nLegs, int nToGroup) const;

  /**
  * @brief recursively creates combinations of elements from differnt vectors
    @param[in] all: initial vector of decision legs
    @parma[in] local: temporary vector of combinations
    @param[in] lindex: leg index
    @param[out] tocombine: vector of combinations
  **/
  void recursive_combine(const std::vector<ElementLinkVector<TrigCompositeUtils::DecisionContainer>> &all,
			 std::vector<ElementLinkVector<TrigCompositeUtils::DecisionContainer>> & tocombine,
			 ElementLinkVector<TrigCompositeUtils::DecisionContainer> & local, u_int lindex)  const;


  /**
  * @brief contains the real algorithm to apply on the given combination of decisions
  **/
  virtual bool executeAlg(ElementLinkVector<TrigCompositeUtils::DecisionContainer>& /* thecomb  */) const {return true;}

  
 /**
  * @brief Retrieves the decisionIds from the passing combinations and produce a new LegDecisionMap
  **/
  void setDecisionIds(const ElementLinkVector<TrigCompositeUtils::DecisionContainer>& thecomb, LegDecisionsMap & passingCombinations) const;


  /**
  * @brief Print the Tool results stored in the passing combinations
  **/

  StatusCode printExecute(const LegDecisionsMap & passingCombinations) const;


// add here WriteHandkles of combinations
  
private:

    HLT::Identifier m_decisionId;
    

};

#endif // DECISIONHANDLING_COMBOHYPOTOOLBASE_H
