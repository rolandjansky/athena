/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RESOLUTION_TOOL_H 
#define RESOLUTION_TOOL_H 

#include "MissingETData.h"

#include <string>
#include <vector>

#include "TH1.h"
#include "TH2.h"
#include "TGraphErrors.h"
#include "TProfile.h"
#include "TFile.h"

class ResolutionTool  {

public:

  ResolutionTool();
  ResolutionTool(const ResolutionTool &other);
  ~ResolutionTool() {}
  void initialize(TFile *);

  void execute(MissingETData *data);
  void resolution(MissingETData *data);
  std::string m_folderName;

  //parameters for histograms
  int m_iset;
  int m_nbinres;
  int m_nbinres1;
  float m_scares1;
  int m_nbinres2;
  float m_scares2;
  int m_nbinres3;
  float m_scares3;
  int m_nbinphires1;
  float lowET;
  float suET;
  int m_useTruth;
  int m_use_sumet;

  //donatella curvaresol
  int m_rescurve;
  int m_numptt;
  float m_startpt;
  float m_binpt;
  float m_scares;
  float m_fitrange;
  float m_ns;
  float m_percerr;
  int m_entries;
  float m_curvasres;
  float m_curvasET;

  float m_lowET;
  float m_suET;

  TGraphErrors* grres;
  
  std::vector<TH1F *> h_;    //RefFinal
  std::vector<TH1F *> h1_;   //LocHad topo
  std::vector<TH1F *> h2_;   //EM topo MET_Topo
  std::vector<TH1F *> h3_;   //GCW topo MET_CorrTopo
  std::vector<TH1F *> h4_;   //RefFinal LCW_pt20
  std::vector<TH1F *> h5_;   //RefFinal GCW_pt20
  std::vector<TH1F *> h6_;   //RefFinal_em
  std::vector<TH1F *> h7_;   //RefFinal LCW_NI_eflow4
  std::vector<TH1F *> h8_;   //RefFinal LCW_NI_eflow_CB 
  std::vector<TH1F *> h9_;   //RefFinal LCW_NI_eflow_400 
  std::vector<TH1F *> h10_;  //RefFinal LCW_NI (no eflow)
  
  
  std::vector<TH1F *> h_45_;    //RefFinal in eta < 4.5
  std::vector<TH1F *> h1_45_;   //LocHad topo in eta < 4.5
  std::vector<TH1F *> h2_45_;   //EM topo MET_Topo in eta < 4.5
  std::vector<TH1F *> h3_45_;   //GCW topo MET_CorrTopo in eta < 4.5
  std::vector<TH1F *> h4_45_;   //RefFinal LCW pt20 in eta < 4.5
  std::vector<TH1F *> h5_45_;   //RefFinal GCW pt20 in eta < 4.5
  std::vector<TH1F *> h6_45_;	//RefFinal_em in eta < 4.5
  std::vector<TH1F *> h7_45_;	//RefFinal LCW_NI_eflow4 in eta < 4.5
  std::vector<TH1F *> h8_45_;	//RefFinal LCW_NI_eflow_CB in eta < 4.5 
  std::vector<TH1F *> h9_45_;   //RefFinal LCW_NI_eflow_400 
  std::vector<TH1F *> h10_45_;  //RefFinal LCW_NI (no eflow)
  
  
/*  
  std::vector<TH1F *> hHT_;   //EM HT
  std::vector<TH1F *> hHT_45_;   //EM HT in eta < 4.5
  std::vector<TH1F *> hHTRef_;   //Default RefFinalHT
  std::vector<TH1F *> hHTRef_45_;   //Default RefFinalHT in eta < 4.5
  std::vector<TH1F *> hHTLCW_pt20_;   //RefFinalHT LCW_pt20
  std::vector<TH1F *> hHTLCW_pt20_45_;   //RefFinalHT Default R in eta < 4.5
*/
  std::vector<TH1F *> hHT_;   //RefFinal Default HT
  std::vector<TH1F *> hHT7_;   //RF_LCW_NI_eflow4 HT
  std::vector<TH1F *> hHT9_;   //RF_LCW_NI_eflow_400 HT
  std::vector<TH1F *> hHT_45_;   //RefFinal Default HT in eta < 4.5
  std::vector<TH1F *> hHT7_45_;   //RF_LCW_NI_eflow4 HT in eta < 4.5
  std::vector<TH1F *> hHT9_45_;   //RF_LCW_NI_eflow_400 HT in eta < 4.5

/*
  std::vector<TH1F *> hHTLCW4_;   //RefFinalHT LCW_NI_pt20_MuID_eflow (type 4 in Table)
  std::vector<TH1F *> hHTLCW4_45_;   //RefFinalHT LCW_NI... in eta < 4.5
  
  std::vector<TH1F *> hemjes_;   //RefFinalemjes  
  std::vector<TH1F *> hemjes_45_;   //RefFinalemjes in eta < 4.5
 
  std::vector<TH1F *> h_LCW_eflow_;   //RefFinal_LCW_eflow  
  std::vector<TH1F *> h_LCW_eflow_45_;   //RefFinal_LCW_eflow in eta < 4.5
  
  std::vector<TH1F *> h_LCW_NI_;   //RefFinal_LCW_NI  
  std::vector<TH1F *> h_LCW_NI_45_;   //RefFinal_LCW_NI in eta < 4.5
*/ 
/*
  std::vector<TH1F *> h11_;  //EM Topo, ratioHT < 0.25
  std::vector<TH1F *> h12_;  //EM Topo, ratioHT > 0.25 && < 0.5
  std::vector<TH1F *> h13_;  //EM Topo, ratioHT > 0.5
  std::vector<TH1F *> h14_;  //EM Topo, no FCAL 
  std::vector<TH1F *> h15_;  //EM Topo, FCAL fraction < 0.25
  std::vector<TH1F *> h16_;  //EM Topo, FCAL fraction > 0.25 && < 0.5
  std::vector<TH1F *> h17_;  //EM Topo, FCAL fraction > 0.5
*/
  std::vector<TH1F *> hbias_;   //MET bias
  std::vector<TH1F *> hbias1_;  //
  std::vector<TH1F *> hbias2_;  //
  std::vector<TH1F *> hbias3_;  //
  std::vector<TH1F *> hbias4_;
  std::vector<TH1F *> hbias5_;  //
  std::vector<TH1F *> hbias6_;
  std::vector<TH1F *> hbias7_;
  std::vector<TH1F *> hbias8_;
  std::vector<TH1F *> hbias9_;
  std::vector<TH1F *> hbias10_;
  
  
  std::vector<TH1F *> h21_;  //EM topo, x component
  std::vector<TH1F *> h22_;  //EM topo, y component

