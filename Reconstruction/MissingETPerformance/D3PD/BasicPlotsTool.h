/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include <string>

#include "TH1.h"
#include "TH2.h"
#include "TFile.h"

#include "MissingETData.h"

class BasicPlotsTool {

public:
  BasicPlotsTool();
  BasicPlotsTool(const BasicPlotsTool &other);
  ~BasicPlotsTool() {}
  void initialize(TFile *);

  void execute(MissingETData *data);
  void basicPlots(MissingETData *data);
  std::string m_folderName;

  //parameters for histograms
  int m_iset;
  int m_nbinp;
  float m_suptmi;
  float m_suptmi_part;
  int m_nbinE;
  float m_lowET;
  float m_suET;
  int m_nbinpxy;
  float m_suptmixy;
  int m_nbinphi;
  float m_binphi;
  int m_nbinsig;
  float m_binsig;
  int m_nbineta;
  float m_bineta;

  //histograms
  TH1F *vertex_x;
  TH1F *vertex_y;
  TH1F *vertex_z;
  TH1F *h_nVertex;
  TH2D *h_metLvsptjet;
  TH2D *h_metEFLvsptjet;

  TH2D *h_MET_Topo_vs_MET_LocHadTopo;
  TH2D *h_MET_Topo_vs_MET_CorrTopo;
  TH2D *h_MET_Topo_vs_MET_LCW_NI_pt20_noSoftJets_eflow;
  TH2D *h_MET_CellOut_vs_MET_Eflow;
  
  TH1F *h_mT;
  //Topo & Corr Topo &LocHadTopo
  TH1D *h_CorrTopo_METphi;
  TH1D *h_CorrTopo_METx;
  TH1D *h_CorrTopo_METy;
  TH1D *h_CorrTopo_MET;
  TH1D *h_CorrTopo_SET;
  TH2D *h_CorrTopo_MET_v_METphi;

  TH1D *h_Topo_METphi;
  TH1D *h_Topo_METx;
  TH1D *h_Topo_METy;
  TH1D *h_Topo_MET;
  TH1D *h_Topo_SET;
  TH2D *h_Topo_MET_v_METphi;
  
  TH1D *h_LocHadTopo_METphi;
  TH1D *h_LocHadTopo_METx;
  TH1D *h_LocHadTopo_METy;
  TH1D *h_LocHadTopo_MET;
  TH1D *h_LocHadTopo_SET;
  TH2D *h_LocHadTopo_MET_v_METphi;
 
  
  //RefFinal standard and its components
  
  TH1D *h_RefFinal_METphi;
  TH1D *h_RefFinal_METx;
  TH1D *h_RefFinal_METy;
  TH1D *h_RefFinal_MET;
  TH1D *h_RefFinal_SET;
  TH2D *h_RefFinal_MET_v_METphi;

  TH1D *h_RefEle_METphi;
  TH1D *h_RefEle_METx;
  TH1D *h_RefEle_METy;
  TH1D *h_RefEle_MET;
  TH1D *h_RefEle_SET;
  TH2D *h_RefEle_MET_v_METphi;

  TH1D *h_RefJet_METphi;
  TH1D *h_RefJet_METx;
  TH1D *h_RefJet_METy;
  TH1D *h_RefJet_MET;
  TH1D *h_RefJet_SET;
  TH2D *h_RefJet_MET_v_METphi;

  TH1D *h_SoftJets_METphi;
  TH1D *h_SoftJets_METx;
  TH1D *h_SoftJets_METy;
  TH1D *h_SoftJets_MET;
  TH1D *h_SoftJets_SET;
  TH2D *h_SoftJets_MET_v_METphi;

  TH1D *h_MuonBoy_METphi;
  TH1D *h_MuonBoy_METx;
  TH1D *h_MuonBoy_METy;
  TH1D *h_MuonBoy_MET;
  TH1D *h_MuonBoy_SET;
  TH2D *h_MuonBoy_MET_v_METphi;

  TH1D *h_RefGamma_METphi;
  TH1D *h_RefGamma_METx;
  TH1D *h_RefGamma_METy;
  TH1D *h_RefGamma_MET;
  TH1D *h_RefGamma_SET;
  TH2D *h_RefGamma_MET_v_METphi;

  TH1D *h_RefTau_METphi;
  TH1D *h_RefTau_METx;
  TH1D *h_RefTau_METy;
  TH1D *h_RefTau_MET;
  TH1D *h_RefTau_SET;
  TH2D *h_RefTau_MET_v_METphi;

  TH1D *h_RefMuon_METphi;
  TH1D *h_RefMuon_METx;
  TH1D *h_RefMuon_METy;
  TH1D *h_RefMuon_MET;
  TH1D *h_RefMuon_SET;
 
  TH1D *h_CellOut_METphi;
  TH1D *h_CellOut_METx;
  TH1D *h_CellOut_METy;
  TH1D *h_CellOut_MET;
  TH1D *h_CellOut_SET;
  TH2D *h_CellOut_MET_v_METphi;
 
  TH1D *h_CellOut_Eflow_METphi;
  TH1D *h_CellOut_Eflow_METx;
  TH1D *h_CellOut_Eflow_METy;
  TH1D *h_CellOut_Eflow_MET;
  TH1D *h_CellOut_Eflow_SET;
  TH2D *h_CellOut_Eflow_MET_v_METphi;
  
  TH1D *h_Cryo_METphi;
  TH1D *h_Cryo_METx;
  TH1D *h_Cryo_METy;
  TH1D *h_Cryo_MET;
  TH1D *h_Cryo_SET;
  TH2D *h_Cryo_MET_v_METphi;

// RefFinal_em

  TH1D *h_RefFinal_em_METphi;
  TH1D *h_RefFinal_em_METx;
  TH1D *h_RefFinal_em_METy;
  TH1D *h_RefFinal_em_MET;
  TH1D *h_RefFinal_em_SET;
  TH2D *h_RefFinal_em_MET_v_em_METphi;

  TH1D *h_RefEle_em_METphi;
  TH1D *h_RefEle_em_METx;
  TH1D *h_RefEle_em_METy;
  TH1D *h_RefEle_em_MET;
  TH1D *h_RefEle_em_SET;
  TH2D *h_RefEle_em_MET_v_em_METphi;

  TH1D *h_RefJet_em_METphi;
  TH1D *h_RefJet_em_METx;
  TH1D *h_RefJet_em_METy;
  TH1D *h_RefJet_em_MET;
  TH1D *h_RefJet_em_SET;
  TH2D *h_RefJet_em_MET_v_em_METphi;

