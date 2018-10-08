/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//Author: Regina Moles Valls - <regina.moles.valls@cern.ch>
//R21 migration: Matteo Scornajenghi - <matteo.scornajenghi@cern.ch>

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
#include "TXMLEngine.h"
#include <map>
#include <fstream>

// ===============================================================================
//   Macro to compute the gluon initiated jets fraction in your sample/selection
//  ===============================================================================
struct config_NominalInput{
   config_NominalInput(): path(""),folder("JetFlavorPlots"){}
   std::string path;
   std::string folder;
};
struct config_1PointSyst{
   config_1PointSyst(): name(""),central(""),variation(""),scalefactor(1),folder_c("JetFlavorPlots"),folder_v("JetFlavorPlots"){}
   std::string name;
   std::string central;
   std::string variation;
   float scalefactor;
   std::string folder_c;
   std::string folder_v;
};
struct config_2PointSyst{
   config_2PointSyst(): name(""),central(""),variation_up(""),variation_down(""),scalefactor(1),folder_c("JetFlavorPlots"),folder_up("JetFlavorPlots"),folder_down("JetFlavorPlots"){}
   std::string name;
   std::string central;
   std::string variation_up;
   std::string variation_down;
   float scalefactor;
   std::string folder_c;
   std::string folder_up;
   std::string folder_down;
};

class MakeQuarkGluonFractionPlots {

 public:
//new functions
  void printXMLConfigFile();
  void readXMLConfigFile(std::string configfile);
  void readXMLNode(TXMLEngine xml, XMLNodePointer_t node);
  bool decodeBool(TXMLEngine xml, XMLAttrPointer_t attr, std::string match, bool &value);
  bool decodeString(TXMLEngine xml, XMLAttrPointer_t attr, std::string match, std::string &value);
  bool decodeFloat(TXMLEngine xml, XMLAttrPointer_t attr, std::string match, float &value);
  void AbortXMLDecode(std::string value);
//end new
  
  //function that reads the configuration file and makes the gluon fraction plots
  MakeQuarkGluonFractionPlots(std::string configfile);
  
  //function to check if the file exists
  void checkFile(std::string filename);
  
  //function to draw histograms from a map
  void drawhistos(std::map<std::string, TH2D*> histos, std::string psfilename);
  //function to draw histograms from a vector
  void drawhistos(std::vector<TH2D*> histos, std::string psfilename);

  //function to extract the JetCollectionName
  std::string extractJetCollectionName(std::string histoname);

  //function to extract the JetNumber
  std::string extractJetNumber(std::string histoname);
  
  //function to compute the gluon fraction
  std::vector<TH2D*> computeQuarkGluonFraction (std::map< std::string,TH2D* > inputhistos, std::string quarkflavour, std::string prename);
  
  //function to create the output file
  void createOutputFile(std::string filename,std::vector<TH2D*> histos, std::vector<TH2D*> histosUnc );
  
  //Subtract
  std::vector<TH2D*> getDelta (std::vector<TH2D*> h_nom, std::vector<TH2D*> h_var, double scale, std::string prehistname);
  
  //function to compute the total systematic uncertainty
  std::vector<TH2D*> evaluateQGFUncertaity(std::vector<std::vector<TH2D*> > f_1P, std::vector<std::vector<TH2D*> > f_2PUp, std::vector<std::vector<TH2D*> > f_2PDown, std::string channel);
  

  
  
 private:
  // QuarkGluon fraction can be calculated respect to different quark flavours
  // light: computed only talking into account light quarks (gluons/light-quarks)
  // lightC default): computed only talking into account light quarks  (gluons/light-quarks+c-quarks)
  // lightCB: computed only talking into account light quarks  (gluons/light-quarks+c-quarks+b-quarks)
  std::string m_doGluVsFlavour;
  
  // channel:name of the folder where the histograms has been stored: ee, mumu, emu, mujets, ejets...
  std::vector<std::string> m_channel;

  //compute the gluon fraction versus jet multiplicity
  bool m_detailedConfig;
  
  // name of the output file
  std::string m_OutputFile;

  // store plots in PS files
  bool m_doPS;

  config_NominalInput m_configNominal; 
  std::vector<config_1PointSyst> m_config1PointVec;
  std::vector<config_2PointSyst> m_config2PointVec;

  bool m_mergechannels;

  void DumpToMap(std::map<std::string, TH2D*> &h_map, std::string filename, std::string channel, std::string folder, std::string keyname, bool createMap);
  void DumpFileToMap(std::map<std::string, TH2D*> &h_map, std::string filename, std::string channel, std::string folder, std::string keyname, bool createMap);
  void CreateQGFFile(std::string prename, std::map<std::string, TH2D*> h_input,std::vector< std::map<std::string, TH2D*> > h_input_1P,std::vector< std::map<std::string, TH2D*> > h_input_1PVar,std::vector< std::map<std::string, TH2D*> > h_input_2P,std::vector< std::map<std::string, TH2D*> > h_input_2PUp,std::vector< std::map<std::string, TH2D*> > h_input_2PDown);

};


#endif

