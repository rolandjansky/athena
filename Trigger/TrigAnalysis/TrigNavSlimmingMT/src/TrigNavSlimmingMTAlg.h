/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGNAVSLIMMINGMT_TRIGNAVSLIMMINGMTALG_H
#define TRIGNAVSLIMMINGMT_TRIGNAVSLIMMINGMTALG_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "StoreGate/WriteHandle.h"
#include "StoreGate/ReadHandle.h"

#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "TrigCompositeUtils/NavGraph.h"

#include "xAODParticleEvent/ParticleContainer.h"
#include "xAODTrigMissingET/TrigMissingETContainer.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

#include "TrigDecisionTool/TrigDecisionTool.h"

/**
 * @brief Consumes a set of Run 3 trigger navigation collection(s), applies slimming/thinning,
 * writes out a single output collection.
 *
 * Operations include:
 * - Keeping graph nodes which correspond to a particular set of chains. Good for small dAOD sizes.
 * - Keeping only the bottom of the graph, from the final "feature" onwards. Good for physics analysis / trigger matching.
 * - Keeping failed branches. Used for trigger performance studies / T0 monitoring.
 * - Removing named nodes from the graph, ("F"ilter nodes are only used online).
 * - Removing namded edges from the graph, ("view" edges are only used online).
 **/
class TrigNavSlimmingMTAlg : public AthReentrantAlgorithm {
public:
  TrigNavSlimmingMTAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode initialize() override;
  virtual StatusCode execute (const EventContext& ctx) const override;

private:

  typedef std::map<const TrigCompositeUtils::Decision*, TrigCompositeUtils::Decision*> IOCacheMap;

  struct Outputs {
    SG::WriteHandle<TrigRoiDescriptorCollection>* rois;
    SG::WriteHandle<xAOD::ParticleContainer>* particles;
    SG::WriteHandle<xAOD::TrigMissingETContainer>* mets;
    SG::WriteHandle<xAOD::TrigCompositeContainer>* nav;
  };

  SG::ReadHandleKey<xAOD::TrigCompositeContainer> m_primaryInputCollection{
    this, "PrimaryInputCollection", "HLTNav_Summary",
    "Input collection containing the HLTPassRaw terminus node of the navigation."};

  SG::WriteHandleKey<xAOD::TrigCompositeContainer> m_outputCollection{
    this, "OutputCollection", "HLTNav_Summary_ESDSlimmed",
    "Single output collection containing the slimmed navigation nodes."};

  SG::WriteHandleKey<TrigRoiDescriptorCollection> m_outputRepackedROICollectionKey{
    this, "RepackROIsOutputCollection", "HLTNav_RepackedROIs",
    "Single output collection containing any repacked ROIs (use with RepackROIs)."};

  SG::WriteHandleKey<xAOD::ParticleContainer> m_outputRepackedFeaturesCollectionKey_Particle{
    this, "RepackFeaturesOutputCollection_Particle", "HLTNav_RepackedFeatures_Particle",
    "Single output collection containing any repacked Particle features (use with RepackFeatures)."};

  SG::WriteHandleKey<xAOD::TrigMissingETContainer> m_outputRepackedFeaturesCollectionKey_MET{
    this, "RepackFeaturesOutputCollection_MET", "HLTNav_RepackedFeatures_MET",
    "Single output collection containing any repacked IParticle features (use with RepackFeatures)."};

  Gaudi::Property<bool> m_keepFailedBranches{
    this, "KeepFailedBranched", true,
    "Keep nodes which are in branches of the graph which fail selection for all triggers, these are used by T0 monitoring."};

  Gaudi::Property<bool> m_keepOnlyFinalFeatures{
    this, "KeepOnlyFinalFeatures", false,
    "Keeps only the final features which accepted a chain. Fine for analysis-use."};

  Gaudi::Property<bool> m_repackROIs{
    this, "RepackROIs", false,
    "Re=pack the target of all 'roi' and 'initialRoI' edges into a single container (WriteHandle defined above)"};

  Gaudi::Property<bool> m_repackFeatures{
    this, "RepackFeatures", false,
    "Re=pack the target of all 'feature' edges into a small number of containers (WriteHandle defined above)"};

  Gaudi::Property<std::vector<std::string>> m_edgesToDrop{
    this, "EdgesToDrop", {"view"},
    "Any edges within the set of names will be dropped as part of the thinning"};

  Gaudi::Property<std::vector<std::string>> m_nodesToDrop{
    this, "NodesToDrop", {"F"},
    "Any nodes within the set of names will be dropped as part of the thinning and the navigation re-wired around them"};

  Gaudi::Property<std::vector<std::string>> m_allOutputContainers{
    this, "AllOutputContainers", {},
    "List of SG keys of all possible output containers at differing verbosity. Used to stop different instances of the alg interfering with each other."};

  Gaudi::Property<std::vector<std::string>> m_chainsFilter{
    this, "ChainsFilter", {},
    "Optional list of HLT chains. If provided, only navigation data corresponding to these chains will be kept. "
    "Matching multiple chains via regular expressions is supported."};