  TH1D *h_RefGamma_em_METphi;
  TH1D *h_RefGamma_em_METx;
  TH1D *h_RefGamma_em_METy;
  TH1D *h_RefGamma_em_MET;
  TH1D *h_RefGamma_em_SET;
  TH2D *h_RefGamma_em_MET_v_em_METphi;

  TH1D *h_RefTau_em_METphi;
  TH1D *h_RefTau_em_METx;
  TH1D *h_RefTau_em_METy;
  TH1D *h_RefTau_em_MET;
  TH1D *h_RefTau_em_SET;
  TH2D *h_RefTau_em_MET_v_em_METphi;

  TH1D *h_CellOut_em_METphi;
  TH1D *h_CellOut_em_METx;
  TH1D *h_CellOut_em_METy;
  TH1D *h_CellOut_em_MET;
  TH1D *h_CellOut_em_SET;
  TH2D *h_CellOut_em_MET_v_em_METphi;

  TH1D *h_RefMuon_em_METphi;
  TH1D *h_RefMuon_em_METx;
  TH1D *h_RefMuon_em_METy;
  TH1D *h_RefMuon_em_MET;
  TH1D *h_RefMuon_em_SET;
 
  
//GCW_pt20
  
  TH1D *h_RefFinal_GCW_pt20_METphi;
  TH1D *h_RefFinal_GCW_pt20_METx;
  TH1D *h_RefFinal_GCW_pt20_METy;
  TH1D *h_RefFinal_GCW_pt20_MET;
  TH1D *h_RefFinal_GCW_pt20_SET;
  TH2D *h_RefFinal_GCW_pt20_MET_v_GCW_pt20_METphi;

  TH1D *h_RefEle_GCW_pt20_METphi;
  TH1D *h_RefEle_GCW_pt20_METx;
  TH1D *h_RefEle_GCW_pt20_METy;
  TH1D *h_RefEle_GCW_pt20_MET;
  TH1D *h_RefEle_GCW_pt20_SET;
  TH2D *h_RefEle_GCW_pt20_MET_v_GCW_pt20_METphi;

  TH1D *h_RefJet_GCW_pt20_METphi;
  TH1D *h_RefJet_GCW_pt20_METx;
  TH1D *h_RefJet_GCW_pt20_METy;
  TH1D *h_RefJet_GCW_pt20_MET;
  TH1D *h_RefJet_GCW_pt20_SET;
  TH2D *h_RefJet_GCW_pt20_MET_v_GCW_pt20_METphi;
  
  TH1D *h_RefJet_old_GCW_pt20_METphi;
  TH1D *h_RefJet_old_GCW_pt20_METx;
  TH1D *h_RefJet_old_GCW_pt20_METy;
  TH1D *h_RefJet_old_GCW_pt20_MET;
  TH1D *h_RefJet_old_GCW_pt20_SET;
  //TH2D *h_RefJet_old_GCW_pt20_MET_v_GCW_pt20_METphi;

  TH1D *h_SoftJets_GCW_pt20_METphi;
  TH1D *h_SoftJets_GCW_pt20_METx;
  TH1D *h_SoftJets_GCW_pt20_METy;
  TH1D *h_SoftJets_GCW_pt20_MET;
  TH1D *h_SoftJets_GCW_pt20_SET;
  TH2D *h_SoftJets_GCW_pt20_MET_v_GCW_pt20_METphi;

  TH1D *h_RefMuon_Staco_GCW_pt20_METphi;
  TH1D *h_RefMuon_Staco_GCW_pt20_METx;
  TH1D *h_RefMuon_Staco_GCW_pt20_METy;
  TH1D *h_RefMuon_Staco_GCW_pt20_MET;
  TH1D *h_RefMuon_Staco_GCW_pt20_SET;
  TH2D *h_RefMuon_Staco_GCW_pt20_MET_v_GCW_pt20_METphi;
  
  TH1D *h_RefMuon_Track_Staco_GCW_pt20_METphi;
  TH1D *h_RefMuon_Track_Staco_GCW_pt20_METx;
  TH1D *h_RefMuon_Track_Staco_GCW_pt20_METy;
  TH1D *h_RefMuon_Track_Staco_GCW_pt20_MET;
  TH1D *h_RefMuon_Track_Staco_GCW_pt20_SET;
  TH2D *h_RefMuon_Track_Staco_GCW_pt20_MET_v_GCW_pt20_METphi;
  
  TH1D *h_Muon_Isol_Staco_GCW_pt20_METphi;
  TH1D *h_Muon_Isol_Staco_GCW_pt20_METx;
  TH1D *h_Muon_Isol_Staco_GCW_pt20_METy;
  TH1D *h_Muon_Isol_Staco_GCW_pt20_MET;
  TH1D *h_Muon_Isol_Staco_GCW_pt20_SET;
  TH2D *h_Muon_Isol_Staco_GCW_pt20_MET_v_GCW_pt20_METphi;

  TH1D *h_Muon_NonIsol_Staco_GCW_pt20_METphi;
  TH1D *h_Muon_NonIsol_Staco_GCW_pt20_METx;
  TH1D *h_Muon_NonIsol_Staco_GCW_pt20_METy;
  TH1D *h_Muon_NonIsol_Staco_GCW_pt20_MET;
  TH1D *h_Muon_NonIsol_Staco_GCW_pt20_SET;
  TH2D *h_Muon_NonIsol_Staco_GCW_pt20_MET_v_GCW_pt20_METphi;

  TH1D *h_Muon_Total_Staco_GCW_pt20_METphi;
  TH1D *h_Muon_Total_Staco_GCW_pt20_METx;
  TH1D *h_Muon_Total_Staco_GCW_pt20_METy;
  TH1D *h_Muon_Total_Staco_GCW_pt20_MET;
  TH1D *h_Muon_Total_Staco_GCW_pt20_SET;
  TH2D *h_Muon_Total_Staco_GCW_pt20_MET_v_GCW_pt20_METphi;

  TH1D *h_RefGamma_GCW_pt20_METphi;
  TH1D *h_RefGamma_GCW_pt20_METx;
  TH1D *h_RefGamma_GCW_pt20_METy;
  TH1D *h_RefGamma_GCW_pt20_MET;
  TH1D *h_RefGamma_GCW_pt20_SET;
  TH2D *h_RefGamma_GCW_pt20_MET_v_GCW_pt20_METphi;

