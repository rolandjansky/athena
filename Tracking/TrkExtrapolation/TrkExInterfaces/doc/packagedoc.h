/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page TrkExInterfaces_page TrkExInterfaces Package

This package contains all interface classes
to be used in the TrkExtrapolation container package.
Concrete implementations can be found in the various propagator packages or in the common
TrkExTools package.

@author Andreas.Salzburger@cern.ch

@section TrkExInterfaces_TrkExInterfacesIntro List of interfaces


The following interface classes are defined

\htmlonly
<!-- disable linking: doxygen puts the classes' @brief statement as descriptions to the links and causes mess if that statement has a wikiword inside -->
<ul>
 <li> <b>Trk::IExtrapolator</b>              : interface for the standard Trk::Extrapolator</li>
 <li> <b>Trk::IPropagator</b>                : interface for mathematical propagators</li>
 <li> <b>Trk::IPatternParametersPropagator</b> : interface for propagation of track states given as Trk::PatternTrackParameters</li>
 <li> <b>Trk::IIntersector</b>               : interface for Intersector AlgTools as they are used within iPatRec</li>
 <li> <b>Trk::INavigator</b>                 : interface for the navigator used in the extrapolation process</li>
 <li> <b>Trk::IMaterialEffectsUpdator</b>    : interface for the standard material effects updator</li>
 <li> <b>Trk::IEnergyLossUpdator</b>         : interface for material effects updator (only eloss)</li>
 <li> <b>Trk::IEnergyLossCalculator</b>      : interface for calculating an energy loss correction from a variety of effects</li>
 <li> <b>Trk::IMultipleScatteringUpdator</b> : interface for material effects updator (only multiple scattering)</li>
</ul>
This package is not built as a library, it is a simple include package.   
\endhtmlonly
      
@section TrkExInterfaces_ExtrasTrkExInterfaces Extra Pages

 - @ref reqsTrkExInterfaces
*/
