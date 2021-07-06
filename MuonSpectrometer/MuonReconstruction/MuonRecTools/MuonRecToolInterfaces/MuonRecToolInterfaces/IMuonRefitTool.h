/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_IMUONREFITTOOL_H
#define MUON_IMUONREFITTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "Identifier/Identifier.h"
#include "MuonStationIndex/MuonStationIndex.h"

namespace Trk {
    class Track;
}

namespace Muon {

    /** @brief The IMuonSegmentMaker is a pure virtual interface for tools to find tracks starting from MuonSegmentCombinations  */
    class IMuonRefitTool : virtual public IAlgTool {
    public:
        struct Settings {
            Settings() :
                broad(false),
                refit(true),
                removeOutliers(true),
                discardNotCleanedTracks(true),
                recalibrate(false),
                updateErrors(true),
                updateTriggerErrors(false),
                prepareForFit(true),
                recreateStartingParameters(true),
                removeOtherSectors(false),
                deweightOtherSectors(false),
                extrapolateToMuonEntry(false),
                removeTriggerEtaHits(false),
                removeBarrelEndcapOverlap(false),
                removeBEE(false) {}

            bool broad;                       /// use broad error settings
            bool refit;                       /// toogle on/off refit of the track
            bool removeOutliers;              /// remove MDT outliers locally redoing the local segment fit
            bool discardNotCleanedTracks;     /// toogle on/off the dropping of track that could not be cleaned
            bool recalibrate;                 /// recalibrate the hits
            bool updateErrors;                /// update the errors without recalibrating
            bool updateTriggerErrors;         /// update the errors of the trigger hits
            bool prepareForFit;               /// prepare the input track for a refit
            bool recreateStartingParameters;  /// recreate starting parameters by extrapolating from first hit on track
            bool removeOtherSectors;          /// remove all hits in sectors that are not the main sector
            bool deweightOtherSectors;        /// de-weight all hits in sectors that are not the main sector
            bool extrapolateToMuonEntry;      /// extrapolate muon to muon entrance after refit
            bool removeTriggerEtaHits;        /// all trigger eta hits are turned into outliers
            bool removeBarrelEndcapOverlap;   /// turn all hits in the barrel/endcap part of the track with least hits into outliers
            bool removeBEE;                   /// turn all hits in the BEE chambers into outliers
            std::set<Identifier> chambersToBeremoved;  /// all hits in chambers that are in the list are turned into outliers
            std::set<MuonStationIndex::StIndex>
                precisionLayersToBeremoved;  /// all precision hits in station layers that are in the list are turned into outliers
            std::set<MuonStationIndex::PhiIndex>
                phiLayersToBeremoved;  /// all phi hits in station layers that are in the list are turned into outliers
        };

    public:
        /** access to tool interface */
        static const InterfaceID& interfaceID() {
            static const InterfaceID IID_IMuonRefitTool("Muon::IMuonRefitTool", 1, 0);
            return IID_IMuonRefitTool;
        }

        virtual ~IMuonRefitTool() = default;
        /** refit a track */
        virtual std::unique_ptr<Trk::Track> refit(const Trk::Track& track, const EventContext& ctx,
                                                  const Settings* settings = nullptr) const = 0;
        /** refit and back extrapolate a vector of track pairs */
        virtual std::vector<std::unique_ptr<Trk::Track> > refit(const std::vector<Trk::Track*>& tracks, const EventContext& ctx,
                                                                const Settings* settings = nullptr) const = 0;
    };

}  // namespace Muon

#endif
