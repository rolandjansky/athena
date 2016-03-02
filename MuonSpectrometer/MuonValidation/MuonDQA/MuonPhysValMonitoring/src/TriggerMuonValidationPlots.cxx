/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TriggerMuonValidationPlots.h"
#include "MuonHistUtils/MuonEnumDefs.h"
TriggerMuonValidationPlots::TriggerMuonValidationPlots(PlotBase* pParent, std::string sDir,std::vector<unsigned int> authors, bool isData, bool doTrigMuonL1Validation, bool doTrigMuonL2Validation, bool doTrigMuonEFValidation, std::vector<std::vector<std::string>> ChainSeed, std::vector<std::string> L1MuonItems):
  PlotBase(pParent, sDir),  m_selectedAuthors(authors), m_isData(isData), m_doTrigMuonL1Validation(doTrigMuonL1Validation), m_doTrigMuonL2Validation(doTrigMuonL2Validation), m_doTrigMuonEFValidation(doTrigMuonEFValidation), m_ChainSeed(ChainSeed), m_L1MuonItems(L1MuonItems)

{
  for (unsigned int i=0; i<m_ChainSeed.size(); i++){
    m_chains.push_back(m_ChainSeed[i][0]);
    m_seeds.push_back(m_ChainSeed[i][1]);
  }



  if (m_doTrigMuonL1Validation) m_oL1TriggerMuonPlots = new L1TriggerMuonPlots(this,"trigger/L1");
  if (m_doTrigMuonL2Validation) m_oL2TriggerMuonPlots.push_back(new HLTriggerMuonPlots(this,"trigger/L2/StandAlone"));
  if (m_doTrigMuonL2Validation) m_oL2TriggerMuonPlots.push_back(new HLTriggerMuonPlots(this,"trigger/L2/Combined"));

  //define a histogram class for each of the selected muon authors 
  for (unsigned int i=0; i<m_selectedAuthors.size(); i++) {
    std::string sAuthor = Muon::EnumDefs::toString( (xAOD::Muon::Author) m_selectedAuthors[i] );
    if (m_doTrigMuonEFValidation) m_oEFTriggerMuonPlots.push_back(new HLTriggerMuonPlots(this,"trigger/EF/"+sAuthor));
    if (m_doTrigMuonEFValidation) m_oEFTriggerMuonResolutionPlots.push_back(new Muon::ResoTriggerMuonPlots(this, "trigger/EF/"+sAuthor+"/Resolution/",""));
  }

  for (unsigned int i=0; i<m_L1MuonItems.size(); i++) {
    if (m_doTrigMuonEFValidation) m_oL1TriggerChainBarrelEfficiencyPlots.push_back(new TriggerEfficiencyPlots(this, "trigger/"+m_L1MuonItems[i]+"_Efficiency/BARREL/"));
    if (m_doTrigMuonEFValidation) m_oL1TriggerChainEndcapsEfficiencyPlots.push_back(new TriggerEfficiencyPlots(this, "trigger/"+m_L1MuonItems[i]+"_Efficiency/ENDCAPS/"));
    if (m_doTrigMuonEFValidation) m_oL1TriggerChainEfficiencyPlots.push_back(new TriggerEfficiencyPlots(this, "trigger/"+m_L1MuonItems[i]+"_Efficiency/WHOLE_DETECT/")); 
  }
  for (unsigned int i=0; i<m_chains.size(); i++) {
    if (m_doTrigMuonEFValidation) m_oEFTriggerChainBarrelEfficiencyPlots.push_back(new TriggerEfficiencyPlots(this, "trigger/"+m_chains[i]+"_Efficiency/BARREL/"));
    if (m_doTrigMuonEFValidation) m_oEFTriggerChainEndcapsEfficiencyPlots.push_back(new TriggerEfficiencyPlots(this, "trigger/"+m_chains[i]+"_Efficiency/ENDCAPS/"));
    if (m_doTrigMuonEFValidation) m_oEFTriggerChainEfficiencyPlots.push_back(new TriggerEfficiencyPlots(this, "trigger/"+m_chains[i]+"_Efficiency/WHOLE_DETECT/")); 
  }
  for (unsigned int i=0; i<m_chains.size(); i++) {
      if (m_doTrigMuonEFValidation) m_oEFTriggerChainBarrelRELEfficiencyPlots.push_back(new TriggerEfficiencyPlots(this, "trigger/"+m_chains[i]+"_wrt_"+m_seeds[i]+"_Efficiency/BARREL/"));
      if (m_doTrigMuonEFValidation) m_oEFTriggerChainEndcapsRELEfficiencyPlots.push_back(new TriggerEfficiencyPlots(this, "trigger/"+m_chains[i]+"_wrt_"+m_seeds[i]+"_Efficiency/ENDCAPS/"));
      if (m_doTrigMuonEFValidation) m_oEFTriggerChainRELEfficiencyPlots.push_back(new TriggerEfficiencyPlots(this, "trigger/"+m_chains[i]+"_wrt_"+m_seeds[i]+"_Efficiency/WHOLE_DETECT/")); 
      
  }
  PlateauTreshold=0.;
}

