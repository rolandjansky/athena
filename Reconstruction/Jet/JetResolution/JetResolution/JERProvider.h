/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///***************************************************
/// Class: JERProvider
/// Author: Sabrina Sacerdoti <Sabrina.Sacerdoti@cern.ch>
/// Plots by: Sabrina Sacerdoti <Sabrina.Sacerdoti@cern.ch>
///
/// Provide the JER and its uncertainty.
/// Created: Jan/19/2011
/// Version (see cmt/version.cmt)
///
/// The uncertainty corresponds to the data-mc difference in the in situ method
///
/// Based on ATL-COM-PHYS-2011-240
///
/// 1) Link the final library, located in: JERProvider/StandAlone/libJERProvider.so
/// 2) Create an instance, ie:
///    JERProvider myJER("AntiKt6TopoJES","Truth","JERProviderPlots_2012.root");
/// 3) if using 7 TeV data: myJER.is7TeV(true);
/// Notice that AFII and Alt2 are not available for 7 TeV
/// 4) Initialize the instance: myJER.init();
/// 5) If you wish to have all values in GeV: myJER.useGeV(true);
/// 6) Call myJER.getRelResolutionMC(pt,eta) to get the resolution in Monte Carlo 
/// 7) Call myJER.getResolutionUncert(pt,eta) to get its uncertainty
///     or
///   myJER.getResolutionUncert_Alt2(pt,eta)
/// If using AFII Monte Carlo, use 
///   myJER.getResolutionUncert_AFII(pt,eta)  or  myJER.getResolutionUncert_Alt2_AFII(pt,eta)
///***************************************************

#ifndef _JERPROVIDER_
#define _JERPROVIDER_

#include "TNamed.h"
#include "TFile.h"
#include "TF1.h"
#include "TGraphErrors.h"
#include <TRandom.h>
#include <iostream>
#include <cmath>
#include <map>
#include <TSystem.h>
#include <TString.h>
#include <cstdlib>
#include <TAxis.h>

using std::cout;
using std::cerr;
using std::endl;
using std::map;

class JERProvider : public TNamed
{
  enum JETALGO { AKt4EM, AKt4LC, AKt6EM, AKt6LC };
  
 public: 

  // Constructor, destructor
  JERProvider() { }
  JERProvider(std::string CollectionName, std::string MethodName = "Truth", 
	      std::string FileName="$ROOTCOREBIN/data/JetResolution/JERProviderPlots_2012.root");
  virtual ~JERProvider();
  
  #ifdef JER_STANDALONE
  ClassDef(JERProvider,1);
  #endif

  // filename -- only allowed before initialisation
  void setFileName(std::string fileName);
  void fatal(TString msg) {
    std::cerr << "\nERROR in JERProvider:\n " << msg.Data() << std::endl;
    std::cout << "\nERROR in JERProvider:\n " << msg.Data() << std::endl;
    abort();
  }
  void warning(TString msg){std::cout << "\nWARNING in JERProvider:\n " << msg.Data() << std::endl;}
  
  //initialization
  void init();
  void useGeV(bool isGeV=true) { if (isGeV){ m_GeV = 1; m_invGeV=1;} else {m_GeV = 1000; m_invGeV = 0.001;}}
  void is7TeV(bool use7TeV=true) { if (use7TeV) m_is7TeV = true; else m_is7TeV = false;}

  // inputCollection 
  void setInputCollection(std::string CollectionName, std::string MethodName);  
  
  // Read the offset from data/MC for the pT bin considered
  double getOffset(double pt, double y);
  
  // Read the resolution from the MC parameterisation
  double getRelResolutionMC(double pt, double y);
  
  // Obtain the resolution for data (sigma_MC + offset)
  double getRelResolutionData(double pt, double y);
 
  // Obtain the resolution for AFII MC
  double getRelResolutionMC_AFII(double pt, double y);

  // Access to the insitu measurements and their errors
  double getInsituRelResolutionMC(double pt, double eta);
  double getInsituRelResolutionData(double pt, double eta);
  double getInsituUncertData(double pt, double eta);
  double getInsituUncertMC(double pt, double eta);
  double getInsituUncert(double pt, double eta);
  
