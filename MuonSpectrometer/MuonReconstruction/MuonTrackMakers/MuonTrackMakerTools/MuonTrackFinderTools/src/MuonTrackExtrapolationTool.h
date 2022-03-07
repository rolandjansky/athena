/*
 Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */

#ifndef MUON_MUONTRACKEXTRAPOLATIONTOOL_H
#define MUON_MUONTRACKEXTRAPOLATIONTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonRecHelperTools/IMuonEDMHelperSvc.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "MuonRecToolInterfaces/IMuonTrackExtrapolationTool.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkGeometry/TrackingGeometry.h"

// For magneticfield
#include "MagFieldConditions/AtlasFieldCacheCondObj.h"
#include "TrkParameters/TrackParameters.h"

namespace Muon {

    /**
     @brief tool to select tracks

     */
    class MuonTrackExtrapolationTool : virtual public IMuonTrackExtrapolationTool, public AthAlgTool {
    public:
        /** @brief constructor */
        MuonTrackExtrapolationTool(const std::string &, const std::string &, const IInterface *);

        /** @brief destructor */
        virtual ~MuonTrackExtrapolationTool() = default;

        /** @brief AlgTool initilize */
        virtual StatusCode initialize() override;

        /** @brief extrapolates a muon track to the muon entry record and returns a new track expressed at the destination.
         @param track input track
         @return a pointer to the extrapolated track, zero if extrapolation failed.
         The ownership of the track is passed to the client calling the tool.
         */
        virtual std::unique_ptr<Trk::Track> extrapolate(const Trk::Track &track, const EventContext &ctx) const override;

        /** @brief extrapolates a muon track collection to the muon entry record and returns a new track expressed at the destination.
         @param tracks input track collection
         @return a pointer to the extrapolated track collection, zero if extrapolation failed.
         The ownership of the collection is passed to the client calling the tool.
         */
        virtual std::unique_ptr<TrackCollection> extrapolate(const TrackCollection &tracks, const EventContext &ctx) const override;

    private:
        double estimateDistanceToEntryRecord(const EventContext &ctx, const Trk::TrackParameters &pars) const;
        const Trk::TrackParameters *checkForSecondCrossing(const Trk::TrackParameters &firstCrossing, const Trk::Track &track) const;
        const Trk::TrackParameters *findClosestParametersToMuonEntry(const EventContext &ctx, const Trk::Track &track) const;

        /** extrapolates track parameters to muon entry record, will return a zero pointer if the extrapolation fails. The caller gets
         * ownership of the new parameters */
        const Trk::TrackParameters *extrapolateToMuonEntryRecord(const EventContext &ctx, const Trk::TrackParameters &pars,
                                                                 Trk::ParticleHypothesis particleHypo = Trk::muon) const;

        /** extrapolates track parameters to muon entry record, will return a zero pointer if the extrapolation fails. The caller gets
         * ownership of the new parameters */
        virtual const Trk::TrackParameters *extrapolateToIP(const Trk::TrackParameters &pars,
                                                            Trk::ParticleHypothesis particleHypo = Trk::muon) const;

        const Trk::Perigee *createPerigee(const Trk::TrackParameters &pars) const;

        // Read handle for conditions object to get the field cache
        SG::ReadCondHandleKey<AtlasFieldCacheCondObj> m_fieldCacheCondObjInputKey{this, "AtlasFieldCacheCondObj", "fieldCondObj",
                                                                                  "Name of the Magnetic Field conditions object key"};

        SG::ReadCondHandleKey<Trk::TrackingGeometry> m_trackingGeometryReadKey{this, "TrackingGeometryReadKey", "",
                                                                               "Key of input TrackingGeometry"};
        ServiceHandle<Muon::IMuonEDMHelperSvc> m_edmHelperSvc{this, "edmHelper", "Muon::MuonEDMHelperSvc/MuonEDMHelperSvc",
                                                              "Handle to the service providing the IMuonEDMHelperSvc interface"};
        ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc{this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

        ToolHandle<Muon::MuonEDMPrinterTool> m_printer{this, "EDMPrinter", "Muon::MuonEDMPrinterTool/MuonEDMPrinterTool",
                                                       "helper to nicely print out tracks"};
        ToolHandle<Trk::IExtrapolator> m_atlasExtrapolator{this, "AtlasExtrapolator", "Trk::Extrapolator/AtlasExtrapolator"};
        ToolHandle<Trk::IExtrapolator> m_muonExtrapolator{this, "MuonExtrapolator", "Trk::Extrapolator/MuonExtrapolator"};
        ToolHandle<Trk::IExtrapolator> m_muonExtrapolator2{this, "MuonExtrapolator2", "Trk::Extrapolator/MuonExtrapolator"};

        Gaudi::Property<bool> m_cosmics{this, "Cosmics", false};
        Gaudi::Property<bool> m_keepOldPerigee{this, "KeepInitialPerigee", true};
        Gaudi::Property<std::string> m_msEntranceName{this, "MuonSystemEntranceName", "MuonSpectrometerEntrance"};

        inline const Trk::TrackingVolume *getVolume(const std::string &vol_name, const EventContext &ctx) const {
            SG::ReadCondHandle<Trk::TrackingGeometry> handle(m_trackingGeometryReadKey, ctx);
            if (!handle.isValid()) {
                ATH_MSG_WARNING("Could not retrieve a valid tracking geometry");
                return nullptr;
            }
            return handle.cptr()->trackingVolume(vol_name);
        }
    };
}  // namespace Muon

#endif
