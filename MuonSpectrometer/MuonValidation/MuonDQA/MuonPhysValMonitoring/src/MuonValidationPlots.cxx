/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonValidationPlots.h"

typedef ElementLink< xAOD::TrackParticleContainer > TrackLink;
typedef ElementLink< xAOD::MuonContainer > MuonLink;

MuonValidationPlots::MuonValidationPlots(PlotBase* pParent, std::string sDir,std::vector<unsigned int> authors, bool doBinnedResolutionPlots):
  PlotBase(pParent, sDir),
  m_authorNames(xAOD::Muon::NumberOfMuonAuthors+1,""),
  m_truthSelections(2,"")
{
  m_truthSelections[0] = "all"; //no selection on truth muons (minimum selection is |eta|<2.5, pt>5 GeV, defined in MuonPhysValMonitoringTool::handleTruthMuon() 
  m_truthSelections[1] = "MSAcceptance"; //truth muons in MS acceptance (at least 4 associated hits in the MS)

  for(const auto truthSelection : m_truthSelections) {
    m_oTruthMuonPlots.push_back(new TruthMuonPlots(this,"truth/"+truthSelection));
  }

  //list of author names taken from: Event/xAOD/xAODMuon/xAODMuon/versions/Muon_v1.h
  //19.0.3
  m_authorNames[xAOD::Muon::unknown]="unknown";      // 0
  m_authorNames[xAOD::Muon::MuidCo]="MuidCombined";
  m_authorNames[xAOD::Muon::STACO]="STACO";
  m_authorNames[xAOD::Muon::MuTag]="MuTag";
  m_authorNames[xAOD::Muon::MuTagIMO]="MuTagIMO";
  m_authorNames[xAOD::Muon::MuidSA]="MuidStandalone";
  m_authorNames[xAOD::Muon::MuGirl]="MuGirl";
  m_authorNames[xAOD::Muon::MuGirlLowBeta]="MuGirlLowBeta";  // 15
  m_authorNames[xAOD::Muon::CaloTag]="CaloTag";
  m_authorNames[xAOD::Muon::CaloLikelihood]="CaloLikelihood";
  m_authorNames[xAOD::Muon::ExtrapolateMuonToIP]="ExtrapolateMuonToIP"; // 20
   
  m_authorNames[xAOD::Muon::NumberOfMuonAuthors]="AllAuthors"; //inclusive


  //19.0.2
  // m_authorNames[0]="undefined";      // 0
  // m_authorNames[1]="unknown";
  // m_authorNames[2]="unknown";
  // m_authorNames[3]="unknown";
  // m_authorNames[4]="MuonboySP";
  // m_authorNames[5]="Muonboy";        // 5
  // m_authorNames[6]="STACO";
  // m_authorNames[7]="MuTag";
  // m_authorNames[8]="unknown";
  // m_authorNames[9]="unknown";
  // m_authorNames[10]="Moore";          // 10
  // m_authorNames[11]="MuidStandalone";
  // m_authorNames[12]="MuidCombined";
  // m_authorNames[13]="MuGirl";
  // m_authorNames[14]="CaloMuonID";
  // m_authorNames[15]="MuGirlLowBeta";  // 15
  // m_authorNames[16]="CaloTag";
  // m_authorNames[17]="CaloLikelihood";
  // m_authorNames[18]="MuTagIMO";
  // m_authorNames[19]="MuonCombinedRefit";
  // m_authorNames[20]="ExtrapolateMuonToIP"; // 20

  // m_authorNames[21]="AllAuthors"; //inclusive

  //define a histogram class for each of the selected muon authors (+one inclusive for all authors
  m_selectedAuthors = authors;
  m_selectedAuthors.push_back(xAOD::Muon::NumberOfMuonAuthors);
  for(const auto author : m_selectedAuthors) {   
    m_oRecoMuonPlots.push_back(new RecoMuonPlots(this, "reco/"+m_authorNames[author]));
    m_oTruthRelatedMuonPlots.push_back(new TruthRelatedMuonPlots(this, "matched/"+m_authorNames[author], doBinnedResolutionPlots));
  }

}

MuonValidationPlots::~MuonValidationPlots()
{
  for (unsigned int i=0; i<m_oRecoMuonPlots.size(); i++) {    
    RecoMuonPlots *recoMuonPlots = m_oRecoMuonPlots[i];
    TruthRelatedMuonPlots *truthRelatedMuonPlots = m_oTruthRelatedMuonPlots[i];
    
    delete recoMuonPlots;
    delete truthRelatedMuonPlots;

    recoMuonPlots = 0;
    truthRelatedMuonPlots = 0;
  }

  for (unsigned int i=0; i<m_oTruthMuonPlots.size(); i++) {    
    TruthMuonPlots *truthMuonPlots = m_oTruthMuonPlots[i];
    delete truthMuonPlots;
    truthMuonPlots = 0;
  }
}

void MuonValidationPlots::fillRecoMuonPlots(const xAOD::Muon& mu) {
  //fill separate hists for each author
  for (unsigned int i=0; i<m_selectedAuthors.size(); i++) {
    if (mu.isAuthor( (xAOD::Muon::Author)m_selectedAuthors[i] ) || m_selectedAuthors[i]==xAOD::Muon::NumberOfMuonAuthors) {
      m_oRecoMuonPlots[i]->fill(mu);
    }
  }
}
void MuonValidationPlots::fillTruthMuonPlots(const xAOD::TruthParticle &truthMu) {

  m_oTruthMuonPlots[0]->fill(truthMu); //no selections
 
  if (isGoodTruthTrack(truthMu)) { //in MS acceptance (minimum precision hits)
    m_oTruthMuonPlots[1]->fill(truthMu);
  }

}

void MuonValidationPlots::fill(const xAOD::Muon& mu) {
  fillRecoMuonPlots(mu);
}

void MuonValidationPlots::fill(const xAOD::TruthParticle &truthMu){
  fillTruthMuonPlots(truthMu);
}

void MuonValidationPlots::fill(const xAOD::TruthParticle* truthMu, const xAOD::Muon* mu){

  if (truthMu) fillTruthMuonPlots(*truthMu);
  if (mu) fillRecoMuonPlots(*mu);

  if ( (mu) && (truthMu) ) {
    for (unsigned int i=0; i<m_selectedAuthors.size(); i++) {
      if (mu->isAuthor( (xAOD::Muon::Author)m_selectedAuthors[i] ) || m_selectedAuthors[i]==xAOD::Muon::NumberOfMuonAuthors) {
	m_oTruthRelatedMuonPlots[i]->fill(*truthMu, *mu);
      }
    }
  }

}

bool MuonValidationPlots::isGoodTruthTrack(const xAOD::TruthParticle& truthMu) {
  std::string hitTypes[6]={"innerSmallHits","innerLargeHits","middleSmallHits","middleLargeHits","outerSmallHits","outerLargeHits"}; //MDT + CSC
  int minPrecHits=5;

  int nPrecHits=0;
  bool hasEnoughPrecHits=false;

  for (const auto hitTypeItr : hitTypes) {
    if (truthMu.isAvailable<uint8_t>(hitTypeItr)) {
      nPrecHits+=truthMu.auxdata<uint8_t>(hitTypeItr);
      if (nPrecHits>=minPrecHits) {
	hasEnoughPrecHits=true;
	break;
      }
    }
  }
  return (hasEnoughPrecHits);
}
