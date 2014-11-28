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
/// It is 100% correlated
//
/// Based on ATL-COM-PHYS-2011-240
///
/// Usage:https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/JetEnergyResolutionProvider2012 
/// 
/// 1) Link the final library, located in: JERProvider/StandAlone/libJERProvider.so
/// 2) Create an instance, ie:
///    JERProvider myJER("AntiKt6TopoJES","Truth","JERProviderPlots.root");
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

#include "JetResolution/JERProvider.h"
#include "JetResolution/InSituSystematics.h"
#include <TSystem.h>

JERProvider::JERProvider(std::string CollectionName, std::string MethodName, std::string FileName):
  m_collectionName(CollectionName), m_methodName(MethodName), m_fileName(FileName), m_isInit(false),m_GeV(1000),m_invGeV(0.001),m_is7TeV(false)
{
  double etaBins[7] = {0,0.8,1.2,2.1,2.8,3.6,4.5};
  m_etaAxis = new TAxis(6,etaBins);
}

void JERProvider::setFileName(std::string fileName)
{
  if(!m_isInit) m_fileName = fileName;
  else cout << "ERROR: Input file cannot be changed once JERProvider is initialized!" << endl;
}

void JERProvider::init()
{
  if(!m_isInit) {

    // Open Input File
    m_inputFile = new TFile(gSystem->ExpandPathName(m_fileName.c_str()));
    //if(m_inputFile==NULL) fatal("JER input file "+m_fileName+" could not be found.");
    if(!m_inputFile->IsOpen()) fatal("JER input file "+m_fileName+" could not be found.");
    
    // Initialzie 
    setInputCollection(m_collectionName, m_methodName);
    //cout << "JERProvider initialized for " << m_collectionName << " jets." << endl;
    m_isInit = true;

    // Close the file
    m_inputFile->Close();
    delete m_inputFile;
  }
  else cout << "JERProvider already initialized!" << endl;
}

JERProvider::~JERProvider()
{
  // free memory - probbaly need more of these
  if (m_etaAxis) delete m_etaAxis;
}

void JERProvider::setInputCollection(std::string CollectionName, std::string MethodName)
{
  TString jetAlgo(CollectionName);

  // Get the jet distance paramter
  int jetR = jetAlgo.Contains("Kt4")?4:jetAlgo.Contains("Kt6")?6:0;
  if (jetR==0) fatal("Do not have support for JER of "+CollectionName+" jets.");

  // EM+JES or LC+JES ?
  TString cal=jetAlgo.Contains("LC")?"LCTopoJES":"TopoJES";
  if (cal=="LCTopoJES") m_jetAlgo = jetR == 4 ? AKt4LC : AKt6LC; 
  else m_jetAlgo = jetR == 4 ? AKt4EM : AKt6EM;
  
  TString prefix=Form("%s_AntiKt%d%s",MethodName.c_str(),jetR,cal.Data());

  printf("= JERProvider =\n  Initating JER for AntiKt%d jets with %s+JES calibration\n",
	 jetR,jetAlgo.Contains("LC")?"LCW":"EM");
  printf("  Input file: %s\n",gSystem->ExpandPathName(m_fileName.c_str()));
  
  if (MethodName != "Truth") 
    fatal("Do not have support the "+MethodName+" JER method.");

  std::string regions[m_nY] = {"_0","_1","_2","_3","_4","_5"};
  if(!m_is7TeV){
    for(int i=0; i < m_nY; i++) {
      TString graphName = prefix + regions[i];

      m_jerFunc[i] = pullFromFile<TF1>(graphName);
      m_jerFuncAFII[i] = pullFromFile<TF1>("TruthAF2_"+CollectionName+regions[i]);

      // (only up to 2.8 due to statistics) ---> m_nY-2
      if ( i < m_nY - 2 ) {
        m_jerMC[i]   = pullFromFile<TGraphErrors>("BisectorFit_"+CollectionName+regions[i]);
        m_jerData[i] = pullFromFile<TGraphErrors>("BisectorDataFit_"+CollectionName+regions[i]);
        m_errorMC[i] = pullFromFile<TGraph>("BisectorFitUNCERT_"+CollectionName+regions[i]);
        m_errorData[i] = pullFromFile<TGraph>("BisectorDataFitUNCERT_"+CollectionName+regions[i]);
      }
    }
  }
  else{
    for(int i=0; i < m_nY; i++) {
      TString graphName = prefix + regions[i];

      m_jerFunc[i] = pullFromFile<TF1>(graphName);

      // (only up to 2.8 due to statistics) ---> m_nY-2
      if ( i < m_nY - 2 ) {
        m_diffDataMC[i] = pullFromFile<TGraphErrors>("DataMCBisector_"+CollectionName+regions[i]);
        m_errorDataMC[i] = pullFromFile<TGraph>("DataMCBisectorUNCERT_"+CollectionName+regions[i]);
      }
    }
  }
  printf("===============\n");
}




