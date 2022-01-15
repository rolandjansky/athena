/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaKernel/RNGWrapper.h"
#include "CLHEP/Random/RandomEngine.h"
#include "crc_combine.h"

#undef NDEBUG

ATHRNG::RNGWrapper::RNGWrapper(const factoryFunc& genFact, size_t nSlots)
{
  // Construct the random engines; one per event slot.
  m_engines.reserve(nSlots);
  for(size_t t = 0; t < nSlots; t++){
    m_engines.emplace_back(genFact());
  }
  m_evtSeeded.resize (nSlots, EventContext::INVALID_CONTEXT_EVT);
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
           ctx.eventID().run_number(),
           ctx.evt() );
}

void ATHRNG::RNGWrapper::setSeed(const std::string& algName, size_t slot,
                                 uint64_t ev, uint64_t run,
                                 EventContext::ContextEvt_t evt /*= EventContext::INVALID_CONTEXT_EVT*/)
{
  auto algHash = std::hash<std::string>{}(algName);
  auto evHash = std::hash<uint64_t>{}(ev);
  auto runHash = std::hash<uint64_t>{}(run);
  auto hsh = evHash ^ (runHash + (evHash << 6) + (evHash >> 2));
  hsh = hsh ^ (algHash + (hsh << 6) + (hsh >> 2));

  setSeed(slot, hsh, evt);
}

void ATHRNG::RNGWrapper::setSeedLegacy(const std::string& algName, const EventContext& ctx, uint32_t offset, bool useLegacy,
                                       EventContext::ContextEvt_t evt /*= EventContext::INVALID_CONTEXT_EVT*/)
{
  if (useLegacy) {
    // Use legacy seeding
    setSeedLegacy( algName, ctx.slot(),
                   ctx.eventID().event_number(),
                   ctx.eventID().run_number(),
                   offset,
                   evt);
  }
  else {
    // Use new seeding
    setSeed( algName, ctx.slot(),
             ctx.eventID().event_number(),
             ctx.eventID().run_number(),
             evt );
  }
}

void ATHRNG::RNGWrapper::setSeedLegacy(const std::string& algName, size_t slot, uint64_t ev, uint64_t run, uint32_t offset,
                                       EventContext::ContextEvt_t evt /*= EventContext::INVALID_CONTEXT_EVT*/)
{
  uint32_t theHash = static_cast<uint32_t>(ev);
  if (0 != offset) theHash=crc_combine(theHash, offset);
  uint32_t runNumber = static_cast<uint32_t>(run);
  theHash=crc_combine(theHash, runNumber);
  theHash=crc_combine(theHash, algName);
  #ifdef NDEBUG
  std::cout << "RNGWrapper Reseeding stream " << algName
            << " with eventNumber " << ev
            << ", runNumber " << run
            << ", applied offset " <<  offset
            << ". Final seed " << theHash << std::endl;
  #endif
  setSeed(slot, theHash, evt);
}

void ATHRNG::RNGWrapper::setSeed(size_t slot, size_t seed,
                                 EventContext::ContextEvt_t evt /*= EventContext::INVALID_CONTEXT_EVT*/)
{
  m_engines[slot]->setSeed(seed, 0);
  if (evt == EventContext::INVALID_CONTEXT_EVT) {
    evt = Gaudi::Hive::currentContext().evt();
  }
  m_evtSeeded[slot] = evt;
}
