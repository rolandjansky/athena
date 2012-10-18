/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetUncertainties/MultijetJESUncertaintyProvider.h"
#include "TROOT.h"
#include "TEnv.h"
#include "TIterator.h"
#include "TKey.h"
#include <cstdlib>

// Constructor
MultijetJESUncertaintyProvider::MultijetJESUncertaintyProvider(TString multijetConfig,
							       TString configFile,
							       TString jetAlgo,
							       TString MCtype, TString path):
  JESUncertaintyProvider(configFile,jetAlgo,MCtype,path),
  m_flavorGlu(NULL), m_flavorLight(NULL), m_deltaR(NULL), m_gluonResponseDifference(NULL),
  m_bJESResponse(NULL), 
  m_multiIsInit(false), m_relGluonResponseUncert(-1), m_maxNumJets(0)
{
  initMultijet(multijetConfig);
}

//PROOF
MultijetJESUncertaintyProvider::MultijetJESUncertaintyProvider():
  m_flavorGlu(NULL), m_flavorLight(NULL), m_deltaR(NULL), m_gluonResponseDifference(NULL),
  m_bJESResponse(NULL), 
  m_multiIsInit(false), m_relGluonResponseUncert(-1) { }

// Destructor
MultijetJESUncertaintyProvider::~MultijetJESUncertaintyProvider()
{
  //delete histograms, we own them
  if(m_flavorGlu) delete m_flavorGlu;
  if(m_flavorLight) delete m_flavorLight;  
  if(m_deltaR) delete m_deltaR;
  if(m_gluonResponseDifference) delete m_gluonResponseDifference;
  if(m_bJESResponse) delete m_bJESResponse;
  for ( unsigned int iNJet = 0; iNJet<m_gluonFraction.size(); iNJet++) {
     delete m_gluonFraction[iNJet];
     delete m_gluonFractionError[iNJet];
  }
}

