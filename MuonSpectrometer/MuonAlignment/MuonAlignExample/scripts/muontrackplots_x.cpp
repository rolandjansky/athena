/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////
//
// muontrackplots_x
// ----------------
// 
//////////////////////////////////////////////////////////////////////////////

#include "MuonFixedId.h"
#include "Tools.h"

#include "TStyle.h"
#include <TChain.h>
#include <TTree.h>
#include <TEventList.h>
#include <TLorentzVector.h>
#include <TMatrixDSym.h>
#include <TVectorD.h>
#include <TDecompBK.h>
#include <TMatrixTSparse.h>
#include <TGraph.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TPaveText.h>
#include <TAxis.h>
#include "TFile.h"
#include "TH1F.h"
#include "TStyle.h"
#include "TLine.h"
#include "TSystem.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <string>
#include <map>
#include <cmath>
#include <vector>
#include <set>

#include "TH2F.h"

using namespace std;
using namespace MuonCalib;
 
const bool BATCH=false;
const bool doL1=true;

bool DEBUG=0;

int m_ntrees=0;

void help(const char *argv0) {
    
  cerr << "Usage: " << argv0 << " [options] " << endl
       << "Options:" << endl
       << " -o :\tOutput path"  << endl
       << " -i :\tInput files" << endl
       << " -r :\tRootname"  << endl
       << " -j :\tIteration"  << endl
       << " -a :\tDraw absolute lines"  << endl
       << " -l :\tL1 alignment" << endl
       << " -d :\tDebug"  << endl
       << " -h :\tShow this message and quit"  << endl
       << endl;
  exit(1);
}

