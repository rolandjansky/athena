/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
// CombinedMuonTrackBuilder
//  AlgTool gathering  material effects along a combined muon track, in
//  particular the TSOS'es representing the calorimeter energy deposit and
//  Coulomb scattering.
//  The resulting track is fitted at the IP
//
//  (c) ATLAS Combined Muon software
//////////////////////////////////////////////////////////////////////////////

#ifndef MUIDTRACKBUILDER_COMBINEDMUONTRACKBUILDER_H
#define MUIDTRACKBUILDER_COMBINEDMUONTRACKBUILDER_H

#include "MuidInterfaces/ICombinedMuonTrackBuilder.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "MagFieldConditions/AtlasFieldCacheCondObj.h"
#include "MuidInterfaces/IMuidCaloEnergy.h"
#include "MuidInterfaces/IMuidCaloTrackStateOnSurface.h"
#include "MuidInterfaces/IMuonTrackQuery.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "MuonRecToolInterfaces/IMdtDriftCircleOnTrackCreator.h"
#include "MuonRecToolInterfaces/IMuonClusterOnTrackCreator.h"
#include "MuonRecToolInterfaces/IMuonErrorOptimisationTool.h"
#include "MuonRecToolInterfaces/IMuonHoleRecoveryTool.h"
#include "MuonRecToolInterfaces/IMuonTrackCleaner.h"
#include "TrkDetDescrInterfaces/ITrackingGeometrySvc.h"
#include "TrkDetDescrInterfaces/ITrackingVolumesSvc.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkExInterfaces/IIntersector.h"
#include "TrkExInterfaces/IPropagator.h"
#include "TrkFitterInterfaces/ITrackFitter.h"
#include "TrkGeometry/MagneticFieldProperties.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkToolInterfaces/ITrackSummaryTool.h"
#include "TrkToolInterfaces/ITrkMaterialProviderTool.h"
#include "TrkTrack/TrackInfo.h"
#include "TrkiPatFitterUtils/IMaterialAllocator.h"

#include <atomic>

class CaloEnergy;
class MessageHelper;

namespace Trk {
    class PerigeeSurface;
    class PseudoMeasurementOnTrack;
    class RecVertex;
    class TrackStateOnSurface;
}

namespace Rec {

class CombinedMuonTrackBuilder : public AthAlgTool, virtual public ICombinedMuonTrackBuilder {
  public:
    CombinedMuonTrackBuilder(const std::string& type, const std::string& name, const IInterface* parent);
    ~CombinedMuonTrackBuilder() = default;

    StatusCode initialize();
    StatusCode finalize();

    /** ICombinedMuonTrackBuilder interface: build and fit combined ID/Calo/MS track */
    Trk::Track* combinedFit(const Trk::Track& indetTrack, const Trk::Track& extrapolatedTrack,
                            const Trk::Track& spectrometerTrack) const;

    /** ICombinedMuonTrackBuilder interface:
        build and fit indet track extended to include MS Measurement set.
        Adds material effects as appropriate plus calo energy-loss treatment */
    Trk::Track* indetExtension(const Trk::Track& indetTrack, const Trk::MeasurementSet& spectrometerMeas,
                               const Trk::TrackParameters* innerParameters,
                               const Trk::TrackParameters* middleParameters,
                               const Trk::TrackParameters* outerParameters) const;

    /** ICombinedMuonTrackBuilder interface:
        propagate to perigee adding calo energy-loss and material to MS track */
    Trk::Track* standaloneFit(const Trk::Track& spectrometerTrack, const Trk::Vertex* vertex, float bs_x, float bs_y,
                              float bs_z) const;

    /** ICombinedMuonTrackBuilder interface:
        refit a track removing any indet measurements with optional addition of pseudoMeasurements */
    Trk::Track* standaloneRefit(const Trk::Track& combinedTrack, float bs_x, float bs_y, float bs_z) const;

    using ICombinedMuonTrackBuilder::fit;

    /*refit a track */
    Trk::Track* fit(Trk::Track& track, const Trk::RunOutlierRemoval runOutlier = false,
                    const Trk::ParticleHypothesis particleHypothesis = Trk::muon) const;

    /**
        fit a set of MeasurementBase objects with starting value for perigeeParameters */
    Trk::Track* fit(const Trk::MeasurementSet& /*measurementSet*/, const Trk::TrackParameters& /*perigeeStartValue*/,
                    const Trk::RunOutlierRemoval /*runOutlier*/,
                    const Trk::ParticleHypothesis /*particleHypothesis*/) const;

