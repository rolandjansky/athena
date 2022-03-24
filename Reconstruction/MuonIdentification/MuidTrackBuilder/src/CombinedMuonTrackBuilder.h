/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
// CombinedMuonTrackBuilder
//  AlgTool gathering  material effects along a combined muon track, in
//  particular the TSOS'es representing the calorimeter energy deposit and
//  Coulomb scattering.
//  The resulting track is fitted at the IP
//
//////////////////////////////////////////////////////////////////////////////

#ifndef MUIDTRACKBUILDER_COMBINEDMUONTRACKBUILDER_H
#define MUIDTRACKBUILDER_COMBINEDMUONTRACKBUILDER_H

#include <atomic>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "MagFieldConditions/AtlasFieldCacheCondObj.h"
#include "MuidInterfaces/ICombinedMuonTrackBuilder.h"
#include "MuidInterfaces/IMuidCaloEnergy.h"
#include "MuidInterfaces/IMuidCaloTrackStateOnSurface.h"
#include "MuidInterfaces/IMuonAlignmentUncertTool.h"
#include "MuidInterfaces/IMuonTrackQuery.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "MuonRecToolInterfaces/IMdtDriftCircleOnTrackCreator.h"
#include "MuonRecToolInterfaces/IMuonClusterOnTrackCreator.h"
#include "MuonRecToolInterfaces/IMuonErrorOptimisationTool.h"
#include "MuonRecToolInterfaces/IMuonHoleRecoveryTool.h"
#include "MuonRecToolInterfaces/IMuonTrackCleaner.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "TrkDetDescrInterfaces/ITrackingVolumesSvc.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkExInterfaces/IIntersector.h"
#include "TrkExInterfaces/IPropagator.h"
#include "TrkFitterInterfaces/ITrackFitter.h"
#include "TrkGeometry/MagneticFieldProperties.h"
#include "TrkGeometry/TrackingGeometry.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkToolInterfaces/ITrackSummaryTool.h"
#include "TrkToolInterfaces/ITrkMaterialProviderTool.h"
#include "TrkTrack/TrackInfo.h"
#include "TrkiPatFitterUtils/IMaterialAllocator.h"

class CaloEnergy;
class MessageHelper;

namespace Trk {
    class PerigeeSurface;
    class PseudoMeasurementOnTrack;
    class RecVertex;
    class TrackStateOnSurface;
}  // namespace Trk

namespace Rec {

    class CombinedMuonTrackBuilder : public AthAlgTool, virtual public ICombinedMuonTrackBuilder {
    public:
        CombinedMuonTrackBuilder(const std::string& type, const std::string& name, const IInterface* parent);
        virtual ~CombinedMuonTrackBuilder();

        virtual StatusCode initialize() override;
        virtual StatusCode finalize() override;

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
            refit a track removing any indet measurements with optional addition of pseudoMeasurements */
        virtual std::unique_ptr<Trk::Track> standaloneRefit(const Trk::Track& combinedTrack, const EventContext& ctx, float bs_x,
                                                            float bs_y, float bs_z) const override;

        /*refit a track */
        virtual std::unique_ptr<Trk::Track> fit(Trk::Track& track, const EventContext& ctx, const Trk::RunOutlierRemoval runOutlier,
                                                const Trk::ParticleHypothesis particleHypothesis) const override;

    private:
        /**
            fit a set of MeasurementBase objects with starting value for perigeeParameters */
        std::unique_ptr<Trk::Track> fit(const EventContext& ctx, const Trk::MeasurementSet& measurementSet,
                                        const Trk::TrackParameters& perigeeStartValue, const Trk::RunOutlierRemoval runOutlier,
                                        const Trk::ParticleHypothesis particleHypothesis) const;

        /**
            combined muon fit */
        std::unique_ptr<Trk::Track> fit(const EventContext& ctx, const Trk::Track& indetTrack, Trk::Track& extrapolatedTrack,
                                        const Trk::RunOutlierRemoval runOutlier = false,
                                        const Trk::ParticleHypothesis particleHypothesis = Trk::muon) const;

        bool optimizeErrors(const EventContext& ctx, Trk::Track* track) const;
        std::unique_ptr<Trk::Track> addIDMSerrors(const Trk::Track* track) const;

        void appendSelectedTSOS(DataVector<const Trk::TrackStateOnSurface>& trackStateOnSurfaces,
                                DataVector<const Trk::TrackStateOnSurface>::const_iterator begin,
                                DataVector<const Trk::TrackStateOnSurface>::const_iterator end) const;

