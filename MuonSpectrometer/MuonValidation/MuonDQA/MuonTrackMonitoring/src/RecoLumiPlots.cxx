/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTrackMonitoring/RecoLumiPlots.h"

RecoLumiPlots::RecoLumiPlots(PlotBase* pParent, std::string sDir, std::string recObj):PlotBase(pParent, sDir),
  //book the 1D hists
  m_hNSegment_LB_1D(NULL),
  m_hNMuonTrack_LB_1D(NULL),
  m_hNMuon_LB_1D(NULL),
  m_hNResonance_LB_1D(NULL),

  // m_hNSegment_Inst_1D(NULL),
  // m_hNMuonTrack_Inst_1D(NULL),
  // m_hNMuon_Inst_1D(NULL),
  // m_hNResonance_Inst_1D(NULL),

  // m_hNSegment_IntLumi_1D(NULL),
  // m_hNMuonTrack_IntLumi_1D(NULL),
  // m_hNMuon_IntLumi_1D(NULL),
  // m_hNResonance_IntLumi_1D(NULL),

  // m_hNSegment_LB_BA_1D(NULL),
  // m_hNSegment_LB_BC_1D(NULL),
  // m_hNSegment_LB_EA_1D(NULL),
  // m_hNSegment_LB_EC_1D(NULL),
  
  //book the usual 2D hists
  m_hNSegment_LB(NULL),
  m_hNMuonTrack_LB(NULL),
  m_hNMuon_LB(NULL),
  m_hNResonance_LB(NULL),

  // m_hNSegment_Inst(NULL),
  // m_hNMuonTrack_Inst(NULL),
  // m_hNMuon_Inst(NULL),
  // m_hNResonance_Inst(NULL),

  // m_hNSegment_IntLumi(NULL),
  // m_hNMuonTrack_IntLumi(NULL),
  // m_hNMuon_IntLumi(NULL),
  // m_hNResonance_IntLumi(NULL),

  // m_hNSegment_LB_BA(NULL),
  // m_hNSegment_LB_BC(NULL),
  // m_hNSegment_LB_EA(NULL),
  // m_hNSegment_LB_EC(NULL),

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

  if (!type.compare(Zsig)) {name="Z";}
  else if (!type.compare(Jsig)) {name="J/#psi";}
  else if (!type.compare(CBMuons)) {name="CBMuons";}
  else if (!type.compare(OtherMuons)) {name="NonCBMuons";}
  else {name = "other";}

  //Make sure the 1D hists has the same dimension as the 2D!
  if (name == "other"){
    m_hNSegment_LB = Book2D("nSegment_LB_2D", "Number of Muon Segments Per LumiBlock;LumiBlock;Number of Segments", 1600, -0.5, 1599.5, 100, 0.5, 101.5);
    m_hNMuonTrack_LB = Book2D("nMuonTrack_LB_2D", "Number of Muon MS Tracks Per LumiBlock;LumiBlock;Number of Tracks", 1600, -0.5, 1599.5, 21, 0.5, 21.5);
    m_hNSegment_LB_1D = Book1D("nSegment_LB", "Number of Muon Segments Per LumiBlock;LumiBlock;Number of Segments", 1600, -0.5, 1599.5);
    m_hNMuonTrack_LB_1D = Book1D("nMuonTrack_LB", "Number of Muon MS Tracks Per LumiBlock;LumiBlock;Number of Tracks", 1600, -0.5, 1599.5);
  }
  else if (name == "CBMuons"){
    m_hNMuon_LB = Book2D("nMuon_LB_2D", "Number of Muons Per LumiBlock;LumiBlock;Nmuons", 1600, -0.5, 1599.5, 21, 0.5, 21.5);
    m_hNMuon_LB_1D = Book1D("nMuon_LB", "Number of Muons Per LumiBlock;LumiBlock;Nmuons", 1600, -0.5, 1599.5);
  }
  else if (name == "NonCBMuons"){
    m_hNMuon_LB = Book2D("nMuon_LB_2D", "Number of non-CB Muons Per LumiBlock;LumiBlock;Nmuons", 1600, -0.5, 1599.5, 21, 0.5, 21.5);
    m_hNMuon_LB_1D = Book1D("nMuon_LB", "Number of non-CB Muons Per LumiBlock;LumiBlock;Nmuons", 1600, -0.5, 1599.5);
  }
  else if (name == "Z" || name == "J/#psi"){
    m_hNResonance_LB = Book2D("n" + type + "_LB_2D", "Number of " + name + " Per LumiBlock;LumiBlock;Number of " + name, 1600, -0.5, 1599.5, 5, 0.5, 5.5);
    m_hNResonance_LB_1D = Book1D("n" + type + "_LB", "Number of " + name + " Per LumiBlock;LumiBlock;Number of " + name, 1600, -0.5, 1599.5);
  }

  // m_hNSegment_Inst = Book2D("nSegment_Inst_2D", "Number of Muon Segments vs Instant Lumi per BCID;Instant Lumi per BCID, 10^{30}cm^{-2}s^{-1};Nsegments", 150, -0.5, 10.5, 100, 0.5, 101.5);
  // m_hNMuonTrack_Inst = Book2D("nMuonTrack_Inst_2D", "Number of Muon MS Tracks vs Instant Lumi per BCID;Instant Lumi per BCID, 10^{30}cm^{-2}s^{-1};Ntracks", 150, -0.5, 10.5, 21, 0.5, 21.5);
  // if (name != "other") m_hNMuon_Inst = Book2D("nMuon_Inst_2D", "Number of Muons vs Instant Lumi per BCID;Instant Lumi per BCID, 10^{30}cm^{-2}s^{-1};Nmuons", 150, -0.5, 10.5, 21, 0.5, 21.5);
  // if (name == "Z" || name == "J/#psi"){m_hNResonance_Inst = Book2D("n" + type + "_Inst_2D", "Number of " + name + " vs Instant Lumi per BCID;Instant Lumi per BCID, 10^{30}cm^{-2}s^{-1};N " + name, 150, -0.5, 10.5, 5, 0.5, 5.5);}

  // m_hNSegment_IntLumi = Book2D("nSegment_IntLumi_2D", "Number of Muon Segments vs Average Inst Lumi per Sec;Average Inst Lumi per Sec, 10^{30}cm^{-2}s^{-1};Nsegments", 600, -0.5, 5999.5, 100, 0.5, 101.5);
  // m_hNMuonTrack_IntLumi = Book2D("nMuonTrack_IntLumi_2D", "Number of Muon MS Tracks vs Average Inst Lumi per Sec;Average Inst Lumi per Sec, 10^{30}cm^{-2}s^{-1};Ntracks", 600, -0.5, 5999.5, 21, 0.5, 21.5);
  // if (name != "other") m_hNMuon_IntLumi = Book2D("nMuon_IntLumi_2D", "Number of Muons vs Average Inst Lumi per Sec;Average Inst Lumi per Sec, 10^{30}cm^{-2}s^{-1};Nmuons", 600, -0.5, 5999.5, 21, 0.5, 21.5);
  // if (name == "Z" || name == "J/#psi"){m_hNResonance_IntLumi = Book2D("n" + type + "_IntLumi_2D", "Number of " + name + " vs Average Inst Lumi per Sec;Average Inst Lumi per Sec, 10^{30}cm^{-2}s^{-1};N " + name, 600, -0.5, 5999.5, 5, 0.5, 5.5);}

  // m_hNSegment_LB_BA = Book2D("nSegment_LB_BA_2D", "Number of Muon Segments in Barrel A side Per LumiBlock;LumiBlock;Number of Segments", 1600, -0.5, 1599.5, 80, 0.5, 81.5);
  // m_hNSegment_LB_BC = Book2D("nSegment_LB_BC_2D", "Number of Muon Segments in Barrel C side Per LumiBlock;LumiBlock;Number of Segments", 1600, -0.5, 1599.5, 80, 0.5, 81.5);
  // m_hNSegment_LB_EA = Book2D("nSegment_LB_EA_2D", "Number of Muon Segments in Endcap A side Per LumiBlock;LumiBlock;Number of Segments", 1600, -0.5, 1599.5, 80, 0.5, 81.5);
  // m_hNSegment_LB_EC = Book2D("nSegment_LB_EC_2D", "Number of Muon Segments in Endcap C side Per LumiBlock;LumiBlock;Number of Segments", 1600, -0.5, 1599.5, 80, 0.5, 81.5);

  //book the 1D hists for webdisplay

  // m_hNSegment_Inst_1D = Book1D("nSegment_Inst", "Number of Muon Segments vs Instant Lumi per BCID;Instant Lumi per BCID, 10^{30}cm^{-2}s^{-1};Nsegments", 150, -0.5, 10.5);
  // m_hNMuonTrack_Inst_1D = Book1D("nMuonTrack_Inst", "Number of Muon MS Tracks vs Instant Lumi per BCID;Instant Lumi per BCID, 10^{30}cm^{-2}s^{-1};Ntracks", 150, -0.5, 10.5);
  // if (name != "other") m_hNMuon_Inst_1D = Book1D("nMuon_Inst", "Number of Muons vs Instant Lumi per BCID;Instant Lumi per BCID, 10^{30}cm^{-2}s^{-1};Nmuons", 150, -0.5, 10.5);
  // if (name == "Z" || name == "J/#psi"){m_hNResonance_Inst_1D = Book1D("n" + type + "_Inst", "Number of " + name + " vs Instant Lumi per BCID;Instant Lumi per BCID, 10^{30}cm^{-2}s^{-1};N " + name, 150, -0.5, 10.5);}

  // m_hNSegment_IntLumi_1D = Book1D("nSegment_IntLumi", "Number of Muon Segments vs Average Inst Lumi per Sec;Average Inst Lumi per Sec, 10^{30}cm^{-2}s^{-1};Nsegments", 500, -0.5, 1000.5);
  // m_hNMuonTrack_IntLumi_1D = Book1D("nMuonTrack_IntLumi", "Number of Muon MS Tracks vs Average Inst Lumi per Sec;Average Inst Lumi per Sec, 10^{30}cm^{-2}s^{-1};Ntracks", 500, -0.5, 1000.5);
  // if (name != "other") m_hNMuon_IntLumi_1D = Book1D("nMuon_IntLumi", "Number of Muons vs Average Inst Lumi per Sec;Average Inst Lumi per Sec, 10^{30}cm^{-2}s^{-1};Nmuons", 500, -0.5, 1000.5);
  // if (name == "Z" || name == "J/#psi"){m_hNResonance_IntLumi_1D = Book1D("n" + type + "_IntLumi", "Number of " + name + " vs Average Inst Lumi per Sec;Average Inst Lumi per Sec, 10^{30}cm^{-2}s^{-1};N " + name, 500, -0.5, 1000.5);}

  // m_hNSegment_LB_BA_1D = Book1D("nSegment_LB_BA", "Number of Muon Segments in Barrel A side Per LumiBlock;LumiBlock;Number of Segments", 1600, -0.5, 1599.5);
  // m_hNSegment_LB_BC_1D = Book1D("nSegment_LB_BC", "Number of Muon Segments in Barrel C side Per LumiBlock;LumiBlock;Number of Segments", 1600, -0.5, 1599.5);
  // m_hNSegment_LB_EA_1D = Book1D("nSegment_LB_EA", "Number of Muon Segments in Endcap A side Per LumiBlock;LumiBlock;Number of Segments", 1600, -0.5, 1599.5);
  // m_hNSegment_LB_EC_1D = Book1D("nSegment_LB_EC", "Number of Muon Segments in Endcap C side Per LumiBlock;LumiBlock;Number of Segments", 1600, -0.5, 1599.5);
}

