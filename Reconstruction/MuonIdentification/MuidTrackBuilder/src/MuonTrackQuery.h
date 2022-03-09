/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUIDTRACKBUILDER_MUONTRACKQUERY_H
#define MUIDTRACKBUILDER_MUONTRACKQUERY_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuidInterfaces/IMuonTrackQuery.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonRecHelperTools/IMuonEDMHelperSvc.h"
#include "MuonRecToolInterfaces/IMdtDriftCircleOnTrackCreator.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "TrkFitterInterfaces/ITrackFitter.h"
#include "TrkGeometry/TrackingGeometry.h"
namespace Rec {

    class MuonTrackQuery : public AthAlgTool, virtual public IMuonTrackQuery {
    public:
        MuonTrackQuery(const std::string& type, const std::string& name, const IInterface* parent);
        ~MuonTrackQuery() = default;

        StatusCode initialize() override;

        /** IMuonTrackQuery interface:
            caloEnergy from appropriate TSOS */
        const CaloEnergy* caloEnergy(const Trk::Track& track) const override;

        /** IMuonTrackQuery interface:
            track energy deposit in calorimeters (as fitted or otherwise applied) */
        double caloEnergyDeposit(const Trk::Track& track, const EventContext& ctx) const override;

        /** IMuonTrackQuery interface:
            field integral along track from momentum kick between measurements */
        FieldIntegral fieldIntegral(const Trk::Track& track, const EventContext& ctx) const override;

        /** IMuonTrackQuery interface:
            does track have at least 3 TSOS's representing calorimeter ? */
        bool isCaloAssociated(const Trk::Track& track, const EventContext& ctx) const override;

        /** IMuonTrackQuery interface:
            does track have measurements from indet and spectrometer ? */
        bool isCombined(const Trk::Track& track, const EventContext& ctx) const override;

        /** IMuonTrackQuery interface:
            does track have perigee inside indet ? */
        bool isExtrapolated(const Trk::Track& track, const EventContext& ctx) const override;

        /** IMuonTrackQuery interface:
            does track have fitted curvature ? */
        bool isLineFit(const Trk::Track& track) const override;

        /** IMuonTrackQuery interface:
            is track (roughly) projective towards IP? */
        bool isProjective(const Trk::Track& track) const override;

        /** IMuonTrackQuery interface:
            is there a long/short chamber overlap? */
        bool isSectorOverlap(const Trk::Track& track) const override;

        /** IMuonTrackQuery interface:
            does track have TrackParameters at every TSOS ? */
        bool isSlimmed(const Trk::Track& track) const override;

        /** IMuonTrackQuery interface:
            significance of momentum balance for combined tracks (biassed residual) */
        double momentumBalanceSignificance(const Trk::Track& track, const EventContext& ctx) const override;

        /** IMuonTrackQuery interface:
            number of PseudoMeasurements on track (counts one for any vertex measurement) */
        unsigned numberPseudoMeasurements(const Trk::Track& track) const override;

        /** IMuonTrackQuery interface:
            perigee expressed outgoing from IP */
        std::unique_ptr<const Trk::Perigee> outgoingPerigee(const Trk::Track& track) const override;

        /** IMuonTrackQuery interface:
            significance of early scattering angle pattern for combined tracks (wider than gaussian) */
        ScatteringAngleSignificance scatteringAngleSignificance(const Trk::Track& track, const EventContext& ctx) const override;

        /** IMuonTrackQuery interface:
            trackParameters at innermost measurement TSOS in MS */
        std::unique_ptr<Trk::TrackParameters> spectrometerParameters(const Trk::Track& track, const EventContext& ctx) const override;

        /** IMuonTrackQuery interface:
            assess the number of additional phi measurements needed for MS (or SA) track fit */
        unsigned spectrometerPhiQuality(const Trk::Track& track, const EventContext& ctx) const override;

        /** IMuonTrackQuery interface:
            trackParameters at innermost trigger chamber TSOS in MS */
        std::unique_ptr<const Trk::TrackParameters> triggerStationParameters(const Trk::Track& track,
                                                                             const EventContext& ctx) const override;

    private:
        static std::unique_ptr<Trk::TrackParameters> flippedParameters(const Trk::TrackParameters& params);

        // tools and services
        ToolHandle<Trk::ITrackFitter> m_fitter{this, "Fitter", "", "Track fitter tool"};

        ServiceHandle<Muon::IMuonEDMHelperSvc> m_edmHelperSvc{
            this,
            "edmHelper",
            "Muon::MuonEDMHelperSvc/MuonEDMHelperSvc",
            "Handle to the service providing the IMuonEDMHelperSvc interface",
        };

        ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc{this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

        ToolHandle<Muon::IMdtDriftCircleOnTrackCreator> m_mdtRotCreator{
            this, "MdtRotCreator", "Muon::MdtDriftCircleOnTrackCreator/MdtDriftCircleOnTrackCreator", "MdtDriftCircleOnTrackCreator tool"};

        SG::ReadCondHandleKey<Trk::TrackingGeometry> m_trackingGeometryReadKey{this, "TrackingGeometryReadKey", "",
                                                                               "Key of the TrackingGeometry conditions data."};

        inline const Trk::TrackingVolume* getVolume(const std::string&& vol_name, const EventContext& ctx) const {
            /// Tracking geometry is provided by the TrackingGeometryAlg
            SG::ReadCondHandle<Trk::TrackingGeometry> handle(m_trackingGeometryReadKey, ctx);
            if (!handle.isValid()) {
                ATH_MSG_WARNING("Could not retrieve a valid tracking geometry");
                return nullptr;
            }
            return handle->trackingVolume(vol_name);
            
        }

    };  // end of class MuonTrackQuery

}  // end of namespace Rec

#endif  // MUIDTRACKBUILDER_MUONTRACKQUERY_H
