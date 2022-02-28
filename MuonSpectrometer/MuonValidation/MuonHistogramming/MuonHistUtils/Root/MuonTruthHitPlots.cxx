/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonHistUtils/MuonTruthHitPlots.h"

namespace Muon{
  
MuonTruthHitPlots::MuonTruthHitPlots(PlotBase* pParent, const std::string& sDir):
PlotBase(pParent, sDir)

{
  precMatchedHitsSummed=Book1D("precMatchedHitsSummed","summed precision hits matched to truth (all chambers)",21,-0.5,20.5);
  precMatchedHitsBIS=Book1D("precMatchedHitsBIS","BIS precision hits matched to truth;matched hits;entries",21,-0.5,20.5);
  precMatchedHitsBIL=Book1D("precMatchedHitsBIL","BIL precision hits matched to truth;matched hits;entries",21,-0.5,20.5);
  precMatchedHitsBMS=Book1D("precMatchedHitsBMS","BMS precision hits matched to truth;matched hits;entries",21,-0.5,20.5);
  precMatchedHitsBML=Book1D("precMatchedHitsBML","BML precision hits matched to truth;matched hits;entries",21,-0.5,20.5);
  precMatchedHitsBOS=Book1D("precMatchedHitsBOS","BOS precision hits matched to truth;matched hits;entries",21,-0.5,20.5);
  precMatchedHitsBOL=Book1D("precMatchedHitsBOL","BOL precision hits matched to truth;matched hits;entries",21,-0.5,20.5);
  precMatchedHitsBEE=Book1D("precMatchedHitsBEE","BEE precision hits matched to truth;matched hits;entries",21,-0.5,20.5);
  precMatchedHitsEIS=Book1D("precMatchedHitsEIS","EIS precision hits matched to truth;matched hits;entries",21,-0.5,20.5);
  precMatchedHitsEIL=Book1D("precMatchedHitsEIL","EIL precision hits matched to truth;matched hits;entries",21,-0.5,20.5);
  precMatchedHitsEMS=Book1D("precMatchedHitsEMS","EMS precision hits matched to truth;matched hits;entries",21,-0.5,20.5);
  precMatchedHitsEML=Book1D("precMatchedHitsEML","EML precision hits matched to truth;matched hits;entries",21,-0.5,20.5);
  precMatchedHitsEOS=Book1D("precMatchedHitsEOS","EOS precision hits matched to truth;matched hits;entries",21,-0.5,20.5);
  precMatchedHitsEOL=Book1D("precMatchedHitsEOL","EOL precision hits matched to truth;matched hits;entries",21,-0.5,20.5);
  precMatchedHitsEES=Book1D("precMatchedHitsEES","EES precision hits matched to truth;matched hits;entries",21,-0.5,20.5);
  precMatchedHitsEEL=Book1D("precMatchedHitsEEL","EEL precision hits matched to truth;matched hits;entries",21,-0.5,20.5);
  precMatchedHitsCSS=Book1D("precMatchedHitsCSS","CSS precision hits matched to truth;matched hits;entries",21,-0.5,20.5);
  precMatchedHitsCSL=Book1D("precMatchedHitsCSL","CSL precision hits matched to truth;matched hits;entries",21,-0.5,20.5);

  phiMatchedHitsSummed=Book1D("phiMatchedHitsSummed","summed phi hits matched to truth (all chambers)",21,-0.5,20.5 );
  phiMatchedHitsBM1=Book1D("phiMatchedHitsBM1","BM1 phi hits matched to truth;matched hits;entries",21,-0.5,20.5 );
  phiMatchedHitsBM2=Book1D("phiMatchedHitsBM2","BM2 phi hits matched to truth;matched hits;entries",21,-0.5,20.5 );
  phiMatchedHitsBO1=Book1D("phiMatchedHitsBO1","BO1 phi hits matched to truth;matched hits;entries",21,-0.5,20.5 );
  phiMatchedHitsBO2=Book1D("phiMatchedHitsBO2","BO2 phi hits matched to truth;matched hits;entries",21,-0.5,20.5 );
  phiMatchedHitsT1=Book1D("phiMatchedHitsT1","T1 phi hits matched to truth;matched hits;entries",21,-0.5,20.5 );
  phiMatchedHitsT2=Book1D("phiMatchedHitsT2","T2 phi hits matched to truth;matched hits;entries",21,-0.5,20.5 );
  phiMatchedHitsT3=Book1D("phiMatchedHitsT3","T3 phi hits matched to truth;matched hits;entries",21,-0.5,20.5 );
  phiMatchedHitsT4=Book1D("phiMatchedHitsT4","T4 phi hits matched to truth;matched hits;entries",21,-0.5,20.5 );
  phiMatchedHitsCSC=Book1D("phiMatchedHitsCSC","CSC phi hits matched to truth;matched hits;entries",21,-0.5,20.5 );
  
  trigEtaMatchedHitsSummed=Book1D("trigEtaMatchedHitsSummed","summed trigEta hits matched to truth (all chambers)",21,-0.5,20.5 );
  trigEtaMatchedHitsBM1=Book1D("trigEtaMatchedHitsBM1","BM1 trigEta hits matched to truth;matched hits;entries",21,-0.5,20.5 );
  trigEtaMatchedHitsBM2=Book1D("trigEtaMatchedHitsBM2","BM2 trigEta hits matched to truth;matched hits;entries",21,-0.5,20.5 );
  trigEtaMatchedHitsBO1=Book1D("trigEtaMatchedHitsBO1","BO1 trigEta hits matched to truth;matched hits;entries",21,-0.5,20.5 );
  trigEtaMatchedHitsBO2=Book1D("trigEtaMatchedHitsBO2","BO2 trigEta hits matched to truth;matched hits;entries",21,-0.5,20.5 );
  trigEtaMatchedHitsT1=Book1D("trigEtaMatchedHitsT1","T1 trigEta hits matched to truth;matched hits;entries",21,-0.5,20.5 );
  trigEtaMatchedHitsT2=Book1D("trigEtaMatchedHitsT2","T2 trigEta hits matched to truth;matched hits;entries",21,-0.5,20.5 );
  trigEtaMatchedHitsT3=Book1D("trigEtaMatchedHitsT3","T3 trigEta hits matched to truth;matched hits;entries",21,-0.5,20.5 );
  trigEtaMatchedHitsT4=Book1D("trigEtaMatchedHitsT4","T4 trigEta hits matched to truth;matched hits;entries",21,-0.5,20.5 );
  // trigEtaMatchedHitsCSC=Book1D("trigEtaMatchedHitsCSC","CSC trigEta hits matched to truth;matched hits;entries",21,-0.5,20.5 );
 
 }	
 
