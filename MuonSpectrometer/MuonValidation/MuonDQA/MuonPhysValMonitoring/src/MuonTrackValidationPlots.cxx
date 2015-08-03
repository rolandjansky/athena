/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTrackValidationPlots.h"

MuonTrackValidationPlots::MuonTrackValidationPlots(PlotBase* pParent, std::string sDir, std::string sTrackType, bool isData):
  PlotBase(pParent, sDir)
  , m_oRecoMuonTrackPlots(NULL)
  , m_oIDHitPlots(NULL)
  , m_oMSHitPlots(NULL)
  , m_oMatchedMuonTrackPlots(NULL)
  , m_sTrackType(sTrackType)
{
  m_oRecoMuonTrackPlots = new Muon::RecoMuonTrackPlotOrganizer(this, "reco/"+sTrackType);
  if (!isData) m_oMatchedMuonTrackPlots = new Muon::TruthRelatedMuonPlotOrganizer(this,"matched/"+sTrackType,true);

  if (sTrackType.find("ID")!= std::string::npos) {
    m_oIDHitPlots = new Muon::IDHitSummaryPlots(this,"reco/"+sTrackType+"/hits/");    
  } else {
    m_oMSHitPlots = new Trk::MSHitPlots(this,"reco/"+sTrackType+"/hits/");
  }
}

MuonTrackValidationPlots::~MuonTrackValidationPlots()
{
  //delete m_oRecoMuonTrackPlots;
  if (m_oMatchedMuonTrackPlots) delete m_oMatchedMuonTrackPlots;
  if (m_oIDHitPlots) delete m_oIDHitPlots;
  else if (m_oMSHitPlots) delete m_oMSHitPlots;
}

void MuonTrackValidationPlots::fill(const xAOD::TrackParticle& muTP) {
  m_oRecoMuonTrackPlots->fill(muTP);
  if (m_oIDHitPlots) m_oIDHitPlots->fill(muTP);
  else if (m_oMSHitPlots) m_oMSHitPlots->fill(muTP);
}

void MuonTrackValidationPlots::fill(const xAOD::TruthParticle* truthMu, const xAOD::TrackParticle* muTP){
  if (muTP) {
    m_oRecoMuonTrackPlots->fill(*muTP);
    if (m_oIDHitPlots) m_oIDHitPlots->fill(*muTP);
    else if (m_oMSHitPlots) m_oMSHitPlots->fill(*muTP);

    if (truthMu) {
      m_oMatchedMuonTrackPlots->fill(*truthMu,*muTP);
    }
  }
 
}
