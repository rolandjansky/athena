/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetUncertainties/JESUncertaintyProvider.h"
#include "JetUncertainties/JESUtils.h"
#include "TEnv.h"
#include "TROOT.h"
#include <stdlib.h>


/////////////////////
// Constructor/destructor
/////////////////////

// Constructor
//compatibility with MultijetJES: LC needs to be both here and in the config file
//the function parameter is ignored later on in the JESUncertaintyProvider
JESUncertaintyProvider::JESUncertaintyProvider(TString inputConfigFile, TString jetAlgo, TString MCtype, TString path):
  m_path(path), m_uncert(0), m_corrUncert(0), m_muUncert(0), m_npvUncert(0),
  m_GeV(1000.0), m_isInit(false), m_NPVRef(4.9), m_muRef(5.4)
{
  initJES(inputConfigFile, jetAlgo, MCtype);
}

//for persistification, doesn't call initJES() automatically
JESUncertaintyProvider::JESUncertaintyProvider():
  m_uncert(0), m_corrUncert(0), m_muUncert(0), m_npvUncert(0),
  m_GeV(1000.0), m_isInit(false), m_NPVRef(4.9), m_muRef(5.4) {

  }

// Destructor
JESUncertaintyProvider::~JESUncertaintyProvider()
{
  for (unsigned i=0;i<m_uncertComp.size();++i)
    if (m_uncertComp[i]) delete m_uncertComp[i];
  if (m_uncert) delete m_uncert;
  if (m_corrUncert) delete m_corrUncert;
  //if (m_muUncert) delete m_muUncert;
  //if (m_npvUncert) delete m_npvUncert;
}

int JESUncertaintyProvider::getComponentIndex(TString compName) {
  if (m_compIndex.find(compName)==m_compIndex.end())
    Fatal("JESUncertaintyProvider::getComponentIndex()",
	  "ERROR: No such uncertainty component: %s",compName.Data());
  return m_compIndex[compName];
}

TH2 *JESUncertaintyProvider::GetHisto(TFile *file, TString hname, TString method) {
  TH2 *h = (TH2*)file->Get(hname);
  if (h==NULL) 
    Fatal(method.Data(),"Cannot access histo %s in file %s",hname.Data(),file->GetName());
  return h;
}

TH2 *JESUncertaintyProvider::MakeEmptyPtEtaHisto(TString hname) {
  return JESUtils::MakeEmptyPtEtaHisto(hname);
}

double JESUncertaintyProvider::readPtEtaHisto(TH2 *h2d, double pt, double eta) {
  static int nWarn = 0;
  if ( pt/m_GeV<15 || pt/m_GeV>=2500 ) {
    if (++nWarn<10) 
      Warning("JESUncertaintyProvider::readPtEtaHisto()","jet pT outside 15-2500 range: pT=%.1f GeV. Using closest valid value. \n (only first 10 messages will be printed). ",pt/m_GeV);
    if (pt/m_GeV<15) pt=15.0*m_GeV;
    if (pt/m_GeV>=2500) pt=2499.9*m_GeV;
  }
  if (fabs(eta)>=4.5) {
   if (++nWarn<10) 
     Warning("JESUncertaintyProvider::readPtEtaHisto()","jet eta=%.2f outside |eta|<4.5. Using closest valid value. \n (only first 10 messages will be printed). ",eta);
     eta=eta>=0?4.499:-4.499;
  }
  return h2d->Interpolate(pt/m_GeV,eta);
  
}

