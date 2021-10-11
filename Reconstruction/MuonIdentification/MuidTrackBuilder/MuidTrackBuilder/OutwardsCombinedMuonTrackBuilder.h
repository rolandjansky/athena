/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
// OutwardsCombinedMuonTrackBuilder
//  AlgTool gathering  material effects along a combined muon track, in
//  particular the TSOS'es representing the calorimeter energy deposit and
//  Coulomb scattering.
//  The resulting track is fitted at the IP using the ITrackFitter interface.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef MUIDTRACKBUILDER_OUTWARDSCOMBINEDMUONTRACKBUILDER_H
#define MUIDTRACKBUILDER_OUTWARDSCOMBINEDMUONTRACKBUILDER_H



#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuidInterfaces/ICombinedMuonTrackBuilder.h"
#include "MuonRecToolInterfaces/IMuonErrorOptimisationTool.h"
#include "MuonRecToolInterfaces/IMuonHoleRecoveryTool.h"
#include "MuonRecToolInterfaces/IMuonTrackCleaner.h"
#include "TrkDetDescrInterfaces/ITrackingVolumesSvc.h"
#include "TrkFitterInterfaces/ITrackFitter.h"
#include "TrkGeometry/TrackingVolume.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkToolInterfaces/ITrackSummaryTool.h"
#include "TrkTrack/TrackInfo.h"
#include <memory>

namespace Trk {
    class RecVertex;
    class PseudoMeasurementOnTrack;
    class VertexOnTrack;
}  // namespace Trk

namespace Rec {

    class OutwardsCombinedMuonTrackBuilder : public AthAlgTool, virtual public ICombinedMuonTrackBuilder {
    public:
        OutwardsCombinedMuonTrackBuilder(const std::string& type, const std::string& name, const IInterface* parent);
        virtual ~OutwardsCombinedMuonTrackBuilder() = default;

        virtual StatusCode initialize() override;

        /** ICombinedMuonTrackBuilder interface: build and fit combined ID/Calo/MS track */
        virtual std::unique_ptr<Trk::Track> combinedFit(const Trk::Track& indetTrack, const Trk::Track& extrapolatedTrack,
                                                        const Trk::Track& spectrometerTrack, const EventContext& ctx) const override;

        /** ICombinedMuonTrackBuilder interface:
            build and fit indet track extended to include MS Measurement set.
            Adds material effects as appropriate plus calo energy-loss treatment */
        virtual std::unique_ptr<Trk::Track> indetExtension(const Trk::Track& indetTrack, const Trk::MeasurementSet& spectrometerMeas,
                                                           const EventContext& ctx, const Trk::TrackParameters* innerParameters,
                                                           const Trk::TrackParameters* middleParameters,
                                                           const Trk::TrackParameters* outerParameters) const override;

        /** ICombinedMuonTrackBuilder interface:
            propagate to perigee adding calo energy-loss and material to MS track */
        virtual std::unique_ptr<Trk::Track> standaloneFit(const Trk::Track& spectrometerTrack, const EventContext& ctx,
                                                          const Trk::Vertex* vertex, float bs_x, float bs_y, float bs_z) const override;

        /** ICombinedMuonTrackBuilder interface:
            refit a track removing any indet measurements with optional addition of pseudoMeasurements
            according to original extrapolation */
        virtual std::unique_ptr<Trk::Track> standaloneRefit(const Trk::Track& combinedTrack, const EventContext& ctx, float bs_x,
                                                            float bs_y, float bs_z) const override;

        /** refit a track */
        virtual std::unique_ptr<Trk::Track> fit(Trk::Track& track, const EventContext& ctx, const Trk::RunOutlierRemoval runOutlier = false,
                                                const Trk::ParticleHypothesis particleHypothesis = Trk::muon) const override;

    private:
        /**
            combined muon fit */
        std::unique_ptr<Trk::Track> fit(const Trk::Track& indetTrack, const Trk::Track& extrapolatedTrack, const EventContext& ctx,
                                        const Trk::RunOutlierRemoval runOutlier = false,
                                        const Trk::ParticleHypothesis particleHypothesis = Trk::muon) const;

        Trk::Track* addIDMSerrors(Trk::Track* track) const;

        static std::unique_ptr<Trk::PseudoMeasurementOnTrack> 
        vertexOnTrack(const Trk::TrackParameters* parameters, const Trk::RecVertex& vertex) ;

        // helpers, managers, tools
        ToolHandle<Muon::IMuonTrackCleaner> m_cleaner{
            this,
            "Cleaner",
            "",
            "MuonTrackCleaner tool",
        };

        ToolHandle<Trk::ITrackFitter> m_fitter{
            this,
            "Fitter",
            "",
            "TrackFitter tool",
        };  // curved track fitter

        ToolHandle<Trk::ITrackSummaryTool> m_trackSummary{
            this,
            "TrackSummaryTool",
            "",
            "TrackSummary tool",
        };

        ToolHandle<Muon::IMuonHoleRecoveryTool> m_muonHoleRecovery{
            this,
            "MuonHoleRecovery",
            "",
            "MuonHoleRecovery tool",
        };

        ToolHandle<Muon::IMuonErrorOptimisationTool> m_muonErrorOptimizer{
            this,
            "MuonErrorOptimizer",
            "",
            "MuonErrorOptimizer tool",
        };

        ServiceHandle<Trk::ITrackingVolumesSvc> m_trackingVolumesSvc{this, "TrackingVolumesSvc", "TrackingVolumesSvc/TrackingVolumesSvc"};

        std::unique_ptr<const Trk::Volume> m_calorimeterVolume;
        std::unique_ptr<const Trk::Volume> m_indetVolume;

        // other configuration and tolerances
        bool m_allowCleanerVeto;
        bool m_cleanCombined;
        bool m_recoverCombined;
        double m_IDMS_xySigma;
        double m_IDMS_rzSigma;
        bool m_addIDMSerrors;

    };  // end of class OutwardsCombinedMuonTrackBuilder

}  // end of namespace Rec

#endif  // MUIDTRACKBUILDER_OUTWARDSCOMBINEDMUONTRACKBUILDER_H