TriggerMuonValidationPlots::~TriggerMuonValidationPlots()
{ 
  if (m_doTrigMuonL2Validation) {
    for (unsigned int i=0; i<m_oL2TriggerMuonPlots.size(); i++) { 
       HLTriggerMuonPlots* L2TriggerMuonPlots = m_oL2TriggerMuonPlots[i];  
       delete L2TriggerMuonPlots;
       L2TriggerMuonPlots=0;
    }

  }

  if (m_doTrigMuonEFValidation) {
    for (unsigned int i=0; i<m_oEFTriggerMuonPlots.size(); i++) {  
      HLTriggerMuonPlots *trigMuonPlots = m_oEFTriggerMuonPlots[i];
      delete trigMuonPlots;
      trigMuonPlots=0;
    }
    for (unsigned int i=0; i<m_oEFTriggerMuonResolutionPlots.size(); i++) {  
      Muon::ResoTriggerMuonPlots *EFTriggerMuonResoMuonPlots = m_oEFTriggerMuonResolutionPlots[i];
      delete EFTriggerMuonResoMuonPlots;
      EFTriggerMuonResoMuonPlots=0;
    }
    for (unsigned int i=0; i<m_oL1TriggerChainEfficiencyPlots.size(); i++) {  
      TriggerEfficiencyPlots* L1TriggerChainEfficiencyPlots = m_oL1TriggerChainEfficiencyPlots[i];
      delete L1TriggerChainEfficiencyPlots;
      L1TriggerChainEfficiencyPlots=0;
    }
    for (unsigned int i=0; i<m_oL1TriggerChainBarrelEfficiencyPlots.size(); i++) {  
      TriggerEfficiencyPlots* L1TriggerChainEfficiencyPlots = m_oL1TriggerChainBarrelEfficiencyPlots[i];
      delete L1TriggerChainEfficiencyPlots;
      L1TriggerChainEfficiencyPlots=0;
    }
    for (unsigned int i=0; i<m_oL1TriggerChainEndcapsEfficiencyPlots.size(); i++) {  
      TriggerEfficiencyPlots* L1TriggerChainEfficiencyPlots = m_oL1TriggerChainEndcapsEfficiencyPlots[i];
      delete L1TriggerChainEfficiencyPlots;
      L1TriggerChainEfficiencyPlots=0;
    }
    for (unsigned int i=0; i<m_oEFTriggerChainEfficiencyPlots.size(); i++) {  
      TriggerEfficiencyPlots* EFTriggerChainEfficiencyPlots = m_oEFTriggerChainEfficiencyPlots[i];
      delete EFTriggerChainEfficiencyPlots;
      EFTriggerChainEfficiencyPlots=0;
    }
    for (unsigned int i=0; i<m_oEFTriggerChainBarrelEfficiencyPlots.size(); i++) {  
      TriggerEfficiencyPlots* EFTriggerChainEfficiencyPlots = m_oEFTriggerChainBarrelEfficiencyPlots[i];
      delete EFTriggerChainEfficiencyPlots;
      EFTriggerChainEfficiencyPlots=0;
    }
    for (unsigned int i=0; i<m_oEFTriggerChainEndcapsEfficiencyPlots.size(); i++) {  
      TriggerEfficiencyPlots* EFTriggerChainEfficiencyPlots = m_oEFTriggerChainEndcapsEfficiencyPlots[i];
      delete EFTriggerChainEfficiencyPlots;
      EFTriggerChainEfficiencyPlots=0;
    }
    for (unsigned int i=0; i<m_oEFTriggerChainRELEfficiencyPlots.size(); i++) {  
      TriggerEfficiencyPlots* EFTriggerChainRELEfficiencyPlots = m_oEFTriggerChainRELEfficiencyPlots[i];
      delete EFTriggerChainRELEfficiencyPlots;
      EFTriggerChainRELEfficiencyPlots=0;
    }
    for (unsigned int i=0; i<m_oEFTriggerChainBarrelRELEfficiencyPlots.size(); i++) {  
      TriggerEfficiencyPlots* EFTriggerChainRELEfficiencyPlots = m_oEFTriggerChainBarrelRELEfficiencyPlots[i];
      delete EFTriggerChainRELEfficiencyPlots;
      EFTriggerChainRELEfficiencyPlots=0;
    }
    for (unsigned int i=0; i<m_oEFTriggerChainEndcapsRELEfficiencyPlots.size(); i++) {  
      TriggerEfficiencyPlots* EFTriggerChainRELEfficiencyPlots = m_oEFTriggerChainEndcapsRELEfficiencyPlots[i];
      delete EFTriggerChainRELEfficiencyPlots;
      EFTriggerChainRELEfficiencyPlots=0;
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
      //std::cout<<m_selectedAuthors[i]<<"*** Reco to be matched :  pt " << Recomu.pt() <<std::endl;
      //std::cout<<m_selectedAuthors[i]<<"******************* EF :  pt " << Trigmu.pt() <<std::endl;
      //if (Trigmu.isAuthor( (xAOD::Muon::Author)m_selectedAuthors[i] ) || m_selectedAuthors[i]==xAOD::Muon::NumberOfMuonAuthors) {
      m_oEFTriggerMuonResolutionPlots[i]->fill(Trigmu, Recomu);
    }
  }
}

