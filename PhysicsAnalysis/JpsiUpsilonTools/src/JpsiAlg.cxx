/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// JpsiAlg.cxx
// Author: James Catmore (James.Catmore@cern.ch), E. Bouhova-Thacker (e.bouhova@cern.ch)
//         James Walder (jwalder@cern.ch)
// This algorithm calls the JpsiFinder tool and writes the resulting ExtendedVxCandidates to 
// StoreGate. All analyses requiring J/psi and upsilon should therefore call this first in job
// options and collect the J/psi candidates from StoreGate. Example is in JpsiExample. 

#include "JpsiUpsilonTools/JpsiAlg.h"


#include "xAODTracking/VertexAuxContainer.h"


//////////////////////////////////////////////////////////////

JpsiAlg::JpsiAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_eventCntr(0), m_jpsiCntr(0),
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

  // get the JpsiFinder tool
  ATH_CHECK(m_jpsiFinder.retrieve());
  ATH_CHECK(m_jpsiContainerName.initialize());
  m_eventCntr = 0;
  m_jpsiCntr = 0;

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode JpsiAlg::execute() {


  ATH_MSG_DEBUG("in execute()");
  const EventContext& ctx = Gaudi::Hive::currentContext();

  // Increment counter
  ++m_eventCntr;

  // Jpsi container and its auxilliary store
  std::unique_ptr<xAOD::VertexContainer> jpsiContainer = std::make_unique<xAOD::VertexContainer>();
  std::unique_ptr<xAOD::VertexAuxContainer> jpsiAuxContainer = std::make_unique<xAOD::VertexAuxContainer>();
  jpsiContainer->setStore(jpsiAuxContainer.get());
  // call Jpsi finder
  if( !m_jpsiFinder->performSearch(ctx, *jpsiContainer).isSuccess() ) {
    ATH_MSG_FATAL("Jpsi finder (" << m_jpsiFinder << ") failed.");
    return StatusCode::FAILURE;
  }

  // Extracting information from the Jpsi candidates
  m_jpsiCntr += jpsiContainer->size(); // Count the Jpsis

  // save in the StoreGate
  ATH_MSG_DEBUG("Recording to StoreGate: " << m_jpsiContainerName.key() << " size:" <<jpsiContainer->size());
  
  SG::WriteHandle<xAOD::VertexContainer> whandle (m_jpsiContainerName);
  ATH_CHECK(whandle.record(std::move(jpsiContainer), std::move(jpsiAuxContainer)));
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
  ATH_MSG_INFO("Total number of events analysed: " << m_eventCntr);
  ATH_MSG_INFO("Total number of jpsi candidates: " << m_jpsiCntr); 

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