void MultijetJESUncertaintyProvider::initMultijet(TString multijetConfig)
{
  //cache current directory 
  TDirectory* curdir = gDirectory;  
  gROOT->cd(); 
  
  // Prevent multiple initializations 
  if (m_multiIsInit == false) {

    m_multijetConfig = m_path+multijetConfig;

    // this has already been checked
    int jetR = m_jetAlgo.Contains("Kt6") ? 6 : 4;
    TString calib=m_jetAlgo.Contains("LC") ? "LCJES" : "EMJES";

    Info( "MultijetJESUncertaintyProvider::init()", "======================================================" );
    Info( "MultijetJESUncertaintyProvider::init()", " Initializing the MultijetJESUncertaintyProvider tool");
    Info( "MultijetJESUncertaintyProvider::init()","  Configuration read in from:" );
    Info( "MultijetJESUncertaintyProvider::init()","    %s",m_multijetConfig.Data());
    Info( "MultijetJESUncertaintyProvider::init()", "======================================================" );
    
    TEnv settings;
    //this returns -1 if unsuccessful
    int status = settings.ReadFile(m_multijetConfig.Data(),EEnvLevel(0));
    
    if (status!=0) Fatal("MultijetJESUncertaintyProvider::init()","Cannot read file %s",m_multijetConfig.Data());

    // MJES input file name
    TString multijetFN = settings.GetValue("MultijetJESUncertaintyRootFile","MJESUncertainty.root");

    // Analysis input file name
    TString analysisFN = settings.GetValue("AnalysisRootFile","analysisFiles/UnknownFlavourComp.root");

    // Open the input files
    TFile *multijet_file = openInputFile(multijetFN);
    if (multijet_file == NULL) 
      Fatal( "MultijetJESUncertaintyProvider::init()", "ERROR: Cannot open file %s",multijetFN.Data());
    TFile* analysis_file = multijet_file;
    if ( analysisFN!="" ) {
      analysis_file = openInputFile(analysisFN);
      if (analysis_file==NULL)
	Fatal( "MultijetJESUncertaintyProvider::init()", "ERROR: Cannot open file %s",analysisFN.Data());
    }

    TString suffix=Form("_AntiKt%dTopo_",jetR)+calib;
    
    int index=0;
    // Now check which MJES components we are interested in
    for (int ci=0;ci<100;++ci) {
      
      //easier to printf-style this (thanks Dag)
      TString prefix = Form("MultijetJESComponent.%d.",ci);
      
      // Read in the name and the description of the uncertainty component
      TString compName = settings.GetValue(prefix+"Name","");
      TString desc = settings.GetValue(prefix+"Desc","");
      
      compName.ReplaceAll(" ","");
      if (compName=="") continue;
      index++;

      //set the flags for the two multijet components
      if ( compName.Contains("Flavor_Comp",TString::kIgnoreCase) || 
	   compName.Contains("Flavour_Comp",TString::kIgnoreCase)) {

	//m_FlavorCompName = inputName;
	Info( "MultijetJESUncertaintyProvider::init()", " %2d. %-15s : %s",index,compName.Data(),desc.Data()); 

	// Gluon flavor composition graph
	m_flavorGlu   = (TH2D*)GetHisto(multijet_file,"flavorCompGlu"+suffix,"MultijetJESUncertaintyProvider::init()");
	m_flavorLight = (TH2D*)GetHisto(multijet_file,"flavorCompLight"+suffix,"MultijetJESUncertaintyProvider::init()");

	// Some magic needed for PROOF
	m_flavorGlu->SetName(TString("the")+"flavorCompGlu"+suffix); m_flavorGlu->SetDirectory(0);
	m_flavorLight->SetName(TString("the")+"flavorCompLight"+suffix); m_flavorLight->SetDirectory(0);
	
	// Analysis sample graphs

	// Inclusive gluon fraction/gluon fraction uncertainty 2D histo
	m_gluonFraction.push_back( (TH2D*)GetHisto(analysis_file,"gluonFraction"+suffix,"MultijetJESUncertaintyProvider::init()") );
	m_gluonFraction[0]->SetName(TString("the")+"gluonFraction"+suffix); m_gluonFraction[0]->SetDirectory(0);
        m_gluonFractionError.push_back( (TH2D*)GetHisto(analysis_file,"gluonFractionError"+suffix,"MultijetJESUncertaintyProvider::init()") );
        m_gluonFractionError[0]->SetName(TString("the")+"gluonFractionError"+suffix); m_gluonFractionError[0]->SetDirectory(0);

        // Read in other nJets histos
        
        unsigned int numJetCounter = 0;

        TIter nextkey(analysis_file->GetListOfKeys());
        TKey *key;          
        
        while ( ( key = (TKey*)nextkey() ) ) {
          
          std::string histoName = key->GetName();
          
          if (histoName.find("njet")!=std::string::npos && histoName.find("gluonFractionError")!=std::string::npos) {
            
            numJetCounter++;
            int nJetDigitPosition = histoName.find("njet");         
            std::string nJetDigit = histoName.substr(nJetDigitPosition+4, histoName.length());            
            
            if ( (unsigned int)( atoi(nJetDigit.c_str()) ) > m_maxNumJets) m_maxNumJets = (unsigned int)( atoi(nJetDigit.c_str()) );
            
          }
                      
        }
        
        //check there is none missing
        if (numJetCounter != m_maxNumJets) Fatal("MultijetJESUncertaintyProvider::init()",
                                               "Not enough histograms for maximum number of jets requested: maxNumJets=%d",m_maxNumJets);

        //now loop on the number of jets and insert in vector
        for ( unsigned int iNJet = 0; iNJet<m_maxNumJets; iNJet++) {
        
          TString njetstring = Form("_njet%d",iNJet+1);
          m_gluonFraction.push_back( (TH2D*)GetHisto(analysis_file,"gluonFraction"+suffix+njetstring,"MultijetJESUncertaintyProvider::init()") );
          m_gluonFraction[iNJet+1]->SetName(TString("the")+"gluonFraction"+suffix+njetstring); 
          m_gluonFraction[iNJet+1]->SetDirectory(0);
       
          m_gluonFractionError.push_back( (TH2D*)GetHisto(analysis_file,"gluonFractionError"+suffix+njetstring,"MultijetJESUncertaintyProvider::init()") );
          m_gluonFractionError[iNJet+1]->SetName(TString("the")+"gluonFractionError"+suffix+njetstring); 
          m_gluonFractionError[iNJet+1]->SetDirectory(0);
          
        }

        if (m_maxNumJets == 0) {
          
          Warning( "MultijetJESUncertaintyProvider::init()", " No Njet-dependent flavour uncertainty available. You will not be able to use the nJets argument of the getRelUncert functions. ");         
        
        }
        
//     
//         // Try to read in the 1-jet
//         TString njetstring = Form("_njet%d",1);
//         TH2D* histoGluonFractionNjet = (TH2D*)GetHisto(analysis_file,"gluonFraction"+suffix+njetstring,"MultijetJESUncertaintyProvider::init()");
//         
//         if ( histoGluonFractionNjet == NULL ) {
//           Warning( "MultijetJESUncertaintyProvider::init()", " No Njet-dependent flavour uncertainty available. You will not be able to use the nJets argument of the getRelUncert functions. ");         
//         }
//         else {
//           
//           //push back the 1-jet        
//           m_gluonFraction.push_back( histoGluonFractionNjet  );
//           
//           //loop on the others up to 7 jets
//           for(int i=2; i<8; i++){
//             njetstring = Form("_njet%d",i);
//             TH2D* h = (TH2D*)GetHisto(analysis_file,"gluonFraction"+suffix+njetstring,"MultijetJESUncertaintyProvider::init()");
//             
//             Fatal("MultijetJESUncertaintyProvider::init()","Don't know how to interpret %s",compName.Data());
// 
//             if(!h) std::cout << " MultijetJESUncertaintyProvide::initMultijet: ERROR input histogramm can't be found!" << std::endl;
//             //      std::cout << "MJES: read histogram " << h->GetName() << std::endl; 
//             h->SetName(TString("the")+"gluonFraction"+suffix+njetstring); 
//             h->SetDirectory(0);
//             m_gluonFraction.push_back(h);
//           }
//           
//         }
//         
        
        
        
	// Pull the correct gluon fraction error 2D histo
	m_gluonFractionError.push_back( (TH2D*)GetHisto(analysis_file,"gluonFractionError"+suffix,"MultijetJESUncertaintyProvider::init()") );
	m_gluonFractionError[0]->SetName(TString("the")+"gluonFractionError"+suffix); m_gluonFractionError[0]->SetDirectory(0);

        // Pull the correct gluon fraction 2D histo     
//         for(int i=0; i<8; i++){
//           TString njetstring = Form("_njet%d",i);
//           TH2D* h = (TH2D*)GetHisto(analysis_file,"gluonFraction"+suffix+njetstring,"MultijetJESUncertaintyProvider::init()");
//           if(!h) std::cout << " MultijetJESUncertaintyProvide::initMultijet: ERROR input histogramm can't be found!" << std::endl;
//           //      std::cout << "MJES: read histogram " << h->GetName() << std::endl; 
//           h->SetName(TString("the")+"gluonFraction"+suffix+njetstring); 
//           h->SetDirectory(0);
//           m_gluonFraction.push_back(h);
// 
//           // Pull the correct gluon fraction error 2D histo
//           TH2D* he = (TH2D*)GetHisto(analysis_file,"gluonFractionError"+suffix+njetstring,"MultijetJESUncertaintyProvider::init()");
//           he->SetName(TString("the")+"gluonFractionError"+suffix+njetstring); 
//           he->SetDirectory(0);
//           m_gluonFractionError.push_back(he); 
//         }

	// Pull the correct response graph
// 	m_responseSample = (TH2D*)GetHisto(analysis_file,"rSample"+suffix,"MultijetJESUncertaintyProvider::init()");
// 	m_responseSample->SetName(TString("the")+"rSample"+suffix); m_responseSample->SetDirectory(0);

      }
      else if ( compName.Contains("Close_By",TString::kIgnoreCase)) {

	Info( "MultijetJESUncertaintyProvider::init()", " %2d. %-15s : %s",index,compName.Data(),desc.Data()); 

	// Pull the correct deltaR graph
	m_deltaR = (TH2D*)GetHisto(multijet_file,"CloseBy"+suffix,"MultijetJESUncertaintyProvider::init()");
    	m_deltaR->SetName(TString("the")+"CloseBy"+suffix); m_deltaR->SetDirectory(0);
	
      }
      else if (compName.Contains("Flavor_Response",TString::kIgnoreCase) || 
	       compName.Contains("Flavour_Response",TString::kIgnoreCase)) {
	
	m_relGluonResponseUncert = settings.GetValue("RelativeGluonResponseUncertainty",0.02);
	Info( "MultijetJESUncertaintyProvider::init()", " %2d. %-15s : %s",index,compName.Data(),desc.Data()); 

	//Pull the correct flavor response graph (Herwig-Pythia gluon response)
	m_gluonResponseDifference = (TH2D*)GetHisto(multijet_file,"FlavorResponse"+suffix,"MultijetJESUncertaintyProvider::init()");
	m_gluonResponseDifference->SetName(TString("the")+"FlavorResponse"+suffix); m_gluonResponseDifference->SetDirectory(0);
	
      }
      else if ( compName.Contains("bJES_Response",TString::kIgnoreCase)) {
              
        Info( "MultijetJESUncertaintyProvider::init()", " %2d. %-15s : %s",index,compName.Data(),desc.Data()); 
              
        // Pull the correct bJES graph
        m_bJESResponse = (TH2D*)GetHisto(multijet_file,"bJES"+suffix,"MultijetJESUncertaintyProvider::init()");
        m_bJESResponse->SetName(TString("the")+"bJES"+suffix); m_bJESResponse->SetDirectory(0);
                                      
      }
                  
      else 
	Fatal("MultijetJESUncertaintyProvider::init()","Don't know how to interpret %s",compName.Data());
    }
    
    if (analysis_file) {
      analysis_file->Close();
      delete analysis_file;
    }
    
    multijet_file->Close();
    delete multijet_file;
    
    m_multiIsInit = true;

    Info( "MultijetJESUncertaintyProvider::init()", "======================================================");  
  }
  else {
    Warning( "MultijetJESUncertaintyProvider::init()", 
	     "WARNING: MultijetJESUncertaintyProvider already initialized, skipping re-initialization");
  }
  
  //go back to earlier directory 
  gDirectory = curdir;  
  
}

