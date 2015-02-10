/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonValidationPlots.h"
#include "MuonHistUtils/MuonEnumDefs.h"

typedef ElementLink< xAOD::TrackParticleContainer > TrackLink;
typedef ElementLink< xAOD::MuonContainer > MuonLink;

MuonValidationPlots::MuonValidationPlots(PlotBase* pParent, std::string sDir,std::vector<unsigned int> authors, bool isData, bool doBinnedResolutionPlots, bool doTrigMuonL1Validation, bool doTrigMuonL2Validation, bool doTrigMuonEFValidation):
  PlotBase(pParent, sDir),  m_selectedAuthors(authors), m_truthSelections(2,""), m_isData(isData), m_doTrigMuonL1Validation(doTrigMuonL1Validation), m_doTrigMuonL2Validation(doTrigMuonL2Validation), m_doTrigMuonEFValidation(doTrigMuonEFValidation)

{
  if (!m_isData) {
    m_truthSelections[0] = "all"; //no selection on truth muons (minimum selection is |eta|<2.5, pt>5 GeV, defined in MuonPhysValMonitoringTool::handleTruthMuon() 
    m_truthSelections[1] = "MSAcceptance"; //truth muons in MS acceptance (at least 4 associated hits in the MS)

    //histogram classes for all muons
    for(const auto truthSelection : m_truthSelections) {
      m_oTruthMuonPlots.push_back(new TruthMuonPlots(this,"truth/"+truthSelection));
    }
    m_oTruthRelatedMuonPlots = new TruthRelatedMuonPlots(this, "matched/AllMuons", doBinnedResolutionPlots);
  }
  //histogram classes for all muons
  m_oRecoMuonPlots = new RecoMuonPlots(this, "reco/AllMuons");
  
  //define a histogram class for each of the selected muon qualities
  for(unsigned int i=0; i<Muon::EnumDefs::nMuonQualities(); i++) {
    std::string sQuality = Muon::EnumDefs::toString( (xAOD::Muon::Quality) i);
    m_oRecoMuonPlots_perQuality.push_back(new RecoMuonPlots(this, "reco/"+sQuality));
    if (!m_isData) m_oTruthRelatedMuonPlots_perQuality.push_back(new TruthRelatedMuonPlots(this, "matched/"+sQuality, false)); //disable binned resolution plots for qualities 
    //m_oTruthRelatedMuonPlots_perQuality.push_back(new TruthRelatedMuonPlots(this, "matched/"+sQuality, doBinnedResolutionPlots));    
  }

  //define a histogram class for each of the selected muon authors (+one inclusive for all authors
  for (unsigned int i=0; i<m_selectedAuthors.size(); i++) {
    std::string sAuthor = Muon::EnumDefs::toString( (xAOD::Muon::Author) m_selectedAuthors[i] );
    m_oRecoMuonPlots_perAuthor.push_back(new RecoMuonPlots(this, "reco/"+sAuthor));
    if (!m_isData) m_oTruthRelatedMuonPlots_perAuthor.push_back(new TruthRelatedMuonPlots(this, "matched/"+sAuthor, doBinnedResolutionPlots));
    if (m_doTrigMuonEFValidation) m_oEFTriggerMuonPlots.push_back(new EFTriggerMuonPlots(this,"trigger/EF/"+sAuthor));
  }
  if (m_doTrigMuonL1Validation) m_oL1TriggerMuonPlots = new L1TriggerMuonPlots(this,"trigger/L1");
}

MuonValidationPlots::~MuonValidationPlots()
{
  delete m_oRecoMuonPlots;
  m_oRecoMuonPlots=0;

  if (!m_isData) {
    delete m_oTruthRelatedMuonPlots;
    m_oTruthRelatedMuonPlots=0;    

    for (unsigned int i=0; i<m_oTruthMuonPlots.size(); i++) {    
      TruthMuonPlots *truthMuonPlots = m_oTruthMuonPlots[i];
      delete truthMuonPlots;
      truthMuonPlots = 0;
    }
    for (unsigned int i=0; i<m_oTruthRelatedMuonPlots_perQuality.size(); i++) {    
      TruthRelatedMuonPlots *truthRelatedMuonPlots = m_oTruthRelatedMuonPlots_perQuality[i];
      delete truthRelatedMuonPlots;
      truthRelatedMuonPlots = 0;
    }
    for (unsigned int i=0; i<m_oTruthRelatedMuonPlots_perAuthor.size(); i++) {    
      TruthRelatedMuonPlots *truthRelatedMuonPlots = m_oTruthRelatedMuonPlots_perAuthor[i];
      delete truthRelatedMuonPlots;
      truthRelatedMuonPlots = 0;
    }
  }
  
  if (m_doTrigMuonL1Validation) {
    delete m_oL1TriggerMuonPlots;
    m_oL1TriggerMuonPlots=0;
  }
  
  for (unsigned int i=0; i<m_oRecoMuonPlots_perQuality.size(); i++) {    
    RecoMuonPlots *recoMuonPlots = m_oRecoMuonPlots_perQuality[i];    
    delete recoMuonPlots;
    recoMuonPlots = 0;
  }
  for (unsigned int i=0; i<m_oRecoMuonPlots_perAuthor.size(); i++) {    
    RecoMuonPlots *recoMuonPlots = m_oRecoMuonPlots_perAuthor[i];
    delete recoMuonPlots;
    recoMuonPlots = 0;

    if (m_doTrigMuonEFValidation) {
      EFTriggerMuonPlots *trigMuonPlots = m_oEFTriggerMuonPlots[i];
      delete trigMuonPlots;
      trigMuonPlots=0;
    }
  }        
}

