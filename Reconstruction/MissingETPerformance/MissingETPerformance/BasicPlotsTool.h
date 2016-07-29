/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BASICPLOTS_TOOL_H 
#define BASICPLOTS_TOOL_H 

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ObjectVector.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ITHistSvc.h"

#include "MissingETPerformance/MissingETData.h"
#include "MissingETPerformance/JetVariablesTool.h"
#include "MissingETPerformance/TopoClusterVariablesTool.h"

#include <string>

#include "TH1.h"
#include "TH2.h"

static const InterfaceID IID_BasicPlotsTool("BasicPlotsTool", 1, 0);

class BasicPlotsTool : public AthAlgTool {

public:

  BasicPlotsTool(const std::string& type, const std::string& name,
	           const IInterface* parent);

  /** AlgTool and IAlgTool interface methods */
  static const InterfaceID& interfaceID( ) { return IID_BasicPlotsTool; };

  StatusCode CBNT_initialize();

  /** Overriding initialize and finalize */
  virtual StatusCode initialize();
  /** finalization */
  virtual StatusCode finalize();
  virtual StatusCode execute(MissingETData *data);

   inline void SetFolderName(const std::string& folderName) { m_folderName=folderName; }

protected:

   /** Standard destructor */
   virtual ~BasicPlotsTool( );

 private:
  struct MissingETHists {
    TH1D* METphi;
    TH1D* METx;
    TH1D* METy;
    TH1D* MET;
    TH1D* SET;
    TH2D* MET_v_METphi;
  };

  struct MissingETCaloHists {
    MissingETHists base;
    TH1D* SET[MissingEtCalo::Size];
    TH1D* METx[MissingEtCalo::Size];
    TH1D* METy[MissingEtCalo::Size];
    TH1D* NCELLS[MissingEtCalo::Size];
  };

  void book1d (const std::string& title,
               int nbins, float xlo, float xhi,
               TH1D*& var,
               const char* subfolder = "");
  void book2d (const std::string& title,
               int nbinsx, float xlo, float xhi,
               int nbinsy, float ylo, float yhi,
               TH2D*& var,
               const char* subfolder = "");

  void book_missingETHists (const char* name, MissingETHists& h);
  void book_missingETCaloHists  (const char* name, MissingETCaloHists& h);

  void fill_missingETHists (MissingETHists& h, const MissingET* m);
  void fill_missingETCaloHists (MissingETCaloHists& h, const MissingEtCalo* m);

  
  StatusCode basicPlots(MissingETData *data);

  /** a handle on the Hist/TTree registration service */
  ITHistSvc * m_thistSvc;

  ToolHandle<JetVariablesTool> m_jetVariables;
  bool m_plotJetVariables;

  ToolHandle<TopoClusterVariablesTool> m_clusterVariables;
  bool m_plotClusterVariables;

  std::string m_folderName;
  std::string m_evtInfoKey;

  //parameters for histograms
  int m_iset;
  int m_nbinp;
  float m_suptmi;
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

  int m_nbinJetEt;
  float m_lowerbinJetEt;
  float m_upperbinJetEt;
  int m_nbinClusterEt;
  float m_lowerbinClusterEt;
  float m_upperbinClusterEt;

  int m_regions_ncells;
  int m_regions_nbins_ncells;

  int m_nbinBCID;
  float m_lowBCID;
  float m_upBCID;
  int m_nbinMU;
  float m_lowMU;
  float m_upMU;

  //histograms
  TH2F *m_h_MET_Topo_METx_V_lbn;
  TH2F *m_h_MET_Topo_METy_V_lbn;
  TH2F *m_h_MET_RefFinal_METx_V_lbn;
  TH2F *m_h_MET_RefFinal_METy_V_lbn;

  TH1D *m_h_jetPtWeightedEventEMfraction;
  TH1D *m_h_jetPtWeightedNumAssociatedTracks;
  TH1D *m_h_jetPtWeightedSize;

  TH1D *m_h_RefFinal_simple_Significance;
  TH2D *m_h_RefFinal_simple_Significance_v_leadJetEta;
  TH2D *m_h_RefFinal_simple_Significance_v_secondJetEta;

