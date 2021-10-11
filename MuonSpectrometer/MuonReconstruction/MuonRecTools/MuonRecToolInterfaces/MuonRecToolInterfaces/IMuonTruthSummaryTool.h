/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_IMUONTRUTHSUMMARYTOOL_H
#define MUON_IMUONTRUTHSUMMARYTOOL_H

#include <string>

#include "GaudiKernel/IAlgTool.h"
#include "Identifier/Identifier.h"
#include "MuonSegment/MuonSegment.h"
#include "TrkTrack/Track.h"

namespace Muon {

    /** Interface for tools calculating hit count summaries for track */
    class IMuonTruthSummaryTool : virtual public IAlgTool {
    public:
    public:
        /** IAlgTool interface */
        static const InterfaceID& interfaceID() {
            static const InterfaceID IID_IMuonTruthSummaryTool("Muon::IMuonTruthSummaryTool", 1, 0);
            return IID_IMuonTruthSummaryTool;
        }

        /** clear tool */
        virtual void clear() = 0;

        /** init truth */
        virtual void init() const = 0;

        /** get the associated barcode for the identifier, return -1 if the channel was not hit by a muon */
        virtual int getBarcode(const Identifier& id) const = 0;

        /** get the associated pdgId for a given barcode */
        virtual int getPdgId(int barcode) const = 0;

        /** add identifier */
        virtual void add(const Identifier& id, int level) const = 0;

        /** add segment */
        virtual void add(const MuonSegment& seg, int level) const = 0;

        /** add track */
        virtual void add(const Trk::Track& track, int level) const = 0;

        /** print summary */
        virtual std::string printSummary() = 0;

        virtual ~IMuonTruthSummaryTool() = default;
    };

}  // namespace Muon

#endif
