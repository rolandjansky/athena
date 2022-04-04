/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   AFPVertexContainer_v1.h
 * @author Leszek Adamczyk <leszek.adamczyk@agh.edu.pl>
 * @date   2021-12-08
 * 
 * @brief  Header file with typedef for AFPVertexContainer_v1
 * 
 */


#ifndef XAODFORWARD_VERSIONS_AFPVERTEXCONTAINER_V1_H
#define XAODFORWARD_VERSIONS_AFPVERTEXCONTAINER_V1_H

// EDM include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODForward/versions/AFPVertex_v1.h"

namespace xAOD {
  typedef DataVector< AFPVertex_v1 > AFPVertexContainer_v1;
}
 

#endif

