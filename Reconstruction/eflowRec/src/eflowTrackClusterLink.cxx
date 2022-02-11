/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/*
 * eflowTrackClusterLink.cxx
 *
 *  Created on: 30.09.2013
 *      Author: tlodd
 */

#include "eflowRec/eflowTrackClusterLink.h"
#include "AthenaKernel/SlotSpecificObj.h"
#include "CxxUtils/checker_macros.h"



eflowTrackClusterLink*
eflowTrackClusterLink::getInstance(eflowRecTrack* track,
                                   eflowRecCluster* cluster,
                                   const EventContext& ctx)
{
  static SG::SlotSpecificObj<Cache> slots ATLAS_THREAD_SAFE;
  Cache& c = *slots.get (ctx);
  std::lock_guard lock (c.m_mutex);
  if (c.m_evt != ctx.evt()) {
    c.m_map.clear();
    c.m_evt = ctx.evt();
  }

  key_t key (track, cluster);
  auto it = c.m_map.try_emplace (key, std::unique_ptr<eflowTrackClusterLink>()).first;
  if (!it->second) {
    it->second = std::make_unique<eflowTrackClusterLink>(track,cluster);
  }
  return it->second.get();
}
