/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENT_BOOKKEEPER_TOOLS__CUT_BOOKKEEPERS_LOCAL_CACHE_H
#define EVENT_BOOKKEEPER_TOOLS__CUT_BOOKKEEPERS_LOCAL_CACHE_H

/// @author Tadej Novak <tadej@cern.ch>
#include <xAODCutFlow/CutBookkeeperContainer.h>
#include <xAODCutFlow/CutBookkeeperAuxContainer.h>


/// Helper in-memory structure
struct CutBookkeepersLocalCache final
{
  std::vector<std::unique_ptr<xAOD::CutBookkeeperContainer>> cont;
  std::vector<std::unique_ptr<xAOD::CutBookkeeperAuxContainer>> aux;

  bool empty() const { return cont.empty(); }
  std::size_t size() const { return cont.size(); }
  xAOD::CutBookkeeperContainer *at(std::size_t n) const { return cont.at(n).get(); }
  void clear() { cont.clear(); aux.clear(); }

  // Helper function to initialise the cache
  static void prepareContainers(CutBookkeepersLocalCache &target,
                                size_t size,
                                bool extend = false)
  {
    if (!target.empty() && !extend) {
      return;
    }

    size_t toAdd = size - target.size();
    for (std::size_t i = 0; i < toAdd; ++i) {
      auto container = std::make_unique<xAOD::CutBookkeeperContainer>();
      auto auxContainer = std::make_unique<xAOD::CutBookkeeperAuxContainer>();
      container->setStore(auxContainer.get());
      target.cont.push_back(std::move(container));
      target.aux.push_back(std::move(auxContainer));
    }
  }
};

#endif // EVENT_BOOKKEEPER_TOOLS__CUT_BOOKKEEPERS_LOCAL_CACHE_H
