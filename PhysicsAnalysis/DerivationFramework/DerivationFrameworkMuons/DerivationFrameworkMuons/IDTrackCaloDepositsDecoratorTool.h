/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef IDTRACKCALODEPOSITSDECORATORTOOL_H_
#define IDTRACKCALODEPOSITSDECORATORTOOL_H_

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "DerivationFrameworkMuons/IIDTrackCaloDepositsDecoratorTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "ICaloTrkMuIdTools/ITrackDepositInCaloTool.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteDecorHandleKey.h"
#include "xAODMuon/MuonContainer.h"
///     Tool to store decorate the IParticle with all energy deposits in the calorimeter needed for post fine tuning of the cut-based
///     calorimeter tagging working points. The tool adds the following three decorations
///           Vector containing all energy deposits                               std::vector<float>    CaloDeposits
///           Vector containing all energy losses                                 std::vector<float>    CaloElosses
///           Vector declaring the type of the deposit/loss (EMB1,HEC0,etc.)      std::vector<unit_16>  CaloDepType
///     Deposits can be either saved to the Muon particle itself or to the associated ID track particle
///     However, in the latter case, the first ID track from the container is always decorated to ensure file integrety
class IDTrackCaloDepositsDecoratorTool : public AthAlgTool,
                                         virtual public IIDTrackCaloDepositsDecoratorTool,
                                         virtual public DerivationFramework::IAugmentationTool {
public:
    IDTrackCaloDepositsDecoratorTool(const std::string& t, const std::string& n, const IInterface* p);

    virtual ~IDTrackCaloDepositsDecoratorTool() = default;
    StatusCode initialize() override;

    StatusCode decorate(const xAOD::IParticle* part) const override;

    StatusCode addBranches() const override;

private:
    StatusCode recompute_and_decorate(const xAOD::IParticle* track_part) const;

    ToolHandle<ITrackDepositInCaloTool> m_trkDepositInCalo{this, "TrackDepositInCaloTool", "TrackDepositInCaloTool/TrackDepositInCaloTool"};

    /// Store gate key of the muon container
    SG::ReadHandleKey<xAOD::MuonContainer> m_muon_key{this, "MuonContainer", "Muons"};
    /// Key of the associated ID tracks.
    SG::ReadHandleKey<xAOD::TrackParticleContainer> m_id_trk_key{this, "InDetTrks", "InDetTrackParticles"};

    Gaudi::Property<bool> m_decor_muons{this, "DecorateMuons", true, "Switch to attach the decorations to the muon particles."};
    ///
    Gaudi::Property<bool> m_use_SAF{this, "IncludeSAF_Muons", false, "If the muons are not decorated directly"};

    SG::WriteDecorHandleKey<xAOD::IParticleContainer> m_deposit_key{
        this, "Calo_Deposit", "", "Name of the decorator to store all the energy deposits in the calorimeter"};
    SG::WriteDecorHandleKey<xAOD::IParticleContainer> m_eloss_key{this, "Calo_EnergyLoss", "",
                                                                  "Name of the decorator to store the energy loss from EMB1."};
    SG::WriteDecorHandleKey<xAOD::IParticleContainer> m_type_key{this, "Calo_Type", "",
                                                                 "Name of the decorator to store the energy deposit from EMB2."};
};

#endif