/*
 *   Access the MC fullsim and AFII truth resolutions
 *   and the "data truth" resolution as the MC truth one + data-MC insitu difference
 */

double JERProvider::getRelResolutionMC(double pt, double eta) {
  if (!m_isInit) fatal("JERProvider not initialized.");
  if (pt*cosh(eta)>10000.0*m_GeV) 
    fatal(Form("I refuse! You are giving me a jet with E=%.2f TeV. Can't be right. Check your units. OR this might be new physics!",
	       pt*cosh(eta)*m_invGeV*0.001));
  if (pt*cosh(eta)>4200.0*m_GeV) warning(Form("You have a jet with suspisiosly large energy E= %.2f TeV.",pt*cosh(eta)*m_invGeV*0.001));
  if (pt*m_invGeV < 10 ) pt=10.0*m_GeV; if (pt*m_invGeV > 2000 ) pt=2000.0*m_GeV;
  return m_jerFunc.at(getEtaBin(eta))->Eval(pt*m_invGeV);
}

double JERProvider::getRelResolutionMC_AFII(double pt, double eta) {
  if (!m_isInit) fatal("JERProvider not initialized.");
  if (m_is7TeV) fatal("AFII resolution not available in 7TeV.");
  if (pt*cosh(eta)>10000.0*m_GeV) 
    fatal(Form("I refuse! You are giving me a jet with E=%.2f TeV. Can't be right. Check your units. OR this might be new physics!",
	       pt*cosh(eta)*m_invGeV*0.001));
  if (pt*cosh(eta)>4200.0*m_GeV) warning(Form("You are using a large jet: E=%.2f TeV.",pt*cosh(eta)*m_invGeV*0.001));

  if (pt*m_invGeV < 10 ) pt=10.0*m_GeV; if (pt*m_invGeV > 2000 ) pt=2000.0*m_GeV;
  return m_jerFuncAFII.at(getEtaBin(eta))->Eval(pt*m_invGeV);
}

double JERProvider::getRelResolutionData(double pt, double eta) {
  // reso(MC fullsim truth) + reso(data in-situ) - reso(MC fullsim in-situ)
  if(m_is7TeV){
    double sigma_MC = getRelResolutionMC(pt, eta);
    double relOffset = getInsituDiffDataMC(pt,eta)/100;
    return sigma_MC + relOffset*sigma_MC;
  } 
  return getRelResolutionMC(pt, eta) + getOffset(pt, eta); 
}


/*
 *   Access the instiu resolution, it's error and the data-MC offset
 */

double JERProvider::getInsituRelResolutionData(double pt, double eta) {
  if (m_is7TeV) fatal("Not available in 7 TeV. Use getInsituDiffDataMC(pt,eta) to get in situ data-mc relative difference.");
  int etaBin = getEtaBin(eta);
  if (etaBin>3) etaBin=3; // due to lacking of stats from data/mc
  if (pt>1000*m_GeV) pt=1000*m_GeV; if (fabs(eta)>2.1 && pt>300*m_GeV) pt=300*m_GeV;
  return m_jerData[etaBin]->Eval(pt*m_invGeV);
}