/*
 *  Method to access total uncertainty - excluding pile-up uncertainty
 */
double MultijetJESUncertaintyProvider::getRelUncert(double pT, double Eta, double dRmin, bool isUp, bool isBJet, unsigned int nJets) {

  // Add the uncertainties in quadrature
    
  return sqrt( pow(JESUncertaintyProvider::getRelUncert(pT,Eta),2) +
               pow(getRelFlavorUncert(pT,Eta,isUp,nJets),2)*int(!isBJet) +
               pow(getRelClosebyUncert(pT,dRmin),2) +
               pow(getRelBJESUncert(pT,Eta),2)*int(isBJet) );
  
  //this is equivalent to:
  /*if (!isBJet)
   *    return sqrt( pow(JESUncertaintyProvider::getRelUncert(pT,Eta),2) +
   *                 pow(getRelFlavorUncert(pT,Eta,isUp),2) +
   *                 pow(getRelClosebyUncert(pT,dRmin),2) );
   *    
   *  if (isBJet)
   *    return sqrt( pow(JESUncertaintyProvider::getRelUncert(pT,Eta),2) +
   *                 pow(getRelClosebyUncert(pT,dRmin),2) +
   *                 pow(getRelBJESUncert(pT,Eta),2)
   *               );
   */
  
}

