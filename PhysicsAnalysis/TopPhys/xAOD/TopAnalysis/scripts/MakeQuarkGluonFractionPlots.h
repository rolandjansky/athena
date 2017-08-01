/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MAKEQUARKGLUONFRACTIONPLOTS_H
#define MAKEQUARKGLUONFRACTIONPLOTS_H

#include <iostream>
#include <string>
#include <fstream> 
#include <sstream> 
#include <stdlib.h> 
#include <vector>
#include "TStyle.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TKey.h"
#include "TH2D.h"
#include "TPostScript.h"
#include <TError.h>
#include "TROOT.h"
#include "TClass.h"

using namespace std;

// ===============================================================================
//   Macro to compute the gluon initiated jets fraction in your sample/selection
//   Required input from AnalysisTop: JetFlavorPlots directory
//  ===============================================================================

class MakeQuarkGluonFractionPlots {

 public:
  
  //function that reads the configuration file and makes the gluon fraction plots
  MakeQuarkGluonFractionPlots(std::string configfile);
  
  //function to read the configuration file
  void readConfigFile(std::string configfile);

  //function to check if the file exists
  bool checkFile(std::string filename);
  
  //function to store the histograms that are in a certain directory in a vector
  std::vector<TH2D*> vec2Dhist( std::string filepath, std::string channel, std::string prehistname);
  
  // function to draw histograms from a vector
  void drawhistos(std::vector<TH2D*> histos, std::vector<TH2D*> histosfraction, std::string psfilename, TFile* hitosrootfile);

  //function to add another histogram in the same ps file
  void addhistos( std::vector<TH2D*> histo, std::string psfilename, TFile* hitosrootfile);
    
  // function to extract the JetCollectionName
  std::string extractJetCollectionName(std::string histoname);
  
  std::string extractJetNumber(std::string histoname);
  
  // function to compute the gluon fraction
  std::vector<TH2D*> computeQuarkGluonFraction (std::vector<TH2D*> inputhistos, std::string quarkflavour, std::string prehistname);
  
   // function to create the output file
  //void createOutputFile(std::string filename, std::vector<TH2D*> histos, std::vector<TH2*> histosUnc );
  void createOutputFile(std::string filename,std::vector<TH2D*> histos, std::vector<TH2D*> histosUnc );
  
  
  //Subtract
  std::vector<TH2D*> subtracthist (std::vector<std::vector<TH2D*>> systhistos, double diff, std::string prehistname);
  
  // function to compute the systematic uncertainty
  std::vector<TH2D*> computeQuarkGluonFractionUnc (vector<vector<TH2D*>> systhisto, std::string channel);
  
  
 private:
  // path of the input file
  std::string m_inputfile;
  
  // QuarkGluon fraction can be calculated respect to different quark flavours
  // light: computed only talking into account light quarks (gluons/light-quarks)
  // lightC default): computed only talking into account light quarks  (gluons/light-quarks+c-quarks)
  // lightCB: computed only talking into account light quarks  (gluons/light-quarks+c-quarks+b-quarks)
  std::string m_doGluVsFlavour;
  
  // systematic generator samples
  std::vector<std::string> m_systGenerator; // it has to be converted in a vector
  
  // difference taken to calculate the systematic uncertainty
  double m_systDiffGenerator;
  
  // systematic fragmentation samples
  std::vector<std::string> m_systFragmentation;
  
  // difference taken to calculate the systematic uncertainty
  double m_systDiffFragmentation;
  
  // systematic radiation samples
  std::vector<std::string> m_systRadiation;

  // difference taken to calculate the systematic uncertainty
  double m_systDiffRadiation;

  // channel:name of the folder where the histograms has been stored: ee, mumu, emu, mujets, ejets... 
  std::vector<std::string> m_channel;

  // to get the flavour composition based on the histograms in JetFlavor_Loose sub-directory 
  bool m_doLoose;
  
  //compute the gluon fraction versus jet multiplicity
  bool m_detailedConfig;
  
  // name of the output file
  std::string m_OutputFile;

  // store plots in PS files
  bool m_doPS;


};


#endif
