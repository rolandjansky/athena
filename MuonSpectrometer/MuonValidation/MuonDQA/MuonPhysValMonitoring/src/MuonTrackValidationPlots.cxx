/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTrackValidationPlots.h"

MuonTrackValidationPlots::MuonTrackValidationPlots(PlotBase* pParent, const std::string& sDir, const std::string& sTrackType, bool isData):
  PlotBase(pParent, sDir)
  , m_oRecoMuonTrackPlots(nullptr)
  , m_oMatchedMuonTrackPlots(nullptr)
  , m_sTrackType(sTrackType)
{

  std::vector<int> selectedRecoPlots(0);
  selectedRecoPlots.push_back(Muon::TRK_PARAM);
  selectedRecoPlots.push_back(Muon::TRK_RECOINFO);
  selectedRecoPlots.push_back(Muon::TRK_IMPACT);
  if (sTrackType.find("ID")!= std::string::npos)
    selectedRecoPlots.push_back(Muon::TRK_IDHITS);
  else 
    selectedRecoPlots.push_back(Muon::TRK_MSHITS);  
  m_oRecoMuonTrackPlots = new Muon::RecoMuonTrackPlotOrganizer(this, "reco/"+sTrackType,&selectedRecoPlots);

  if (!isData) {
    std::vector<int> selectedMatchedPlots(0);
    selectedMatchedPlots.push_back(Muon::TRK_MATCHEDTRUE);
    selectedMatchedPlots.push_back(Muon::TRK_MATCHEDRECO);
    selectedMatchedPlots.push_back(Muon::TRK_DEFPARAMPULLS);
    selectedMatchedPlots.push_back(Muon::MUON_RESOL);
    m_oMatchedMuonTrackPlots = new Muon::TruthRelatedMuonPlotOrganizer(this,"matched/"+sTrackType,true,&selectedMatchedPlots);
  }
  
}

MuonTrackValidationPlots::~MuonTrackValidationPlots()
{
  //delete m_oRecoMuonTrackPlots;
  if (m_oMatchedMuonTrackPlots) delete m_oMatchedMuonTrackPlots;
}

void MuonTrackValidationPlots::fill(const xAOD::TrackParticle& muTP, float weight) {
  m_oRecoMuonTrackPlots->fill(muTP, weight);
}

void MuonTrackValidationPlots::fill(const xAOD::TruthParticle* truthMu, const xAOD::TrackParticle* muTP, float weight){
  if (muTP) {
    m_oRecoMuonTrackPlots->fill(*muTP, weight);
    if (truthMu) {
      m_oMatchedMuonTrackPlots->fill(*truthMu,*muTP, weight);
    }
  }
}
