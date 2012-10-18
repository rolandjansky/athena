/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetUncertainties/UJUncertaintyProvider.h"
#include "TEnv.h"
#include <stdlib.h>

/////////////////////
// Constructor/destructor
/////////////////////

// Constructor
//compatibility with MultijetUJ: LC needs to be both here and in the config file
//the function parameter is ignored later on in the UJUncertaintyProvider
UJUncertaintyProvider::UJUncertaintyProvider(TString jetAlgo, TString MCtype):
  m_uncert(0), m_corrUncert(0), m_muUncert(0), m_npvUncert(0),
  m_GeV(1000.0), m_isInit(false), m_NPVRef(4.9), m_muRef(5.4)
{
  initUJ(jetAlgo, MCtype);
}

//for persistification, doesn't call initUJ() automatically
UJUncertaintyProvider::UJUncertaintyProvider():
  m_uncert(0), m_corrUncert(0), m_muUncert(0), m_npvUncert(0),
  m_GeV(1000.0), m_isInit(false), m_NPVRef(4.9), m_muRef(5.4) { }

// Destructor
UJUncertaintyProvider::~UJUncertaintyProvider()
{
}

int UJUncertaintyProvider::getComponentIndex(TString compName) {
  if (m_compIndex.find(compName)==m_compIndex.end())
    Fatal("UJUncertaintyProvider::getComponentIndex()",
	  "ERROR: No such uncertainty component: %s",compName.Data());
  return m_compIndex[compName];
}

double UJUncertaintyProvider::readPtEtaHisto(TH2 *h2d, double pt, double eta) {
  static int nWarn = 0;
  if ( pt/m_GeV<200.0 || pt/m_GeV>=1000.0 ) {
    if (++nWarn<10) 
      Warning("UJUncertaintyProvider::readPtEtaHisto()","jet pT outside 200-1000 range: pT=%.1f GeV \n (only first 10 messages will be printed).",pt/m_GeV);
    if (pt/m_GeV<200.0) pt=200.0*m_GeV;
    if (pt/m_GeV>=1000.0) pt=1000.0*m_GeV;
  }
  if (fabs(eta)>=1.2) {
   if (++nWarn<10) 
     Warning("UJUncertaintyProvider::readPtEtaHisto()","jet eta=%.2f outside |eta|<1.2 \n (only first 10 messages will be printed).",eta);
     eta=1.2;
  }
  // NOTE WE HAVE fabs HERE, NOT THE CASE IN JESUncertaintyProvider!
  //printf("%f:%f\n", pt/m_GeV, fabs(eta));
  return h2d->Interpolate(pt/m_GeV,fabs(eta));
  
}

// Handy macro for doing the algorithm selection
#define _SELECTALG(a) if (!jetAlgo.CompareTo(#a, TString::kIgnoreCase)) { cae = a##_E; cam = a##_m; caetrk = a##_Etrk; camtrk = a##_mtrk; }

