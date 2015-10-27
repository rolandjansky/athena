/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  Author:  Jiri Kvita
  Purpose: Test the calib interfaces
  Created: Wed Feb 23 18:02:57 CET 2011
  Usage:

    root -l 
    []  gSystem -> Load("libCalibrationDataInterface.so");
    [] .x TestInterface.C+

*/

#include "CalibrationDataInterfaceBase.h"
#include "CalibrationDataInterfaceROOT.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <utility>

using std::cout;
using std::cerr;
using std::endl;

#include "TStyle.h"
#include "TSystem.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TF2.h"
#include "TF1.h"
#include "TString.h"
#include "TCanvas.h"
#include "TGraphErrors.h"
#include "TLegend.h"
#include "TLatex.h"

#include "TROOT.h"
#include "SetStyle.h"

struct calib{ 
  Analysis::CalibrationDataInterfaceROOT* cal;
  std::string root;
  std::string tagger;
  std::string analysis;
  std::string flav;
  std::string cut;
  Analysis::CalibrationDataVariables btagjet; 
  std::string name;
  Analysis::Uncertainty unc;
  bool area;
};

std::string createFile(std::string, std::string, std::string, std::string);
void  addToList(std::vector <calib>&, calib);
void  setBinning(std::vector<double>&, std::string);
//-------- Place to configure plot -----------//
void setAnalyses(std::vector <calib>& calib_list){

  std::string filetmp;
  calib tmp;

  // tmp.root               = "2013-LP-MC12-CDI.root";
  // tmp.analysis           = "default";
  // tmp.tagger             = "MV1";
  // tmp.flav               = "B";
  // tmp.cut                = "0_8119";
  // tmp.btagjet.jetAuthor  = "AntiKt4TopoEMJVF0_5";
  // tmp.name               = "EM JVF0_5";
  // tmp.unc                = Analysis::Total;
  // tmp.area               = true;
  // addToList(calib_list,tmp);

  // tmp.root               = "2013-LP-MC12-CDI.root";
  // tmp.analysis           = "default";
  // tmp.tagger             = "MV1";
  // tmp.flav               = "B";
  // tmp.cut                = "0_8119";
  // tmp.btagjet.jetAuthor  = "AntiKt4TopoEMJVF0_5";
  // tmp.name               = "EM JVF0_5";
  // tmp.unc                = Analysis::Statistical;
  // tmp.area               = true;
  // addToList(calib_list,tmp);

  tmp.root               = "2013-LP-MC12-CDI_2709.root";
  tmp.analysis           = "default";
  tmp.tagger             = "MV1";
  tmp.flav               = "B";
  tmp.cut                = "0_8119";
  tmp.btagjet.jetAuthor  = "AntiKt4TopoEMJVF0_5";
  tmp.name               = "Comb (tot. error)";
  tmp.unc                = Analysis::Total;
  tmp.area               = true;
  addToList(calib_list,tmp);

  tmp.root               = "2013-LP-MC12-CDI_2709.root";
  tmp.analysis           = "default";
  tmp.tagger             = "MV1";
  tmp.flav               = "B";
  tmp.cut                = "0_8119";
  tmp.btagjet.jetAuthor  = "AntiKt4TopoEMJVF0_5";
  tmp.name               = "Comb (stat. error)";
  tmp.unc                = Analysis::Statistical;
  tmp.area               = true;
  addToList(calib_list,tmp);

  // tmp.root               = "2013-LP-MC12-CDI-CDI_2709.root";
  // tmp.analysis           = "default";
  // tmp.tagger             = "MV1";
  // tmp.flav               = "B";
  // tmp.cut                = "0_9867";
  // tmp.btagjet.jetAuthor  = "AntiKt4TopoEMJVF0_5";
  // tmp.name               = "EMJVF05 noJVF";
  // tmp.unc                = Analysis::Total;
  // tmp.area               = false;
  // addToList(calib_list,tmp);

  // tmp.root               = "2013-LP-MC12-CDI-CDI_2709.root";
  // tmp.analysis           = "default";
  // tmp.tagger             = "MV1";
  // tmp.flav               = "B";
  // tmp.cut                = "0_9867";
  // tmp.btagjet.jetAuthor  = "AntiKt4TopoEMJVF0_5";
  // tmp.name               = "EMJVF05 noJVF";
  // tmp.unc                = Analysis::Total;
  // tmp.area               = false;
  // addToList(calib_list,tmp);


  // 
  // tmp.root               = "2013-LP-MC12-CDI-CDI_2709.root";
  // tmp.analysis           = "KinSel_dilep_3j";
  // tmp.tagger             = "MV1";
  // tmp.flav               = "B";
  // tmp.cut                = "0_9867";
  // tmp.btagjet.jetAuthor  = "AntiKt4TopoEMJVF0_5";
  // tmp.name               = "EMJVF05 KinSel dilep (3j)";
  // tmp.unc                = Analysis::Total;
  // tmp.area               = false;
  // addToList(calib_list,tmp);

  // tmp.root               = "2013-LP-MC12-CDI-CDI_2709.root";
  // tmp.analysis           = "KinSel_dilep_2j";
  // tmp.tagger             = "MV1";
  // tmp.flav               = "B";
  // tmp.cut                = "0_9867";
  // tmp.btagjet.jetAuthor  = "AntiKt4TopoEMJVF0_5";
  // tmp.name               = "EMJVF05 KinSel dilep (2j)";
  // tmp.unc                = Analysis::Total;
  // tmp.area               = false;
  // addToList(calib_list,tmp);

  tmp.root               = "2013-LP-MC12-CDI_2709.root";
  tmp.analysis           = "system8";
  tmp.tagger             = "MV1";
  tmp.flav               = "B";
  tmp.cut                = "0_8119";
  tmp.btagjet.jetAuthor  = "AntiKt4TopoEMJVF0_5";
  tmp.name               = "system8";
  tmp.unc                = Analysis::Total;
  tmp.area               = false;
  addToList(calib_list,tmp);

  tmp.root               = "2013-LP-MC12-CDI_2709.root";
  tmp.analysis           = "PDF_ll_7_fit";
  tmp.tagger             = "MV1";
  tmp.flav               = "B";
  tmp.cut                = "0_8119";
  tmp.btagjet.jetAuthor  = "AntiKt4TopoEMJVF0_5";
  tmp.name               = "ttbar PDF method";
  tmp.unc                = Analysis::Total;
  tmp.area               = false;
  addToList(calib_list,tmp);

}
//--------------------------------------------//

