/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   AFPProtonContainer_v1.h
 * @author Krzysztof Cieśla <krzysztof.marcin.ciesla@cern.ch>
 * @date   2018-04-05
 * 
 * @brief  Header file with typedef for class AFPProtonContainer_v1
 * 
 */

#ifndef XAODFORWARD_VERSIONS_AFPPROTONCONTAINER_V1_H
#define XAODFORWARD_VERSIONS_AFPPROTONCONTAINER_V1_H

// EDM include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODForward/versions/AFPProton_v1.h"

namespace xAOD {

    typedef DataVector< AFPProton_v1 > AFPProtonContainer_v1;

} // namespace xAOD

#endif