/////////////////////
// Initialisation
/////////////////////
void JESUncertaintyProvider::initJES(TString inputConfigFile, TString jetAlgo, TString MCtype) 
{
  
  //cache current directory 
  TDirectory* curdir = gDirectory;  
  gROOT->cd(); 
  
  //prevent multiple initializations
  if (m_isInit == false) {
    
    // Identify the path to the uncertainty configuration files based on the location of
    // the main configuration file according to the below priority:
    //   1. User specified path using the setPath("myPath") method
    //   2. The current directory: ./
    //   3. $ROOTCOREDIR/JetUncertainties/data 
    //      (which is a soft link to JetUncertainites/share)
    if ( m_path!="" ) {
      // 1. User specified path
      if (m_path[m_path.Length()-1]!='/') m_path+='/';
      if (!FileExist(m_path+inputConfigFile)) 
	Fatal("JESUncertaintyProvider::init()","Cannot find file %s\nUser specified path: %s",
	      (m_path+inputConfigFile).Data(),m_path.Data());
    } else if (FileExist(inputConfigFile)) {
      m_path="./"; 
    } else {
      TString RootCoreDir = gSystem->Getenv("ROOTCOREBIN");
      if (RootCoreDir == "") RootCoreDir = gSystem->Getenv("ROOTCOREDIR");
      m_path = RootCoreDir+"/data/JetUncertainties/";
      if (!FileExist(m_path+inputConfigFile)) 
	Fatal("JESUncertaintyProvider::init()","\nCannot find file %s\nSearched in:\n  1. ./\n  2. %s",
	      inputConfigFile.Data(),m_path.Data());
    }
    m_inputConfigFile = m_path+inputConfigFile;

    Info("JESUncertaintyProvider::init()", "Path for input files: %s",m_path.Data());

    m_jetAlgo = jetAlgo;
    m_MCtype = MCtype;
 
    // 1. Check jet algo
    int jetR = 4;
    if (m_jetAlgo.Contains("Kt6")) jetR=6;
    else if (!m_jetAlgo.Contains("Kt4")) 
      Fatal("JESUncertaintyProvider::init()", "ERROR: Cannot interpret jet algo argument: %s",m_jetAlgo.Data());
    
    TString calib="EMJES";
    if (m_jetAlgo.Contains("LC")) calib="LCJES";
    else if (!m_jetAlgo.Contains("EM")) 
      Fatal("JESUncertaintyProvider::init()", "ERROR: Cannot interpret jet algo argument: %s",m_jetAlgo.Data());
    
    // 2. Check the MC type
    TString mcType=getMCtype();
    if (mcType=="Error") 
      Fatal("JESUncertaintyProvider::init()", "ERROR: Cannot interpret MC type argument: %s",m_MCtype.Data());
    
    // -- Input arguments seems OK, lets print a message:
    // --------------------------------------------------
    
    Info("JESUncertaintyProvider::init()","================================================");
    Info("JESUncertaintyProvider::init()","  Initializing the JESUncertaintyProvider tool");
    Info("JESUncertaintyProvider::init()","  Using combination of in-situ techniques" );
    Info("JESUncertaintyProvider::init()","  For Anti kt R=0.%d, %s",jetR,TString(calib).ReplaceAll("JES","+JES").Data());
    Info("JESUncertaintyProvider::init()","  Configuration read in from:" );
    Info("JESUncertaintyProvider::init()","    %s",m_inputConfigFile.Data());
    
    // -- Now read the config file, and identify and open the input file

    TEnv settings;
    //this returns -1 if unsuccessful
    int status = settings.ReadFile(m_inputConfigFile.Data(),EEnvLevel(0));
    if (status!=0) 
      Fatal("JESUncertaintyProvider::init()", 
	    "Cannot read file  %s",m_inputConfigFile.Data());

    //avoid ROOT's "Replacing existing TH1" by having specialised file name
    TString emptyHistoSuffix = m_inputConfigFile;
    m_uncert = MakeEmptyPtEtaHisto("TotalUncertainty_"+emptyHistoSuffix+"_"+m_jetAlgo+"_"+m_MCtype);
    m_corrUncert = MakeEmptyPtEtaHisto("TotalBias_"+emptyHistoSuffix+"_"+m_jetAlgo+"_"+m_MCtype);

    m_fileName = settings.GetValue("JESUncertaintyRootFile","JESUncertainty.root");
    
    //need to do this via separate function for ATHENA's PathResolver
    TFile* inputFile = NULL;
    inputFile = openInputFile(m_fileName);

    if (inputFile==NULL)
      Fatal("JESUncertaintyProvider::init()","Cannot open %s",m_fileName.Data());

    TString suffix=Form("_AntiKt%dTopo_",jetR)+calib;
    
    // Let's read in all those uncertainty components!
    Info("JESUncertaintyProvider::init()","%6s %-30s : %s","","JES uncert. comp.","Description");
    Info("JESUncertaintyProvider::init()","%6s %-30s --%s","","-----------------","-----------");
    for (int ci=0;ci<250;++ci) {
      
      //easier to printf-style this 
      TString prefix=Form("JESComponent.%d.",ci);

      // Read in the name and the description of the uncertainty component
      TString compName = settings.GetValue(prefix+"Name","");
      TString desc = settings.GetValue(prefix+"Desc","");
      
      // ignore if component is not defined
      if (compName=="") continue;

      bool addAsCorrelated = settings.GetValue(prefix+"AddAsCorrelated",false);
      
      compName.ReplaceAll("MCTYPE",mcType);

      int index=m_uncertComp.size();

      /*
       *  Read in the uncertainty component
       */
      TH2 *uncComp;
      StrV subComponents = Vectorize(settings.GetValue(prefix+"SubComponents",""));
      StrV corrSubComponents = Vectorize(settings.GetValue(prefix+"CorrSubComponents",""));
      if (subComponents.size())
        uncComp = MergeSubcomponents(subComponents,compName,suffix,inputFile,false);// "false" as not correlated subcomponents
      else if (corrSubComponents.size())
      {
	  std::cout << "Merging correlated subcomponents" << std::endl;
          uncComp = MergeSubcomponents(corrSubComponents,compName,suffix,inputFile,true);// "true" as correlated subcomponents
      }
      else 
	uncComp = GetHisto(inputFile,compName+suffix,"JESUncertaintyProvider::init()");
     
      uncComp->SetName(TString("the").Append(compName)); // for PROOF compatibility
      uncComp->SetDirectory(0);
      
      /*
       *  If this is the pile-up uncertainty, store it away and jump to next component
       */
      if (compName.Contains("offsetmu",TString::kIgnoreCase)) {
	//m_compType.push_back(mu_nuis);
	if ( m_muUncert == NULL ) m_muUncert = uncComp;
	else Fatal("JESUncertaintyProvider::init()","Mu offset component specified twice? %s",compName.Data());
	continue;
      } else if (compName.Contains("offsetnpv",TString::kIgnoreCase)) {
	//m_compType.push_back(npv_nuis);
	if ( m_npvUncert == NULL ) m_npvUncert = uncComp;
	else Fatal("JESUncertaintyProvider::init()","NPV offset component specified twice? %s",compName.Data());
	continue;
      }
      
      /*
       *  If not pile-up, print it, cache it, and add it to the list ...
       */
      Info("JESUncertaintyProvider::init()","%5d. %-30s : %s",index+1,compName.Data(),desc.Data());
      if (addAsCorrelated) 
	Info("JESUncertaintyProvider::init()","%6s  => added as fully correlated","");

      // a. Cache, cache baby
      if (addAsCorrelated) {
	m_compType.push_back(corr_nuis);
	Add(uncComp,m_corrUncert);
      } else {
	m_compType.push_back(uncorr_nuis);
	AddInQuad(uncComp,m_uncert);
      }

      // b. Store histo and the index in the histogram vector
      m_compIndex[compName]=index;
      m_uncertComp.push_back(uncComp);
      
      // c. Store the name, the description and correlation information
      m_compNames.push_back(compName);
      m_compDesc.push_back(desc);

    }

    // Read in the pile-up uncertainty refernece point
    double muRef = settings.GetValue("Pileup.MuRef",-1.0);
    double npvRef = settings.GetValue("Pileup.NPVRef",-1.0);
    if (muRef>=0) setMuRef(muRef); // sets m_muRef
    if (npvRef>=0) setNPVRef(npvRef); // sets m_npvRef
    
    // write lines
    //printf(  "%6s %-30s --%s\n","---","-----------------","-----------");
    m_isInit = true;
    if (  m_muUncert ) 
      Info("JESUncertaintyProvider::init()","    Pileup: Offset, mu term");
    if (  m_npvUncert ) 
      Info("JESUncertaintyProvider::init()","    Pileup: Offset, NPV term");

    Info("JESUncertaintyProvider::init()","    Pileup reference: (NPV,mu) = (%.1f,%.1f)",m_muRef,m_NPVRef);
    Info("JESUncertaintyProvider::init()","================================================");

    inputFile->Close(); 
    delete inputFile;
  }//end of if not initialised
  else {
    Warning( "JESUncertaintyProvider::init()", "WARNING: JESUncertaintyProvider already initialized, skipping re-initialization");
  }
  
  //go back to earlier directory 
  gDirectory = curdir;  

}