        const CaloEnergy* caloEnergyParameters(const Trk::Track* combinedTrack, const Trk::Track* muonTrack,
                                               const Trk::TrackParameters*& combinedEnergyParameters,
                                               const Trk::TrackParameters*& muonEnergyParameters) const;

        std::unique_ptr<Trk::Track> createExtrapolatedTrack(
            const EventContext& ctx, const Trk::Track& spectrometerTrack, const Trk::TrackParameters& parameters,
            Trk::ParticleHypothesis particleHypothesis, Trk::RunOutlierRemoval runOutlier,
            const std::vector<std::unique_ptr<const Trk::TrackStateOnSurface>>& trackStateOnSurfaces, const Trk::RecVertex* vertex,
            const Trk::RecVertex* mbeamAxis, const Trk::PerigeeSurface* mperigeeSurface, const Trk::Perigee* seedParameter = nullptr) const;

        std::unique_ptr<Trk::Track> createIndetTrack(const Trk::TrackInfo& info,
                                                     DataVector<const Trk::TrackStateOnSurface>::const_iterator begin,
                                                     DataVector<const Trk::TrackStateOnSurface>::const_iterator end) const;
        /// Summarizes the available information about the ID track,
        ///            the deposited calorimeter energies and the track states into a new track
        ///   --> m_materialUpdator as only tool called , but does not provide an interface method with ctx thus far
        std::unique_ptr<Trk::Track> createMuonTrack(const EventContext& ctx, const Trk::Track& muonTrack,
                                                    const Trk::TrackParameters* parameters, const CaloEnergy* caloEnergy,
                                                    DataVector<const Trk::TrackStateOnSurface>::const_iterator begin,
                                                    DataVector<const Trk::TrackStateOnSurface>::const_iterator end, unsigned size) const;

        std::unique_ptr<Trk::TrackStateOnSurface> createPhiPseudoMeasurement(const Trk::Track& track, const EventContext& ctx) const;

        std::vector<std::unique_ptr<const Trk::TrackStateOnSurface>> createSpectrometerTSOS(const Trk::Track& spectrometerTrack,
                                                                                            const EventContext& ctx) const;

        const Trk::TrackStateOnSurface* entrancePerigee(const Trk::TrackParameters* parameters, const EventContext& ctx) const;

        std::unique_ptr<const Trk::TrackParameters> extrapolatedParameters(const EventContext& ctx, bool& badlyDeterminedCurvature,
                                                                           const Trk::Track& spectrometerTrack,
                                                                           const Trk::RecVertex* mvertex,
                                                                           const Trk::PerigeeSurface* mperigeeSurface) const;

        void finalTrackBuild(std::unique_ptr<Trk::Track>& track, const EventContext& ctx) const;

        void momentumUpdate(std::unique_ptr<const Trk::TrackParameters>& parameters, double updatedP, bool directionUpdate = false,
                            double deltaPhi = 0., double deltaTheta = 0.) const;

        double normalizedChi2(const Trk::Track& track) const;
        std::unique_ptr<Trk::Track> reallocateMaterial(const Trk::Track& spectrometerTrack, const EventContext& ctx) const;
        void replaceCaloEnergy(const CaloEnergy* caloEnergy, Trk::Track* track) const;
        void removeSpectrometerMaterial(std::unique_ptr<Trk::Track>& track) const;

        static std::unique_ptr<Trk::PseudoMeasurementOnTrack> vertexOnTrack(const Trk::TrackParameters& parameters,
                                                                            const Trk::RecVertex* vertex, const Trk::RecVertex* mbeamAxis);

        void dumpCaloEloss(const Trk::Track* track, const std::string& txt) const;
        int countAEOTs(const Trk::Track* track, const std::string& txt) const;
        bool checkTrack(const std::string& txt, const Trk::Track* newTrack, const Trk::Track* track) const;

        // helpers, managers, tools
        ToolHandle<Rec::IMuidCaloEnergy> m_caloEnergyParam{
            this,
            "CaloEnergyParam",
            "",
        };
        ToolHandle<Rec::IMuidCaloTrackStateOnSurface> m_caloTSOS{
            this,
            "CaloTSOS",
            "",
        };
        ToolHandle<Muon::IMuonTrackCleaner> m_cleaner{
            this,
            "Cleaner",
            "Muon::MuonTrackCleaner/MuidTrackCleaner",
        };
        ToolHandle<Muon::IMuonClusterOnTrackCreator> m_cscRotCreator{
            this,
            "CscRotCreator",
            "",
        };