    /**
        combined muon fit */
    Trk::Track* fit(const Trk::Track& indetTrack, Trk::Track& extrapolatedTrack,
                    const Trk::RunOutlierRemoval  runOutlier         = false,
                    const Trk::ParticleHypothesis particleHypothesis = Trk::muon) const;

  private:
    bool        optimizeErrors(Trk::Track* track) const;
    Trk::Track* addIDMSerrors(Trk::Track* track) const;

    void appendSelectedTSOS(DataVector<const Trk::TrackStateOnSurface>&                trackStateOnSurfaces,
                            DataVector<const Trk::TrackStateOnSurface>::const_iterator begin,
                            DataVector<const Trk::TrackStateOnSurface>::const_iterator end) const;

    const CaloEnergy* caloEnergyParameters(const Trk::Track* combinedTrack, const Trk::Track* muonTrack,
                                           const Trk::TrackParameters*& combinedEnergyParameters,
                                           const Trk::TrackParameters*& muonEnergyParameters) const;

    Trk::Track* createExtrapolatedTrack(const Trk::Track& spectrometerTrack, const Trk::TrackParameters& parameters,
                                        Trk::ParticleHypothesis particleHypothesis, Trk::RunOutlierRemoval runOutlier,
                                        const std::vector<const Trk::TrackStateOnSurface*>& trackStateOnSurfaces,
                                        const Trk::RecVertex* vertex, const Trk::RecVertex* mbeamAxis,
                                        const Trk::PerigeeSurface* mperigeeSurface,
                                        const Trk::Perigee*        seedParameter = nullptr) const;

    Trk::Track* createIndetTrack(const Trk::TrackInfo&                                      info,
                                 DataVector<const Trk::TrackStateOnSurface>::const_iterator begin,
                                 DataVector<const Trk::TrackStateOnSurface>::const_iterator end) const;

    Trk::Track* createMuonTrack(const Trk::Track& muonTrack, const Trk::TrackParameters* parameters,
                                const CaloEnergy*                                          caloEnergy,
                                DataVector<const Trk::TrackStateOnSurface>::const_iterator begin,
                                DataVector<const Trk::TrackStateOnSurface>::const_iterator end, unsigned size) const;

    const Trk::TrackStateOnSurface* createPhiPseudoMeasurement(const Trk::Track& track) const;

    std::vector<const Trk::TrackStateOnSurface*>* createSpectrometerTSOS(const Trk::Track& spectrometerTrack) const;

    const Trk::TrackStateOnSurface* entrancePerigee(const Trk::TrackParameters* parameters) const;

    const Trk::TrackParameters* extrapolatedParameters(bool&                      badlyDeterminedCurvature,
                                                       const Trk::Track&          spectrometerTrack,
                                                       const Trk::RecVertex*      mvertex,
                                                       const Trk::PerigeeSurface* mperigeeSurface) const;

    void        finalTrackBuild(Trk::Track*& track) const;
    Trk::Track* interfaceNotImplemented() const;

    void momentumUpdate(const Trk::TrackParameters*& parameters, double updatedP, bool directionUpdate = false,
                        double deltaPhi = 0., double deltaTheta = 0.) const;

    double            normalizedChi2(const Trk::Track& track) const;
    const Trk::Track* reallocateMaterial(const Trk::Track& spectrometerTrack) const;
    void              replaceCaloEnergy(const CaloEnergy* caloEnergy, Trk::Track* track) const;
    void              removeSpectrometerMaterial(Trk::Track*& track) const;

    Trk::Track* trackCleaner(Trk::Track* combinedTrack, const Trk::Track* indetTrack,
                             const Trk::Track& muonTrack) const;

    Trk::PseudoMeasurementOnTrack* vertexOnTrack(const Trk::TrackParameters& parameters, const Trk::RecVertex* vertex,
                                                 const Trk::RecVertex* mbeamAxis) const;

    void dumpCaloEloss(const Trk::Track* track, std::string txt) const;
    int  countAEOTs(const Trk::Track* track, std::string txt) const;
    bool checkTrack(std::string txt, Trk::Track* newTrack, Trk::Track* track) const;

