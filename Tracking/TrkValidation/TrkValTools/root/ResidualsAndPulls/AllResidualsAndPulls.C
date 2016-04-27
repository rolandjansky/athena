/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TApplication.h"
#include "TFile.h"
#include "TTree.h"
#include "TF1.h"
#include "TGraphErrors.h"
#include "TProfile.h"
#include "TCanvas.h"
#include "TString.h"
#include "TAxis.h"
#include "TStyle.h"
#include "TLatex.h"
#include "TCut.h"

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <cmath>

int main(int argc, char **argv);

class BinData
{
public:
  BinData(int nbins, double xmax);
  BinData(int nbins, double xmin, double xmax);
  int nbins() const {return m_nbins;}
  double xmin() const {return m_xmin;}
  double xmax() const {return m_xmax;}
private:
  int m_nbins;
  double m_xmin;
  double m_xmax;
};


class FitParam
{
public:
  FitParam(double sigma = 0, double sigmaError = 0, double mean = 0, double meanError = 0, double rms = 0, double rmsError = 0);
  
  double sigma() const      {return m_sigma;}
  double sigmaError() const {return m_sigmaError;}
  double mean() const       {return m_mean;}
  double meanError() const  {return m_meanError;}
  double rms() const        {return m_rms;}
  double rmsError() const   {return m_rmsError;}
private:
  double m_sigma;
  double m_sigmaError;
  double m_mean;
  double m_meanError;
  double m_rms;
  double m_rmsError;
};


class ResidualsAndPulls
{
public:
  ResidualsAndPulls(TTree * tree);
  ~ResidualsAndPulls();

  void doPixel(const TCut & cuts);
  void doSCT(const TCut & cuts);
  void doTRT(const TCut & cuts);
  void drawPixel();
  void drawSCT();
  void drawTRT();

  void setNBins(int nbins) {m_nbins = nbins;}
  void setPullsFile(const std::string & filename);

  const FitParam & getFitParam(const std::string & name) const; 
  TH1 * getHist(const std::string & name) const;
  FitParam averageParam(const FitParam &paramA, const FitParam & paramB) const;

private:
  typedef std::map<std::string, FitParam> FitParamStore;
  void storeFitParam(const std::string & name, const FitParam & params); 
  void process(const std::string & name, const char * title, const char * var, const BinData & bins, const TCut & cuts);
  void writePulls(const std::string & name, const FitParam & param);

  FitParamStore m_paramStore;
  std::ofstream m_pullsFile;
  TTree * m_tree;
  int m_nbins;
};

ResidualsAndPulls::ResidualsAndPulls(TTree * tree)
  : m_tree(tree),
    m_nbins(100)
{} 

ResidualsAndPulls::~ResidualsAndPulls()
{
  m_pullsFile.close();
} 

void ResidualsAndPulls::setPullsFile(const std::string & filename)
{
  m_pullsFile.open(filename.c_str());
}