//fill Segment related lb plots
void RecoLumiPlots::fill(const xAOD::MuonSegmentContainer* MuonSegments, int m_current_lb, float m_inst_lumi_bcid, float m_inst_lumi_lb) {
  
  m_hNSegment_LB->Fill(m_current_lb, MuonSegments->size() + m_inst_lumi_bcid - m_inst_lumi_bcid + m_inst_lumi_lb - m_inst_lumi_lb);
  // if (m_inst_lumi_bcid > 0) m_hNSegment_Inst->Fill(m_inst_lumi_bcid, MuonSegments->size());
  // if (m_inst_lumi_lb > 0) m_hNSegment_IntLumi->Fill(m_inst_lumi_lb, MuonSegments->size());

  // int m_NsegBA = 0;
  // int m_NsegBC = 0;
  // int m_NsegEA = 0;
  // int m_NsegEC = 0;
  // for (const auto museg : *MuonSegments) {
  //   if ((museg->chamberIndex() < Muon::MuonStationIndex::BEE) && (museg->z() > 0)) m_NsegBA++;
  //   if ((museg->chamberIndex() < Muon::MuonStationIndex::BEE) && (museg->z() < 0)) m_NsegBC++;
  //   if ((museg->chamberIndex() >= Muon::MuonStationIndex::BEE) && (museg->z() > 0)) m_NsegEA++;
  //   if ((museg->chamberIndex() >= Muon::MuonStationIndex::BEE) && (museg->z() < 0)) m_NsegEC++;
  // }
  // m_hNSegment_LB_BA->Fill(m_current_lb, m_NsegBA);
  // m_hNSegment_LB_BC->Fill(m_current_lb, m_NsegBC);
  // m_hNSegment_LB_EA->Fill(m_current_lb, m_NsegEA);
  // m_hNSegment_LB_EC->Fill(m_current_lb, m_NsegEC);
}