  std::vector<TH1F *> h23_;  //EM topo for 1 jet events
  std::vector<TH1F *> h24_;  //EM topo for > 1 jet events
  std::vector<TH1F *> h25_;  //EM topo for 1 jet events, |eta|<0.6
  

   
  TH2F *he_resvset;

///////////////////////////////////////////////////////////////
//s sven plot
  //em scale set / cal scale set for each set
  std::vector<TH1F *> r_;
  std::vector<TH1F *> r1_;
  std::vector<TH1F *> r2_;
  std::vector<TH1F *> r3_;
  std::vector<TH1F *> r4_;
  std::vector<TH1F *> r5_;
  std::vector<TH1F *> r6_;
  std::vector<TH1F *> r7_;
  std::vector<TH1F *> r8_; 
  std::vector<TH1F *> r9_; 
  std::vector<TH1F *> r10_; 
  
  
  std::vector<TH1F *> r_45_;  //eta < 4.5
  std::vector<TH1F *> r1_45_;
  std::vector<TH1F *> r2_45_;
  std::vector<TH1F *> r3_45_;
  std::vector<TH1F *> r4_45_;
  std::vector<TH1F *> r5_45_;
  std::vector<TH1F *> r6_45_;
  std::vector<TH1F *> r7_45_;
  std::vector<TH1F *> r8_45_; 
  std::vector<TH1F *> r9_45_; 
  std::vector<TH1F *> r10_45_; 
  

  std::vector<TH1F *> rHT_; 
  std::vector<TH1F *> rHT7_; 
  std::vector<TH1F *> rHT9_; 

  std::vector<TH1F *> rHT_45_; 
  std::vector<TH1F *> rHT7_45_; 
  std::vector<TH1F *> rHT9_45_; 


/*
  std::vector<TH1F *> rHTRef_; 
  std::vector<TH1F *> rHTRef_45_; 
  std::vector<TH1F *> rHTLCW_pt20_; 
  std::vector<TH1F *> rHTLCW_pt20_45_; 
  std::vector<TH1F *> rHTLCW4_; 
  std::vector<TH1F *> rHTLCW4_45_; 
*/  
/*  
  std::vector<TH1F *> remjes_; 
  std::vector<TH1F *> remjes_45_; 
   
  std::vector<TH1F *> r_LCW_eflow_; 
  std::vector<TH1F *> r_LCW_eflow_45_; 
  
  std::vector<TH1F *> r_LCW_NI_; 
  std::vector<TH1F *> r_LCW_NI_45_; 
*/ 
 
