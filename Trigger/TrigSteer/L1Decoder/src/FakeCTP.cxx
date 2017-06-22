/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <fstream>
#include "CxxUtils/make_unique.h"
#include "L1Decoder/TrigIdentifiers.h"
#include "./FakeCTP.h"


FakeCTP::FakeCTP(const std::string& name, ISvcLocator* pSvcLocator) 
  : AthAlgorithm(name, pSvcLocator),
    m_inputFileName("ChainsToActivate"), 
    m_decisions("CTPDecisions"),
    m_decisionsAux("CTPDecisionsAux."),    
    m_currentEvent(0) {  
  declareProperty("InputFilename", m_inputFileName, "FakeROI input filename");
  declareProperty("OutputDecisions", m_decisions, "Name of the chains decisions object (one decision obj per chain)");
  declareProperty("OutputDecisionsAux", m_decisionsAux, "Aux for chains decision");
  
}

StatusCode FakeCTP::parseInputFile() {
  std::ifstream inputFile(m_inputFileName);
  if ( not inputFile.good() ) {
    ATH_MSG_ERROR("Input file " << m_inputFileName << " inaccessible");
    return StatusCode::FAILURE;
  }
    
  std::string chain;
  m_events.resize(1);
  while ( inputFile ) {
    inputFile >> chain; 
    m_events.back().push_back(TrigConf::HLTUtils::string2hash(chain, "chain"));
    while ( inputFile and inputFile.peek() == ' ') inputFile.ignore(); // read till the end of line or new chain name
    if ( inputFile and inputFile.peek() == '\n') 
      m_events.resize( m_events.size() + 1 ); // new event
  }
  inputFile.close();  
  return StatusCode::SUCCESS;
}

StatusCode FakeCTP::initialize() {
  CHECK( parseInputFile() );
  return StatusCode::SUCCESS;
}

StatusCode FakeCTP::execute() {
  m_decisions = CxxUtils::make_unique< xAOD::TrigCompositeContainer >();
  m_decisionsAux = CxxUtils::make_unique< xAOD::TrigCompositeAuxContainer>();
  m_decisions->setStore(m_decisionsAux.ptr());



  
  for ( const auto& chain : m_events[m_currentEvent] ) {
    ATH_MSG_DEBUG("Enabling chain " << TrigConf::HLTUtils::hash2string(chain, "chain"));
    xAOD::TrigComposite * decision = new xAOD::TrigComposite();
    m_decisions->push_back(decision);
    decision->setDetail("ID", ChainID(chain));
    decision->setDetail("Decision", 1);
  }


  m_currentEvent += 1;
  if ( m_currentEvent >= m_events.size() ) 
    m_currentEvent = 0;
  return StatusCode::SUCCESS;
}