double JERProvider::getInsituRelResolutionMC(double pt, double eta) {
  if (m_is7TeV) fatal("Not available in 7 TeV. Use getInsituDiffDataMC(pt,eta) to get in situ data-mc relative difference.");
  int etaBin = getEtaBin(eta);
  if (etaBin>3) etaBin=3; // due to lacking of stats from data/mc
  if (pt>1000*m_GeV) pt=1000*m_GeV; if (fabs(eta)>2.1 && pt>300*m_GeV) pt=300*m_GeV;
  return m_jerMC[etaBin]->Eval(pt*m_invGeV);
}

double JERProvider::getInsituUncertData(double pt, double eta) {
  if (m_is7TeV) fatal("Not available in 7 TeV. Use getInsituDiffDataMCerror(pt,eta) to get the error of the in situ data-mc relative difference.");
  int etaBin = getEtaBin(eta);
  if (etaBin>3) etaBin=3; // due to lacking of stats from data/mc
  if (pt>1000*m_GeV) pt=1000*m_GeV; if (fabs(eta)>2.1 && pt>300*m_GeV) pt=300*m_GeV;
  return m_errorData[etaBin]->Eval(pt*m_invGeV);
}

double JERProvider::getInsituUncertMC(double pt, double eta) {
  if (m_is7TeV) fatal("Not available in 7 TeV. Use getInsituDiffDataMCerror(pt,eta) to get the error of the in situ data-mc relative difference.");
  int etaBin = getEtaBin(eta);
  if (etaBin>3) etaBin=3; // due to lacking of stats from data/mc
  if (pt>1000*m_GeV) pt=1000*m_GeV; if (fabs(eta)>2.1 && pt>300*m_GeV) pt=300*m_GeV;
  return m_errorMC[etaBin]->Eval(pt*m_invGeV);
}

double JERProvider::getInsituDiffDataMC(double pt, double eta) {
  int etaBin = getEtaBin(eta);
  if (etaBin>3) etaBin=3; // due to lack of stats from data/mc
  if (pt>1000*m_GeV) pt=1000*m_GeV; if (fabs(eta)>2.1 && pt>300*m_GeV) pt=300*m_GeV;
  return m_diffDataMC[etaBin]->Eval(pt*m_invGeV);
}
double JERProvider::getInsituDiffDataMCerror(double pt, double eta) {
  int etaBin = getEtaBin(eta);
  double m_pt = pt;
  if (etaBin>3){  // due to lack of stats from data/mc
    double diffError = 0;
    if(etaBin == 4) {
      if (m_collectionName == "AntiKt6TopoJES")       {diffError = 0.05;}
      else if( m_collectionName == "AntiKt6LCTopoJES"){diffError = 0.10;}
      else if (m_collectionName == "AntiKt4TopoJES" ) {diffError = 0.10;}
      else if( m_collectionName == "AntiKt4LCTopoJES"){diffError = 0.08;}
    }
    else if(etaBin == 5) {
      if (m_collectionName == "AntiKt6TopoJES")       {diffError = 0.05;}
      else if( m_collectionName == "AntiKt6LCTopoJES"){diffError = 0.10;}
      else if (m_collectionName == "AntiKt4TopoJES" ) {diffError = 0.10;}
      else if( m_collectionName == "AntiKt4LCTopoJES"){diffError = 0.08;}
    }
    return diffError;
  }
  
  if (m_pt>1000*m_GeV) m_pt=1000*m_GeV; if (fabs(eta)>2.1 && m_pt>300*m_GeV) m_pt=300*m_GeV;
  return m_errorDataMC[etaBin]->Eval(m_pt*m_invGeV)*0.01;
}
double JERProvider::getInsituUncert(double pt, double eta) {
//  int etaBin = getEtaBin(eta);
//  if (etaBin==4) { 
//    double reso_MC=getRelResolutionMC(pt,eta);
//    if (etaBin==5) { // |eta| > 3.6
//      if (m_jetAlgo==AKt4EM) return reso_MC*0.11;
//      if (m_jetAlgo==AKt4LC) return reso_MC*0.08;
//      if (m_jetAlgo==AKt6EM) return reso_MC*0.07;
//      if (m_jetAlgo==AKt6LC) return reso_MC*0.02;
//    }
//    // 2.8 < |eta| < 3.6
//    if (m_jetAlgo==AKt4EM) return reso_MC*0.05;
//    if (m_jetAlgo==AKt4LC) return reso_MC*0.04;
//    if (m_jetAlgo==AKt6EM) return reso_MC*0.08;
//    if (m_jetAlgo==AKt6LC) return reso_MC*0.08;
//  }
  return sqrt( pow(getInsituUncertData(pt,eta),2) + pow(getInsituUncertMC(pt,eta),2) );
}