//fill Track related plots
void RecoLumiPlots::fill(const xAOD::TrackParticleContainer* MSTracks, int m_current_lb, float m_inst_lumi_bcid, float m_inst_lumi_lb){

  m_hNMuonTrack_LB->Fill(m_current_lb, MSTracks->size() + m_inst_lumi_bcid - m_inst_lumi_bcid + m_inst_lumi_lb - m_inst_lumi_lb);
  //if (m_inst_lumi_bcid > 0) m_hNMuonTrack_Inst->Fill(m_inst_lumi_bcid, MSTracks->size());
  //if (m_inst_lumi_lb > 0) m_hNMuonTrack_IntLumi->Fill(m_inst_lumi_lb, MSTracks->size());
}

//fill CB muon related plots
void RecoLumiPlots::fill_CB(const xAOD::MuonContainer* Muons, int m_current_lb, float m_inst_lumi_bcid, float m_inst_lumi_lb){



  int m_NMuIDco = 0;
  //int m_NTrkIDco = 0;
  //int m_NSegIDco = 0;
  // int m_NsegBA = 0;
  // int m_NsegBC = 0;
  // int m_NsegEA = 0;
  // int m_NsegEC = 0;

  for(const auto mu : *Muons) {
    //do a muon quality cut: 0 tight, 1 medium, 2 loss
    if(mu->muonType() == xAOD::Muon::Combined){m_NMuIDco++;}
    //check the muon related segments
    // const std::vector<ElementLink<DataVector<xAOD::MuonSegment_v1>>> Mu_Segments = mu->muonSegmentLinks();
    // for(const auto Mu_Segment : Mu_Segments){
    //   const ElementLink<DataVector<xAOD::MuonSegment_v1>> Mu_seg = Mu_Segment;
    //   if(Mu_seg.isValid()){
    //     m_NSegIDco++;
    //     // const xAOD::MuonSegment* museg = *Mu_seg;
    //     // if ((museg->chamberIndex() < Muon::MuonStationIndex::BEE) && (museg->z() > 0)) m_NsegBA++;
    //     // if ((museg->chamberIndex() < Muon::MuonStationIndex::BEE) && (museg->z() < 0)) m_NsegBC++;
    //     // if ((museg->chamberIndex() >= Muon::MuonStationIndex::BEE) && (museg->z() > 0)) m_NsegEA++;
    //     // if ((museg->chamberIndex() >= Muon::MuonStationIndex::BEE) && (museg->z() < 0)) m_NsegEC++;
    //   }
    // }
    // //check muon related tracks
    // ElementLink<xAOD::TrackParticleContainer> Mu_MStrack = mu->muonSpectrometerTrackParticleLink();
    // if(Mu_MStrack.isValid()){
    //   m_NTrkIDco++;
    // }
  }
  //only do this for combined muons
  m_hNMuon_LB->Fill(m_current_lb, m_NMuIDco + m_inst_lumi_bcid - m_inst_lumi_bcid + m_inst_lumi_lb - m_inst_lumi_lb);
  //if (m_inst_lumi_bcid > 0) m_hNMuon_Inst->Fill(m_inst_lumi_bcid, m_NMuIDco);
  //if (m_inst_lumi_lb > 0) m_hNMuon_IntLumi->Fill(m_inst_lumi_lb, m_NMuIDco);
  //m_hNMuonTrack_LB->Fill(m_current_lb, m_NTrkIDco);
  //if (m_inst_lumi_bcid > 0) m_hNMuonTrack_Inst->Fill(m_inst_lumi_bcid, m_NTrkIDco);
  //if (m_inst_lumi_lb > 0) m_hNMuonTrack_IntLumi->Fill(m_inst_lumi_lb, m_NTrkIDco);
  //m_hNSegment_LB->Fill(m_current_lb, m_NSegIDco);
  //if (m_inst_lumi_bcid > 0) m_hNSegment_Inst->Fill(m_inst_lumi_bcid, m_NSegIDco);
  //if (m_inst_lumi_lb > 0) m_hNSegment_IntLumi->Fill(m_inst_lumi_lb, m_NSegIDco);
  // m_hNSegment_LB_BA->Fill(m_current_lb, m_NsegBA);
  // m_hNSegment_LB_BC->Fill(m_current_lb, m_NsegBC);
  // m_hNSegment_LB_EA->Fill(m_current_lb, m_NsegEA);
  // m_hNSegment_LB_EC->Fill(m_current_lb, m_NsegEC);

}