  TH1D *h_RefTau_GCW_pt20_METphi;
  TH1D *h_RefTau_GCW_pt20_METx;
  TH1D *h_RefTau_GCW_pt20_METy;
  TH1D *h_RefTau_GCW_pt20_MET;
  TH1D *h_RefTau_GCW_pt20_SET;
  TH2D *h_RefTau_GCW_pt20_MET_v_GCW_pt20_METphi;

  TH1D *h_CellOut_GCW_pt20_METphi;
  TH1D *h_CellOut_GCW_pt20_METx;
  TH1D *h_CellOut_GCW_pt20_METy;
  TH1D *h_CellOut_GCW_pt20_MET;
  TH1D *h_CellOut_GCW_pt20_SET;
  TH2D *h_CellOut_GCW_pt20_MET_v_GCW_pt20_METphi;
  
  TH1D *h_Cryo_GCW_pt20_METphi;
  TH1D *h_Cryo_GCW_pt20_METx;
  TH1D *h_Cryo_GCW_pt20_METy;
  TH1D *h_Cryo_GCW_pt20_MET;
  TH1D *h_Cryo_GCW_pt20_SET;
  TH2D *h_Cryo_GCW_pt20_MET_v_GCW_pt20_METphi;



//LCW_pt20

  
  TH1D *h_RefFinal_LCW_pt20_METphi;
  TH1D *h_RefFinal_LCW_pt20_METx;
  TH1D *h_RefFinal_LCW_pt20_METy;
  TH1D *h_RefFinal_LCW_pt20_MET;
  TH1D *h_RefFinal_LCW_pt20_SET;
  TH2D *h_RefFinal_LCW_pt20_MET_v_LCW_pt20_METphi;

  TH1D *h_RefEle_LCW_pt20_METphi;
  TH1D *h_RefEle_LCW_pt20_METx;
  TH1D *h_RefEle_LCW_pt20_METy;
  TH1D *h_RefEle_LCW_pt20_MET;
  TH1D *h_RefEle_LCW_pt20_SET;
  TH2D *h_RefEle_LCW_pt20_MET_v_LCW_pt20_METphi;

  TH1D *h_RefJet_LCW_pt20_METphi;
  TH1D *h_RefJet_LCW_pt20_METx;
  TH1D *h_RefJet_LCW_pt20_METy;
  TH1D *h_RefJet_LCW_pt20_MET;
  TH1D *h_RefJet_LCW_pt20_SET;
  TH2D *h_RefJet_LCW_pt20_MET_v_LCW_pt20_METphi;
  
  TH1D *h_RefJet_old_LCW_pt20_METphi;
  TH1D *h_RefJet_old_LCW_pt20_METx;
  TH1D *h_RefJet_old_LCW_pt20_METy;
  TH1D *h_RefJet_old_LCW_pt20_MET;
  TH1D *h_RefJet_old_LCW_pt20_SET;
  TH2D *h_RefJet_old_LCW_pt20_MET_v_LCW_pt20_METphi;
  
  TH1D *h_SoftJets_LCW_pt20_METphi;
  TH1D *h_SoftJets_LCW_pt20_METx;
  TH1D *h_SoftJets_LCW_pt20_METy;
  TH1D *h_SoftJets_LCW_pt20_MET;
  TH1D *h_SoftJets_LCW_pt20_SET;
  TH2D *h_SoftJets_LCW_pt20_MET_v_LCW_pt20_METphi;

  TH1D *h_RefMuon_Staco_LCW_pt20_METphi;
  TH1D *h_RefMuon_Staco_LCW_pt20_METx;
  TH1D *h_RefMuon_Staco_LCW_pt20_METy;
  TH1D *h_RefMuon_Staco_LCW_pt20_MET;
  TH1D *h_RefMuon_Staco_LCW_pt20_SET;
  TH2D *h_RefMuon_Staco_LCW_pt20_MET_v_LCW_pt20_METphi;
  
  TH1D *h_RefMuon_Track_Staco_LCW_pt20_METphi;
  TH1D *h_RefMuon_Track_Staco_LCW_pt20_METx;
  TH1D *h_RefMuon_Track_Staco_LCW_pt20_METy;
  TH1D *h_RefMuon_Track_Staco_LCW_pt20_MET;
  TH1D *h_RefMuon_Track_Staco_LCW_pt20_SET;
  TH2D *h_RefMuon_Track_Staco_LCW_pt20_MET_v_LCW_pt20_METphi;
  
  TH1D *h_Muon_Isol_Staco_LCW_pt20_METphi;
  TH1D *h_Muon_Isol_Staco_LCW_pt20_METx;
  TH1D *h_Muon_Isol_Staco_LCW_pt20_METy;
  TH1D *h_Muon_Isol_Staco_LCW_pt20_MET;
  TH1D *h_Muon_Isol_Staco_LCW_pt20_SET;
  TH2D *h_Muon_Isol_Staco_LCW_pt20_MET_v_LCW_pt20_METphi;

  TH1D *h_Muon_NonIsol_Staco_LCW_pt20_METphi;
  TH1D *h_Muon_NonIsol_Staco_LCW_pt20_METx;
  TH1D *h_Muon_NonIsol_Staco_LCW_pt20_METy;
  TH1D *h_Muon_NonIsol_Staco_LCW_pt20_MET;
  TH1D *h_Muon_NonIsol_Staco_LCW_pt20_SET;
  TH2D *h_Muon_NonIsol_Staco_LCW_pt20_MET_v_LCW_pt20_METphi;

  TH1D *h_Muon_Total_Staco_LCW_pt20_METphi;
  TH1D *h_Muon_Total_Staco_LCW_pt20_METx;
  TH1D *h_Muon_Total_Staco_LCW_pt20_METy;
  TH1D *h_Muon_Total_Staco_LCW_pt20_MET;
  TH1D *h_Muon_Total_Staco_LCW_pt20_SET;
  TH2D *h_Muon_Total_Staco_LCW_pt20_MET_v_LCW_pt20_METphi;

  TH1D *h_RefGamma_LCW_pt20_METphi;
  TH1D *h_RefGamma_LCW_pt20_METx;
  TH1D *h_RefGamma_LCW_pt20_METy;
  TH1D *h_RefGamma_LCW_pt20_MET;
  TH1D *h_RefGamma_LCW_pt20_SET;
  TH2D *h_RefGamma_LCW_pt20_MET_v_LCW_pt20_METphi;

  TH1D *h_RefTau_LCW_pt20_METphi;
  TH1D *h_RefTau_LCW_pt20_METx;
  TH1D *h_RefTau_LCW_pt20_METy;
  TH1D *h_RefTau_LCW_pt20_MET;
  TH1D *h_RefTau_LCW_pt20_SET;
  TH2D *h_RefTau_LCW_pt20_MET_v_LCW_pt20_METphi;