void  setBinning(std::vector<double>& bins,std::string flav){

  bins.clear();
  if (flav=="B"){
    bins.push_back(20);
    bins.push_back(30);
    bins.push_back(40);
    bins.push_back(50);
    bins.push_back(60);
    bins.push_back(75);
    bins.push_back(90);
    bins.push_back(110);
    bins.push_back(140);
    bins.push_back(200);
    bins.push_back(300);
  } else if (flav=="C"){
    bins.push_back(20);
    bins.push_back(30);
    bins.push_back(60);
    bins.push_back(90);
    bins.push_back(140);
    bins.push_back(200);
    bins.push_back(300);
  } else if (flav=="Light"){
    bins.push_back( 20);
    bins.push_back( 30);
    bins.push_back( 60);
    bins.push_back(140);
    bins.push_back(300);
    bins.push_back(450);
    bins.push_back(750);
  }

}

void  addToList(std::vector <calib>& calib_list, calib tmp){
  std::string filetmp    = createFile(tmp.root,tmp.tagger,tmp.flav,tmp.analysis);
  tmp.cal                = new Analysis::CalibrationDataInterfaceROOT(tmp.tagger.c_str(),filetmp.c_str());
  calib_list.push_back(tmp);
}

std::string createFile(std::string root, std::string tagger, std::string flav, std::string analysis){
  std::string file = root; file+=".env";
  std::ofstream fout(file.c_str());
  fout << "File: " << root <<endl;
  fout << tagger << ".ScaleFactorCalibration" << flav << "Name: " << analysis << endl;
  fout.close();
  return file;
}

