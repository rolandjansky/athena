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
 * @class $(klass)s
 * @brief 
 **/

class RoRSeqFilter
  : public ::AthAlgorithm
{ 
 public: 
  RoRSeqFilter( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~RoRSeqFilter(); 

  //RoRSeqFilter &operator=(const RoRSeqFilter &alg); 

  StatusCode  initialize() override;
  StatusCode  execute() override;
  StatusCode  finalize() override;

 private: 
  RoRSeqFilter();
  SG::ReadHandleKeyArray<TrigCompositeUtils::DecisionContainer> m_inputKeys{ this, "Input", {}, "Inputs to the filter" };
  SG::WriteHandleKeyArray< ConstDataVector<TrigCompositeUtils::DecisionContainer> > m_outputKeys{ this, "Output", {}, "Output" };

  Gaudi::Property<std::vector<std::string> > m_chainsProperty{ this, "Chains", {}, "Chains of whihc this filter is concerned" };
  Gaudi::Property<bool> m_mergeInputs{ this, "MergeInputs", false, "Produce one output" };
  std::set<HLT::Identifier> m_chains;

  size_t copyPassing( const TrigCompositeUtils::DecisionContainer& input, 
		      ConstDataVector<TrigCompositeUtils::DecisionContainer>& output ) const;

}; 


#endif //> !DECISIONHANDLING_RORSEQFILTER_H