        ToolHandle<Muon::IMuonClusterOnTrackCreator> m_mmRotCreator{this, "MMRotCreator", ""};

        ToolHandle<Trk::IExtrapolator> m_extrapolator{
            this,
            "Extrapolator",
            "Trk::Extrapolator/AtlasExtrapolator",
        };
        ToolHandle<Trk::ITrackFitter> m_fitter{
            this,
            "Fitter",
            "Trk::iPatFitter/iPatFitter",
        };  // curved track fitter
        ToolHandle<Trk::ITrackFitter> m_fitterSL{
            this,
            "SLFitter",
            "Trk::iPatFitter/iPatSLFitter",
        };  // straight line fitter
        ToolHandle<Trk::IMaterialAllocator> m_materialAllocator{
            this,
            "MaterialAllocator",
            "",
        };
        ToolHandle<Muon::IMdtDriftCircleOnTrackCreator> m_mdtRotCreator{
            this,
            "MdtRotCreator",
            "",
        };
        ToolHandle<Muon::IMuonErrorOptimisationTool> m_muonErrorOptimizer{
            this,
            "MuonErrorOptimizer",
            "",
        };
        ToolHandle<Muon::IMuonHoleRecoveryTool> m_muonHoleRecovery{
            this,
            "MuonHoleRecovery",
            "",
        };
        ToolHandle<Trk::IPropagator> m_propagator{
            this,
            "Propagator",
            "Trk::IntersectorWrapper/IntersectorWrapper",
        };
        ToolHandle<Trk::IPropagator> m_propagatorSL{
            this,
            "SLPropagator",
            "Trk::StraightLinePropagator/MuonStraightLinePropagator",
        };
        ToolHandle<Muon::MuonEDMPrinterTool> m_printer{
            this,
            "Printer",
            "Muon::MuonEDMPrinterTool/MuonEDMPrinterTool",
        };
        ToolHandle<Rec::IMuonTrackQuery> m_trackQuery{
            this,
            "TrackQuery",
            "Rec::MuonTrackQuery/MuonTrackQuery",
        };
        ToolHandle<Trk::ITrackSummaryTool> m_trackSummary{
            this,
            "TrackSummaryTool",
            "Trk::TrackSummaryTool/MuidTrackSummaryTool",
        };
        ToolHandle<Trk::ITrkMaterialProviderTool> m_materialUpdator{
            this,
            "CaloMaterialProvider",
            "",
        };