  TH1D *h_CellOut_LCW_pt20_METphi;
  TH1D *h_CellOut_LCW_pt20_METx;
  TH1D *h_CellOut_LCW_pt20_METy;
  TH1D *h_CellOut_LCW_pt20_MET;
  TH1D *h_CellOut_LCW_pt20_SET;
  TH2D *h_CellOut_LCW_pt20_MET_v_LCW_pt20_METphi;
  
  TH1D *h_Cryo_LCW_pt20_METphi;
  TH1D *h_Cryo_LCW_pt20_METx;
  TH1D *h_Cryo_LCW_pt20_METy;
  TH1D *h_Cryo_LCW_pt20_MET;
  TH1D *h_Cryo_LCW_pt20_SET;
  TH2D *h_Cryo_LCW_pt20_MET_v_LCW_pt20_METphi;

//LCW_NI_eflow4
  TH1D *h_RefFinal_LCW_NI_eflow4_METphi;
  TH1D *h_RefFinal_LCW_NI_eflow4_METx;
  TH1D *h_RefFinal_LCW_NI_eflow4_METy;
  TH1D *h_RefFinal_LCW_NI_eflow4_MET;
  TH1D *h_RefFinal_LCW_NI_eflow4_SET;
  TH2D *h_RefFinal_LCW_NI_eflow4_MET_v_LCW_NI_eflow4_METphi;

  TH1D *h_RefEle_LCW_NI_eflow4_METphi;
  TH1D *h_RefEle_LCW_NI_eflow4_METx;
  TH1D *h_RefEle_LCW_NI_eflow4_METy;
  TH1D *h_RefEle_LCW_NI_eflow4_MET;
  TH1D *h_RefEle_LCW_NI_eflow4_SET;
  TH2D *h_RefEle_LCW_NI_eflow4_MET_v_LCW_NI_eflow4_METphi;

  TH1D *h_RefJet_LCW_NI_eflow4_METphi;
  TH1D *h_RefJet_LCW_NI_eflow4_METx;
  TH1D *h_RefJet_LCW_NI_eflow4_METy;
  TH1D *h_RefJet_LCW_NI_eflow4_MET;
  TH1D *h_RefJet_LCW_NI_eflow4_SET;
  TH2D *h_RefJet_LCW_NI_eflow4_MET_v_LCW_NI_eflow4_METphi;
  
  TH1D *h_RefJet_old_LCW_NI_eflow4_METphi;
  TH1D *h_RefJet_old_LCW_NI_eflow4_METx;
  TH1D *h_RefJet_old_LCW_NI_eflow4_METy;
  TH1D *h_RefJet_old_LCW_NI_eflow4_MET;
  TH1D *h_RefJet_old_LCW_NI_eflow4_SET;
  TH2D *h_RefJet_old_LCW_NI_eflow4_MET_v_LCW_NI_eflow4_METphi;
 
  TH1D *h_SoftJets_LCW_NI_eflow4_METphi;
  TH1D *h_SoftJets_LCW_NI_eflow4_METx;
  TH1D *h_SoftJets_LCW_NI_eflow4_METy;
  TH1D *h_SoftJets_LCW_NI_eflow4_MET;
  TH1D *h_SoftJets_LCW_NI_eflow4_SET;
  TH2D *h_SoftJets_LCW_NI_eflow4_MET_v_LCW_NI_eflow4_METphi;

  TH1D *h_RefMuon_Muid_LCW_NI_eflow4_METphi;
  TH1D *h_RefMuon_Muid_LCW_NI_eflow4_METx;
  TH1D *h_RefMuon_Muid_LCW_NI_eflow4_METy;
  TH1D *h_RefMuon_Muid_LCW_NI_eflow4_MET;
  TH1D *h_RefMuon_Muid_LCW_NI_eflow4_SET;
  TH2D *h_RefMuon_Muid_LCW_NI_eflow4_MET_v_LCW_NI_eflow4_METphi;
  
  TH1D *h_RefMuon_Track_Muid_LCW_NI_eflow4_METphi;
  TH1D *h_RefMuon_Track_Muid_LCW_NI_eflow4_METx;
  TH1D *h_RefMuon_Track_Muid_LCW_NI_eflow4_METy;
  TH1D *h_RefMuon_Track_Muid_LCW_NI_eflow4_MET;
  TH1D *h_RefMuon_Track_Muid_LCW_NI_eflow4_SET;
  TH2D *h_RefMuon_Track_Muid_LCW_NI_eflow4_MET_v_LCW_NI_eflow4_METphi;
  
  TH1D *h_Muon_Isol_Muid_LCW_NI_eflow4_METphi;
  TH1D *h_Muon_Isol_Muid_LCW_NI_eflow4_METx;
  TH1D *h_Muon_Isol_Muid_LCW_NI_eflow4_METy;
  TH1D *h_Muon_Isol_Muid_LCW_NI_eflow4_MET;
  TH1D *h_Muon_Isol_Muid_LCW_NI_eflow4_SET;
  TH2D *h_Muon_Isol_Muid_LCW_NI_eflow4_MET_v_LCW_NI_eflow4_METphi;

  TH1D *h_Muon_NonIsol_Muid_LCW_NI_eflow4_METphi;
  TH1D *h_Muon_NonIsol_Muid_LCW_NI_eflow4_METx;
  TH1D *h_Muon_NonIsol_Muid_LCW_NI_eflow4_METy;
  TH1D *h_Muon_NonIsol_Muid_LCW_NI_eflow4_MET;
  TH1D *h_Muon_NonIsol_Muid_LCW_NI_eflow4_SET;
  TH2D *h_Muon_NonIsol_Muid_LCW_NI_eflow4_MET_v_LCW_NI_eflow4_METphi;

  TH1D *h_Muon_Total_Muid_LCW_NI_eflow4_METphi;
  TH1D *h_Muon_Total_Muid_LCW_NI_eflow4_METx;
  TH1D *h_Muon_Total_Muid_LCW_NI_eflow4_METy;
  TH1D *h_Muon_Total_Muid_LCW_NI_eflow4_MET;
  TH1D *h_Muon_Total_Muid_LCW_NI_eflow4_SET;
  TH2D *h_Muon_Total_Muid_LCW_NI_eflow4_MET_v_LCW_NI_eflow4_METphi;

