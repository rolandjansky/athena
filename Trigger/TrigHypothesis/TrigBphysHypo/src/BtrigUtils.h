/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BTRIGUTILS_H
#define BTRIGUTILS_H

#include <vector>
#include "xAODTracking/TrackParticle.h"
#include "GeoPrimitives/GeoPrimitives.h"  // for Vector3D

double PtVal(const std::vector<const xAOD::TrackParticle*>&);

double LxyVal(const double, const double, const Amg::Vector3D&, const Amg::Vector3D&);

#endif // BTRIGUTILS_H