  //missingET ratios (etmiss calib / etmiss EM)
  std::vector<TH1F *> p_;
  std::vector<TH1F *> p1_;
  std::vector<TH1F *> p3_;
  std::vector<TH1F *> p4_;
  std::vector<TH1F *> p5_;
  std::vector<TH1F *> p6_;
  std::vector<TH1F *> p7_;
  std::vector<TH1F *> p8_;
  std::vector<TH1F *> p9_;
  std::vector<TH1F *> p10_;
  
  
  std::vector<TH1F *> p_45_;  //eta < 4.5
  std::vector<TH1F *> p1_45_;
  std::vector<TH1F *> p2_45_;
  std::vector<TH1F *> p3_45_;
  std::vector<TH1F *> p4_45_;
  std::vector<TH1F *> p5_45_;
  std::vector<TH1F *> p6_45_;
  std::vector<TH1F *> p7_45_;
  std::vector<TH1F *> p8_45_; 
  std::vector<TH1F *> p9_45_; 
  std::vector<TH1F *> p10_45_; 
  
  
///////////////////////////////////////////////////////////////
//correct SumEt bins
  std::vector<TH1F *> su_;
  std::vector<TH1F *> su1_;
  std::vector<TH1F *> su2_;
  std::vector<TH1F *> su3_;
  std::vector<TH1F *> su4_;
  std::vector<TH1F *> su5_;
  std::vector<TH1F *> su6_;
  std::vector<TH1F *> su7_;
  std::vector<TH1F *> su8_;
  std::vector<TH1F *> su9_;
  std::vector<TH1F *> su10_;
  

///////////////////////////////////////////////////////////////
//Ncell plots = sqrt of number of cells in each sumet bin
  std::vector<TH1F *> nc_;
  std::vector<TH1F *> nc1_;
  std::vector<TH1F *> nc2_;
  std::vector<TH1F *> nc3_;
  std::vector<TH1F *> nc4_;
  std::vector<TH1F *> nc5_;
  std::vector<TH1F *> nc6_;
  std::vector<TH1F *> nc7_;
  std::vector<TH1F *> nc8_;
  std::vector<TH1F *> nc9_;
  std::vector<TH1F *> nc10_;
  
///////////////////////////////////////////////////////////////

  //histograms
  TH1D *h_tot;
  TH1D *h1_tot;
  TH1D *h2_tot;
  TH1D *h3_tot;
  TH1D *h4_tot;
  TH1D *h5_tot;
  TH1D *h6_tot;
  TH1D *h7_tot;
  TH1D *h8_tot;
  TH1D *h9_tot;
  TH1D *h10_tot;
  
  
  TH1D *h_45_tot;
  TH1D *h1_45_tot;
  TH1D *h2_45_tot;
  TH1D *h3_45_tot;
  TH1D *h4_45_tot;
  TH1D *h5_45_tot;
  TH1D *h6_45_tot;
  TH1D *h7_45_tot;
  TH1D *h8_45_tot;
  TH1D *h9_45_tot;
  TH1D *h10_45_tot;
  

  TH1D *hHT_tot;
  TH1D *hHTt_tot;
  TH1D *hHT_tot7;
  TH1D *hHTt_tot7;
  TH1D *hHT_tot9;
  TH1D *hHTt_tot9;

  TH1D *hHT_45_tot;
  TH1D *hHTt_45_tot;
  TH1D *hHT_45_tot7;
  TH1D *hHTt_45_tot7;
  TH1D *hHT_45_tot9;
  TH1D *hHTt_45_tot9;
 
/*
  TH1D *hHTRef_tot;
  TH1D *hHTRef_45_tot;
  TH1D *hHTReft_tot;
  TH1D *hHTReft_45_tot;
 
  TH1D *hHTLCW_pt20_tot;
  TH1D *hHTLCW_pt20_45_tot;
  TH1D *hHTLCW_pt20t_tot;
  TH1D *hHTLCW_pt20t_45_tot;
 
  TH1D *hHTLCW4_tot;
  TH1D *hHTLCW4_45_tot;
  TH1D *hHTLCW4t_tot;
  TH1D *hHTLCW4t_45_tot;
*/ 

  TH1D *h_sumetT;
  TH1D *h_sumet;
  TH1D *h1_sumet;
  TH1D *h2_sumet;
  TH1D *h3_sumet;
  TH1D *h4_sumet;
  TH1D *h5_sumet;
  TH1D *h6_sumet;
  TH1D *h7_sumet;
  TH1D *h8_sumet;
  TH1D *h9_sumet;
  TH1D *h10_sumet;
  