double JERProvider::getOffset(double pt, double eta) {
  double insitu_reso_data=getInsituRelResolutionData(pt,eta), insitu_reso_MC=getInsituRelResolutionMC(pt,eta);
//  int etaBin = getEtaBin(eta); 
//  reso_MC=getRelResolutionMC(pt,eta);
  
//  // Hardcoded for now in forward region
//  if (etaBin==4) { // 2.8 < |eta| < 3.6
//    if (m_jetAlgo==AKt4EM) return reso_MC*0.05;
//    if (m_jetAlgo==AKt4LC) return reso_MC*0.06;
//    if (m_jetAlgo==AKt6EM) return reso_MC*0.08;
//    if (m_jetAlgo==AKt6LC) return reso_MC*0.08;
//  } else if (etaBin==5) { // |eta| > 3.6
//    if (m_jetAlgo==AKt4EM) return reso_MC*0.12;
//    if (m_jetAlgo==AKt4LC) return reso_MC*0.10;
//    if (m_jetAlgo==AKt6EM) return reso_MC*0.11;
//    if (m_jetAlgo==AKt6LC) return reso_MC*0.10;
//  }

  // reso(in-situ data) - reso(in situ MC fullsim)
  return (insitu_reso_data-insitu_reso_MC);
}


/*
 *  The uncertainty a-la 2012
 */
double JERProvider::getUncertainty(double pt, double eta, bool alt2, bool isAFII) {

  if(m_is7TeV){
    if(alt2 || isAFII) fatal("=== Alt2/AFII option not available for 7 TeV ====");
    return getUncertainty_7TeV(pt,eta);
  }
  
  // avoid specific issue wiht low pT extrapolation
  if ( isAFII && m_jetAlgo==AKt4LC && getEtaBin(eta)==0 && pt<25.0*m_GeV ) pt=25.0*m_GeV;

  double insituFitErr = getInsituUncert(pt,eta); // total insitu uncertainty
  double jer_data = getRelResolutionData(pt,eta), 
    jer_MC = isAFII ? getRelResolutionMC_AFII(pt,eta) : getRelResolutionMC(pt,eta);

  double offset = jer_data-jer_MC;

  // More optimisitc uncertainty
  if (alt2) {
    if ( jer_data<jer_MC ) offset=0;
    return TMath::Max(fabs(offset),insituFitErr);
  }
  
  // Default for Moriond2013, CONSERVATIVE
  return sqrt(pow(insituFitErr,2)+pow(offset,2));
}

/*
 *  The uncertainty a-la 2011
 */
double JERProvider::getUncertainty_7TeV(double pt, double eta) {
  int i_eta =getEtaBin(eta);
   // Setting boundaries due to lack of statistics
  float ptmin = 10; float ptmax = 1000; float ptpiv = 2000;
  if(i_eta == 1){ptmax = 800; ptpiv = 1600;}
  if(i_eta > 2){ptmax = 500; ptpiv = 1000;}

  //get error in data-mc diff.
  double m_dataMCerror = getInsituDiffDataMCerror(pt,eta);
  //get systematics
  double m_syst = getSystematics_7TeV(i_eta);

  // Above validated range (no data available), we double the uncertainty at
  // ptpiv to be conservative
  float factor = 0;
  if ( pt*m_invGeV >= ptmin && pt*m_invGeV <= ptmax) factor = 0;
  else if( pt*m_invGeV > ptmax && pt*m_invGeV <= ptpiv) factor = (pt*m_invGeV - ptmax)/(ptpiv-ptmax);
  else if( pt*m_invGeV > ptpiv) factor = 1;
  
//  cout << "factor = " << factor << endl;
//  cout <<" data reso = " << getRelResolutionData(pt,eta) << endl;
//  cout << "data-mc error = " << m_dataMCerror << endl;
//  cout << "syst = " << m_syst << endl;
  double uncertainty = (1+factor)*sqrt(m_dataMCerror*m_dataMCerror+m_syst*m_syst)*getRelResolutionData(pt,eta);
  
 return uncertainty;

}