  TH1F *m_h_cluster_n;
  TH1F *m_h_cluster_et;
  TH1F *m_h_cluster_eta;
  TH1F *m_h_cluster_phi;
  TH2F *m_h_cluster_n_V_MET_Topo_MET;
  TH2F *m_h_cluster_n_V_MET_RefFinal_MET;
  TH2F *m_h_cluster_n_V_MET_Topo_SET;
  TH2F *m_h_cluster_n_V_MET_RefFinal_SET;
  TH2F *m_h_cluster_eta_V_phi;
  TH2F *m_h_cluster_et_V_eta;
  TH2F *m_h_cluster_et_V_phi;

  TH1F *m_h_jet_n;
  TH1F *m_h_jet_et;
  TH1F *m_h_jet_eta;
  TH1F *m_h_jet_phi;

  TH2F *m_h_jet_n_V_MET_Topo_MET;
  TH2F *m_h_jet_n_V_MET_Topo_SET;
  TH2F *m_h_jet_leadjet_et_V_MET_Topo_MET;
  TH2F *m_h_jet_leadjet_eta_V_MET_Topo_MET;
  TH2F *m_h_jet_leadjet_phi_V_MET_Topo_MET;
  TH2F *m_h_jet_secondjet_et_V_MET_Topo_MET;
  TH2F *m_h_jet_secondjet_eta_V_MET_Topo_MET;
  TH2F *m_h_jet_secondjet_phi_V_MET_Topo_MET;

  TH2F *m_h_jet_n_V_MET_RefFinal_MET;
  TH2F *m_h_jet_n_V_MET_RefFinal_SET;
  TH2F *m_h_jet_leadjet_et_V_MET_RefFinal_MET;
  TH2F *m_h_jet_leadjet_eta_V_MET_RefFinal_MET;
  TH2F *m_h_jet_leadjet_phi_V_MET_RefFinal_MET;
  TH2F *m_h_jet_secondjet_et_V_MET_RefFinal_MET;
  TH2F *m_h_jet_secondjet_eta_V_MET_RefFinal_MET;
  TH2F *m_h_jet_secondjet_phi_V_MET_RefFinal_MET;

  TH1D *m_h_ETmissTruthInt_plus_Muons;

  TH1D *m_h_ETmissCorrFinal;
  TH1D *m_h_SumETCorrFinal;

  TH1D *m_h_EtMissH1calibrated;

  MissingETHists m_h_RefFinal;
  MissingETHists m_h_RefEle;
  MissingETHists m_h_RefJet;
  MissingETHists m_h_SoftJets;
  MissingETHists m_h_MuonBoy;
  MissingETHists m_h_MuID;
  MissingETHists m_h_RefMuon;
  MissingETHists m_h_RefMuon_Track;
  MissingETHists m_h_RefGamma;
  MissingETHists m_h_RefTau;
  MissingETHists m_h_CellOut;
  MissingETHists m_h_CellOutEFlow;
  MissingETCaloHists m_h_Base;
  MissingETCaloHists m_h_CorrTopo;
  MissingETCaloHists m_h_Calib;
  MissingETHists m_h_CryoCone;
  MissingETHists m_h_TruthInt;
  MissingETHists m_h_TruthNonInt;
  MissingETHists m_h_TruthMuons;
  MissingETHists m_h_Cryo;
  MissingETHists m_h_DM_All;
  MissingETHists m_h_DM_Crack1;
  MissingETHists m_h_DM_Crack2;
  MissingETHists m_h_DM_Cryo;
  MissingETHists m_h_Final;
  MissingETHists m_h_LocHadTopoObj;
  MissingETHists m_h_TopoObj;
  MissingETHists m_h_MuonBoy_Spectro;
  MissingETHists m_h_MuonBoy_Track;
  MissingETHists m_h_Muon;
  MissingETHists m_h_MuonMuons;
  MissingETCaloHists m_h_Base0;
  MissingETCaloHists m_h_Topo;
  MissingETCaloHists m_h_LocHadTopo;

  TH1F *m_h_METBase_EMfraction;

  TH1F *m_h_MU;
  TH1F *m_h_BCID;

  TH1D *m_h_Resolution_TruthNonInt_RefFinal_METx;
  TH1D *m_h_Resolution_TruthNonInt_RefFinal_METy;

};

#endif // BASICPLOTS_TOOL_H 