    // helpers, managers, tools
    ToolHandle<Rec::IMuidCaloEnergy> m_caloEnergyParam{
        this,
        "CaloEnergyParam",
        "Rec::MuidCaloEnergyTool/MuidCaloEnergyToolParam",
    };
    ToolHandle<Rec::IMuidCaloTrackStateOnSurface> m_caloTSOS{
        this,
        "CaloTSOS",
        "Rec::MuidCaloTrackStateOnSurface/MuidCaloTrackStateOnSurface",
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
    ToolHandle<Trk::IIntersector> m_intersector{
        this,
        "Intersector",
        "Trk::RungeKuttaIntersector/RungeKuttaIntersector",
    };
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
        "Muon::MuonErrorOptimisationTool/MuidErrorOptimisationTool",
    };
    ToolHandle<Muon::IMuonHoleRecoveryTool> m_muonHoleRecovery{
        this,
        "MuonHoleRecovery",
        "Muon::MuonChamberHoleRecoveryTool/MuonChamberHoleRecoveryTool",
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
        "Trk::TrkMaterialProviderTool/TrkMaterialProviderTool",
    };

    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc{this, "MuonIdHelperSvc",
                                                        "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

    // Read handle for conditions object to get the field cache
    SG::ReadCondHandleKey<AtlasFieldCacheCondObj> m_fieldCacheCondObjInputKey{
        this, "AtlasFieldCacheCondObj", "fieldCondObj", "Name of the Magnetic Field conditions object key"};
    ServiceHandle<Trk::ITrackingGeometrySvc> m_trackingGeometrySvc{this,"TrackingGeometrySvc","TrackingGeometrySvc/AtlasTrackingGeometrySvc"};  // init with callback
    ServiceHandle<Trk::ITrackingVolumesSvc>  m_trackingVolumesSvc{this,"TrackingVolumesSvc","TrackingVolumesSvc/TrackingVolumesSvc"};

    Trk::MagneticFieldProperties m_magFieldProperties;

    // other configuration and tolerances
    mutable std::atomic_bool m_perigeeAtSpectrometerEntranceLocal{false};

    bool     m_allowCleanerVeto;
    bool     m_cleanCombined;
    bool     m_cleanStandalone;
    bool     m_perigeeAtSpectrometerEntrance;
    bool     m_reallocateMaterial;
    double   m_badFitChi2;
    double   m_largeImpact;
    double   m_largeMomentumChange;
    double   m_largeMomentumError;
    double   m_largePhiError;
    double   m_lineMomentum;
    double   m_lowMomentum;
    unsigned m_maxWarnings;
    double   m_minEnergy;
    double   m_numberSigmaFSR;
    bool     m_redoRots;
    double   m_vertex2DSigmaRPhi;
    double   m_vertex2DSigmaZ;
    double   m_vertex3DSigmaRPhi;
    double   m_vertex3DSigmaZ;
    double   m_zECToroid;
    double   m_IDMS_xySigma;
    double   m_IDMS_rzSigma;

    // dummy (unused - kept for backwards compatibility)
    bool m_indetSlimming;
    bool m_inputSlimming;

    // constants
    const Trk::Volume*         m_calorimeterVolume;
    const Trk::Volume*         m_indetVolume;
    const Trk::TrackingVolume* m_spectrometerEntrance;

    // vertex region and phi modularity for pseudo-measurement constraints
    Trk::RecVertex*      m_beamAxis;
    Trk::PerigeeSurface* m_perigeeSurface;
    double               m_sigmaPhiSector;
    Trk::RecVertex*      m_vertex;

    // counters
    mutable std::atomic_uint m_countAcceptedStandaloneFit;
    mutable std::atomic_uint m_countBeamAxis;
    mutable std::atomic_uint m_countCombinedCleanerVeto;
    mutable std::atomic_uint m_countDegradedStandaloneFit;
    mutable std::atomic_uint m_countExtensionCleanerVeto;
    mutable std::atomic_uint m_countStandaloneCleanerVeto;
    mutable std::atomic_uint m_countVertexRegion;

    // count warnings
    MessageHelper* m_messageHelper;

    bool m_updateWithCaloTG;
    bool m_useCaloTG;
    bool m_iterateCombinedTrackFit;
    bool m_refineELossCombinedTrackFit;
    bool m_refineELossStandAloneTrackFit;
    bool m_addElossID;
    bool m_addIDMSerrors;
    bool m_useRefitTrackError;

};  // end of class CombinedMuonTrackBuilder


}  // end of namespace Rec

#endif  // MUIDTRACKBUILDER_COMBINEDMUONTRACKBUILDER_H
