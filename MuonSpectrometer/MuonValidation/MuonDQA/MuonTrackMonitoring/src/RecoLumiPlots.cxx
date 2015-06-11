/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTrackMonitoring/RecoLumiPlots.h"

RecoLumiPlots::RecoLumiPlots(PlotBase* pParent, std::string sDir, std::string recObj):PlotBase(pParent, sDir),

  m_hNSegment_LB(NULL),
  m_hNMuonTrack_LB(NULL),
  m_hNMuon_LB(NULL),
  m_hNResonance_LB(NULL),

  m_hNSegment_Inst(NULL),
  m_hNMuonTrack_Inst(NULL),
  m_hNMuon_Inst(NULL),
  m_hNResonance_Inst(NULL),
  type(recObj),
  name("other")
{}

void RecoLumiPlots::initializePlots()
{
  //Specify the names for different resonances
  std::string Zsig("Z");
  std::string Jsig("Jpsi");
  std::string AllMuons("AllMuons");

  if (!type.compare(Zsig)) {name="Z";}
  else if (!type.compare(Jsig)) {name="J/#psi";}
  else if (!type.compare(AllMuons)) {name="Muons";}
  else {name = "other";}

  m_hNSegment_LB = Book2D("nSegment_LB", "Number of Muon Segments Per LumiBlock;LumiBlock;Number of Segments", 2000, -0.5, 1999.5, 100, 0.5, 101.5);
  m_hNMuonTrack_LB = Book2D("nMuonTrack_LB", "Number of Muon MS Tracks Per LumiBlock;LumiBlock;Number of Tracks", 2000, -0.5, 1999.5, 21, 0.5, 21.5);
  if (name != "other") m_hNMuon_LB = Book2D("nMuon_LB", "Number of Loose+ Muons Per LumiBlock;LumiBlock;Nmuons", 2000, -0.5, 1999.5, 21, 0.5, 21.5);
  if (name == "Z" || name == "J/#psi"){m_hNResonance_LB = Book2D("n" + type + "_LB", "Number of " + name + " Per LumiBlock;LumiBlock;Number of " + name, 2000, -0.5, 1999.5, 5, 0.5, 5.5);}

  m_hNSegment_Inst = Book2D("nSegment_Inst", "Number of Muon Segments vs Instant Lumi;Instant Lumi, 10^{30}cm^{-2}s^{-1};Nsegments", 150, -0.5, 10.5, 100, 0.5, 101.5);
  m_hNMuonTrack_Inst = Book2D("nMuonTrack_Inst", "Number of Muon MS Tracks vs Instant Lumi;Instant Lumi, 10^{30}cm^{-2}s^{-1};Ntracks", 150, -0.5, 10.5, 21, 0.5, 21.5);
  if (name != "other") m_hNMuon_Inst = Book2D("nMuon_Inst", "Number of Loose+  Muons vs Instant Lumi;Instant Lumi, 10^{30}cm^{-2}s^{-1};Nmuons", 150, -0.5, 10.5, 21, 0.5, 21.5);
  if (name == "Z" || name == "J/#psi"){m_hNResonance_Inst = Book2D("n" + type + "_Inst", "Number of " + name + " vs Instant Lumi;Instant Lumi, 10^{30}cm^{-2}s^{-1};N " + name, 150, -0.5, 10.5, 5, 0.5, 5.5);}
}

//fill Segment related lb plots
void RecoLumiPlots::fill(const xAOD::MuonSegmentContainer* MuonSegments, int m_current_lb, float m_inst_lumi) {
  
  m_hNSegment_LB->Fill(m_current_lb, MuonSegments->size());
  if (m_inst_lumi > 0) m_hNSegment_Inst->Fill(m_inst_lumi, MuonSegments->size());
}

//fill Track related plots
void RecoLumiPlots::fill(const xAOD::TrackParticleContainer* MSTracks, int m_current_lb, float m_inst_lumi){

  m_hNMuonTrack_LB->Fill(m_current_lb, MSTracks->size());
  if (m_inst_lumi > 0) m_hNMuonTrack_Inst->Fill(m_inst_lumi, MSTracks->size());
}