void plotCalib(){

  gStyle -> SetPalette(1);
  SetStyle();

  std::vector <calib> calib_list;
  setAnalyses(calib_list);

  int n = calib_list.size();
  TGraphErrors** gr = new TGraphErrors*[n];
  for (int i=0;i<n;i++)
    gr[i] = new TGraphErrors;


  int sym[10] ={1,20,20,21,22,23,32,34,36,40};
  int colp[10]={2,1,50,49,92,9,42,10,10,10};
  int cola[10]={kGreen-8,kGreen-5,kGreen-5,0,0,0,0,0,0,0};

  std::vector<double> xbins;
  for (int j=0;j<n;j++){
    setBinning(xbins,calib_list[j].flav);

    //double wdel = (log10(xbins[xbins.size()-1])-log10(xbins[0]))/2;
 
    int m = n-1;
    double del=2;
    vector<double> offset;
    if (m!=0){
      if (m==1){
	del=0; 
      } else if (m%2==0){
	for (int k=0;k<m;k++){
	  double xdel=del;
	  int sign=1;
	  if (k%2==0) sign=-1;
	  if (k<2){
	    xdel = xdel/2;
	  } else {
	    xdel = xdel*(k/2);
	  }
	  //        xdel = 0;
	  offset.push_back(pow(10,1)*xdel*sign);
	  cout << " " << xdel*sign << endl;
	}
      } else if (m%2==1){
	for (int k=0;k<m;k++){
	  double xdel=del;
	  int sign=1;
	  if (k-1%2) sign=-1;
	  if (k==0){
	    xdel = 0;
	  } else {
	    xdel = xdel*((k+1)/2);
	}
	  //        xdel = 0;
	  offset.push_back(xdel*sign);
	}
      }
    }


    //   double w=xbins[xbins.size()-1]-xbins[0];
    for (unsigned int i=0;i<xbins.size()-1;i++){
      if (calib_list[j].name.find("DStar/HCP2012")!=string::npos && i==5) continue;
      if (calib_list[j].name.find("pTrel")!=string::npos && i==9) continue;
      if (calib_list[j].name.find("system8")!=string::npos && i==0) continue;
      if (calib_list[j].name.find("muon")!=string::npos && i==9) continue;
      if (calib_list[j].name.find("fit")!=string::npos && i==9) continue;
      //      if (calib_list[j].name.find("ttbar")!=string::npos && i==0) continue;
      if (calib_list[j].name.find("dijet")!=string::npos && i==9) continue;
      if (j==0 || calib_list[j].area) offset[j]=0;
      double ept = (xbins[i+1]-xbins[i])/2;
      double  pt = (xbins[i+1]+xbins[i])/2 + (xbins[i+1]-xbins[i])/20*offset[j];
      calib_list[j].btagjet.jetPt   = pt*1000.; 
      calib_list[j].btagjet.jetEta  = 1.;
      std::pair<double,double> SFtot = calib_list[j].cal->getScaleFactor( calib_list[j].btagjet, calib_list[j].flav, calib_list[j].cut,calib_list[j].unc);    
      //      std::pair<double,double> SFtot = calib_list[j].cal->getScaleFactor(calib_list[j].btagjet,"B","0_0",unc);
      //      if (calib_list[j].analysis.find("ttbar")!=string::npos && i==0) SFtot.first=-100;

      gr[j]->SetPoint(i,pt,SFtot.first);	

      if (calib_list[j].area){
	gr[j]->SetPointError(i,ept,SFtot.second);
      } else {
	gr[j]->SetPointError(i,0,SFtot.second);
      }
    }
    if (j==0){
      new TCanvas("c","",10,10,800,400);
      //      c->SetBottomMargin(4.0);
      if (calib_list[j].area){
	gr[j]->Draw("AE2");
      } else {
	gr[j]->Draw("AP");
      }
      if (calib_list[j].flav=="C"){
        gr[j]->GetXaxis()->SetLimits(0,300);
      }
      gr[j]->SetMinimum(0.6);
      gr[j]->SetMaximum(1.4);
      gr[j]->GetXaxis()->SetRangeUser(10.,800.);
      gr[j]->SetFillColor(cola[j]);
      gr[j]->GetYaxis()->SetTitleOffset(1.2);
      if (calib_list[j].flav=="B")
	gr[j]->GetYaxis()->SetTitle("b-jet efficiency scale factor");
      else if (calib_list[j].flav=="C")
	gr[j]->GetYaxis()->SetTitle("c-jet efficiency SF");
      else if (calib_list[j].flav=="Light")
	gr[j]->GetYaxis()->SetTitle("mistag rate SF");
      gr[j]->GetXaxis()->SetTitle("jet p_{T} [GeV]");
    } else {
      if (calib_list[j].area){
	gr[j]->Draw("E2");
	gr[j]->SetFillColor(cola[j]);
      } else {
	gr[j]->Draw("P");
	gr[j]->SetMarkerStyle(sym[j]);
	gr[j]->SetMarkerColor(colp[j]);
      }
    }
    gr[j]->SetMarkerSize(1.0);
  }
  
  TLegend *leg = new TLegend(0.14,0.24,0.6,0.40);
  leg->SetFillColor(0);
  for (int j=0;j<n;j++){
    if (calib_list[j].area)
      leg->AddEntry(gr[j],calib_list[j].name.c_str(),"F");  
    else
      leg->AddEntry(gr[j],calib_list[j].name.c_str(),"P");  
  }
  leg->Draw();
  leg->SetBorderSize(0);
  gr[0]->GetYaxis()->SetTitleOffset(0.8);
  gr[0]->GetYaxis()->SetNdivisions(505);
  gr[0]->GetXaxis()->SetTitleOffset(1.5);

  gPad->SetLogx();
  //  gr[0]->SetMinimum(0.2);
  // gr[0]->SetMaximum(3);
  gr[0]->SetMinimum(0.4);
  gr[0]->SetMaximum(1.4);
  gr[0]->GetXaxis()->SetMoreLogLabels();
  
  TLatex la;
  
  la.SetNDC();
  la.SetTextFont(72);
  la.SetTextColor(kBlack);
  la.DrawLatex(0.15,0.82,"ATLAS");
  la.SetTextFont(42);
  TLatex lx;
  lx.SetNDC();
  
  
  TText *textPa = new TText(0,0,"");
  textPa->SetTextColor(1);
  textPa->DrawTextNDC(0.25,0.82,"Preliminary");

  TLatex *textLu = new TLatex(0,0,"");
  textLu->SetNDC();
  textLu->DrawLatex(0.47,0.82,"#int L dt = 20.3 fb^{-1}     #sqrt{s} = 8 TeV");

  TLatex *textLux = new TLatex(0,0,"");
  textLux->SetNDC();
  textLux->DrawLatex(0.67,0.25,"MV1, #epsilon_{b} = 70%");


}
