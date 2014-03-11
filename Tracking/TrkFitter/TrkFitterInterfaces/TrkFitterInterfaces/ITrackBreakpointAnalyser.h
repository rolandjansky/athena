/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ITrackBreakpointAnalyser.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef TRKFITTERINTERFACES_ITRACKBREAKPOINTANALYSER_H
#define TRKFITTERINTERFACES_ITRACKBREAKPOINTANALYSER_H

#include "GaudiKernel/IAlgTool.h"
#include "TrkFitterUtils/ProtoTrackStateOnSurface.h"
#include "TrkFitterUtils/TrackBreakpointType.h"

namespace Trk {
  typedef std::vector<Trk::ProtoTrackStateOnSurface> Trajectory;
  class Track;

  static const InterfaceID IID_ITrackBreakpointAnalyser("Trk::ITrackBreakpointAnalyser", 1, 0);

  class ITrackBreakpointAnalyser : virtual public IAlgTool {
  public:
    static const InterfaceID& interfaceID( ) ;

    /** @brief evaluate track for breakpoints inside the KalmanFitter,
        using full access to its internal trajectory.

        The currently existing implementation of this interface 
        will separate DNA action associated to a change in momentum
        from DNA action without that the noise term was picked up
        by the KalmanFitter to allow a momentum reduction.

        @author Wolfgang.Liebig -at- cern.ch
      */

    virtual TrackBreakpointType confirmBreakpoint(const Trajectory&) const = 0;

    /** @brief again evaluate KF-internal trajectory for breakpoint, this time
        returning the detailed significance value */

    virtual double breakpointSignificance(const Trajectory&) const = 0;

    /** @brief evaluate EDM-track for breakpoints, which will require
        MaterialEffectsOnTrack objects present. */

    virtual TrackBreakpointType confirmBreakpoint(const Track&) const = 0;

    /** same analysis of full EDM-tracks, this time returning the
        detailed significance value */
    virtual double breakpointSignificance(const Track&) const = 0;

  };

  inline const InterfaceID& Trk::ITrackBreakpointAnalyser::interfaceID()
    { 
      return IID_ITrackBreakpointAnalyser; 
    }

} // end of namespace

#endif 
