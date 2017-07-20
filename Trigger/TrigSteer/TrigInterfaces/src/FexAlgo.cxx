/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInterfaces/FexAlgo.h"

#include "TrigNavigation/ComboIterator.h"
#include "TrigInterfaces/AlgoConfig.h"
#include "TrigInterfaces/ExceptionHandler.h"
#include "TrigNavigation/Navigation.h"

#include "GaudiKernel/GaudiException.h"

using namespace HLT;
  
FexAlgo::FexAlgo(const std::string& name, ISvcLocator* pSvcLocator)
  : TECombineAlgo(name, pSvcLocator) 
{  
  declareProperty("doCaching", m_doCaching  = true);
}


ErrorCode FexAlgo::init_inputTE_iteration(std::vector<unsigned int>& inputTypes, const std::vector<unsigned int>& /*topologicalStartFrom*/)
{
   if (inputTypes.size() != 1) 
      return HLT::ErrorCode( Action::ABORT_CHAIN, Reason::BAD_JOB_SETUP );  

  m_TEs.clear();
  config()->getNavigation()->getAllOfType(inputTypes[0], m_TEs);
  m_TEIter = m_TEs.begin();

  return HLT::OK;
}

ErrorCode FexAlgo::next_inputTE_iteration(TEVec& inputTEs, const std::vector<unsigned int>& /*topologicalStartFrom*/)
{
  inputTEs.clear();
  if ( m_TEIter != m_TEs.end() ) {
    inputTEs.push_back(*m_TEIter);
    ++m_TEIter;
  }
  return HLT::OK;
}

/*
  ErrorCode FexAlgo::next_inputTE_iteration(TEVec& inputTEs, const std::vector<unsigned int>& topologicalStartFrom)
  {
  inputTEs.clear();
  
  for (; m_TEIter != m_TEs.end(); ++m_TEIter) {

  // Get the input TE
  TriggerElement* inputTE = *m_TEIter;
  
  // Check if it is valid
  bool pass = true;
  
  beforeExecMonitors().ignore();    
  m_returnErrorCode = acceptInput(inputTE, pass);
  if (m_returnErrorCode.action() > HLT::Action::CONTINUE) {
  afterExecMonitors().ignore();    
  return m_returnErrorCode;
  }
  if (!pass) { 
  afterExecMonitors().ignore();    
  continue;
  }
  
  // Found one: return the result (increment iterator so we don't get it again next time!)
  inputTEs.push_back(inputTE);
  ++m_TEIter;
  return m_returnErrorCode = HLT::OK;
  }
  
  // out of TEs: return empty inputTEs
  return m_returnErrorCode = HLT::OK;
}
*/




ErrorCode FexAlgo::plugin_processTE(TEConstVec& inputTEs, TriggerElement* outputTE)
{
  if (inputTEs.size() != 1) return HLT::BAD_ALGO_CONFIG;
  try {    
    return hltExecute(inputTEs[0], outputTE); // actual execution
  } catch (...) {
    return trigExceptionHandler(&msg());
  } 
}


HLT::ErrorCode FexAlgo::plugin_acceptInputs(TEVec& inputTEs, bool& pass) {
  if (inputTEs.size() != 1) return HLT::BAD_ALGO_CONFIG;
  try {
    return acceptInput(inputTEs[0], pass);
  } catch (...) {
    return trigExceptionHandler(&msg());
  } 
}


HLT::ErrorCode
FexAlgo::processRobRequests(const std::vector< unsigned int >& inputs ) {

  // FEX should normally have exactly one input type but some chains call this method with 0 inputs
  if (inputs.size() != 1) {
    ATH_MSG_DEBUG ( "The size of the input to FexAlgo::processRobRequests(v<uint> input) is " << inputs.size() << ", should be 1! No ROBs for prefetching will be prepared");
    return HLT::OK;
  }

  // get input TEs from Navigation
  TEVec inputTEs;
  config()->getNavigation()->getAllOfType(inputs[0], inputTEs);

  // and make them const
  TEConstVec constInputTEs = constify(inputTEs);    

  // and call the ROB Request method
  ErrorCode ec = HLT::OK;
  try {
    ATH_MSG_DEBUG ( "FexAlgo::processRobRequests(): calling prepareRobRequests() of FexAlgo implementation" );
    ec = prepareRobRequests(constInputTEs);
  } catch (...) {
    ec = trigExceptionHandler(&msg());
  } 

  return ec;
}



HLT::ErrorCode
FexAlgo::prepareRobRequests(const HLT::TEConstVec& inputs ) {
   HLT::ErrorCode retCode( Action::CONTINUE );
   for(HLT::TEConstVec::const_iterator inp = inputs.begin();
       inp != inputs.end(); ++inp) {
      retCode = std::max(retCode, prepareRobRequests(*inp ) );
      if ( retCode.action() > HLT::Action::CONTINUE ) return retCode;
   }
   return retCode;
}


HLT::ErrorCode
FexAlgo::prepareRobRequests(const HLT::TriggerElement* /*input*/ ) {
  return HLT::ErrorCode( Action::CONTINUE );
}