//bool JESUncertaintyProvider::setInputCollection(TString jetAlgo) {} 

TString JESUncertaintyProvider::getMCtype() {
  if (m_MCtype.Contains("MC11b", TString::kIgnoreCase)) return "MC11b";
  if (m_MCtype.Contains("MC11c", TString::kIgnoreCase)) return "MC11c";
  if (m_MCtype.Contains("FrozenShowers", TString::kIgnoreCase)) return "FrozenShowers";
  if (m_MCtype.Contains("AFII", TString::kIgnoreCase)) return "AFII";
  if (m_MCtype.Contains("Pythia8", TString::kIgnoreCase)) return "Pythia8";
  if (m_MCtype.Contains("MC12a", TString::kIgnoreCase)) return "Pythia8";
  return "Error";
}

bool JESUncertaintyProvider::checkBinning(const TH2 *in, const TH2 *tot) {
  for (int i=1;i<=in->GetNbinsX()+1;++i) {
    double x=in->GetXaxis()->GetBinLowEdge(i);
    int bin=tot->GetXaxis()->FindBin(x+1e-5);
    if (fabs(x-tot->GetXaxis()->GetBinLowEdge(bin))>1e-3) 
      Fatal("JESUncertaintyProvider::checkBinning","Bad binning at pT=%.2f for histo %s",x,in->GetName());
  }
  for (int i=1;i<=in->GetNbinsY()+1;++i) {
    double x=in->GetYaxis()->GetBinLowEdge(i);
    int bin=tot->GetYaxis()->FindBin(x+1e-5);
    if (fabs(x-tot->GetYaxis()->GetBinLowEdge(bin))>1e-3) 
      Fatal("JESUncertaintyProvider::checkBinning","Bad binning at eta=%.2f for histo %s",x,in->GetName());
  }
  return true;
}

