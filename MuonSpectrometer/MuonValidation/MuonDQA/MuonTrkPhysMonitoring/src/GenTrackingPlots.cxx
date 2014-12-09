/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTrkPhysMonitoring/GenTrackingPlots.h"
#include "MuonTrkPhysMonitoring/MuonTrkPhysMonUtils.h"


using namespace MuonTrkPhysMonUtils;

GenTrackingPlots::GenTrackingPlots(PlotBase *pParent):PlotBase(pParent),
m_N_Mu_LB_per_DRegion(0),m_N_HpT_pLB_CSC(0),m_N_HpT_pLB_CSC_Weights(0),m_N_HpT_pLB_CSC_Entries(0),
m_N_HpT_pLB_TGC(0),m_N_HpT_pLB_TGC_Weights(0),m_N_HpT_pLB_TGC_Entries(0),m_N_HpT_pLB_MDT(0),
m_N_HpT_pLB_MDT_Weights(0),m_N_HpT_pLB_MDT_Entries(0),m_N_HpT_pLB_RPC(0),m_N_HpT_pLB_RPC_Weights(0),
m_N_HpT_pLB_RPC_Entries(0),m_N_Mu_Lumi(0),m_N_Mu_Lumi_Run(0),m_N_Mu_Lumi_LB(0),m_N_Precision(0),
m_N_Precision_entries(0),m_N_Precision_weights(0),m_N_Precision_mean(0),m_N_Phi_Hits(0),
m_N_Phi_Hits_entries(0),m_N_Phi_Hits_weights(0),m_N_Phi_Hits_mean(0),m_N_Hits_Segment(0),
m_N_Hits_Segment_entries(0),m_N_Hits_Segment_weights(0),m_N_Hits_Segment_mean(0),m_N_Segment_Trk(0),
m_N_Segment_Trk_entries(0),m_N_Segment_Trk_weights(0),m_N_Segment_Trk_mean(0),m_t0_eta_phi(0),
m_t0_eta_phi_entries(0),m_t0_eta_phi_weights(0),m_t0_eta_phi_mean(0),m_t0_eta_phi_i(0),
m_t0_eta_phi_i_entries(0),m_t0_eta_phi_i_weights(0),m_t0_eta_phi_i_mean(0),m_t0_eta_phi_m(0),
m_t0_eta_phi_m_entries(0),m_t0_eta_phi_m_weights(0),m_t0_eta_phi_m_mean(0),m_t0_eta_phi_o(0),
m_t0_eta_phi_o_entries(0),m_t0_eta_phi_o_weights(0),m_t0_eta_phi_o_mean(0),m_Good_t0(0),m_Good_t0_EA(0),
m_Good_t0_BA(0),m_Good_t0_BC(0),m_Good_t0_EC(0),m_t0_LB_EA_entries(0),m_t0_LB_EA_weights(0),
m_t0_LB_EA_mean(0),m_t0_LB_BA_entries(0),m_t0_LB_BA_weights(0),m_t0_LB_BA_mean(0),m_t0_LB_BC_entries(0),
m_t0_LB_BC_weights(0),m_t0_LB_BC_mean(0),m_t0_LB_EC_entries(0),m_t0_LB_EC_weights(0),m_t0_LB_EC_mean(0),
m_Buffer_Heap_Protection(0){}