  TH1D *h_RefGamma_LCW_NI_eflow4_METphi;
  TH1D *h_RefGamma_LCW_NI_eflow4_METx;
  TH1D *h_RefGamma_LCW_NI_eflow4_METy;
  TH1D *h_RefGamma_LCW_NI_eflow4_MET;
  TH1D *h_RefGamma_LCW_NI_eflow4_SET;
  TH2D *h_RefGamma_LCW_NI_eflow4_MET_v_LCW_NI_eflow4_METphi;

  TH1D *h_RefTau_LCW_NI_eflow4_METphi;
  TH1D *h_RefTau_LCW_NI_eflow4_METx;
  TH1D *h_RefTau_LCW_NI_eflow4_METy;
  TH1D *h_RefTau_LCW_NI_eflow4_MET;
  TH1D *h_RefTau_LCW_NI_eflow4_SET;
  TH2D *h_RefTau_LCW_NI_eflow4_MET_v_LCW_NI_eflow4_METphi;

  TH1D *h_CellOut_LCW_NI_eflow4_METphi;
  TH1D *h_CellOut_LCW_NI_eflow4_METx;
  TH1D *h_CellOut_LCW_NI_eflow4_METy;
  TH1D *h_CellOut_LCW_NI_eflow4_MET;
  TH1D *h_CellOut_LCW_NI_eflow4_SET;
  TH2D *h_CellOut_LCW_NI_eflow4_MET_v_LCW_NI_eflow4_METphi;
  
  TH1D *h_Cryo_LCW_NI_eflow4_METphi;
  TH1D *h_Cryo_LCW_NI_eflow4_METx;
  TH1D *h_Cryo_LCW_NI_eflow4_METy;
  TH1D *h_Cryo_LCW_NI_eflow4_MET;
  TH1D *h_Cryo_LCW_NI_eflow4_SET;
  TH2D *h_Cryo_LCW_NI_eflow4_MET_v_LCW_NI_eflow4_METphi;
  
//LCW_NI_eflow_400

  TH1D *h_RefFinal_LCW_NI_eflow_400_METphi;
  TH1D *h_RefFinal_LCW_NI_eflow_400_METx;
  TH1D *h_RefFinal_LCW_NI_eflow_400_METy;
  TH1D *h_RefFinal_LCW_NI_eflow_400_MET;
  TH1D *h_RefFinal_LCW_NI_eflow_400_SET;
  TH2D *h_RefFinal_LCW_NI_eflow_400_MET_v_LCW_NI_eflow_400_METphi;

  TH1D *h_RefEle_LCW_NI_eflow_400_METphi;
  TH1D *h_RefEle_LCW_NI_eflow_400_METx;
  TH1D *h_RefEle_LCW_NI_eflow_400_METy;
  TH1D *h_RefEle_LCW_NI_eflow_400_MET;
  TH1D *h_RefEle_LCW_NI_eflow_400_SET;
  TH2D *h_RefEle_LCW_NI_eflow_400_MET_v_LCW_NI_eflow_400_METphi;

  TH1D *h_RefJet_LCW_NI_eflow_400_METphi;
  TH1D *h_RefJet_LCW_NI_eflow_400_METx;
  TH1D *h_RefJet_LCW_NI_eflow_400_METy;
  TH1D *h_RefJet_LCW_NI_eflow_400_MET;
  TH1D *h_RefJet_LCW_NI_eflow_400_SET;
  TH2D *h_RefJet_LCW_NI_eflow_400_MET_v_LCW_NI_eflow_400_METphi;
  
  TH1D *h_RefJet_old_LCW_NI_eflow_400_METphi;
  TH1D *h_RefJet_old_LCW_NI_eflow_400_METx;
  TH1D *h_RefJet_old_LCW_NI_eflow_400_METy;
  TH1D *h_RefJet_old_LCW_NI_eflow_400_MET;
  TH1D *h_RefJet_old_LCW_NI_eflow_400_SET;
  TH2D *h_RefJet_old_LCW_NI_eflow_400_MET_v_LCW_NI_eflow_400_METphi;
 
  TH1D *h_SoftJets_LCW_NI_eflow_400_METphi;
  TH1D *h_SoftJets_LCW_NI_eflow_400_METx;
  TH1D *h_SoftJets_LCW_NI_eflow_400_METy;
  TH1D *h_SoftJets_LCW_NI_eflow_400_MET;
  TH1D *h_SoftJets_LCW_NI_eflow_400_SET;
  TH2D *h_SoftJets_LCW_NI_eflow_400_MET_v_LCW_NI_eflow_400_METphi;

  TH1D *h_RefMuon_Muid_LCW_NI_eflow_400_METphi;
  TH1D *h_RefMuon_Muid_LCW_NI_eflow_400_METx;
  TH1D *h_RefMuon_Muid_LCW_NI_eflow_400_METy;
  TH1D *h_RefMuon_Muid_LCW_NI_eflow_400_MET;
  TH1D *h_RefMuon_Muid_LCW_NI_eflow_400_SET;
  TH2D *h_RefMuon_Muid_LCW_NI_eflow_400_MET_v_LCW_NI_eflow_400_METphi;
  
  TH1D *h_RefMuon_Track_Muid_LCW_NI_eflow_400_METphi;
  TH1D *h_RefMuon_Track_Muid_LCW_NI_eflow_400_METx;
  TH1D *h_RefMuon_Track_Muid_LCW_NI_eflow_400_METy;
  TH1D *h_RefMuon_Track_Muid_LCW_NI_eflow_400_MET;
  TH1D *h_RefMuon_Track_Muid_LCW_NI_eflow_400_SET;
  TH2D *h_RefMuon_Track_Muid_LCW_NI_eflow_400_MET_v_LCW_NI_eflow_400_METphi;
  
  TH1D *h_Muon_Isol_Muid_LCW_NI_eflow_400_METphi;
  TH1D *h_Muon_Isol_Muid_LCW_NI_eflow_400_METx;
  TH1D *h_Muon_Isol_Muid_LCW_NI_eflow_400_METy;
  TH1D *h_Muon_Isol_Muid_LCW_NI_eflow_400_MET;
  TH1D *h_Muon_Isol_Muid_LCW_NI_eflow_400_SET;
  TH2D *h_Muon_Isol_Muid_LCW_NI_eflow_400_MET_v_LCW_NI_eflow_400_METphi;