/*
 *  Method to access total uncertainty - including pile-up uncertainty
 */
double MultijetJESUncertaintyProvider::getRelUncert(double pT, double Eta, double dRmin, bool isUp, double NPV, double mu, bool isBJet, unsigned int nJets) {
  return sqrt( pow(getRelUncert(pT,Eta,dRmin,isUp,isBJet,nJets),2) + pow(JESUncertaintyProvider::getRelOffsetUncert(pT,Eta,NPV,mu),2) );
}

/*
 *  The close-by uncertainty
 */
double MultijetJESUncertaintyProvider::getRelClosebyUncert(double pT, double dRmin) {
  static TString method = "MultijetJESUncertaintyProvider::getRelClosebyUncert()";
  static int nbadclose = 0;
  double minDRmin = m_deltaR->GetYaxis()->GetBinLowEdge(1);
  double maxDRmin = m_deltaR->GetYaxis()->GetBinLowEdge(m_deltaR->GetNbinsY()+1);
  
  if (m_deltaR==NULL) 
    Fatal("MultijetJESUncertaintyProvider::getRelClosebyUncert()","Close-by histogram not initialized");
  if (dRmin>=maxDRmin) return 0; //this happens in any case, both for R=0.4 and R=0.6
  if (dRmin<minDRmin) {
      
    TString msg=Form("dRMin < %.1f for jet of pT=%.1f GeV: dRMin=%.2f. Setting dRMin to %.1f.", 
		     minDRmin, pT/m_GeV, dRmin, minDRmin);
    if (++nbadclose==10) msg+=" Last warning!";
    if (nbadclose<=10) Warning(method,"%s",msg.Data());
    
    dRmin = minDRmin+1e-6;
  }//this is the same for R=0.4 and R=0.6 jets as the histograms dRMin vs pT both start there
  
  double pTMax = m_deltaR->GetXaxis()->GetBinUpEdge(m_deltaR->GetNbinsX())*m_GeV;
  double pTMin = m_deltaR->GetXaxis()->GetBinLowEdge(1)*m_GeV;//low edge of first bin
  
  if (pT>=pTMax) pT = pTMax-1e-6;
  if (pT<pTMin) pT = pTMin+1e-6;
  
  return m_deltaR->Interpolate(pT/m_GeV,dRmin);
}



