/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * eflowTrackClusterLink.cxx
 *
 *  Created on: 30.09.2013
 *      Author: tlodd
 */

#include "eflowRec/eflowTrackClusterLink.h"

std::map<std::pair<eflowRecTrack*, eflowRecCluster*>, eflowTrackClusterLink*> eflowTrackClusterLink::m_instances;

void eflowTrackClusterLink::clearInstances() {
  InstanceMap::iterator itInstance = m_instances.begin();
  InstanceMap::iterator endInstance = m_instances.end();
  for (; itInstance != endInstance; ++itInstance) {
    delete itInstance->second;
  }
  m_instances.clear();
}
