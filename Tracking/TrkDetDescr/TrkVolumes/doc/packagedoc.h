/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page TrkVolumes_page TrkVolumes Package

The package for a generic Volume description in the Tracking realm,
Trk::TrackingVolumes and Trk::MagneticFieldVolumes may extend the Trk::Volume class.

@author Andreas.Salzburger@cern.ch

@section TrkVolumes_IntroductionTrkVolumes Introduction

To enable sub detector independend tracking algorithms a generic Trk::Volume description
for Tracking algorithms has been created.
The Volume class is designed to be mainly used in terms of the TrkExtrapolation package, 
therefor the Trk::BoundarySurfaces that confine a Volume are extending to Surface class so
to be used directly by the Extrapolator, resp. Propagators.

@section TrkVolumes_OverviewTrkVolumes Class Overview
  The TrkVolumes package contains of following classes:
 
  - Trk::Volume: oriented using a <nop>HepTransform3D, the shape is given by the passed through VolumeBounds.
  - Trk::VolumeBounds: Abstract Base class for boundary description of a Trk::Volume, the interface requires
       inherited classes to implement a decomposeToSurfaces() method.
    - Trk::VolumeCuboidBounds: extends Trk::VolumeBounds, describes volume boundaries of a cubical volume   
    - Trk::VolumeCylinderBounds: extends Trk::VolumeBounds, describes volume boundaries of a cylindrical volume
    - Trk::VolumeTrapezoidBounds: extends Trk::VolumeBounds, describes volume boundaries of a ctrapezoidal volume
    - Trk::BoundarySurface: Abstract Base class for surfaces that are boundaries of a Volume.
    - Trk::BoundaryCylinderSurface: extends Trk::CylinderSurface and Trk::BoundarySurface (used for cylindrical volume)
    - Trk::BoundaryDiscSurface: extends Trk::CylinderSurface and Trk::BoundarySurface (used for cylindrical volume)
    - Trk::BoundaryPlaneSurface: extends Trk::CylinderSurface and Trk::BoundarySurface (used for all volume types)
    
@section TrkVolumes_ExtrasTrkVolumes Extra Pages

*/