  void MuonTruthHitPlots::fill(const xAOD::Muon& muon, float weight){

    fillPlot(precMatchedHitsSummed,"nprecMatchedHitsPerChamberLayer", muon, weight);
    fillPlot(precMatchedHitsBIS,"nprecMatchedHitsPerChamberLayer", Muon::MuonStationIndex::BIS, muon, weight);
    fillPlot(precMatchedHitsBIL,"nprecMatchedHitsPerChamberLayer", Muon::MuonStationIndex::BIL, muon, weight);
    fillPlot(precMatchedHitsBMS,"nprecMatchedHitsPerChamberLayer", Muon::MuonStationIndex::BMS, muon, weight);
    fillPlot(precMatchedHitsBML,"nprecMatchedHitsPerChamberLayer", Muon::MuonStationIndex::BML, muon, weight);
    fillPlot(precMatchedHitsBOS,"nprecMatchedHitsPerChamberLayer", Muon::MuonStationIndex::BOS, muon, weight);
    fillPlot(precMatchedHitsBOL,"nprecMatchedHitsPerChamberLayer", Muon::MuonStationIndex::BOL, muon, weight);
    fillPlot(precMatchedHitsBEE,"nprecMatchedHitsPerChamberLayer", Muon::MuonStationIndex::BEE, muon, weight);
    fillPlot(precMatchedHitsEIS,"nprecMatchedHitsPerChamberLayer", Muon::MuonStationIndex::EIS, muon, weight);
    fillPlot(precMatchedHitsEIL,"nprecMatchedHitsPerChamberLayer", Muon::MuonStationIndex::EIL, muon, weight);
    fillPlot(precMatchedHitsEMS,"nprecMatchedHitsPerChamberLayer", Muon::MuonStationIndex::EMS, muon, weight);
    fillPlot(precMatchedHitsEML,"nprecMatchedHitsPerChamberLayer", Muon::MuonStationIndex::EML, muon, weight);
    fillPlot(precMatchedHitsEOS,"nprecMatchedHitsPerChamberLayer", Muon::MuonStationIndex::EOS, muon, weight);
    fillPlot(precMatchedHitsEOL,"nprecMatchedHitsPerChamberLayer", Muon::MuonStationIndex::EOL, muon, weight);
    fillPlot(precMatchedHitsEES,"nprecMatchedHitsPerChamberLayer", Muon::MuonStationIndex::EES, muon, weight);
    fillPlot(precMatchedHitsEEL,"nprecMatchedHitsPerChamberLayer", Muon::MuonStationIndex::EEL, muon, weight);
    fillPlot(precMatchedHitsCSS,"nprecMatchedHitsPerChamberLayer", Muon::MuonStationIndex::CSS, muon, weight);
    fillPlot(precMatchedHitsCSL,"nprecMatchedHitsPerChamberLayer", Muon::MuonStationIndex::CSL, muon, weight);
 
    fillPlot(phiMatchedHitsSummed,"nphiMatchedHitsPerChamberLayer", muon, weight);
    fillPlot(phiMatchedHitsBM1,  "nphiMatchedHitsPerChamberLayer", Muon::MuonStationIndex::BM1, muon,weight);
    fillPlot(phiMatchedHitsBM2,  "nphiMatchedHitsPerChamberLayer", Muon::MuonStationIndex::BM2, muon, weight);
    fillPlot(phiMatchedHitsBO1,  "nphiMatchedHitsPerChamberLayer", Muon::MuonStationIndex::BO1, muon, weight);
    fillPlot(phiMatchedHitsBO2,  "nphiMatchedHitsPerChamberLayer", Muon::MuonStationIndex::BO2, muon, weight);
    fillPlot(phiMatchedHitsT1,   "nphiMatchedHitsPerChamberLayer", Muon::MuonStationIndex::T1, muon, weight);
    fillPlot(phiMatchedHitsT2,   "nphiMatchedHitsPerChamberLayer", Muon::MuonStationIndex::T2, muon, weight);
    fillPlot(phiMatchedHitsT3,   "nphiMatchedHitsPerChamberLayer", Muon::MuonStationIndex::T3, muon, weight);
    fillPlot(phiMatchedHitsT4,   "nphiMatchedHitsPerChamberLayer", Muon::MuonStationIndex::T4, muon, weight);
    fillPlot(phiMatchedHitsCSC,  "nphiMatchedHitsPerChamberLayer", Muon::MuonStationIndex::CSC, muon, weight);
 
    fillPlot(trigEtaMatchedHitsSummed,"ntrigEtaMatchedHitsPerChamberLayer", muon, weight);
    fillPlot(trigEtaMatchedHitsBM1,  "ntrigEtaMatchedHitsPerChamberLayer", Muon::MuonStationIndex::BM1, muon, weight);
    fillPlot(trigEtaMatchedHitsBM2,  "ntrigEtaMatchedHitsPerChamberLayer", Muon::MuonStationIndex::BM2, muon, weight);
    fillPlot(trigEtaMatchedHitsBO1,  "ntrigEtaMatchedHitsPerChamberLayer", Muon::MuonStationIndex::BO1, muon, weight);
    fillPlot(trigEtaMatchedHitsBO2,  "ntrigEtaMatchedHitsPerChamberLayer", Muon::MuonStationIndex::BO2, muon, weight);
    fillPlot(trigEtaMatchedHitsT1,   "ntrigEtaMatchedHitsPerChamberLayer", Muon::MuonStationIndex::T1, muon, weight);
    fillPlot(trigEtaMatchedHitsT2,   "ntrigEtaMatchedHitsPerChamberLayer", Muon::MuonStationIndex::T2, muon, weight);
    fillPlot(trigEtaMatchedHitsT3,   "ntrigEtaMatchedHitsPerChamberLayer", Muon::MuonStationIndex::T3, muon, weight);
    fillPlot(trigEtaMatchedHitsT4,   "ntrigEtaMatchedHitsPerChamberLayer", Muon::MuonStationIndex::T4, muon, weight);
  //  fillPlot(trigEtaMatchedHitsCSC,  "ntrigEtaMatchedHitsPerChamberLayer", Muon::MuonStationIndex::CSC, muon);
  
}