  TH1D *h_45_sumet;  
  TH1D *h1_45_sumet;
  TH1D *h2_45_sumet;
  TH1D *h3_45_sumet;
  TH1D *h4_45_sumet;
  TH1D *h5_45_sumet;
  TH1D *h6_45_sumet;
  TH1D *h7_45_sumet;
  TH1D *h8_45_sumet;
  TH1D *h9_45_sumet;
  TH1D *h10_45_sumet;
  
  TH1D *h_sumetHT;
  TH1D *h_sumetHT7;
  TH1D *h_sumetHT9;
  TH1D *h_sumetHT_45;
  TH1D *h_sumetHT7_45;
  TH1D *h_sumetHT9_45;
  
//  TH1D *h_sumetHTRef;
//  TH1D *h_sumetHTRef_45;
//  TH1D *h_sumetHTLCW_pt20;
//  TH1D *h_sumetHTLCW_pt20_45;
//  TH1D *h_sumetHTLCW4;
//  TH1D *h_sumetHTLCW4_45;
/*
  TH1D *h_sumetemjes;
  TH1D *h_sumetemjes_45;
  
  TH1D *h_sumet_LCW_eflow;
  TH1D *h_sumet_LCW_eflow_45;
  
  TH1D *h_sumet_LCW_NI;
  TH1D *h_sumet_LCW_NI_45;
*/  
  TProfile *h_METGCWdivMETem;
  TProfile *h_METLCdivMETem;
  TProfile *h_METRefdivMETem;
  
  TProfile *h_METem;
  TProfile *h_SETHTovSET;
  TProfile *h_FCALfvSET;

  TH1D *h_FCALf;
  TH1D *h_ratioHT;
  TH1D *h_ratioHTRef;

  TH1D *h_RefFinalHTResolution_x;
  TH1D *h_RefFinalHTResolution_y;
  TH1D *h_RefFinalHTResolution_EtMiss;

  TH1D *h_DetOnlyRefFinalResolution_x;
  TH1D *h_DetOnlyRefFinalResolution_y;
  TH1D *h_DetOnlyRefFinalResolution_EtMiss;

  TH2D *h_DetOnlyRefFinalResolution_V_truth_x;
  TH2D *h_DetOnlyRefFinalResolution_V_truth_y;
  TH2D *h_DetOnlyRefFinalReco_V_truth_x;
  TH2D *h_DetOnlyRefFinalReco_V_truth_y;

  TH1D *h_RefFinalResolution_x;
  TH1D *h_RefFinalResolution_y;
  TH1D *h_RefFinalResolution_EtMiss;
  TH1D *h_RefFinalResolution_phi;
  TH2D *h_RefFinalResolution_phi_V_truth_EtMiss;

  TH2D *h_RefFinalResolution_V_truth_x;
  TH2D *h_RefFinalResolution_V_truth_y;
  TH2D *h_RefFinalReco_V_truth_x;
  TH2D *h_RefFinalReco_V_truth_y;

  TH1D *h_DetOnlyCorrResolution_x;
  TH1D *h_DetOnlyCorrResolution_y;
  TH1D *h_DetOnlyCorrResolution_EtMiss;

  TH1D *h_FinalCorrResolution_x;
  TH1D *h_FinalCorrResolution_y;
  TH1D *h_FinalCorrResolution_EtMiss;

  TH1D *h_MuonBoyResolution_x;
  TH1D *h_MuonBoyResolution_y;
  TH1D *h_MuonBoyResolution_EtMiss;

  TH2D *h_MuonBoyResolution_V_truth_x;
  TH2D *h_MuonBoyResolution_V_truth_y;
  TH2D *h_MuonBoyReco_V_truth_x;
  TH2D *h_MuonBoyReco_V_truth_y;


  //histo pt
  TH1D *h_leading_pt; 
  TH1D *h_subleading_pt;
  TH1D *h_jet_pt;
  TH1D *h_sumpt;

//------------------------------------------------------------------
//PETER plots

  //CellOut fraction
  std::vector<TH1F *> hco_;

  //SoftJets fraction
  std::vector<TH1F *> hsj_;

  //SoftJets fraction
  std::vector<TH1F *> hrj_;


  //eta<4.5
  std::vector<TH1F *> hco_45_;
  std::vector<TH1F *> hsj_45_;
  std::vector<TH1F *> hrj_45_;


  //PETER plots
  
  TH1D *h_CellOutf;
  TH1D *h_SoftJetsf;
  TH1D *h_RefJetf;

  TH1D *h_CellOutf_45;
  TH1D *h_SoftJetsf_45;
  TH1D *h_RefJetf_45;
		      
//------------------------------------------------------------------


};

#endif // RESOLUTION_TOOL_H 
