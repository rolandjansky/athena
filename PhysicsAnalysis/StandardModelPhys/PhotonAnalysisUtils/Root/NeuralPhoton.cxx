/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <cstdlib>
#include <vector>
#include <sys/stat.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "math.h"

#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TStopwatch.h"
#include "TSystem.h"

#include "TMVA/Tools.h"
#include "TMVA/Reader.h"
#include "TMVA/MethodCuts.h"

#include "PhotonAnalysisUtils/NeuralPhoton.h"

using namespace std;
using namespace TMVA;

NeuralPhoton::NeuralPhoton (string f, int netNumber, int cutNumber) :
  tmvaFile(f),
  netName("fudge99_MLP_N3"),
  cutSel(cutNumber),
  netSel(netNumber)
{
  
  if (tmvaFile == "") 
    tmvaFile = string(gSystem->ExpandPathName("$ROOTCOREDIR/data/PhotonAnalysisUtils/NeuralPhoton_TMVA"));
  
  SelectNetwork();
  cout << "Initializing TMVA " << netName << " from: " << tmvaFile << endl;

  LoadPtsFromFile();
  LoadCutsFromFile(cutSel);
  map<string, bool> vars = LoadVariablesFromFile();
  
  reader = new TMVA::Reader("Silent");

  if (vars["fside"])  reader->AddVariable("fside", &fside);
  if (vars["w1"])     reader->AddVariable("w1", &w1);
  if (vars["wtot"])   reader->AddVariable("wtot", &wtot);
  if (vars["deltae"]) reader->AddVariable("deltae", &deltae);
  if (vars["eratio"]) reader->AddVariable("eratio", &eratio);
  if (vars["rphi"])   reader->AddVariable("rphi", &rphi);
  if (vars["rhad"])   reader->AddVariable("rhad", &rhad);
  if (vars["reta"])   reader->AddVariable("reta", &reta);
  if (vars["weta2"])  reader->AddVariable("weta2", &weta2);
  if (vars["f1"])     reader->AddVariable("f1", &f1);

  reader->AddSpectator("pt", &ptSp);
  reader->AddSpectator("etas2", &etaSp);
  if (netName != "lowPt_MLP_N3_N7")
    reader->AddSpectator("convFlag", &convFlagSp);
  reader->AddSpectator("tight", &tightSp);

  for (int e = 1; e <= 8; e++) {
    if (e == 5) continue;
    for (unsigned int p = 1; p < NP_pt.size(); p++) {
      for (unsigned int c = 0; c < m_Nconv; c++) {

        string net_id = GetNetworkIdentifier(NP_eta[e], NP_pt[p], c);
        string fileName = Form("%s/%s/weights/%s.weights.xml", 
          tmvaFile.c_str(), netName.c_str(), net_id.c_str());

        struct stat stFileInfo;
        int intStat = stat(fileName.c_str(), & stFileInfo);
        if (intStat != 0) {
          cerr << "NEURAL PHOTON could not find WEIGHTS -- ALL PHOTONS WILL FAIL!!" << endl;
          return;
        }

        m_bookedMethods.push_back(reader->BookMVA(net_id.c_str(), fileName.c_str()));
      }
    }
  }

}


NeuralPhoton::~NeuralPhoton () {

  for(unsigned int i=0; i<m_bookedMethods.size(); i++)
    delete m_bookedMethods[i];

  delete reader;

  return;

}

void NeuralPhoton::SelectNetwork() {

  if (netSel == 1)      netName = "fudge99_MLP_N3";
  else if (netSel == 2) netName = "nn12_MLP_N3";
  else if (netSel == 3) netName = "fudge9_MLP_N3";
  else if (netSel == 4) netName = "lowPt_MLP_N3_N7";
  else 
  {
    cerr << "NEURAL PHOTON: Invalid network number: " << netSel << endl;
    return;
  }
  
  // number of bins of conversion (0,1 or 0,1,2-tracks)
  m_Nconv = (netName != "lowPt_MLP_N3_N7") ? 3 : 2 ;
  
  return;

}

void NeuralPhoton::LoadCutsFromFile(int cs) {

  if (cs >= 0) cutSel = cs;

  string fileName = tmvaFile + "/" + netName;
  if      (cutSel == 1) fileName += "/const_eff_dict_NP.dat";
  else if (cutSel == 2) fileName += "/cuts_tune2_NP.dat";
  else if (cutSel == 3) fileName += "/cuts_tune3_NP.dat";
  else if (cutSel == 4) fileName += "/cuts_tune4_NP.dat";
  else    fileName += "/const_rej_dict_NP.dat";
  
  cout << "Loading Cuts from: " << fileName << endl;

  for (int e = 0; e < 8; e++) {
    for (int p = 0; p < 6; p++) {
      for (unsigned int c = 0; c < m_Nconv; c++) {
        disc_constr[e][p][c] = 9999.;
      }
    }
  }

  struct stat stFileInfo;
  int intStat = stat(fileName.c_str(), & stFileInfo);
  if (intStat != 0) {
    cerr << "NEURAL PHOTON could not find DISCRIMINANT CUTS -- ALL PHOTONS WILL FAIL!!" << endl;
    return;
  }

  ifstream discrFile;
  discrFile.open(fileName.c_str());
  string line;
  
  unsigned int e, p, c;
  float cutVal;
  
  while (!discrFile.eof()) {
    getline(discrFile, line);
    istringstream parse(line);
    parse >> e >> p >> c >> cutVal;
    disc_constr[e][p][c] = cutVal;
  }

  discrFile.close();

  return;

}