//fill other muon related plots
void RecoLumiPlots::fill_Other(const xAOD::MuonContainer* Muons, int m_current_lb, float m_inst_lumi_bcid, float m_inst_lumi_lb){

  int m_NMuIDco = 0;
  // int m_NTrkIDco = 0;
  // int m_NSegIDco = 0;
  // int m_NsegBA = 0;
  // int m_NsegBC = 0;
  // int m_NsegEA = 0;
  // int m_NsegEC = 0;

  for(const auto mu : *Muons) {
    //do a muon quality cut: 0 tight, 1 medium, 2 loss
    if(mu->muonType() != xAOD::Muon::Combined){m_NMuIDco++;}
    //check the muon related segments
    // const std::vector<ElementLink<DataVector<xAOD::MuonSegment_v1>>> Mu_Segments = mu->muonSegmentLinks();
    // for(const auto Mu_Segment : Mu_Segments){
    //   const ElementLink<DataVector<xAOD::MuonSegment_v1>> Mu_seg = Mu_Segment;
    //   if(Mu_seg.isValid()){
    //     m_NSegIDco++;
    //     // const xAOD::MuonSegment* museg = *Mu_seg;
    //     // if ((museg->chamberIndex() < Muon::MuonStationIndex::BEE) && (museg->z() > 0)) m_NsegBA++;
    //     // if ((museg->chamberIndex() < Muon::MuonStationIndex::BEE) && (museg->z() < 0)) m_NsegBC++;
    //     // if ((museg->chamberIndex() >= Muon::MuonStationIndex::BEE) && (museg->z() > 0)) m_NsegEA++;
    //     // if ((museg->chamberIndex() >= Muon::MuonStationIndex::BEE) && (museg->z() < 0)) m_NsegEC++;
    //   }
    // }
    // //check muon related tracks
    // ElementLink<xAOD::TrackParticleContainer> Mu_MStrack = mu->muonSpectrometerTrackParticleLink();
    // if(Mu_MStrack.isValid()){
    //   m_NTrkIDco++;
    // }
  }
  //only do this for combined muons
  m_hNMuon_LB->Fill(m_current_lb, m_NMuIDco + m_inst_lumi_bcid - m_inst_lumi_bcid + m_inst_lumi_lb - m_inst_lumi_lb);
  // if (m_inst_lumi_bcid > 0) m_hNMuon_Inst->Fill(m_inst_lumi_bcid, m_NMuIDco);
  // if (m_inst_lumi_lb > 0) m_hNMuon_IntLumi->Fill(m_inst_lumi_lb, m_NMuIDco);
  // m_hNMuonTrack_LB->Fill(m_current_lb, m_NTrkIDco);
  // if (m_inst_lumi_bcid > 0) m_hNMuonTrack_Inst->Fill(m_inst_lumi_bcid, m_NTrkIDco);
  // if (m_inst_lumi_lb > 0) m_hNMuonTrack_IntLumi->Fill(m_inst_lumi_lb, m_NTrkIDco);
  // m_hNSegment_LB->Fill(m_current_lb, m_NSegIDco);
  // if (m_inst_lumi_bcid > 0) m_hNSegment_Inst->Fill(m_inst_lumi_bcid, m_NSegIDco);
  // if (m_inst_lumi_lb > 0) m_hNSegment_IntLumi->Fill(m_inst_lumi_lb, m_NSegIDco);
  // m_hNSegment_LB_BA->Fill(m_current_lb, m_NsegBA);
  // m_hNSegment_LB_BC->Fill(m_current_lb, m_NsegBC);
  // m_hNSegment_LB_EA->Fill(m_current_lb, m_NsegEA);
  // m_hNSegment_LB_EC->Fill(m_current_lb, m_NsegEC);

}