void JESUncertaintyProvider::Add(TH2 *in, TH2 *tot) {
  checkBinning(in,tot);
  for (int ipt=1;ipt<=tot->GetNbinsX();++ipt)
    for (int ieta=1;ieta<=tot->GetNbinsY();++ieta) {
      double pt=tot->GetXaxis()->GetBinCenter(ipt);
      double eta=tot->GetYaxis()->GetBinCenter(ieta);
      tot->SetBinContent(ipt,ieta,in->Interpolate(pt,eta)+tot->GetBinContent(ipt,ieta));
    }
}

void JESUncertaintyProvider::AddInQuad(TH2 *in, TH2 *tot) {
  checkBinning(in,tot);
  for (int ipt=1;ipt<=tot->GetNbinsX();++ipt)
    for (int ieta=1;ieta<=tot->GetNbinsY();++ieta) {
      double pt=tot->GetXaxis()->GetBinCenter(ipt);
      double eta=tot->GetYaxis()->GetBinCenter(ieta);
      double uin=in->Interpolate(pt,eta), uold=tot->GetBinContent(ipt,ieta);
      tot->SetBinContent(ipt,ieta,sqrt(uin*uin+uold*uold));
    }
}

double JESUncertaintyProvider::getRelUncertComponent(TString compName, double pT, double eta) {
  return getRelUncertComponent(getComponentIndex(compName),pT,eta);
}

double JESUncertaintyProvider::getRelUncertComponent(int iComp, double pT, double eta) {
  if (iComp>getNUncertaintyComponents())
    Fatal("JESUncertaintyProvider::getRelUncertComponent","JES uncertainty component index out-of-range.");
  TH2 *h2d = m_uncertComp.at(iComp);
  return readPtEtaHisto(h2d,pT,eta);
}



/////////////////////
// Uncertainty 
/////////////////////

// Absolute Uncertainty
/*
double JESUncertaintyProvider::getAbsUncert(double pT, double eta, double NPV, double mu) {
  return getRelUncert(pT, eta, NPV, mu)*pT;
}

double JESUncertaintyProvider::getRelUncert(double pT, double eta, double NPV, double mu) {
  return sqrt( pow(getRelUncert(pT,eta),2) + pow(getRelOffsetUncert(pT,eta,NPV,mu),2) );
}
*/

double JESUncertaintyProvider::getRelUncert(double pT, double eta) {
  return fabs(getRelUncertUncorr(pT,eta)) + fabs(getRelUncertBias(pT,eta));
}

double JESUncertaintyProvider::getRelUncertUncorr(double pT, double eta) {
  return readPtEtaHisto(m_uncert,pT,eta);
}

double JESUncertaintyProvider::getRelUncertBias(double pT, double eta) {
  return readPtEtaHisto(m_corrUncert,pT,eta);
}

double JESUncertaintyProvider::getRelNPVOffsetTerm(double pT, double eta, double NPV) {
  if ( m_npvUncert == NULL ) 
    Fatal("JESUncertaintyProvider::getRelNPVOffsetTerm",
	  "Offset NPV-dependent uncertainty not initialized");
  return readPtEtaHisto(m_npvUncert,pT,eta) * ( NPV - m_NPVRef);
}