        /// ToolHandles to retrieve the uncertainties for theta and phi for
        /// the scattering uncertainties
        ToolHandle<Muon::IMuonAlignmentUncertTool> m_alignUncertTool_theta;
        ToolHandle<Muon::IMuonAlignmentUncertTool> m_alignUncertTool_phi;

        ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc{this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

        // Read handle for conditions object to get the field cache
        SG::ReadCondHandleKey<AtlasFieldCacheCondObj> m_fieldCacheCondObjInputKey{this, "AtlasFieldCacheCondObj", "fieldCondObj",
                                                                                  "Name of the Magnetic Field conditions object key"};

        SG::ReadCondHandleKey<Trk::TrackingGeometry> m_trackingGeometryReadKey{this, "TrackingGeometryReadKey", "",
                                                                               "Key of the TrackingGeometry conditions data."};

        ServiceHandle<Trk::ITrackingVolumesSvc> m_trackingVolumesSvc{this, "TrackingVolumesSvc", "TrackingVolumesSvc/TrackingVolumesSvc"};

        Trk::MagneticFieldProperties m_magFieldProperties{Trk::FullField};

        Gaudi::Property<bool> m_allowCleanerVeto{this, "AllowCleanerVeto", true};
        Gaudi::Property<bool> m_cleanCombined{this, "CleanCombined", true};
        Gaudi::Property<bool> m_cleanStandalone{this, "CleanStandalone", true};

        Gaudi::Property<bool> m_perigeeAtSpectrometerEntrance{this, "PerigeeAtSpectrometerEntrance", false};
        Gaudi::Property<bool> m_reallocateMaterial{this, "ReallocateMaterial", true};

        Gaudi::Property<double> m_badFitChi2{this, "BadFitChi2", 2.5};

        Gaudi::Property<double> m_largeImpact{this, "LargeImpact", 100. * Gaudi::Units::mm};

        Gaudi::Property<double> m_largeMomentumChange{this, "LargeMomentumChange", 0.05};
        Gaudi::Property<double> m_largeMomentumError{this, "LargeMomentumError", 0.15};
        Gaudi::Property<double> m_largePhiError{this, "LargePhiError", 0.020};
        Gaudi::Property<double> m_lineMomentum{this, "LineMomentum", 2. * Gaudi::Units::GeV};
        Gaudi::Property<double> m_lowMomentum{this, "LowMomentum", 10. * Gaudi::Units::GeV};

        Gaudi::Property<unsigned> m_maxWarnings{this, "MaxNumberOfWarnings", 10,
                                                "Maximum number of permitted WARNING messages per message type."};
        Gaudi::Property<double> m_minEnergy{this, "MinEnergy", 0.3 * Gaudi::Units::GeV};
        Gaudi::Property<double> m_numberSigmaFSR{this, "NumberSigmaFSR", 2.5};

        Gaudi::Property<double> m_vertex2DSigmaRPhi{this, "Vertex2DSigmaRPhi", 100. * Gaudi::Units::mm};
        Gaudi::Property<double> m_vertex2DSigmaZ{this, "Vertex2DSigmaZ", 100. * Gaudi::Units::meter};
        Gaudi::Property<double> m_vertex3DSigmaRPhi{this, "Vertex3DSigmaRPhi", 6. * Gaudi::Units::mm};
        Gaudi::Property<double> m_vertex3DSigmaZ{this, "Vertex3DSigmaZ", 60. * Gaudi::Units::mm};
        Gaudi::Property<double> m_zECToroid{this, "zECToroid", 10. * Gaudi::Units::meter};

        bool m_redoRots{false};

        // constants
        std::unique_ptr<const Trk::Volume> m_calorimeterVolume;
        std::unique_ptr<const Trk::Volume> m_indetVolume;

        // vertex region and phi modularity for pseudo-measurement constraints
        std::unique_ptr<Trk::RecVertex> m_beamAxis;
        std::unique_ptr<Trk::PerigeeSurface> m_perigeeSurface;

        std::unique_ptr<Trk::RecVertex> m_vertex;

        // counters
        mutable std::atomic_uint m_countAcceptedStandaloneFit{0};
        mutable std::atomic_uint m_countBeamAxis{0};
        mutable std::atomic_uint m_countCombinedCleanerVeto{0};
        mutable std::atomic_uint m_countDegradedStandaloneFit{0};
        mutable std::atomic_uint m_countExtensionCleanerVeto{0};
        mutable std::atomic_uint m_countStandaloneCleanerVeto{0};
        mutable std::atomic_uint m_countVertexRegion{0};

        // count warnings
        std::unique_ptr<MessageHelper> m_messageHelper;

        Gaudi::Property<bool> m_updateWithCaloTG{this, "UpdateWithCaloTG", false};
        Gaudi::Property<bool> m_useCaloTG{this, "UseCaloTG", false};
        Gaudi::Property<bool> m_iterateCombinedTrackFit{this, "IterateCombinedTrackFit", false};
        Gaudi::Property<bool> m_refineELossCombinedTrackFit{this, "RefineELossCombinedTrackFit", true};
        Gaudi::Property<bool> m_refineELossStandAloneTrackFit{this, "RefineELossStandAloneTrackFit", true};
        Gaudi::Property<bool> m_addElossID{this, "AddElossID", true};
        Gaudi::Property<bool> m_addIDMSerrors{this, "AddIDMSerrors", true};
        Gaudi::Property<bool> m_useRefitTrackError{this, "UseRefitTrackError", true};

        const Trk::TrackingVolume* getVolume(const std::string&& vol_name, const EventContext& ctx) const;
        const AtlasFieldCacheCondObj* getFieldCacheObj(const EventContext& ctx) const;
        bool loadMagneticField(const EventContext& ctx, MagField::AtlasFieldCache& field_cache) const;
        /// Helper method to retrieve the CaloTSO from the Material provider
        /// in a memory safe way
        std::vector<std::unique_ptr<const Trk::TrackStateOnSurface>> getCaloTSOSfromMatProvider(const Trk::TrackParameters& track_params,
                                                                                                const Trk::Track& me_track) const;

    };  // end of class CombinedMuonTrackBuilder

}  // end of namespace Rec

#endif  // MUIDTRACKBUILDER_COMBINEDMUONTRACKBUILDER_H
