/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page TrkFitterUtils_page 

@section TrkFitterUtils_TrkFitterUtilsDocIntro Overview

The package TrkFitterUtils provides two types of classes: definitions
which are in use by the interfaces for track fits and internal data
or helper classes for one or several track fitter implementations.

Definitions are provided by FitterTypes.h, namely the typedefs
for objects commonly used in track fitting, like MeasurementSet,
PrepRawDataSet and RIO_OnTrackSet (= vectors of this kind of object)
and the switch to control outlier removal. These definitions are
used in the ITrackFitter interface as well as in some of the helper
classes here - therefore the definitions are provided here and not
inside TrkFitterInterfaces. To allow a more refined understanding
of problematic fits than a simple good-fail, a class FitterStatusCode.h
is provided to extend the Gaudi StatusCode, describing different
error conditions (of mainly iterative track fit algorithms).

The classes DNA_MaterialEffects.h and ProtoTrackStateOnSurface.h are
designed for internal use by track fitters when the trajectory is still
under formation (i.e. track parameters added, outliers flagged).
When the fitter gets ready, its objects can be quickly transferred
onto a Trk::Track. Code to access high-level information about a list
 of Proto-TSoS, i.e. a trajectory candidate, is contained in
ProtoTrajectoryUtility.h, allowing the fitter implementations to
be relatively free of track-state-management code which is not
directly related to the individual fitting algorithms.

Some of the fitter interface methods, for example the fit of a track
extension, exist for convenience only and do not always necessitate
a stand-alone algorithmic implementation in addition to what is
already used for the core method (fit of a list of measurements).
Instead, the track or track extension input should be dissolved
back into a list of measurements and the core interface called.
For that purpose the TrackFitInputPreparator.h is provided.

This package also holds one more comparison functor for sorting,
MeasBaseIndexComparisonFunction.h, which uses an index to keeps
a link to the original position of a measurement on a trajectory
before sorting. This is needed for using measurements as sorting
criterium but maintaining access to e.g. the TrackState holding
the current measurement.

Full details can be found under the "Class List" section.
The ITrackFitter interface, which first had been part of this
package, has been moved to a separate package: TrkFitterInterfaces.

@author Wolfgang Liebig <http://consult.cern.ch/xwho/people/54608>

@section TrkFitterUtils_ExtrasTrkFitterUtils Extra Pages

*/
