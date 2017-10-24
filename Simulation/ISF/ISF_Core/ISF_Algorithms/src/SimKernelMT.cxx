/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @author Elmar Ritsch <Elmar.Ritsch@cern.ch>
 * @date October 2016
 * @brief Core Athena algorithm for the Integrated Simulation Framework
 */

#include "SimKernelMT.h"

// STL
#include <queue>

// ATLAS cxx utils
#include "CxxUtils/make_unique.h"


ISF::SimKernelMT::SimKernelMT( const std::string& name, ISvcLocator* pSvcLocator ) :
    ::AthAlgorithm( name, pSvcLocator ),
    m_inputEvgenKey(),
    m_outputTruthKey(),
    m_inputConverter("", name)
{
    declareProperty("InputEvgenCollection",
                    m_inputEvgenKey,
                    "Input EVGEN collection.");
    declareProperty("OutputTruthCollection",
                    m_outputTruthKey,
                    "Output Truth collection.");
    declareProperty("InputConverter",
                    m_inputConverter,
                    "Input McEventCollection->ISFParticleContainer conversion service.");
}


ISF::SimKernelMT::~SimKernelMT() {
}


StatusCode ISF::SimKernelMT::initialize() {

  ATH_CHECK( m_inputEvgenKey.initialize() );
  ATH_CHECK( m_outputTruthKey.initialize() );

  ATH_CHECK( m_inputConverter.retrieve() );

  return StatusCode::SUCCESS;
}


StatusCode ISF::SimKernelMT::execute() {

  SG::ReadHandle<McEventCollection> inputEvgen(m_inputEvgenKey);
  if (!inputEvgen.isValid()) {
    ATH_MSG_FATAL("Unable to read input GenEvent collection '" << inputEvgen.key() << "'");
    return StatusCode::FAILURE;
  }

  // copy input Evgen collection to output Truth collection
  SG::WriteHandle<McEventCollection> outputTruth(m_outputTruthKey);
  outputTruth = CxxUtils::make_unique<McEventCollection>(*inputEvgen);

  // read and convert input
  ISFParticleContainer simParticles; // particles for ISF simulation
  bool isPileup = false;
  ATH_CHECK( m_inputConverter->convert(*outputTruth, simParticles, isPileup) );

  // loop until there are no more particles to simulate
  while ( simParticles.size() ) {
    //auto& curParticle = simParticles.back();
    simParticles.pop_back();

    ISFParticleContainer newSecondaries;
    // TODO (this is work in progress and these might be the next steps):
    //auto& simSvc = identifySimulator(curParticle);
    //auto& newSecondaries = simSvc.simulate( std::move(curParticle) );

    simParticles.splice( end(simParticles), std::move(newSecondaries) );
  }

  return StatusCode::SUCCESS;
}


StatusCode ISF::SimKernelMT::finalize() {
  return StatusCode::SUCCESS;
}