  void MuonTruthHitPlots::fillPlot(TH1* hitPlots, const std::string& sInfo,const xAOD::Muon& muon, float weight){
  //protection
  if (muon.author()!=xAOD::Muon::MuidCo && muon.author()!=xAOD::Muon::MuidSA &&  muon.author()!=xAOD::Muon::MuGirl)  return;

  if (!muon.isAvailable<std::vector<unsigned int >>(sInfo)) return;
      std::vector<unsigned int> vec=muon.auxdata<std::vector<unsigned int>>(sInfo);
      for (unsigned int i=0;i<vec.size();i++) {
	hitPlots->Fill(vec[i], weight);
      }	     
}

  void MuonTruthHitPlots::fillPlot(TH1* hitPlots, const std::string& sInfo, const Muon::MuonStationIndex::ChIndex& index, const xAOD::Muon& muon, float weight){
  //protection
  if (muon.author()!=xAOD::Muon::MuidCo && muon.author()!=xAOD::Muon::MuidSA &&  muon.author()!=xAOD::Muon::MuGirl)  return;

  if (!muon.isAvailable<std::vector<unsigned int >>(sInfo)) return;
      std::vector<unsigned int> vec=muon.auxdata<std::vector<unsigned int>>(sInfo);
      hitPlots->Fill(vec[index], weight);
           
}
  void MuonTruthHitPlots::fillPlot(TH1* hitPlots, const std::string& sInfo, const Muon::MuonStationIndex::PhiIndex& index, const xAOD::Muon& muon, float weight){
  //protection
  if (muon.author()!=xAOD::Muon::MuidCo && muon.author()!=xAOD::Muon::MuidSA &&  muon.author()!=xAOD::Muon::MuGirl)  return;

  if (!muon.isAvailable<std::vector<unsigned int >>(sInfo)) return;
      std::vector<unsigned int> vec=muon.auxdata<std::vector<unsigned int>>(sInfo);
      hitPlots->Fill(vec[index], weight);
           
}


}
