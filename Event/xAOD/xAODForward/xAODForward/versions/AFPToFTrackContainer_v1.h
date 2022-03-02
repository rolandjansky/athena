/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   AFPToFTrackContainer_v1.h
 * @author Leszek Adamczyk <leszek.adamczyk@agh.edu.pl>
 * @date   2021-11-11
 * 
 * @brief  Header file with typedef for AFPToFTrackContainer_v1
 * 
 */


#ifndef XAODFORWARD_VERSIONS_AFPTOFTRACKCONTAINER_V1_H
#define XAODFORWARD_VERSIONS_AFPTOFTRACKCONTAINER_V1_H

// EDM include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODForward/versions/AFPToFTrack_v1.h"

namespace xAOD {
  typedef DataVector< AFPToFTrack_v1 > AFPToFTrackContainer_v1;
}
 

#endif

