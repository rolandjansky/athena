/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ResolutionTool.h"

#include "TCanvas.h"
#include "TLegend.h"
#include "TF1.h"
#include "TGraphErrors.h"

#include <iostream>
using namespace std;

ResolutionTool::ResolutionTool() {
  m_numptt=80;
  m_startpt=0.;
  m_binpt=5.;
  m_nbinres=100;
  m_scares=100.;
  m_curvasres=30.;
  m_fitrange=3;
  m_ns=2.;
  m_percerr=0.15;
  m_entries=100;

  m_iset=5013;
  m_nbinres1=100;
  m_scares1=100.;
  m_nbinres2=100;
  m_scares2=500.;
  m_nbinres3=200;
  m_scares3=100.;
  m_nbinphires1=315;

  m_lowET=-5.;
  m_suET=1595.;

  m_useTruth=0;
  m_use_sumet=2;
  m_folderName="";

  // beamdata
  if(m_iset >87000) {
    m_nbinres1=200;
    m_scares1=100.;
  }
}

ResolutionTool::ResolutionTool(const ResolutionTool &other) {

  m_numptt        = other.m_numptt;
  m_startpt       = other.m_startpt;
  m_binpt         = other.m_binpt;
  m_nbinres       = other.m_nbinres;
  m_scares        = other.m_scares;
  m_curvasres     = other.m_curvasres;
  m_fitrange      = other.m_fitrange;
  m_ns            = other.m_ns;
  m_percerr       = other.m_percerr;
  m_entries       = other.m_entries;
  m_iset          = other.m_iset;
  m_nbinres1      = other.m_nbinres1;
  m_scares1       = other.m_scares1;
  m_nbinres2      = other.m_nbinres2;
  m_scares2       = other.m_scares2;
  m_nbinres3      = other.m_nbinres3;
  m_scares3       = other.m_scares3;
  m_nbinphires1   = other.m_nbinphires1;
  m_lowET         = other.m_lowET;
  m_suET          = other.m_suET;
  m_useTruth      = other.m_useTruth;
  m_use_sumet     = other.m_use_sumet;
  m_folderName    = other.m_folderName;
}