  // methods for 7TeV
  // get dta-mc in situ difference and error
  double getInsituDiffDataMC(double pt, double eta);
  double getInsituDiffDataMCerror(double pt, double eta);
  // get uncertainties - 7TeV way
  double getUncertainty_7TeV(double pt, double eta);
  double getSystematics_7TeV(int etaBin);// contains parameters to get uncertainty
  
  //common function to calculate uncertainty
  double getUncertainty(double pt, double y, bool atl2, bool isAFII);

  // short versions to obtain uncertainties on FS/AFII
  double getResolutionUncert(double pt, double eta, bool alt2=false) { return getUncertainty(pt,eta,alt2,false);}
  double getResolutionUncert_AFII(double pt, double eta, bool alt2=false) { if(m_is7TeV) fatal("AFII resolution not available in 7TeV.");return getUncertainty(pt,eta,alt2,true); }
  // alternative uncertainty
  double getResolutionUncert_Alt2(double pt, double eta)      { if(m_is7TeV) fatal("Alt2 method is not recommended for 7TeV.");return getUncertainty(pt,eta,true,false); }
  double getResolutionUncert_AFII_Alt2(double pt, double eta) { if(m_is7TeV) fatal("AFII resolution not available in 7TeV.");return getUncertainty(pt,eta,true,true);  }

  // Smearing factors
  double getSmearingFactor(double reso, double unc) { return sqrt((reso+unc)*(reso+unc)-reso*reso); }

  double getSmearingFactorMC(double pt, double eta) { return getSmearingFactor(getRelResolutionMC(pt,eta),getResolutionUncert(pt,eta)); }
  double getSmearingFactorMC_AFII(double pt, double eta) { return getSmearingFactor(getRelResolutionMC_AFII(pt,eta),getResolutionUncert_AFII(pt,eta)); }

  double getSmearingFactorMC_Alt2(double pt, double eta) { return getSmearingFactor(getRelResolutionMC(pt,eta),getResolutionUncert_Alt2(pt,eta)); }
  double getSmearingFactorMC_AFII_Alt2(double pt, double eta) { return getSmearingFactor(getRelResolutionMC_AFII(pt,eta),getResolutionUncert_AFII_Alt2(pt,eta)); }

   // Get values of systematics
  double getDeltaClosure();
  double getDeltaInSitu(double pt, double y);
  double getDeltaMC();
  
  template <class T> T *pullFromFile(TString fname) {
    T *f = (T*)m_inputFile->Get(fname); 
    if (f==NULL) fatal("Cannot find function\n    "+fname+"\n  in\n    "+m_inputFile->GetName());
    f->SetName("the"+fname); return f;
  }

  int getEtaBin(double eta) {
    //  static double etaBins[7] = {0,0.8,1.2,2.1,2.8,3.6,4.5};
    //  static TAxis *etaAxis = new TAxis(6,etaBins);
    int bin=m_etaAxis->FindBin(fabs(eta)); // gives 1 for bin 0
    return (bin>6) ? 5 : bin-1; // return 0 for first bin, 5 for last or overflow
  }

#ifdef ROOTCORE
  ClassDef(JERProvider,1);
#endif

 private:
  
  static const int m_nParam = 6;
  static const int m_nY = 6;
  //  static double etaBins[7];
  TAxis *m_etaAxis;

  // strings
  std::string m_collectionName;
  std::string m_methodName;
  std::string m_fileName;
  
  //initialization flag
  bool m_isInit;
  double m_GeV;
  double m_invGeV;
  bool m_is7TeV;

  // Input file
  TFile* m_inputFile; //!

  // Pointer to the resolution
  map<int,TF1*> m_jerFunc;
  map<int,TF1*> m_jerFuncAFII;

  // Parameters and their errors
  map<int,float> m_param;
  map<int,float> m_param_AFII;

  // Pointers to the correct graphs
  // in situ resolutions
  map<int,TGraphErrors*> m_jerData;
  map<int,TGraph*> m_errorData;
  // errors of the resolutions
  map<int,TGraphErrors*> m_jerMC;
  map<int,TGraph*> m_errorMC;
  // data-mc diff in 7TeV
  map<int,TGraphErrors*> m_diffDataMC;
  map<int,TGraph*> m_errorDataMC;

  // Uncertainty from Data/MC comparison
  float m_uncert;

  // Systematics
  float m_syst;

  JETALGO m_jetAlgo;
};

#endif

