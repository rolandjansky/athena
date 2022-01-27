/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef MuonTPExtrapolationTool_H
#define MuonTPExtrapolationTool_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "DerivationFrameworkMuons/IMuonTPExtrapolationTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteDecorHandleKey.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "xAODMuon/MuonContainer.h"

class MuonTPExtrapolationTool : public AthAlgTool,
                                virtual public IMuonTPExtrapolationTool,
                                virtual public DerivationFramework::IAugmentationTool {
public:
    MuonTPExtrapolationTool(const std::string& t, const std::string& n, const IInterface* p);
    virtual ~MuonTPExtrapolationTool() = default;

    virtual StatusCode initialize() override;

    /// compute dR on trigger pivot plane
    /// see interface class for full description
    double dROnTriggerPivotPlane(const xAOD::Muon& tag, const xAOD::IParticle* probe) const override;

    // this method is intended for use in the DAOD production. It takes only one particle as argument, and handles the decoration.
    StatusCode decoratePivotPlaneCoords(const xAOD::IParticle* particle) const override;

    StatusCode addBranches() const override;

private:
    /// run the extrapolation - only available in full athena
    std::unique_ptr<const Trk::TrackParameters> extrapolateToTriggerPivotPlane(const xAOD::TrackParticle& track) const;

    // Utility method to handle extrapolation and decoration for one TrackParticle.
    // In AthAnalysis, it attempts to read the decoration that should be present in the DAODs we use (MUON1/2).
    // In full athena, it looks for the decoration, and, if it is missing, runs track extrapolation, decorating the result
    // to the particle to avoid repeating the process unnecessarily.
    // Returns success (true) or failure (false) of the procedure, fills eta and phi coordinates via reference
    // If the extrapolation fails or the decoration is missing in AthAnalysis, it will *not* change eta and phi
    // So you can set them to defaults before calling this guy, and they will be preserved in case of failure.
    bool extrapolateAndDecorateTrackParticle(const xAOD::IParticle* particle, float& eta, float& phi) const;

    /// Checks whether the extrapolation has been applied on the probe or nt
    bool is_extrapolated(const xAOD::IParticle* probe) const;

    // utility method: Obtains the track particle which we want to extrapolate into the MS.
    // Works for all kinds of probes.
    const xAOD::TrackParticle* getPreferredTrackParticle(const xAOD::IParticle* probe) const;

    // these define the surfaces that we extrapolate to.
    // We approximate the pivot plane in the form of a cylinder surface and two disks
    ToolHandle<Trk::IExtrapolator> m_extrapolator{this, "Extrapolator", "Trk::Extrapolator/AtlasExtrapolator"};

    Gaudi::Property<float> m_endcapPivotPlaneZ{this, "EndcapPivotPlaneZ", 15525., "z position of pivot plane in endcap region"};
    Gaudi::Property<float> m_endcapPivotPlaneMinimumRadius{this, "EndcapPivotPlaneMinimumRadius", 0.,
                                                           "minimum radius of pivot plane in endcap region"};
    Gaudi::Property<float> m_endcapPivotPlaneMaximumRadius{this, "EndcapPivotPlaneMaximumRadius", 11977.,
                                                           "maximum radius of pivot plane in endcap region"};
    Gaudi::Property<float> m_barrelPivotPlaneRadius{this, "BarrelPivotPlaneRadius", 8000., "adius of pivot plane in barrel region"};
    Gaudi::Property<float> m_barrelPivotPlaneHalfLength{this, "BarrelPivotPlaneHalfLength", 9700,
                                                        "half length of pivot plane in barrel region"};

    Gaudi::Property<bool> m_run_extrapolation{this, "Extrapolate", true,
                                              "Switch whether the particles shall be extrapolated to the pivot planes"};

    SG::ReadHandleKey<xAOD::MuonContainer> m_muon_key{this, "MuonKey", "Muons"};
    /// Key of the associated ID tracks.
    SG::ReadHandleKey<xAOD::TrackParticleContainer> m_id_trk_key{this, "InDetTrks", "InDetTrackParticles"};

    SG::WriteDecorHandleKey<xAOD::IParticleContainer> m_ext_eta_key{this, "Key_ExtrTP_Eta", ""};
    SG::WriteDecorHandleKey<xAOD::IParticleContainer> m_ext_phi_key{this, "Key_ExtrTP_Phi", ""};
    SG::WriteDecorHandleKey<xAOD::IParticleContainer> m_ext_stat_key{this, "Key_Extr_Status", ""};

    /// Property to directly cache the decorations to the muons
    Gaudi::Property<bool> m_decor_muons{this, "DecorateMuons", true, "Switch to attach the decorations to the muon particles."};
    /// Use SAF muons
    Gaudi::Property<bool> m_use_SAF{this, "IncludeSAF_Muons", false, "If the muons are not decorated directly"};
};

#endif
