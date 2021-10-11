/*
 Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file ComponentParameters.h
 * @date Sunday 8th May 2005
 * @author atkinson, Anthony Morley, Christos Anastopoulos
 * @brief  Definition of component parameters for use in a mixture
 * of many components. In this regime each track parameters
 * object comes with a weighting (double) attached
 */

#ifndef TrkComponentParameters
#define TrkComponentParameters

#include "TrkParameters/TrackParameters.h"

namespace Trk {
/**
 * Component Parameters is just a typedef
 */
using ComponentParameters =
  std::pair<std::unique_ptr<Trk::TrackParameters>, double>;
}

#endif
