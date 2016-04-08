/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackParameters.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKPARAMETERS_TRACKPARAMETERS_H
#define TRKPARAMETERS_TRACKPARAMETERS_H


#include "TrkParametersBase/ParametersT.h"
#include "TrkParametersBase/CurvilinearParametersT.h"
#include "TrkParametersBase/Charged.h"
#include "TrkSurfaces/ConeSurface.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkSurfaces/DiscSurface.h"
#include "TrkSurfaces/PerigeeSurface.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkSurfaces/StraightLineSurface.h"

namespace Trk {

    typedef ParametersBase<5, Charged>                       TrackParameters;
    typedef CurvilinearParametersT<5, Charged, PlaneSurface> CurvilinearParameters;
    typedef ParametersT<5, Charged, ConeSurface>             AtaCone;
    typedef ParametersT<5, Charged, CylinderSurface>         AtaCylinder;
    typedef ParametersT<5, Charged, DiscSurface>             AtaDisc;
    typedef ParametersT<5, Charged, PerigeeSurface>          Perigee;
    typedef ParametersT<5, Charged, PlaneSurface>            AtaPlane;
    typedef ParametersT<5, Charged, StraightLineSurface>     AtaStraightLine;
    
}

/**Overload of << operator for both, MsgStream and std::ostream for debug output*/ 
MsgStream& operator << ( MsgStream& sl, const Trk::TrackParameters& pars);

/**Overload of << operator for both, MsgStream and std::ostream for debug output*/ 
std::ostream& operator << ( std::ostream& sl, const Trk::TrackParameters& pars);

#endif
