/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInterfaces/ComboAlgo.h"

#include "TrigInterfaces/AlgoConfig.h"
#include "TrigInterfaces/ExceptionHandler.h"
#include "TrigNavigation/ComboIterator.h"

#include "TrigNavigation/Navigation.h"
#include "TrigNavigation/TriggerElement.h"

#include "GaudiKernel/GaudiException.h"


using namespace HLT;

ComboAlgo::ComboAlgo(const std::string& name, ISvcLocator* pSvcLocator)
  : TECombineAlgo(name, pSvcLocator), m_combIter(0),
    m_combinationsCount(0),
    m_successfulCombinationsCount(0){ 

  declareProperty("CombinationsLimit", m_combinationsLimit=0, 
		  "The number which limits maximum number pf created combinatorial TEs. 0==no limit");
  
  declareProperty( "SuccessfulCombinationsLimit", m_successfulCombinationsLimit=0, 
		   "The number which limits maximum number of good (passed by acceptImputs method) combinations. 0==no limit");
}

ComboAlgo::~ComboAlgo() {
  if ( m_combIter )
    delete m_combIter;
}

ErrorCode ComboAlgo::init_inputTE_iteration(std::vector<unsigned int>& inputs, const std::vector<unsigned int>& topologicalStartFrom)
{    
  if ( m_combIter )
    delete m_combIter;

  if(topologicalStartFrom.size()==0)
    m_combIter = newComboIterator(inputs);
  else
    m_combIter = newComboIterator(inputs, topologicalStartFrom);

  m_combinationsCount = 0;
  m_successfulCombinationsCount = 0;

  return HLT::OK;
}


ErrorCode ComboAlgo::next_inputTE_iteration(TEVec& inputTEs, const std::vector<unsigned int>& /*topologicalStartFrom*/)
{   
  inputTEs.clear();          
  // see if we exceeded limit of combinations
  if ( m_combinationsLimit != 0 ) // is set
    if ( m_combinationsCount >= m_combinationsLimit ) {      
      ATH_MSG_DEBUG  ( "Hit limit for the number of combinations " 
		       << m_combinationsCount  << " >= " << m_combinationsLimit );
      return HLT::OK; 
    }
  // see if we exceeded limit of good combinations
  if ( m_successfulCombinationsLimit != 0 )
    if ( m_successfulCombinationsCount >= m_successfulCombinationsLimit ) {      
      ATH_MSG_DEBUG  ( "Hit limit for the number of successful combinations " 
		       << m_successfulCombinationsCount << " >= " << m_successfulCombinationsLimit );
      return HLT::OK; 
    }
    
  // Found one: return the result (increment iterator so we don't get it again next time!)
  inputTEs = **m_combIter;
  ++(*m_combIter);
  return HLT::OK;
}

// This is called if algo is not in 1st position -- should never happen for Topologicals.

ErrorCode ComboAlgo::plugin_processTE(TEConstVec& inputTEs, TriggerElement* outputTE)
{
  try {
    return hltExecute(inputTEs, outputTE);
  } catch (...) {
    return trigExceptionHandler(&msg());
  }     
}

ErrorCode ComboAlgo::plugin_acceptInputs(TEVec& inputTEs, bool& pass) {    
    // Check precuts
  TEConstVec constVec = constify(inputTEs);
  try {
    ErrorCode ec = acceptInputs(constVec, pass);    

    m_combinationsCount++;
    if (pass) m_successfulCombinationsCount ++;
    return ec;
  } catch (...) {
    return trigExceptionHandler(&msg());
  }
}


bool ComboAlgo::reset() {
  return TECombineAlgo::reset();
}

HLT::ErrorCode
ComboAlgo::processRobRequests(const std::vector< unsigned int >& inputs ) {

  // to process the input ROBs it is not necessary to loop through the
  // combinations, so the implementation is actually the same as in
  // AllTEAlgo::processRobRequests

  std::vector<TEConstVec> inputSets;

  for (std::vector<unsigned int>::const_iterator input = inputs.begin();
       input != inputs.end(); ++input) {

    TEVec inputSet;
    config()->getNavigation()->getAllOfType(*input, inputSet);
    inputSets.push_back(constify(inputSet));
  }

  ErrorCode ec = HLT::OK;
  try {
    ATH_MSG_DEBUG ( "ComboAlgo::processRobRequests(): calling prepareRobRequests() of ComboAlgo implementation" );
    ec = prepareRobRequests(inputSets );
  } catch (...) {
    ec = trigExceptionHandler(&msg());
  } 
  return ec;
}


HLT::ErrorCode
ComboAlgo::prepareRobRequests(const std::vector<HLT::TEConstVec>& /*inputs*/ ) {
  return HLT::ErrorCode( Action::CONTINUE );
}