void GenTrackingPlots::initializePlots(){
		
  Book2D(m_N_HpT_pLB_CSC, "m_N_HpT_pLB_CSC",m_sTrackCollectionName+": <Holes/Track> / LB for CSC;Lumi Block;", 150,0.,1500.,2,0.,2.);
	Book2D(m_N_HpT_pLB_CSC_Weights, "m_N_HpT_pLB_CSC_Weights",m_sTrackCollectionName+": <Holes/Track> / LB for CSC [Sum Hits];Lumi Block;", 150,0.,1500.,2,0.,2.);
  Book2D(m_N_HpT_pLB_CSC_Entries,"m_N_HpT_pLB_CSC_Entries",m_sTrackCollectionName+": <Holes/Track> / LB for CSC [Sum Tracks];Lumi Block;", 150,0.,1500.,2,0.,2.);
  
  Book2D(m_N_HpT_pLB_TGC, "m_N_HpT_pLB_TGC",m_sTrackCollectionName+": <Holes/Track> / LB for TGC;Lumi Block;", 150,0.,1500.,2,0.,2.);
  Book2D(m_N_HpT_pLB_TGC_Weights, "m_N_HpT_pLB_TGC_Weights",m_sTrackCollectionName+": <Holes/Track> / LB for TGC [Sum Hits];Lumi Block;", 150,0.,1500.,2,0.,2.);
  Book2D(m_N_HpT_pLB_TGC_Entries, "m_N_HpT_pLB_TGC_Entries",m_sTrackCollectionName+": <Holes/Track> / LB for TGC [Sum Tracks];Lumi Block;", 150,0.,1500.,2,0.,2.);

  Book2D(m_N_HpT_pLB_RPC, "m_N_HpT_pLB_RPC",m_sTrackCollectionName+": <Holes/Track> / LB for RPC;Lumi Block;", 150,0.,1500.,2,0.,2.);
  Book2D(m_N_HpT_pLB_RPC_Weights, "m_N_HpT_pLB_RPC_Weights",m_sTrackCollectionName+": <Holes/Track> / LB for RPC [Sum Hits];Lumi Block;", 150,0.,1500.,2,0.,2.);
  Book2D(m_N_HpT_pLB_RPC_Entries, "m_N_HpT_pLB_RPC_Entries",m_sTrackCollectionName+": <Holes/Track> / LB for RPC [Sum Tracks];Lumi Block;", 150,0.,1500.,2,0.,2.);

  Book2D(m_N_HpT_pLB_MDT,                 "m_N_HpT_pLB_MDT",m_sTrackCollectionName+": <Holes/Track> / LB for MDT;Lumi Block;", 150,0.,1500.,4,0.,4.);
  Book2D(m_N_HpT_pLB_MDT_Weights, "m_N_HpT_pLB_MDT_Weights",m_sTrackCollectionName+": <Holes/Track> / LB for MDT [Sum Hits];Lumi Block;", 150,0.,1500.,4,0.,4.);
  Book2D(m_N_HpT_pLB_MDT_Entries, "m_N_HpT_pLB_MDT_Entries",m_sTrackCollectionName+": <Holes/Track> / LB for MDT [Sum Tracks];Lumi Block;", 150,0.,1500.,4,0.,4.);

  Book2D(m_N_Mu_LB_per_DRegion, "m_N_Mu_LB_per_DRegion",m_sTrackCollectionName+": Total 3x3 Extrapolated #mu's per LB per Detector #eta Region;Lumi Block Number;#eta Region", 300,0.,1500.,8,0.,8.);
  Book1D(m_N_Mu_Lumi, "m_N_Mu_Lumi",m_sTrackCollectionName+": Total 3x3 Extrapolated #mu's;;N_{#mu}", 8,0.,8.);
  Book1D(m_N_Mu_Lumi_Run, "m_N_Mu_Lumi_Run",m_sTrackCollectionName+": Total 3x3 Extrapolated #mu's N_{#mu};Run;N_{#mu}", 400,170000.0,210000.0);    
  Book1D(m_N_Mu_Lumi_LB, "m_N_Mu_Lumi_LB",m_sTrackCollectionName+": Total 3x3 Extrapolated #mu's;LumiBlock;N_{#mu}", 100,0.,1500.);
    
  Book1D(m_N_Precision, "m_N_Precision",m_sTrackCollectionName+": N_{Precision Hits}/Track;N_{Precision Hits}^{trk}", 50,0.,50);
  Book1D(m_N_Precision_entries, "m_N_Precision_entries",m_sTrackCollectionName+": N_{Precision Hits}/Track : Entries", 8,0.,8.);
  Book1D(m_N_Precision_weights, "m_N_Precision_weights",m_sTrackCollectionName+": N_{Precision Hits}/Track : Weights", 8,0.,8.);
  Book1D(m_N_Precision_mean, "m_N_Precision_mean",m_sTrackCollectionName+": <N_{Precision Hits}/Track>", 8,0.,8.);

  Book1D(m_N_Phi_Hits, "m_N_Phi_Hits",m_sTrackCollectionName+": N_{#phi Hits}/Track;N_{#phi Hits}^{trk}", 50,0.,50);
  Book1D(m_N_Phi_Hits_entries, "m_N_Phi_Hits_entries",m_sTrackCollectionName+": N_{#phi Hits}/Track : Entries", 8,0.,8.);
  Book1D(m_N_Phi_Hits_weights, "m_N_Phi_Hits_weights",m_sTrackCollectionName+": N_{#phi Hits}/Track : Weights", 8,0.,8.);
  Book1D(m_N_Phi_Hits_mean, "m_N_Phi_Hits_mean",m_sTrackCollectionName+": < N_{#phi Hits}/Track >;", 8,0.,8.);

  Book1D(m_N_Hits_Segment, "m_N_Hits_Segment",m_sTrackCollectionName+": N_{Hits}/Segment;N_{Hits}^{trk}", 50,0.,50.);
  Book1D(m_N_Hits_Segment_weights, "m_N_Hits_Segment_weights",m_sTrackCollectionName+": N_{Hits}/Segment : Weights", 8,0.,8.);
  Book1D(m_N_Hits_Segment_entries, "m_N_Hits_Segment_entries",m_sTrackCollectionName+": N_{Hits}/Segment : Entries", 8,0.,8.);
  Book1D(m_N_Hits_Segment_mean, "m_N_Hits_Segment_mean",m_sTrackCollectionName+": <N_{Hits}/Segment>", 8,0.,8.);

  Book1D(m_N_Segment_Trk, "m_N_Segment_Trk",m_sTrackCollectionName+": N_{Segments}/Track;N_{Segments}^{trk}", 50,0.,50.);    
  Book1D(m_N_Segment_Trk_weights, "m_N_Segment_Trk_weights",m_sTrackCollectionName+": N_{Segments}/Track : Weights", 8,0.,8.);
  Book1D(m_N_Segment_Trk_entries, "m_N_Segment_Trk_entries",m_sTrackCollectionName+": N_{Segments}/Track : Entries", 8,0.,8.);
  Book1D(m_N_Segment_Trk_mean, "m_N_Segment_Trk_mean",m_sTrackCollectionName+": <N_{Segments}/Track>", 8,0.,8.);
    
  Book1D(m_t0_eta_phi, "m_t0_eta_phi",m_sTrackCollectionName+": Refitted t0 - [ns]", 100,-10.0,10.0);
  Book2D(m_t0_eta_phi_entries, "m_t0_eta_phi_entries",m_sTrackCollectionName+": Refitted t0 : Entries - N_{#mu};#eta", 50,-3.0,3.0,32,1.,17.);
  Book2D(m_t0_eta_phi_weights, "m_t0_eta_phi_weights",m_sTrackCollectionName+": Refitted t0 : Weights - [ns];t0 [ns];#eta",  50,-3.0,3.0,32,1.,17.);
  Book2D(m_t0_eta_phi_mean, "m_t0_eta_phi_mean",m_sTrackCollectionName+": <Refitted t0> - [ns];#eta",  50,-3.0,3.0,32,1.,17.);
    
  Book1D(m_t0_eta_phi_i, "m_t0_eta_phi_i",m_sTrackCollectionName+": Refitted t0 - Inner Layer - [ns];t0 [ns]", 100,-10.0,10.0);
  Book2D(m_t0_eta_phi_i_entries, "m_t0_eta_phi_i_entries",m_sTrackCollectionName+": Refitted t0 - Inner Layer : Entries - N_{#mu};#eta",  50,-3.0,3.0,32,1.,17.);
  Book2D(m_t0_eta_phi_i_weights, "m_t0_eta_phi_i_weights",m_sTrackCollectionName+": Refitted t0 - Inner Layer : Weights - [ns];#eta", 50,-3.0,3.0,32,1.,17.);
  Book2D(m_t0_eta_phi_i_mean, "m_t0_eta_phi_i_mean",m_sTrackCollectionName+": <Refitted t0> - Inner Layer - [ns];#eta", 50,-3.0,3.0,32,1.,17.);
    
  Book1D(m_t0_eta_phi_m, "m_t0_eta_phi_m",m_sTrackCollectionName+": Refitted t0 - Mid Layer - [ns];t0 [ns]", 100,-10.0,10.0);
  Book2D(m_t0_eta_phi_m_entries, "m_t0_eta_phi_m_entries",m_sTrackCollectionName+": Refitted t0 - Mid Layer : Entries - N_{#mu};#eta", 50,-3.0,3.0,32,1.,17.);
  Book2D(m_t0_eta_phi_m_weights, "m_t0_eta_phi_m_weights",m_sTrackCollectionName+": Refitted t0 - Mid Layer : Weights - [ns];#eta", 50,-3.0,3.0,32,1.,17.);
  Book2D(m_t0_eta_phi_m_mean, "m_t0_eta_phi_m_mean",m_sTrackCollectionName+": <Refitted t0> - Mid Layer - [ns];#eta", 50,-3.0,3.0,32,1.,17.);
    
  Book1D(m_t0_eta_phi_o, "m_t0_eta_phi_o",m_sTrackCollectionName+": Refitted t0 - Outer Layer - [ns];t0 [ns]", 100,-10.0,10.0);    
  Book2D(m_t0_eta_phi_o_entries, "m_t0_eta_phi_o_entries",m_sTrackCollectionName+": Refitted t0 - Outer Layer : Entries - N_{#mu};#eta", 50,-3.0,3.0,32,1.,17.);    
  Book2D(m_t0_eta_phi_o_weights, "m_t0_eta_phi_o_weights",m_sTrackCollectionName+": Refitted t0 - Outer Layer : Weights - [ns];#eta", 50,-3.0,3.0,32,1.,17.);
  Book2D(m_t0_eta_phi_o_mean, "m_t0_eta_phi_o_mean",m_sTrackCollectionName+": <Refitted t0> - Outer Layer - [ns];#eta", 50,-3.0,3.0,32,1.,17.);
    
  Book1D(m_Good_t0, "m_Good_t0",m_sTrackCollectionName+":High Quality Refitted t0 per Region::<t0> [ns];Large Detector #eta Regions",4,0,4.);
  Book1D(m_Good_t0_EA, "m_Good_t0_EA",m_sTrackCollectionName+": Good Refitted t0 - EA - [ns];[ns]", 100,-10.0,10.0);
  Book1D(m_Good_t0_BA, "m_Good_t0_BA",m_sTrackCollectionName+": Good Refitted t0 - BA - [ns];[ns]", 100,-10.0,10.0);
  Book1D(m_Good_t0_BC, "m_Good_t0_BC",m_sTrackCollectionName+": Good Refitted t0 - BC - [ns];[ns]", 100,-10.0,10.0);
  Book1D(m_Good_t0_EC, "m_Good_t0_EC",m_sTrackCollectionName+": Good Refitted t0 - EC - [ns];[ns]", 100,-10.0,10.0);
	
  Book1D(m_t0_LB_EA_entries, "m_t0_LB_EA_entries",m_sTrackCollectionName+": Refitted t0 Entries/LB - EA;LumiBlock;[ns]", 300,0.0,1500.0);    
  Book1D(m_t0_LB_EA_weights, "m_t0_LB_EA_weights",m_sTrackCollectionName+": Refitted t0 Weights/LB - EA;LumiBlock;[ns]", 300,0.0,1500.0);
  Book1D(m_t0_LB_EA_mean, "m_t0_LB_EA_mean",m_sTrackCollectionName+": < Refitted t0 > /LB - EA;LumiBlock;[ns]", 300,0.0,1500.0);
    
  Book1D(m_t0_LB_BA_entries, "m_t0_LB_BA_entries",m_sTrackCollectionName+": Refitted t0 Entries/LB - BA;LumiBlock;[ns]", 300,0.0,1500.0);    
  Book1D(m_t0_LB_BA_weights, "m_t0_LB_BA_weights",m_sTrackCollectionName+": Refitted t0 Weights/LB - BA;LumiBlock;[ns]", 300,0.0,1500.0);
  Book1D(m_t0_LB_BA_mean, "m_t0_LB_BA_mean",m_sTrackCollectionName+": < Refitted t0 > /LB - BA;LumiBlock;[ns]", 300,0.0,1500.0);
    
  Book1D(m_t0_LB_BC_entries, "m_t0_LB_BC_entries",m_sTrackCollectionName+": Refitted t0 Entries/LB - BC;LumiBlock;[ns]", 300,0.0,1500.0);
  Book1D(m_t0_LB_BC_weights, "m_t0_LB_BC_weights",m_sTrackCollectionName+": Refitted t0 Weights/LB - BC;LumiBlock;[ns]", 300,0.0,1500.0);
  Book1D(m_t0_LB_BC_mean, "m_t0_LB_BC_mean",m_sTrackCollectionName+": < Refitted t0 > /LB - BC;LumiBlock;[ns]", 300,0.0,1500.0);
    
  Book1D(m_t0_LB_EC_entries, "m_t0_LB_EC_entries",m_sTrackCollectionName+": Refitted t0 Entries/LB - EC;LumiBlock;[ns]", 300,0.0,1500.0);
  Book1D(m_t0_LB_EC_weights, "m_t0_LB_EC_weights",m_sTrackCollectionName+": Refitted t0 Weights/LB - EC;LumiBlock;[ns]", 300,0.0,1500.0);
  Book1D(m_t0_LB_EC_mean, "m_t0_LB_EC_mean",m_sTrackCollectionName+": < Refitted t0 > / LB - EC;LumiBlock;[ns]", 300,0.0,1500.0);
  Book1D(m_Buffer_Heap_Protection, "m_Buffer_Heap_Protection",m_sTrackCollectionName+": m_Buffer_Heap_Protection", 300,-1500.0,1500.0);

}

