/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
// OutwardsCombinedMuonTrackBuilder
//  AlgTool gathering  material effects along a combined muon track, in
//  particular the TSOS'es representing the calorimeter energy deposit and
//  Coulomb scattering.
//  The resulting track is fitted at the IP using the ITrackFitter interface.
//
//  (c) ATLAS Combined Muon software
//////////////////////////////////////////////////////////////////////////////

#ifndef MUIDTRACKBUILDER_OUTWARDSCOMBINEDMUONTRACKBUILDER_H
#define MUIDTRACKBUILDER_OUTWARDSCOMBINEDMUONTRACKBUILDER_H

#include "MuidInterfaces/ICombinedMuonTrackBuilder.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonRecToolInterfaces/IMuonErrorOptimisationTool.h"
#include "MuonRecToolInterfaces/IMuonHoleRecoveryTool.h"
#include "MuonRecToolInterfaces/IMuonTrackCleaner.h"
#include "TrkDetDescrInterfaces/ITrackingVolumesSvc.h"
#include "TrkGeometry/TrackingVolume.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkFitterInterfaces/ITrackFitter.h"
#include "TrkToolInterfaces/ITrackSummaryTool.h"
#include "TrkTrack/TrackInfo.h"

#include <memory>

namespace Trk {
    class RecVertex;
    class PseudoMeasurementOnTrack;
    class VertexOnTrack;
}

namespace Rec {

class OutwardsCombinedMuonTrackBuilder : public AthAlgTool, virtual public ICombinedMuonTrackBuilder {
  public:
    OutwardsCombinedMuonTrackBuilder(const std::string& type, const std::string& name, const IInterface* parent);
    ~OutwardsCombinedMuonTrackBuilder()=default;

    StatusCode initialize();

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
        refit a track removing any indet measurements with optional addition of pseudoMeasurements
        according to original extrapolation */
    Trk::Track* standaloneRefit(const Trk::Track& combinedTrack, float bs_x, float bs_y, float bs_z) const;
    
    using ICombinedMuonTrackBuilder::fit;
    /** refit a track */
    Trk::Track* fit(Trk::Track& track, const Trk::RunOutlierRemoval runOutlier = false,
                    const Trk::ParticleHypothesis particleHypothesis = Trk::muon) const;


    /** 
    fit a set of PrepRawData objects */
    Trk::Track* fit(const Trk::PrepRawDataSet&, const Trk::TrackParameters& /*perigeeStartValue*/,
                    const Trk::RunOutlierRemoval /*runOutlier*/,
                    const Trk::ParticleHypothesis /*particleHypothesis*/) const
    {
        return nullptr;
    };

    /** 
        refit a track adding a MeasurementSet */
    Trk::Track* fit(const Trk::Track& /*track*/, const Trk::MeasurementSet& /*measurementSet*/,
                    const Trk::RunOutlierRemoval /*runOutlier*/,
                    const Trk::ParticleHypothesis /*particleHypothesis*/) const
    {
        return nullptr;
    };

    /** 
        fit a set of MeasurementBase objects with starting value for perigeeParameters */
    Trk::Track* fit(const Trk::MeasurementSet& /*measurementSet*/, const Trk::TrackParameters& /*perigeeStartValue*/,
                    const Trk::RunOutlierRemoval /*runOutlier*/,
                    const Trk::ParticleHypothesis /*particleHypothesis*/) const;

    /** 
        combined muon fit */
    Trk::Track* fit(const Trk::Track& indetTrack, const Trk::Track& extrapolatedTrack,
                    const Trk::RunOutlierRemoval  runOutlier         = false,
                    const Trk::ParticleHypothesis particleHypothesis = Trk::muon) const;

  private:
    Trk::Track* addIDMSerrors(Trk::Track* track) const;

    Trk::PseudoMeasurementOnTrack* vertexOnTrack(const Trk::TrackParameters* parameters,
                                                 const Trk::RecVertex&       vertex) const;


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

    ServiceHandle<Trk::ITrackingVolumesSvc> m_trackingVolumesSvc{this,"TrackingVolumesSvc","TrackingVolumesSvc/TrackingVolumesSvc"};

    std::unique_ptr<const Trk::Volume> m_calorimeterVolume;
    std::unique_ptr<const Trk::Volume> m_indetVolume;

    // other configuration and tolerances
    bool   m_allowCleanerVeto;
    bool   m_cleanCombined;
    bool   m_recoverCombined;
    double m_IDMS_xySigma;
    double m_IDMS_rzSigma;
    bool   m_addIDMSerrors;

};  // end of class OutwardsCombinedMuonTrackBuilder


}  // end of namespace Rec

#endif  // MUIDTRACKBUILDER_OUTWARDSCOMBINEDMUONTRACKBUILDER_H
