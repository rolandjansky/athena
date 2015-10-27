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
#include <fstream>
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
#include "TROOT.h"

string myflav;
struct calib{ 
  Analysis::CalibrationDataInterfaceROOT* cal;
  std::string root;
  std::string tagger;
  std::string analysis;
  std::string flav;
  std::string cut;
  Analysis::Uncertainty unc;
  Analysis::CalibrationDataVariables btagjet; 
};

std::string createFile(std::string, std::string, std::string, std::string);
void  addToList(std::vector <calib>&, calib);
void  setBinning(std::vector<double>&, std::string);

void setAnalyses(std::vector <calib>& calib_list){

  std::string filetmp;
  calib tmp;

  tmp.root               = "Summer2013-continuous.root";
  tmp.analysis           = "default";
  tmp.tagger             = "MV1";
  tmp.flav               = myflav;
  tmp.cut                = "continuous";
  tmp.btagjet.jetAuthor  = "AntiKt4TopoEMJVF0_5";
  tmp.unc                = Analysis::Total;
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

void TestInterfaceContinuous(string flav,double eta)
{

  myflav=flav;

  gStyle -> SetPalette(1);

  std::vector <calib> calib_list;
  setAnalyses(calib_list);

  std::vector <double> xbins;
  setBinning(xbins,calib_list[0].flav);

  int ntag=7;
  double tagw[7]={0,0.3900,0.8119,0.9867,0.992515446,0.992670537,1};  

  if (calib_list[0].flav=="B"){
    ntag=7;
  } else   if (calib_list[0].flav=="C"){
    ntag=6;
    tagw[5]=1;
  } else   if (calib_list[0].flav=="Light"){
    ntag=7;
  }

  TCanvas *c = new TCanvas;
  c->Divide(3,2);

  for (unsigned int ib=0;ib<xbins.size()-1;ib++){
    c->cd(ib+1);

    string hs="hs";
    hs+=(long int)ib;
    TH1D *h = new TH1D(hs.c_str(),"",ntag-1,0,ntag-1);
      
    calib_list[0].btagjet.jetPt  = 1000*(xbins[ib]+xbins[ib+1])/2;

    for (int i=0;i<ntag-1;i++){

      calib_list[0].btagjet.jetEta       = eta;
      calib_list[0].btagjet.jetTagWeight = (tagw[i]+tagw[i+1])/2;

      std::pair<double,double> SFtot = calib_list[0].cal->getScaleFactor(calib_list[0].btagjet, calib_list[0].flav, calib_list[0].cut,calib_list[0].unc);

      h->SetBinContent(i+1,SFtot.first);
      h->SetBinError(i+1,SFtot.second);
      if (flav=="Light"){
	h->SetMinimum(0);
	h->SetMaximum(4);
      } else {
	h->SetMinimum(0);
	h->SetMaximum(2);
      }
      h->Draw();

    }

  }

}