  TH1D *h_Muon_NonIsol_Muid_LCW_NI_eflow_400_METphi;
  TH1D *h_Muon_NonIsol_Muid_LCW_NI_eflow_400_METx;
  TH1D *h_Muon_NonIsol_Muid_LCW_NI_eflow_400_METy;
  TH1D *h_Muon_NonIsol_Muid_LCW_NI_eflow_400_MET;
  TH1D *h_Muon_NonIsol_Muid_LCW_NI_eflow_400_SET;
  TH2D *h_Muon_NonIsol_Muid_LCW_NI_eflow_400_MET_v_LCW_NI_eflow_400_METphi;

  TH1D *h_Muon_Total_Muid_LCW_NI_eflow_400_METphi;
  TH1D *h_Muon_Total_Muid_LCW_NI_eflow_400_METx;
  TH1D *h_Muon_Total_Muid_LCW_NI_eflow_400_METy;
  TH1D *h_Muon_Total_Muid_LCW_NI_eflow_400_MET;
  TH1D *h_Muon_Total_Muid_LCW_NI_eflow_400_SET;
  TH2D *h_Muon_Total_Muid_LCW_NI_eflow_400_MET_v_LCW_NI_eflow_400_METphi;

  TH1D *h_RefGamma_LCW_NI_eflow_400_METphi;
  TH1D *h_RefGamma_LCW_NI_eflow_400_METx;
  TH1D *h_RefGamma_LCW_NI_eflow_400_METy;
  TH1D *h_RefGamma_LCW_NI_eflow_400_MET;
  TH1D *h_RefGamma_LCW_NI_eflow_400_SET;
  TH2D *h_RefGamma_LCW_NI_eflow_400_MET_v_LCW_NI_eflow_400_METphi;

  TH1D *h_RefTau_LCW_NI_eflow_400_METphi;
  TH1D *h_RefTau_LCW_NI_eflow_400_METx;
  TH1D *h_RefTau_LCW_NI_eflow_400_METy;
  TH1D *h_RefTau_LCW_NI_eflow_400_MET;
  TH1D *h_RefTau_LCW_NI_eflow_400_SET;
  TH2D *h_RefTau_LCW_NI_eflow_400_MET_v_LCW_NI_eflow_400_METphi;

  TH1D *h_CellOut_LCW_NI_eflow_400_METphi;
  TH1D *h_CellOut_LCW_NI_eflow_400_METx;
  TH1D *h_CellOut_LCW_NI_eflow_400_METy;
  TH1D *h_CellOut_LCW_NI_eflow_400_MET;
  TH1D *h_CellOut_LCW_NI_eflow_400_SET;
  TH2D *h_CellOut_LCW_NI_eflow_400_MET_v_LCW_NI_eflow_400_METphi;
  
  TH1D *h_Cryo_LCW_NI_eflow_400_METphi;
  TH1D *h_Cryo_LCW_NI_eflow_400_METx;
  TH1D *h_Cryo_LCW_NI_eflow_400_METy;
  TH1D *h_Cryo_LCW_NI_eflow_400_MET;
  TH1D *h_Cryo_LCW_NI_eflow_400_SET;
  TH2D *h_Cryo_LCW_NI_eflow_400_MET_v_LCW_NI_eflow_400_METphi;
  
//LCW_NI_eflow_CB

  TH1D *h_RefFinal_LCW_NI_eflow_CB_METphi;
  TH1D *h_RefFinal_LCW_NI_eflow_CB_METx;
  TH1D *h_RefFinal_LCW_NI_eflow_CB_METy;
  TH1D *h_RefFinal_LCW_NI_eflow_CB_MET;
  TH1D *h_RefFinal_LCW_NI_eflow_CB_SET;
  TH2D *h_RefFinal_LCW_NI_eflow_CB_MET_v_LCW_NI_eflow_CB_METphi;

  TH1D *h_RefEle_LCW_NI_eflow_CB_METphi;
  TH1D *h_RefEle_LCW_NI_eflow_CB_METx;
  TH1D *h_RefEle_LCW_NI_eflow_CB_METy;
  TH1D *h_RefEle_LCW_NI_eflow_CB_MET;
  TH1D *h_RefEle_LCW_NI_eflow_CB_SET;
  TH2D *h_RefEle_LCW_NI_eflow_CB_MET_v_LCW_NI_eflow_CB_METphi;

  TH1D *h_RefJet_LCW_NI_eflow_CB_METphi;
  TH1D *h_RefJet_LCW_NI_eflow_CB_METx;
  TH1D *h_RefJet_LCW_NI_eflow_CB_METy;
  TH1D *h_RefJet_LCW_NI_eflow_CB_MET;
  TH1D *h_RefJet_LCW_NI_eflow_CB_SET;
  TH2D *h_RefJet_LCW_NI_eflow_CB_MET_v_LCW_NI_eflow_CB_METphi;
  
  TH1D *h_RefJet_old_LCW_NI_eflow_CB_METphi;
  TH1D *h_RefJet_old_LCW_NI_eflow_CB_METx;
  TH1D *h_RefJet_old_LCW_NI_eflow_CB_METy;
  TH1D *h_RefJet_old_LCW_NI_eflow_CB_MET;
  TH1D *h_RefJet_old_LCW_NI_eflow_CB_SET;
  TH2D *h_RefJet_old_LCW_NI_eflow_CB_MET_v_LCW_NI_eflow_CB_METphi;
 
  TH1D *h_SoftJets_LCW_NI_eflow_CB_METphi;
  TH1D *h_SoftJets_LCW_NI_eflow_CB_METx;
  TH1D *h_SoftJets_LCW_NI_eflow_CB_METy;
  TH1D *h_SoftJets_LCW_NI_eflow_CB_MET;
  TH1D *h_SoftJets_LCW_NI_eflow_CB_SET;
  TH2D *h_SoftJets_LCW_NI_eflow_CB_MET_v_LCW_NI_eflow_CB_METphi;

  TH1D *h_RefMuon_Muid_LCW_NI_eflow_CB_METphi;
  TH1D *h_RefMuon_Muid_LCW_NI_eflow_CB_METx;
  TH1D *h_RefMuon_Muid_LCW_NI_eflow_CB_METy;
  TH1D *h_RefMuon_Muid_LCW_NI_eflow_CB_MET;
  TH1D *h_RefMuon_Muid_LCW_NI_eflow_CB_SET;
  TH2D *h_RefMuon_Muid_LCW_NI_eflow_CB_MET_v_LCW_NI_eflow_CB_METphi;
  