void
ResidualsAndPulls::doPixel(const TCut & cuts)
{

  TCut pixBarrelCut = "DetectorType==1 && abs(HitSurfaceZ)<400";
  TCut pixPosEcCut  = "DetectorType==1 && HitSurfaceZ>400";
  TCut pixNegEcCut  = "DetectorType==1 && HitSurfaceZ<-400";
   
  BinData pixResBinsLocX (m_nbins, 0.06);
  BinData pixResBinsLocY (m_nbins, 0.6);
  BinData pixPullBins (m_nbins, 3.5);

  process("pix_barrel_resX",    "Residual locX - Pixel Barrel",     "residualLocX", pixResBinsLocX, pixBarrelCut+cuts);
  process("pix_barrel_resY",    "Residual locY - Pixel Barrel",     "residualLocY", pixResBinsLocY, pixBarrelCut+cuts);
  process("pix_posendcap_resX", "Residual locX - Pixel Pos Endcap", "residualLocX", pixResBinsLocX, pixPosEcCut+cuts);
  process("pix_posendcap_resY", "Residual locY - Pixel Pos Endcap", "residualLocY", pixResBinsLocY, pixPosEcCut+cuts);
  process("pix_negendcap_resX", "Residual locX - Pixel Neg Endcap", "residualLocX", pixResBinsLocX, pixNegEcCut+cuts);
  process("pix_negendcap_resY", "Residual locY - Pixel Neg Endcap", "residualLocY", pixResBinsLocY, pixNegEcCut+cuts);

  process("pix_barrel_pullX",    "Pull locX - Pixel Barrel",     "pullLocX", pixPullBins, pixBarrelCut+cuts);
  process("pix_barrel_pullY",    "Pull locY - Pixel Barrel",     "pullLocY", pixPullBins, pixBarrelCut+cuts);
  process("pix_posendcap_pullX", "Pull locX - Pixel Pos Endcap", "pullLocX", pixPullBins, pixPosEcCut+cuts);
  process("pix_posendcap_pullY", "Pull locY - Pixel Pos Endcap", "pullLocY", pixPullBins, pixPosEcCut+cuts);
  process("pix_negendcap_pullX", "Pull locX - Pixel Neg Endcap", "pullLocX", pixPullBins, pixNegEcCut+cuts);
  process("pix_negendcap_pullY", "Pull locY - Pixel Neg Endcap", "pullLocY", pixPullBins, pixNegEcCut+cuts);

  FitParam pixbarX_param = getFitParam("pix_barrel_pullX");
  FitParam pixecAX_param = getFitParam("pix_posendcap_pullX");
  FitParam pixecCX_param = getFitParam("pix_negendcap_pullX");
  FitParam pixecX_param  = averageParam(pixecAX_param, pixecCX_param);
  FitParam pixbarY_param = getFitParam("pix_barrel_pullY");
  FitParam pixecAY_param = getFitParam("pix_posendcap_pullY");
  FitParam pixecCY_param = getFitParam("pix_negendcap_pullY");
  FitParam pixecY_param  = averageParam(pixecAY_param, pixecCY_param);

  writePulls("pixbarx", pixbarX_param);
  writePulls("pixecAx", pixecAX_param);
  writePulls("pixecCx", pixecCX_param);
  writePulls("pixecx" , pixecX_param);
  writePulls("pixbary", pixbarY_param);
  writePulls("pixecAy", pixecAY_param);
  writePulls("pixecCy", pixecCY_param);
  writePulls("pixecy" , pixecY_param);

}



void
ResidualsAndPulls::doSCT(const TCut & cuts)
{
  TCut sctBarrelCut = "DetectorType==2 && abs(HitSurfaceZ)<800";
  TCut sctPosEcCut  = "DetectorType==2 && HitSurfaceZ>800";
  TCut sctNegEcCut  = "DetectorType==2 && HitSurfaceZ<-800";
   
  BinData sctResBins  (m_nbins, 0.09);
  BinData sctPullBins (m_nbins, 3.5);

  process("sct_barrel_resX",    "Residual locX - SCT Barrel",     "residualLocX", sctResBins, sctBarrelCut+cuts);
  process("sct_posendcap_resX", "Residual locX - SCT Pos Endcap", "residualLocX", sctResBins, sctPosEcCut+cuts);
  process("sct_negendcap_resX", "Residual locX - SCT Neg Endcap", "residualLocX", sctResBins, sctNegEcCut+cuts);

  process("sct_barrel_pullX",    "Pull locX - SCT Barrel",     "pullLocX", sctPullBins, sctBarrelCut+cuts);
  process("sct_posendcap_pullX", "Pull locX - SCT Pos Endcap", "pullLocX", sctPullBins, sctPosEcCut+cuts);
  process("sct_negendcap_pullX", "Pull locX - SCT Neg Endcap", "pullLocX", sctPullBins, sctNegEcCut+cuts);

  FitParam sctbar_param = getFitParam("sct_barrel_pullX");
  FitParam sctecA_param = getFitParam("sct_posendcap_pullX");
  FitParam sctecC_param = getFitParam("sct_negendcap_pullX");
  FitParam sctec_param  = averageParam(sctecA_param, sctecC_param);

  writePulls("sctbar", sctbar_param);
  writePulls("sctecA", sctecA_param);
  writePulls("sctecC", sctecC_param);
  writePulls("sctec" , sctec_param);

}