//fill muon related plots
void RecoLumiPlots::fill(const xAOD::MuonContainer* Muons, int m_current_lb, float m_inst_lumi){

  int m_NMuIDco = 0;
  int m_NTrkIDco = 0;
  int m_NSegIDco = 0;
  for(const auto mu : *Muons) {
    //do a muon quality cut: 0 tight, 1 medium, 2 loss
    if(mu->quality() <= 2){m_NMuIDco++;}
    //check the muon related segments
    const std::vector<ElementLink<DataVector<xAOD::MuonSegment_v1>>> Mu_Segments = mu->muonSegmentLinks();
    for(const auto Mu_Segment : Mu_Segments){
      const ElementLink<DataVector<xAOD::MuonSegment_v1>> Mu_seg = Mu_Segment;
      if(Mu_seg.isValid()){
        m_NSegIDco++;
      }
    }
    //check muon related tracks
    ElementLink<xAOD::TrackParticleContainer> Mu_MStrack = mu->muonSpectrometerTrackParticleLink();
    if(Mu_MStrack.isValid()){
      m_NTrkIDco++;
    }
  }
  //only do this for combined muons
  m_hNMuon_LB->Fill(m_current_lb, m_NMuIDco);
  if (m_inst_lumi > 0) m_hNMuon_Inst->Fill(m_inst_lumi, m_NMuIDco);
  m_hNMuonTrack_LB->Fill(m_current_lb, m_NTrkIDco);
  if (m_inst_lumi > 0) m_hNMuonTrack_Inst->Fill(m_inst_lumi, m_NTrkIDco);
  m_hNSegment_LB->Fill(m_current_lb, m_NSegIDco);
  if (m_inst_lumi > 0) m_hNSegment_Inst->Fill(m_inst_lumi, m_NSegIDco);

}


//fill muon related plots
void RecoLumiPlots::fill(std::vector<std::pair<const xAOD::Muon*, const xAOD::Muon*> > resonances, int m_current_lb, float m_inst_lumi){

  int m_Nresonance = resonances.size();
  //only do this for combined muons
  m_hNResonance_LB->Fill(m_current_lb, m_Nresonance);
  if (m_inst_lumi != 0) m_hNResonance_Inst->Fill(m_inst_lumi, m_Nresonance);


  int m_NMuIDco = 0;
  int m_NTrkIDco = 0;
  int m_NSegIDco = 0;
  for (auto resonance: resonances)
  {

    if(resonance.first->quality() <= 2){m_NMuIDco++;}
    if(resonance.second->quality() <= 2){m_NMuIDco++;}
    //do a muon quality cut: 0 tight, 1 medium, 2 loss
    //check the muon related segments
    const std::vector<ElementLink<DataVector<xAOD::MuonSegment_v1>>> Mu_Segments_first = resonance.first->muonSegmentLinks();
    for(const auto Mu_Segment : Mu_Segments_first){
      const ElementLink<DataVector<xAOD::MuonSegment_v1>> Mu_seg = Mu_Segment;
      if(Mu_seg.isValid()){
        m_NSegIDco++;
      }
    }
    //check muon related tracks
    ElementLink<xAOD::TrackParticleContainer> Mu_MStrack_first = resonance.first->muonSpectrometerTrackParticleLink();
    if(Mu_MStrack_first.isValid()){
      m_NTrkIDco++;
    }
    //do a muon quality cut: 0 tight, 1 medium, 2 loss
    //check the muon related segments
    const std::vector<ElementLink<DataVector<xAOD::MuonSegment_v1>>> Mu_Segments_second = resonance.second->muonSegmentLinks();
    for(const auto Mu_Segment : Mu_Segments_second){
      const ElementLink<DataVector<xAOD::MuonSegment_v1>> Mu_seg = Mu_Segment;
      if(Mu_seg.isValid()){
        m_NSegIDco++;
      }
    }
    //check muon related tracks
    ElementLink<xAOD::TrackParticleContainer> Mu_MStrack_second= resonance.second->muonSpectrometerTrackParticleLink();
    if(Mu_MStrack_second.isValid()){
      m_NTrkIDco++;
    }
  }
  //only do this for combined muons
  m_hNMuon_LB->Fill(m_current_lb, m_NMuIDco);
  if (m_inst_lumi > 0) m_hNMuon_Inst->Fill(m_inst_lumi, m_NMuIDco);
  m_hNMuonTrack_LB->Fill(m_current_lb, m_NTrkIDco);
  if (m_inst_lumi > 0) m_hNMuonTrack_Inst->Fill(m_inst_lumi, m_NTrkIDco);
  m_hNSegment_LB->Fill(m_current_lb, m_NSegIDco);
  if (m_inst_lumi > 0) m_hNSegment_Inst->Fill(m_inst_lumi, m_NSegIDco);
} 

