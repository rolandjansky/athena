/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef DECISIONHANDLING_COMBOHYPO_H
#define DECISIONHANDLING_COMBOHYPO_H

// Framework includes
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"

// STL includes
#include <string>
#include <utility>  
#include "DecisionHandling/ComboHypoToolBase.h"

/**
 * @class ComboHypo for combined hypotheses required only counting (multiplicity requirements)
 * @warning while configuring it the order of specified multiplicities has to match order of input decision containers
 * i.e. if feed with:
 * electronDecisions
 * muonDecisions
 * jetDecisions
 * the multiplicity specification like this:
 * "HLT_4e10_2mu7_j100" : [ 4, 2, 1 ] will apply respectively requirement of 4, 2, 1 positive decisions in electron, muon and jet inputs
 * the leg specification like this:
 * "HLT_4e10_2mu7_j100" : [ 0, 1, 2 ], where the names of legs are leg000, leg001, and leg002.
 **/


class ComboHypo : public ::AthReentrantAlgorithm {
 public:
  ComboHypo(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~ComboHypo() override;

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& context) const override;
  virtual StatusCode finalize() override;

 protected:
  const SG::ReadHandleKeyArray<TrigCompositeUtils::DecisionContainer>& decisionsInput() const { return m_inputs; }
  const SG::WriteHandleKeyArray<TrigCompositeUtils::DecisionContainer>& decisionsOutput() const { return m_outputs; }
  typedef std::map<std::string, std::vector<int>> MultiplicityReqMap;
  typedef std::map<std::string, std::vector<int>> LegMap;
  const MultiplicityReqMap& triggerMultiplicityMap() const { return m_multiplicitiesReqMap.value(); }
  const LegMap& triggerLegMap() const { return m_legMap.value(); }
  ToolHandleArray<ComboHypoToolBase>& hypoTools() { return m_hypoTools; }
  const ToolHandleArray<ComboHypoToolBase>& hypoTools() const { return m_hypoTools; }

 private:

  SG::ReadHandleKeyArray<TrigCompositeUtils::DecisionContainer> m_inputs { this, "HypoInputDecisions", {}, "Input Decisions" };
  SG::WriteHandleKeyArray<TrigCompositeUtils::DecisionContainer> m_outputs { this, "HypoOutputDecisions", {}, "Ouput Decisions" };

  Gaudi::Property<bool> m_requireUniqueROI {this, "RequireUniqueROI", false,
    "Require each Feature in each leg of the combination to come from a unique L1 seeding ROI."};

  Gaudi::Property< MultiplicityReqMap > m_multiplicitiesReqMap{this, "MultiplicitiesMap", {}, 
    "Map from the chain name to multiplicities required at each input"};

  Gaudi::Property< LegMap > m_legMap{this, "LegMap", {},
    "Map from the chain name to legs required at each input"};

  Gaudi::Property<bool> m_checkMultiplicityMap { this, "CheckMultiplicityMap", true,
    "Perform a consistency check of the MultiplicitiesMap"};

  /**
  * @brief iterates over the inputs and for every object (no filtering) crates output object linked to input moving 
  * the decisions that are mentioned in the passing set
  **/
  
  StatusCode copyDecisions( const LegDecisionsMap & passingLegs, const EventContext& context ) const;


  /**
   * @brief For a given Decision node from a HypoAlg, extracts type-less identification data on the node's Feature and seeding ROI.
   * @param[in] d The Decision node from the HypoAlg, expected to have a "feature" link attached to it.
   *   Expected to be able to locate a "initialRoI" in its history if RequireUniqueROI=True.
   * @param[out] featureKey Type-less SG Key hash of the collection hosting the Decision node's feature .
   * @param[out] featureIndex Index inside the featureKey collection. 
   * @param[out] roiKey Type-less SG Key hash of the collection hosting the Decision node's initial ROI collection. 
   * @param[out] roiIndex Index inside the roiKey collection. 
   * @param[out] roiFullscan Flag indicating if the located initial ROI has the FullScan flag enabled. 
   *                         Triggers special behaviour allowing the ROI to satisfy arbitrary multiplicities in an arbitrary number of legs.
   * @param[inout] priorFeaturesMap Data structure collating for a given feature (key) what the prior features were integrated over all previous steps (value set). 
   **/
  StatusCode extractFeatureAndRoI(const ElementLink<TrigCompositeUtils::DecisionContainer>& EL,
    uint32_t& featureKey, uint16_t& featureIndex, uint32_t& roiKey, uint16_t& roiIndex, bool& roiFullscan, std::map<uint32_t, std::set<uint32_t>>& priorFeaturesMap) const; 


  /**
   * @brief iterates over all inputs, associating inputs to legs
   **/

  StatusCode fillDecisionsMap( LegDecisionsMap& dmap, const EventContext& context) const;

  ToolHandleArray< ComboHypoToolBase > m_hypoTools {this, "ComboHypoTools", {}, "Tools to perform selection"};

};


#endif // DECISIONHANDLING_COMBOHYPO_H