void TriggerMuonValidationPlots::fill(const xAOD::Muon& Trigmu) {
  fillTriggerMuonPlots(Trigmu);
}

void TriggerMuonValidationPlots::fill(const xAOD::Muon& Trigmu, const xAOD::Muon& Recomu) {
  //std::cout<<"*** Reco to be matched :  pt " << Recomu.pt() <<std::endl;
  //std::cout<<"******************* EF :  pt " << Trigmu.pt() <<std::endl;
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



void TriggerMuonValidationPlots::fillNumEff(const xAOD::Muon& mu, std::string selectedChain) {
  float PlateauTreshold = findTrigTreshold(selectedChain);
  PlateauTreshold =PlateauTreshold*1000.*1.05;
  for (unsigned int i=0; i<m_chains.size(); i++) {
    if (m_chains[i]==selectedChain) {
      m_oEFTriggerChainEfficiencyPlots[i]->fillNumerator(mu, PlateauTreshold);
      if( ((mu.eta())>-1.05) && ((mu.eta())<1.05) ) m_oEFTriggerChainBarrelEfficiencyPlots[i]->fillNumerator(mu, PlateauTreshold);
      if( ((mu.eta())<-1.05) || ((mu.eta())>1.05) ) m_oEFTriggerChainEndcapsEfficiencyPlots[i]->fillNumerator(mu, PlateauTreshold);
    }
  }
}

void TriggerMuonValidationPlots::fillDenEff(const xAOD::Muon& mu, std::string selectedChain) {
  float PlateauTreshold = findTrigTreshold(selectedChain);
  PlateauTreshold =PlateauTreshold*1000.*1.05;
  for (unsigned int i=0; i<m_chains.size(); i++) {
    if (m_chains[i]==selectedChain) {
      m_oEFTriggerChainEfficiencyPlots[i]->fillDenominator(mu, PlateauTreshold);
      if( ((mu.eta())>-1.05) && ((mu.eta())<1.05) ) m_oEFTriggerChainBarrelEfficiencyPlots[i]->fillDenominator(mu, PlateauTreshold);
      if( ((mu.eta())<-1.05) || ((mu.eta())>1.05) ) m_oEFTriggerChainEndcapsEfficiencyPlots[i]->fillDenominator(mu, PlateauTreshold);
    }
  }
}

void TriggerMuonValidationPlots::fillFeatPlots(const xAOD::Muon& mu, std::string selectedChain) {
  float PlateauTreshold = findTrigTreshold(selectedChain);
  PlateauTreshold =PlateauTreshold*1000.*1.05;
  for (unsigned int i=0; i<m_chains.size(); i++) {
    if (m_chains[i]==selectedChain) {
      m_oEFTriggerChainEfficiencyPlots[i]->fillFeatures(mu, PlateauTreshold);
      if( ((mu.eta())>-1.05) && ((mu.eta())<1.05) ) m_oEFTriggerChainBarrelEfficiencyPlots[i]->fillFeatures(mu, PlateauTreshold);
      if( ((mu.eta())<-1.05) || ((mu.eta())>1.05) ) m_oEFTriggerChainEndcapsEfficiencyPlots[i]->fillFeatures(mu, PlateauTreshold);
    }
  }
}

void TriggerMuonValidationPlots::fillNumL1Eff(const xAOD::Muon& mu, std::string selectedL1MuonItem) {
  float PlateauTreshold = findTrigTreshold(selectedL1MuonItem);
  PlateauTreshold =PlateauTreshold*1000.*1.05;
  for (unsigned int i=0; i<m_L1MuonItems.size(); i++) {
    if (m_L1MuonItems[i]==selectedL1MuonItem) {
      m_oL1TriggerChainEfficiencyPlots[i]->fillNumerator(mu, PlateauTreshold);
      if( ((mu.eta())>-1.05) && ((mu.eta())<1.05) ) m_oL1TriggerChainBarrelEfficiencyPlots[i]->fillNumerator(mu, PlateauTreshold);
      if( ((mu.eta())<-1.05) || ((mu.eta())>1.05) ) m_oL1TriggerChainEndcapsEfficiencyPlots[i]->fillNumerator(mu, PlateauTreshold);
    }
  }
}

void TriggerMuonValidationPlots::fillDenL1Eff(const xAOD::Muon& mu, std::string selectedL1MuonItem) {
  float PlateauTreshold = findTrigTreshold(selectedL1MuonItem);
  PlateauTreshold =PlateauTreshold*1000.*1.05;
  for (unsigned int i=0; i<m_L1MuonItems.size(); i++) {
    if (m_L1MuonItems[i]==selectedL1MuonItem) {
      m_oL1TriggerChainEfficiencyPlots[i]->fillDenominator(mu, PlateauTreshold);
      if( ((mu.eta())>-1.05) && ((mu.eta())<1.05) ) m_oL1TriggerChainBarrelEfficiencyPlots[i]->fillDenominator(mu, PlateauTreshold);
      if( ((mu.eta())<-1.05) || ((mu.eta())>1.05) ) m_oL1TriggerChainEndcapsEfficiencyPlots[i]->fillDenominator(mu, PlateauTreshold);
    }
  }
}
void TriggerMuonValidationPlots::fillFeatPlots(const xAOD::MuonRoI& TrigL1mu, std::string selectedL1MuonItem) {
  float PlateauTreshold = findTrigTreshold(selectedL1MuonItem);
  PlateauTreshold =PlateauTreshold*1000.*1.05;
  xAOD::MuonContainer* ROIinMuons = new xAOD::MuonContainer;
  xAOD::MuonAuxContainer* ROIinMuonsAux = new xAOD::MuonAuxContainer;
  ROIinMuons->setStore( ROIinMuonsAux );
  xAOD::Muon* myROI=new xAOD::Muon();
  ROIinMuons->push_back(myROI);
  myROI->setP4(TrigL1mu.thrValue(),TrigL1mu.eta(),TrigL1mu.phi());

  for (unsigned int i=0; i<m_L1MuonItems.size(); i++) {
    if (m_L1MuonItems[i]==selectedL1MuonItem) {
      m_oL1TriggerChainEfficiencyPlots[i]->fillFeatures(*myROI, PlateauTreshold);
      if( ((TrigL1mu.eta())>-1.05) && ((TrigL1mu.eta())<1.05) ) m_oL1TriggerChainBarrelEfficiencyPlots[i]->fillFeatures(*myROI, PlateauTreshold);
      if( ((TrigL1mu.eta())<-1.05) || ((TrigL1mu.eta())>1.05) ) m_oL1TriggerChainEndcapsEfficiencyPlots[i]->fillFeatures(*myROI, PlateauTreshold);
    }
  }
  delete ROIinMuons;
  delete ROIinMuonsAux;
}

void TriggerMuonValidationPlots::fillDenRELEff(const xAOD::Muon& mu, std::string selectedChain ) {
  float PlateauTreshold = findTrigTreshold(selectedChain);
  PlateauTreshold =PlateauTreshold*1000.*1.05;
  for (unsigned int i=0; i<m_chains.size(); i++) {
      if(m_chains[i]==selectedChain) {
        m_oEFTriggerChainRELEfficiencyPlots[i]->fillDenominator(mu, PlateauTreshold);
        if( ((mu.eta())>-1.05) && ((mu.eta())<1.05) ) m_oEFTriggerChainBarrelRELEfficiencyPlots[i]->fillDenominator(mu, PlateauTreshold);
        if( ((mu.eta())<-1.05) || ((mu.eta())>1.05) ) m_oEFTriggerChainEndcapsRELEfficiencyPlots[i]->fillDenominator(mu, PlateauTreshold);    
      }
  }
}

void TriggerMuonValidationPlots::fillNumRELEff(const xAOD::Muon& mu, std::string selectedChain ) {
  float PlateauTreshold = findTrigTreshold(selectedChain);
  PlateauTreshold =PlateauTreshold*1000.*1.05;
  for (unsigned int i=0; i<m_chains.size(); i++) {
      if(m_chains[i]==selectedChain) {
        m_oEFTriggerChainRELEfficiencyPlots[i]->fillNumerator(mu, PlateauTreshold);
        if( ((mu.eta())>-1.05) && ((mu.eta())<1.05) ) m_oEFTriggerChainBarrelRELEfficiencyPlots[i]->fillNumerator(mu, PlateauTreshold);
        if( ((mu.eta())<-1.05) || ((mu.eta())>1.05) ) m_oEFTriggerChainEndcapsRELEfficiencyPlots[i]->fillNumerator(mu, PlateauTreshold);          
      }
  }
}

float TriggerMuonValidationPlots::findTrigTreshold(TString chain){
  float PlateauTreshold;
  std::string str(chain);
  TString temp;
  if (str.find("HLT_") != std::string::npos){
    for (unsigned int l=0; l < str.size(); l++){
      if (isdigit(str[l])){
        for (unsigned int a=l; a<str.size(); a++){
          //if (str[a]=='_') break;
          if ((isdigit(str[a]))==0) break;
          temp += str[a];  
        }
        break;
      }
    }
  }
  if (str.find("L1_MU") != std::string::npos){
    for (unsigned int l=0; l < str.size(); l++){
      if ((str[l])=='U'){
        for (unsigned int a=l+1; a<str.size(); a++){
          //if (str[a]=='_') break;
          temp += str[a];  
        }
        break;
      }
    }
  }
  PlateauTreshold = temp.Atof();
  return PlateauTreshold;
}