/////////////////////
// Initialisation
/////////////////////
void UJUncertaintyProvider::initUJ(TString jetAlgo, TString MCtype) 
{

  //prevent multiple initializations
  if (m_isInit == false) {
  
    m_compIndex["Total Uncertainty"] = 0;

    m_jetAlgo = jetAlgo;
    m_MCtype = MCtype;

    static const int nptbins = 8;
    static const int netabins = 2;

    // Tack on some extra bins so the interpolation is flat at the edges
    static const float ptbins[nptbins+3] = {0.0, 100.0, 150.0, 200.0, 300.0, 400.0, 500.0, 600.0, 800.0, 1000.0, 1000000.0};
    static const float etabins[netabins+3] = {-100.0, 0.0, 0.8, 1.2, 100.0};

    const float (*cae)[nptbins] = 0;
    const float (*cam)[nptbins] = 0;
    const float (*caetrk)[nptbins] = 0;
    const float (*camtrk)[nptbins] = 0;

// Bring in constants from this automatically generated external file
#include "JetUncertainties/UJConstants.h"

    // This uses the macro above, basically this just compares jetAlgo to the argument
    // and sets cae and cam to the right arrays of constants from the header above
    _SELECTALG(CamKt12LCTopoSplitFilteredmassFraction67minSplitR0);
    _SELECTALG(CamKt12LCTopo)
    //_SELECTALG(AntiKt10LCTopoTrimmedPtFrac3SmallR30)
    _SELECTALG(AntiKt10LCTopoTrimmedPtFrac5SmallR30)
    _SELECTALG(AntiKt10LCTopo)

    if (!cae) {
      Fatal("UJUncertaintyProvider::init()", "Algorithm %s is unknown!", (const char*)jetAlgo);
    } 

//    static const float cae[][nptbins] = {{0.05, 0.06, 0.06, 0.06, 0.07}, {0.06, 0.06, 0.07, 0.08, 0.08}};
//    static const float cam[][nptbins] = {{0.05, 0.06, 0.06, 0.06, 0.07}, {0.06, 0.06, 0.07, 0.08, 0.08}};

    TH2F* unce = new TH2F("", "", nptbins+2, ptbins, netabins+2, etabins);
    TH2F* uncm = new TH2F("", "", nptbins+2, ptbins, netabins+2, etabins);

    for (int ip = 1; ip <= nptbins+2; ip++) {
      for (int ie = 1; ie <= netabins+2; ie++) {
        int rip = ip - 2;
        int rie = ie - 2;
	// Find edges for interpolation
        if (rip == -1) { rip++; }
        if (rie == -1) { rie++; }
        if (rip == nptbins) { rip--; }
        if (rie == netabins) { rie--; }
        //printf("Setting %i, %i = %f from %i, %i (%f,%f)\n", ip, ie, cam[rie][rip], rie, rip, unce->GetXaxis()->GetBinLowEdge(ip), unce->GetYaxis()->GetBinLowEdge(ie));

        static const float noncle = 0.02;
        static const float nonclm = 0.03;
	float coree = cae[rie][rip];
	float corem = cam[rie][rip];
        float trke = caetrk[rie][rip];
        float trkm = camtrk[rie][rip];
        float finale = sqrt(coree*coree + trke*trke + noncle*noncle);
        float finalm = sqrt(corem*corem + trkm*trkm + nonclm*nonclm);
	//printf("Finalm: %f %f %f = %f\n", corem, trkm, nonclm, finalm);

        unce->SetBinContent(ip, ie, finale);
        uncm->SetBinContent(ip, ie, finalm);
      }
    }

    m_unc.push_back(unce);
    m_unc.push_back(uncm);

    Info("UJUncertaintyProvider::init()","================================================");
    Info("UJUncertaintyProvider::init()","  Initializing the UJUncertaintyProvider tool");
    Info("UJUncertaintyProvider::init()","  Using track-jet uncertainties" );
    Info("UJUncertaintyProvider::init()","  For %s",(const char*)jetAlgo);
    
  }
  else {
    Warning( "UJUncertaintyProvider::init()", "WARNING: UJUncertaintyProvider already initialized, skipping re-initialization");
  }
}

//bool UJUncertaintyProvider::setInputCollection(TString jetAlgo) {} 

TString UJUncertaintyProvider::getMCtype() {
  if (m_MCtype.Contains("MC11b", TString::kIgnoreCase)) return "MC11b";
  if (m_MCtype.Contains("MC11c", TString::kIgnoreCase)) return "MC11c";
  if (m_MCtype.Contains("FrozenShowers", TString::kIgnoreCase)) return "FrozenShowers";
  if (m_MCtype.Contains("AFII", TString::kIgnoreCase)) return "AFII";
  return "Error";
}

double UJUncertaintyProvider::getRelUncertComponent(TString compName, double pT, double eta, e_prop p) {
  return getRelUncertComponent(getComponentIndex(compName),pT,eta,p);
}

double UJUncertaintyProvider::getRelUncertComponent(int iComp, double pT, double eta, e_prop p) {
  if (iComp>getNUncertaintyComponents())
    Fatal("UJUncertaintyProvider::getRelUncertComponent","UJ uncertainty component index out-of-range.");
  TH2 *h2d = m_unc.at(p);
  return readPtEtaHisto(h2d,pT,eta);
}

// This method should be removed at some point
double UJUncertaintyProvider::getRelUncertComponent(TString compName, double pT, double eta, 
						     double NPV, double mu, e_prop p) {
  return getRelUncertComponent(getComponentIndex(compName),pT,eta,NPV,mu, p);
}

