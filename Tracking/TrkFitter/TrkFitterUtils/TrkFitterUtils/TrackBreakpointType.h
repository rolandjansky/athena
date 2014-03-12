/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackBreakpointType.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKFITTERUTILS_TRACKBREAKPOINTTYPE_H
#define TRKFITTERUTILS_TRACKBREAKPOINTTYPE_H

namespace Trk {

 /** @enum TrackBreakpointType
    
     This file defines the enums in the Trk namespace for the different
     results when analysing a track for break points, points at which
     one or several of the track parmeters change abruptly.
     There are three types of breakpoints known in the literature
     - type 1, electron bremsstrahlung
     - type 2, strong elastic scatterer
     - type 3, particle decay
   
    @author Wolfgang.Liebig -at- cern.ch
   */
  enum TrackBreakpointType {
    //! setting for uninitialised or error condition
    BreakpointNotSpecified = 0,
    //! no breakpoint present or found on track
    NoBreakpoint = 1,
    //! brem-like breakpoint present and confirmed
    DnaBremPointUseful = 2,
    //! brem-like breakpoint present but not confirmed
    DnaBremPointNotUseful = 3
  };
} // end of namespace
#endif