void GenTrackingPlots::finalizePlots(){

  FinalizeGoodt0Results(m_Good_t0_EC, 1, "Endcap C");
  FinalizeGoodt0Results(m_Good_t0_BC, 2, "Barrel C");
  FinalizeGoodt0Results(m_Good_t0_BA, 3, "Barrel A");
  FinalizeGoodt0Results(m_Good_t0_EA, 4, "Endcap A");

  FinalizeT0Plots(m_t0_eta_phi_i_weights, m_t0_eta_phi_i_entries, m_t0_eta_phi_i_mean);
  FinalizeT0Plots(m_t0_eta_phi_m_weights, m_t0_eta_phi_m_entries, m_t0_eta_phi_m_mean);
  FinalizeT0Plots(m_t0_eta_phi_o_weights, m_t0_eta_phi_o_entries, m_t0_eta_phi_o_mean);
  FinalizeT0Plots(m_t0_eta_phi_weights, m_t0_eta_phi_entries, m_t0_eta_phi_mean);
                    
  m_t0_LB_EA_mean->Divide(m_t0_LB_EA_weights,m_t0_LB_EA_entries);
  m_t0_LB_BA_mean->Divide(m_t0_LB_BA_weights,m_t0_LB_BA_entries);
  m_t0_LB_BC_mean->Divide(m_t0_LB_BC_weights,m_t0_LB_BC_entries);
  m_t0_LB_EC_mean->Divide(m_t0_LB_EC_weights,m_t0_LB_EC_entries);
              
  SetLabels(m_N_HpT_pLB_CSC, m_N_HpT_pLB_CSC_Entries, m_N_HpT_pLB_CSC_Weights, {{MTPM_Location_Bins::CSC_C,"CSC Side C"}, {MTPM_Location_Bins::CSC_A,"CSC Side A"}});
  SetLabels(m_N_HpT_pLB_TGC, m_N_HpT_pLB_TGC_Entries, m_N_HpT_pLB_TGC_Weights, {{MTPM_Location_Bins::TGC_C,"TGC Side C"}, {MTPM_Location_Bins::TGC_A,"TGC Side A"}});
  SetLabels(m_N_HpT_pLB_RPC, m_N_HpT_pLB_RPC_Entries, m_N_HpT_pLB_RPC_Weights, {{MTPM_Location_Bins::RPC_C,"RPC Side C"}, {MTPM_Location_Bins::RPC_A,"RPC Side A"}});
  SetLabels(m_N_HpT_pLB_MDT, m_N_HpT_pLB_MDT_Entries, m_N_HpT_pLB_MDT_Weights, {{MTPM_Location_Bins::MDT_EC,"MDT Endcap C"},{MTPM_Location_Bins::MDT_BC,"MDT Barrel C"},
                                                                                {MTPM_Location_Bins::MDT_BA,"MDT Barrel A"}, {MTPM_Location_Bins::MDT_EA,"MDT Endcap A"}} );
      
  m_N_HpT_pLB_CSC->Divide(m_N_HpT_pLB_CSC_Weights , m_N_HpT_pLB_CSC_Entries);
  m_N_HpT_pLB_TGC->Divide(m_N_HpT_pLB_TGC_Weights , m_N_HpT_pLB_TGC_Entries);
  m_N_HpT_pLB_RPC->Divide(m_N_HpT_pLB_RPC_Weights , m_N_HpT_pLB_RPC_Entries);
  m_N_HpT_pLB_MDT->Divide(m_N_HpT_pLB_MDT_Weights , m_N_HpT_pLB_MDT_Entries);
      
  LabelLargeEtaRegions(m_N_Mu_LB_per_DRegion->GetYaxis());
  LabelLargeEtaRegions(m_N_Mu_Lumi->GetXaxis());
  
  FinalizeMeanHistograms(m_N_Precision, m_N_Precision_entries, m_N_Precision_weights, m_N_Precision_mean);
  FinalizeMeanHistograms(m_N_Phi_Hits, m_N_Phi_Hits_entries, m_N_Phi_Hits_weights, m_N_Phi_Hits_mean);
  FinalizeMeanHistograms(m_N_Precision, m_N_Precision_entries, m_N_Precision_weights, m_N_Precision_mean);
  FinalizeMeanHistograms(m_N_Hits_Segment, m_N_Hits_Segment_entries, m_N_Hits_Segment_weights, m_N_Hits_Segment_mean);
  FinalizeMeanHistograms(m_N_Segment_Trk, m_N_Segment_Trk_entries, m_N_Segment_Trk_weights, m_N_Segment_Trk_mean);
      
}

