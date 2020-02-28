/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef DECISIONHANDLING_ICOMBOHYPOTOOL_H
#define DECISIONHANDLING_ICOMBOHYPOTOOL_H 1

#include "DecisionHandling/HLTIdentifier.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "GaudiKernel/IAlgTool.h"
#include "ComboHypoCombination.h"

/**
 * @class $(klass) 
 * @brief This class describe the base functionalities of a HypoTool used by the ComboAlg
 **/

 /**
  * @brief describes the association between each decId and the decisions associated to that 
  **/
typedef std::map<TrigCompositeUtils::DecisionID, ComboHypoCombination> CombinationMap;


class IComboHypoTool: virtual public ::IAlgTool { 
  
 public: 
  DeclareInterfaceID(IComboHypoTool, 1, 0);
  
  virtual ~IComboHypoTool() {};
  IComboHypoTool(const std::string& name)   : m_decisionId(  HLT::Identifier::fromToolName( name ) ) {}

  /**
  * @brief retreives the decisions associated to this decId, make their combinations and apply the algorithm
    @param[in]  InputDecisions
    @param[in]  Cobminaiton map that lists all the decisions passing the multiplicity map of the ComboHypo
    @param[out] Combination map that lists all the decisions passing the HypoTool algorithm
  **/  
  virtual StatusCode decide(const std::vector<TrigCompositeUtils::DecisionContainer>& decisions,
			    const CombinationMap & IDCombMap,
			    std::map<TrigCompositeUtils::DecisionID, std::vector< std::pair<uint32_t,uint16_t> >> & passingComb ) const = 0;

  /**
  * @brief retrieves this decision Id
  **/
  virtual HLT::Identifier decisionId() const { return m_decisionId; }
  
  
 protected:
  
  HLT::Identifier m_decisionId;

  /**
  * @brief creates combinations of decisions starting from the legs vector of pairs, given the number of legs and the number of elements to combine
    @param[in] v_combinations: vector of legs (vector), each containing the corresponding decision pairs
    @param[in] nLegs: number of legs to combine
    @param[in] nToGroup: number of elements to group in a combination, in case one leg is used
    @param[out] tocombine: vector of combinations (vectors) of decision pairs
  **/
  void createCombinations(const std::vector<std::vector< std::pair<uint32_t,uint16_t> >> & v_combinations,
			  std::vector<std::vector< std::pair<uint32_t,uint16_t>> >& tocombine, int nLegs, int nToGroup) const;

  /**
  * @brief recursively creates combinations of elements from differnt vectors
    @param[in] all: initial vector of decision legs
    @parma[in] local: temporary vector of combinations
    @param[in] lindex: leg index
    @param[out] tocombine: vector of combinations
  **/
  void recursive_combine(const std::vector<std::vector< std::pair<uint32_t,uint16_t>>> &all,
			 std::vector<std::vector< std::pair<uint32_t,uint16_t> >> & tocombine,
			 std::vector<std::pair<uint32_t,uint16_t>>& local, u_int lindex)  const;
  
}; 



#endif //> !DECISIONHANDLING_ICOMBOHYPOTOOL_H
