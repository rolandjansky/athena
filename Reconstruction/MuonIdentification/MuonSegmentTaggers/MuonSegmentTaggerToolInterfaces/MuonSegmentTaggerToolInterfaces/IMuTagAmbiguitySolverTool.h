/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IMuTagAmbiguitySolverTool_H
#define IMuTagAmbiguitySolverTool_H

#include "GaudiKernel/IAlgTool.h"
#include "MuonCombinedEvent/MuonSegmentInfo.h"
#include "MuonSegmentTaggerToolInterfaces/IMuTagMatchingTool.h"

/**
   @class IMuTagAmbiguitySolverTool

   @author Zdenko.van.Kesteren@cern.ch

*/
namespace Rec {
    class TrackParticleContainer;
}
class MuTagObject;
class MuTaggedSegment;
namespace Muon {
    class IMuonSegmentMatchingTool;
}
static const InterfaceID IID_IMuTagAmbiguitySolverTool("IMuTagAmbiguitySolverTool", 1, 0);

class IMuTagAmbiguitySolverTool : virtual public IAlgTool {
public:
    static const InterfaceID& interfaceID();

    virtual std::vector<MuonCombined::MuonSegmentInfo> solveAmbiguities(const EventContext& ctx, std::vector<MuonCombined::MuonSegmentInfo> mtos) const = 0;

    virtual std::vector<MuonCombined::MuonSegmentInfo> selectBestMuTaggedSegments(
        const EventContext& ctx, std::vector<MuonCombined::MuonSegmentInfo> mtss) const = 0;
    virtual double Rseg(unsigned int nseg) const = 0;
};

inline const InterfaceID& IMuTagAmbiguitySolverTool::interfaceID() { return IID_IMuTagAmbiguitySolverTool; }

#endif  // IMuTagAmbiguitySolverTool_H