//_____________________________________________________________________________
int main(int argc, char** argv)
{

  //----------------------------
  //-- Parse the command line --
  //---------------------------- 
  
  extern char *optarg;
  char c;
  TString inputdir("");
  TString outputdir("");
  TString rootname("");
  int iteration = 0;
 
  bool L1ALIGNMENT = false;
  bool ABSLINES = false;

  while((c = getopt(argc, argv, "-hdalo:i:r:j:")) !=-1){
    switch(c){
    case 'o': 
      outputdir = optarg;
      break;
    case 'i':
      inputdir = optarg;
      break;
    case 'r':
      rootname = optarg;
      break;
    case 'j':
      iteration = atoi(optarg);
      break;
    case 'd':
      DEBUG = true;
      break;
    case 'a':
      ABSLINES = true;
      break;
    case 'l':
      L1ALIGNMENT = true;
      break;
    default:
      cerr << "Help or unknown argument!" << endl;
      help(argv[0]);
      break; 
    }
  }
  if(rootname==""){
    cerr << "no rootname passed, exiting..." << endl;
    exit(1);
  } 
  if(inputdir==""){
    inputdir = rootname;
  } 
  if(outputdir==""){
    outputdir = inputdir;
  }  


  gStyle->SetOptStat(1);
  //gStyle->SetOptFit(11111);
  gStyle->SetFrameFillStyle(0);
  gStyle->SetTitleFillColor(0);
  gStyle->SetTitleBorderSize(1);
  gStyle->SetPadLeftMargin(0.15);
  gStyle->SetPadRightMargin(0.04);
  gStyle->SetPalette(1);

  void *directory = gSystem->OpenDirectory(inputdir);
  if (!directory){
    cerr << "input directory does not exist, exiting..." << endl;
    exit(1);
  } 
  directory = gSystem->OpenDirectory(outputdir);
  if (!directory){
    cerr << "create output directory: " << outputdir << endl;
    int status = gSystem->mkdir(outputdir,false);
    if(status!=0){
      cerr << "could not create output directory, exiting..." << endl;
      exit(1);
    }
  } 
  
  TString iterdir = inputdir+"/iter0";
  directory = gSystem->OpenDirectory(inputdir+"/iter0");
  bool multiplejobs = directory ? true : false; 
  cout << "multiplejobs: " << multiplejobs << endl; 

  TString basefilename;
  if(multiplejobs){
    TString iterdir = inputdir+Form("/iter%i/",iteration);
    directory = gSystem->OpenDirectory(iterdir);
    basefilename = iterdir;
    if(!directory){
      cerr << "iteration: " << iteration << "does not exists, exiting..." << endl;
      exit(1);
    } 
  }
  else{
    basefilename = inputdir+"/";  
  }

  //basefilename += "MuonAlign_"+rootname+Form("_iter%i",iteration);
  basefilename += rootname;

  TString muonalign_filename = basefilename;
  TString alignpar_filename = basefilename;
  if(multiplejobs){  
    //muonalign_filename += "_index*";
    muonalign_filename += "*";
    alignpar_filename += "_solve";
  }
  muonalign_filename += ".root";
  alignpar_filename += ".root";

  TChain* tree=new TChain("MuonAlign");
  tree->Add(muonalign_filename);

  m_ntrees=tree->GetNtrees();
  std::cout<<"have "<<m_ntrees<<" trees"<<std::endl;

  int     run,event;
  double* chi2VAlignParamQuality=new double[6]; 
  double  trackChi2,trackChi2Dof,idTrackChi2,idTrackChi2Dof,msTrackChi2,msTrackChi2Dof;
  double  trackEta,energyLoss;   
  int     nSctHits,nPixHits,nTrtHits,nMdtHits,nRpcPhiHits,nTgcPhiHits;
  double  msidPtDiff,idPtAtIP,idZ0AtIP,idD0AtIP,msPtAtIP,msZ0AtIP,msD0AtIP;
  double  truth_pt_gen,truth_pt_msentrance;
  int     nMuonOutliers,nTgcOutliers;

  // set branch addresses
  tree->SetBranchAddress("run",                   &run);
  tree->SetBranchAddress("evt",                   &event);
  tree->SetBranchAddress("chi2VAlignParamQuality", chi2VAlignParamQuality);
  tree->SetBranchAddress("trackChi2",           &trackChi2);
  tree->SetBranchAddress("trackChi2Dof",        &trackChi2Dof);
  tree->SetBranchAddress("idTrackChi2",         &idTrackChi2);
  tree->SetBranchAddress("idTrackChi2Dof",      &idTrackChi2Dof);
  tree->SetBranchAddress("msTrackChi2",         &msTrackChi2);
  tree->SetBranchAddress("msTrackChi2Dof",      &msTrackChi2Dof);
  tree->SetBranchAddress("trackEta",            &trackEta);
  tree->SetBranchAddress("energyLoss",          &energyLoss);
  tree->SetBranchAddress("nSctHits",            &nSctHits);
  tree->SetBranchAddress("nPixHits",            &nPixHits);
  tree->SetBranchAddress("nTrtHits",            &nTrtHits);
  tree->SetBranchAddress("msidPtDiff",          &msidPtDiff);
  tree->SetBranchAddress("idPtAtIP",            &idPtAtIP);
  tree->SetBranchAddress("idZ0AtIP",            &idZ0AtIP);
  tree->SetBranchAddress("idD0AtIP",            &idD0AtIP);
  tree->SetBranchAddress("msPtAtIP",            &msPtAtIP);
  tree->SetBranchAddress("msZ0AtIP",            &msZ0AtIP);
  tree->SetBranchAddress("msD0AtIP",            &msD0AtIP);
  tree->SetBranchAddress("nMdtHits",            &nMdtHits);
  tree->SetBranchAddress("nRpcPhiHits",         &nRpcPhiHits);
  tree->SetBranchAddress("nTgcPhiHits",         &nTgcPhiHits);
  tree->SetBranchAddress("truth_pt_gen",        &truth_pt_gen);
  tree->SetBranchAddress("truth_pt_msentrance", &truth_pt_msentrance);
  tree->SetBranchAddress("nMuonOutliers",       &nMuonOutliers);
  tree->SetBranchAddress("nTgcOutliers",        &nTgcOutliers);

  const int NHIST=21;

  /*string names[NHIST]={"trackChi2","trackChi2Dof","IDTrackChi2","IDTrackChi2Dof",
		       "MSTrackChi2","MSTrackChi2Dof",
		       "trackEta", "energyLoss",
			"nSCTHits", "nPixHits", "nTRTHits",
			"msidPtDiff",
			"ID_pTatIP","ID_z0atIP","ID_d0atIP",
			"MS_pTatIP","MS_z0atIP","MS_d0atIP",
			"nMDT_hits", "nRPC_phiHits", "nTGC_phiHits"};		  
  */

  TH2F** hVarVFitChi2=new TH2F*[NHIST];
  const int NCHI2BIN=50;
  const double CHI2MIN=-30.;
  const double CHI2MAX=10.;
  
  hVarVFitChi2[0]=new TH2F("hVarVFitChi2_0","track chi2",NCHI2BIN,CHI2MIN,CHI2MAX,
			   30,0.,150.);
  hVarVFitChi2[0]->GetYaxis()->SetTitle("track #chi^{2}");

  hVarVFitChi2[1]=new TH2F("hVarVFitChi2_1","track chi2 per DOF",NCHI2BIN,CHI2MIN,CHI2MAX,
			   30,0.,3.);
  hVarVFitChi2[1]->GetYaxis()->SetTitle("track #chi^{2}/DOF");

  hVarVFitChi2[2]=new TH2F("hVarVFitChi2_2","ID track chi2",NCHI2BIN,CHI2MIN,CHI2MAX,
			   30,0.,100.);
  hVarVFitChi2[2]->GetYaxis()->SetTitle("ID track #chi^{2}");

  hVarVFitChi2[3]=new TH2F("hVarVFitChi2_3","ID track chi2 per DOF",NCHI2BIN,CHI2MIN,CHI2MAX,
			   30,0.,1.6);
  hVarVFitChi2[3]->GetYaxis()->SetTitle("ID track #chi^{2}/DOF");

  hVarVFitChi2[4]=new TH2F("hVarVFitChi2_4","MS track chi2",NCHI2BIN,CHI2MIN,CHI2MAX,
			   30,0.,150.);
  hVarVFitChi2[4]->GetYaxis()->SetTitle("MS track #chi^{2}");

  hVarVFitChi2[5]=new TH2F("hVarVFitChi2_5","MS track chi2 per DOF",NCHI2BIN,CHI2MIN,CHI2MAX,
			   30,0.,2.5);
  hVarVFitChi2[5]->GetYaxis()->SetTitle("MS track #chi^{2}/DOF");

  hVarVFitChi2[6]=new TH2F("hVarVFitChi2_6","muon track eta",NCHI2BIN,CHI2MIN,CHI2MAX,
			   30,1.0,2.8);
  hVarVFitChi2[6]->GetYaxis()->SetTitle("muon track |#eta|");

  hVarVFitChi2[7]=new TH2F("hVarVFitChi2_7","energy loss",NCHI2BIN,CHI2MIN,CHI2MAX,
			   30,3.,5.);
  hVarVFitChi2[7]->GetYaxis()->SetTitle("energy loss (GeV)");

  hVarVFitChi2[8]=new TH2F("hVarVFitChi2_8","number SCT hits",NCHI2BIN,CHI2MIN,CHI2MAX,
			   30,0.,20.);
  hVarVFitChi2[8]->GetYaxis()->SetTitle("number SCT hits");

  hVarVFitChi2[9]=new TH2F("hVarVFitChi2_9","number pixel hits",NCHI2BIN,CHI2MIN,CHI2MAX,
			   30,0.,10.);
  hVarVFitChi2[9]->GetYaxis()->SetTitle("number pixel hits");

  hVarVFitChi2[10]=new TH2F("hVarVFitChi2_10","number TRT hits",NCHI2BIN,CHI2MIN,CHI2MAX,
			   30,0.,50.);
  hVarVFitChi2[10]->GetYaxis()->SetTitle("number TRT hits");

  hVarVFitChi2[11]=new TH2F("hVarVFitChi2_11","msid_ptDiff",NCHI2BIN,CHI2MIN,CHI2MAX,
			   30,0.,.16);
  hVarVFitChi2[11]->GetYaxis()->SetTitle("|1-pT_{ID}/pT_{MS}|");

  hVarVFitChi2[12]=new TH2F("hVarVFitChi2_12","ID pT at IP",NCHI2BIN,CHI2MIN,CHI2MAX,
			   30,0.,150.);
  hVarVFitChi2[12]->GetYaxis()->SetTitle("ID pT at IP");

  hVarVFitChi2[13]=new TH2F("hVarVFitChi2_13","ID z0 at IP",NCHI2BIN,CHI2MIN,CHI2MAX,
			   30,-170.,170.);
  hVarVFitChi2[13]->GetYaxis()->SetTitle("ID z0 at IP");

  hVarVFitChi2[14]=new TH2F("hVarVFitChi2_14","ID d0 at IP",NCHI2BIN,CHI2MIN,CHI2MAX,
			   30,-0.15,0.15);
  hVarVFitChi2[14]->GetYaxis()->SetTitle("ID d0 at IP");

  hVarVFitChi2[15]=new TH2F("hVarVFitChi2_15","MS pT at IP",NCHI2BIN,CHI2MIN,CHI2MAX,
			   30,0.,50.);
  hVarVFitChi2[15]->GetYaxis()->SetTitle("MS pT at IP");

  hVarVFitChi2[16]=new TH2F("hVarVFitChi2_16","MS z0 at IP",NCHI2BIN,CHI2MIN,CHI2MAX,
			   30,-180.,180.);
  hVarVFitChi2[16]->GetYaxis()->SetTitle("MS z0 at IP");

  hVarVFitChi2[17]=new TH2F("hVarVFitChi2_17","MS d0 at IP",NCHI2BIN,CHI2MIN,CHI2MAX,
			   30,-0.25,0.25);
  hVarVFitChi2[17]->GetYaxis()->SetTitle("MS d0 at IP");

  hVarVFitChi2[18]=new TH2F("hVarVFitChi2_18","number MDT hits",NCHI2BIN,CHI2MIN,CHI2MAX,
			   30,0.,50.);
  hVarVFitChi2[18]->GetYaxis()->SetTitle("number MDT hits");

  hVarVFitChi2[19]=new TH2F("hVarVFitChi2_19","number RPC phi hits",NCHI2BIN,CHI2MIN,CHI2MAX,
			   30,0.,50.);
  hVarVFitChi2[19]->GetYaxis()->SetTitle("number RPC phi hits");

  hVarVFitChi2[20]=new TH2F("hVarVFitChi2_20","number TGC phi hits",NCHI2BIN,CHI2MIN,CHI2MAX,
			   30,0.,50.);
  hVarVFitChi2[20]->GetYaxis()->SetTitle("number TGC phi hits");

  TH2F* hDiscVNMuonOutliers = new TH2F("hDiscVNMuonOutliers", "log(discontinuity chi2) vs. number muon outliers",
				       5,0,5,50,-10,0.);
  TH2F* hDiscVNTgcOutliers = new TH2F("hDiscVNTgcOutliers", "log(discontinuity chi2) vs. number TGC outliers",
				       5,0,5,50,-10,0.);
  TH1F* hnTgcOutliers = new TH1F("hnTgcOutliers", "number TGC outliers",
				       5,0,5);
  for (int i=0;i<NHIST;i++) 
    hVarVFitChi2[i]->GetXaxis()->SetTitle("log(#chi^{2} of #chi^{2} v. align param)"); 

  // fill histograms
  for (int ientry=0;ientry<tree->GetEntries();ientry++) {
    
    tree->GetEntry(ientry);
    /*      
	    double worstchi2=0.;
	    for (int i=0;i<6;i++) 
	    if (chi2VAlignParamQuality[i]>worstchi2)
	    worstchi2=chi2VAlignParamQuality[i];
	    std::cout<<"evt "<<event<<", nMuonOutliers="<<nMuonOutliers<<", worstchi2="<<worstchi2<<std::endl;
	    hDiscVNMuonOutliers->Fill(nMuonOutliers,std::log10(worstchi2)); 
    */
    for (int i=0;i<6;i++) {
        std::cout<<"evt "<<event<<", chi2VAlignParamQuality["<<i<<"]="<<chi2VAlignParamQuality[i]<<std::endl;
	hDiscVNMuonOutliers->Fill(nMuonOutliers,std::log10(chi2VAlignParamQuality[i])); 
	hDiscVNTgcOutliers->Fill(nTgcOutliers,std::log10(chi2VAlignParamQuality[i])); 
	std::cout<<"nTgcOutliers="<<nTgcOutliers<<std::endl;
    }
    hnTgcOutliers->Fill(nTgcOutliers);

    for (int i=0;i<6;i++) {
      //if (chi2VAlignParamQuality[i]>.001) continue;
      double chi2val=std::log10(chi2VAlignParamQuality[i]);
      hVarVFitChi2[0] ->Fill(chi2val,trackChi2);
      hVarVFitChi2[1] ->Fill(chi2val,trackChi2Dof);
      hVarVFitChi2[2] ->Fill(chi2val,idTrackChi2);
      hVarVFitChi2[3] ->Fill(chi2val,idTrackChi2Dof);
      hVarVFitChi2[4] ->Fill(chi2val,msTrackChi2);
      hVarVFitChi2[5] ->Fill(chi2val,msTrackChi2Dof);
      hVarVFitChi2[6] ->Fill(chi2val,trackEta);
      hVarVFitChi2[7] ->Fill(chi2val,energyLoss);
      hVarVFitChi2[8] ->Fill(chi2val,nSctHits);
      hVarVFitChi2[9] ->Fill(chi2val,nPixHits);
      hVarVFitChi2[10]->Fill(chi2val,nTrtHits);
      hVarVFitChi2[11]->Fill(chi2val,msidPtDiff);
      hVarVFitChi2[12]->Fill(chi2val,idPtAtIP);
      hVarVFitChi2[13]->Fill(chi2val,idZ0AtIP);
      hVarVFitChi2[14]->Fill(chi2val,idD0AtIP);
      hVarVFitChi2[15]->Fill(chi2val,msPtAtIP*1000.);
      hVarVFitChi2[16]->Fill(chi2val,msZ0AtIP);
      hVarVFitChi2[17]->Fill(chi2val,msD0AtIP);
      hVarVFitChi2[18]->Fill(chi2val,nMdtHits);
      hVarVFitChi2[19]->Fill(chi2val,nRpcPhiHits);
      hVarVFitChi2[20]->Fill(chi2val,nTgcPhiHits);
    }            
  }

  TCanvas* canv=new TCanvas("canv","canv");
  canv->SetFillColor(10);
  int ipad(0);
  int icanv(0);
  for (int ihist=0;ihist<NHIST+1;ihist++) {
    if (ihist%6==0 || ihist==NHIST) {
      if (ihist>0) {
	stringstream canvname;
	canvname<<outputdir<<"/muontrackplots_iter"<<iteration<<"_"<<icanv<<".eps";
	canv->Print(canvname.str().c_str());
	canv->Clear();
	icanv++;
      }
      if (ihist==NHIST) continue;
      canv->Divide(3,2);
      ipad=0;
    }   
    canv->cd(++ipad);
    hVarVFitChi2[ihist]->Draw("colz");
    TLine* line0=new TLine(0.,hVarVFitChi2[ihist]->GetYaxis()->GetXmin(),
			   0.,hVarVFitChi2[ihist]->GetYaxis()->GetXmax());
    //line0->SetLineColor(kRed);
    line0->Draw();
    TLine* line1=new TLine(-1.,hVarVFitChi2[ihist]->GetYaxis()->GetXmin(),
			   -1.,hVarVFitChi2[ihist]->GetYaxis()->GetXmax());
    //line1->SetLineColor(kGreen);
    line1->Draw();
    TLine* line2=new TLine(-2.,hVarVFitChi2[ihist]->GetYaxis()->GetXmin(),
			   -2.,hVarVFitChi2[ihist]->GetYaxis()->GetXmax());
    //line2->SetLineColor(kBlue);
    line2->Draw();
    TLine* line3=new TLine(-3.,hVarVFitChi2[ihist]->GetYaxis()->GetXmin(),
			   -3.,hVarVFitChi2[ihist]->GetYaxis()->GetXmax());
    //line2->SetLineColor(kBlue);
    line3->Draw();
  }

  canv->Clear();
  hDiscVNMuonOutliers->Draw("colz");
  stringstream canvname;
  canvname<<outputdir<<"/discVNMuonOutliers_iter"<<iteration<<".eps";
  canv->Print(canvname.str().c_str());

  canv->Clear();
  hDiscVNTgcOutliers->Draw("colz");
  stringstream canvname_tgc;
  canvname_tgc<<outputdir<<"/discVNTgcOutliers_iter"<<iteration<<".eps";
  canv->Print(canvname_tgc.str().c_str());

  canv->Clear();
  hnTgcOutliers->Draw();
  stringstream canvname_tgc1;
  canvname_tgc1<<outputdir<<"/nTgcOutliers_iter"<<iteration<<".eps";
  canv->Print(canvname_tgc1.str().c_str());
  
}


