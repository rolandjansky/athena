/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: getCaloRingsDecorator.h 668862 2015-05-20 20:15:30Z wsfreund $
#ifndef XAODCALORINGS_UTILS_GETCALORINGSDECORATOR_H
#define XAODCALORINGS_UTILS_GETCALORINGSDECORATOR_H

// Athena includes:
#include "AthContainers/AuxElement.h"

// Local includes:
#include "xAODCaloRings/CaloRingsContainer.h"

namespace xAOD {

/**
 * @brief Get CaloRings decorator with write permissions even on const
 *        containers.
 **/
caloRingsDeco_t* getCaloRingsDecorator();

/**
 * @brief Get CaloRings accessor with read only permissions.
 **/
caloRingsReader_t* getCaloRingsReader();

} // namespace xAOD

#endif // XAODCALORINGS_TOOLS_GETCALORINGSDECORATOR_H