  TH1D *h_RefMuon_Track_Muid_LCW_NI_eflow_CB_METphi;
  TH1D *h_RefMuon_Track_Muid_LCW_NI_eflow_CB_METx;
  TH1D *h_RefMuon_Track_Muid_LCW_NI_eflow_CB_METy;
  TH1D *h_RefMuon_Track_Muid_LCW_NI_eflow_CB_MET;
  TH1D *h_RefMuon_Track_Muid_LCW_NI_eflow_CB_SET;
  TH2D *h_RefMuon_Track_Muid_LCW_NI_eflow_CB_MET_v_LCW_NI_eflow_CB_METphi;
  
  TH1D *h_Muon_Isol_Muid_LCW_NI_eflow_CB_METphi;
  TH1D *h_Muon_Isol_Muid_LCW_NI_eflow_CB_METx;
  TH1D *h_Muon_Isol_Muid_LCW_NI_eflow_CB_METy;
  TH1D *h_Muon_Isol_Muid_LCW_NI_eflow_CB_MET;
  TH1D *h_Muon_Isol_Muid_LCW_NI_eflow_CB_SET;
  TH2D *h_Muon_Isol_Muid_LCW_NI_eflow_CB_MET_v_LCW_NI_eflow_CB_METphi;

  TH1D *h_Muon_NonIsol_Muid_LCW_NI_eflow_CB_METphi;
  TH1D *h_Muon_NonIsol_Muid_LCW_NI_eflow_CB_METx;
  TH1D *h_Muon_NonIsol_Muid_LCW_NI_eflow_CB_METy;
  TH1D *h_Muon_NonIsol_Muid_LCW_NI_eflow_CB_MET;
  TH1D *h_Muon_NonIsol_Muid_LCW_NI_eflow_CB_SET;
  TH2D *h_Muon_NonIsol_Muid_LCW_NI_eflow_CB_MET_v_LCW_NI_eflow_CB_METphi;

  TH1D *h_Muon_Total_Muid_LCW_NI_eflow_CB_METphi;
  TH1D *h_Muon_Total_Muid_LCW_NI_eflow_CB_METx;
  TH1D *h_Muon_Total_Muid_LCW_NI_eflow_CB_METy;
  TH1D *h_Muon_Total_Muid_LCW_NI_eflow_CB_MET;
  TH1D *h_Muon_Total_Muid_LCW_NI_eflow_CB_SET;
  TH2D *h_Muon_Total_Muid_LCW_NI_eflow_CB_MET_v_LCW_NI_eflow_CB_METphi;

  TH1D *h_RefGamma_LCW_NI_eflow_CB_METphi;
  TH1D *h_RefGamma_LCW_NI_eflow_CB_METx;
  TH1D *h_RefGamma_LCW_NI_eflow_CB_METy;
  TH1D *h_RefGamma_LCW_NI_eflow_CB_MET;
  TH1D *h_RefGamma_LCW_NI_eflow_CB_SET;
  TH2D *h_RefGamma_LCW_NI_eflow_CB_MET_v_LCW_NI_eflow_CB_METphi;

  TH1D *h_RefTau_LCW_NI_eflow_CB_METphi;
  TH1D *h_RefTau_LCW_NI_eflow_CB_METx;
  TH1D *h_RefTau_LCW_NI_eflow_CB_METy;
  TH1D *h_RefTau_LCW_NI_eflow_CB_MET;
  TH1D *h_RefTau_LCW_NI_eflow_CB_SET;
  TH2D *h_RefTau_LCW_NI_eflow_CB_MET_v_LCW_NI_eflow_CB_METphi;

  TH1D *h_CellOut_LCW_NI_eflow_CB_METphi;
  TH1D *h_CellOut_LCW_NI_eflow_CB_METx;
  TH1D *h_CellOut_LCW_NI_eflow_CB_METy;
  TH1D *h_CellOut_LCW_NI_eflow_CB_MET;
  TH1D *h_CellOut_LCW_NI_eflow_CB_SET;
  TH2D *h_CellOut_LCW_NI_eflow_CB_MET_v_LCW_NI_eflow_CB_METphi;
  
  TH1D *h_Cryo_LCW_NI_eflow_CB_METphi;
  TH1D *h_Cryo_LCW_NI_eflow_CB_METx;
  TH1D *h_Cryo_LCW_NI_eflow_CB_METy;
  TH1D *h_Cryo_LCW_NI_eflow_CB_MET;
  TH1D *h_Cryo_LCW_NI_eflow_CB_SET;
  TH2D *h_Cryo_LCW_NI_eflow_CB_MET_v_LCW_NI_eflow_CB_METphi;
  
//
    
//  TH1D *h_CellOut_Eflow_METphi;
//  TH1D *h_CellOut_Eflow_METx;
//  TH1D *h_CellOut_Eflow_METy;
//  TH1D *h_CellOut_Eflow_MET;
//  TH1D *h_CellOut_Eflow_SET;
//  TH2D *h_CellOut_Eflow_MET_v_METphi;

/////
  
  TH1D *h_CellOut_CentReg_METphi;
  TH1D *h_CellOut_CentReg_METx;
  TH1D *h_CellOut_CentReg_METy;
  TH1D *h_CellOut_CentReg_MET;
  TH1D *h_CellOut_CentReg_SET;
 
  TH1D *h_CellOut_ECReg_METphi;
  TH1D *h_CellOut_ECReg_METx;
  TH1D *h_CellOut_ECReg_METy;
  TH1D *h_CellOut_ECReg_MET;
  TH1D *h_CellOut_ECReg_SET;
  
  TH1D *h_CellOut_ForwReg_METphi;
  TH1D *h_CellOut_ForwReg_METx;
  TH1D *h_CellOut_ForwReg_METy;
  TH1D *h_CellOut_ForwReg_MET;
  TH1D *h_CellOut_ForwReg_SET;
 

  TH1D *h_CellOut_GC_CentReg_METphi;
  TH1D *h_CellOut_GC_CentReg_METx;
  TH1D *h_CellOut_GC_CentReg_METy;
  TH1D *h_CellOut_GC_CentReg_MET;
  TH1D *h_CellOut_GC_CentReg_SET;
  TH2D *h_CellOut_GC_CentReg_MET_v_METphi;

  TH1D *h_CellOut_GC_ECReg_METphi;
  TH1D *h_CellOut_GC_ECReg_METx;
  TH1D *h_CellOut_GC_ECReg_METy;
  TH1D *h_CellOut_GC_ECReg_MET;
  TH1D *h_CellOut_GC_ECReg_SET;
  TH2D *h_CellOut_GC_ECReg_MET_v_METphi;
  
