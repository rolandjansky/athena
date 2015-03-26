/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TriggerMuonValidationPlots.h"
#include "MuonHistUtils/MuonEnumDefs.h"

TriggerMuonValidationPlots::TriggerMuonValidationPlots(PlotBase* pParent, std::string sDir,std::vector<unsigned int> authors, bool isData, bool doBinnedResolutionPlots, bool doTrigMuonL1Validation, bool doTrigMuonL2Validation, bool doTrigMuonEFValidation):
  PlotBase(pParent, sDir),  m_selectedAuthors(authors), m_oL1TriggerMuonPlots(NULL), m_oL1TriggerMuonResolutionPlots(NULL), m_isData(isData), m_doTrigMuonL1Validation(doTrigMuonL1Validation), m_doTrigMuonL2Validation(doTrigMuonL2Validation), m_doTrigMuonEFValidation(doTrigMuonEFValidation)

{
  if (m_doTrigMuonL1Validation) m_oL1TriggerMuonPlots = new L1TriggerMuonPlots(this,"trigger/L1");
  if (m_doTrigMuonL1Validation) m_oL1TriggerMuonResolutionPlots = new RecoRelatedTriggerMuonPlots(this, "trigger/L1", doBinnedResolutionPlots);
  if (m_doTrigMuonL2Validation) m_oL2TriggerMuonPlots.push_back(new HLTriggerMuonPlots(this,"trigger/L2/StandAlone"));
  if (m_doTrigMuonL2Validation) m_oL2TriggerMuonPlots.push_back(new HLTriggerMuonPlots(this,"trigger/L2/Combined"));
  if (m_doTrigMuonL2Validation) m_oL2TriggerMuonResolutionPlots.push_back(new RecoRelatedTriggerMuonPlots(this, "trigger/L2/StandAlone", doBinnedResolutionPlots));
  if (m_doTrigMuonL2Validation) m_oL2TriggerMuonResolutionPlots.push_back(new RecoRelatedTriggerMuonPlots(this, "trigger/L2/Combined", doBinnedResolutionPlots));

  //define a histogram class for each of the selected muon authors 
  for (unsigned int i=0; i<m_selectedAuthors.size(); i++) {
    std::string sAuthor = Muon::EnumDefs::toString( (xAOD::Muon::Author) m_selectedAuthors[i] );
    if (m_doTrigMuonEFValidation) m_oEFTriggerMuonPlots.push_back(new HLTriggerMuonPlots(this,"trigger/EF/"+sAuthor));
    if (m_doTrigMuonEFValidation) m_oEFTriggerMuonResolutionPlots.push_back(new RecoRelatedTriggerMuonPlots(this, "trigger/EF/"+sAuthor, doBinnedResolutionPlots));
  }
}

TriggerMuonValidationPlots::~TriggerMuonValidationPlots()
{
  if (m_doTrigMuonL1Validation) {
    delete m_oL1TriggerMuonPlots;
    m_oL1TriggerMuonPlots=0;
  }
  
  if (m_doTrigMuonL2Validation) {
    for (unsigned int i=0; i<m_oL2TriggerMuonPlots.size(); i++) { 
       HLTriggerMuonPlots* L2TriggerMuonPlots = m_oL2TriggerMuonPlots[i];  
       delete L2TriggerMuonPlots;
       L2TriggerMuonPlots=0;
    }
  }

  if (m_doTrigMuonEFValidation) {
    for (unsigned int i=0; i<m_selectedAuthors.size(); i++) {  
      HLTriggerMuonPlots *trigMuonPlots = m_oEFTriggerMuonPlots[i];
      delete trigMuonPlots;
      trigMuonPlots=0;
    }
  }        
}

void TriggerMuonValidationPlots::fillTriggerMuonPlots(const xAOD::MuonRoI &TrigL1mu) {
  m_oL1TriggerMuonPlots->fill(TrigL1mu);  
}

void TriggerMuonValidationPlots::fillTriggerMuonPlots(const xAOD::L2StandAloneMuon &L2SAmu) {
  m_oL2TriggerMuonPlots[0]->fill(L2SAmu);  

}

void TriggerMuonValidationPlots::fillTriggerMuonPlots(const xAOD::L2CombinedMuon &L2CBmu) {
  m_oL2TriggerMuonPlots[1]->fill(L2CBmu);  

}

void TriggerMuonValidationPlots::fillTriggerMuonPlots(const xAOD::Muon &Trigmu) {
  for (unsigned int i=0; i<m_selectedAuthors.size(); i++) {
    if (Trigmu.isAuthor( (xAOD::Muon::Author)m_selectedAuthors[i] )) {
      //if (Trigmu.isAuthor( (xAOD::Muon::Author)m_selectedAuthors[i] ) || m_selectedAuthors[i]==xAOD::Muon::NumberOfMuonAuthors) {
      m_oEFTriggerMuonPlots[i]->fill(Trigmu);
    }
  }
}

void TriggerMuonValidationPlots::fillTriggerMuonPlots(const xAOD::Muon &Trigmu, const xAOD::Muon &Recomu) {
  for (unsigned int i=0; i<m_selectedAuthors.size(); i++) {
    if (Trigmu.isAuthor( (xAOD::Muon::Author)m_selectedAuthors[i] )) {
      //if (Trigmu.isAuthor( (xAOD::Muon::Author)m_selectedAuthors[i] ) || m_selectedAuthors[i]==xAOD::Muon::NumberOfMuonAuthors) {
      m_oEFTriggerMuonResolutionPlots[i]->fill(Trigmu, Recomu);
    }
  }
}

void TriggerMuonValidationPlots::fill(const xAOD::Muon& Trigmu) {
  fillTriggerMuonPlots(Trigmu);
}

void TriggerMuonValidationPlots::fill(const xAOD::Muon& Trigmu, const xAOD::Muon& Recomu) {
  fillTriggerMuonPlots(Trigmu, Recomu);
}

void TriggerMuonValidationPlots::fill(const xAOD::MuonRoI& TrigL1mu) {
  fillTriggerMuonPlots(TrigL1mu);
}


void TriggerMuonValidationPlots::fill(const xAOD::L2StandAloneMuon& L2SAmu) {
  fillTriggerMuonPlots(L2SAmu);
}

void TriggerMuonValidationPlots::fill(const xAOD::L2CombinedMuon& L2CBmu) {
  fillTriggerMuonPlots(L2CBmu);
}


