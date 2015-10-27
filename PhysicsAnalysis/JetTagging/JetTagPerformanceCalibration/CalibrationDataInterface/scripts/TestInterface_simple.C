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
    [] .L TestInterface_simple.C+

*/

#include "CalibrationDataInterfaceBase.h"
#include "CalibrationDataInterfaceROOT.h"

#include <iostream>
#include <string>
#include <utility>

using std::cout;
using std::cerr;
using std::endl;


#include "TSystem.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TF2.h"
#include "TF1.h"
// #include ".h"

//#include 

void TestInterface_simple()
{
  // gSystem -> Load("libBTagCalib.so");

  
  // initialize:

  Analysis::CalibrationDataInterfaceROOT *m_btagCalib = 0;
  Analysis::CalibrationDataVariables m_variables;
  std::string m_TaggerToUse = "SV0";

  // btag Calib interface
  cout << "Initialising btag calib with " << m_TaggerToUse.c_str() << endl;
  m_btagCalib = new Analysis::CalibrationDataInterfaceROOT(m_TaggerToUse, "BTagCalibration.env");

  int m_Debug =  1;


  // in the loop

  //  m_variables.jetPt = m_jets[ijet].Pt() / GeV;
  //  m_variables.jetEta = m_jets[ijet].Eta();
  m_variables.jetPt = 50.e3;
  m_variables.jetEta = -1.1;
  m_variables.jetAuthor = "AntiKt4Topo";

  if (m_Debug > 0) 
    cout << " === btag Calib ===" << endl;
  Analysis::Uncertainty uncertainty = Analysis::Total;
  std::string OperationPoint = "5_85";
  std::string jetlabel = "B"; // "C", "Light" - depending on your jet flavour
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

  // test printout:
  if (m_Debug > 0) {
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

}


  

