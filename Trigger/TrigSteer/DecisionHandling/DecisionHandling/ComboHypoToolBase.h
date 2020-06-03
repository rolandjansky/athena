/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
 * @brief
 **/


class ComboHypoToolBase : public extends<AthAlgTool, IComboHypoTool> {

public:
  ComboHypoToolBase(const std::string& type, const std::string& name, const IInterface* parent);

  
  /**
   * @brief retreives the decisions associated to this decId, make their combinations and apply the algorithm
   @param[in]  LegDecisionsMap that lists all the passing decisions, to be updated  
   @param[in]  Event Context passed to store the output collection
  **/  
  virtual StatusCode decide(LegDecisionsMap & passingLegs, const EventContext& /* ctx */ ) const override;
  
  virtual StatusCode decideOnSingleObject(TrigCompositeUtils::Decision*, const std::vector<TrigCompositeUtils::DecisionIDContainer*>&) const { return StatusCode::SUCCESS; }
  
  /**
   * @brief retrieves this decision Id
   **/
  virtual HLT::Identifier decisionId() const { return m_decisionId; }

  void setLegDecisionIds(const std::vector<HLT::Identifier>& legDecisionIds) { m_legDecisionIds = legDecisionIds; }
  HLT::Identifier legDecisionId(size_t i) const { return m_legDecisionIds.at(i); }
  const std::vector<HLT::Identifier>& legDecisionIds() const { return m_legDecisionIds; }
    
 protected:

  /**
  * @brief creates the decision legs starting from the initial LegDecision map, storing only those concerning this decisionID
  **/
  
  StatusCode selectLegs(const LegDecisionsMap & IDCombMap, std::vector<std::vector<LegDecision>>& leg_decisions) const;
    
   /**
  * @brief creates combinations of decisions starting from the legs vector of pairs, given the number of legs and the number of elements to combine
    @param[in] leg_decisions: vector of legs (vector), each containing the corresponding decision pairs
    @param[in] nLegs: number of legs to combine
    @param[in] nToGroup: number of elements to group in a combination, in case one leg is used
    @param[out] combinations: vector of combinations (vectors) of decision pairs
  **/
  void createCombinations( const std::vector<std::vector<LegDecision>>& leg_decisions,
			   std::vector<std::vector<LegDecision>> & combinations,
			   int nLegs, int nToGroup) const;

  /**
  * @brief recursively creates combinations of elements from differnt vectors
    @param[in] all: initial vector of decision legs
    @parma[in] local: temporary vector of combinations
    @param[in] lindex: leg index
    @param[out] tocombine: vector of combinations
  **/
  void recursive_combine( const std::vector<std::vector<LegDecision>> & all,
			  std::vector<std::vector<LegDecision>> & tocombine,
			  std::vector<LegDecision> &local, u_int lindex)  const;

  /**
  * @brief contains the real algorithm to apply on the given combination of decisions
  **/
  virtual bool executeAlg(std::vector<LegDecision> &  /* thecomb  */) const {return true;}

  
 /**
  * @brief Converts the leg decision vector back to the map
  **/
  void updateLegDecisionsMap(const std::vector<std::vector<LegDecision>> & passing_comb,
			     LegDecisionsMap & passingLegs) const;


  /**
  * @brief Print the Tool results stored in the passing combinations
  **/
  StatusCode printDebugInformation(const LegDecisionsMap & passingLegs) const;

  
 
  /**
  * @brief Fill out the WriteHandle of legs passing
  **/
  /*
  // comment now, because this must be designed properly
    StatusCode createOutput( LegDecisionsMap & passingLegs, const EventContext& ctx ) const;

  // add here WriteHandkles of combinations
  // problem: the size of the array depends on the number of combinations, so cannot use the WriteHandleKeyArray
    SG::WriteHandleKeyArray<TrigCompositeUtils::DecisionContainer> m_passingLegs { this, "PassingLegs", {}, "Ouput Decisions legs passing the selection" };
  */
  
private:

  HLT::Identifier m_decisionId;
  std::vector<HLT::Identifier> m_legDecisionIds;

};

#endif // DECISIONHANDLING_COMBOHYPOTOOLBASE_H