double JERProvider::getSystematics_7TeV(int etaBin)
{

  double syst = 0;
  // Return the requested parametrization
  if (etaBin == 0) {
    if (m_collectionName == "AntiKt6TopoJES")syst = 0.09;  else if( m_collectionName == "AntiKt6LCTopoJES") syst = 0.11;
    else if (m_collectionName == "AntiKt4TopoJES" )syst = 0.17;  else if( m_collectionName == "AntiKt4LCTopoJES") syst = 0.15;
  }
  else if (etaBin == 1) {
    if (m_collectionName == "AntiKt6TopoJES")syst = 0.1; else if( m_collectionName == "AntiKt6LCTopoJES") syst = 0.10;
    else if (m_collectionName == "AntiKt4TopoJES" )syst = 0.17;  else if( m_collectionName == "AntiKt4LCTopoJES") syst = 0.15;
  }
  else if (etaBin == 2) {
    if (m_collectionName == "AntiKt6TopoJES")syst = 0.12;  else if( m_collectionName == "AntiKt6LCTopoJES") syst = 0.23;
    else if (m_collectionName == "AntiKt4TopoJES" )syst = 0.19;  else if( m_collectionName == "AntiKt4LCTopoJES") syst = 0.15;
  }
  else if (etaBin == 3) {
    if (m_collectionName == "AntiKt6TopoJES")syst = 0.12;  else if( m_collectionName == "AntiKt6LCTopoJES") syst = 0.23;
    else if (m_collectionName == "AntiKt4TopoJES" )syst = 0.19;  else if( m_collectionName == "AntiKt4LCTopoJES") syst = 0.19;
  }
  else if (etaBin == 4) {
    if (m_collectionName == "AntiKt6TopoJES")syst = 0.13;  else if( m_collectionName == "AntiKt6LCTopoJES") syst = 0.23;
    else if (m_collectionName == "AntiKt4TopoJES" )syst = 0.19;  else if( m_collectionName == "AntiKt4LCTopoJES") syst = 0.19;
  }
  else if (etaBin == 5) {
    if (m_collectionName == "AntiKt6TopoJES")syst = 0.13;  else if( m_collectionName == "AntiKt6LCTopoJES") syst = 0.23;
    else if (m_collectionName == "AntiKt4TopoJES" )syst = 0.19;  else if( m_collectionName == "AntiKt4LCTopoJES") syst = 0.19;
  }
  return syst;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// Get in-situ method systematic
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
double JERProvider::getDeltaInSitu(double pt, double y)
{
  double InSituSyst = 0;
  InsituSystematic mySyst = InsituSystematic();
  mySyst.GetSRIsystematic(m_collectionName, pt*m_invGeV, y, InSituSyst);

  return InSituSyst;
}
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// Get Closure Syst
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
double JERProvider::getDeltaClosure()
{
  double ClosureSyst = 0;
  if(m_collectionName == "AntiKt6TopoJES" || m_collectionName == "AntiKt6LCTopoJES") ClosureSyst = 0.035;
  if(m_collectionName == "AntiKt4TopoJES" || m_collectionName == "AntiKt4LCTopoJES") ClosureSyst = 0.02;
  return ClosureSyst;
}
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// Get syst due to choice of MC
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
double JERProvider::getDeltaMC()
{
  double MCsyst = 0.05;
  return MCsyst;
}