// This method should be removed at some point
double UJUncertaintyProvider::getRelUncertComponent(int iComp, double pT, double eta, 
						     double /*NPV*/, double /*mu*/, e_prop p) {

  return getRelUncertComponent(iComp,pT,eta,p);
}


/////////////////////
// Uncertainty 
/////////////////////

// Absolute Uncertainty
double UJUncertaintyProvider::getAbsUncert(double pT, double eta, double NPV, double mu, double val, e_prop p) {
  return getRelUncert(pT, eta, NPV, mu, p)*val;
}

double UJUncertaintyProvider::getRelUncert(double pT, double eta, double NPV, double mu, e_prop p) {
  return sqrt( pow(getRelUncert(pT,eta,p),2) + pow(getRelOffsetUncert(pT,eta,NPV,mu,p),2) );
}

double UJUncertaintyProvider::getRelUncert(double pT, double eta, e_prop p) {
  return fabs(getRelUncertUncorr(pT,eta, p)) + fabs(getRelUncertBias(pT,eta, p));
}

double UJUncertaintyProvider::getRelUncertUncorr(double pT, double eta, e_prop p) {
  return getRelUncertComponent(0, pT, eta, p); 
}

double UJUncertaintyProvider::getRelUncertBias(double pT, double eta, e_prop p) {
  // Cast parameters to void to tell the compiler that we know the variables aren't used right now
  (void)pT;
  (void)eta;
  (void)p;
  return 0.0;
}

double UJUncertaintyProvider::getRelNPVOffsetTerm(double pT, double eta, double NPV, e_prop p) {
  // Cast parameters to void to tell the compiler that we know the variables aren't used right now
  (void)pT;
  (void)eta;
  (void)p;
  (void)NPV;
  return 0.0;
}

double UJUncertaintyProvider::getRelMuOffsetTerm(double pT, double eta, double mu, e_prop p) {
  // Cast parameters to void to tell the compiler that we know the variables aren't used right now
  (void)pT;
  (void)eta;
  (void)mu;
  (void)p;
  return 0.0;
}

double UJUncertaintyProvider::getRelOffsetUncert(double pT, double eta, double NPV, double mu, e_prop p) {
  double muUncert = getRelMuOffsetTerm(pT,eta,mu,p);
  double npvUncert = getRelNPVOffsetTerm(pT,eta,NPV,p);
  return sqrt( muUncert*muUncert + npvUncert*npvUncert );
}

//function for easier looping on nuisance parameters
//these are the names that should be used as components when calling getRelUncert
std::vector<TString> UJUncertaintyProvider::getComponentNames() {  
  return m_compNames;
}

TString UJUncertaintyProvider::getComponentName(int iComp) {
  if (iComp>=int(m_compNames.size()))
    Fatal("UJUncertaintyProvider::getComponentName()",
	  "You are asking for comp. %d. Only %d components available.",iComp+1,int(m_compNames.size()));
  return m_compNames.at(iComp);
}



//these are the titles of the nuisance parameters (index-parallel with the vector of names)
std::vector<TString> UJUncertaintyProvider::getComponentDescriptions() {
  return m_compDesc;
}

bool UJUncertaintyProvider::isIntercalibrationComponent (const TString & component) {
  return component.Contains("etaIntercalibration", TString::kIgnoreCase);
}

bool UJUncertaintyProvider::isInSituComponent (const TString & component) {
  return ( component.Contains("InSitu", TString::kIgnoreCase) || 
	   component.Contains("Zjet", TString::kIgnoreCase) || 
	   component.Contains("MPF", TString::kIgnoreCase) ||
	   component.Contains("MJB", TString::kIgnoreCase) );
}

bool UJUncertaintyProvider::isNonClosureComponent (const TString & component) {
  return ( component.Contains("NonClosure", TString::kIgnoreCase) );
}

bool UJUncertaintyProvider::isPileupComponent (const TString & component) {
  return ( component.Contains("Pileup", TString::kIgnoreCase) );
}

StrV UJUncertaintyProvider::Vectorize(TString str, TString sep) {
  StrV result; TObjArray *strings = str.Tokenize(sep.Data());
  if (strings->GetEntries()==0) return result;
  TIter istr(strings);
  while (TObjString* os=(TObjString*)istr())
    if (os->GetString()[0]!='#') result.push_back(os->GetString());
    else break;
  delete strings;
  return result;
}