void
ResidualsAndPulls::doTRT(const TCut & cuts)
{
  TCut trtBarrelCut = "DetectorType==3 && abs(HitSurfaceZ)<800";
  TCut trtPosEcCut  = "DetectorType==3 && HitSurfaceZ>800";
  TCut trtNegEcCut  = "DetectorType==3 && HitSurfaceZ<-800";
   
  BinData trtResBins  (m_nbins, 0.5);
  BinData trtPullBins (m_nbins, 3.5);

  process("trt_barrel_resX",    "Residual locX - TRT Barrel",     "residualLocX", trtResBins, trtBarrelCut+cuts);
  process("trt_posendcap_resX", "Residual locX - TRT Pos Endcap", "residualLocX", trtResBins, trtPosEcCut+cuts);
  process("trt_negendcap_resX", "Residual locX - TRT Neg Endcap", "residualLocX", trtResBins, trtNegEcCut+cuts);

  process("trt_barrel_pullX",    "Pull locX - TRT Barrel",     "pullLocX", trtPullBins, trtBarrelCut+cuts);
  process("trt_posendcap_pullX", "Pull locX - TRT Pos Endcap", "pullLocX", trtPullBins, trtPosEcCut+cuts);
  process("trt_negendcap_pullX", "Pull locX - TRT Neg Endcap", "pullLocX", trtPullBins, trtNegEcCut+cuts);

  FitParam trtbar_param = getFitParam("trt_barrel_pullX");
  FitParam trtecA_param = getFitParam("trt_posendcap_pullX");
  FitParam trtecC_param = getFitParam("trt_negendcap_pullX");
  FitParam trtec_param  = averageParam(trtecA_param, trtecC_param);

  writePulls("trtbar", trtbar_param);
  writePulls("trtecA", trtecA_param);
  writePulls("trtecC", trtecC_param);
  writePulls("trtec" , trtec_param);

}

void 
ResidualsAndPulls::process(const std::string & name, const char * title, const char * var, const BinData & bins, const TCut & cuts)
{
  std::cout << "Processing: " << name << std::endl;
  std::cout << " Cuts: " << cuts.GetTitle() << std::endl;

  TH1 * hist = new TH1F(name.c_str(), title, bins.nbins(), bins.xmin(), bins.xmax());
  m_tree->Project(name.c_str(), var, cuts.GetTitle());

  // Fit
  hist->Fit("gaus","0");
  TF1*   gaus        = hist->GetFunction("gaus");
  double sigma       = gaus->GetParameter(2);
  double sigmaError  = gaus->GetParError(2);
  double mean        = gaus->GetParameter(1);
  double meanError   = gaus->GetParError(1);
  double rms         = hist->GetRMS();
  double rmsError    = hist->GetRMSError();

  // make function visible again. This tip is in the src code TH1::Fit.
  gaus->ResetBit(TF1::kNotDraw);

  FitParam param(sigma,sigmaError,mean,meanError,rms,rmsError);
  storeFitParam(name, param);
  return;
}

void
ResidualsAndPulls::writePulls(const std::string & name, const FitParam & param)
{
  if (m_pullsFile) {
    m_pullsFile << name << " " 
		<< param.sigma() << " " 
		<< param.sigmaError() << " "
		<< param.rms() << " "  
		<< param.rmsError()  
		<< std::endl;
  }
}
 
FitParam 
ResidualsAndPulls::averageParam(const FitParam &paramA, const FitParam & paramB) const
{
  // Averaging of errors for +ve/-ve is not exact but assumes the errors are about the same 
  double sigma = 0.5 * (paramA.sigma() + paramB.sigma());
  double sigmaError = 0.5 * (paramA.sigmaError() + paramB.sigmaError()) / sqrt(2.) ;
  double mean = 0.5 * (paramA.mean() + paramB.mean());
  double meanError = 0.5 * (paramA.meanError() + paramB.meanError()) / sqrt(2.) ;
  double rms = 0.5 * (paramA.rms() + paramB.rms());
  double rmsError = 0.5 * (paramA.rmsError() + paramB.rmsError()) / sqrt(2.) ;
  return FitParam(sigma,sigmaError, mean, meanError, rms, rmsError);
}



