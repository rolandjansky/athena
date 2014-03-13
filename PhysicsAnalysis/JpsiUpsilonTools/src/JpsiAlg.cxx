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

#include <sstream>                                      // C++ utilities
#include <string>
#include <algorithm>
#include <fstream>

#include "GaudiKernel/ISvcLocator.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "DataModel/DataVector.h"
#include "DataModel/ElementLink.h"
#include "DataModel/DataVector.h"

#include "StoreGate/StoreGateSvc.h"             // Storegate stuff
#include "StoreGate/DataHandle.h"
#include "AthenaKernel/DefaultKey.h"

//////////////////////////////////////////////////////////////

JpsiAlg::JpsiAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_jpsiFinder("Analysis::JpsiFinder"),
  m_JpsiCandidatesOutputName("JpsiCandidates")
{
  
  // Declare user-defined properties - cuts and vertexing method
  declareProperty("JpsiFinderName",m_jpsiFinder);
  declareProperty("JpsiCandidatesOutputName",m_JpsiCandidatesOutputName="JpsiCandidates");
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

  // Declare the VxContainer which will hold the Jpsis
  VxContainer* JpsiContainer ( 0 );

  // Get the Jpsi candidates and associated values
  JpsiContainer = m_jpsiFinder->performSearch();

  // Extracting information from the Jpsi candidates
  jpsiCntr += JpsiContainer->size(); // Count the Jpsis

  ATH_MSG_DEBUG("Recording to StoreGate: " << m_JpsiCandidatesOutputName << " size:" <<JpsiContainer->size());
  StatusCode sc = evtStore()->record(JpsiContainer, m_JpsiCandidatesOutputName,false);
  if (sc.isFailure()) {
    ATH_MSG_ERROR("Unable to record JpsiContainer in TDS");
    return sc;
  }

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

