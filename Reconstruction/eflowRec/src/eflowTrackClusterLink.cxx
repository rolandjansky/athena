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

std::map<std::pair<eflowRecTrack*, eflowRecCluster*>, std::unique_ptr<eflowTrackClusterLink> > eflowTrackClusterLink::m_instances;
