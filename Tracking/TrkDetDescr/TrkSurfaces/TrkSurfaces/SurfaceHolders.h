/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKSURFACES_SURFACEHOLDERS_H
#define TRKSURFACES_SURFACEHOLDERS_H

#include "TrkEventPrimitives/SurfaceHolderImpl.h"
#include "TrkSurfaces/ConeSurface.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkSurfaces/DiscSurface.h"
#include "TrkSurfaces/PerigeeSurface.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkSurfaces/StraightLineSurface.h"
#include "TrkSurfaces/Surface.h"

namespace Trk {

using ConeSurfacePtrHolder = SurfacePtrHolderImpl<ConeSurface>;
using CylinderSurfacePtrHolder = SurfacePtrHolderImpl<CylinderSurface>;
using DiscSurfacePtrHolder = SurfacePtrHolderImpl<DiscSurface>;
using PerigeeSurfacePtrHolder = SurfacePtrHolderImpl<PerigeeSurface>;
using PlaneSurfacePtrHolder = SurfacePtrHolderImpl<PlaneSurface>;
using StraightLineSurfacePtrHolder = SurfacePtrHolderImpl<StraightLineSurface>;
using SurfacePtrHolder = SurfacePtrHolderImpl<Surface>;

using ConeSurfaceUniqHolder = SurfaceUniqHolderImpl<ConeSurface>;
using CylinderSurfaceUniqHolder = SurfaceUniqHolderImpl<CylinderSurface>;
using DiscSurfaceUniqHolder = SurfaceUniqHolderImpl<DiscSurface>;
using PerigeeSurfaceUniqHolder = SurfaceUniqHolderImpl<PerigeeSurface>;
using PlaneSurfaceUniqHolder = SurfaceUniqHolderImpl<PlaneSurface>;
using StraightLineSurfaceUniqHolder =
  SurfaceUniqHolderImpl<StraightLineSurface>;
using SurfaceUniqHolder = SurfaceUniqHolderImpl<Surface>;

using SurfacePtrHolderDetEl = Trk::SurfacePtrHolderImplDetEl<Surface>;
}

#endif
