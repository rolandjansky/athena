/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ICscClusterUtilTool.h

#ifndef ICscClusterUtilTool_H
#define ICscClusterUtilTool_H

// Woochun Park
// April 2009
//
// Interface for CscSegmentMakers to use tools in CscClusterization.
// It needs an input as CscClusterOnTrack, RIO_OnTrack, on top of CscPrepData

#include <vector>

#include "CscClusterization/ICscClusterFitter.h"
#include "GaudiKernel/IAlgTool.h"

namespace Muon {
    class CscClusterOnTrack;
    class CscPrepData;
    class CscStripPrepData;
}  // namespace Muon
namespace Trk {
    class RIO_OnTrack;
}

/** Must declare this, with name of interface*/
static const InterfaceID IID_ICscClusterUtilTool("ICscClusterUtilTool", 1, 0);

class ICscClusterUtilTool : virtual public IAlgTool {
public:  // Static methods.
    // Return the interface ID.
    static const InterfaceID& interfaceID() { return IID_ICscClusterUtilTool; }

public:  // Interface methods
    virtual void getStripFits(const Trk::RIO_OnTrack* rot, ICscClusterFitter::StripFitList& sfits) const = 0;
    virtual void getStripFits(const Muon::CscClusterOnTrack* pclu, ICscClusterFitter::StripFitList& sfits) const = 0;
    virtual void getStripFits(const Muon::CscPrepData*, ICscClusterFitter::StripFitList&) const = 0;
    virtual ICscClusterFitter::Results getRefitCluster(const Muon::CscPrepData* MClus, double tantheta) const = 0;
    virtual std::vector<const Muon::CscStripPrepData*> getStrips(const Muon::CscPrepData* MClus) const = 0;
};

#endif
