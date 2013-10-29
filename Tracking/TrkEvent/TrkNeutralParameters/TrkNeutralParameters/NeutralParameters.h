/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// NeutralParameters.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKNEUTRALPARAMETERS_NEUTRALPARAMETERS_H
#define TRKNEUTRALPARAMETERS_NEUTRALPARAMETERS_H

#include "TrkParametersBase/ParametersT.h"
#include "TrkParametersBase/Neutral.h"
#include "TrkParametersBase/CurvilinearParametersT.h"
#include "TrkSurfaces/ConeSurface.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkSurfaces/DiscSurface.h"
#include "TrkSurfaces/PerigeeSurface.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkSurfaces/StraightLineSurface.h"

namespace Trk {

    typedef ParametersBase<5, Neutral>                       NeutralParameters;
    typedef CurvilinearParametersT<5, Neutral, PlaneSurface> NeutralCurvilinearParameters;
    typedef ParametersT<5, Neutral, ConeSurface>             NeutralAtaCone;
    typedef ParametersT<5, Neutral, CylinderSurface>         NeutralAtaCylinder;
    typedef ParametersT<5, Neutral, DiscSurface>             NeutralAtaDisc;
    typedef ParametersT<5, Neutral, PerigeeSurface>          NeutralPerigee;
    typedef ParametersT<5, Neutral, PlaneSurface>            NeutralAtaPlane;
    typedef ParametersT<5, Neutral, StraightLineSurface>     NeutralAtaStraightLine;
}

/**Overload of << operator for both, MsgStream and std::ostream for debug output*/ 
MsgStream& operator << ( MsgStream& sl, const Trk::NeutralParameters& pars);

/**Overload of << operator for both, MsgStream and std::ostream for debug output*/ 
std::ostream& operator << ( std::ostream& sl, const Trk::NeutralParameters& pars);

#endif // TRKNEUTRALPARAMETERS_NEUTRALPARAMETERS_H
