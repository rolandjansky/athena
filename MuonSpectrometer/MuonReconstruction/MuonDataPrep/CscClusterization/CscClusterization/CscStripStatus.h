/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CscClusterStatus.h
//   Header file for class CscClusterStatus
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Class to implement ClusterStatus for Muon - CSC
///////////////////////////////////////////////////////////////////
// Woochun Park
///////////////////////////////////////////////////////////////////

#ifndef MUONPREPRAWDATA_CSCSTRIPSTATUS_H
#define MUONPREPRAWDATA_CSCSTRIPSTATUS_H

#include <string>

namespace Muon {
    /** Enum to represent the strip status - see the specific enum values for more details*/
    enum CscStripStatus {

        CscStrStatSuccess = 0,  // succeeded parabola fit

        CscStrStatParabolaFailed = 1,  // parabola fit failed aa>0

        CscStrStatHot = 2,  //  hot channel from DB.

        CscStrStatDead = 3,  // dead channel from DB.

        CscStrStatSaturated = 4,  // >4090 ADC

        CscStrStatStuckBit = 5,  // from DB /STAT? What kind of values in STAT?

        CscStrStatNoBipolaShape = 6,  // decided by chisquare with bipolar fit

        CscStrStatSomethingNew = 7,  // decided by chisquare with bipolar fit

        // Do we need more??

        /** Undefined, should not happen, most likely indicates a problem */
        CscStrStatUndefined = 255
    };

    /** Return a string description of a CSC cluster status flag. */
    inline std::string toString(CscStripStatus cstat) {
        switch (cstat) {
            case CscStrStatSuccess: return "unspoiled";
            case CscStrStatParabolaFailed: return "parabolaFailed";
            case CscStrStatHot: return "hotStrip";
            case CscStrStatDead: return "deadStrip";
            case CscStrStatSaturated: return "saturated";
            case CscStrStatStuckBit: return "stuckbit";
            case CscStrStatNoBipolaShape: return "nobipolarShape";
            case CscStrStatSomethingNew: return "somethingNew";
            case CscStrStatUndefined: return "undefined";
        }
        return "unknown";
    }

}  // namespace Muon

#endif
