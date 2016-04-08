/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JpsiAlg.cxx
// Author: James Catmore (James.Catmore@cern.ch), E. Bouhova-Thacker (e.bouhova@cern.ch)
//         James Walder (jwalder@cern.ch)
// This algorithm calls the JpsiFinder tool and writes the resulting ExtendedVxCandidates to 
// StoreGate. All analyses requiring J/psi and upsilon should therefore call this first in job
// options and collect the J/psi candidates from StoreGate. Example is in JpsiExample. 

#include "JpsiUpsilonTools/JpsiAlg.h"

#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"


//////////////////////////////////////////////////////////////

JpsiAlg::JpsiAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  eventCntr(0), jpsiCntr(0),
  m_jpsiFinder("Analysis::JpsiFinder"),
  m_jpsiContainerName("JpsiCandidates")
{
  
  // Declare user-defined properties - cuts and vertexing method
  declareProperty("JpsiFinderName",m_jpsiFinder);
  declareProperty("JpsiCandidatesOutputName",m_jpsiContainerName="JpsiCandidates");
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode JpsiAlg::initialize(){


  ATH_MSG_DEBUG("in initialize()");

  // get the tool service
  IToolSvc* toolSvc;
  StatusCode sc = service("ToolSvc",toolSvc);
  if (StatusCode::SUCCESS != sc) {
    ATH_MSG_ERROR("Unable to retrieve ToolSvc");
    return StatusCode::FAILURE;
  }
 
  // get the JpsiFinder tool
  if ( m_jpsiFinder.retrieve().isFailure() ) {
    ATH_MSG_FATAL("Failed to retrieve tool " << m_jpsiFinder);
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO("Retrieved tool " << m_jpsiFinder);
  }

  eventCntr = 0;
  jpsiCntr = 0;

  return sc;
  
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode JpsiAlg::execute() {


  ATH_MSG_DEBUG("in execute()");

  // Increment counter
  ++eventCntr;

  // Jpsi container and its auxilliary store
  xAOD::VertexContainer*    jpsiContainer = NULL;
  xAOD::VertexAuxContainer* jpsiAuxContainer = NULL;
  
  // call Jpsi finder
  if( !m_jpsiFinder->performSearch(jpsiContainer, jpsiAuxContainer).isSuccess() ) {
    ATH_MSG_FATAL("Jpsi finder (" << m_jpsiFinder << ") failed.");
    return StatusCode::FAILURE;
  }

  // Extracting information from the Jpsi candidates
  jpsiCntr += jpsiContainer->size(); // Count the Jpsis

  // save in the StoreGate
  ATH_MSG_DEBUG("Recording to StoreGate: " << m_jpsiContainerName << " size:" <<jpsiContainer->size());
  
  if (!evtStore()->contains<xAOD::VertexContainer>(m_jpsiContainerName))       
    CHECK(evtStore()->record(jpsiContainer, m_jpsiContainerName));
  
  if (!evtStore()->contains<xAOD::VertexAuxContainer>(m_jpsiContainerName+"Aux.")) 
    CHECK(evtStore()->record(jpsiAuxContainer, m_jpsiContainerName+"Aux."));
  
  // END OF ANALYSIS
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode JpsiAlg::finalize() {

  ATH_MSG_DEBUG("in finalize()");
  ATH_MSG_INFO("===================");
  ATH_MSG_INFO("SUMMARY OF ANALYSIS");
  ATH_MSG_INFO("===================");
  ATH_MSG_INFO(" ");
  ATH_MSG_INFO("Total number of events analysed: " << eventCntr);
  ATH_MSG_INFO("Total number of jpsi candidates: " << jpsiCntr); 

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

