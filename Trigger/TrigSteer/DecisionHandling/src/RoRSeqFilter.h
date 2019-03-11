/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef DECISIONHANDLING_RORSEQFILTER_H
#define DECISIONHANDLING_RORSEQFILTER_H 1

#include <string>
#include <set>

#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthContainers/ConstDataVector.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "DecisionHandling/HLTIdentifier.h"

/**
 * @class Filtering algorithm for HLT Step sequencer
 * @brief Consumes multiple decision inputs and filters those that are labelled with the required decision IDs
 * 
 * A RoRSeqFilter is a generic filter over collections of Decision objects. 
 * It is the first thing to run in a Step, and all RoRSeqFilter within a Step must run before the Step proper can run.
 * It manages the inter-Step logic and gates the execution of all algorithms needed by the Filter's set of chains 
 * within the following Step. 
 *
 * Typically each input collection will correspond to a collection of reconstructed objects which have been subject to a 
 * hypothesis algorithm. There is hence a one-to-one mapping between each input Decision object and a physics object in another
 * collection (link key: "feature"). The decision is additionally decorated with the chain-ID of all Hypothesis Tools which passed
 * the object in whole/as part of their selection.
 *
 * Execution of the RoRSeqFilter is managed by Control Flow, it will be scheduled once all hypothesis algorithms in the 
 * previous step which feed it have finished or are known to not run in the event.
 * It attempts to read in all implicit read handles.
 *
 * All successfully read handles are filtered with respect to m_chainsProperty. Only Decision objects which contain an
 * affirmative decision from at least one of the Chain-IDs within m_chainsProperty will be duplicated into the output collection.
 * All non-empty output collections will be written to their write handles.
 *
 * If at least one (non-empty) write handle is written, the filter will report TRUE to the Scheduler, unlocking the next Step
 * of execution for the set of Chains which utilise this Filter in their path through the HLT Control Flow graph. Otherwise,
 * it will report a filter decision of FALSE and terminate.
 *
 * Note: Algorithms and hypos can be placed under the control flow of more than one RoRSeqFilter, in this case, only one is
 * required to return a positive filter decision to unlock the control flow for this set of algorithms and hypo. 
 **/

class RoRSeqFilter
  : public ::AthAlgorithm
{ 
 public: 
  RoRSeqFilter( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~RoRSeqFilter(); 

/**
 * @brief Setup input and output handles. Renounce all input handles. Get IDs for all configured chains.
 **/
  virtual StatusCode  initialize() override;
  
/**
 * @brief Apply this filter in-between Steps of trigger execution. Fully implicit inputs, requires Control Flow to unlock.
 * will signal a negative filter result to the Scheduler if zero chains remain active upon termination.
 **/
  virtual StatusCode  execute() override;

/**
 * @brief Currently a noop
 **/
  virtual StatusCode  finalize() override;

 private: 
  RoRSeqFilter();
  SG::ReadHandleKeyArray<TrigCompositeUtils::DecisionContainer>  m_inputKeys{ this, "Input", {}, "Inputs to the filter" };
  SG::WriteHandleKeyArray<TrigCompositeUtils::DecisionContainer> m_outputKeys{ this, "Output", {}, "Output" };

  Gaudi::Property<std::vector<std::string> > m_chainsProperty{ this, "Chains", {}, "Chains of which this filter is concerned" };
  Gaudi::Property<bool> m_mergeInputs{ this, "MergeInputs", false, "Produce one output" };
  std::set<HLT::Identifier> m_chains;


/**
 * @brief Applies generic filter to input container, keeping only the decision objects with at least 
 * one affirmative decision from the previous Stage. Considering only decisions from chains utilising this filter.
 * @param inputKey Storegate key of input, needed to link newly created decision objects to their parents.
 * @param output Writeable output container to store copies of decision objects which pass the filter.
 * @return The number of decision objects which passed the filter.
 *
 * Produced a selective copy of all Decision objects in the input container which possess a positive decision from
 * at least one of the chains which this filter algorithm is configured for. This new object is linked to its 
 * parent and stored in the output collection. It will form the starting point for the next Step. 
 **/
  size_t copyPassing( const TrigCompositeUtils::DecisionContainer& input, 
                      TrigCompositeUtils::DecisionContainer& output) const;

}; 


#endif //> !DECISIONHANDLING_RORSEQFILTER_H
