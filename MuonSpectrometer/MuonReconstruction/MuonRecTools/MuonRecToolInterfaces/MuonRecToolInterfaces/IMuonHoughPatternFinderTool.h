/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_IMUONHOUGHPATTERNFINDERTOOL_H
#define MUON_IMUONHOUGHPATTERNFINDERTOOL_H

#include <vector>

#include "GaudiKernel/IAlgTool.h"
#include "MuonPattern/MuonPatternCombinationCollection.h"
#include "MuonPrepRawData/CscPrepDataCollection.h"
#include "MuonPrepRawData/MdtPrepDataCollection.h"
#include "MuonPrepRawData/RpcPrepDataCollection.h"
#include "MuonPrepRawData/TgcPrepDataCollection.h"
#include "MuonRecToolInterfaces/HoughDataPerSec.h"
#include "MuonSegment/MuonSegmentCombinationCollection.h"

static const InterfaceID IID_IMuonHoughPatternFinderTool("Muon::IMuonHoughPatternFinderTool", 1, 0);

namespace Trk {
    class Track;
}

namespace Muon {

    /** @brief The IMuonSegmentMaker is a pure virtual interface for tools to find tracks starting from MuonSegmentCombinations  */
    class IMuonHoughPatternFinderTool : virtual public IAlgTool {
    public:
        /** access to tool interface */
        static const InterfaceID& interfaceID();

        /** find patterns for a give set of MuonPrepData collections + optionally CSC segment combinations */
        virtual std::pair<std::unique_ptr<MuonPatternCombinationCollection>, std::unique_ptr<Muon::HoughDataPerSectorVec>> find(
            const std::vector<const MdtPrepDataCollection*>& mdtCols, const std::vector<const CscPrepDataCollection*>& cscCols,
            const std::vector<const TgcPrepDataCollection*>& tgcCols, const std::vector<const RpcPrepDataCollection*>& rpcCols,
            const MuonSegmentCombinationCollection* cscSegmentCombis, const EventContext& ctx) const = 0;
    };

    inline const InterfaceID& IMuonHoughPatternFinderTool::interfaceID() { return IID_IMuonHoughPatternFinderTool; }

}  // namespace Muon

#endif