//fill muon related plots
void RecoLumiPlots::fill(std::vector<std::pair<const xAOD::Muon*, const xAOD::Muon*> > resonances, int m_current_lb, float m_inst_lumi_bcid, float m_inst_lumi_lb){

  int m_Nresonance = resonances.size();
  //only do this for combined muons
  m_hNResonance_LB->Fill(m_current_lb, m_Nresonance + m_inst_lumi_bcid - m_inst_lumi_bcid + m_inst_lumi_lb - m_inst_lumi_lb);//playing really dumb here
  //if (m_inst_lumi_bcid != 0) m_hNResonance_Inst->Fill(m_inst_lumi_bcid, m_Nresonance);
  //if (m_inst_lumi_lb != 0) m_hNResonance_IntLumi->Fill(m_inst_lumi_lb, m_Nresonance);


  // int m_NMuIDco = 0;
  // int m_NTrkIDco = 0;
  // int m_NSegIDco = 0;
  // int m_NsegBA = 0;
  // int m_NsegBC = 0;
  // int m_NsegEA = 0;
  // int m_NsegEC = 0;
  // for (auto resonance: resonances)
  // {

  //   if(resonance.first->muonType() == xAOD::Muon::Combined){m_NMuIDco++;}
  //   if(resonance.second->muonType() == xAOD::Muon::Combined){m_NMuIDco++;}
  //   //do a muon quality cut: 0 tight, 1 medium, 2 loss
  //   //check the muon related segments
  //   const std::vector<ElementLink<DataVector<xAOD::MuonSegment_v1>>> Mu_Segments_first = resonance.first->muonSegmentLinks();
  //   for(const auto Mu_Segment : Mu_Segments_first){
  //     const ElementLink<DataVector<xAOD::MuonSegment_v1>> Mu_seg = Mu_Segment;
  //     if(Mu_seg.isValid()){
  //       m_NSegIDco++;
  //     }
  //   }
  //   //check muon related tracks
  //   ElementLink<xAOD::TrackParticleContainer> Mu_MStrack_first = resonance.first->muonSpectrometerTrackParticleLink();
  //   if(Mu_MStrack_first.isValid()){
  //     m_NTrkIDco++;
  //   }
  //   //do a muon quality cut: 0 tight, 1 medium, 2 loss
  //   //check the muon related segments
  //   const std::vector<ElementLink<DataVector<xAOD::MuonSegment_v1>>> Mu_Segments_second = resonance.second->muonSegmentLinks();
  //   for(const auto Mu_Segment : Mu_Segments_second){
  //     const ElementLink<DataVector<xAOD::MuonSegment_v1>> Mu_seg = Mu_Segment;
  //     if(Mu_seg.isValid()){
  //       m_NSegIDco++;
  //       // const xAOD::MuonSegment* museg = *Mu_seg;
  //       // if ((museg->chamberIndex() < Muon::MuonStationIndex::BEE) && (museg->z() > 0)) m_NsegBA++;
  //       // if ((museg->chamberIndex() < Muon::MuonStationIndex::BEE) && (museg->z() < 0)) m_NsegBC++;
  //       // if ((museg->chamberIndex() >= Muon::MuonStationIndex::BEE) && (museg->z() > 0)) m_NsegEA++;
  //       // if ((museg->chamberIndex() >= Muon::MuonStationIndex::BEE) && (museg->z() < 0)) m_NsegEC++;
  //     }
  //   }
  //   //check muon related tracks
  //   ElementLink<xAOD::TrackParticleContainer> Mu_MStrack_second = resonance.second->muonSpectrometerTrackParticleLink();
  //   if(Mu_MStrack_second.isValid()){
  //     m_NTrkIDco++;
  //   }
  // }
  //only do this for combined muons
  // m_hNMuon_LB->Fill(m_current_lb, m_NMuIDco);
  // if (m_inst_lumi_bcid > 0) m_hNMuon_Inst->Fill(m_inst_lumi_bcid, m_NMuIDco);
  // if (m_inst_lumi_lb > 0) m_hNMuon_IntLumi->Fill(m_inst_lumi_lb, m_NMuIDco);
  // m_hNMuonTrack_LB->Fill(m_current_lb, m_NTrkIDco);
  // if (m_inst_lumi_bcid > 0) m_hNMuonTrack_Inst->Fill(m_inst_lumi_bcid, m_NTrkIDco);
  // if (m_inst_lumi_lb > 0) m_hNMuonTrack_IntLumi->Fill(m_inst_lumi_lb, m_NTrkIDco);
  // m_hNSegment_LB->Fill(m_current_lb, m_NSegIDco);
  // if (m_inst_lumi_bcid > 0) m_hNSegment_Inst->Fill(m_inst_lumi_bcid, m_NSegIDco);
  // if (m_inst_lumi_lb > 0) m_hNSegment_IntLumi->Fill(m_inst_lumi_lb, m_NSegIDco);
  // m_hNSegment_LB_BA->Fill(m_current_lb, m_NsegBA);
  // m_hNSegment_LB_BC->Fill(m_current_lb, m_NsegBC);
  // m_hNSegment_LB_EA->Fill(m_current_lb, m_NsegEA);
  // m_hNSegment_LB_EC->Fill(m_current_lb, m_NsegEC);
} 

