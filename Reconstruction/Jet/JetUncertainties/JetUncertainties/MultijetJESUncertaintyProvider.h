/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _MULTIJETJESUNCERTAINTYPROVIDER_
#define _MULTIJETJESUNCERTAINTYPROVIDER_

#include "JetUncertainties/JESUncertaintyProvider.h"
#include <TString.h>

class MultijetJESUncertaintyProvider : public JESUncertaintyProvider{

 public:
  
  // Constructor
  // Input arguments:
  // 1. path to multijet configuration file
  // 2. path to JES conf file, 3. jet algorithm, 4. MCtype
  MultijetJESUncertaintyProvider(TString multijetConfig,//="Multijet.config", 
				 TString configFile="JESInputs.config", 
				 TString jetAlgo="AntiKt4TopoEM", 
				 TString MCtype="MC11b", TString path="");
  MultijetJESUncertaintyProvider(); //for persistification, can also be used with subsequent initJES() and initMultijet() calls.
  ~MultijetJESUncertaintyProvider();

  // Initialize the provider
  void initMultijet(TString multijetConfig="Multijet.config");

  // The various uncertainty components: call these functions to get separate components
  double getRelClosebyUncert(double pT, double dRmin);
  
  //FIXME: isUp argument will be deprecated 
  double getRelFlavorCompUncert(double pT, double eta, bool isUp, unsigned int nJets = 0);
  double getRelFlavorResponseUncert(double pT, double eta, unsigned int nJets = 0);
  double getRelBJESUncert(double pT, double eta);//only call this for b-jets!
  
  // addition in quadrature of the two flavor uncertainties
  double getRelFlavorUncert(double pT, double Eta, bool isUp, unsigned int nJets = 0);
  
  // total uncertainties above, including the in-situ one from the JES uncertainty provider
  double getRelUncert(double pT, double Eta, double dRmin, bool isUp, bool isBJet, unsigned int nJets = 0);
  double getRelUncert(double pT, double Eta, double dRmin, bool isUp, double NPV, double mu, bool isBJet, unsigned int nJets = 0);

  //wrappers for functions above, positive and negative separately
  double getRelPosUncert(double pT, double eta, double dRmin, bool isBJet, unsigned int nJets = 0) 
  { return getRelUncert(pT,eta,dRmin,true,isBJet,nJets); };
  
  double getRelNegUncert(double pT, double eta, double dRmin, bool isBJet, unsigned int nJets = 0)
  { return getRelUncert(pT,eta,dRmin,false, isBJet,nJets); };

  double getRelPosUncert(double pT, double eta, double dRmin, double NPV, double mu, bool isBJet, unsigned int nJets = 0)
  { return getRelUncert(pT,eta,dRmin,true,NPV,mu,isBJet,nJets); };

  double getRelNegUncert(double pT, double eta, double dRmin, double NPV, double mu, bool isBJet, unsigned int nJets = 0)
  { return getRelUncert(pT,eta,dRmin,false,NPV,mu,isBJet,nJets); };

 
#ifdef JES_STANDALONE
  ClassDef(MultijetJESUncertaintyProvider,1);
#endif //JES_STANDALONE
  
 private:
  
  TString m_multijetConfig;

  //Pointers to the flavor composition histograms 
  TH2D*  m_flavorGlu;
  TH2D*  m_flavorLight;

  //Pointer to the deltaR histogram
  TH2D*  m_deltaR;
  
  //Pointer to the flavor response histogram
  TH2D*  m_gluonResponseDifference;

  //Pointer to the bJES histograms
  TH2D*  m_bJESResponse; 
  
  //Pointers to the sample related histograms
  //Pointer to the gluon fraction histogram
  std::vector<TH2D*>  m_gluonFraction; 
  //Pointer to the gluon fraction error histogram
  std::vector<TH2D*>  m_gluonFractionError; 
  
  bool m_multiIsInit;

  double m_relGluonResponseUncert;

  //maximum number of jets in nJet-based histogram
  unsigned int m_maxNumJets;
  
  //Helper functions to access histogram content
  
  // get the gluon and light quark fractions - ignore heavy flavor components for now
  double gluonFraction(double pt, double eta, unsigned int nJets = 0);
  double lightQuarkFraction(double pt, double eta, unsigned int nJets = 0) {
    return 1.0-gluonFraction(pt,eta,nJets);
  };
  double gluonFractionError(double pt, double eta, unsigned int nJets = 0);
  
  // The light quark, and gluon response: R(light quark)/R(inclusive), where inclusive is the MC11b JX samplesB
  double lightQuarkResponse(double pt, double eta);
  double gluonResponse(double pt, double eta);
  
  //The gluon response difference in Pythia vs Herwig++
  double gluonResponseDifference(double pt, double eta);
  
  //The bJES response from various components
  double bJESResponse(double pt, double eta);
  
  // Helper funtion to read off a TH2D
  double getValue(const TH2 *h, double pt, double eta);

};

#endif
