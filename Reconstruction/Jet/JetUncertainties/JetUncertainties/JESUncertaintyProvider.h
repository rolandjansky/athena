/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///***********************************************************************
///
/// JESUncertaintyProvider
/// Authors: C. Doglioni, D. Gillberg, P.O. DeViveiros, S. Schramm, J. Taenzer
/// PROOF-compatibility: B. Butler
///
///***********************************************************************
#ifndef _JESUNCERTAINTYPROVIDER_
#define _JESUNCERTAINTYPROVIDER_

#include "TNamed.h"
#include "TFile.h"
#include "TString.h"
#include "TH2D.h"
#include <iostream>
#include <cmath>
#include <map>
#include "TObjArray.h"
#include "TObjString.h"
#include "TSystem.h"


using std::map;
typedef std::vector<TString> StrV;

// different kind of supported uncertainties
enum compType { uncorr_nuis, corr_nuis, mu_nuis, npv_nuis };

class JESUncertaintyProvider : public TNamed
{

 public:
  
  // Constructor
  // Input arguments:
  // 1. path to configuration file
  // 2. jet algorithm
  // 3. MC type MC11b
  JESUncertaintyProvider(TString configFile,//="InsituJES2011_AllNuisanceParameters.config",
			 TString jetAlgo="AntiKt4TopoEM", 
			 TString MCtype="MC11b", TString path="");
			 
  JESUncertaintyProvider(); //for persistification, can also be used with subsequent initJES() call.
  
  // Destructor
  ~JESUncertaintyProvider();

  // Initialize the provider - do we really need this??
  void initJES(TString configFile="InsituJES2011_AllNuisanceParameters.config",
	       TString jetAlgo="AntiKt4TopoEM", 
	       TString MCtype="MC11b");

  TFile* openInputFile(TString fn) { 
    if ( !FileExist(m_path+fn)) 
      Fatal( "JESUncertaintyProvider::openInputFile()",
	     "Cannot find file %s in %s",fn.Data(),m_path.Data() );
    TFile* fileHandle = new TFile(m_path+fn);
    if (fileHandle == NULL)
      Fatal( "JESUncertaintyProvider::openInputFile()","Found but could not open file %s in %s",fn.Data(),m_path.Data() );
    return fileHandle;
  };
  

  // Get total, default uncertainty EXCLUDING offset term 
  // (uncertainty components added in quadrature)
  double getRelUncert(double pT, double eta);
  
  // Get relative offset uncertainty only: each term separatly, and their sum in quad.
  double getRelNPVOffsetTerm(double pT, double eta, double NPV);
  double getRelMuOffsetTerm(double pT, double eta, double mu);
  double getRelOffsetUncert(double pT, double eta, double NPV, double mu);

  // Get total uncertainty (all uncertainty components added in quadrature)
  //double getRelUncert(double pT, double eta, double NPV, double mu);
  //double getAbsUncert(double pT, double eta, double NPV, double mu);
  
  // return the number of uncertainty components
  int getNUncertaintyComponents() { return m_uncertComp.size(); }

  // 
  TString getUncertaintyDescription(int iComp) { return m_compDesc.at(iComp); };
  TString getUncertaintyDescription(TString compName) { return getUncertaintyDescription(getComponentIndex(compName)); };

  // Get relative JES uncertainty from single component, by name or number
  double getRelUncertComponent(TString compName, double pT, double eta);
  double getRelUncertComponent(int iComp, double pT, double eta);

  // Set the path to the share folder
  //  void setPath(TString path) { m_path=path; };

  // Returns MC type
  TString getMCtype();

#ifdef JES_STANDALONE
  ClassDef(JESUncertaintyProvider,1);
#endif //JES_STANDALONE
  
  // Set NPV and mu reference values
  void setNPVRef(double NPVRef = 4.9) { m_NPVRef = NPVRef; }
  
  void setMuRef(double muRef = 5.4) { m_muRef = muRef; }
  
  void useGeV(bool doIt=true) { m_GeV = ( doIt ? 1.0 : 1000.0 ); }
  
  // Helper function for easier looping on nuisance parameters:
  // these are the names that should be used as components when calling getRelUncert
  // Note: only the component names that are listed in the .config file will be listed
  std::vector<TString> getComponentNames();
  TString getComponentName(int iComp);
  
  // these are the titles of the nuisance parameters (index-parallel with the vector of nuisance parameter names)
  std::vector<TString> getComponentDescriptions();
  
 protected:

  // path to the share folder
  TString m_path;

  TH2 *GetHisto(TFile *file, TString hname, TString methodName);

  bool FileExist(TString fn) { return gSystem->AccessPathName(fn)==false; };
  
  // Input arguments, config file, jet collection and MC type (e.g. MC11b, MC11c...)
  TString m_inputConfigFile, m_jetAlgo, m_MCtype, m_fileName;
  
  // vector of uncertainty components
  std::vector<TH2*> m_uncertComp;
  
  // quadratic sum of uncorrelated uncertainty components, correlated components
  TH2 *m_uncert, *m_corrUncert;
  
  // The NPV and mu uncertainty
  TH2 *m_muUncert, *m_npvUncert;
  
  StrV m_compNames, m_compDesc;
  map<TString,int> m_compIndex;
  std::vector<compType> m_compType;
  //  map<TString,int> m_compIndex;
  
  // For standalone compatibility
  double m_GeV;
  
  // Initialization flags
  bool m_isInit;
  
  //NPV and mu (initialised in constructor to 4.9 and 5.4 respectively, as in MC11a_valid averages)
  double m_NPVRef, m_muRef;
  
  //calibration and jet type
  TString m_calib;

  //helper function to help those doing pseudoexperiments
  //this is the most finely binned histogram (log binning for pT and binned in eta as well)
  TH2 *MakeEmptyPtEtaHisto(TString hname);

  TFile *m_inputFile;
  
 private :

  TH2 *MergeSubcomponents(StrV compVec, TString name, TString suffix, TFile *file, bool isCorrelated);

  // The uncorrelated components added in quadrature
  double getRelUncertUncorr(double pT, double eta);
  // the correlated components
  double getRelUncertBias(double pT, double eta);

  int getComponentIndex(TString compName);

  double readPtEtaHisto(TH2 *h, double pT, double eta);

  void Add(TH2 *in, TH2 *sum);
  void AddInQuad(TH2 *in, TH2 *sum);
  bool checkBinning(const TH2 *in, const TH2 *sum);
  StrV Vectorize(TString str, TString sep=" ");

  //helpers to test which component we're dealing with (for binning)
  bool isIntercalibrationComponent (const TString & component);
  bool isInSituComponent (const TString & component);
  bool isNonClosureComponent (const TString & component);
  bool isPileupComponent (const TString & component);
  
};

#endif