void NeuralPhoton::LoadPtsFromFile() {

  string fileName = tmvaFile + "/" + netName + "/NP_pt.list";

  struct stat stFileInfo;
  int intStat = stat(fileName.c_str(), & stFileInfo);
  if (intStat != 0) {
    cerr << "NEURAL PHOTON could not find PT LIST -- ALL PHOTONS WILL FAIL!!" << endl;
    return;
  }

  ifstream ptFile;
  ptFile.open(fileName.c_str());
  string line;

  while (ptFile.is_open() && !ptFile.eof()) {
    getline(ptFile, line);
    if (line != "") NP_pt.push_back(1000.*atoi(line.c_str()));
  }

  ptFile.close();

}

map<string, bool> NeuralPhoton::LoadVariablesFromFile() {

  string fileName = tmvaFile + "/" + netName + "/NP_vars.list";

  map<string, bool> varMap;

  varMap["fside"] = false;
  varMap["w1"] = false;
  varMap["wtot"] = false;
  varMap["deltae"] = false;
  varMap["eratio"] = false;
  varMap["rphi"] = false;
  varMap["reta"] = false;
  varMap["weta2"] = false;
  varMap["rhad"] = false;
  varMap["f1"] = false;

  struct stat stFileInfo;
  int intStat = stat(fileName.c_str(), & stFileInfo);
  if (intStat != 0) {
    cerr << "NEURAL PHOTON could not find VARIABLE LIST -- ALL PHOTONS WILL FAIL!!" << endl;
    return varMap;
  }

  ifstream varFile;
  varFile.open(fileName.c_str());
  string line;

  while (varFile.is_open() && !varFile.eof()) {
    getline(varFile, line);
    varMap[line] = true;
  }

  varFile.close();

  return varMap;

}

string NeuralPhoton::GetNetworkIdentifier(float eta, float pt, int convFlag) {

  int e = -1, p = -1, c = -1;

  for (unsigned int i = 0; i < NP_pt.size()-1; i++)
    if (NP_pt[i] < pt && pt <= NP_pt[i+1])
      p = i + 1;

  for (int i = 0; i < 8; i++)
    if (i != 4 && NP_eta[i] < fabs(eta) && fabs(eta) <= NP_eta[i+1])
      e = i + 1;
  
  c = (m_Nconv == 3) ? convFlag % 10 : (convFlag % 10) != 0;

  if (e < 0 || p < 0) return "";

  string id = Form("%s.pt%d_eta%4.2f_conv%d", netName.c_str(), (int) (NP_pt[p]/1000.), NP_eta[e], c);

  return id;

}

void NeuralPhoton::SetPhotonVars(float eta, float rhadX, float rhad1X, float retaX, float weta2X, float rphiX, float w1X, float wtotX, float deltaeX, float eratioX, float fsideX, float f1X, float e277X) {

  rhad = (0.8 <= fabs(eta) && fabs(eta) <= 1.37) ? rhadX : rhad1X;
  reta = retaX;
  weta2 = weta2X;
  rphi = rphiX;
  w1 = w1X;
  wtot = wtotX;
  deltae = deltaeX;
  eratio = eratioX;
  fside = fsideX;
  f1 = f1X;
  e277 = e277X;

  return;

}

float NeuralPhoton::GetNetworkResponse(float eta, float pt, int convFlag, float rhadX, float rhad1X, float retaX, float weta2X, float rphiX, float w1X, float wtotX, float deltaeX, float eratioX, float fsideX, float f1X, float e277X) {

  SetPhotonVars(eta, rhadX, rhad1X, retaX, weta2X, rphiX, w1X, wtotX, deltaeX, eratioX, fsideX, f1X, e277X);
  
  return NeuralPhoton::GetNetworkResponse(eta, pt, convFlag);

}

float NeuralPhoton::GetNetworkResponse(float eta, float pt, int convFlag) {
  
  string net_id = GetNetworkIdentifier(eta, pt, convFlag);

  if (net_id != "") return reader->EvaluateMVA(net_id.c_str());
  else return 0.;

}


bool NeuralPhoton::GetPassNetwork(float eta, float pt, int convFlag, float rhadX, float rhad1X, float retaX, float weta2X, float rphiX, float w1X, float wtotX, float deltaeX, float eratioX, float fsideX, float f1X, float e277X, float* netResponse) { 

  SetPhotonVars(eta, rhadX, rhad1X, retaX, weta2X, rphiX, w1X, wtotX, deltaeX, eratioX, fsideX, f1X, e277X);

  return GetPassNetwork(eta, pt, convFlag, netResponse);

}

bool NeuralPhoton::GetPassNetwork(float eta, float pt, int convFlag, float* netResponse) {

  float resp = GetNetworkResponse(eta, pt, convFlag);
  float cut = GetNetworkDiscriminantCut(eta, pt, convFlag);

  if(netResponse) *netResponse = resp;

  return (resp >= cut);

}

float NeuralPhoton::GetNetworkDiscriminantCut(float eta, float pt, int convFlag) {

  int e = -1, p = -1, c = -1;

  for (unsigned int i = 0; i < NP_pt.size()-1; i++)
    if (NP_pt[i] < pt && pt <= NP_pt[i+1])
      p = i;

  for (int i = 0; i < 8; i++)
    if (i != 4 && NP_eta[i] < fabs(eta) && fabs(eta) <= NP_eta[i+1])
      e = i;
  
  c = (m_Nconv == 3) ? convFlag % 10 : (convFlag % 10) != 0;

  if (e < 0) {
    cerr << "There's no neural net for this photon: eta = " << eta << "." << endl;
    return 1;
  } else if (p < 0) {
    cerr << "There's no neural net for this photon: pt = " << pt/1000. << " GeV." << endl;
    return 1;
  } else if (c < 0) {
    cerr << "There's no neural net for this photon: strange conversion value." << endl;
    return 1;
  }

  return disc_constr[e][p][c];

}