  PublicToolHandle<Trig::TrigDecisionTool> m_trigDec{
    this, "TrigDecisionTool", "Trig::TrigDecisionTool/TrigDecisionTool",
    "Trigger Decision Tool, used to apply the ChainsFilter"};

  std::set<std::string> m_allOutputContainersSet; //!< Processed form of m_allOutputContainers

  /**
   * @brief Convert the ChainsFilter into the set of chain-IDd and chain-leg-IDs which comprises 
   * all of the DecisionIDs used by the members of the ChainsFilter.
   * @param[out] chainIDs The set to be populated from m_chainsFilter
   **/
  StatusCode fillChainIDs(TrigCompositeUtils::DecisionIDContainer& chainIDs) const;

  /**
   * @brief Map a const Decision object from an input collection to its equivalent in the output collection
   * Where a new Decision object is required in the output collection, the propagateLinks function is applied.
   * @param[in] input The const Decision object from a input collection.
   * @param[out] output The Decision object in the output collection.
   * @param[inout] cache Cached mapping of input->output objects. Cached output is returned if present.
   * @param[inout] outputContainers The write handles, contains the nav write handle which should own any new Decision objects which need to be created.
   * @param[in] chainIDs DecisionIDs which should propagate from input to output DecisionObjects.
   * @param[in] ctx Event context.
   **/
  StatusCode inputToOutput(
    const TrigCompositeUtils::Decision* input, 
    TrigCompositeUtils::Decision* output,
    IOCacheMap& cache, 
    Outputs& outputContainers, 
    const TrigCompositeUtils::DecisionIDContainer& chainIDs, 
    const EventContext& ctx) const;

  /**
   * @brief Copy links (graph edges) from input to output.
   * Applies the EdgesToDrop thinning.
   * Note: Does not copy "seed" links which form the underlying graph structure.
   * @param[in] input The Decision object to copy from.
   * @param[out] output The Decision object to copy to.
   **/
  StatusCode propagateLinks(
    const TrigCompositeUtils::Decision* input, 
    TrigCompositeUtils::Decision* output) const;

  /**
   * @brief Copy DecisionIDs (passing chains and passing chain-legs) from input to output.
   * Applies the ChainsFilter.
   * @param[in] input The Decision object to copy from.
   * @param[out] output The Decision object to copy to.
   * @param[in] chainIDs DecisionIDs which should propagate from input to output DecisionObjects.
   **/
  StatusCode propagateDecisionIDs(
    const TrigCompositeUtils::Decision* input,
    TrigCompositeUtils::Decision* output,
    const TrigCompositeUtils::DecisionIDContainer& chainIDs) const;

  /**
   * @brief Copy the subset of "seed" links which are present in the inputNode.
   * This function uses the IOCacheMap to perform all linking only against the mutable Decision objects
   * which form the slimmed and thinned navigation.
   * It must only be called after inputToOutput has been used to create all of the output Decision objects.
   * @param[in] inputNode The thinned transient NavGraphNode which only now has seed links which we want to keep
   * @param[inout] cache Cached mapping of input->output objects.
   * @param[in] ctx Event context.
   **/
  StatusCode propagateSeedingRelation(  
    const TrigCompositeUtils::NavGraphNode* inputNode, 
    IOCacheMap& cache,
    const EventContext& ctx) const;

  /**
   * @brief Repacks ElementLinks in the DecisionObject to point to compact output containers
   * written by this alg.
   * @param[in] decision A mutable decision object we're outputting from the nav slimming
   * @param[in] outputContainers The mutable write handles, in these we will make a copy of the rois or features we repack.
   **/
  StatusCode repackLinks(
    TrigCompositeUtils::Decision* output,
    Outputs& outputContainers) const;

  /**
   * @brief Look for an ElementLink<COLLECTION> with the given edge-name in 'decision',
   * if found then make a copy of the linked object to the supplied writeHandle and update the link
   * in 'decision' to point to this rewritten object.
   * @param[in] decision A mutable decision object we're outputting from the nav slimming
   * @param[in] writeHandle Mutable write handle where we can make a copy of the roi or feature.
   **/
  template< typename COLLECTION >
  StatusCode doRepack(TrigCompositeUtils::Decision* decision,
    SG::WriteHandle<COLLECTION>* writeHandle,
    const std::string& edgeName) const;

  /**
   * @brief Performs the xAOD Copy. Any non-xAOD collections will need to provide
   * an explicit implementation of this function. For example, TrigRoiDescriptorCollection for example.
   **/
  template< typename COLLECTION >
  StatusCode doRepackCopy(const typename COLLECTION::base_value_type* object,
    SG::WriteHandle<COLLECTION>* writeHandle) const;

};

#include "TrigNavSlimmingMTAlg.icc"

#endif // TRIGNAVSLIMMINGMT_TRIGNAVSLIMMINGMTALG_H
