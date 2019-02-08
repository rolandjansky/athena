/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthRNGSvc.h"
#include "AthenaKernel/RNGWrapper.h"
#include "AthenaKernel/SlotSpecificObj.h"

#include "AtlasCLHEP_RandomGenerators/dSFMTEngine.h"
#include "CLHEP/Random/MixMaxRng.h"
#include "CLHEP/Random/Ranlux64Engine.h"
#include "CLHEP/Random/RanecuEngine.h"


AthRNGSvc::AthRNGSvc(const std::string& name, ISvcLocator* svc)
  : base_class(name, svc),
    m_rngType("dSFMT")
{
  declareProperty("EngineType", m_rngType, "CLHEP RandomEngine type");
}

StatusCode AthRNGSvc::initialize()
{
  if(m_rngType == "dSFMT") {
    m_fact = [](void)->CLHEP::HepRandomEngine*{
      return new CLHEP::dSFMTEngine();
    };
  } else if(m_rngType == "Ranlux64") {
    m_fact = [](void)->CLHEP::HepRandomEngine*{
      return new CLHEP::Ranlux64Engine();
    };
  } else if(m_rngType == "Ranecu") {
    m_fact = [](void)->CLHEP::HepRandomEngine*{
      return new CLHEP::RanecuEngine();
    };
  } else if(m_rngType == "MixMax") {
    m_fact = [](void)->CLHEP::HepRandomEngine*{
      return new CLHEP::MixMaxRng();
    };
  } else {
    ATH_MSG_WARNING("Supported Generator types are 'dSFMT', 'Ranlux64', 'Ranecu', 'MixMax'");
    ATH_MSG_FATAL("Generator type \"" << m_rngType << "\" is not known. Check Joboptions");
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
}

ATHRNG::RNGWrapper* AthRNGSvc::getEngine(const INamedInterface* client,
                                         const std::string& streamName)
{
  // Retrieve number of event slots
  size_t numSlots = SG::getNSlots();

  // The name key combines the client name and optional stream name
  std::string rngName = client->name();
  if(!streamName.empty()) rngName += "/" + streamName;

  // Look for an existing RNG stream
  std::lock_guard<std::mutex> lock(m_mutex);
  auto it = m_wrappers.find(rngName);

  // Construct a new RNGWrapper if none exists
  if(it == m_wrappers.end()){
    ATH_MSG_INFO("Creating engine " << rngName);
    auto wrp = new ATHRNG::RNGWrapper(m_fact, numSlots);
    m_wrappers.insert( std::make_pair(rngName, wrp) );
    return wrp;
  }

  ATH_MSG_DEBUG("Returning engine " << rngName);
  return it->second;
}

void AthRNGSvc::printEngineState(const INamedInterface* client,
                                 const std::string& streamName)
{
  // Retrieve the current slot's engine
  ATHRNG::RNGWrapper* wrapper = getEngine(client, streamName);
  CLHEP::HepRandomEngine* engine( *wrapper );

  // Extract the engine state numbers
  std::vector<unsigned long> rngStates = engine->put();

  // Print the state numbers
  std::string rngName = client->name();
  if(!streamName.empty()) rngName += "/" + streamName;
  std::lock_guard<std::mutex> lock(m_mutex);
  msg(MSG::ALWAYS) << rngName << " ";
  // We mask 32 bits because the other bits are garbage and unused
  for(const unsigned long s : rngStates) {
    msg() << (s & 0xffffffffu) << " ";
  }
  msg() << endmsg;
}

AthRNGSvc::~AthRNGSvc()
{
  // Clean up the RNGWrappers and HepRandomEngines
  for(auto& wrapperPair : m_wrappers) {
    delete wrapperPair.second;
  }
}
