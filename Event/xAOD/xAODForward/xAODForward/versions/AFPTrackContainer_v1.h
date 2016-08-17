/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   AFPTrackContainer_v1.h
 * @author Grzegorz Gach <grzegorz.gach@cern.ch>
 * @date   2016-07-14
 * 
 * @brief  Header file with typedef for AFPTrackContainer_v1
 * 
 */


#ifndef XAODFORWARD_VERSIONS_AFPTRACKCONTAINER_V1_H
#define XAODFORWARD_VERSIONS_AFPTRACKCONTAINER_V1_H

// EDM include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODForward/versions/AFPTrack_v1.h"

namespace xAOD {
  typedef DataVector< AFPTrack_v1 > AFPTrackContainer_v1;
}
 

#endif

