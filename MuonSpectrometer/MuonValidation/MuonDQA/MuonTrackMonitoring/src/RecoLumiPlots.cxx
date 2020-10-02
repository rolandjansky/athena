/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTrackMonitoring/RecoLumiPlots.h"

RecoLumiPlots::RecoLumiPlots(PlotBase* pParent, std::string sDir, std::string recObj):PlotBase(pParent, sDir),

  m_hNSegment_LB_1D(nullptr),
  m_hNMuonTrack_LB_1D(nullptr),
  m_hNMuon_LB_1D(nullptr),
  m_hNResonance_LB_1D(nullptr),

  m_hNSegment_LB(nullptr),
  m_hNMuonTrack_LB(nullptr),
  m_hNMuon_LB(nullptr),
  m_hNResonance_LB(nullptr),

  type(recObj),
  name("other")
{}

void RecoLumiPlots::initializePlots()
{

  //Specify the names for different resonances
  std::string Zsig("Z");
  std::string Jsig("Jpsi");
  std::string CBMuons("CBMuons");
  std::string OtherMuons("NonCBMuons");

  if      (!type.compare(Zsig))       {name = "Z";         }
  else if (!type.compare(Jsig))       {name = "J/#psi";    }
  else if (!type.compare(CBMuons))    {name = "CBMuons";   }
  else if (!type.compare(OtherMuons)) {name = "NonCBMuons";}
  else                                {name = "other";     }

  n_lbs = 2500;

  //Make sure the 1D hists has the same dimension as the 2D!
  if (name == "other"){
    m_hNSegment_LB      = Book2D("nSegment_LB_2D", "Number of Muon Segments Per LumiBlock;LumiBlock;Number of Segments",  n_lbs, -0.5, (float)(n_lbs)-0.5, 100, 0.5, 101.5);
    m_hNMuonTrack_LB    = Book2D("nMuonTrack_LB_2D", "Number of Muon MS Tracks Per LumiBlock;LumiBlock;Number of Tracks", n_lbs, -0.5, (float)(n_lbs)-0.5,  21, 0.5, 21.5);
    m_hNSegment_LB_1D   = Book1D("nSegment_LB", "Number of Muon Segments Per LumiBlock;LumiBlock;Number of Segments",     n_lbs, -0.5, (float)(n_lbs)-0.5);
    m_hNMuonTrack_LB_1D = Book1D("nMuonTrack_LB", "Number of Muon MS Tracks Per LumiBlock;LumiBlock;Number of Tracks",    n_lbs, -0.5, (float)(n_lbs)-0.5);
  }
  else if (name == "CBMuons"){
    m_hNMuon_LB = Book2D("nMuon_LB_2D", "Number of Muons Per LumiBlock;LumiBlock;Nmuons", n_lbs, -0.5, (float)(n_lbs)-0.5, 21, 0.5, 21.5);
    m_hNMuon_LB_1D = Book1D("nMuon_LB", "Number of Muons Per LumiBlock;LumiBlock;Nmuons", n_lbs, -0.5, (float)(n_lbs)-0.5);
  }
  else if (name == "NonCBMuons"){
    m_hNMuon_LB = Book2D("nMuon_LB_2D", "Number of non-CB Muons Per LumiBlock;LumiBlock;Nmuons", n_lbs, -0.5, (float)(n_lbs)-0.5, 21, 0.5, 21.5);
    m_hNMuon_LB_1D = Book1D("nMuon_LB", "Number of non-CB Muons Per LumiBlock;LumiBlock;Nmuons", n_lbs, -0.5, (float)(n_lbs)-0.5);
  }
  else if (name == "Z" || name == "J/#psi"){
    m_hNResonance_LB = Book2D("n" + type + "_LB_2D", "Number of " + name + " Per LumiBlock;LumiBlock;Number of " + name, n_lbs, -0.5, (float)(n_lbs)-0.5, 5, 0.5, 5.5);
    m_hNResonance_LB_1D = Book1D("n" + type + "_LB", "Number of " + name + " Per LumiBlock;LumiBlock;Number of " + name, n_lbs, -0.5, (float)(n_lbs)-0.5);
  }

}

//fill Segment related lb plots
void RecoLumiPlots::fill(const xAOD::MuonSegmentContainer* MuonSegments, int current_lb, float inst_lumi_bcid, float inst_lumi_lb) {
  
  m_hNSegment_LB->Fill(current_lb, MuonSegments->size() + inst_lumi_bcid - inst_lumi_bcid + inst_lumi_lb - inst_lumi_lb);

}

//fill Track related plots
void RecoLumiPlots::fill(const xAOD::TrackParticleContainer* MSTracks, int current_lb, float inst_lumi_bcid, float inst_lumi_lb){

  m_hNMuonTrack_LB->Fill(current_lb, MSTracks->size() + inst_lumi_bcid - inst_lumi_bcid + inst_lumi_lb - inst_lumi_lb);

}

//fill CB muon related plots
void RecoLumiPlots::fill_CB(const xAOD::MuonContainer* Muons, int current_lb, float inst_lumi_bcid, float inst_lumi_lb){

  int NMuIDco = 0;

  for(const auto mu : *Muons) {

    //do a muon quality cut: 0 tight, 1 medium, 2 loss
    if(mu->muonType() == xAOD::Muon::Combined){NMuIDco++;}

  }

  //only do this for combined muons
  m_hNMuon_LB->Fill(current_lb, NMuIDco + inst_lumi_bcid - inst_lumi_bcid + inst_lumi_lb - inst_lumi_lb);

}

//fill other muon related plots
void RecoLumiPlots::fill_Other(const xAOD::MuonContainer* Muons, int current_lb, float inst_lumi_bcid, float inst_lumi_lb){

  int NMuIDco = 0;

  for(const auto mu : *Muons) {

    //do a muon quality cut: 0 tight, 1 medium, 2 loss
    if(mu->muonType() != xAOD::Muon::Combined){NMuIDco++;}

  }

  //only do this for combined muons
  m_hNMuon_LB->Fill(current_lb, NMuIDco + inst_lumi_bcid - inst_lumi_bcid + inst_lumi_lb - inst_lumi_lb);

}

//fill muon related plots
void RecoLumiPlots::fill(std::vector<std::pair<const xAOD::Muon*, const xAOD::Muon*> > resonances, int current_lb, float inst_lumi_bcid, float inst_lumi_lb){

  int Nresonance = resonances.size();

  //only do this for combined muons
  m_hNResonance_LB->Fill(current_lb, Nresonance + inst_lumi_bcid - inst_lumi_bcid + inst_lumi_lb - inst_lumi_lb);//playing really dumb here

} 

