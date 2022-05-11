/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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
        virtual std::unique_ptr<Trk::Track> combinedFit(const EventContext& ctx, const Trk::Track& indetTrack, const Trk::Track& extrapolatedTrack,
                                                        const Trk::Track& spectrometerTrack) const override;

        /** ICombinedMuonTrackBuilder interface:
            build and fit indet track extended to include MS Measurement set.
            Adds material effects as appropriate plus calo energy-loss treatment */
        virtual std::unique_ptr<Trk::Track> indetExtension(const EventContext& ctx, const Trk::Track& indetTrack, const Trk::MeasurementSet& spectrometerMeas,
                                                           std::unique_ptr<Trk::TrackParameters> innerParameters,
                                                           std::unique_ptr<Trk::TrackParameters> middleParameters,
                                                           std::unique_ptr<Trk::TrackParameters> outerParameters) const override;

        /** ICombinedMuonTrackBuilder interface:
            propagate to perigee adding calo energy-loss and material to MS track */
        virtual std::unique_ptr<Trk::Track> standaloneFit(const EventContext& ctx, const Trk::Track& spectrometerTrack,
                                                          const Amg::Vector3D& bs, const Trk::Vertex* vertex) const override;

        /** ICombinedMuonTrackBuilder interface:
            refit a track removing any indet measurements with optional addition of pseudoMeasurements
            according to original extrapolation */
        virtual std::unique_ptr<Trk::Track> standaloneRefit( const EventContext& ctx, const Trk::Track& combinedTrack, const Amg::Vector3D& bs) const override;

        /** refit a track */
        virtual std::unique_ptr<Trk::Track> fit(const EventContext& ctx, Trk::Track& track, const Trk::RunOutlierRemoval runOutlier,
                                                const Trk::ParticleHypothesis particleHypothesis) const override;

    private:
        /**
            combined muon fit */
        std::unique_ptr<Trk::Track> fit(const EventContext& ctx, const Trk::Track& indetTrack, const Trk::Track& extrapolatedTrack, 
                                        const Trk::RunOutlierRemoval runOutlier, const Trk::ParticleHypothesis particleHypothesis) const;

        std::unique_ptr<Trk::Track> addIDMSerrors(const Trk::Track& track) const;

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

        std::unique_ptr<Trk::Volume> m_calorimeterVolume;
        std::unique_ptr<Trk::Volume> m_indetVolume;

     
        // other configuration and tolerances
        Gaudi::Property<bool> m_allowCleanerVeto{this, "AllowCleanerVeto", true};
        Gaudi::Property<bool> m_cleanCombined{this, "CleanCombined", true};
        Gaudi::Property<bool> m_recoverCombined{this, "RecoverCombined", false };
        Gaudi::Property<double> m_IDMS_xySigma{this,"IDMS_xySigma", 1.* Gaudi::Units::mm};
        Gaudi::Property<double> m_IDMS_rzSigma{this,"IDMS_rzSigma", 1.* Gaudi::Units::mm};
        Gaudi::Property<bool> m_addIDMSerrors{this, "AddIDMSerrors", true};

        Gaudi::Property<double> m_badFitChi2{this, "BadFitChi2", 2.5};
        double normalizedChi2(const Trk::Track& track) const;

    };  // end of class OutwardsCombinedMuonTrackBuilder

}  // end of namespace Rec

#endif  // MUIDTRACKBUILDER_OUTWARDSCOMBINEDMUONTRACKBUILDER_H