void GenTrackingPlots::FinalizeGoodt0Results(TH1F* hist, int iBin, std::string sLabel){
  m_Good_t0->SetBinContent(iBin,hist->GetMean(1));
  m_Good_t0->SetBinError(  iBin,hist->GetEntries() ? hist->GetRMS(1)/sqrt(hist->GetEntries()) : 0.0);
  m_Good_t0->GetXaxis()->SetBinLabel(  iBin,sLabel.c_str());
}

void GenTrackingPlots::FinalizeT0Plots(TH2F* hist_weights, TH2F* hist_entries, TH2F* hist_mean){
  LabelSectorAxis(hist_weights->GetYaxis());
  LabelSectorAxis(hist_entries->GetYaxis());
  LabelSectorAxis(hist_mean->GetYaxis());
  hist_mean->Divide(hist_weights,hist_entries);
  
}

void GenTrackingPlots::FinalizeMeanHistograms(TH1F* hist_overall, TH1F* hist_entries, TH1F* hist_weights, TH1F* hist_mean){
  LabelLargeEtaRegions(hist_mean->GetXaxis());
  LabelLargeEtaRegions(hist_entries->GetXaxis());
  LabelLargeEtaRegions(hist_weights->GetXaxis());
  hist_mean->Divide(hist_weights,hist_entries);
  hist_overall->SetBit(TH1::kIsAverage);
  for (int j = 1; j < hist_mean->GetNbinsX()+1; j++) 
    hist_mean->SetBinError(j,hist_entries->GetBinContent(j) ? hist_overall->GetRMS(1)/sqrt(hist_entries->GetBinContent(j)) : 0.0);
}

void GenTrackingPlots::SetLabels(TH2F* hist,TH2F* hist_weights,TH2F* hist_entries, std::vector<std::pair<int,std::string>> vLabels){
  for (auto& entry: vLabels ){
    hist->GetYaxis()->SetBinLabel(entry.first, entry.second.c_str());
    hist_weights->GetYaxis()->SetBinLabel(entry.first, entry.second.c_str());
    hist_entries->GetYaxis()->SetBinLabel(entry.first, entry.second.c_str());
  }
}