void ResolutionTool::initialize(TFile *outfile) {

  // beamdata
  if(m_iset >87000) {
    m_nbinres1=200;
    m_scares1=100.;
  }

  TDirectory *dir = 0;
  if (m_folderName!="") {
    if ( (dir = outfile->GetDirectory(m_folderName.c_str())) == 0) {
      dir = outfile->mkdir(m_folderName.c_str());
    }
    outfile->cd(m_folderName.c_str());
    dir = dir->mkdir("Resolution");
    outfile->cd((m_folderName+"/Resolution").c_str());
  } else {
    dir = outfile->mkdir("Resolution");
    outfile->cd("Resolution");
  }

  h_MuonBoyResolution_x = new TH1D("h_MuonBoyResolution_x","MuonBoyResolution(x)",m_nbinres1,-m_scares1,m_scares1);
  h_MuonBoyResolution_y = new TH1D("h_MuonBoyResolution_y","MuonBoyResolution(y)",m_nbinres1,-m_scares1,m_scares1);
  h_RefFinalResolution_phi = new TH1D("h_RefFinalResolution_phi","RefFinalResolution(phi)",m_nbinphires1,0.,3.15);

  //truth
  if (m_useTruth == 1) {
  
    h_DetOnlyRefFinalResolution_x = new TH1D("h_DetOnlyRefFinalResolution_x","DetOnlyRefFinalResolution(x)",m_nbinres1,-m_scares1,m_scares1 );
    h_DetOnlyRefFinalResolution_y = new TH1D("h_DetOnlyRefFinalResolution_y","DetOnlyRefFinalResolution(y)",m_nbinres1,-m_scares1,m_scares1);
    h_DetOnlyRefFinalResolution_EtMiss = new TH1D("h_DetOnlyRefFinalResolution_EtMiss","DetOnlyRefFinalResolution(EtMiss)",m_nbinres1,-m_scares1,m_scares1);
    h_MuonBoyResolution_EtMiss = new TH1D("h_MuonBoyResolution_EtMiss","MuonBoyResolution(EtMiss)",m_nbinres1,-m_scares1,m_scares1);
    h_DetOnlyRefFinalResolution_V_truth_x = new TH2D("h_DetOnlyRefFinalResolution_V_truth_x","DetOnlyRefFinalResolution_V_truth(x)"
    						     ,m_nbinres2,-m_scares2,m_scares2,m_nbinres3,-m_scares3,m_scares3 );
    h_DetOnlyRefFinalResolution_V_truth_y = new TH2D("h_DetOnlyRefFinalResolution_V_truth_y","DetOnlyRefFinalResolution_V_truth(y)",
    						     m_nbinres2,-m_scares2,m_scares2,m_nbinres3,-m_scares3,m_scares3 );
    h_DetOnlyRefFinalReco_V_truth_x = new TH2D("h_DetOnlyRefFinalReco_V_truth_x","DetOnlyRefFinalReco_V_truth(x)",
    					       m_nbinres2,-m_scares2,m_scares2,m_nbinres2,-m_scares2,m_scares2 );
    h_DetOnlyRefFinalReco_V_truth_y = new TH2D("h_DetOnlyRefFinalReco_V_truth_y","DetOnlyRefFinalReco_V_truth(y)",
    					       m_nbinres2,-m_scares2,m_scares2,m_nbinres2,-m_scares2,m_scares2 );
    h_RefFinalResolution_x = new TH1D("h_RefFinalResolution_x","RefFinalResolution(x)",m_nbinres1,-m_scares1,m_scares1);
    h_RefFinalResolution_y = new TH1D("h_RefFinalResolution_y","RefFinalResolution(y)",m_nbinres1,-m_scares1,m_scares1);
    h_RefFinalResolution_EtMiss = new TH1D("h_RefFinalResolution_EtMiss","RefFinalResolution(EtMiss)",m_nbinres1,-m_scares1,m_scares1);
    h_RefFinalResolution_phi_V_truth_EtMiss = new TH2D("h_RefFinalResolution_phi_V_truth_EtMiss","RefFinalResolution(phi) vs truth EtMiss",
    						       m_nbinres2,0.,m_scares2,m_nbinphires1,0.,3.15);
    h_RefFinalResolution_V_truth_x = new TH2D("h_RefFinalResolution_V_truth_x","RefFinalResolution_V_truth(x)",
    					      m_nbinres2,-m_scares2,m_scares2,m_nbinres3,-m_scares3,m_scares3 );
    h_RefFinalResolution_V_truth_y = new TH2D("h_RefFinalResolution_V_truth_y","RefFinalResolution_V_truth(y)",
    					      m_nbinres2,-m_scares2,m_scares2,m_nbinres3,-m_scares3,m_scares3 );
    h_RefFinalReco_V_truth_x = new TH2D("h_RefFinalReco_V_truth_x","RefFinalReco_V_truth(x)",m_nbinres2,-m_scares2,m_scares2,m_nbinres2,-m_scares2,m_scares2 );
    h_RefFinalReco_V_truth_y = new TH2D("h_RefFinalReco_V_truth_y","RefFinalReco_V_truth(y)",m_nbinres2,-m_scares2,m_scares2,m_nbinres2,-m_scares2,m_scares2 );  
    h_MuonBoyResolution_V_truth_x = new TH2D("h_MuonBoyResolution_V_truth_x","MuonBoyResolution_V_truth(x)",
    					     m_nbinres2,-m_scares2,m_scares2,m_nbinres3,-m_scares3,m_scares3 );
    h_MuonBoyResolution_V_truth_y = new TH2D("h_MuonBoyResolution_V_truth_y","MuonBoyResolution_V_truth(y)",
    					     m_nbinres2,-m_scares2,m_scares2,m_nbinres3,-m_scares3,m_scares3 );
    h_MuonBoyReco_V_truth_x = new TH2D("h_MuonBoyReco_V_truth_x","MuonBoyReco_V_truth(x)",m_nbinres2,-m_scares2,m_scares2,m_nbinres2,-m_scares2,m_scares2 );
    h_MuonBoyReco_V_truth_y = new TH2D("h_MuonBoyReco_V_truth_y","MuonBoyReco_V_truth(y)",m_nbinres2,-m_scares2,m_scares2,m_nbinres2,-m_scares2,m_scares2 );

    h_DetOnlyCorrResolution_x = new TH1D("h_DetOnlyCorrResolution_x","DetOnlyCorrResolution(x)",m_nbinres1,-m_scares1,m_scares1 );
    h_DetOnlyCorrResolution_y = new TH1D("h_DetOnlyCorrResolution_y","DetOnlyCorrResolution(y)",m_nbinres1,-m_scares1,m_scares1);

    h_FinalCorrResolution_x = new TH1D("h_FinalCorrResolution_x","FinalCorrResolution(x)",m_nbinres1,-m_scares1,m_scares1);
    h_FinalCorrResolution_y = new TH1D("h_FinalCorrResolution_y","FinalCorrResolution(y)",m_nbinres1,-m_scares1,m_scares1);
    h_DetOnlyCorrResolution_EtMiss = new TH1D("h_DetOnlyCorrResolution_EtMiss","DetOnlyCorrResolution(EtMiss)",m_nbinres1,-m_scares1,m_scares1);
    h_FinalCorrResolution_EtMiss = new TH1D("h_FinalCorrResolution_EtMiss","FinalCorrResolution(EtMiss)",m_nbinres3,-m_scares3,m_scares3);

  }
   

  //effect of not adding CellOut
  h_RefFinalHTResolution_x = new TH1D("h_RefFinalHTResolution_x","RefFinalHTResolution(x)",m_nbinres1,-m_scares1,m_scares1);
  h_RefFinalHTResolution_y = new TH1D("h_RefFinalHTResolution_y","RefFinalHTResolution(y)",m_nbinres1,-m_scares1,m_scares1);
  h_RefFinalHTResolution_EtMiss = new TH1D("h_RefFinalHTResolution_EtMiss","RefFinalHTResolution(EtMiss)",m_nbinres1,-m_scares1,m_scares1);

  // book histo curva resol vs SumET 
  he_resvset = new TH2F("resvset","resolution vs set",100,0.,m_curvasET,100,0.,m_curvasres);

  h_tot = new TH1D("h_tot","h_tot", m_nbinres,-m_scares,m_scares);
  h1_tot = new TH1D("h1_tot","h1_tot", m_nbinres,-m_scares,m_scares);  
  h2_tot = new TH1D("h2_tot","h2_tot", m_nbinres,-m_scares,m_scares);
  h3_tot = new TH1D("h3_tot","h3_tot", m_nbinres,-m_scares,m_scares);
  h4_tot = new TH1D("h4_tot","h4_tot", m_nbinres,-m_scares,m_scares);
  h5_tot = new TH1D("h5_tot","h5_tot", m_nbinres,-m_scares,m_scares);
  h6_tot = new TH1D("h6_tot","h6_tot", m_nbinres,-m_scares,m_scares);
  h7_tot = new TH1D("h7_tot","h7_tot", m_nbinres,-m_scares,m_scares);
  h8_tot = new TH1D("h8_tot","h8_tot", m_nbinres,-m_scares,m_scares);
  h9_tot = new TH1D("h9_tot","h9_tot", m_nbinres,-m_scares,m_scares);
  h10_tot = new TH1D("h10_tot","h10_tot", m_nbinres,-m_scares,m_scares);
  
  h_45_tot = new TH1D("h_45_tot","h_45_tot", m_nbinres,-m_scares,m_scares);
  h1_45_tot = new TH1D("h1_45_tot","h1_45_tot", m_nbinres,-m_scares,m_scares);  
  h2_45_tot = new TH1D("h2_45_tot","h2_45_tot", m_nbinres,-m_scares,m_scares);
  h3_45_tot = new TH1D("h3_45_tot","h3_45_tot", m_nbinres,-m_scares,m_scares);
  h4_45_tot = new TH1D("h4_45_tot","h4_45_tot", m_nbinres,-m_scares,m_scares);
  h5_45_tot = new TH1D("h5_45_tot","h5_45_tot", m_nbinres,-m_scares,m_scares);
  h6_45_tot = new TH1D("h6_45_tot","h6_45_tot", m_nbinres,-m_scares,m_scares);
  h7_45_tot = new TH1D("h7_45_tot","h7_45_tot", m_nbinres,-m_scares,m_scares);
  h8_45_tot = new TH1D("h8_45_tot","h8_45_tot", m_nbinres,-m_scares,m_scares);
  h9_45_tot = new TH1D("h9_45_tot","h9_45_tot", m_nbinres,-m_scares,m_scares);
  h10_45_tot = new TH1D("h10_45_tot","h10_45_tot", m_nbinres,-m_scares,m_scares);

/*
  hHTRef_tot = new TH1D("hHTRef_tot","hHTRef_tot", m_nbinres,-m_scares,m_scares);
  hHTRef_45_tot = new TH1D("hHTRef_45_tot","hHTRef_45_tot", m_nbinres,-m_scares,m_scares);
  hHTReft_tot = new TH1D("hHTReft_tot","hHTReft_tot", 150,0.,150.);
  hHTReft_45_tot = new TH1D("hHTReft_45_tot","hHTReft_45_tot", 150,0.,150.);
*/
 
  hHT_tot = new TH1D("hHT_tot","hHT_tot", m_nbinres,-m_scares,m_scares);
  hHTt_tot = new TH1D("hHTt_tot","hHTt_tot", 150,0.,150.);
  hHT_tot7 = new TH1D("hHT_tot7","hHT_tot7", m_nbinres,-m_scares,m_scares);
  hHTt_tot7 = new TH1D("hHTt_tot7","hHTt_tot7", 150,0.,150.);
  hHT_tot9 = new TH1D("hHT_tot9","hHT_tot9", m_nbinres,-m_scares,m_scares);
  hHTt_tot9 = new TH1D("hHTt_tot9","hHTt_tot9", 150,0.,150.);

  hHT_45_tot = new TH1D("hHT_45_tot","hHT_45_tot", m_nbinres,-m_scares,m_scares);
  hHTt_45_tot = new TH1D("hHTt_45_tot","hHTt_45_tot", 150,0.,150.);
  hHT_45_tot7 = new TH1D("hHT_45_tot7","hHT_45_tot7", m_nbinres,-m_scares,m_scares);
  hHTt_45_tot7 = new TH1D("hHTt_45_tot7","hHTt_45_tot7", 150,0.,150.);
  hHT_45_tot9 = new TH1D("hHT_45_tot9","hHT_45_tot9", m_nbinres,-m_scares,m_scares);
  hHTt_45_tot9 = new TH1D("hHTt_45_tot9","hHTt_45_tot9", 150,0.,150.);

/*
  hHTLCW_pt20_tot = new TH1D("hHTLCW_pt20_tot","hHTLCW_pt20_tot", m_nbinres,-m_scares,m_scares);
  hHTLCW_pt20_45_tot = new TH1D("hHTLCW_pt20_45_tot","hHTLCW_pt20_45_tot", m_nbinres,-m_scares,m_scares);
  hHTLCW_pt20t_tot = new TH1D("hHTLCW_pt20t_tot","hHTLCW_pt20t_tot", 150,0.,150.);
  hHTLCW_pt20t_45_tot = new TH1D("hHTLCW_pt20t_45_tot","hHTLCW_pt20t_45_tot", 150,0.,150.);
*/  
 
  h_sumetT = new TH1D("h_sumetT","h_sumetT", m_nbinres,m_lowET,m_suET); //sumet truth
  h_sumet = new TH1D("h_sumet","h_sumet", m_nbinres,m_lowET,m_suET);
  h1_sumet = new TH1D("h1_sumet","h1_sumet", m_nbinres,m_lowET,m_suET);
  h2_sumet = new TH1D("h2_sumet","h2_sumet", m_nbinres,m_lowET,m_suET);
  h3_sumet = new TH1D("h3_sumet","h3_sumet", m_nbinres,m_lowET,m_suET);
  h4_sumet = new TH1D("h4_sumet","h4_sumet", m_nbinres,m_lowET,m_suET);
  h5_sumet = new TH1D("h5_sumet","h5_sumet", m_nbinres,m_lowET,m_suET);
  h6_sumet = new TH1D("h6_sumet","h6_sumet", m_nbinres,m_lowET,m_suET);
  h7_sumet = new TH1D("h7_sumet","h7_sumet", m_nbinres,m_lowET,m_suET);
  h8_sumet = new TH1D("h8_sumet","h8_sumet", m_nbinres,m_lowET,m_suET);
  h9_sumet = new TH1D("h9_sumet","h9_sumet", m_nbinres,m_lowET,m_suET);
  h10_sumet = new TH1D("h10_sumet","h10_sumet", m_nbinres,m_lowET,m_suET);
  
  h_45_sumet = new TH1D("h_45_sumet","h_45_sumet", m_nbinres,m_lowET,m_suET);
  h1_45_sumet = new TH1D("h1_45_sumet","h1_45_sumet", m_nbinres,m_lowET,m_suET);
  h2_45_sumet = new TH1D("h2_45_sumet","h2_45_sumet", m_nbinres,m_lowET,m_suET);
  h3_45_sumet = new TH1D("h3_45_sumet","h3_45_sumet", m_nbinres,m_lowET,m_suET);
  h4_45_sumet = new TH1D("h4_45_sumet","h4_45_sumet", m_nbinres,m_lowET,m_suET);
  h5_45_sumet = new TH1D("h5_45_sumet","h5_45_sumet", m_nbinres,m_lowET,m_suET);
  h6_45_sumet = new TH1D("h6_45_sumet","h6_45_sumet", m_nbinres,m_lowET,m_suET);
  h7_45_sumet = new TH1D("h7_45_sumet","h7_45_sumet", m_nbinres,m_lowET,m_suET);
  h8_45_sumet = new TH1D("h8_45_sumet","h8_45_sumet", m_nbinres,m_lowET,m_suET);
  h9_45_sumet = new TH1D("h9_45_sumet","h9_45_sumet", m_nbinres,m_lowET,m_suET);
  h10_45_sumet = new TH1D("h10_45_sumet","h10_45_sumet", m_nbinres,m_lowET,m_suET);

  h_sumetHT = new TH1D("h_sumetHT","h_sumetHT", m_nbinres,m_lowET,m_suET);
  h_sumetHT7 = new TH1D("h_sumetHT7","h_sumetHT7", m_nbinres,m_lowET,m_suET);
  h_sumetHT9 = new TH1D("h_sumetHT9","h_sumetHT9", m_nbinres,m_lowET,m_suET);
  h_sumetHT_45 = new TH1D("h_sumetHT_45","h_sumetHT_45", m_nbinres,m_lowET,m_suET);
  h_sumetHT7_45 = new TH1D("h_sumetHT7_45","h_sumetHT7_45", m_nbinres,m_lowET,m_suET);
  h_sumetHT9_45 = new TH1D("h_sumetHT9_45","h_sumetHT9_45", m_nbinres,m_lowET,m_suET);
/*
  h_sumetHTRef = new TH1D("h_sumetHTRef","h_sumetHTRef", m_nbinres,m_lowET,m_suET);
  h_sumetHTRef_45 = new TH1D("h_sumetHTRef_45","h_sumetHTRef_45", m_nbinres,m_lowET,m_suET);

  h_sumetHTLCW_pt20 = new TH1D("h_sumetHTLCW_pt20","h_sumetHTLCW_pt20", m_nbinres,m_lowET,m_suET);
  h_sumetHTLCW_pt20_45 = new TH1D("h_sumetHTLCW_pt20_45","h_sumetHTLCW_pt20_45", m_nbinres,m_lowET,m_suET);
*/

  //Profile plots ratio calibs
  h_METGCWdivMETem = new  TProfile("h_METGCWdivMETem","h_METGCWdivMETem",
				   100,m_lowET,m_suET, 0., 2.,"");
  h_METLCdivMETem = new  TProfile("h_METLCdivMETem","METLCdivMET vs SumETem",
				  100,m_lowET,m_suET, 0., 2.,"");
  h_METRefdivMETem = new  TProfile("h_METRefdivMETem","METRefdivMET vs SumETem",
				   100,m_lowET,m_suET, 0., 2.,"");

  //Apr2010
  h_SETHTovSET = new  TProfile("h_SETHTovSET","SETHTovSET vs SET",
			       100,m_lowET,m_suET, 0., 1.,"");
  h_FCALfvSET = new  TProfile("h_FCALfvSET","FCALf vs SET",
			      100,m_lowET,m_suET, 0., 1.,"");
  h_METem = new  TProfile("h_METem","METem vs SumETem",
			  100,m_lowET,m_suET, 0., 50.,"");


  //histo pt
  h_leading_pt = new TH1D("h_leading_pt","h_leading_pt", 100,m_lowET,m_suET);
  h_subleading_pt = new TH1D("h_subleading_pt","h_subleading_pt", 100,m_lowET,m_suET);
  h_sumpt = new TH1D("h_sumpt","h_sumpt", 400,m_lowET,4*m_suET); 
  h_jet_pt = new TH1D("h_jet_pt","h_jet_pt", 100,m_lowET,m_suET); 


  //histos FCALf and ratioHT
  float r_lowET=-0.5;
  float r_suET=1.5;
  h_FCALf = new TH1D("h_FCALf","h_FCALf", 100,r_lowET,r_suET);
  h_ratioHT = new TH1D("h_ratioHT","h_ratioHT", 100,0,1.);
  h_ratioHTRef = new TH1D("h_ratioHTRef","h_ratioHTRef", 100,0,1.);

  //PETER plots
  h_CellOutf = new TH1D("h_CellOutf","h_CellOutf", 100,r_lowET,r_suET);
  h_SoftJetsf = new TH1D("h_SoftJetsf","h_SoftJetsf", 100,r_lowET,r_suET);
  h_RefJetf = new TH1D("h_RefJetf","h_RefJetf", 100,r_lowET,r_suET);
  
  h_CellOutf_45 = new TH1D("h_CellOutf_45","h_CellOutf_45", 100,r_lowET,r_suET);
  h_SoftJetsf_45 = new TH1D("h_SoftJetsf_45","h_SoftJetsf_45", 100,r_lowET,r_suET);
  h_RefJetf_45 = new TH1D("h_RefJetf_45","h_RefJetf_45", 100,r_lowET,r_suET);

  //donatella Resolution curves
  //curvaresol---------------------------------
  m_curvasET=m_startpt+m_numptt*m_binpt;


  ///////////////////////////////////////////////////////////////////////////////////////////////////////////
  //Dichiaro string per i nomi degli istogrammi

  //--------------------------------------------------------------------------------------------------------
  //curve resol vs sumet
  std::string name;
  std::string name1;
  std::string name2;
  std::string name3;
  std::string name4;
  std::string name5;
  std::string name6;
  std::string name7;
  std::string name8;
  std::string name9;
  std::string name10;

  //PETER plots
  //curve CellOut fraction vs sumet
  std::string nameco;
  std::string namesj;
  std::string namerj;

  //curve resol vs sumet eta<4.5
  std::string nameco_45;
  std::string namesj_45;
  std::string namerj_45;
  std::string name_45;
  std::string name1_45;
  std::string name2_45;
  std::string name3_45;
  std::string name4_45;
  std::string name5_45;
  std::string name6_45;
  std::string name7_45;
  std::string name8_45;
  std::string name9_45;
  std::string name10_45;
  
  //curve bias vs sumet
  std::string namebias;  
  std::string namebias1;
  std::string namebias2;
  std::string namebias3;
  std::string namebias4;
  std::string namebias5;
  std::string namebias6;
  std::string namebias7;
  std::string namebias8;
  std::string namebias9;
  std::string namebias10;

  //plot HT  --> senza CellOut
  std::string nameHT;
  std::string nameHT7;
  std::string nameHT9;

  std::string nameHT_45;
  std::string nameHT7_45;
  std::string nameHT9_45;

  //
  std::string name21;
  std::string name22;
  std::string name23;
  std::string name24;
  std::string name25;
  

  //--------------------------------------------------------------------------------------------------------
  //Sven Menke plot
  //ResMET*SumET_at_em_scale/SumET_at_calibrated_scale vs SumET_at_em_scale
  //book here plots of SumET_at_em-scale/SumET_at_calibrated-scale
  //in each Sumet bin
  std::string namer; 
  std::string namer1;
  std::string namer2;
  std::string namer3;
  std::string namer4;
  std::string namer5;
  std::string namer6;
  std::string namer7;
  std::string namer8;
  std::string namer9;
  std::string namer10;
  
  //Sven Menke plot eta<4.5
  std::string namer_45; 
  std::string namer1_45;
  std::string namer2_45;
  std::string namer3_45;
  std::string namer4_45;
  std::string namer5_45;
  std::string namer6_45;
  std::string namer7_45;
  std::string namer8_45;
  std::string namer9_45;
  std::string namer10_45;

  //plot HT  --> senza CellOut  
  std::string namerHT;
  std::string namerHT7;
  std::string namerHT9;

  std::string namerHT_45;
  std::string namerHT7_45;
  std::string namerHT9_45;


  //--------------------------------------------------------------------------------------------------------
  //MET_at_em_scale/MET_at_calibrated_scale
  std::string namep;
  std::string namep1;
  std::string namep2;
  std::string namep3;
  std::string namep4;
  std::string namep5;
  std::string namep6;
  std::string namep7;
  std::string namep8;
  std::string namep9;
  std::string namep10;

  //MET_at_em_scale/MET_at_calibrated_scale eta<4.5
  std::string namep_45;
  std::string namep1_45;
  std::string namep2_45;
  std::string namep3_45;
  std::string namep4_45;
  std::string namep5_45;
  std::string namep6_45;
  std::string namep7_45;
  std::string namep8_45;
  std::string namep9_45;
  std::string namep10_45;

  //------------------------------------------------------------------------
  //correct SumEt bins
  std::string namesu;
  std::string namesu1;
  std::string namesu2;
  std::string namesu3;
  std::string namesu4;
  std::string namesu5;
  std::string namesu6;
  std::string namesu7;
  std::string namesu8;
  std::string namesu9;
  std::string namesu10;
 
  //------------------------------------------------------------------------
  //number of cells
  std::string namenc;
  std::string namenc1;
  std::string namenc2;
  std::string namenc3;
  std::string namenc4;
  std::string namenc5;
  std::string namenc6;
  std::string namenc7;
  std::string namenc8;
  std::string namenc9;
  std::string namenc10;
  
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////
  
    
  dir = dir->mkdir("Binned");

  if (m_folderName!="") {
    outfile->cd((m_folderName+"/Resolution/Binned").c_str());
  } else {
    outfile->cd("Resolution/Binned");
  }
  
  
  
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////
  //Dichiaro gli istogrammi

  for (int ih= 0; ih< m_numptt ; ih++){
  
    //--------------------------------------------------------------------------------------------------------
    //curve resol vs sumet
    name = Form("h_%d",ih);
    h_.push_back(new TH1F(name.c_str(), name.c_str() , m_nbinres,-m_scares,m_scares));
    name1 = Form("h1_%d",ih);
    h1_.push_back(new TH1F(name1.c_str(), name1.c_str() , m_nbinres,-m_scares,m_scares));
    name2 = Form("h2_%d",ih);
    h2_.push_back(new TH1F(name2.c_str(), name2.c_str() , m_nbinres,-m_scares,m_scares));
    name3 = Form("h3_%d",ih);
    h3_.push_back(new TH1F(name3.c_str(), name3.c_str() , m_nbinres,-m_scares,m_scares));
    name4 = Form("h4_%d",ih);
    h4_.push_back(new TH1F(name4.c_str(), name4.c_str() , m_nbinres,-m_scares,m_scares));
    name5 = Form("h5_%d",ih);
    h5_.push_back(new TH1F(name5.c_str(), name5.c_str() , m_nbinres,-m_scares,m_scares));
    name6 = Form("h6_%d",ih);
    h6_.push_back(new TH1F(name6.c_str(), name6.c_str() , m_nbinres,-m_scares,m_scares));
    name7 = Form("h7_%d",ih);
    h7_.push_back(new TH1F(name7.c_str(), name7.c_str() , m_nbinres,-m_scares,m_scares));
    name8 = Form("h8_%d",ih);
    h8_.push_back(new TH1F(name8.c_str(), name8.c_str() , m_nbinres,-m_scares,m_scares));
    name9 = Form("h9_%d",ih);
    h9_.push_back(new TH1F(name9.c_str(), name9.c_str() , m_nbinres,-m_scares,m_scares));
    name10 = Form("h10_%d",ih);
    h10_.push_back(new TH1F(name10.c_str(), name10.c_str() , m_nbinres,-m_scares,m_scares));
    
    //curve resol vs sumet eta<4.5
    name_45 = Form("h_45_%d",ih);
    h_45_.push_back(new TH1F(name_45.c_str(), name_45.c_str() , m_nbinres,-m_scares,m_scares));    
    name1_45 = Form("h1_45_%d",ih);
    h1_45_.push_back(new TH1F(name1_45.c_str(), name1_45.c_str() , m_nbinres,-m_scares,m_scares));    
    name2_45 = Form("h2_45_%d",ih);
    h2_45_.push_back(new TH1F(name2_45.c_str(), name2_45.c_str() , m_nbinres,-m_scares,m_scares));    
    name3_45 = Form("h3_45_%d",ih);
    h3_45_.push_back(new TH1F(name3_45.c_str(), name3_45.c_str() , m_nbinres,-m_scares,m_scares));    
    name4_45 = Form("h4_45_%d",ih);
    h4_45_.push_back(new TH1F(name4_45.c_str(), name4_45.c_str() , m_nbinres,-m_scares,m_scares));    
    name5_45 = Form("h5_45_%d",ih);
    h5_45_.push_back(new TH1F(name5_45.c_str(), name5_45.c_str() , m_nbinres,-m_scares,m_scares));    
    name6_45 = Form("h6_45_%d",ih);
    h6_45_.push_back(new TH1F(name6_45.c_str(), name6_45.c_str() , m_nbinres,-m_scares,m_scares));    
    name7_45 = Form("h7_45_%d",ih);
    h7_45_.push_back(new TH1F(name7_45.c_str(), name7_45.c_str() , m_nbinres,-m_scares,m_scares));    
    name8_45 = Form("h8_45_%d",ih);
    h8_45_.push_back(new TH1F(name8_45.c_str(), name8_45.c_str() , m_nbinres,-m_scares,m_scares));    
    name9_45 = Form("h9_45_%d",ih);
    h9_45_.push_back(new TH1F(name9_45.c_str(), name9_45.c_str() , m_nbinres,-m_scares,m_scares));    
    name10_45 = Form("h10_45_%d",ih);
    h10_45_.push_back(new TH1F(name10_45.c_str(), name10_45.c_str() , m_nbinres,-m_scares,m_scares));    

    //--------------------------------------------------------------------------------------------------------
    //curve bias vs sumet
    
    namebias = Form("hbias_%d",ih);
    hbias_.push_back(new TH1F(namebias.c_str(), namebias.c_str() , m_nbinres,-m_scares,m_scares));
    namebias1 = Form("hbias1_%d",ih);
    hbias1_.push_back(new TH1F(namebias1.c_str(), namebias1.c_str() , m_nbinres,-m_scares,m_scares));
    namebias2 = Form("hbias2_%d",ih);
    hbias2_.push_back(new TH1F(namebias2.c_str(), namebias2.c_str() , m_nbinres,-m_scares,m_scares));
    namebias3 = Form("hbias3_%d",ih);
    hbias3_.push_back(new TH1F(namebias3.c_str(), namebias3.c_str() , m_nbinres,-m_scares,m_scares));
    namebias4 = Form("hbias4_%d",ih);
    hbias4_.push_back(new TH1F(namebias4.c_str(), namebias4.c_str() , m_nbinres,-m_scares,m_scares));
    namebias5 = Form("hbias5_%d",ih);
    hbias5_.push_back(new TH1F(namebias5.c_str(), namebias5.c_str() , m_nbinres,-m_scares,m_scares));
    namebias6 = Form("hbias6_%d",ih);
    hbias6_.push_back(new TH1F(namebias6.c_str(), namebias6.c_str() , m_nbinres,-m_scares,m_scares));
    namebias7 = Form("hbias7_%d",ih);
    hbias7_.push_back(new TH1F(namebias7.c_str(), namebias7.c_str() , m_nbinres,-m_scares,m_scares));
    namebias8 = Form("hbias8_%d",ih);
    hbias8_.push_back(new TH1F(namebias8.c_str(), namebias8.c_str() , m_nbinres,-m_scares,m_scares));
    namebias9 = Form("hbias9_%d",ih);
    hbias9_.push_back(new TH1F(namebias9.c_str(), namebias9.c_str() , m_nbinres,-m_scares,m_scares));
    namebias10 = Form("hbias10_%d",ih);
    hbias10_.push_back(new TH1F(namebias10.c_str(), namebias10.c_str() , m_nbinres,-m_scares,m_scares));
    
    //--------------------------------------------------------------------------------------------------------
 
    
    nameHT = Form("hHT_%d",ih);
    hHT_.push_back(new TH1F(nameHT.c_str(), nameHT.c_str() , m_nbinres,-m_scares,m_scares));
    nameHT7 = Form("hHT7_%d",ih);
    hHT7_.push_back(new TH1F(nameHT7.c_str(), nameHT7.c_str() , m_nbinres,-m_scares,m_scares));
    nameHT9 = Form("hHT9_%d",ih);
    hHT9_.push_back(new TH1F(nameHT9.c_str(), nameHT9.c_str() , m_nbinres,-m_scares,m_scares));
 
    nameHT_45 = Form("hHT_45_%d",ih);
    hHT_45_.push_back(new TH1F(nameHT_45.c_str(), nameHT_45.c_str() , m_nbinres,-m_scares,m_scares));
    nameHT7_45 = Form("hHT7_45_%d",ih);
    hHT7_45_.push_back(new TH1F(nameHT7_45.c_str(), nameHT7_45.c_str() , m_nbinres,-m_scares,m_scares));
    nameHT9_45 = Form("hHT9_45_%d",ih);
    hHT9_45_.push_back(new TH1F(nameHT9_45.c_str(), nameHT9_45.c_str() , m_nbinres,-m_scares,m_scares));
    
    //--------------------------------------------------------------------------------------------------------


    
    name21 = Form("h21_%d",ih);
    h21_.push_back(new TH1F(name21.c_str(), name21.c_str() , m_nbinres,-m_scares,m_scares));
    name22 = Form("h22_%d",ih);
    h22_.push_back(new TH1F(name22.c_str(), name22.c_str() , m_nbinres,-m_scares,m_scares));

    name23 = Form("h23_%d",ih);
    h23_.push_back(new TH1F(name23.c_str(), name23.c_str() , m_nbinres,-m_scares,m_scares));
    name24 = Form("h24_%d",ih);
    h24_.push_back(new TH1F(name24.c_str(), name24.c_str() , m_nbinres,-m_scares,m_scares));
    name25 = Form("h25_%d",ih);
    h25_.push_back(new TH1F(name25.c_str(), name25.c_str() , m_nbinres,-m_scares,m_scares));
  
    //--------------------------------------------------------------------------------------------------------
    //Sven Menke plot
    namer = Form("r_%d",ih);
    r_.push_back(new TH1F(namer.c_str(), namer.c_str() , 100,0.,2.));
    namer1 = Form("r1_%d",ih);
    r1_.push_back(new TH1F(namer1.c_str(), namer1.c_str() , 100,0.,2.));
    namer2 = Form("r2_%d",ih);
    r2_.push_back(new TH1F(namer2.c_str(), namer2.c_str() , 100,0.,2.));
    namer3 = Form("r3_%d",ih);
    r3_.push_back(new TH1F(namer3.c_str(), namer3.c_str() , 100,0.,2.));
    namer4 = Form("r4_%d",ih);
    r4_.push_back(new TH1F(namer4.c_str(), namer4.c_str() , 100,0.,2.));
    namer5 = Form("r5_%d",ih);
    r5_.push_back(new TH1F(namer5.c_str(), namer5.c_str() , 100,0.,2.));
    namer6 = Form("r6_%d",ih);
    r6_.push_back(new TH1F(namer6.c_str(), namer6.c_str() , 100,0.,2.));
    namer7 = Form("r7_%d",ih);
    r7_.push_back(new TH1F(namer7.c_str(), namer7.c_str() , 100,0.,2.));
    namer8 = Form("r8_%d",ih);
    r8_.push_back(new TH1F(namer8.c_str(), namer8.c_str() , 100,0.,2.));
    namer9 = Form("r9_%d",ih);
    r9_.push_back(new TH1F(namer9.c_str(), namer9.c_str() , 100,0.,2.));
    namer10 = Form("r10_%d",ih);
    r10_.push_back(new TH1F(namer10.c_str(), namer10.c_str() , 100,0.,2.));
 
    
    namer_45 = Form("r_45_%d",ih);
    r_45_.push_back(new TH1F(namer_45.c_str(), namer.c_str() , 100,0.,2.));
    namer1_45 = Form("r1_45_%d",ih);
    r1_45_.push_back(new TH1F(namer1_45.c_str(), namer1_45.c_str() , 100,0.,2.));
    namer2_45 = Form("r2_45_%d",ih);
    r2_45_.push_back(new TH1F(namer2_45.c_str(), namer2_45.c_str() , 100,0.,2.));
    namer3_45 = Form("r3_45_%d",ih);
    r3_45_.push_back(new TH1F(namer3_45.c_str(), namer3_45.c_str() , 100,0.,2.));
    namer4_45 = Form("r4_45_%d",ih);
    r4_45_.push_back(new TH1F(namer4_45.c_str(), namer4_45.c_str() , 100,0.,2.));
    namer5_45 = Form("r5_45_%d",ih);
    r5_45_.push_back(new TH1F(namer5_45.c_str(), namer5_45.c_str() , 100,0.,2.));
    namer6_45 = Form("r6_45_%d",ih);
    r6_45_.push_back(new TH1F(namer6_45.c_str(), namer6_45.c_str() , 100,0.,2.));
    namer7_45 = Form("r7_45_%d",ih);
    r7_45_.push_back(new TH1F(namer7_45.c_str(), namer7_45.c_str() , 100,0.,2.));
    namer8_45 = Form("r8_45_%d",ih);
    r8_45_.push_back(new TH1F(namer8_45.c_str(), namer8_45.c_str() , 100,0.,2.));
    namer9_45 = Form("r9_45_%d",ih);
    r9_45_.push_back(new TH1F(namer9_45.c_str(), namer9_45.c_str() , 100,0.,2.));
    namer10_45 = Form("r10_45_%d",ih);
    r10_45_.push_back(new TH1F(namer10_45.c_str(), namer10_45.c_str() , 100,0.,2.));
 

    namerHT = Form("rHT_%d",ih);
    rHT_.push_back(new TH1F(namerHT.c_str(), namerHT.c_str() , 100,0.,2.));
    namerHT7 = Form("rHT7_%d",ih);
    rHT7_.push_back(new TH1F(namerHT7.c_str(), namerHT7.c_str() , 100,0.,2.));
    namerHT9 = Form("rHT9_%d",ih);
    rHT9_.push_back(new TH1F(namerHT9.c_str(), namerHT9.c_str() , 100,0.,2.));

    namerHT_45 = Form("rHT_45_%d",ih);
    rHT_45_.push_back(new TH1F(namerHT_45.c_str(), namerHT_45.c_str() , 100,0.,2.));
    namerHT7_45 = Form("rHT7_45_%d",ih);
    rHT7_45_.push_back(new TH1F(namerHT7_45.c_str(), namerHT7_45.c_str() , 100,0.,2.));
    namerHT9_45 = Form("rHT9_45_%d",ih);
    rHT9_45_.push_back(new TH1F(namerHT9_45.c_str(), namerHT9_45.c_str() , 100,0.,2.));
 

    //--------------------------------------------------------------------------------------------------------
    //MET_at_em_scale/MET_at_calibrated_scale 
    namep = Form("p_%d",ih);
    p_.push_back(new TH1F(namep.c_str(), namep.c_str() , 100,0.,2.));
    namep1 = Form("p1_%d",ih);
    p1_.push_back(new TH1F(namep1.c_str(), namep1.c_str() , 100,0.,2.));
    namep3 = Form("p3_%d",ih);
    p3_.push_back(new TH1F(namep3.c_str(), namep3.c_str() , 100,0.,2.));
    namep4 = Form("p4_%d",ih);
    p4_.push_back(new TH1F(namep4.c_str(), namep4.c_str() , 100,0.,2.));
    namep5 = Form("p5_%d",ih);
    p5_.push_back(new TH1F(namep5.c_str(), namep5.c_str() , 100,0.,2.));
    namep6 = Form("p6_%d",ih);
    p6_.push_back(new TH1F(namep6.c_str(), namep6.c_str() , 100,0.,2.));
    namep7 = Form("p7_%d",ih);
    p7_.push_back(new TH1F(namep7.c_str(), namep7.c_str() , 100,0.,2.));
    namep8 = Form("p8_%d",ih);
    p8_.push_back(new TH1F(namep8.c_str(), namep8.c_str() , 100,0.,2.));
    namep9 = Form("p9_%d",ih);
    p9_.push_back(new TH1F(namep9.c_str(), namep9.c_str() , 100,0.,2.));
    namep10 = Form("p10_%d",ih);
    p10_.push_back(new TH1F(namep10.c_str(), namep10.c_str() , 100,0.,2.));
    
    namep_45 = Form("p_45_%d",ih);
    p_45_.push_back(new TH1F(namep_45.c_str(), namep_45.c_str() , 100,0.,2.));
    namep1_45 = Form("p1_45_%d",ih);
    p1_45_.push_back(new TH1F(namep1_45.c_str(), namep1_45.c_str() , 100,0.,2.));
    namep3_45 = Form("p3_45_%d",ih);
    p3_45_.push_back(new TH1F(namep3_45.c_str(), namep3_45.c_str() , 100,0.,2.));
    namep4_45 = Form("p4_45_%d",ih);
    p4_45_.push_back(new TH1F(namep4_45.c_str(), namep4_45.c_str() , 100,0.,2.));
    namep5_45 = Form("p5_45_%d",ih);
    p5_45_.push_back(new TH1F(namep5_45.c_str(), namep5_45.c_str() , 100,0.,2.));
    namep6_45 = Form("p6_45_%d",ih);
    p6_45_.push_back(new TH1F(namep6_45.c_str(), namep6_45.c_str() , 100,0.,2.));
    namep7_45 = Form("p7_45_%d",ih);
    p7_45_.push_back(new TH1F(namep7_45.c_str(), namep7_45.c_str() , 100,0.,2.));
    namep8_45 = Form("p8_45_%d",ih);
    p8_45_.push_back(new TH1F(namep8_45.c_str(), namep8_45.c_str() , 100,0.,2.));
    namep9_45 = Form("p9_45_%d",ih);
    p9_45_.push_back(new TH1F(namep9_45.c_str(), namep9_45.c_str() , 100,0.,2.));
    namep10_45 = Form("p10_45_%d",ih);
    p10_45_.push_back(new TH1F(namep10_45.c_str(), namep10_45.c_str() , 100,0.,2.));


    //--------------------------------------------------------------------------------------------------------
    //correct SumEt bins
    namesu = Form("su_%d",ih);
    su_.push_back(new TH1F(namesu.c_str(), namesu.c_str() , 100,m_lowET,m_suET));
    namesu1 = Form("su1_%d",ih);
    su1_.push_back(new TH1F(namesu1.c_str(), namesu1.c_str() , 100,m_lowET,m_suET));
    namesu2 = Form("su2_%d",ih);
    su2_.push_back(new TH1F(namesu2.c_str(), namesu2.c_str() , 100,m_lowET,m_suET));
    namesu3 = Form("su3_%d",ih);
    su3_.push_back(new TH1F(namesu3.c_str(), namesu3.c_str() , 100,m_lowET,m_suET));
    namesu4 = Form("su4_%d",ih);
    su4_.push_back(new TH1F(namesu4.c_str(), namesu4.c_str() , 100,m_lowET,m_suET));
    namesu5 = Form("su5_%d",ih);
    su5_.push_back(new TH1F(namesu5.c_str(), namesu5.c_str() , 100,m_lowET,m_suET));
    namesu6 = Form("su6_%d",ih);
    su6_.push_back(new TH1F(namesu6.c_str(), namesu6.c_str() , 100,m_lowET,m_suET));
    namesu7 = Form("su7_%d",ih);
    su7_.push_back(new TH1F(namesu7.c_str(), namesu7.c_str() , 100,m_lowET,m_suET));
    namesu8 = Form("su8_%d",ih);
    su8_.push_back(new TH1F(namesu8.c_str(), namesu8.c_str() , 100,m_lowET,m_suET));
    namesu9 = Form("su9_%d",ih);
    su9_.push_back(new TH1F(namesu9.c_str(), namesu9.c_str() , 100,m_lowET,m_suET));
    namesu10 = Form("su10_%d",ih);
    su10_.push_back(new TH1F(namesu10.c_str(), namesu10.c_str() , 100,m_lowET,m_suET));
    
    
    //--------------------------------------------------------------------------------------------------------
    //number of cells
    namenc = Form("nc_%d",ih);
    nc_.push_back(new TH1F(namenc.c_str(), namenc.c_str() , 100,0.,500.));
    namenc1 = Form("nc1_%d",ih);
    nc1_.push_back(new TH1F(namenc1.c_str(), namenc1.c_str() , 100,0.,500.));
    namenc2 = Form("nc2_%d",ih);
    nc2_.push_back(new TH1F(namenc2.c_str(), namenc2.c_str() , 100,0.,500.));
    namenc3 = Form("nc3_%d",ih);
    nc3_.push_back(new TH1F(namenc3.c_str(), namenc3.c_str() , 100,0.,500.));
    namenc4 = Form("nc4_%d",ih);
    nc4_.push_back(new TH1F(namenc4.c_str(), namenc4.c_str() , 100,0.,500.));
    namenc5 = Form("nc5_%d",ih);
    nc5_.push_back(new TH1F(namenc5.c_str(), namenc5.c_str() , 100,0.,500.));
    namenc6 = Form("nc6_%d",ih);
    nc6_.push_back(new TH1F(namenc6.c_str(), namenc6.c_str() , 100,0.,500.));
    namenc7 = Form("nc7_%d",ih);
    nc7_.push_back(new TH1F(namenc7.c_str(), namenc7.c_str() , 100,0.,500.));
    namenc8 = Form("nc8_%d",ih);
    nc8_.push_back(new TH1F(namenc8.c_str(), namenc8.c_str() , 100,0.,500.));
    namenc9 = Form("nc9_%d",ih);
    nc9_.push_back(new TH1F(namenc9.c_str(), namenc9.c_str() , 100,0.,500.));
    namenc10 = Form("nc10_%d",ih);
    nc10_.push_back(new TH1F(namenc10.c_str(), namenc10.c_str() , 100,0.,500.));
 
 
    //PETER plots
    //curve CellOut vs sumet

    nameco = Form("hco_%d",ih);
    hco_.push_back(new TH1F(nameco.c_str(), nameco.c_str() , 100,-0.5,1.5));

    namesj = Form("hsj_%d",ih);
    hsj_.push_back(new TH1F(namesj.c_str(), namesj.c_str() , 100,-0.5,1.5));

    namerj = Form("hrj_%d",ih);
    hrj_.push_back(new TH1F(namerj.c_str(), namerj.c_str() , 100,-0.5,1.5));

    //eta<4.5
    nameco_45 = Form("hco_45_%d",ih);
    hco_45_.push_back(new TH1F(nameco_45.c_str(), nameco_45.c_str() , 100,-0.5,1.5));

    namesj_45 = Form("hsj_45_%d",ih);
    hsj_45_.push_back(new TH1F(namesj_45.c_str(), namesj_45.c_str() , 100,-0.5,1.5));
      
    namerj_45 = Form("hrj_45_%d",ih);
    hrj_45_.push_back(new TH1F(namerj_45.c_str(), namerj_45.c_str() , 100,-0.5,1.5));
      
 }//fine for su m_numptt

}

