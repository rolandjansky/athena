/*
 Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************************
  ComponentParameters.h  -  description
 -------------------------------------
begin                : Sunday 8th May 2005
author               : atkinson, amorley,anastopoulos
email                : Anthony.Morley@cern.ch
decription           : Definition of component parameters for use in a mixture
                       of many components. In this reigme each track parameters
                       object comes with a weighting (double) attached
 *********************************************************************************/

#ifndef TrkComponentParameters
#define TrkComponentParameters
#include "TrkParameters/TrackParameters.h"

namespace Trk{
/**
 * Typedefs using unique_ptr
 */
typedef std::pair<std::unique_ptr<Trk::TrackParameters>, double> SimpleComponentParameters;
typedef std::vector<SimpleComponentParameters> SimpleMultiComponentState;
/**
 * Component parameter typedef
 */
typedef std::pair<const TrackParameters*, double> ComponentParameters;//Needed by MultiComponentState
} // end Trk namespace

#endif
