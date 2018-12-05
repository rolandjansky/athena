/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGUPGRADETEST_TESTRORSEQFILTER_H
#define TRIGUPGRADETEST_TESTRORSEQFILTER_H 1


#include <string>
#include "DecisionHandling/TrigCompositeUtils.h"
#include "AthenaBaseComps/AthAlgorithm.h"

namespace HLTTest {

  /**
   * @class TestRoRSeqFilter for validation of the CF sequencers barrier concept 
   * @brief This alg will read in implicit data, potentially merge, but above all it will decide to accep/reject 
   * and thus to continue reco in the sequence
   **/
  
  
  class TestRoRSeqFilter
    : public ::AthAlgorithm
  { 
  public: 
    TestRoRSeqFilter( const std::string& name, ISvcLocator* pSvcLocator );
    
    virtual ~TestRoRSeqFilter(); 
    
    //TestRoRSeqFilter &operator=(const TestRoRSeqFilter &alg); 
    
    StatusCode  initialize() override;
    StatusCode  execute( ) override;
    StatusCode  finalize() override;
    
  private: 
    TestRoRSeqFilter();
    std::vector<std::string> m_inputs;
    std::vector<std::string> m_outputs;
    
    std::vector<std::string> m_chainsProperty;
    TrigCompositeUtils::DecisionIDContainer  m_chains; // this is the set of chains we care about in this filter
    bool m_alwaysPass = false; // override decision 
    
    /*
      @brief fills the output with decision objects lining back to the succesfully passing objs
      NB> The positive/negative decision can be red from the output.size()
    */
    void copyPassing(const std::string& inputKey,
                     TrigCompositeUtils::DecisionContainer* output) const;
  }; 
  
} //> end namespace HLTTest
#endif //> !TRIGUPGRADETEST_TESTRORSEQFILTER_H