void
ResidualsAndPulls::drawPixel()
{
  TH1 * htmp = 0;
  const int iCanvX = 100;
  const int iCanvY = 100;
  const int iCanvWidth = 800;
  const int iCanvHeight= 720;

  // BARREL SECTION -----------------------------------
  TCanvas* pixBarrelCanv = new TCanvas("pixBarrelCanv", "Pixel Barrel", iCanvX, iCanvY, iCanvWidth, iCanvHeight);
  pixBarrelCanv->Divide(2,2);    
  
  // ------------------------------------------------- 
  pixBarrelCanv->cd(1);
  htmp = getHist("pix_barrel_resX");
  htmp->Draw();
  // ------------------------------------------------- 
  pixBarrelCanv->cd(2);
  htmp = getHist("pix_barrel_pullX");
  htmp->Draw();
  // ------------------------------------------------- 
  pixBarrelCanv->cd(3);
  htmp = getHist("pix_barrel_resY");
  htmp->Draw();
  // ------------------------------------------------- 
  pixBarrelCanv->cd(4);
  htmp = getHist("pix_barrel_pullY");
  htmp->Draw();
    

  // ENDCAP SECTION A: positive EC -----------------------------------
  TCanvas* pixPosEndcapCanv = new TCanvas("pixPosEndcapCanv", "Pixel Positive Endcap", iCanvX+20, iCanvY+20,  iCanvWidth, iCanvHeight);
  pixPosEndcapCanv->Divide(2,2);    
  // ------------------------------------------------- 
  pixPosEndcapCanv->cd(1);
  htmp = getHist("pix_posendcap_resX");
  htmp->Draw();
  // ------------------------------------------------- 
  pixPosEndcapCanv->cd(2);
  htmp = getHist("pix_posendcap_pullX");
  htmp->Draw();
  // ------------------------------------------------- 
  pixPosEndcapCanv->cd(3);
  htmp = getHist("pix_posendcap_resY");
  htmp->Draw();
  // ------------------------------------------------- 
  pixPosEndcapCanv->cd(4);
  htmp = getHist("pix_posendcap_pullY");
  htmp->Draw();

 
  // ENDCAP SECTION C: negative EC -----------------------------------
  TCanvas* pixNegEndcapCanv = new TCanvas("pixNegEndcapCanv", "Pixel Negative Endcap", iCanvX+40, iCanvY+40,  iCanvWidth, iCanvHeight);
  pixNegEndcapCanv->Divide(2,2);    
  // ------------------------------------------------- 
  pixNegEndcapCanv->cd(1);
  htmp = getHist("pix_negendcap_resX");
   htmp->Draw();
  // ------------------------------------------------- 
  pixNegEndcapCanv->cd(2);
  htmp = getHist("pix_negendcap_pullX");
  htmp->Draw();
  // ------------------------------------------------- 
  pixNegEndcapCanv->cd(3);
  htmp = getHist("pix_negendcap_resY");
  htmp->Draw();
  // ------------------------------------------------- 
  pixNegEndcapCanv->cd(4);
  htmp = getHist("pix_negendcap_pullY");
  htmp->Draw();
  

}    
       