  TH1D *h_CellOut_GC_ForwReg_METphi;
  TH1D *h_CellOut_GC_ForwReg_METx;
  TH1D *h_CellOut_GC_ForwReg_METy;
  TH1D *h_CellOut_GC_ForwReg_MET;
  TH1D *h_CellOut_GC_ForwReg_SET;
  TH2D *h_CellOut_GC_ForwReg_MET_v_METphi;
 

  TH1D *h_CellOut_LC_CentReg_METphi;
  TH1D *h_CellOut_LC_CentReg_METx;
  TH1D *h_CellOut_LC_CentReg_METy;
  TH1D *h_CellOut_LC_CentReg_MET;
  TH1D *h_CellOut_LC_CentReg_SET;
  TH2D *h_CellOut_LC_CentReg_MET_v_METphi;

  TH1D *h_CellOut_LC_ECReg_METphi;
  TH1D *h_CellOut_LC_ECReg_METx;
  TH1D *h_CellOut_LC_ECReg_METy;
  TH1D *h_CellOut_LC_ECReg_MET;
  TH1D *h_CellOut_LC_ECReg_SET;
  TH2D *h_CellOut_LC_ECReg_MET_v_METphi;
  
  TH1D *h_CellOut_LC_ForwReg_METphi;
  TH1D *h_CellOut_LC_ForwReg_METx;
  TH1D *h_CellOut_LC_ForwReg_METy;
  TH1D *h_CellOut_LC_ForwReg_MET;
  TH1D *h_CellOut_LC_ForwReg_SET;
  TH2D *h_CellOut_LC_ForwReg_MET_v_METphi;
  
  TH1D *h_RefFinal_eflow_SET;
  TH1D *h_RefFinal_eflow_MET;
  TH1D *h_RefFinal_eflow_METx;
  TH1D *h_RefFinal_eflow_METy;

  TH1D *h_OutJet_SET;
  TH1D *h_OutJet_MET;
  TH1D *h_OutJet_METx;
  TH1D *h_OutJet_METy;

  TH1D *h_SumETCorrFinal;

  TH1D *h_EtMissH1calibrated;

  TH1D *h_RefFinal_simple_Significance;
  TH1D *h_Topo_simple_Significance;
  TH1D *h_LocHadTopo_simple_Significance;


  TH1D *h_RefFinalc_SET;
  TH1D *h_RefFinalc_METx;
  TH1D *h_RefFinalc_METy;
  TH1D *h_RefFinalc_MET;

  TH1D *h_RefFinalcc_SET;
  TH1D *h_RefFinalcc_METx;
  TH1D *h_RefFinalcc_METy;
  TH1D *h_RefFinalcc_MET;
  
  TH1D *h_RefFinalc_em_jetcut4_SET;
  TH1D *h_RefFinalc_em_jetcut4_METx;
  TH1D *h_RefFinalc_em_jetcut4_METy;
  TH1D *h_RefFinalc_em_jetcut4_MET;  


  //LCW_NI

  TH1D *h_RefFinal_LCW_NI_METphi;
  TH1D *h_RefFinal_LCW_NI_METx;
  TH1D *h_RefFinal_LCW_NI_METy;
  TH1D *h_RefFinal_LCW_NI_MET;
  TH1D *h_RefFinal_LCW_NI_SET;
 
  
  ///////////////
  //Calo Plots 
  ///////////////
 
  TH1D *h_Topo_METx_PEMB;
  TH1D *h_Topo_METy_PEMB;
  TH1D *h_Topo_SET_PEMB;
  TH1D *h_Topo_METphi_PEMB;
    
  TH1D *h_Topo_METx_EMB;
  TH1D *h_Topo_METy_EMB;
  TH1D *h_Topo_SET_EMB;
  TH1D *h_Topo_METphi_EMB;
  
  TH1D *h_Topo_METx_EME;
  TH1D *h_Topo_METy_EME;
  TH1D *h_Topo_SET_EME;
  TH1D *h_Topo_METphi_EME;
  
  TH1D *h_Topo_METx_PEMEC;
  TH1D *h_Topo_METy_PEMEC;
  TH1D *h_Topo_SET_PEMEC;
  TH1D *h_Topo_METphi_PEMEC;
 
  TH1D *h_Topo_METx_HEC;
  TH1D *h_Topo_METy_HEC;
  TH1D *h_Topo_SET_HEC;
  TH1D *h_Topo_METphi_HEC;
 
  TH1D *h_Topo_METx_TILE;
  TH1D *h_Topo_METy_TILE;
  TH1D *h_Topo_SET_TILE;
  TH1D *h_Topo_METphi_TILE;
  
  TH1D *h_Topo_METx_FCAL;
  TH1D *h_Topo_METy_FCAL;
  TH1D *h_Topo_SET_FCAL;
  TH1D *h_Topo_METphi_FCAL;

  TH1D *h_Topo_METx_CentralReg;
  TH1D *h_Topo_METy_CentralReg;
  TH1D *h_Topo_SET_CentralReg;
  TH1D *h_Topo_METphi_CentralReg;  
  
  TH1D *h_Topo_METx_EndcapReg;
  TH1D *h_Topo_METy_EndcapReg;
  TH1D *h_Topo_SET_EndcapReg;
  TH1D *h_Topo_METphi_EndcapReg;  
  
  TH1D *h_Topo_METx_ForwardReg;
  TH1D *h_Topo_METy_ForwardReg;
  TH1D *h_Topo_SET_ForwardReg;
  TH1D *h_Topo_METphi_ForwardReg; 
  
  TH1D *h_Topo_METx_SumReg;
  TH1D *h_Topo_METy_SumReg;
  TH1D *h_Topo_SET_SumReg;
  TH1D *h_Topo_METphi_SumReg;  
  
  TH1D *h_Topo_METx_SumCal;
  TH1D *h_Topo_METy_SumCal;
  TH1D *h_Topo_SET_SumCal;
  TH1D *h_Topo_METphi_SumCal;  

  //Calo plots rappo
  TH1D *r_Topo_SET_PEMB;
  TH1D *r_Topo_SET_PEMEC;
  TH1D *r_Topo_SET_EMB;
  TH1D *r_Topo_SET_EME;
  TH1D *r_Topo_SET_HEC;
  TH1D *r_Topo_SET_TILE;
  TH1D *r_Topo_SET_FCAL;
  
  //nCell plot
  TH1D *h_Topo_nCell;

  ///////////////
 
  TH2D * h_Topo_METvsSET;


  //MET significance plots
  TH1D *h_RefFinal_METsignificance;
  TH1D *h_Topo_METsignificance;
  TH1D *h_RefFinal_sqrtSET; 
  TH1D *h_Topo_sqrtSET; 


};
