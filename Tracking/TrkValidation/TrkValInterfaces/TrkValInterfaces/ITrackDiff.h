/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// ITrackDiff.h
//   Header file for interface of TrackDiff
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Sebastian.Fleischmann@cern.ch
///////////////////////////////////////////////////////////////////

#ifndef TRK_ITRACKDIFF_H
#define TRK_ITRACKDIFF_H

#include "GaudiKernel/IAlgTool.h"
#include <vector>

namespace Trk {
static const InterfaceID IID_ITrackDiff("ITrackDiff",1,0);

class Track;

/** @class ITrackDiff
    provides the interface for a track diff tool
*/

class ITrackDiff : virtual public IAlgTool {
public:
     /**Interface ID, declared here, and defined below*/
    static const InterfaceID& interfaceID();

    /** diff of two given tracks */
    virtual StatusCode diff (
        const Trk::Track& referenceTrack,
        const Trk::Track& ) const = 0;
};

inline const InterfaceID& Trk::ITrackDiff::interfaceID() {
    return IID_ITrackDiff;
}

} // end of namespace

#endif // TRK_ITRACKDIFF_H
