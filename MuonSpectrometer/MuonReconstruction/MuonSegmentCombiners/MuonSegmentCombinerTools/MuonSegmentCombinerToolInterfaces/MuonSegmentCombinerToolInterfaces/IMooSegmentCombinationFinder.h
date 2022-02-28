/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IMOOSEGMENTCOMBINATIONFINDER_H
#define IMOOSEGMENTCOMBINATIONFINDER_H

#include "GaudiKernel/IAlgTool.h"
#include "MuonPattern/MuonPatternCombinationCollection.h"
#include "MuonPrepRawData/CscPrepDataCollection.h"
#include "MuonPrepRawData/MdtPrepDataCollection.h"
#include "MuonPrepRawData/RpcPrepDataCollection.h"
#include "MuonPrepRawData/TgcPrepDataCollection.h"
#include "MuonRecToolInterfaces/HoughDataPerSec.h"
#include "MuonSegment/MuonSegmentCombinationCollection.h"
#include "TrkSegment/SegmentCollection.h"

namespace Muon {

    class IMooSegmentCombinationFinder : virtual public IAlgTool {
    public:
        struct Output {
            std::unique_ptr<MuonPatternCombinationCollection> patternCombinations{nullptr};
            std::unique_ptr<Muon::HoughDataPerSectorVec> houghDataPerSectorVec{nullptr};
            Trk::SegmentCollection* segmentCollection{nullptr};
            
            Output() = default;
        };

        static const InterfaceID& interfaceID() {
            static const InterfaceID IID_IMooSegmentCombinationFinder("Muon::IMooSegmentCombinationFinder", 1, 0);
            return IID_IMooSegmentCombinationFinder;
        }

        // enter declaration of your interface-defining member functions here
        virtual void findSegments(const std::vector<const MdtPrepDataCollection*>& mdtCols,
                                  const std::vector<const CscPrepDataCollection*>& cscCols,
                                  const std::vector<const TgcPrepDataCollection*>& tgcCols,
                                  const std::vector<const RpcPrepDataCollection*>& rpcCols, Output& output,
                                  const EventContext& ctx) const = 0;

        virtual ~IMooSegmentCombinationFinder() = default;
    };

}  // namespace Muon

#endif