void MuonValidationPlots::fillRecoMuonPlots(const xAOD::Muon& mu)
{
  //fill hists for all muons
  m_oRecoMuonPlots->fill(mu);
	
  //fill separate hists for each muon quality
  xAOD::Muon::Quality muqual = mu.quality();
  for (unsigned int i=0; i<Muon::EnumDefs::nMuonQualities(); i++) {    
    if ( muqual <= (xAOD::Muon::Quality)i ) {
      m_oRecoMuonPlots_perQuality[i]->fill(mu);
    }
  }
  //fill separate hists for each author
  for (unsigned int i=0; i<m_selectedAuthors.size(); i++) {
    if (mu.isAuthor( (xAOD::Muon::Author)m_selectedAuthors[i] )) {
      m_oRecoMuonPlots_perAuthor[i]->fill(mu);
    }
  }
}

void MuonValidationPlots::fillTruthMuonPlots(const xAOD::TruthParticle &truthMu)
{
  m_oTruthMuonPlots[0]->fill(truthMu); //no selections
 
  if (isGoodTruthTrack(truthMu)) { //in MS acceptance (minimum precision hits)
    m_oTruthMuonPlots[1]->fill(truthMu);
  }

}

void MuonValidationPlots::fillTriggerMuonPlots(const xAOD::MuonRoI &TrigL1mu) {
  m_oL1TriggerMuonPlots->fill(TrigL1mu);  
}

void MuonValidationPlots::fillTriggerMuonPlots(const xAOD::Muon &mu,const xAOD::Muon &Trigmu) {
  for (unsigned int i=0; i<m_selectedAuthors.size(); i++) {
    if (mu.isAuthor( (xAOD::Muon::Author)m_selectedAuthors[i] )) {
      //if (Trigmu.isAuthor( (xAOD::Muon::Author)m_selectedAuthors[i] ) || m_selectedAuthors[i]==xAOD::Muon::NumberOfMuonAuthors) {
      m_oEFTriggerMuonPlots[i]->fill(Trigmu,Trigmu);
    }
  }
}

void MuonValidationPlots::fill(const xAOD::Muon& /* mu */,const xAOD::Muon& Trigmu) {
  fillTriggerMuonPlots(Trigmu,Trigmu);
}

void MuonValidationPlots::fill(const xAOD::MuonRoI& TrigL1mu) {
  fillTriggerMuonPlots(TrigL1mu);
}


void MuonValidationPlots::fill(const xAOD::Muon& mu) {
  fillRecoMuonPlots(mu);
}

void MuonValidationPlots::fill(const xAOD::TruthParticle &truthMu){
  fillTruthMuonPlots(truthMu);
}

void MuonValidationPlots::fill(const xAOD::TruthParticle* truthMu, const xAOD::Muon* mu, const xAOD::TrackParticleContainer* MSTracks)
{
  if (truthMu) fillTruthMuonPlots(*truthMu);
  if (mu) fillRecoMuonPlots(*mu);

  if ( (mu) && (truthMu) ) {
    //plots for all
    m_oTruthRelatedMuonPlots->fill(*truthMu, *mu, MSTracks);
    
    //plots per quality
    xAOD::Muon::Quality muqual = mu->quality();
    for (unsigned int i=0; i<Muon::EnumDefs::nMuonQualities(); i++) {    
      if ( muqual <= (xAOD::Muon::Quality)i ) {
	m_oTruthRelatedMuonPlots_perQuality[i]->fill(*truthMu, *mu, MSTracks);
      }
    }
    //plots per author
    for (unsigned int i=0; i<m_selectedAuthors.size(); i++) {
      if (mu->isAuthor( (xAOD::Muon::Author)m_selectedAuthors[i] )) {
	m_oTruthRelatedMuonPlots_perAuthor[i]->fill(*truthMu, *mu, MSTracks);
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