double JESUncertaintyProvider::getRelMuOffsetTerm(double pT, double eta, double mu) {
  if ( m_muUncert == NULL ) 
    Fatal("JESUncertaintyProvider::getRelMuOffsetTerm",
  	  "Offset mu-dependent uncertainty not initialized");
  return readPtEtaHisto(m_muUncert,pT,eta) * ( mu - m_muRef);
}

double JESUncertaintyProvider::getRelOffsetUncert(double pT, double eta, double NPV, double mu) {
  double muUncert = getRelMuOffsetTerm(pT,eta,mu);
  double npvUncert = getRelNPVOffsetTerm(pT,eta,NPV);
  return sqrt( muUncert*muUncert + npvUncert*npvUncert );
}

//function for easier looping on nuisance parameters
//these are the names that should be used as components when calling getRelUncert
std::vector<TString> JESUncertaintyProvider::getComponentNames() {  
  return m_compNames;
}

TString JESUncertaintyProvider::getComponentName(int iComp) {
  if (iComp>=int(m_compNames.size()))
    Fatal("JESUncertaintyProvider::getComponentName()",
	  "You are asking for comp. %d. Only %d components available.",iComp+1,int(m_compNames.size()));
  return m_compNames.at(iComp);
}



//these are the titles of the nuisance parameters (index-parallel with the vector of names)
std::vector<TString> JESUncertaintyProvider::getComponentDescriptions() {
  return m_compDesc;
}

bool JESUncertaintyProvider::isIntercalibrationComponent (const TString & component) {
  return component.Contains("etaIntercalibration", TString::kIgnoreCase);
}

bool JESUncertaintyProvider::isInSituComponent (const TString & component) {
  return ( component.Contains("InSitu", TString::kIgnoreCase) || 
	   component.Contains("Zjet", TString::kIgnoreCase) || 
	   component.Contains("MPF", TString::kIgnoreCase) ||
	   component.Contains("MJB", TString::kIgnoreCase) );
}

bool JESUncertaintyProvider::isNonClosureComponent (const TString & component) {
  return ( component.Contains("NonClosure", TString::kIgnoreCase) );
}

bool JESUncertaintyProvider::isPileupComponent (const TString & component) {
  return ( component.Contains("Pileup", TString::kIgnoreCase) );
}

StrV JESUncertaintyProvider::Vectorize(TString str, TString sep) {
  StrV result; TObjArray *strings = str.Tokenize(sep.Data());
  if (strings->GetEntries()==0) return result;
  TIter istr(strings);
  while (TObjString* os=(TObjString*)istr())
    if (os->GetString()[0]!='#') result.push_back(os->GetString());
    else break;
  delete strings;
  return result;
}

TH2 *JESUncertaintyProvider::MergeSubcomponents(StrV subComponents, TString name, TString suffix, TFile *file, bool isCorrelated) {
  StrV compVec;
  for (unsigned i=0;i<subComponents.size();++i) {
    TString compName=subComponents[i];
    compName.ReplaceAll(" ",""); // cleanup
    if (compName.Contains("FROM")) {
      int start=compName.Index("FROM"), mid=compName.Index("TO"), end=compName.Sizeof();
      int starti=atol(compName(start+4,mid-start-4).Data());
      int endi=atol(compName(mid+2,end-mid-3).Data());
      if (endi<=starti)
	Fatal("JESUncertaintyProvider::MergeSubcomponents","Can not interpret %s",compName.Data());
      compName.ReplaceAll(Form("FROM%dTO%d",starti,endi),"");
      for (int i=starti;i<=endi;++i) {
	compVec.push_back(compName+Form("%d",i));
      }
    } else {
      compVec.push_back(compName);
    }
  }
  if (compVec.size()==0)
    Fatal("JESUncertaintyProvider::MergeSubcomponents","No components?");
  
  TH2 *totUnc = MakeEmptyPtEtaHisto(name);
  for (unsigned i=0;i<compVec.size();++i) {
    TH2 *uncComp = GetHisto(file,compVec[i]+suffix,
			    "JESUncertaintyProvider::MergeSubcomponents()");
    if (!isCorrelated) AddInQuad(uncComp,totUnc);
    else Add(uncComp,totUnc);
  }
  return totUnc;
}

