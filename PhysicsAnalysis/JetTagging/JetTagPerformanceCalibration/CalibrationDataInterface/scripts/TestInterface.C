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
// #include ".h"



void TestInterface()
{

  gStyle -> SetPalette(1);
 
  
  // initialize:

  Analysis::CalibrationDataInterfaceROOT *m_btagCalib = 0;
  Analysis::CalibrationDataVariables m_variables;
  m_variables.jetAuthor = "AntiKt4Topo";
  Analysis::Uncertainty uncertainty = Analysis::Total;

  /*
  std::string m_TaggerToUse = "SV0";
  std::string OperationPoint = "5_85";
  */

  /*
  std::string m_TaggerToUse = "JetProb";
  std::string OperationPoint = "3_25";
  std::string OperationPoint = "2_05";
  std::string OperationPoint = "1_40";
  std::string OperationPoint = "0_60";
  */

  /*
  std::string m_TaggerToUse = "JetFitterCOMBNN";
  std::string OperationPoint = "2_00";
  std::string OperationPoint = "0_35";
  std::string OperationPoint = "-1_25";
  */

  std::string m_TaggerToUse = "SV1IP3D";
  //  std::string OperationPoint = "4_50";
  //  std::string OperationPoint = "1_55";
  std::string OperationPoint = "-0_85";


  // btag Calib interface
  cout << "Initialising btag calib with " << m_TaggerToUse.c_str() << endl;
  m_btagCalib = new Analysis::CalibrationDataInterfaceROOT(m_TaggerToUse, "BTagCalibration.env");

  int m_Debug =  1;




  // in the loop

  const int maxNpt = 120;
  double pt[maxNpt];
  double ptmin = 10;
  double ptmax = 250;


  double hackpt = 510.e3;
  double hacketa = 0.0990945;
  bool hack = false;
  //   bool hack = true;
  /*
   //    hack to one single pT,eta as desired:

   //  hackpt = 52.4467e3;   hacketa = 0.185569;
   */

  double ptstep = (ptmax-ptmin)/maxNpt;
  for (int i = 0; i < maxNpt; ++i)
    pt[i] = ptmin + i*ptstep;

  const int maxNeta = 54;
  double eta[maxNeta];
  double etamin = -2.7;
  double etamax = 2.7;

  // make histos

  std::vector<TH2D*> eff_h;
  std::vector<TH2D*> effSF_h;
  std::vector<TH2D*> dataeff_h;
  std::vector<TH2D*> dataeffSF_h;

  std::vector<TH2D*> effErr_h;
  std::vector<TH2D*> effSFErr_h;
  std::vector<TH2D*> dataeffErr_h;
  std::vector<TH2D*> dataeffSFErr_h;

  std::vector<TString> tags;
  tags.push_back("B");
  tags.push_back("C");
  tags.push_back("Light");

  for (int i = 0; i < tags.size(); ++i) {
    TString name = "eff_" + tags[i];
    TString title = name;
    eff_h.push_back(new TH2D(name, title, maxNeta, etamin, etamax, maxNpt, ptmin, ptmax));
  }


  double etastep = (etamax-etamin)/maxNeta;
  for (int j = 0; j < maxNeta; ++j)
    eta[j] = etamin + j*etastep;
  
  for (int i = 0; i < maxNpt; ++i) {
    for (int j = 0; j < maxNeta; ++j) {

      //  m_variables.jetPt = m_jets[ijet].Pt() / GeV;
      //  m_variables.jetEta = m_jets[ijet].Eta();
      m_variables.jetPt = pt[i]*1000.;
      m_variables.jetEta = eta[j];

      if (hack) {
	m_variables.jetPt = hackpt;
	m_variables.jetEta = hacketa;
      }


   
      if (m_Debug > 0) 
	cout << " === btag Calib ===" << endl;
   
      std::string jetlabel = "B"; // "B", "C", "Light" - depending on your jet flavour
      // this is usually the needed output in order to weight tagged jets
      std::pair<double,double> SFerr = m_btagCalib -> getScaleFactor(m_variables, jetlabel, OperationPoint, uncertainty);
      // additional functionalities:
      std::pair<double,double> MCeff = m_btagCalib -> getMCEfficiency(m_variables, jetlabel, OperationPoint, uncertainty);
      std::pair<double,double> Dataeff = m_btagCalib -> getEfficiency(m_variables, jetlabel, OperationPoint, uncertainty);
      // this is needed for weighting untagged jets:
      std::pair<double,double> InefSFerr = m_btagCalib -> getInefficiencyScaleFactor(m_variables, jetlabel, OperationPoint, uncertainty);
      // additional functionalities:
      std::pair<double,double> MCineff = m_btagCalib -> getMCInefficiency(m_variables, jetlabel, OperationPoint, uncertainty);
      std::pair<double,double> Dataineff = m_btagCalib -> getInefficiency(m_variables, jetlabel, OperationPoint, uncertainty);

      eff_h[0] -> Fill(eta[j]+etastep/2., pt[i]+ptstep/2., MCeff.first);

      // test printout:
      if (m_Debug > 0) {
	cout << "=======================================================================" << endl;
	cout << "btag Calib" << " Pt: " << m_variables.jetPt
	     << " Eta: " << m_variables.jetEta
	     << endl;
	cout << "btag Calib SF,err      : " << SFerr.first << "," << SFerr.second << endl;
	cout << "btag Calib MCeff,err   : " << MCeff.first << "," << MCeff.second << endl;
	cout << "btag Calib Dataeff,err : " << Dataeff.first << "," << Dataeff.second << endl;

	cout << "btag Calib Ineff. SF,err    : " << InefSFerr.first << "," << InefSFerr.second << endl;
	cout << "btag Calib MCIneff,err      : " << MCineff.first << "," << MCineff.second << endl;
	cout << "btag Calib DataIneff,err    : " << Dataineff.first << "," << Dataineff.second << endl;
      }

    } // eta
  } // pt

  TCanvas *can  = new TCanvas();
  can -> cd();
  eff_h[0] -> Draw("colz");

}
