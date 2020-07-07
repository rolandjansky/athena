/*
 Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 2020 Matthias Schott - Uni Mainz
*/

#ifndef MUON_TRACKALGORITHM_H
#define MUON_TRACKALGORITHM_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"

#include "xAODMuon/MuonContainer.h"
#include <vector>
#include <string>
#include "StoreGate/ReadHandleKey.h"

// AthMonitorAlgorithm
class MuonTrackMonitorAlgorithm : public AthMonitorAlgorithm
{

  public:

    MuonTrackMonitorAlgorithm(const std::string& name, ISvcLocator* pSvcLocator);

    virtual ~MuonTrackMonitorAlgorithm() {};
    virtual StatusCode initialize() override;
    virtual StatusCode fillHistograms( const EventContext& ctx ) const override;

  private:

    SG::ReadHandleKey<xAOD::MuonContainer> m_MuonContainerKey { this, "MuonContainerKey", "Muons", "Key for Muon Containers" };
    SG::ReadDecorHandleKey<xAOD::MuonContainer> m_MuonIsoDecorKey { this, "MuonIsoDecorKey", "Muons.ptcone30" };

    // Toolds
    /// Fills data-quality information (e.g. pt, eta, phi..) to histograms for given selection of muons
    /// std::string sIdentifier = "CB","ZBoson","JPsi": String which is used to match the histogramming
    ///    variables that are defined by the Python script
    /// std::vector<const xAOD::Muon*>	&vecMuons: Vector of muons for which performance plots should be created
    StatusCode	FillMuonInformation(std::string sIdentifier, std::vector<const xAOD::Muon*>	&vecMuons) const;

    /// Function to create performance plots for muon standalone tracks with some detailed informatiom
    StatusCode analyseLowLevelMuonFeatures(const xAOD::MuonContainer& Muons, uint32_t lumiBlockID) const;

    /// Function to create performance plots for all combined muons
    StatusCode analyseCombinedTracks(const xAOD::MuonContainer& Muons, uint32_t lumiBlockID) const;

    /// Function to create performance plots for all combined muons that lead to a Z Boson Candidate event
    StatusCode analyseZBosonCandidates(const xAOD::MuonContainer& Muons, uint32_t lumiBlockID) const;

    /// Function to create performance plots for all combined muons that lead to a JPsi Meson Candidate event
    StatusCode analyseJPsiCandidates(const xAOD::MuonContainer& Muons, uint32_t lumiBlockID) const;

    Gaudi::Property< std::vector<std::string> > m_hltchainList{ this, "HLTTriggerList", {"HLT_2mu14", "HLT_mu26_ivarmedium"}, "High-level triggers used" };

    Gaudi::Property< float > m_CBmuons_minPt{ this, "CBmuons_minPt", 20000., "Minimal muon pt used for CB muons" };

    Gaudi::Property< float > m_ZBosonSelection_minPt{ this, "ZBosonSelection_minPt", 20000., "Minimal muon pt used for Z analysis" };
    Gaudi::Property< float > m_ZBosonSelection_maxEta{ this, "ZBosonSelection_maxEta", 2.5, "Maximal muon eta used for Z analysis" };
    Gaudi::Property< float > m_ZBosonSelection_trkIsolation{ this, "ZBosonSelection_trkIsolation", 0.2, "Track DeltaR isolation criteria" };
    Gaudi::Property< float > m_ZBosonSelection_D0Cut{ this, "ZBosonSelection_D0Cut", 100., "D0 cut applied for Z boson analysis" };
    Gaudi::Property< float > m_ZBosonSelection_Z0Cut{ this, "ZBosonSelection_Z0Cut", 100., "Z0 cut applied for Z boson analysis" };
    Gaudi::Property< float > m_ZBosonSelection_minMass{ this, "ZBosonSelection_minMass", 76000., "Minimal accepted Z  boson mass" };
    Gaudi::Property< float > m_ZBosonSelection_maxMass{ this, "ZBosonSelection_maxMass", 106000., "Maximal accepted Z  boson mass" };

    Gaudi::Property< float > m_JPsiSelection_minPt{ this, "JPsiSelection_minPt", 4000., "Minimal muon pt used for JPsi analysis" };
    Gaudi::Property< float > m_JPsiSelection_maxEta{ this, "JPsiSelection_maxEta", 2.5, "Maximal muon eta used for JPsi analysis" };
    Gaudi::Property< float > m_JPsiSelection_trkIsolation{ this, "JPsiSelection_trkIsolation", 1.0, "JPsi track DeltaR isolation criteria" };
    Gaudi::Property< float > m_JPsiSelection_D0Cut{ this, "JPsiSelection_D0Cut", 100., "D0 cut applied for JPsi analysis" };
    Gaudi::Property< float > m_JPsiSelection_Z0Cut{ this, "JPsiSelection_Z0Cut", 100., "Z0 cut applied for JPsi analysis" };
    Gaudi::Property< float > m_JPsiSelection_minMass{ this, "JPsiSelection_minMass", 2600., "Minimal accepted JPsi mass" };
    Gaudi::Property< float > m_JPsiSelection_maxMass{ this, "JPsiSelection_maxMass", 3600., "Maximal accepted JPsi mass" };
};


#endif

