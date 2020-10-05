/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackParameters.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKPARAMETERS_TRACKPARAMETERS_H
#define TRKPARAMETERS_TRACKPARAMETERS_H

#include "TrkParametersBase/CurvilinearParametersT.h"
#include "TrkParametersBase/ParametersT.h"
#include "TrkSurfaces/ConeSurface.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkSurfaces/DiscSurface.h"
#include "TrkSurfaces/PerigeeSurface.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkSurfaces/StraightLineSurface.h"

namespace Trk {

// Alias declarations
using TrackParameters = ParametersBase<5, Charged>;
using CurvilinearParameters = CurvilinearParametersT<5, Charged, PlaneSurface>;
using AtaCone = ParametersT<5, Charged, ConeSurface>;
using AtaCylinder = ParametersT<5, Charged, CylinderSurface>;
using AtaDisc = ParametersT<5, Charged, DiscSurface>;
using Perigee = ParametersT<5, Charged, PerigeeSurface>;
using AtaPlane = ParametersT<5, Charged, PlaneSurface>;
using AtaStraightLine = ParametersT<5, Charged, StraightLineSurface>;

}

/**Overload of << operator for both, MsgStream and std::ostream for debug
 * output*/
MsgStream&
operator<<(MsgStream& sl, const Trk::TrackParameters& pars);

/**Overload of << operator for both, MsgStream and std::ostream for debug
 * output*/
std::ostream&
operator<<(std::ostream& sl, const Trk::TrackParameters& pars);

#endif
