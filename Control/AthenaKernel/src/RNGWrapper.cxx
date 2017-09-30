/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaKernel/RNGWrapper.h"
#include "CLHEP/Random/RandomEngine.h"

ATHRNG::RNGWrapper::RNGWrapper(factoryFunc& genFact, size_t nSlots)
{
  // Construct the random engines; one per event slot.
  m_engines.reserve(nSlots);
  for(size_t t = 0; t < nSlots; t++){
    m_engines.emplace_back(genFact());
  }
}

ATHRNG::RNGWrapper::~RNGWrapper()
{
  // Clean up the allocated engines
  for(auto engPtr : m_engines) {
    delete engPtr;
  }
}

void ATHRNG::RNGWrapper::setSeed(const std::string& algName, const EventContext& ctx)
{
  setSeed( algName, ctx.slot(),
           ctx.eventID().event_number(),
           ctx.eventID().run_number() );
}

void ATHRNG::RNGWrapper::setSeed(const std::string& algName, size_t slot,
                                 uint64_t ev, uint64_t run)
{
  auto algHash = std::hash<std::string>{}(algName);
  auto evHash = std::hash<uint64_t>{}(ev);
  auto runHash = std::hash<uint64_t>{}(run);
  auto hsh = evHash ^ (runHash + (evHash << 6) + (evHash >> 2));
  hsh = hsh ^ (algHash + (hsh << 6) + (hsh >> 2));
  setSeed(slot, hsh);
}

void ATHRNG::RNGWrapper::setSeed(size_t slot, size_t seed)
{
  m_engines[slot]->setSeed(seed, 0);
}