void
ResidualsAndPulls::drawSCT() 
{
  TH1 * htmp = 0;
  const int iCanvX = 180;
  const int iCanvY = 180;
  const int iCanvWidth = 800;
  const int iCanvHeight= 360;

  // BARREL SECTION -----------------------------------
  TCanvas* sctBarrelCanv = new TCanvas("sctBarrelCanv", "SCT Barrel", iCanvX, iCanvY, iCanvWidth, iCanvHeight);
  sctBarrelCanv->Divide(2,1);    
  
  // ------------------------------------------------- 
  sctBarrelCanv->cd(1);
  htmp = getHist("sct_barrel_resX");
  htmp->Draw();
  // ------------------------------------------------- 
  sctBarrelCanv->cd(2);
  htmp = getHist("sct_barrel_pullX");
  htmp->Draw();

  // ENDCAP SECTION A: positive EC -----------------------------------
  TCanvas* sctPosEndcapCanv = new TCanvas("sctPosEndcapCanv", "SCT Positive Endcap", iCanvX+20, iCanvY+20, iCanvWidth, iCanvHeight);
  sctPosEndcapCanv->Divide(2,1);    
  // ------------------------------------------------- 
  sctPosEndcapCanv->cd(1);
  htmp = getHist("sct_posendcap_resX");
  htmp->Draw();
  // ------------------------------------------------- 
  sctPosEndcapCanv->cd(2);
  htmp = getHist("sct_posendcap_pullX");
  htmp->Draw();
 
  // ENDCAP SECTION C: negative EC -----------------------------------
  TCanvas* sctNegEndcapCanv = new TCanvas("sctNegEndcapCanv", "SCT Negative Endcap", iCanvX+40, iCanvY+40, iCanvWidth, iCanvHeight);
  sctNegEndcapCanv->Divide(2,1);    
  // ------------------------------------------------- 
  sctNegEndcapCanv->cd(1);
  htmp = getHist("sct_negendcap_resX");
   htmp->Draw();
  // ------------------------------------------------- 
  sctNegEndcapCanv->cd(2);
  htmp = getHist("sct_negendcap_pullX");
  htmp->Draw();

}  
void ResidualsAndPulls::drawTRT() {
  TH1 * htmp = 0;
  const int iCanvX = 260;
  const int iCanvY = 260;
  const int iCanvWidth = 800;
  const int iCanvHeight= 360;

  // BARREL SECTION -----------------------------------
  TCanvas* trtBarrelCanv = new TCanvas("trtBarrelCanv", "TRT Barrel", iCanvX, iCanvY, iCanvWidth, iCanvHeight);
  trtBarrelCanv->Divide(2,1);    
  
  // ------------------------------------------------- 
  trtBarrelCanv->cd(1);
  htmp = getHist("trt_barrel_resX");
  htmp->Draw();
  // ------------------------------------------------- 
  trtBarrelCanv->cd(2);
  htmp = getHist("trt_barrel_pullX");
  htmp->Draw();

  // ENDCAP SECTION A: positive EC -----------------------------------
  TCanvas* trtPosEndcapCanv = new TCanvas("trtPosEndcapCanv", "TRT Positive Endcap", iCanvX+20, iCanvY+20, iCanvWidth, iCanvHeight);
  trtPosEndcapCanv->Divide(2,1);    
  // ------------------------------------------------- 
  trtPosEndcapCanv->cd(1);
  htmp = getHist("trt_posendcap_resX");
  htmp->Draw();
  // ------------------------------------------------- 
  trtPosEndcapCanv->cd(2);
  htmp = getHist("trt_posendcap_pullX");
  htmp->Draw();
 
  // ENDCAP SECTION C: negative EC -----------------------------------
  TCanvas* trtNegEndcapCanv = new TCanvas("trtNegEndcapCanv", "TRT Negative Endcap", iCanvX+40, iCanvY+40, iCanvWidth, iCanvHeight);
  trtNegEndcapCanv->Divide(2,1);    
  // ------------------------------------------------- 
  trtNegEndcapCanv->cd(1);
  htmp = getHist("trt_negendcap_resX");
  htmp->Draw();
  // ------------------------------------------------- 
  trtNegEndcapCanv->cd(2);
  htmp = getHist("trt_negendcap_pullX");
  htmp->Draw();

}  


const FitParam & 
ResidualsAndPulls::getFitParam(const std::string & name) const
{
  static FitParam defParam;
  FitParamStore::const_iterator iter;
  iter = m_paramStore.find(name);
  if (iter ==  m_paramStore.end()) {
    std::cout << "ERROR: No parameters stored with key: " << name << std::endl;
    return defParam;
  }
  return iter->second;
}

TH1 * ResidualsAndPulls::getHist(const std::string & name) const
{
  return (TH1F*)gDirectory->Get(name.c_str());
}  

void 
ResidualsAndPulls::storeFitParam(const std::string & name, const FitParam & params)
{
  m_paramStore[name] = params;
}

BinData::BinData(int nbins, double xmax)
  : m_nbins(nbins)
{
  m_xmax = std::abs(xmax);
  m_xmin = -m_xmax;
}
BinData::BinData(int nbins, double xmin, double xmax)
  : m_nbins(nbins), 
    m_xmin(xmin),
    m_xmax(xmax)
{}

FitParam::FitParam(double sigma, double sigmaError, double mean, double meanError, double rms, double rmsError)
  : m_sigma(sigma),
    m_sigmaError(sigmaError),
    m_mean(mean),
    m_meanError(meanError),
    m_rms(rms),
    m_rmsError(rmsError)
{}


////////////////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{

  bool batchmode = false;
  for (int i=0; i < argc; i++) {
    std::string opt =  argv[i];
    if (opt == "-b") batchmode = true;
  }
  

  // Will recognize -b and strip it out.
  TApplication theApp("App", &argc, argv);
 