/*
 *  Below: Flavor composition and flavor response uncertainty method and
 *         helper functions
 */
double MultijetJESUncertaintyProvider::getRelFlavorUncert(double pT, double eta, bool isUp, unsigned int nJets) {
  
  //as this parameter is here for later use, let's silence the compiler warning about it being unused
  (void)isUp;
  return sqrt( pow(getRelFlavorCompUncert(pT,eta,true,nJets),2) + pow(getRelFlavorResponseUncert(pT,eta,nJets),2) );
}


/*
 *   Returns the flavor composition uncertainty using the formula:
 *     
 *   Uncertainty = df * | Rq - Rg | / ( fg * Rg + (1 - fg) * Rq )
 *   with 
 *     Rs = fg * Rg + (1 - fg) * Rq as heavy q uncertainties accounted separately
 *     df = error on fg
 *     fg = fraction of gluons
 *     Rq = light quark response
 *     Rg = gluon response
 */

double MultijetJESUncertaintyProvider::getRelFlavorCompUncert(double pT, double eta, bool isUp, unsigned int nJets) {
  
  //as this parameter is here for later use, let's silence the compiler warning about it being unused
  (void)isUp;

  static TString method = "MultijetJESUncertaintyProvider::getRelFlavorCompUncert()";

  if (m_flavorGlu==NULL || m_flavorLight==NULL || m_gluonFraction[0]==NULL || m_gluonFractionError[0]==NULL) 
    Fatal(method.Data(),"Light quark/gluon response or gluon fraction/error input histograms not initialized. Check your setup!");
  
  //calculating the sample response:
  //fg*Rg + (1-fg)*Rq
  double gluonFrac = gluonFraction(pT,eta,nJets);
  double Rg = gluonResponse(pT,eta);
  double Rq = lightQuarkResponse(pT,eta);
    
  double Rsample = gluonFrac * Rg + (1-gluonFrac) * Rq;
  
  //this should never happen (it means the Rg == Rq == 0), but checking anyway
  if (Rsample==0) Fatal(method,"R(sample) = 0 for pT=%.1f GeV, eta=%.2f",pT/m_GeV,eta);

  //calculating the uncertainty
  double gluonFracError =  gluonFractionError(pT,eta,nJets);
  double flavorCompUnc = gluonFracError*fabs(Rq-Rg)/Rsample;

  return flavorCompUnc;
  
}


/*
 *  Assumption
 *    dR(q) = JES uncertainty (measured in gamma/Z+jet)
 *    dR(g) = JES uncertainty (+) additional gluon response component
 *  component to be added to JES uncertainty: 
 *      fg*dR(gluon response modeling uncertainty)
 *      where gluon response modeling uncertainty is taken as difference between gluon response in Pythia and Herwig++
 */
double MultijetJESUncertaintyProvider::getRelFlavorResponseUncert(double pT, double eta, unsigned int nJets) {
  static TString method = "MultijetJESUncertaintyProvider::getRelFlavorResponseUncert()";
    if (m_gluonResponseDifference==NULL || m_gluonFraction[0]==NULL) 
      Fatal(method.Data(),"Flavor response/fraction input histograms not initialized. Check your setup!");
    
    return gluonResponseDifference(pT,eta)*gluonFraction(pT,eta,nJets);
}


double MultijetJESUncertaintyProvider::getRelBJESUncert(double pT, double eta) {
  return bJESResponse(pT,eta);
}


double MultijetJESUncertaintyProvider::bJESResponse(double pT, double eta) {
  return getValue(m_bJESResponse,pT,eta);
}

double MultijetJESUncertaintyProvider::gluonResponseDifference(double pT, double eta) {
  return getValue(m_gluonResponseDifference,pT,eta);
}

double MultijetJESUncertaintyProvider::lightQuarkResponse(double pT, double eta) {
  return getValue(m_flavorLight,pT,eta);
}

