/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ICscAlignmentTool.h

#ifndef ICscAlignmentTool_H
#define ICscAlignmentTool_H

// Woochun Park
// April 2009
//
// Interface for CscSegmentMakers to use tools in CscClusterization.
// It needs an input as CscClusterOnTrack, RIO_OnTrack, on top of CscPrepData

#include <vector>

#include "CscClusterization/ICscClusterFitter.h"
#include "GaudiKernel/IAlgTool.h"

class Identifier;

namespace Muon {
    class CscClusterOnTrack;
    class CscPrepData;
    class CscStripPrepData;
}  // namespace Muon
namespace Trk {
    class RIO_OnTrack;
}

/** Must declare this, with name of interface*/
static const InterfaceID IID_ICscAlignmentTool("ICscAlignmentTool", 1, 0);

class ICscAlignmentTool : virtual public IAlgTool {
public:  // Static methods.
    // Return the interface ID.
    static const InterfaceID& interfaceID() { return IID_ICscAlignmentTool; }

public:  // Interface methods
    virtual double getAlignmentOffset(Identifier pstripId) const = 0;
};

#endif
