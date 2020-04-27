/*
 Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 2020 Matthias Schott - Uni Mainz
*/

#ifndef MUON_TRACKALGORITHM_H
#define MUON_TRACKALGORITHM_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"

#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/MuonAuxContainer.h"
#include "MuonAnalysisInterfaces/IMuonSelectionTool.h"
#include "MuonResonanceTools/IMuonResonanceSelectionTool.h"
#include "MuonResonanceTools/IMuonResonancePairingTool.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include <memory>


class MuonTrackMonitorAlgorithm : public AthMonitorAlgorithm
{

  public:

    MuonTrackMonitorAlgorithm(const std::string& name, ISvcLocator* svcLocator);

    virtual ~MuonTrackMonitorAlgorithm();
    virtual StatusCode initialize() override;
    virtual StatusCode fillHistograms( const EventContext& ctx ) const override;

  private:

    SG::ReadHandleKey<xAOD::MuonContainer> m_MuonContainerKey
      { this, "MuonContainerKey", "Muons", "Key for Muon Containers" };

	// Toolds
    ToolHandle<CP::IMuonSelectionTool> 		m_muonSelectionTool;
	ToolHandle< Trig::TrigDecisionTool >	m_triggerDecisionTool;

	/// Fills data-quality information (e.g. pt, eta, phi..) to histograms for given selection of muons
	/// std::string sIdentifier = "CB","ZBoson","JPsi": String which is used to match the histogramming
	///    variables that are defined by the Python script
	/// std::vector<const xAOD::Muon*>	&vecMuons: Vector of muons for which performance plots should be created
	StatusCode	FillMuonInformation(std::string sIdentifier, std::vector<const xAOD::Muon*>	&vecMuons) const;

	/// Function to create performance plots for muon standalone tracks with some detailed informatiom
	StatusCode	analyseLowLevelMuonFeatures(const xAOD::MuonContainer* Muons, int lumiBlockID) const;

	/// Function to create performance plots for all combined muons
	StatusCode	analyseCombinedTracks(const xAOD::MuonContainer* Muons, int lumiBlockID) const;

	/// Function to create performance plots for all combined muons that lead to a Z Boson Candidate event
	StatusCode	analyseZBosonCandidates(const xAOD::MuonContainer* Muons, int lumiBlockID) const;

	/// Function to create performance plots for all combined muons that lead to a JPsi Meson Candidate event
	StatusCode	analyseJPsiCandidates(const xAOD::MuonContainer* Muons, int lumiBlockID) const;

    // Define configurable cuts to Select Z Bosons
    float 						m_ZBosonSelection_minPt;
    float 						m_ZBosonSelection_maxEta;
    float 						m_ZBosonSelection_trkIsolation;
    float 						m_ZBosonSelection_D0Cut;
    float 						m_ZBosonSelection_Z0Cut;
    float 						m_ZBosonSelection_minMass;
    float 						m_ZBosonSelection_maxMass;

    // Define configurable cuts to Select JPsi Mesons
    float 						m_JPsiSelection_minPt;
    float 						m_JPsiSelection_maxEta;
    float 						m_JPsiSelection_trkIsolation;
    float 						m_JPsiSelection_D0Cut;
    float 						m_JPsiSelection_Z0Cut;
    float 						m_JPsiSelection_minMass;
    float 						m_JPsiSelection_maxMass;

	// Define which Triggers should be checked
    std::vector<std::string> 	m_hltchainList;
    
};


#endif