//================ Trying out the style ===================/

  TStyle *plain  = new TStyle("Plain","Plain Style (no colors/fill areas)");
  // Canvas
  plain->SetCanvasBorderMode(0);
  plain->SetCanvasColor(0);
  // Pad
  plain->SetPadBorderMode(0);
  plain->SetPadColor(0);
  // Title
  plain->SetTitleColor(0);
  plain->SetTitleBorderSize(0);
  plain->SetTitleX(0.25);
  plain->SetTitleY(0.98);
  // Stat
  plain->SetStatColor(0);
  plain->SetOptStat(11111);
  plain->SetOptFit(111);
  // Labelling
  plain->SetLabelFont(132, "XY");
  plain->SetLabelSize(0.05, "XY");
  
  gROOT->SetStyle("Plain");

// =======================================================/

  if (argc > 4) {

    int     bins   = 0;
    int     pcut   = 500;
    int     etacut = 300;
    
    TString filename = argv[1]; 
    filename        += ".root";
    
    std::cout << "[m] Filename       : " << filename << std::endl;  

    TString dirname  = argv[2];
    TString treename = argv[3];

    std::cout << "[m] Directory/Tree : " << dirname << "/"<< treename << std::endl;  

    bins = atoi(argv[4]);

    std::cout << "[m] Histogram bins : " << bins << std::endl;  


    if (argc >5) 
       pcut = atoi(argv[5]);

    double qOverPcut = double(1./pcut);

    TString qOverPLimit = "abs(RecQoverP) <";
            qOverPLimit += qOverPcut; 

    std::cout << "[m] cut q/p        : " << qOverPLimit << std::endl;  

    if (argc >6) 
       etacut = atoi(argv[6]);

    double etaCut = 0.01*double(etacut);

    TString etaLimit  = "abs(RecEta) <";
            etaLimit += etaCut; 

    std::cout << "[m] cut eta        : " << etaLimit << std::endl;  


    // TRT drifttime cut. Does not effect Pixel and SCT cuts
    TString driftTimeCut = "";
    if (argc > 7) {
      TString t7 = argv[7]; // needed for following string comparison
      if (t7 == "Drifttime") {
	driftTimeCut = "residualLocX/pullLocX < 1.0";
        std::cout << "[m] selecting only TRT hits with drift time meas't" << std::endl;
      } else if (t7 == "NoDrifttime") {
	driftTimeCut = "residualLocX/pullLocX > 1.0";
        std::cout << "[m] selecting only TRT hits with drift time ignored by tracking" << std::endl;
      } else {
        std::cout << "[m] WARNING drift time selection argument not understood, not doing anything!" << std::endl;
      }
    } 


    TCut cuts = TCut(qOverPLimit) + TCut(etaLimit);
    TCut trtcuts = cuts + TCut(driftTimeCut);

    // load the File
    TFile* inputFile = TFile::Open(filename);
    
    if (inputFile){
      
      // construct the full file name 
      TString treefullname = dirname;
      treefullname += "/";
      treefullname += treename;

      TTree* tree = (TTree*)gDirectory->Get(treefullname);

      if (tree){

         std::cout << "[m] Tree " << treefullname << " loaded. " << std::endl;

	 // Open Pulls file.

	 ResidualsAndPulls r(tree);
	 r.setPullsFile("pulls.txt");
	 r.setNBins(bins);

	 r.doPixel(cuts);
	 r.doSCT(cuts);
	 r.doTRT(trtcuts);

	 r.drawPixel();
	 r.drawSCT();
	 r.drawTRT();
	 
      }	   
    }
    
  } else {
    std::cout << "[e] Not enough arguments given! " << std::endl;
    std::cout << "[i] Arguments required: filename [-b] (-.root), Directory, Tree, bins, pcut [MeV] , etacut [eta/100]" << std::endl;
    std::cout << "[i] -b: run in batch mode" << std::endl;
    std::cout << "[i] sample usage for a File 'ID_rec.root' from standard InDetRecExample : " << std::endl;
    std::cout << std::endl;
    std::cout << "   AllResidualsAndPulls ID_rec FitterValidation OutputTrack 65 1000 250 [(No)Drifttime]" << std::endl;
    std::cout << std::endl;
  }
  
  if (!batchmode) theApp.Run();
  return(0);
}