double MultijetJESUncertaintyProvider::gluonResponse(double pT, double eta) {
  return getValue(m_flavorGlu,pT,eta);
}

double MultijetJESUncertaintyProvider::gluonFraction(double pT, double eta, unsigned int nJets) {
  
  if (nJets > m_maxNumJets) nJets = 0;
  TH2D* hGluonFraction = m_gluonFraction.at(nJets);
  if(!hGluonFraction) Fatal("MultijetJESUncertaintyProvider::gluonFraction", "Gluon fraction histogram not found for nJets=%d", nJets);
  return getValue(hGluonFraction,pT,eta);
  
}

double MultijetJESUncertaintyProvider::gluonFractionError(double pT, double eta, unsigned int nJets) {

  if (nJets > m_maxNumJets) nJets = 0;
  TH2D* hGluonFractionError = m_gluonFractionError.at(nJets);
  if(!hGluonFractionError) Fatal("MultijetJESUncertaintyProvider::gluonFractionError", "Gluon fraction error histogram not found for nJets=%d", nJets);
  return getValue(hGluonFractionError,pT,eta);
  
}

// Extrapolates across bins in a 2D histo
double MultijetJESUncertaintyProvider::getValue(const TH2 *h, double pt, double eta) {
  static const char* method = "MultijetJESUncertaintyProvider::getValue";
  static const char* msg = "Using closest valid value. (Only 10 first messages will be printed)";
  static int Nmsg = 0;
  double pT = pt/m_GeV;
  double minPt  = h->GetXaxis()->GetBinLowEdge(1);
  double maxPt  = h->GetXaxis()->GetBinLowEdge(h->GetNbinsX()+1);
  double maxEta = h->GetYaxis()->GetBinLowEdge(h->GetNbinsY()+1);
  if ( (pT<minPt || pT>maxPt) && ++Nmsg<=10 )
    Warning(method,"Jet pT=%.1f outside %.0f-%.0f range for %s.\n  %s",
	    pT,minPt,maxPt,h->GetName(),msg);
  if (pT<minPt) pT=minPt+0.001*m_GeV;
  if (pT>maxPt) pT=maxPt-0.001*m_GeV;
  if ( fabs(eta)>maxEta && ++Nmsg<=10 )
    Warning(method,"Jet eta=%.1f outside |eta|<%.0f for %s.\n  %s",
	    eta,maxEta,h->GetName(),msg);
  if ( fabs(eta) > maxEta ) 
    return ((TH2*)h)->Interpolate(pT,maxEta-1e-6);
  return ((TH2*)h)->Interpolate(pT,fabs(eta));
  
/*
  Using bins
  else
  {
    int ptbin = h->GetXaxis()->FindBin(pT);
    int etabin = h->GetYaxis()->FindBin(fabs(eta));
  
    if ( ptbin == 0 ) { ptbin=1;
      if (++nWarn<20) Warning(method,"Jet pT=%.1f GeV outside histogram range for %s. Using closest valid value.",
  			    pt/m_GeV,h->GetName());
    }
    if ( ptbin  > h->GetNbinsX()) { ptbin = h->GetNbinsX();
      if (++nWarn<20) Warning(method,"Jet pT=%.1f GeV outside histogram range for %s. Using closest valid value.",
  			    pt/m_GeV,h->GetName());
    }
    if ( etabin > h->GetNbinsY()) { etabin = h->GetNbinsY();
      if (++nWarn<20) Warning(method,"Jet eta=%.2f outside histogram range for %s. Using closest valid value.",
  			    eta,h->GetName());
    }
    return h->GetBinContent(ptbin,etabin);
  }
    */

}

// TH2 *MultijetJESUncertaintyProvider::MakeEmptyPtDRMinHisto(TString hname) {
//   //it is the responsibility of the user to delete this...
//   //Ŵorking on this next...
//   TH2* emptyDeltaRHisto = m_deltaR->Clone()
//   double ptbins[101], dx=(log(2500)-log(15))/100;
//   for (int i=0;i<=100;++i) ptbins[i]=exp(log(15)+i*dx);
//   double dRMinBins[] = {};//need to add sensible binning (using Interpolate earlier on, but no big deal up to 0.8/1.0)
//   int dRMinBins=sizeof(dRMinBins)/sizeof(double)-1;
//   return new TH2D(hname,"",100,ptbins,dRMinBins,dRMinBins);
// }