void ResolutionTool::execute(MissingETData *data) {
  resolution(data);
}

void ResolutionTool::resolution(MissingETData *data) { 
 

  //cout<<"RES cellOut_Eflow_eta45 = "<< data->cellOut_Eflow()->sumet_eta45()<<endl;
  //cout<<"RES softJets_eta45 = "<< data->softJets()->sumet_eta45()<<endl;
  //cout<<"RES refJet_eta45 = "<< data->refJet()->sumet_eta45()<<endl;
  //cout<<"RES refFinal_eta45 = "<< data->refFinal()->sumet_eta45() <<endl; 




  // histo pt
  float sumpt=0.;
  //float sumpx=0.;
  //float sumpy=0.;
  
  for (Int_t i = 0; i < data->njets(); ++i) {
      //cout<<"n_jets = "<< data->njets()  <<endl;
      //cout<<"JetPt(i) = "<<  data->JetPt(i) <<endl;
      h_jet_pt->Fill((data->JetPt(i))/1000.);
      sumpt = sumpt + data->JetPt(i);
      //sumpx = sumpx + data->JetPt(i)*cos(data->JetPhi(i));
      //sumpy = sumpy + data->JetPt(i)*sin(data->JetPhi(i));
  }
  
  if (data->njets()>0) h_leading_pt -> Fill(data->JetPt(0)/1000.);  
  if (data->njets()>1) h_subleading_pt -> Fill(data->JetPt(1)/1000.); 

  h_sumpt -> Fill(sumpt/1000.);  
  



  //PETER plots
  if (data->refFinal()->sumet_eta45()>0.){
        
	float frac_CellOut=(data->cellOut_Eflow()->sumet_eta45())/(data->refFinal()->sumet_eta45());
	float frac_SoftJets=(data->softJets()->sumet_eta45())/(data->refFinal()->sumet_eta45());
	float frac_RefJet=(data->refJet()->sumet_eta45())/(data->refFinal()->sumet_eta45());
	
	//cout<<"RES frac_CellOut = "<<frac_CellOut<<endl;
	//cout<<"RES frac_SoftJets = "<<frac_SoftJets<<endl;
	//cout<<"RES frac_RefJet = "<<frac_RefJet<<endl;

	
  	h_CellOutf_45  -> Fill(frac_CellOut);
  	h_SoftJetsf_45 -> Fill(frac_SoftJets);
  	h_RefJetf_45 -> Fill(frac_RefJet);

    
  }
  
  if (data->refFinal()->sumet()>0.){
  	h_CellOutf  -> Fill(data->cellOut_Eflow()->sumet()/data->refFinal()->sumet());
  	h_SoftJetsf -> Fill(data->softJets()->sumet()/data->refFinal()->sumet());
  	h_RefJetf -> Fill(data->refJet()->sumet()/data->refFinal()->sumet());
  }
  

  float dphi = fabs(data->truth_nonInt()->phi() - data->refFinal()->phi());
  if (dphi > M_PI) dphi = fabs(dphi - 2*M_PI);
  h_RefFinalResolution_phi->Fill(dphi);
  h_MuonBoyResolution_x->Fill((data->truth_muons()->etx() - data->MuonBoy()->etx())/1000.0);
  h_MuonBoyResolution_y->Fill((data->truth_muons()->ety() - data->MuonBoy()->ety())/1000.0);
	
					     
  if (m_useTruth == 1) {
  h_DetOnlyRefFinalResolution_x->Fill( (data->truth_int()->etx()+data->truth_muons()->etx()-data->refFinal()->etx()) / 1000. );
  h_DetOnlyRefFinalResolution_y->Fill( (data->truth_int()->ety()+data->truth_muons()->ety()-data->refFinal()->ety()) / 1000. );
  h_DetOnlyRefFinalResolution_EtMiss->Fill( (sqrt(pow(data->truth_int()->etx()+data->truth_muons()->etx(),2)+
						  pow(data->truth_int()->ety()+data->truth_muons()->ety(),2))-
					     sqrt(pow(data->refFinal()->etx(),2)+pow(data->refFinal()->ety(),2)))/1000. );
  h_DetOnlyRefFinalResolution_V_truth_x->Fill( (data->truth_int()->etx()+data->truth_muons()->etx())/1000. , 
					       (data->truth_int()->etx()+data->truth_muons()->etx()-data->refFinal()->etx()) / 1000. );
  h_DetOnlyRefFinalResolution_V_truth_y->Fill( (data->truth_int()->ety()+data->truth_muons()->ety())/1000. , 
					       (data->truth_int()->ety()+data->truth_muons()->ety()-data->refFinal()->ety()) / 1000. );
  h_DetOnlyRefFinalReco_V_truth_x->Fill( (data->truth_int()->etx()+data->truth_muons()->etx())/1000. , data->refFinal()->etx() / 1000. );
  h_DetOnlyRefFinalReco_V_truth_y->Fill( (data->truth_int()->ety()+data->truth_muons()->ety())/1000. , data->refFinal()->ety() / 1000. );
  h_RefFinalResolution_x->Fill((data->truth_nonInt()->etx() - data->refFinal()->etx())/1000.0);
  h_RefFinalResolution_y->Fill((data->truth_nonInt()->ety() - data->refFinal()->ety())/1000.0);
  h_RefFinalResolution_EtMiss->Fill( (sqrt(pow(data->truth_nonInt()->etx(),2)+pow(data->truth_nonInt()->ety(),2))-
				      sqrt(pow(data->refFinal()->etx(),2)+pow(data->refFinal()->ety(),2)))/1000. );
  h_RefFinalResolution_phi_V_truth_EtMiss->Fill(sqrt(pow(data->truth_nonInt()->etx(),2)+pow(data->truth_nonInt()->ety(),2))/1000.0,dphi);
  h_RefFinalResolution_V_truth_x->Fill( (data->truth_nonInt()->etx())/1000. , 
					       (data->truth_nonInt()->etx()-data->refFinal()->etx()) / 1000. );
  h_RefFinalResolution_V_truth_y->Fill( (data->truth_nonInt()->ety())/1000. , 
					       (data->truth_nonInt()->ety()-data->refFinal()->ety()) / 1000. );
  h_RefFinalReco_V_truth_x->Fill( (data->truth_nonInt()->etx())/1000. , data->refFinal()->etx() / 1000. );
  h_RefFinalReco_V_truth_y->Fill( (data->truth_nonInt()->ety())/1000. , data->refFinal()->ety() / 1000. );
  h_DetOnlyCorrResolution_x->Fill( (data->truth_int()->etx()-(data->calib()->etx()+data->cryoCone()->etx()))/1000. );
  h_DetOnlyCorrResolution_y->Fill( (data->truth_int()->ety()-(data->calib()->ety()+data->cryoCone()->ety()))/1000. );
  h_FinalCorrResolution_x->Fill( (data->truth_nonInt()->etx()-(data->final()->etx()-data->corrTopo()->etx()+data->calib()->etx()))/1000. );
  h_FinalCorrResolution_y->Fill( (data->truth_nonInt()->ety()-(data->final()->ety()-data->corrTopo()->ety()+data->calib()->ety()))/1000. );
  h_DetOnlyCorrResolution_EtMiss->Fill( (sqrt(pow(data->truth_int()->etx(),2)+pow(data->truth_int()->ety(),2))-
					 sqrt(pow(data->calib()->etx()+data->cryoCone()->etx(),2) +
					      pow(data->calib()->ety()+data->cryoCone()->ety(),2)))
					/1000.);
  h_FinalCorrResolution_EtMiss->Fill( (sqrt(pow(data->truth_nonInt()->etx(),2)+pow(data->truth_nonInt()->ety(),2))-
				      sqrt(pow(data->final()->etx()-data->corrTopo()->etx()+data->calib()->etx(),2)+
					   pow(data->final()->ety()-data->corrTopo()->ety()+data->calib()->ety(),2)))
				     /1000.);
  h_MuonBoyResolution_EtMiss->Fill( (sqrt(pow(data->truth_muons()->etx(),2)+pow(data->truth_muons()->ety(),2))-
				     sqrt(pow(data->MuonBoy()->etx(),2)+pow(data->MuonBoy()->ety(),2)))
				    /1000. );
  h_MuonBoyResolution_V_truth_x->Fill( data->truth_muons()->etx()/1000. , 
				       (data->truth_muons()->etx()-data->MuonBoy()->etx()) / 1000. );
  h_MuonBoyResolution_V_truth_y->Fill( data->truth_muons()->ety()/1000. , 
				       (data->truth_muons()->ety()-data->MuonBoy()->ety()) / 1000. );
  h_MuonBoyReco_V_truth_x->Fill( data->truth_muons()->etx()/1000. , data->MuonBoy()->etx() / 1000. );
  h_MuonBoyReco_V_truth_y->Fill( data->truth_muons()->ety()/1000. , data->MuonBoy()->ety() / 1000. );

  }

  float sumHT_emscale=0.;
  float Ncell_tot=0;
//  int Numjet=data->njets();



  Ncell_tot=sqrt( (float)(data->topo()->nCell()) );
  
  

  float Sumet_per_curveHT = 0.;
  float Sumet_per_curveHT7 = 0.;
  float Sumet_per_curveHT9 = 0.;


  float Sumet_per_curvetruth = 0.;
  float Sumet_per_curve = 0.;
  float Sumet_per_curve1 = 0.;
  float Sumet_per_curve2 = 0.;
  float Sumet_per_curve3 = 0.;
  float Sumet_per_curve4 = 0.;
  float Sumet_per_curve5 = 0.;
  float Sumet_per_curve6 = 0.;
  float Sumet_per_curve7 = 0.;
  float Sumet_per_curve8 = 0.;
  float Sumet_per_curve9 = 0.;
  float Sumet_per_curve10 = 0.;

  float Sumet_per_curve_45 = 0.;
  float Sumet_per_curve1_45 = 0.;
  float Sumet_per_curve2_45 = 0.;
  float Sumet_per_curve3_45 = 0.;
  float Sumet_per_curve4_45 = 0.;
  float Sumet_per_curve5_45 = 0.;
  float Sumet_per_curve6_45 = 0.;
  float Sumet_per_curve7_45 = 0.;
  float Sumet_per_curve8_45 = 0.;
  float Sumet_per_curve9_45 = 0.;
  float Sumet_per_curve10_45 = 0.;
  
  float Sumet_per_curveHT_45 = 0.;
  float Sumet_per_curveHT7_45 = 0.;
  float Sumet_per_curveHT9_45 = 0.;

  
  float ExMissEmScale = 0.;
  float EyMissEmScale = 0.;
  float ExMissGlobCalib = 0.;
  float EyMissGlobCalib = 0.;
  float ExMissFinalLocHad = 0.;
  float EyMissFinalLocHad = 0.; 
  
  float ExMissEmScale45 = 0.;
  float EyMissEmScale45 = 0.;
  float ExMissGlobCalib45 = 0.;
  float EyMissGlobCalib45 = 0.;
  float ExMissFinalLocHad45 = 0.;
  float EyMissFinalLocHad45 = 0.;
  
  float ExMissRefFinal = 0.;
  float EyMissRefFinal = 0.;
  float ExMissRefFinal_em = 0.;
  float EyMissRefFinal_em = 0.;
  float ExMissRF_LCW_pt20 = 0.;
  float EyMissRF_LCW_pt20 = 0.;
  float ExMissRF_GCW_pt20 = 0.;
  float EyMissRF_GCW_pt20 = 0.;
  float ExMissRF_LCW_NI_eflow4 = 0.;
  float EyMissRF_LCW_NI_eflow4 = 0.;  
  float ExMissRF_LCW_NI_eflow_CB = 0.;
  float EyMissRF_LCW_NI_eflow_CB = 0.;  
  float ExMissRF_LCW_NI_eflow_400 = 0.;
  float EyMissRF_LCW_NI_eflow_400 = 0.;  
  float ExMissRF_LCW_NI = 0.;
  float EyMissRF_LCW_NI = 0.;  
  
  float ExMissRefFinal45 = 0.;
  float EyMissRefFinal45 = 0.;
  float ExMissRefFinal45_em = 0.;
  float EyMissRefFinal45_em = 0.;
  float ExMissRF_LCW_pt20_45 = 0.;
  float EyMissRF_LCW_pt20_45 = 0.;
  float ExMissRF_GCW_pt20_45 = 0.;
  float EyMissRF_GCW_pt20_45 = 0.;
  float ExMissRF_LCW_NI_eflow4_45 = 0.;
  float EyMissRF_LCW_NI_eflow4_45 = 0.;  
  float ExMissRF_LCW_NI_eflow_CB_45 = 0.;
  float EyMissRF_LCW_NI_eflow_CB_45 = 0.;  
  float ExMissRF_LCW_NI_eflow_400_45 = 0.;
  float EyMissRF_LCW_NI_eflow_400_45 = 0.;  
  float ExMissRF_LCW_NI_45 = 0.;
  float EyMissRF_LCW_NI_45 = 0.;  
 
  float ExMissRefFinalHT = 0.;
  float EyMissRefFinalHT = 0.;
  float EtMissRefFinalHT = 0.;
  float SumetRefFinalHT = 0.;
  float ExMissRefFinalHT_45 = 0.;
  float EyMissRefFinalHT_45 = 0.;
  float EtMissRefFinalHT_45 = 0.;
  float SumetRefFinalHT_45 = 0.;

  float ExMissRF_LCW_NI_eflow4HT = 0.;
  float EyMissRF_LCW_NI_eflow4HT = 0.;
  float EtMissRF_LCW_NI_eflow4HT = 0.;
  float SumetRF_LCW_NI_eflow4HT = 0.;
  float ExMissRF_LCW_NI_eflow4HT_45 = 0.;
  float EyMissRF_LCW_NI_eflow4HT_45 = 0.;
  float EtMissRF_LCW_NI_eflow4HT_45 = 0.;
  float SumetRF_LCW_NI_eflow4HT_45 = 0.;

  float ExMissRF_LCW_NI_eflow400HT = 0.;
  float EyMissRF_LCW_NI_eflow400HT = 0.;
  float EtMissRF_LCW_NI_eflow400HT = 0.;
  float SumetRF_LCW_NI_eflow400HT = 0.;
  float ExMissRF_LCW_NI_eflow400HT_45 = 0.;
  float EyMissRF_LCW_NI_eflow400HT_45 = 0.;
  float EtMissRF_LCW_NI_eflow400HT_45 = 0.;
  float SumetRF_LCW_NI_eflow400HT_45 = 0.;


  float verx = 0.;
  float very = 0.;
  float vert = 0.;
  float versumet = 0.;
 
  int ih = 0;

  //various SET
  float Set = 0.;
  float Set1 = 0.;
  float Set2 = 0.;
  float Set3 = 0.;
  float Set4 = 0.;
  float Set5 = 0.;
  float Set6 = 0.;
  float Set7 = 0.;
  float Set8 = 0.;
  float Set9 = 0.;
  float Set10 = 0.;
  
  float Set_45 = 0.;
  float Set1_45 = 0.;
  float Set2_45 = 0.;
  float Set3_45 = 0.;
  float Set4_45 = 0.;
  float Set5_45 = 0.;
  float Set6_45 = 0.;
  float Set7_45 = 0.;
  float Set8_45 = 0.;
  float Set9_45 = 0.;
  float Set10_45 = 0.;

  float SetHT = 0.;
  float SetHT7 = 0.;
  float SetHT9 = 0.;

  float SetHT_45 = 0.;
  float SetHT7_45 = 0.;
  float SetHT9_45 = 0.;


  //------------------------------------------------------------------------
  //Sven Menke plot
  float rap=0.;
  float rap1=0.;
  float rap2=0.;
  float rap3=0.;
  float rap4=0.;
  float rap5=0.;
  float rap6=0.;
  float rap7=0.;
  float rap8=0.;
  float rap9=0.;
  float rap10=0.;
    
  float rap_45=0.;
  float rap1_45=0.;
  float rap2_45=0.;
  float rap3_45=0.;
  float rap4_45=0.;
  float rap5_45=0.;
  float rap6_45=0.;
  float rap7_45=0.;
  float rap8_45=0.;
  float rap9_45=0.;
  float rap10_45=0.;

  float rapHT=0.;
  float rapHT7=0.;
  float rapHT9=0.;
  float rapHT_45=0.;
  float rapHT7_45=0.;
  float rapHT9_45=0.;


  //------------------------------------------------------------------------

  //Profile plots ratio calibs
  float EtMissEmScale = 0.;
  float EtMissGlobCalib = 0.;
  float EtMissFinalLocHad = 0.;
  float EtMissRefFinal = 0.;  
  float EtMissRefFinal_em = 0.;  
  float EtMissRF_LCW_pt20 = 0.;
  float EtMissRF_GCW_pt20 = 0.; 
  float EtMissRF_LCW_NI_eflow4= 0.;
  float EtMissRF_LCW_NI_eflow_CB= 0.;
  float EtMissRF_LCW_NI_eflow_400= 0.;
  float EtMissRF_LCW_NI= 0.;

  float ratio = 0.;
  float ratio1 = 0.;
  float ratio3 = 0.;
  float ratio4 = 0.; 
  float ratio5 = 0.;
  float ratio6 = 0.; 	        
  float ratio7 = 0.; 	        
  float ratio8 = 0.;
  float ratio9 = 0.;
  float ratio10 = 0.;
  
  float EtMissEmScale45 = 0.;
  float EtMissGlobCalib45 = 0.;
  float EtMissFinalLocHad45 = 0.;
  float EtMissRefFinal45 = 0.;
  float EtMissRefFinal45_em = 0.;
  float EtMissRF_LCW_pt20_45 = 0.;
  float EtMissRF_GCW_pt20_45 = 0.;
  float EtMissRF_LCW_NI_eflow4_45= 0.;
  float EtMissRF_LCW_NI_eflow_CB_45= 0.;
  float EtMissRF_LCW_NI_eflow_400_45= 0.;
  float EtMissRF_LCW_NI_45= 0.;
 
  float ratio_45 = 0.;
  float ratio1_45 = 0.;
  float ratio3_45 = 0.;
  float ratio4_45 = 0.; 
  float ratio5_45 = 0.;
  float ratio6_45 = 0.; 	        
  float ratio7_45 = 0.; 	        
  float ratio8_45 = 0.;
  float ratio9_45 = 0.;
  float ratio10_45 = 0.;
 

  float ratioHT = 0.;
  float ratioHTRef = 0.;
  float FCALf = 0.;


  float MET_Topo_3_sumet = 0.;
  float MET_Topo_3_etx = 0.;
  float MET_Topo_3_ety = 0.;
  float MET_Topo_3_et = 0.;
//  float Sumet_per_curvef = 0.;


  ExMissRefFinalHT=data->refFinal()->etx()-data->cellOut()->etx();
  EyMissRefFinalHT=data->refFinal()->ety()-data->cellOut()->ety();
  EtMissRefFinalHT= sqrt(pow(ExMissRefFinalHT,2)+pow(EyMissRefFinalHT,2));
  SumetRefFinalHT=data->refFinal()->sumet()-data->cellOut()->sumet();

  ExMissRF_LCW_NI_eflow4HT=data->refFinal_LCW_NI_eflow4()->etx()-data->cellOut_LCW_NI_eflow4()->etx();
  EyMissRF_LCW_NI_eflow4HT=data->refFinal_LCW_NI_eflow4()->ety()-data->cellOut_LCW_NI_eflow4()->ety();
  EtMissRF_LCW_NI_eflow4HT= sqrt(pow(ExMissRF_LCW_NI_eflow4HT,2)+pow(EyMissRF_LCW_NI_eflow4HT,2));
  SumetRF_LCW_NI_eflow4HT=data->refFinal_LCW_NI_eflow4()->sumet()-data->cellOut_LCW_NI_eflow4()->sumet();  

  ExMissRF_LCW_NI_eflow400HT=data->refFinal_LCW_NI_eflow_400()->etx()-data->cellOut_LCW_NI_eflow_400()->etx();
  EyMissRF_LCW_NI_eflow400HT=data->refFinal_LCW_NI_eflow_400()->ety()-data->cellOut_LCW_NI_eflow_400()->ety();
  EtMissRF_LCW_NI_eflow400HT= sqrt(pow(ExMissRF_LCW_NI_eflow400HT,2)+pow(EyMissRF_LCW_NI_eflow400HT,2));
  SumetRF_LCW_NI_eflow400HT=data->refFinal_LCW_NI_eflow_400()->sumet()-data->cellOut_LCW_NI_eflow_400()->sumet();    
  
  
  ExMissRefFinalHT_45=data->refFinal()->etx_eta45()-data->cellOut()->etx_eta45();
  EyMissRefFinalHT_45=data->refFinal()->ety_eta45()-data->cellOut()->ety_eta45();
  EtMissRefFinalHT_45= sqrt(pow(ExMissRefFinalHT_45,2)+pow(EyMissRefFinalHT_45,2));
  SumetRefFinalHT_45=data->refFinal()->sumet_eta45()-data->cellOut()->sumet_eta45();
  
  ExMissRF_LCW_NI_eflow4HT_45=data->refFinal_LCW_NI_eflow4()->etx_eta45()-data->cellOut_LCW_NI_eflow4()->etx_eta45();
  EyMissRF_LCW_NI_eflow4HT_45=data->refFinal_LCW_NI_eflow4()->ety_eta45()-data->cellOut_LCW_NI_eflow4()->ety_eta45();
  EtMissRF_LCW_NI_eflow4HT_45= sqrt(pow(ExMissRF_LCW_NI_eflow4HT_45,2)+pow(EyMissRF_LCW_NI_eflow4HT_45,2));
  SumetRF_LCW_NI_eflow4HT_45=data->refFinal_LCW_NI_eflow4()->sumet_eta45()-data->cellOut_LCW_NI_eflow4()->sumet_eta45();

  ExMissRF_LCW_NI_eflow400HT_45=data->refFinal_LCW_NI_eflow_400()->etx_eta45()-data->cellOut_LCW_NI_eflow_400()->etx_eta45();
  EyMissRF_LCW_NI_eflow400HT_45=data->refFinal_LCW_NI_eflow_400()->ety_eta45()-data->cellOut_LCW_NI_eflow_400()->ety_eta45();
  EtMissRF_LCW_NI_eflow400HT_45= sqrt(pow(ExMissRF_LCW_NI_eflow400HT_45,2)+pow(EyMissRF_LCW_NI_eflow400HT_45,2));
  SumetRF_LCW_NI_eflow400HT_45=data->refFinal_LCW_NI_eflow_400()->sumet_eta45()-data->cellOut_LCW_NI_eflow_400()->sumet_eta45();



  //-------------------------------------------------------------
  // definition for special case LCW_NI
  float refFinal_LCW_NI_SET   = data->refFinal()->sumet() - data->cellOut_Eflow()->sumet() + data->cellOut()->sumet();
  float refFinal_LCW_NI_SET45 = data->refFinal()->sumet_eta45() - data->cellOut_Eflow()->sumet_eta45() + data->cellOut()->sumet_eta45();
  
  //define various SET
  Set=data->refFinal()->sumet();
  Set1=data->locHadTopo()->sumet();
  Set2=data->topo()->sumet();
  Set3=data->corrTopo()->sumet();
  Set4=data->refFinal_LCW_pt20()->sumet();
  Set5=data->refFinal_GCW_pt20()->sumet();
  Set6=data->refFinal_em()->sumet();
  Set7=data->refFinal_LCW_NI_eflow4()->sumet();
  Set8=data->refFinal_LCW_NI_eflow_CB()->sumet();
  Set9=data->refFinal_LCW_NI_eflow_400()->sumet();
  Set10=refFinal_LCW_NI_SET ;
  
  Set_45=data->refFinal()->sumet_eta45();
  Set1_45=data->locHadTopo()->sumet_eta45();
  Set2_45=data->topo()->sumet_eta45();
  Set3_45=data->corrTopo()->sumet_eta45();
  Set4_45=data->refFinal_LCW_pt20()->sumet_eta45();
  Set5_45=data->refFinal_GCW_pt20()->sumet_eta45();
  Set6_45=data->refFinal_em()->sumet_eta45();
  Set7_45=data->refFinal_LCW_NI_eflow4()->sumet_eta45();
  Set8_45=data->refFinal_LCW_NI_eflow_CB()->sumet_eta45();
  Set9_45=data->refFinal_LCW_NI_eflow_400()->sumet_eta45();
  Set10_45=refFinal_LCW_NI_SET45; 
  

  SetHT=SumetRefFinalHT;
  SetHT7=SumetRF_LCW_NI_eflow4HT;
  SetHT9=SumetRF_LCW_NI_eflow400HT;

  SetHT_45=SumetRefFinalHT_45;
  SetHT7_45=SumetRF_LCW_NI_eflow4HT_45;
  SetHT9_45=SumetRF_LCW_NI_eflow400HT_45;
  
  //------------------------------------------------------------------------
  //Truth etmiss and SET
 
  verx=data->truth_nonInt()->etx();
  very=data->truth_nonInt()->ety();
  vert=sqrt(verx*verx+very*very);
  versumet=data->truth_int()->sumet();


  Sumet_per_curvetruth=versumet/1000.0;

  
  if(m_useTruth == 0) {
     verx=0.;
     very=0.; 
     vert=0.;
   
  }
 
    //different reco Sumet.......  used if (m_use_sumet == 2)
    Sumet_per_curve=Set/1000.0;
    Sumet_per_curve1=Set1/1000.0;   
    Sumet_per_curve2=Set2/1000.0;
    Sumet_per_curve3=Set3/1000.0;
    Sumet_per_curve4=Set4/1000.0;
    Sumet_per_curve5=Set5/1000.0;
    Sumet_per_curve6=Set6/1000.0;
    Sumet_per_curve7=Set7/1000.0;
    Sumet_per_curve8=Set8/1000.0;
    Sumet_per_curve9=Set9/1000.0;
    Sumet_per_curve10=Set10/1000.0;
    
    Sumet_per_curve_45=Set_45/1000.0;
    Sumet_per_curve1_45=Set1_45/1000.0;   
    Sumet_per_curve2_45=Set2_45/1000.0;
    Sumet_per_curve3_45=Set3_45/1000.0;
    Sumet_per_curve4_45=Set4_45/1000.0;
    Sumet_per_curve5_45=Set5_45/1000.0;
    Sumet_per_curve6_45=Set6_45/1000.0;
    Sumet_per_curve7_45=Set7_45/1000.0;
    Sumet_per_curve8_45=Set8_45/1000.0;
    Sumet_per_curve9_45=Set9_45/1000.0;
    Sumet_per_curve10_45=Set10_45/1000.0;
  

    Sumet_per_curveHT=SetHT/1000.0;
    Sumet_per_curveHT7=SetHT7/1000.0;
    Sumet_per_curveHT9=SetHT9/1000.0;
    
    Sumet_per_curveHT_45=SetHT_45/1000.0;
    Sumet_per_curveHT7_45=SetHT7_45/1000.0;
    Sumet_per_curveHT9_45=SetHT9_45/1000.0;

    
    //------------------------------------------------------------------------
    //Sven plot
    rap=Sumet_per_curve2/Sumet_per_curve;
    rap1=Sumet_per_curve2/Sumet_per_curve1;
    rap2=Sumet_per_curve2/Sumet_per_curve2;
    rap3=Sumet_per_curve2/Sumet_per_curve3;
    rap4=Sumet_per_curve2/Sumet_per_curve4;
    rap5=Sumet_per_curve2/Sumet_per_curve5;
    rap6=Sumet_per_curve2/Sumet_per_curve6;
    rap7=Sumet_per_curve2/Sumet_per_curve7;
    rap8=Sumet_per_curve2/Sumet_per_curve8;
    rap9=Sumet_per_curve2/Sumet_per_curve9;
    rap10=Sumet_per_curve2/Sumet_per_curve10;
    
    rap_45=Sumet_per_curve2_45/Sumet_per_curve_45;
    rap1_45=Sumet_per_curve2_45/Sumet_per_curve1_45;
    rap2_45=Sumet_per_curve2_45/Sumet_per_curve2_45;
    rap3_45=Sumet_per_curve2_45/Sumet_per_curve3_45;
    rap4_45=Sumet_per_curve2_45/Sumet_per_curve4_45;
    rap5_45=Sumet_per_curve2_45/Sumet_per_curve5_45;
    rap6_45=Sumet_per_curve2_45/Sumet_per_curve6_45;
    rap7_45=Sumet_per_curve2_45/Sumet_per_curve7_45;
    rap8_45=Sumet_per_curve2_45/Sumet_per_curve8_45;
    rap9_45=Sumet_per_curve2_45/Sumet_per_curve9_45;
    rap10_45=Sumet_per_curve2_45/Sumet_per_curve10_45;
    

 
    rapHT=Sumet_per_curve2/Sumet_per_curveHT;
    rapHT7=Sumet_per_curve2/Sumet_per_curveHT7;
    rapHT9=Sumet_per_curve2/Sumet_per_curveHT9;
    
    rapHT_45=Sumet_per_curve2_45/Sumet_per_curveHT_45;
    rapHT7_45=Sumet_per_curve2_45/Sumet_per_curveHT7_45;
    rapHT9_45=Sumet_per_curve2_45/Sumet_per_curveHT9_45;
    
 
  if(m_use_sumet == 1 && m_useTruth == 1) {
    rap=Sumet_per_curvetruth/Sumet_per_curve;
    rap1=Sumet_per_curvetruth/Sumet_per_curve1;
    rap2=Sumet_per_curvetruth/Sumet_per_curve2;
    rap3=Sumet_per_curvetruth/Sumet_per_curve3;
    rap4=Sumet_per_curvetruth/Sumet_per_curve4;
    rap5=Sumet_per_curvetruth/Sumet_per_curve5;
    rap6=Sumet_per_curvetruth/Sumet_per_curve6;
    rap7=Sumet_per_curvetruth/Sumet_per_curve7;
    rap8=Sumet_per_curvetruth/Sumet_per_curve8;
    rap9=Sumet_per_curvetruth/Sumet_per_curve9;
    rap10=Sumet_per_curvetruth/Sumet_per_curve10;
    
    rap_45=Sumet_per_curvetruth/Sumet_per_curve_45;
    rap1_45=Sumet_per_curvetruth/Sumet_per_curve1_45;
    rap2_45=Sumet_per_curvetruth/Sumet_per_curve2_45;
    rap3_45=Sumet_per_curvetruth/Sumet_per_curve3_45;
    rap4_45=Sumet_per_curvetruth/Sumet_per_curve4_45;
    rap5_45=Sumet_per_curvetruth/Sumet_per_curve5_45;
    rap6_45=Sumet_per_curvetruth/Sumet_per_curve6_45;
    rap7_45=Sumet_per_curvetruth/Sumet_per_curve7_45;
    rap8_45=Sumet_per_curvetruth/Sumet_per_curve8_45;
    rap9_45=Sumet_per_curvetruth/Sumet_per_curve9_45;
    rap10_45=Sumet_per_curvetruth/Sumet_per_curve10_45;


    rapHT=Sumet_per_curvetruth/Sumet_per_curveHT;
    rapHT7=Sumet_per_curvetruth/Sumet_per_curveHT7;
    rapHT9=Sumet_per_curvetruth/Sumet_per_curveHT9;
    
    rapHT_45=Sumet_per_curvetruth/Sumet_per_curveHT_45;
    rapHT7_45=Sumet_per_curvetruth/Sumet_per_curveHT7_45;
    rapHT9_45=Sumet_per_curvetruth/Sumet_per_curveHT9_45;
      
    Sumet_per_curve= Sumet_per_curvetruth;
    Sumet_per_curve1= Sumet_per_curvetruth;
    Sumet_per_curve2= Sumet_per_curvetruth;
    Sumet_per_curve3= Sumet_per_curvetruth;
    Sumet_per_curve4= Sumet_per_curvetruth;
    Sumet_per_curve5= Sumet_per_curvetruth;
    Sumet_per_curve6= Sumet_per_curvetruth;
    Sumet_per_curve7= Sumet_per_curvetruth;
    Sumet_per_curve8= Sumet_per_curvetruth;
    Sumet_per_curve9= Sumet_per_curvetruth;
    Sumet_per_curve10= Sumet_per_curvetruth;
    
    Sumet_per_curve_45= Sumet_per_curvetruth;
    Sumet_per_curve1_45= Sumet_per_curvetruth;
    Sumet_per_curve2_45= Sumet_per_curvetruth;
    Sumet_per_curve3_45= Sumet_per_curvetruth;
    Sumet_per_curve4_45= Sumet_per_curvetruth;
    Sumet_per_curve5_45= Sumet_per_curvetruth;
    Sumet_per_curve6_45= Sumet_per_curvetruth;
    Sumet_per_curve7_45= Sumet_per_curvetruth;
    Sumet_per_curve8_45= Sumet_per_curvetruth;
    Sumet_per_curve9_45= Sumet_per_curvetruth;
    Sumet_per_curve10_45= Sumet_per_curvetruth;

    Sumet_per_curveHT=Sumet_per_curvetruth;
    Sumet_per_curveHT_45=Sumet_per_curvetruth;
  }
    //------------------------------------------------------------------------
    //use Sumet EM scale.......
    if(m_use_sumet == 3) {
      Sumet_per_curve= Sumet_per_curve2;
      Sumet_per_curve1= Sumet_per_curve2;      
      Sumet_per_curve3= Sumet_per_curve2;
      Sumet_per_curve4= Sumet_per_curve2;
      Sumet_per_curve5= Sumet_per_curve2;
      Sumet_per_curve6= Sumet_per_curve2;
      Sumet_per_curve7= Sumet_per_curve2;
      Sumet_per_curve8= Sumet_per_curve2;
      Sumet_per_curve9= Sumet_per_curve2;
      Sumet_per_curve10= Sumet_per_curve2;
      
      Sumet_per_curve_45= Sumet_per_curve2_45;
      Sumet_per_curve1_45= Sumet_per_curve2_45;      
      Sumet_per_curve3_45= Sumet_per_curve2_45;
      Sumet_per_curve4_45= Sumet_per_curve2_45;
      Sumet_per_curve5_45= Sumet_per_curve2_45;
      Sumet_per_curve6_45= Sumet_per_curve2_45;
      Sumet_per_curve7_45= Sumet_per_curve2_45;
      Sumet_per_curve8_45= Sumet_per_curve2_45;
      Sumet_per_curve9_45= Sumet_per_curve2_45;
      Sumet_per_curve10_45= Sumet_per_curve2_45;

      Sumet_per_curveHT=Sumet_per_curve2;
      Sumet_per_curveHT7=Sumet_per_curve2;
      Sumet_per_curveHT9=Sumet_per_curve2;

      Sumet_per_curveHT_45=Sumet_per_curve2_45;
      Sumet_per_curveHT7_45=Sumet_per_curve2_45;
      Sumet_per_curveHT9_45=Sumet_per_curve2_45;

    }
    //use Sumet LH.......
    if(m_use_sumet == 4) {
      Sumet_per_curve= Sumet_per_curve1;
      //Sumet_per_curve1= Sumet_per_curve1;
      Sumet_per_curve2= Sumet_per_curve1;
      Sumet_per_curve3= Sumet_per_curve1;
      Sumet_per_curve4= Sumet_per_curve1;
      Sumet_per_curve5= Sumet_per_curve1;
      Sumet_per_curve6= Sumet_per_curve1;
      Sumet_per_curve7= Sumet_per_curve1;
      Sumet_per_curve8= Sumet_per_curve1;
      Sumet_per_curve9= Sumet_per_curve1;
      Sumet_per_curve10= Sumet_per_curve1;
    
    }
    //use Sumet HT.......
  if(m_use_sumet == 5) {
    rap=Sumet_per_curveHT/Sumet_per_curve;
    rap1=Sumet_per_curveHT/Sumet_per_curve1;
    rap2=Sumet_per_curveHT/Sumet_per_curve2;
    rap3=Sumet_per_curveHT/Sumet_per_curve3;
    rap4=Sumet_per_curveHT/Sumet_per_curve4;
    rap5=Sumet_per_curveHT/Sumet_per_curve5;
    rap6=Sumet_per_curveHT/Sumet_per_curve6;
    rap7=Sumet_per_curveHT/Sumet_per_curve7;
    rap8=Sumet_per_curveHT/Sumet_per_curve8;
    rap9=Sumet_per_curveHT/Sumet_per_curve9;
    rap10=Sumet_per_curveHT/Sumet_per_curve10;
    
    rap_45=Sumet_per_curveHT/Sumet_per_curve_45;
    rap1_45=Sumet_per_curveHT/Sumet_per_curve1_45;
    rap2_45=Sumet_per_curveHT/Sumet_per_curve2_45;
    rap3_45=Sumet_per_curveHT/Sumet_per_curve3_45;
    rap4_45=Sumet_per_curveHT/Sumet_per_curve4_45;
    rap5_45=Sumet_per_curveHT/Sumet_per_curve5_45;
    rap6_45=Sumet_per_curveHT/Sumet_per_curve6_45;
    rap7_45=Sumet_per_curveHT/Sumet_per_curve7_45;
    rap8_45=Sumet_per_curveHT/Sumet_per_curve8_45;
    rap9_45=Sumet_per_curveHT/Sumet_per_curve9_45;
    rap10_45=Sumet_per_curveHT/Sumet_per_curve10_45;

    rapHT=Sumet_per_curveHT/Sumet_per_curveHT;
    rapHT7=Sumet_per_curveHT/Sumet_per_curveHT7;
    rapHT9=Sumet_per_curveHT/Sumet_per_curveHT9;

    rapHT_45=Sumet_per_curveHT/Sumet_per_curveHT_45;
    rapHT7_45=Sumet_per_curveHT/Sumet_per_curveHT7_45;
    rapHT9_45=Sumet_per_curveHT/Sumet_per_curveHT9_45;

    
    Sumet_per_curve= Sumet_per_curveHT;
    Sumet_per_curve1= Sumet_per_curveHT;
    Sumet_per_curve2= Sumet_per_curveHT;
    Sumet_per_curve3= Sumet_per_curveHT;
    Sumet_per_curve4= Sumet_per_curveHT;
    Sumet_per_curve5= Sumet_per_curveHT;
    Sumet_per_curve6= Sumet_per_curveHT;
    Sumet_per_curve7= Sumet_per_curveHT;
    Sumet_per_curve8= Sumet_per_curveHT;
    Sumet_per_curve9= Sumet_per_curveHT;
    Sumet_per_curve10= Sumet_per_curveHT;
    
    Sumet_per_curve_45= Sumet_per_curveHT_45;
    Sumet_per_curve1_45= Sumet_per_curveHT_45;
    Sumet_per_curve2_45= Sumet_per_curveHT_45;
    Sumet_per_curve3_45= Sumet_per_curveHT_45;
    Sumet_per_curve4_45= Sumet_per_curveHT_45;
    Sumet_per_curve5_45= Sumet_per_curveHT_45;
    Sumet_per_curve6_45= Sumet_per_curveHT_45;
    Sumet_per_curve7_45= Sumet_per_curveHT_45;
    Sumet_per_curve8_45= Sumet_per_curveHT_45;
    Sumet_per_curve9_45= Sumet_per_curveHT_45;
    Sumet_per_curve10_45= Sumet_per_curveHT_45;


    Sumet_per_curveHT=Sumet_per_curveHT;
    Sumet_per_curveHT7=Sumet_per_curveHT;
    Sumet_per_curveHT9=Sumet_per_curveHT;

    Sumet_per_curveHT_45=Sumet_per_curveHT_45;
    Sumet_per_curveHT7_45=Sumet_per_curveHT_45;
    Sumet_per_curveHT9_45=Sumet_per_curveHT_45;

  }
//finqui ok    //use Sumet CorrTopo.......
   if(m_use_sumet == 6) {
      Sumet_per_curve= Sumet_per_curve3;
      Sumet_per_curve1= Sumet_per_curve3;
      Sumet_per_curve2= Sumet_per_curve3;
      Sumet_per_curve3= Sumet_per_curve3;
      Sumet_per_curve4= Sumet_per_curve3;
      Sumet_per_curve5= Sumet_per_curve3;
      Sumet_per_curve6= Sumet_per_curve3;
      Sumet_per_curve7= Sumet_per_curve3;
      Sumet_per_curve8= Sumet_per_curve3;
      Sumet_per_curve9= Sumet_per_curve3;
      Sumet_per_curve10= Sumet_per_curve3;
      
    }
 
 
  h_sumetT->Fill(Sumet_per_curvetruth);
  h_sumet->Fill(Sumet_per_curve);
  h1_sumet->Fill(Sumet_per_curve1);
  h2_sumet->Fill(Sumet_per_curve2);
  h3_sumet->Fill(Sumet_per_curve3);
  h4_sumet->Fill(Sumet_per_curve4);
  h5_sumet->Fill(Sumet_per_curve5);
  h6_sumet->Fill(Sumet_per_curve6);
  h7_sumet->Fill(Sumet_per_curve7);
  h8_sumet->Fill(Sumet_per_curve8);
  h9_sumet->Fill(Sumet_per_curve9);
  h10_sumet->Fill(Sumet_per_curve10);
  
  h_45_sumet->Fill(Sumet_per_curve_45);
  h1_45_sumet->Fill(Sumet_per_curve1_45);
  h2_45_sumet->Fill(Sumet_per_curve2_45);
  h3_45_sumet->Fill(Sumet_per_curve3_45);
  h4_45_sumet->Fill(Sumet_per_curve4_45);
  h5_45_sumet->Fill(Sumet_per_curve5_45);
  h6_45_sumet->Fill(Sumet_per_curve6_45);
  h7_45_sumet->Fill(Sumet_per_curve7_45);
  h8_45_sumet->Fill(Sumet_per_curve8_45);
  h9_45_sumet->Fill(Sumet_per_curve9_45);
  h10_45_sumet->Fill(Sumet_per_curve10_45);
  

  h_sumetHT->Fill(Sumet_per_curveHT);
  h_sumetHT_45->Fill(Sumet_per_curveHT_45);
  h_sumetHT7->Fill(Sumet_per_curveHT7);
  h_sumetHT7_45->Fill(Sumet_per_curveHT7_45);
  h_sumetHT9->Fill(Sumet_per_curveHT9);
  h_sumetHT9_45->Fill(Sumet_per_curveHT9_45);


//cate
 
  // definition for special case LCW_NI
  float refFinal_LCW_NI_METx = data->refFinal()->etx() - data->cellOut_Eflow()->etx() + data->cellOut()->etx();
  float refFinal_LCW_NI_METy = data->refFinal()->ety() - data->cellOut_Eflow()->ety() + data->cellOut()->ety();
  float refFinal_LCW_NI_METx45 = data->refFinal()->etx_eta45() - data->cellOut_Eflow()->etx_eta45() + data->cellOut()->etx_eta45();
  float refFinal_LCW_NI_METy45 = data->refFinal()->ety_eta45() - data->cellOut_Eflow()->ety_eta45() + data->cellOut()->ety_eta45();

  ExMissEmScale= data->topo()->etx()+data->MuonBoy()->etx();
  EyMissEmScale= data->topo()->ety()+data->MuonBoy()->ety();
  ExMissGlobCalib= data->corrTopo()->etx()+data->MuonBoy()->etx();
  EyMissGlobCalib= data->corrTopo()->ety()+data->MuonBoy()->ety();
  ExMissFinalLocHad= data->locHadTopo()->etx()+data->MuonBoy()->etx(); 
  EyMissFinalLocHad= data->locHadTopo()->ety()+data->MuonBoy()->ety();
  ExMissRefFinal=data->refFinal()->etx();
  EyMissRefFinal=data->refFinal()->ety();
  ExMissRefFinal_em=data->refFinal_em()->etx();
  EyMissRefFinal_em=data->refFinal_em()->ety();
  ExMissRF_LCW_pt20=data->refFinal_LCW_pt20()->etx();
  EyMissRF_LCW_pt20=data->refFinal_LCW_pt20()->ety();
  ExMissRF_GCW_pt20=data->refFinal_GCW_pt20()->etx();
  EyMissRF_GCW_pt20=data->refFinal_GCW_pt20()->ety();
  ExMissRF_LCW_NI_eflow4=data->refFinal_LCW_NI_eflow4()->etx();
  EyMissRF_LCW_NI_eflow4=data->refFinal_LCW_NI_eflow4()->ety();
  ExMissRF_LCW_NI_eflow_CB=data->refFinal_LCW_NI_eflow_CB()->etx();
  EyMissRF_LCW_NI_eflow_CB=data->refFinal_LCW_NI_eflow_CB()->ety();
  ExMissRF_LCW_NI_eflow_400=data->refFinal_LCW_NI_eflow_400()->etx();
  EyMissRF_LCW_NI_eflow_400=data->refFinal_LCW_NI_eflow_400()->ety();
  ExMissRF_LCW_NI=refFinal_LCW_NI_METx;
  EyMissRF_LCW_NI=refFinal_LCW_NI_METy;

  ExMissEmScale45= data->topo()->etx_eta45()+data->MuonBoy()->etx();
  EyMissEmScale45= data->topo()->ety_eta45()+data->MuonBoy()->ety();
  ExMissGlobCalib45= data->corrTopo()->etx_eta45()+data->MuonBoy()->etx();
  EyMissGlobCalib45= data->corrTopo()->ety_eta45()+data->MuonBoy()->ety();
  ExMissFinalLocHad45= data->locHadTopo()->etx_eta45()+data->MuonBoy()->etx(); 
  EyMissFinalLocHad45= data->locHadTopo()->ety_eta45()+data->MuonBoy()->ety();
  ExMissRefFinal45=data->refFinal()->etx_eta45();
  EyMissRefFinal45=data->refFinal()->ety_eta45();
  ExMissRefFinal45_em=data->refFinal_em()->etx_eta45();
  EyMissRefFinal45_em=data->refFinal_em()->ety_eta45();
  ExMissRF_LCW_pt20_45=data->refFinal_LCW_pt20()->etx_eta45();
  EyMissRF_LCW_pt20_45=data->refFinal_LCW_pt20()->ety_eta45();
  ExMissRF_GCW_pt20_45=data->refFinal_GCW_pt20()->etx_eta45();
  EyMissRF_GCW_pt20_45=data->refFinal_GCW_pt20()->ety_eta45();
  ExMissRF_LCW_NI_eflow4_45=data->refFinal_LCW_NI_eflow4()->etx_eta45();
  EyMissRF_LCW_NI_eflow4_45=data->refFinal_LCW_NI_eflow4()->ety_eta45();
  ExMissRF_LCW_NI_eflow_CB_45=data->refFinal_LCW_NI_eflow_CB()->etx_eta45();
  EyMissRF_LCW_NI_eflow_CB_45=data->refFinal_LCW_NI_eflow_CB()->ety_eta45();
  ExMissRF_LCW_NI_eflow_400_45=data->refFinal_LCW_NI_eflow_400()->etx_eta45();
  EyMissRF_LCW_NI_eflow_400_45=data->refFinal_LCW_NI_eflow_400()->ety_eta45(); 
  ExMissRF_LCW_NI_45=refFinal_LCW_NI_METx45;
  EyMissRF_LCW_NI_45=refFinal_LCW_NI_METy45; 

  
//cate

  h_tot->Fill((verx - data->refFinal()->etx())/1000.0);
  h_tot->Fill((very - data->refFinal()->ety())/1000.0);  
  h1_tot->Fill((verx - ExMissFinalLocHad)/1000.0);
  h1_tot->Fill((very - EyMissFinalLocHad)/1000.0);  
  h2_tot->Fill((verx - ExMissEmScale)/1000.0);
  h2_tot->Fill((very - EyMissEmScale)/1000.0);
  h3_tot->Fill((verx - ExMissGlobCalib)/1000.0);
  h3_tot->Fill((very - EyMissGlobCalib)/1000.0);  
  h4_tot->Fill((verx - ExMissRF_LCW_pt20)/1000.0);
  h4_tot->Fill((very - EyMissRF_LCW_pt20)/1000.0);
  h5_tot->Fill((verx - ExMissRF_GCW_pt20)/1000.0);
  h5_tot->Fill((very - EyMissRF_GCW_pt20)/1000.0);
  h6_tot->Fill((verx - ExMissRefFinal_em)/1000.0);
  h6_tot->Fill((very - EyMissRefFinal_em)/1000.0);
  h7_tot->Fill((verx - ExMissRF_LCW_NI_eflow4)/1000.0);
  h7_tot->Fill((very - EyMissRF_LCW_NI_eflow4 )/1000.0);
  h8_tot->Fill((verx - ExMissRF_LCW_NI_eflow_CB)/1000.0);
  h8_tot->Fill((very - EyMissRF_LCW_NI_eflow_CB)/1000.0);
  h9_tot->Fill((verx - ExMissRF_LCW_NI_eflow_400)/1000.0);
  h9_tot->Fill((very - EyMissRF_LCW_NI_eflow_400)/1000.0);
  h10_tot->Fill((verx - ExMissRF_LCW_NI)/1000.0);
  h10_tot->Fill((very - EyMissRF_LCW_NI)/1000.0);
 
  h_45_tot->Fill((verx - data->refFinal()->etx_eta45())/1000.0);
  h_45_tot->Fill((very - data->refFinal()->ety_eta45())/1000.0);  
  h1_45_tot->Fill((verx - ExMissFinalLocHad45)/1000.0);
  h1_45_tot->Fill((very - EyMissFinalLocHad45)/1000.0);  
  h2_45_tot->Fill((verx - ExMissEmScale45)/1000.0);
  h2_45_tot->Fill((very - EyMissEmScale45)/1000.0);
  h3_45_tot->Fill((verx - ExMissGlobCalib45)/1000.0);
  h3_45_tot->Fill((very - EyMissGlobCalib45)/1000.0);  
  h4_45_tot->Fill((verx - ExMissRF_LCW_pt20_45)/1000.0);
  h4_45_tot->Fill((very - EyMissRF_LCW_pt20_45)/1000.0);
  h5_45_tot->Fill((verx - ExMissRF_GCW_pt20_45)/1000.0);
  h5_45_tot->Fill((very - EyMissRF_GCW_pt20_45)/1000.0);
  h6_45_tot->Fill((verx - ExMissRefFinal45_em)/1000.0);
  h6_45_tot->Fill((very - EyMissRefFinal45_em)/1000.0);
  h7_45_tot->Fill((verx - ExMissRF_LCW_NI_eflow4_45)/1000.0);
  h7_45_tot->Fill((very - EyMissRF_LCW_NI_eflow4_45)/1000.0);
  h8_45_tot->Fill((verx - ExMissRF_LCW_NI_eflow_CB_45)/1000.0);
  h8_45_tot->Fill((very - EyMissRF_LCW_NI_eflow_CB_45)/1000.0);
  h9_45_tot->Fill((verx - ExMissRF_LCW_NI_eflow_400_45)/1000.0);
  h9_45_tot->Fill((very - EyMissRF_LCW_NI_eflow_400_45)/1000.0);
  h10_45_tot->Fill((verx - ExMissRF_LCW_NI_45)/1000.0);
  h10_45_tot->Fill((very - EyMissRF_LCW_NI_45)/1000.0);

  
  hHT_tot->Fill((verx - ExMissRefFinalHT)/1000.0);
  hHT_tot->Fill((very - EyMissRefFinalHT)/1000.0);
  hHTt_tot->Fill(EtMissRefFinalHT/1000.0);
  hHT_tot7->Fill((verx - ExMissRF_LCW_NI_eflow4HT)/1000.0);
  hHT_tot7->Fill((very - EyMissRF_LCW_NI_eflow4HT)/1000.0);
  hHTt_tot7->Fill(EtMissRF_LCW_NI_eflow4HT/1000.0);
  hHT_tot9->Fill((verx - ExMissRF_LCW_NI_eflow400HT)/1000.0);
  hHT_tot9->Fill((very - EyMissRF_LCW_NI_eflow400HT)/1000.0);
  hHTt_tot9->Fill(EtMissRF_LCW_NI_eflow400HT/1000.0);
  
  hHT_45_tot->Fill((verx - ExMissRefFinalHT_45)/1000.0);
  hHT_45_tot->Fill((very - EyMissRefFinalHT_45)/1000.0);
  hHTt_45_tot->Fill(EtMissRefFinalHT_45/1000.0);
  hHT_45_tot7->Fill((verx - ExMissRF_LCW_NI_eflow4HT_45)/1000.0);
  hHT_45_tot7->Fill((very - EyMissRF_LCW_NI_eflow4HT_45)/1000.0);
  hHTt_45_tot7->Fill(EtMissRF_LCW_NI_eflow4HT_45/1000.0);
  hHT_45_tot9->Fill((verx - ExMissRF_LCW_NI_eflow400HT_45)/1000.0);
  hHT_45_tot9->Fill((very - EyMissRF_LCW_NI_eflow400HT_45)/1000.0);
  hHTt_45_tot9->Fill(EtMissRF_LCW_NI_eflow400HT_45/1000.0);  
  
//cate last

  // definition for special case LCW_NI
  float refFinal_LCW_NI_MET   = sqrt(pow(refFinal_LCW_NI_METx ,2)+pow(refFinal_LCW_NI_METy,2));
  float refFinal_LCW_NI_MET45 = sqrt(pow(refFinal_LCW_NI_METx45 ,2)+pow(refFinal_LCW_NI_METy45,2));

  h_RefFinalHTResolution_x->Fill((verx- ExMissRefFinalHT)/1000.0);
  h_RefFinalHTResolution_y->Fill((very- EyMissRefFinalHT)/1000.0);
  h_RefFinalHTResolution_EtMiss->Fill((vert- EtMissRefFinalHT)/1000.0);

  //Profile plots ratio calibs
  EtMissEmScale= sqrt(ExMissEmScale*ExMissEmScale+
  		       EyMissEmScale*EyMissEmScale);
  EtMissGlobCalib= sqrt(ExMissGlobCalib*ExMissGlobCalib+
  			 EyMissGlobCalib*EyMissGlobCalib);
  EtMissFinalLocHad= sqrt(ExMissFinalLocHad*ExMissFinalLocHad+
  			   EyMissFinalLocHad*EyMissFinalLocHad);
 
  EtMissRefFinal=data->refFinal()->et();
  EtMissRF_LCW_pt20=data->refFinal_LCW_pt20()->et();
  EtMissRF_GCW_pt20=data->refFinal_GCW_pt20()->et();
  EtMissRF_LCW_NI_eflow4=data->refFinal_LCW_NI_eflow4()->et();
  EtMissRF_LCW_NI_eflow_CB=data->refFinal_LCW_NI_eflow_CB()->et();
  EtMissRF_LCW_NI_eflow_400=data->refFinal_LCW_NI_eflow_400()->et();
  EtMissRF_LCW_NI = refFinal_LCW_NI_MET; 
   
  EtMissRefFinal45=data->refFinal()->et_eta45();
  EtMissRF_LCW_pt20_45=data->refFinal_LCW_pt20()->et_eta45();
  EtMissRF_GCW_pt20_45=data->refFinal_GCW_pt20()->et_eta45();
  EtMissRF_LCW_NI_eflow4_45=data->refFinal_LCW_NI_eflow4()->et_eta45();
  EtMissRF_LCW_NI_eflow_CB_45=data->refFinal_LCW_NI_eflow_CB()->et_eta45();
  EtMissRF_LCW_NI_eflow_400_45=data->refFinal_LCW_NI_eflow_400()->et_eta45();
  EtMissRF_LCW_NI_45= refFinal_LCW_NI_MET45;
		       
  ratio= EtMissRefFinal/EtMissEmScale;
  ratio1= EtMissFinalLocHad/EtMissEmScale;
  ratio3= EtMissGlobCalib/EtMissEmScale;
  ratio4= EtMissRF_LCW_pt20/EtMissEmScale;
  ratio5= EtMissRF_GCW_pt20/EtMissEmScale;
  ratio6= EtMissRefFinal_em/EtMissEmScale;
  ratio7= EtMissRF_LCW_NI_eflow4/EtMissEmScale;
  ratio8= EtMissRF_LCW_NI_eflow_CB/EtMissEmScale;
  ratio9= EtMissRF_LCW_NI_eflow_400/EtMissEmScale;
  ratio10= EtMissRF_LCW_NI/EtMissEmScale;

  EtMissEmScale45= sqrt(ExMissEmScale45*ExMissEmScale45+
  		       EyMissEmScale45*EyMissEmScale45);
  EtMissGlobCalib45= sqrt(ExMissGlobCalib45*ExMissGlobCalib45+
  			 EyMissGlobCalib45*EyMissGlobCalib45);
  
  EtMissFinalLocHad45= sqrt(ExMissFinalLocHad45*ExMissFinalLocHad45+
  			   EyMissFinalLocHad45*EyMissFinalLocHad45);
  
  
  ratio_45=EtMissRefFinal45/EtMissEmScale45;
  ratio1_45=EtMissFinalLocHad45/EtMissEmScale45;
  ratio3_45=EtMissGlobCalib45/EtMissEmScale45;
  ratio4_45= EtMissRF_LCW_pt20_45/EtMissEmScale45;
  ratio5_45=EtMissRF_GCW_pt20_45/EtMissEmScale45;
  ratio6_45= EtMissRefFinal45_em/EtMissEmScale45;
  ratio7_45= EtMissRF_LCW_NI_eflow4_45/EtMissEmScale45;
  ratio8_45= EtMissRF_LCW_NI_eflow_CB_45/EtMissEmScale45;
  ratio9_45= EtMissRF_LCW_NI_eflow_400_45/EtMissEmScale45;
  ratio10_45= EtMissRF_LCW_NI_45/EtMissEmScale45;

  h_METGCWdivMETem ->Fill(Sumet_per_curve2,ratio3);
  h_METLCdivMETem ->Fill(Sumet_per_curve2,ratio1);
  h_METRefdivMETem ->Fill(Sumet_per_curve2,ratio);

  h_METem ->Fill(Sumet_per_curve2, EtMissEmScale/1000.);

  //Recalculate MET_Topo without FCAL
  MET_Topo_3_sumet =  data->topo()->sumet() - data->topo()->sumet_FCAL();
  MET_Topo_3_etx =  ExMissEmScale - data->topo()->etx_FCAL();
  MET_Topo_3_ety =  EyMissEmScale - data->topo()->ety_FCAL();
  MET_Topo_3_et = sqrt(MET_Topo_3_etx*MET_Topo_3_etx+MET_Topo_3_ety*MET_Topo_3_ety);

  //fraction of SumET in FCAL
  FCALf = data->topo()->sumet_FCAL()/data->topo()->sumet();

  // ratioHT vs SET TProfile
  //  RefFinal 
  ratioHTRef=Sumet_per_curveHT/Sumet_per_curve;

  ratioHT=sumHT_emscale/Set2;

  h_SETHTovSET ->Fill(Sumet_per_curve2,ratioHT);

  h_FCALf  ->Fill(FCALf);
  h_ratioHT  ->Fill(ratioHT);
  h_ratioHTRef  ->Fill(ratioHTRef);

  h_FCALfvSET ->Fill(Sumet_per_curve2,FCALf);


  //donatella Resolution curves
  //curvaresol---------------------------------
  //fill histos for curvaresol vs SumET 

  //and for Sven plot
  //ResMET*SumET_at_em-scale/SumET_at_calibrated-scale vs SumET_at_em_scale
  //RefFinal
  ih =(int)((Sumet_per_curve-m_startpt)/m_binpt);
  if (Sumet_per_curve < m_startpt) {ih = -1;}
  if ( ih >=0 && ih <  m_numptt) {
    h_.at(ih)->Fill((verx - data->refFinal()->etx())/1000.0);
    h_.at(ih)->Fill((very - data->refFinal()->ety())/1000.0);
    r_.at(ih)->Fill(rap1);
    p_.at(ih)->Fill(ratio1);
    su_.at(ih)->Fill(Sumet_per_curve);
    nc_.at(ih)->Fill(Ncell_tot);
    hbias_.at(ih)->Fill((vert - EtMissRefFinal)/1000.0);
    
    //PETER plots
    if (data->refFinal()->sumet()>0.){
    	hco_.at(ih)->Fill(data->cellOut_Eflow()->sumet()/data->refFinal()->sumet());
    	hsj_.at(ih)->Fill(data->softJets()->sumet()/data->refFinal()->sumet()); 
    	hrj_.at(ih)->Fill(data->refJet()->sumet()/data->refFinal()->sumet()); 
    }
    
  }
  
  //LCW
  ih =(int)((Sumet_per_curve1-m_startpt)/m_binpt);
  if (Sumet_per_curve1 < m_startpt) {ih = -1;}
  if ( ih >=0 && ih <  m_numptt) {
    h1_.at(ih)->Fill((verx - ExMissFinalLocHad)/1000.0);
    h1_.at(ih)->Fill((very - EyMissFinalLocHad)/1000.0);
    r1_.at(ih)->Fill(rap1);
    p1_.at(ih)->Fill(ratio1);
    su1_.at(ih)->Fill(Sumet_per_curve1);
    nc1_.at(ih)->Fill(Ncell_tot);
    hbias1_.at(ih)->Fill((vert - EtMissFinalLocHad)/1000.0);
  }  

   //EM SCALE
  ih =(int)((Sumet_per_curve2-m_startpt)/m_binpt);
  if (Sumet_per_curve2 < m_startpt) {ih = -1;}
  if ( ih >=0 && ih <  m_numptt) {
    h2_.at(ih)->Fill((verx - ExMissEmScale)/1000.0);
    h2_.at(ih)->Fill((very - EyMissEmScale)/1000.0);
    r2_.at(ih)->Fill(rap2);
    su2_.at(ih)->Fill(Sumet_per_curve2);
    nc2_.at(ih)->Fill(Ncell_tot);
    hbias2_.at(ih)->Fill((vert - EtMissEmScale)/1000.0);
  }

  //GCW
  ih =(int)((Sumet_per_curve3-m_startpt)/m_binpt);
  if (Sumet_per_curve3 < m_startpt) {ih = -1;}
  if ( ih >=0 && ih <  m_numptt) {
    h3_.at(ih)->Fill((verx - ExMissGlobCalib)/1000.0);
    h3_.at(ih)->Fill((very - EyMissGlobCalib)/1000.0);
    r3_.at(ih)->Fill(rap3);
    p3_.at(ih)->Fill(ratio3);
    su3_.at(ih)->Fill(Sumet_per_curve3);
    nc3_.at(ih)->Fill(Ncell_tot);
    hbias3_.at(ih)->Fill((vert - EtMissGlobCalib)/1000.0);
  }
 //RF LCW_pt20
  ih =(int)((Sumet_per_curve4-m_startpt)/m_binpt);
  if (Sumet_per_curve4 < m_startpt) {ih = -1;}
  if ( ih >=0 && ih <  m_numptt) {
    h4_.at(ih)->Fill((verx - ExMissRF_LCW_pt20)/1000.0);
    h4_.at(ih)->Fill((very - EyMissRF_LCW_pt20)/1000.0);
    r4_.at(ih)->Fill(rap4);
    p4_.at(ih)->Fill(ratio4);
    su4_.at(ih)->Fill(Sumet_per_curve4);
    nc4_.at(ih)->Fill(Ncell_tot);
    hbias4_.at(ih)->Fill((vert - EtMissRF_LCW_pt20)/1000.0);
  }  
  //RF GCW_pt20
  ih =(int)((Sumet_per_curve5-m_startpt)/m_binpt);
  if (Sumet_per_curve5 < m_startpt) {ih = -1;}
  if ( ih >=0 && ih <  m_numptt) {
    h5_.at(ih)->Fill((verx - ExMissRF_GCW_pt20)/1000.0);
    h5_.at(ih)->Fill((very - EyMissRF_GCW_pt20)/1000.0);
    r5_.at(ih)->Fill(rap5);
    p5_.at(ih)->Fill(ratio5);
    su5_.at(ih)->Fill(Sumet_per_curve5);
    nc5_.at(ih)->Fill(Ncell_tot);
    hbias5_.at(ih)->Fill((vert - ExMissRF_GCW_pt20 )/1000.0);
  }
  
  //RefFinal_em
  ih =(int)((Sumet_per_curve6-m_startpt)/m_binpt);
  if (Sumet_per_curve6 < m_startpt) {ih = -1;}
  if ( ih >=0 && ih <  m_numptt) {
    h6_.at(ih)->Fill((verx - ExMissRefFinal_em)/1000.0);
    h6_.at(ih)->Fill((very - EyMissRefFinal_em)/1000.0);
    r6_.at(ih)->Fill(rap6);
    p6_.at(ih)->Fill(ratio6);
    su6_.at(ih)->Fill(Sumet_per_curve6);
    nc6_.at(ih)->Fill(Ncell_tot);
    hbias6_.at(ih)->Fill((vert -EtMissRefFinal_em )/1000.0);
  }  
  
  
  //RefFinal LCW_NI_eflow4
  ih =(int)((Sumet_per_curve7-m_startpt)/m_binpt);
  if (Sumet_per_curve7 < m_startpt) {ih = -1;}
  if ( ih >=0 && ih <  m_numptt) {
    h7_.at(ih)->Fill((verx - ExMissRF_LCW_NI_eflow4 )/1000.0);
    h7_.at(ih)->Fill((very - EyMissRF_LCW_NI_eflow4)/1000.0);
    r7_.at(ih)->Fill(rap7);
    p7_.at(ih)->Fill(ratio7);
    su7_.at(ih)->Fill(Sumet_per_curve7);
    nc7_.at(ih)->Fill(Ncell_tot);
    hbias7_.at(ih)->Fill((vert -EtMissRF_LCW_NI_eflow4 )/1000.0);
  }  
  
  
 //RefFinal LCW_NI_eflow_CB
  ih =(int)((Sumet_per_curve8-m_startpt)/m_binpt);
  if (Sumet_per_curve8 < m_startpt) {ih = -1;}
  if ( ih >=0 && ih <  m_numptt) {
    h8_.at(ih)->Fill((verx - ExMissRF_LCW_NI_eflow_CB )/1000.0);
    h8_.at(ih)->Fill((very - EyMissRF_LCW_NI_eflow_CB)/1000.0);
    r8_.at(ih)->Fill(rap8);
    p8_.at(ih)->Fill(ratio8);
    su8_.at(ih)->Fill(Sumet_per_curve8);
    nc8_.at(ih)->Fill(Ncell_tot);
    hbias8_.at(ih)->Fill((vert -EtMissRF_LCW_NI_eflow_CB )/1000.0);
  }  

  
 //RefFinal LCW_NI_eflow_400
  ih =(int)((Sumet_per_curve9-m_startpt)/m_binpt);
  if (Sumet_per_curve9 < m_startpt) {ih = -1;}
  if ( ih >=0 && ih <  m_numptt) {
    h9_.at(ih)->Fill((verx - ExMissRF_LCW_NI_eflow_400 )/1000.0);
    h9_.at(ih)->Fill((very - EyMissRF_LCW_NI_eflow_400)/1000.0);
    r9_.at(ih)->Fill(rap9);
    p9_.at(ih)->Fill(ratio9);
    su9_.at(ih)->Fill(Sumet_per_curve9);
    nc9_.at(ih)->Fill(Ncell_tot);
    hbias9_.at(ih)->Fill((vert -EtMissRF_LCW_NI_eflow_400 )/1000.0);
  }  
 
 
 //RefFinal LCW_NI
  ih =(int)((Sumet_per_curve10-m_startpt)/m_binpt);
  if (Sumet_per_curve10 < m_startpt) {ih = -1;}
  if ( ih >=0 && ih <  m_numptt) {
    h10_.at(ih)->Fill((verx - ExMissRF_LCW_NI)/1000.0);
    h10_.at(ih)->Fill((very - EyMissRF_LCW_NI)/1000.0);
    r10_.at(ih)->Fill(rap10);
    p10_.at(ih)->Fill(ratio10);
    su10_.at(ih)->Fill(Sumet_per_curve10);
    nc10_.at(ih)->Fill(Ncell_tot);
    hbias10_.at(ih)->Fill((vert -EtMissRF_LCW_NI )/1000.0);
  }  


 //RefFinalHT
  ih =(int)((Sumet_per_curveHT-m_startpt)/m_binpt);
  if (Sumet_per_curveHT < m_startpt) {ih = -1;}
  if ( ih >=0 && ih <  m_numptt) {
    hHT_.at(ih)->Fill((verx - ExMissRefFinalHT)/1000.0);
    hHT_.at(ih)->Fill((very - EyMissRefFinalHT)/1000.0);
    rHT_.at(ih)->Fill(rapHT);
  }  

 //RF_LCW_NI_eflow4HT
  ih =(int)((Sumet_per_curveHT7-m_startpt)/m_binpt);
  if (Sumet_per_curveHT7 < m_startpt) {ih = -1;}
  if ( ih >=0 && ih <  m_numptt) {
    hHT7_.at(ih)->Fill((verx - ExMissRF_LCW_NI_eflow4HT)/1000.0);
    hHT7_.at(ih)->Fill((very - EyMissRF_LCW_NI_eflow4HT)/1000.0);
    rHT7_.at(ih)->Fill(rapHT7);
  }  

 //RF_LCW_NI_eflow400HT
  ih =(int)((Sumet_per_curveHT9-m_startpt)/m_binpt);
  if (Sumet_per_curveHT9 < m_startpt) {ih = -1;}
  if ( ih >=0 && ih <  m_numptt) {
    hHT9_.at(ih)->Fill((verx - ExMissRF_LCW_NI_eflow400HT)/1000.0);
    hHT9_.at(ih)->Fill((very - EyMissRF_LCW_NI_eflow400HT)/1000.0);
    rHT9_.at(ih)->Fill(rapHT9);
  }  



//eta 4.5!!!!!!!!!!!!!!!!!!!!!!!!  

  //RefFinal
  ih =(int)((Sumet_per_curve_45-m_startpt)/m_binpt);
  if (Sumet_per_curve1_45 < m_startpt) {ih = -1;}
  if ( ih >=0 && ih <  m_numptt) {
    h_45_.at(ih)->Fill((verx -ExMissRefFinal45 )/1000.0);
    h_45_.at(ih)->Fill((very -EyMissRefFinal45 )/1000.0);
    r_45_.at(ih)->Fill(rap_45);
    p_45_.at(ih)->Fill(ratio_45);
    
    //PETER plots
    if (data->refFinal()->sumet_eta45()>0.){
    	hco_45_.at(ih)->Fill(data->cellOut_Eflow()->sumet_eta45()/data->refFinal()->sumet_eta45());
    	hsj_45_.at(ih)->Fill(data->softJets()->sumet_eta45()/data->refFinal()->sumet_eta45()); 
    	hrj_45_.at(ih)->Fill(data->refJet()->sumet_eta45()/data->refFinal()->sumet_eta45()); 
    }
    
 }


  //LCW
  ih =(int)((Sumet_per_curve1_45-m_startpt)/m_binpt);
  if (Sumet_per_curve1_45 < m_startpt) {ih = -1;}
  if ( ih >=0 && ih <  m_numptt) {
    h1_45_.at(ih)->Fill((verx -ExMissFinalLocHad45 )/1000.0);
    h1_45_.at(ih)->Fill((very -EyMissFinalLocHad45 )/1000.0);
    r1_45_.at(ih)->Fill(rap1_45);
    p1_45_.at(ih)->Fill(ratio1_45);
  }

  //EMSCALE 
  ih =(int)((Sumet_per_curve2_45-m_startpt)/m_binpt);
  if (Sumet_per_curve2_45 < m_startpt) {ih = -1;}
  if ( ih >=0 && ih <  m_numptt) {
    h2_45_.at(ih)->Fill((verx - ExMissEmScale45)/1000.0);
    h2_45_.at(ih)->Fill((very - EyMissEmScale45)/1000.0);
    r2_45_.at(ih)->Fill(rap2_45);
    //p2_45_.at(ih)->Fill(ratio2_45);
  }

  //GCW
  ih =(int)((Sumet_per_curve3_45-m_startpt)/m_binpt);
  if (Sumet_per_curve3_45 < m_startpt) {ih = -1;}
  if ( ih >=0 && ih <  m_numptt) {
    h3_45_.at(ih)->Fill((verx - ExMissGlobCalib45)/1000.0);
    h3_45_.at(ih)->Fill((very - EyMissGlobCalib45)/1000.0);
    r3_45_.at(ih)->Fill(rap3_45);
    p3_45_.at(ih)->Fill(ratio3_45);
  }


//RF LCW_pt20
  ih =(int)((Sumet_per_curve4_45-m_startpt)/m_binpt);
  if (Sumet_per_curve4_45 < m_startpt) {ih = -1;}
  if ( ih >=0 && ih <  m_numptt) {
    h4_45_.at(ih)->Fill((verx - ExMissRF_LCW_pt20_45)/1000.0);
    h4_45_.at(ih)->Fill((very - EyMissRF_LCW_pt20_45)/1000.0);
    r4_45_.at(ih)->Fill(rap4_45);
    p4_45_.at(ih)->Fill(ratio4_45);
  }  
  //RF GCW_pt20
  ih =(int)((Sumet_per_curve5_45-m_startpt)/m_binpt);
  if (Sumet_per_curve5_45 < m_startpt) {ih = -1;}
  if ( ih >=0 && ih <  m_numptt) {
    h5_45_.at(ih)->Fill((verx - ExMissRF_GCW_pt20_45)/1000.0);
    h5_45_.at(ih)->Fill((very - EyMissRF_GCW_pt20_45)/1000.0);
    r5_45_.at(ih)->Fill(rap5_45);
    p5_45_.at(ih)->Fill(ratio5_45);
 }
  
 
  //RefFinal_em
  ih =(int)((Sumet_per_curve6_45-m_startpt)/m_binpt);
  if (Sumet_per_curve6_45 < m_startpt) {ih = -1;}
  if ( ih >=0 && ih <  m_numptt) {
    h6_45_.at(ih)->Fill((verx - ExMissRefFinal45_em)/1000.0);
    h6_45_.at(ih)->Fill((very - EyMissRefFinal45_em)/1000.0);
    r6_45_.at(ih)->Fill(rap6_45);
    p6_45_.at(ih)->Fill(ratio6_45);
   }  
 
 
  
  //RefFinal LCW_NI_eflow4
  ih =(int)((Sumet_per_curve7_45-m_startpt)/m_binpt);
  if (Sumet_per_curve7_45 < m_startpt) {ih = -1;}
  if ( ih >=0 && ih <  m_numptt) {
    h7_45_.at(ih)->Fill((verx - ExMissRF_LCW_NI_eflow4_45)/1000.0);
    h7_45_.at(ih)->Fill((very - EyMissRF_LCW_NI_eflow4_45)/1000.0);
    r7_45_.at(ih)->Fill(rap7_45);
    p7_45_.at(ih)->Fill(ratio7_45);
   }  
  
  
 //RefFinal LCW_NI_eflow_CB 
  ih =(int)((Sumet_per_curve8_45-m_startpt)/m_binpt);
  if (Sumet_per_curve8_45 < m_startpt) {ih = -1;}
  if ( ih >=0 && ih <  m_numptt) {
    h8_45_.at(ih)->Fill((verx - ExMissRF_LCW_NI_eflow_CB_45)/1000.0);
    h8_45_.at(ih)->Fill((very - EyMissRF_LCW_NI_eflow_CB_45)/1000.0);
    r8_45_.at(ih)->Fill(rap8_45);
    p8_45_.at(ih)->Fill(ratio8_45);
   
  }  
  
 //RefFinal LCW_NI_eflow_400
  ih =(int)((Sumet_per_curve9_45-m_startpt)/m_binpt);
  if (Sumet_per_curve9_45 < m_startpt) {ih = -1;}
  if ( ih >=0 && ih <  m_numptt) {
    h9_45_.at(ih)->Fill((verx - ExMissRF_LCW_NI_eflow_400_45)/1000.0);
    h9_45_.at(ih)->Fill((very - EyMissRF_LCW_NI_eflow_400_45)/1000.0);
    r9_45_.at(ih)->Fill(rap9_45);
    p9_45_.at(ih)->Fill(ratio9_45);
   
  }  

 //RefFinal LCW_NI
  ih =(int)((Sumet_per_curve10_45-m_startpt)/m_binpt);
  if (Sumet_per_curve10_45 < m_startpt) {ih = -1;}
  if ( ih >=0 && ih <  m_numptt) {
    h10_45_.at(ih)->Fill((verx - ExMissRF_LCW_NI_45)/1000.0);
    h10_45_.at(ih)->Fill((very - EyMissRF_LCW_NI_45)/1000.0);
    r10_45_.at(ih)->Fill(rap10_45);
    p10_45_.at(ih)->Fill(ratio10_45);
   
  }  


 //RefFinalHT
  ih =(int)((Sumet_per_curveHT_45-m_startpt)/m_binpt);
  if (Sumet_per_curveHT_45 < m_startpt) {ih = -1;}
  if ( ih >=0 && ih <  m_numptt) {
    hHT_45_.at(ih)->Fill((verx - ExMissRefFinalHT_45)/1000.0);
    hHT_45_.at(ih)->Fill((very - EyMissRefFinalHT_45)/1000.0);
    rHT_45_.at(ih)->Fill(rapHT_45);
  }  

 //RF_LCW_NI_eflow4HT
  ih =(int)((Sumet_per_curveHT7_45-m_startpt)/m_binpt);
  if (Sumet_per_curveHT7_45 < m_startpt) {ih = -1;}
  if ( ih >=0 && ih <  m_numptt) {
    hHT7_45_.at(ih)->Fill((verx - ExMissRF_LCW_NI_eflow4HT_45)/1000.0);
    hHT7_45_.at(ih)->Fill((very - EyMissRF_LCW_NI_eflow4HT_45)/1000.0);
    rHT7_45_.at(ih)->Fill(rapHT7_45);
  }  

 //RF_LCW_NI_eflow400HT
  ih =(int)((Sumet_per_curveHT9_45-m_startpt)/m_binpt);
  if (Sumet_per_curveHT9_45 < m_startpt) {ih = -1;}
  if ( ih >=0 && ih <  m_numptt) {
    hHT9_45_.at(ih)->Fill((verx - ExMissRF_LCW_NI_eflow400HT_45)/1000.0);
    hHT9_45_.at(ih)->Fill((very - EyMissRF_LCW_NI_eflow400HT_45)/1000.0);
    rHT9_45_.at(ih)->Fill(rapHT9_45);
  }  

  
}

