/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@mainpage The Track class 

@author Edward.Moyse@cern.ch

@section introductionTrkTrack Introduction
This package contains the common Track class for ATLAS.
This Track class is also not intended for use in physics analysis (see xAOD::TrackParticle),
and is instead optimised for reconstruction. 

@section designTrkTrack Design
The two main aims are as follows:
- provide all necessary functionality (for reconstruction)
- provide a class that is efficient and fast
- provide a class that works equally well providing as an extremely detailed track object, and as an extremely simple track object:
nothing should be compulsory beyond the absolute basics (i.e. a lot of the possible data which could be found in a track may be missing).

Other points:
- Trk::Track contains a vector of Trk::TrackStateOnSurface which in turn contain Trk::MeasurentBase (i.e. measurements or hits), and Trk::TrackParameters. In other words, the hits and parameters are grouped together by the Trk::Surfaces they exist on.
- Since the Trk::Perigee is a special case and will be used frequently there is a dedicated method ( Trk::Track::perigeeParameters )
to return it.
- There are also methods to return the Trk::FitQuality, and helper methods to return just the 'real' measurements from all the TrackStateOnSurfaces, as well as a method to return the outliers alone. 

Trk::FitQuality will provide only the most basic functionality - the idea is that they should be extended (by inheritance)
to more complete versions as needed (for example, different versions suitable for iPatRec and xKalman).
An example has been provided, Trk::FitQualityExtended, which shows the general idea.

This approach is also continued with the concept of a Trk::FitQualityOnSurface (contained within Trk::TrackStateOnSurface)... the basic example is there, and could be extended to other ways of measuring the quality of a fit.

Click on <a href="annotated.html">Class List</a> to see all the classes available in this package.<br>
Outstanding issues are shown in the <a href="todo.html">ToDo</a> list.

@section guidelinesTrkTrack Guidelines for use
One important point (mentioned above in the design section) is that, in order to provide a flexible class, it is not safe to assume that just because the Track *can* contain e.g. ScatteringAngles, that it does so. In general it won't - a good rule of thumb is that you must test all objects returned by pointer ... they may well not exist (i.e the pointer may = 0)

The TrackStateOnSurface *should* be ordered geometrically (i.e. the first TrackStateOnSurface is the one closest to the IP, and the last TrackStateOnSurface is the furthest from it etc) but this is not checked in the constructors : creators of tracks must be careful! For further details of requirements coming from TrackStateOnSurface please see the documentation for that class.

For examples of how to form and use tracks see e.g. TrackParticleCreator and InDetLegacyCnvTools. Additionially there is a wiki page under development (feel free to add to it!)
<a href="https://uimon.cern.ch/twiki/bin/view/Atlas/UsingTrkTracks">here</a>

@section literatureTrkTrack Full Written Documentation
Documents describing various aspects on Trk::Track and its related Tracking
EDM objects are available in CDS:
<br><b>ATL-SOFT-PUB-2006-004</b> CERN, 20 Jul 2006 - 31 p
<br><b>ATL-SOFT-PUB-2007-003</b> CERN, 14 Jun 2007 - 13 p

*/
