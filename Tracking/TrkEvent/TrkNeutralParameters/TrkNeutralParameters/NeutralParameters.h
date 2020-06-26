/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// NeutralParameters.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKNEUTRALPARAMETERS_NEUTRALPARAMETERS_H
#define TRKNEUTRALPARAMETERS_NEUTRALPARAMETERS_H

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
using NeutralParameters = ParametersBase<5, Neutral>;
using NeutralCurvilinearParameters = CurvilinearParametersT<5, Neutral, PlaneSurface>;
using NeutralAtaCone = ParametersT<5, Neutral, ConeSurface>;
using NeutralAtaCylinder = ParametersT<5, Neutral, CylinderSurface>;
using NeutralAtaDisc = ParametersT<5, Neutral, DiscSurface>;
using NeutralPerigee = ParametersT<5, Neutral, PerigeeSurface>;
using NeutralAtaPlane = ParametersT<5, Neutral, PlaneSurface>;
using NeutralAtaStraightLine = ParametersT<5, Neutral, StraightLineSurface>;
}

/**Overload of << operator for both, MsgStream and std::ostream for debug
 * output*/
MsgStream&
operator<<(MsgStream& sl, const Trk::NeutralParameters& pars);

/**Overload of << operator for both, MsgStream and std::ostream for debug
 * output*/
std::ostream&
operator<<(std::ostream& sl, const Trk::NeutralParameters& pars);

#endif // TRKNEUTRALPARAMETERS_NEUTRALPARAMETERS_H
