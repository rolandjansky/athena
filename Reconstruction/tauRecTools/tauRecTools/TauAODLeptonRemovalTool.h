/*
    Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/**
 *  @brief the tool meant to run at AOD level, to remove the lepton found inside the TauJet.
 */

#ifndef TAURECTOOLS_TAUAODLEPTONREMOVALTOOL_H
#define TAURECTOOLS_TAUAODLEPTONREMOVALTOOL_H

#include "tauRecTools/TauRecToolBase.h"
#include "xAODTau/TauJet.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "AsgDataHandles/ReadHandle.h"
#include "AsgDataHandles/ReadHandleKey.h"
#include "AsgTools/PropertyWrapper.h"

class TauAODLeptonRemovalTool : public TauRecToolBase 
{
    public:
        ASG_TOOL_CLASS2( TauAODLeptonRemovalTool, TauRecToolBase, ITauToolBase )
        TauAODLeptonRemovalTool(const std::string& type);
        virtual StatusCode initialize() override;
        virtual StatusCode execute(xAOD::TauJet&) const override;
    private:
        const std::map<std::string, uint>           m_mapMuonIdWp        = {{"Tight", 0}, {"Medium", 1}, {"Loose", 2}, {"VeryLoose",3}};
        std::string                                 m_elecWpStr          = "DFCommonElectronsLHMedium";
        uint                                        m_muonWpUi           = 1;
        SG::ReadHandleKey<xAOD::MuonContainer>      m_muonInputContainer{this, "Key_MuonInputContainer", "Muons",     "input xAOD muons"};
        SG::ReadHandleKey<xAOD::ElectronContainer>  m_elecInputContainer{this, "Key_ElecInputContainer", "Electrons", "input xAOD electrons"};
        //properties
        Gaudi::Property<bool>        m_doMuonTrkRm       {this, "doMuonTrkRm",        false,                 "Whether to remove the muon tracks from the tau candidate"             };
        Gaudi::Property<bool>        m_doElecTrkRm       {this, "doElecTrkRm",        false,                 "Whether to remove the electron tracks from the tau candidate"         };
        Gaudi::Property<bool>        m_doMuonClsRm       {this, "doMuonClsRm",        false,                 "Whether to remove the muon clusters from the tau candidate"           };
        Gaudi::Property<bool>        m_doElecClsRm       {this, "doElecClsRm",        false,                 "Whether to remove the electron clusters from the tau candidate"       };
        Gaudi::Property<std::string> m_strMinElecIdWp    {this, "elecIDWP",           "Medium",              "minimum electron identification WP, [VeryLoose, Loose, Medium, Tight]"};
        Gaudi::Property<std::string> m_strMinMuonIdWp    {this, "muonIDWP",           "Medium",              "minimum muon identification WP, [VeryLoose, Loose, Medium, Tight]"    };
        Gaudi::Property<std::string> m_strElecIdWpPrefix {this, "eleIDWPPrefix",      "DFCommonElectronsLH", "The prefix of the electron ID WP, leave to default if in confusion"   };
        Gaudi::Property<double>      m_lepRemovalConeSize{this, "lepRemovalConeSize", 0.6,                   "The maximum dR between the lepton and the tau"                        };
        //helpers
        std::vector<const xAOD::CaloCluster*>                                       getOrignalTopoClusters (const xAOD::CaloCluster   *cluster) const;
        const xAOD::TrackParticle*                                                  getOrignalTrackParticle(const xAOD::TrackParticle *trk  )   const;
        std::vector<std::pair<const xAOD::TrackParticle*, const xAOD::Electron*>>   getElecAndTrk(const xAOD::TauJet& tau,  const xAOD::ElectronContainer& elec_cont) const;
        std::vector<std::pair<const xAOD::CaloCluster*,   const xAOD::Electron*>>   getElecAndCls(const xAOD::TauJet& tau,  const xAOD::ElectronContainer& elec_cont) const;
        std::vector<std::pair<const xAOD::TrackParticle*,     const xAOD::Muon*>>   getMuonAndTrk(const xAOD::TauJet& tau,  const xAOD::MuonContainer& muon_cont) const;
        std::vector<std::pair<const xAOD::CaloCluster*,       const xAOD::Muon*>>   getMuonAndCls(const xAOD::TauJet& tau,  const xAOD::MuonContainer& muon_cont) const;
        template<typename Tlep, typename Tlinks> std::vector<Tlep>                  removeTrks(Tlinks& tau_trk_links,   std::vector<std::pair<const xAOD::TrackParticle*, Tlep>>& removings) const;
        template<typename Tlep, typename Tlinks> std::vector<Tlep>                  removeClss(Tlinks& tau_cls_links,   std::vector<std::pair<const xAOD::CaloCluster*, Tlep>>& clusters_and_leps) const;
};

#endif// TAURECTOOLS_TAUAODLEPTONREMOVALTOOL_H
