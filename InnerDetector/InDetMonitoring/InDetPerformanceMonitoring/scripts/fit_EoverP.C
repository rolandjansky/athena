/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __CINT__

#include "RooGlobalFunc.h"
#endif
#include "RooRealVar.h"
#include "RooDataSet.h"
#include "RooAbsData.h"
#include "RooDataHist.h"
#include "RooGaussian.h"
#include "RooLandau.h"
#include "RooChebychev.h"
#include "RooAddPdf.h"
#include "RooExtendPdf.h"
#include "RooCBShape.h"
#include "RooNovosibirsk.h"
#include "RooFFTConvPdf.h"
#include "RooExponential.h"
#include "RooSimultaneous.h"
#include "RooPolynomial.h"
#include "RooFitResult.h"
#include "RooCategory.h"
#include "RooBinning.h"
#include "RooWorkspace.h"
#include "RooChi2Var.h" 
#include "RooMinuit.h"
#include "RooFormulaVar.h"
#include "RooPlot.h"



#include<TFile.h>
#include<TH2F.h>
#include<TH3F.h>
#include<TH1D.h>
#include<TF1.h>
#include<TMath.h>
#include<TTree.h>
#include<TChain.h>
#include<TColor.h>
#include<TGraphErrors.h>
#include<TCanvas.h>
#include<TProfile.h>
#include<iostream>
#include <iomanip>
#include <cmath>

#include "TROOT.h"
#include "THStack.h"
#include "AtlasStyle.C"

using namespace RooFit ;


void makeSuperAwesomePlots( RooAbsData* originalDataSet, 
                                 RooRealVar& eOverP, 
                                 RooRealVar& eta, 
                                 int nBinsEta,  
                                 RooRealVar& phi, 
                                 int nBinsPhi,
                                 TString extraCuts = "",
                                 RooRealVar* Et = NULL );


void makeSuperAwesomePlotsEtaPt( RooAbsData* originalDataSet, 
                                 RooRealVar& eOverP, 
                                 RooRealVar& eta, 
                                 int nBinsEta,  
                                 RooRealVar& Pt, 
                                 int nBinsPt);                                 

void makeSuperAwesomePlotsPhiPt( RooAbsData* originalDataSet, 
                                 RooRealVar& eOverP, 
                                 RooRealVar& phi, 
                                 int nBinsEta,  
                                 RooRealVar& Pt, 
                                 int nBinsPt);

void makeSuperAwesomePlots( RooAbsData* originalDataSet, 
                            RooRealVar& eOverP,
                            RooRealVar& eta, 
                            RooRealVar& phi, 
                            TH2F* meanPlotPos,
                            TH2F* meanPlotNeg,
                            TString additionalCut = ""
                          );


void makeSuperAwesomePlots( RooRealVar& xvar, 
                            RooRealVar& yvar,                                 
                            TH3F* dataPlotPos,
                            TH3F* dataPlotNeg,
                            TH2F* meanPlotPos,
                            TH2F* meanPlotNeg);

TH2F* getCorrections();

double  GetCorrection(TH2F* corrections, double eta, double phi, double Et, double charge);

void  makeEtMeans(RooDataSet* dataSet,RooRealVar& Et,RooRealVar& ClusterEta,int nBinsEats);

                                 
void makeAwesomeSlicePlots(TH2F* pos, TH2F* neg, bool sliceInEta);

void ratioPlots(TH1D* h1, TH1D* h2, TString name);

void MakeScaling(TString inputDir="");


//TString PATH = "./CA_TRTL3Iter4-4Dv3_2011/";
TString PATH = "./Results/";

int main(){
  return 0;
}

void fit_EoverP(int netabins=12, int nphibins=12)
{

  gROOT->LoadMacro("AtlasStyle.C");
  SetAtlasStyle();
   
  RooRealVar ClusterEnergy("ClusterEnergy","ClusterEnergy",0,1e10,"MeV") ;
  RooRealVar ClusterPhi("ClusterPhi","#phi",0);
  RooRealVar ClusterEta("ClusterEta","#eta",0);
  RooRealVar charge("charge","charge",0);
  RooRealVar TrackTheta("TrackTheta","TrackTheta",0);
  RooRealVar TrackQoverP("QoverP","QoverP",-1e10,1e10,"MeV^{-1}");
  RooRealVar EoverP("EoverP","E/p",0,5,"");
  RooRealVar Et("Et","E_{T}",0,1e10,"GeV");
  RooRealVar Eta("Eta","#eta",-3,3,"");
  
  RooArgSet myarg;
  myarg.add(ClusterEnergy);
  myarg.add(ClusterPhi);
  myarg.add(ClusterEta);
  myarg.add(charge);
  myarg.add(TrackTheta);
  myarg.add(TrackQoverP);
  myarg.add(EoverP);
  myarg.add(Et);
  myarg.add(Eta);
 
  TChain* eoverpTree = new TChain("EGrefitterSmall");
  //eoverpTree->Add("./Wenu-EndcapShifts.root");
  eoverpTree->Add("../run/215414/Level2/L2-alignment.root");
  
  
  Double_t         temp_ClusterEnergy;
  Double_t         temp_ClusterPhi;
  Double_t         temp_ClusterEta;
  Double_t         temp_charge;
  Double_t         temp_TrackTheta;
  Double_t         temp_QoverP;
  
  eoverpTree->SetBranchAddress("ClusterEnergy", &temp_ClusterEnergy);
  eoverpTree->SetBranchAddress("ClusterPhi", &temp_ClusterPhi);
  eoverpTree->SetBranchAddress("ClusterEta", &temp_ClusterEta);
  eoverpTree->SetBranchAddress("charge", &temp_charge);
  eoverpTree->SetBranchAddress("TrackTheta", &temp_TrackTheta);  
  eoverpTree->SetBranchAddress("Refitted2_QoverP", &temp_QoverP);
  
  RooDataSet* dataSet = new RooDataSet("DataSet","DataSet",myarg);
  
  
  TH2F* CORRECTIONS=getCorrections();
  
  if(!CORRECTIONS){
    std::cout << "INFO :: Getting Correction" << std::endl;
    TFile f("FinalCorrections.root");
    CORRECTIONS =(TH2F*) f.Get("FinalCorrections");
    if(!CORRECTIONS)
      return;
    else{
    
      std::cout << "INFO :: Got Corrections" << std::endl;      
      CORRECTIONS->Print();
      int binNumber = CORRECTIONS->FindBin(1, 1);
      std::cout << "INFO :: Got Corrections "<< CORRECTIONS->GetBinContent(binNumber) << std::endl;      
    
    }
  }

  // loop on events
  for (int i=0; i< eoverpTree->GetEntries(); i++) {
    eoverpTree->GetEntry(i);
        
    charge.setVal(temp_charge);
    Et.setVal(temp_ClusterEnergy*sin(temp_TrackTheta)*1e-3);
    Eta.setVal(-log(tan(temp_TrackTheta/2)) );
    ClusterPhi.setVal(temp_ClusterPhi);
    ClusterEnergy.setVal(temp_ClusterEnergy);
    ClusterEta.setVal(temp_ClusterEta);

    int binNumber = CORRECTIONS->FindBin(temp_ClusterEta, temp_ClusterPhi);
    double delta = CORRECTIONS->GetBinContent(binNumber); 
  
    double correction =  1.; //1.+temp_charge*Et.getVal()*delta;  
    
    if (false) std::cout << "delta " << delta << " correction " << correction << std::endl; 
    
    //double correction  = GetCorrection(CORRECTIONS,temp_ClusterEta,temp_ClusterPhi,temp_ClusterEnergy*sin(temp_TrackTheta)*1e-3,temp_charge);
    EoverP.setVal(fabs(temp_QoverP)*temp_ClusterEnergy * correction);
    //if(i<10) std::cout << EoverP.getVal() <<std::endl;
    // store info
    dataSet->add(myarg);
  }
  
  TCanvas c;
  c.Print(PATH+"test.pdf(");

  makeSuperAwesomePlots( dataSet, EoverP, ClusterEta, netabins, ClusterPhi, nphibins ); 
  //makeSuperAwesomePlots( dataSet, EoverP, ClusterEta, 25, ClusterPhi, 1, "&& ClusterPhi>0" ); 
  //makeSuperAwesomePlots( dataSet, EoverP, ClusterEta, 25, ClusterPhi, 1, "&& ClusterPhi<0" ); 
  //makeEtMeans( dataSet, Et, ClusterEta, 16 ); 
  //makeSuperAwesomePlotsEtaPt( dataSet, EoverP, ClusterEta, 50, Et, -1 );   
  //makeSuperAwesomePlotsPhiPt( dataSet, EoverP, ClusterPhi, -1, Et, -1 ); 
  
  c.Print(PATH+"test.pdf)");

                                  
}



double  GetCorrection(TH2F* CORRECTIONS, double eta, double phi, double Et,double charge)
{
  //Q/p_constr=Q/p_recon*(1 + pT_recon*Q*delta)
  if(!CORRECTIONS)
    return 1;

  
  std::cout << "INFO :: GetCorrection -> Bin number "; //<< std::endl;
  
  int binNumber = CORRECTIONS->FindBin(eta, phi);

  std::cout<< binNumber << std::endl;
  

  std::cout << "INFO :: GetCorrection -> delta" << std::endl;
  double delta = CORRECTIONS->GetBinContent(binNumber); 
  
  return 1+charge*Et*delta;  
}


void  makeEtMeans(RooDataSet* dataSet,RooRealVar& Et,RooRealVar& ClusterEta,int nBinsEta)
{

  RooBinning etaBinning(-2.5,2.5);
  etaBinning.addUniform(nBinsEta,-2.5,2.5);
  
	ClusterEta.setBinning(etaBinning);
 
 
  RooBinning etBinning(20,100);
  etBinning.addUniform(80,20,100);
  
	Et.setBinning(etBinning);
 
  TH2F*	myHistPos = dataSet->createHistogram(ClusterEta, Et, nBinsEta, 100,"charge > 0 ", "positive ET hit");
  myHistPos->GetXaxis()->SetTitle("#eta");
  myHistPos->GetYaxis()->SetTitle("e^{+} E_{T}  [GeV]");
  
  TH2F*	myHistNeg = dataSet->createHistogram(ClusterEta, Et, nBinsEta, 100,"charge < 0 ", "negative ET hit");
  myHistNeg->GetXaxis()->SetTitle("#eta");
  myHistNeg->GetYaxis()->SetTitle("e^{-} E_{T} [GeV]");

  TCanvas* c =  new TCanvas();
  const UInt_t Number = 3;
  Double_t Green[Number]  = { 0.00, 1.00, 0.00};
  Double_t Red[Number]    = { 1.00, 0.10, 0.00};
  Double_t Blue[Number]   = { 0.10, 0.10, 1.00};
  Double_t Length[Number] = { 0.00, 0.50, 1.00 };
  Int_t nb=50;
  TColor::CreateGradientColorTable(Number,Length,Red,Green,Blue,nb);
  myHistPos->SetContour(nb);
  c->SetRightMargin(0.12);
  c->SetLogy(false);
  myHistPos->Draw("colz");

  TCanvas* c2 =  new TCanvas();
  c2->cd();
  TProfile* prof  = myHistPos->ProfileX("_pfx");
  prof->Draw();
  prof->GetYaxis()->SetTitle("e^{+} <E_{T}> [GeV]");

  TCanvas* c3 =  new TCanvas();
  c3->cd();
  TProfile* prof2  = myHistNeg->ProfileX("_pfx2");
  prof2->Draw();
  prof2->GetYaxis()->SetTitle("e^{-} <E_{T}> [GeV]");

}



void makeSuperAwesomePlots( RooAbsData* originalDataSet, 
                                 RooRealVar& eOverP, 
                                 RooRealVar& eta, 
                                 int nBinsEta,  
                                 RooRealVar& phi, 
                                 int nBinsPhi,
                                 TString extraCuts,
                                 RooRealVar* Et )
{
 
  
  
  // Set up ranges
  double etaRange[100];
  if (nBinsEta == -1){
    etaRange[0] = -2.5;
    etaRange[1] = -2.2;
    etaRange[2] = -2.0;
    etaRange[3] = -1.8;
    etaRange[4] = -1.6;
    etaRange[5] = -1.4;
    etaRange[6] = -1.2;
    etaRange[7] = -1.0;
    etaRange[8] = -0.8;
    etaRange[9] = -0.6;
    etaRange[10] = -0.4;
    etaRange[11] = -0.2;
    etaRange[12] = -0.0;
    etaRange[24] = 2.5;
    etaRange[23] = 2.2;
    etaRange[22] = 2.0;
    etaRange[21] = 1.8;
    etaRange[20] = 1.6;
    etaRange[19] = 1.4;
    etaRange[18] = 1.2;
    etaRange[17] = 1.0;
    etaRange[16] = 0.8;
    etaRange[15] = 0.6;
    etaRange[14] = 0.4;
    etaRange[13] = 0.2;
    nBinsEta = 22;
  } else {
    double stepSize = 5/(double)nBinsEta;
    for (int  i(0); i<= nBinsEta; ++i){
      etaRange[i] = -2.5+stepSize*(double)i;
    }
  }

  double phiRange[102];
  double stepSize = 2 * TMath::Pi()/(double)nBinsPhi;
  for (int  i(0); i<= nBinsPhi; ++i){
    phiRange[i] = -TMath::Pi()+stepSize*(double)i;
  }
  
  
  TH2F *meanPlotPos = new TH2F("meanPlotPos"+extraCuts,"meanPlotPos"+extraCuts,nBinsEta,etaRange,nBinsPhi,phiRange ); 
  meanPlotPos->GetXaxis()->SetTitle("#eta");
  meanPlotPos->GetYaxis()->SetTitle("#phi [rad]");
  TH2F *meanPlotNeg = new TH2F("meanPlotNeg"+extraCuts,"meanPlotNeg"+extraCuts,nBinsEta,etaRange,nBinsPhi,phiRange );
  meanPlotNeg->GetXaxis()->SetTitle("#eta");
  meanPlotNeg->GetYaxis()->SetTitle("#phi [rad]");
  

  
  //RooAbsData* smallAbsDataSetPos = originalDataSet->reduce( Cut("charge > 0"+ extraCuts) );
  //RooDataSet* smallDataSetPos = dynamic_cast<RooDataSet*>( smallAbsDataSetPos );
  //RooAbsData* smallAbsDataSetNeg = originalDataSet->reduce( Cut("charge < 0"+ extraCuts)) ;
  //RooDataSet* smallDataSetNeg = dynamic_cast<RooDataSet*>( smallAbsDataSetNeg );

  makeSuperAwesomePlots(originalDataSet, 
                        eOverP,
                        eta,
                        phi,
                        meanPlotPos,
                        meanPlotNeg,
                        extraCuts);

  TFile f(PATH+"fitEoverP.root","Recreate");
  meanPlotPos->Write();
  meanPlotNeg->Write();
  
  
  if(Et != NULL ){ 
    double ptRange[102];
    stepSize = 1500;
    for (int  i(0); i<= 100; ++i){
      ptRange[i] = 20e3+stepSize*(double)i;
    }
    
    double eopRange[102];
    stepSize = 0.05;
    for (int  i(0); i<= 100; ++i){
      eopRange[i] = 0.8+stepSize*(double)i;
    }
 
    TH2F *ptvsEta =  new TH2F("meanPtVsEta"+extraCuts,"meanPtVsEta"+extraCuts,nBinsEta,etaRange,100,ptRange);
    TH2F *eoverPvsEta =  new TH2F("eoverPvsEta"+extraCuts,"eoverPvsEta"+extraCuts,nBinsEta,etaRange,100,eopRange);
    
    
    for(int i=0; i < originalDataSet->numEntries(); i++)
    {
      originalDataSet->get(i);         
      double valEt     = Et->getVal();
      double valeOverP = eOverP.getVal(); 
      double valEta    = eta.getVal();
      
      ptvsEta->Fill(valEta,valEt);
      eoverPvsEta->Fill(valEta,valeOverP);

    }
  
    ptvsEta->Write();
    eoverPvsEta->Write();
    
  }
  
  
  
  f.Write();
  f.Close();   
}

void makeSuperAwesomePlotsEtaPt( RooAbsData* originalDataSet, 
                                 RooRealVar& eOverP, 
                                 RooRealVar& eta, 
                                 int nBinsEta,  
                                 RooRealVar& Pt, 
                                 int nBinsPt)
{
 
  // Set up ranges
  double etaRange[100];
  if (nBinsEta == -1){
    etaRange[0] = -2.5;
    etaRange[1] = -1.5;
    etaRange[2] = -0.8;
    etaRange[3] =  0.;
    etaRange[4] =  0.8;
    etaRange[5] =  1.5;
    etaRange[6] =  2.5;
    nBinsEta = 6;
  } else {
    double stepSize = 5/(double)nBinsEta;
    for (int  i(0); i<= nBinsEta; ++i){
      etaRange[i] = -2.5+stepSize*(double)i;
    }
  }

  double ptRange[100];
  if (nBinsPt == -1){
    ptRange[0] = 20;
    ptRange[1] = 30;
    ptRange[2] = 40;
    ptRange[3] = 50;
    ptRange[4] = 60;
    ptRange[5] = 100;
    ptRange[6] = 200;
    nBinsPt = 5;
  } else {
    double stepSize = 100./(double)nBinsPt;
    for (int  i(0); i<= nBinsPt; ++i){
      ptRange[i] = 20+stepSize*(double)i;
    }
  }
    
  TH2F *meanPlotPos = new TH2F("meanPlotPosPt","meanPlotPosPt",nBinsEta,etaRange,nBinsPt,ptRange); 
  meanPlotPos->GetXaxis()->SetTitle("#eta");
  meanPlotPos->GetYaxis()->SetTitle("E_{T} [GeV]");
  TH2F *meanPlotNeg = new TH2F("meanPlotNegPt","meanPlotNegPt",nBinsEta,etaRange,nBinsPt,ptRange);
  meanPlotNeg->GetXaxis()->SetTitle("#eta");
  meanPlotNeg->GetYaxis()->SetTitle("E_{T} [GeV]");
  
  
  makeSuperAwesomePlots(originalDataSet, 
                        eOverP,
                        eta,
                        Pt,
                        meanPlotPos,
                        meanPlotNeg );
                        
  TFile f(PATH+"fitEoverP_pt.root","Recreate");
  meanPlotPos->Write();
  meanPlotNeg->Write();
  f.Write();
  f.Close();   
}


void makeSuperAwesomePlotsPhiPt( RooAbsData* originalDataSet, 
                                 RooRealVar& eOverP, 
                                 RooRealVar& phi, 
                                 int nBinsPhi,  
                                 RooRealVar& Pt, 
                                 int nBinsPt)
{
 
  // Set up ranges
  double phiRange[100];
  if (nBinsPhi == -1){
    nBinsPhi = 8;
  } 

  double stepSize = 2*TMath::Pi()/(double)nBinsPhi;
  for (int  i(0); i<= nBinsPhi; ++i){
    phiRange[i] = -TMath::Pi()+stepSize*(double)i;
  }

  double ptRange[100];
  if (nBinsPt == -1){
    ptRange[0] = 20;
    ptRange[1] = 30;
    ptRange[2] = 40;
    ptRange[3] = 50;
    ptRange[4] = 60;
    ptRange[5] = 100;
    ptRange[6] = 200;
    nBinsPt = 5;
  } else {
    stepSize = 100./(double)nBinsPt;
    for (int  i(0); i<= nBinsPt; ++i){
      ptRange[i] = 20+stepSize*(double)i;
    }
  }
    
  TH2F *meanPlotPos = new TH2F("meanPlotPosPtPhi","meanPlotPosPtPhi",nBinsPhi,phiRange,nBinsPt,ptRange); 
  meanPlotPos->GetXaxis()->SetTitle("#phi [rad]");
  meanPlotPos->GetYaxis()->SetTitle("E_{T} [GeV]");
  TH2F *meanPlotNeg = new TH2F("meanPlotNegPtPhi","meanPlotNegPtPhi",nBinsPhi,phiRange,nBinsPt,ptRange);
  meanPlotNeg->GetXaxis()->SetTitle("#phi [rad]");
  meanPlotNeg->GetYaxis()->SetTitle("E_{T} [GeV]");
  
  
  makeSuperAwesomePlots(originalDataSet, 
                        eOverP,
                        phi,
                        Pt,
                        meanPlotPos,
                        meanPlotNeg,
                        " && ClusterEta > 1.5");
                        
  TFile f(PATH+"fitEoverP_PhiPt.root","Recreate");
  meanPlotPos->Write();
  meanPlotNeg->Write();
  f.Write();
  f.Close();   
}




void makeSuperAwesomePlots( RooAbsData* originalDataSet, 
                            RooRealVar& eOverP, 
                            RooRealVar& xvar, 
                            RooRealVar& yvar,                                 
                            TH2F* meanPlotPos,
                            TH2F* meanPlotNeg,
                            TString additionalCut
                          )
{
  
  TCanvas* c = new TCanvas();
  SetAtlasStyle();

  // Signal function
  // -------------------------------------------------------------------------
  
  // Variables
  RooRealVar meanPos("<E/p>+","meanPos",1,0.,2.8,"") ;
  RooRealVar meanNeg("<E/p>-","meanNeg",1,0.,2.8,"") ;
  RooRealVar sigma("#sigma","sigma",0.1,0,5,"") ; 
  RooRealVar alpha("#alpha","alpha",-1,-100,-0.0001,"") ;
  RooRealVar n("n","n",1,0.1,1000,"") ;
  // define Landau parameters
  RooRealVar LandauMeanPos ("LandauM+", "LandauMeanPos", 1,0.7,1.5,""); LandauMeanPos.setConstant(kFALSE);
  RooRealVar LandauMeanNeg ("LandauM-", "LandauMeanNeg", 1,0.7,1.5,""); LandauMeanNeg.setConstant(kFALSE);
  RooRealVar LandauS ("Landau #sigma", "Landau Sigma", 0.1,0,5,""); LandauS.setConstant(kFALSE);
  

  // Functions
  RooGaussian gaussianPos("PosGauss", "initial Gauss", eOverP, meanPos, sigma);
  RooGaussian gaussianNeg("NegGauss", "initial Gauss", eOverP, meanNeg, sigma);
  RooCBShape  crystalBallPos("PosCB", "Crystal Ball shape", eOverP, meanPos, sigma, alpha, n);
  RooCBShape  crystalBallNeg("NegCB", "Crystal Ball shape", eOverP, meanNeg, sigma, alpha, n);
  
  RooLandau  LandauPos ("PosLandau", "initial Landau", eOverP, LandauMeanPos, LandauS);
  RooLandau  LandauNeg ("NegLandau", "initial Landau", eOverP, LandauMeanNeg, LandauS);
  
  //Landau-Gaussian convolution
  RooFFTConvPdf LcwGPos ("LcwG_Pos", "Landau (x) Gauss", eOverP, LandauPos, gaussianPos); 
  RooFFTConvPdf LcwGNeg ("LcwG_Neg", "Landau (x) Gauss", eOverP, LandauNeg, gaussianNeg); 



  
  
  // Set the fit ranges etc
  //eOverP.setRange("fitGauss",0.5,1.5);
  eOverP.setRange("final", 0.0, 3.0) ;
  eOverP.setBins(50);  // sets the number of bins of the E/P distribution
  eOverP.setRange("finalFit", 0.8, 2.5);

  
  
  static const int nBinsX = meanPlotPos->GetXaxis()->GetNbins();
  static const int nBinsY = meanPlotPos->GetYaxis()->GetNbins();
  
  
  RooAbsData* absDataSetPos = originalDataSet->reduce( Cut("charge > 0"+ additionalCut) );
  RooDataSet* dataSetPos = dynamic_cast<RooDataSet*>( absDataSetPos );
  RooAbsData* absDataSetNeg = originalDataSet->reduce( Cut("charge < 0"+ additionalCut)) ;
  RooDataSet* dataSetNeg = dynamic_cast<RooDataSet*>( absDataSetNeg );

  RooArgSet   vars(xvar,yvar,eOverP);
  
  RooDataSet* binnedDataPos[nBinsX][nBinsY];
  RooDataSet* binnedDataNeg[nBinsX][nBinsY];

  for(int xvarStep(0); xvarStep<nBinsX; ++xvarStep){
    for(int yvarStep(0); yvarStep<nBinsY; ++yvarStep){
    
      TString rangeName = xvar.getTitle() + "Bin";
      rangeName += xvarStep;
      rangeName += "_"+yvar.getTitle()+"Bin";
      rangeName += yvarStep;
      
      binnedDataPos[xvarStep][yvarStep] = (RooDataSet*)originalDataSet->emptyClone() ;
      binnedDataNeg[xvarStep][yvarStep] = (RooDataSet*)originalDataSet->emptyClone() ;
    
      binnedDataPos[xvarStep][yvarStep]->SetName(rangeName+"_Pos");
      binnedDataNeg[xvarStep][yvarStep]->SetName(rangeName+"_Neg");
    
    }
  }
  
  for (int i=0; i<dataSetPos->numEntries(); i++) {
    const RooArgSet* args = dataSetPos->get(i);
    vars= *args;
    int globalbin = meanPlotPos->FindBin(xvar.getVal(),yvar.getVal());
    int binx,biny,binz;
    meanPlotPos->GetBinXYZ(globalbin,binx,biny,binz);
    binnedDataPos[binx-1][biny-1]->add(*args);
  }
  
  for (int i=0; i<dataSetNeg->numEntries(); i++) {
    const RooArgSet* args = dataSetNeg->get(i);
    vars= *args;   
    int globalbin = meanPlotNeg->FindBin(xvar.getVal(),yvar.getVal());
    int binx,biny,binz;
    meanPlotNeg->GetBinXYZ(globalbin,binx,biny,binz);
    binnedDataNeg[binx-1][biny-1]->add(*args);
  }

  
  
  for(int xvarStep(0); xvarStep<nBinsX; ++xvarStep){
    for(int yvarStep(0); yvarStep<nBinsY; ++yvarStep){
      TString rangeName = xvar.getTitle() + "Bin";
      rangeName += xvarStep;
      rangeName += "_"+yvar.getTitle()+"Bin";
      rangeName += yvarStep;
      
      
      //Make frame to plot everything on
      RooPlot* framePos = eOverP.frame(Title("E/P pos"),Range("final"));
      RooPlot* frameNeg = eOverP.frame(Title("E/P neg"),Range("final"));


 
      RooDataSet* smallDataSetPos = binnedDataPos[xvarStep][yvarStep];
      RooDataSet* smallDataSetNeg = binnedDataNeg[xvarStep][yvarStep];
  
      // Plot the data on the frame
      smallDataSetPos->plotOn(framePos,LineColor(kBlue),MarkerColor(kBlue));
      smallDataSetNeg->plotOn(frameNeg,LineColor(kRed),MarkerColor(kRed));
      
      // Reset paramaters
      meanPos.setVal(1);
      meanNeg.setVal(1);
      sigma.setVal(0.1);
      alpha.setVal(-1);
      n.setVal(1);


      // C r e a t e   i n d e x   c a t e g o r y   a n d   j o i n   s a m p l e s
      // ---------------------------------------------------------------------------
    
      // Define category to distinguish physics and control samples events
      RooCategory sample("sample","sample") ;
      sample.defineType("Pos") ;
      sample.defineType("Neg") ;
    
      // Construct combined dataset in (x,sample)
      RooDataSet combData("combData","combined data",eOverP,Index(sample),Import("Pos",*smallDataSetPos),Import("Neg",*smallDataSetNeg)) ;
       
    
      // C o n s t r u c t   a   s i m u l t a n e o u s   p d f   i n   ( x , s a m p l e )
      // -----------------------------------------------------------------------------------
    
      // *** Gaussian ***
      // Construct a simultaneous pdf using category sample as index
      RooSimultaneous simPdfGS("simPdfGS","simultaneous pdf",sample) ;
    
      // Associate model with the physics state and model_ctl with the control state
      simPdfGS.addPdf(gaussianPos,"Pos") ;
      simPdfGS.addPdf(gaussianNeg,"Neg") ;

      // *** CristalBall ***
      // Construct a simultaneous pdf using category sample as index
      RooSimultaneous simPdfCB("simPdfCB","simultaneous pdf",sample) ;
    
      // Associate model with the physics state and model_ctl with the control state
      simPdfCB.addPdf(crystalBallPos,"Pos") ;
      simPdfCB.addPdf(crystalBallNeg,"Neg") ;
    
      // *** Landau ***  
      // Construct a simultaneous pdf using category sample as index
      RooSimultaneous simPdfLnd("simPdfLnd","simultaneous pdf",sample) ;
    
      // Associate model with the physics state and model_ctl with the control state
      simPdfLnd.addPdf(LandauPos,"Pos") ;
      simPdfLnd.addPdf(LandauNeg,"Neg") ;

      // *** Landau-Gaussian Convolution ***
      // Construct a simultaneous pdf using category sample as index
      RooSimultaneous simPdfLcwG("simPdfLcwG","simultaneous pdf",sample) ;
    
      // Associate model with the physics state and model_ctl with the control state
      simPdfLcwG.addPdf(LcwGPos,"Pos") ;
      simPdfLcwG.addPdf(LcwGNeg,"Neg") ;


    
      // P e r f o r m   a   s i m u l t a n e o u s   f i t
      // ---------------------------------------------------
    
      double fitMeanPos, fitMeanNeg, fitSigma; 
    
      eOverP.setRange("fitGauss_Pos",0.5,1.5);
      eOverP.setRange("fitGauss_Neg",0.5,1.5);
      RooFitResult* fitResult = simPdfGS.fitTo(combData,Range("fitGauss"),SplitRange(),Save());
      if(fitResult){
        std::cout << "WARNING:: FIRST GAUSSIAN FIT PRODUCED A FIT RESULT" <<std::endl;        
      } else {
        std::cout << "WARNING:: FIRST GAUSSIAN FIT DID NOT PRODUCE A FIT RESULT" <<std::endl;        
      }

      fitMeanPos = meanPos.getVal();
      fitMeanNeg = meanNeg.getVal();
      fitSigma = sigma.getVal();
      
      eOverP.setRange("fitGauss_Pos",fitMeanPos-2*fitSigma,fitMeanPos+2*fitSigma);      
      eOverP.setRange("fitGauss_Neg",fitMeanNeg-2*fitSigma,fitMeanNeg+2*fitSigma);      

      if ( fitResult && fitResult->status() != 0){
        std::cout << "WARNING:: FIRST GAUSSIAN FIT DID NOT CONVERGE" <<std::endl;
        meanPos.setVal(1);
        meanNeg.setVal(1);
        sigma.setVal(0.2);
        eOverP.setRange("fitGauss_Pos",0.1,1.9);
        eOverP.setRange("fitGauss_Neg",0.1,1.9);
      }
      
      fitResult = simPdfGS.fitTo(combData,Range("fitGauss"),SplitRange(),Save() );
      if ( fitResult && fitResult->status() != 0){
        std::cout << "WARNING::SECOND GAUSSIAN FIT DID NOT CONVERGE" <<std::endl;
        meanPos.setVal(1);
        meanNeg.setVal(1);
        sigma.setVal(0.2);
      }
      
      if (fitResult) delete fitResult;

      fitMeanPos = meanPos.getVal();
      fitMeanNeg = meanNeg.getVal();
      fitSigma = sigma.getVal();
     
      eOverP.setRange("fitGauss_Pos",fitMeanPos-1.25*fitSigma,fitMeanPos+1*fitSigma);      
      eOverP.setRange("fitGauss_Neg",fitMeanNeg-1.25*fitSigma,fitMeanNeg+1*fitSigma);      
      fitResult = simPdfGS.fitTo(combData,Range("fitGauss"),SplitRange(), Save() );

      if ( fitResult && fitResult->status() != 0){
        std::cout << "WARNING::THIRD GAUSSIAN FIT DID NOT CONVERGE" <<std::endl;
        meanPos.setVal(1);
        meanNeg.setVal(1);
        sigma.setVal(0.2);
        fitSigma = 0.3;
      }
      if(fitResult) delete fitResult;
      
      
      //Perform simultaneous fit of model to data and model_ctl to data_ctl
      eOverP.setRange("finalFit_Pos",fitMeanPos-1.5*fitSigma,2.5);      
      eOverP.setRange("finalFit_Neg",fitMeanNeg-1.5*fitSigma,2.5);      
 
      if (fitMeanPos-1.5*fitSigma > 1.4 )
        eOverP.setRange("finalFit_Pos",1.2,2.5);      
      if (fitMeanNeg-1.5*fitSigma > 1.4 )
        eOverP.setRange("finalFit_Neg",1.2,2.5);      

 //Here the range is fixed instead of using all teh above (25/Feb/14, S. Marti)
      eOverP.setRange("finalFit_Pos",0.8,2.5);      
      eOverP.setRange("finalFit_Neg",0.8,2.5);  
      
      meanPos.setVal(0);meanPos.setConstant(kTRUE);
      meanNeg.setVal(0);meanNeg.setConstant(kTRUE);
      sigma.setVal(0.2);    
      

//***** Choose one, CB, Landau or LcwG
      //Fit with CrystalBall   
    /*  fitResult = simPdfCB.fitTo(combData, Range("finalFit"),SplitRange(), Save() ) ;
	crystalBallPos.plotOn(framePos,LineColor(kOrange+1),Range("finalFit_Pos"),NormRange("finalFit_Pos"));
        crystalBallNeg.plotOn(frameNeg,LineColor(kGreen+1),Range("finalFit_Neg"),NormRange("finalFit_Neg")); */


      //Fit with Landau
    /*  fitResult = simPdfLnd.fitTo(combData, Range("finalFit"),SplitRange(), Save() ) ;
	LandauPos.plotOn(framePos,LineColor(kOrange+1),Range("finalFit_Pos"),NormRange("finalFit_Pos"));
        LandauNeg.plotOn(frameNeg,LineColor(kGreen+1),Range("finalFit_Neg"),NormRange("finalFit_Neg")); */

      if (true) {
	//Fit with Landau convoluted with Gaussian
	fitResult = simPdfLcwG.fitTo(combData, Range("finalFit"),SplitRange(), Save() ) ;
	LcwGPos.plotOn(framePos,LineColor(kOrange+1),Range("finalFit_Pos"),NormRange("finalFit_Pos"));
	LcwGNeg.plotOn(frameNeg,LineColor(kGreen+1),Range("finalFit_Neg"),NormRange("finalFit_Neg")); 
      }

      // hold on
      if (false) {
	string input = "";
	cout << " ** fit_EoverP ** Please type RETURN to continue:\n>";
	getline(cin, input);
      }

//*****
      
     
      meanPlotPos->SetBinContent(xvarStep+1,yvarStep+1,LandauMeanPos.getVal());
      meanPlotPos->SetBinError  (xvarStep+1,yvarStep+1,LandauMeanPos.getError());
      meanPlotNeg->SetBinContent(xvarStep+1,yvarStep+1,LandauMeanNeg.getVal());
      meanPlotNeg->SetBinError  (xvarStep+1,yvarStep+1,LandauMeanNeg.getError());

      if( fitResult && ( fitResult->status() != 0  || fitResult->covQual() != 3 )  ){
        std::cout << "WARNING::FINAL CB FIT DID NOT CONVERGE FULLY" <<std::endl;
        meanPlotPos->SetBinContent(xvarStep+1,yvarStep+1,1);
        meanPlotPos->SetBinError  (xvarStep+1,yvarStep+1,0.5);
        meanPlotNeg->SetBinContent(xvarStep+1,yvarStep+1,1);
        meanPlotNeg->SetBinError  (xvarStep+1,yvarStep+1,0.5);
      }
      
      if(fitResult) delete fitResult;
          
      c->Clear();    
      c->SetLogy(false);

      framePos->Draw();
      frameNeg->Draw("same");     
      
      //ATLASPRELIM_LABEL(0.25,0.88,0.03,1);
      TString* xvarTString = new TString(xvar.getTitle());
      TString* yvarTString = new TString(yvar.getTitle());
      
      const char *yvarChar = *yvarTString;
      const char *xvarChar = *xvarTString;
      
      
      myText(0.25,0.96,kBlack,Form("Slice %01.2f< %s <%01.2f, %01.2f< %s <%01.2f"
                                  ,meanPlotPos->GetXaxis()->GetBinLowEdge(xvarStep+1)
                                  ,xvarChar
                                  ,meanPlotPos->GetXaxis()->GetBinLowEdge(xvarStep+2)
                                  ,meanPlotPos->GetYaxis()->GetBinLowEdge(yvarStep+1)
                                  ,yvarChar
                                  ,meanPlotPos->GetYaxis()->GetBinLowEdge(yvarStep+2)) );
                                  
      myMarkerText(0.65,0.85,kBlue,20,Form("e^{+},  <E/p> %01.3f #pm %01.3f",meanPlotNeg->GetBinContent(xvarStep+1,yvarStep+1), meanPlotNeg->GetBinError(xvarStep+1,yvarStep+1) ),1.0,0.03);
      myMarkerText(0.65,0.80,kRed,20,Form("e^{-},  <E/p> %01.3f #pm %01.3f",meanPlotPos->GetBinContent(xvarStep+1,yvarStep+1), meanPlotPos->GetBinError(xvarStep+1,yvarStep+1) ),1.0,0.03);
      
      
      c->Print(PATH+"test.pdf");
      
      delete framePos;
      delete frameNeg;
      delete smallDataSetPos;
      delete smallDataSetNeg;
    }
  }
  
  makeAwesomeSlicePlots(meanPlotPos ,meanPlotNeg, false);
  makeAwesomeSlicePlots(meanPlotPos ,meanPlotNeg, true);
  
  const UInt_t Number = 3;
  Double_t Green[Number]  = { 0.00, 1.00, 0.00};
  Double_t Red[Number]    = { 1.00, 0.10, 0.00};
  Double_t Blue[Number]   = { 0.10, 0.10, 1.00};
  Double_t Length[Number] = { 0.00, 0.50, 1.00 };
  Int_t nb=50;
  //TColor::CreateGradientColorTable(Number,Length,Red,Green,Blue,nb);

  
  TH2F* clone =(TH2F*) meanPlotPos->Clone();
  clone->Divide(meanPlotNeg);
  clone->SetContour(nb);
  
  TH2F* clone2 =(TH2F*) meanPlotPos->Clone();
  clone2->Add( meanPlotNeg, -1);
  clone2->SetContour(nb);

  Double_t minPlot,maxPlot;

  c->Clear();
  c->SetRightMargin(0.12);
  c->SetLogy(false);
  clone->Draw("colz");

//Set Z axis centered in 1 so good points are in green
  minPlot= clone->GetMinimum();
  maxPlot= clone->GetMaximum();
  if (maxPlot-1 > 1-minPlot) clone->SetMinimum(2-maxPlot);
  else clone->SetMaximum(2-minPlot);
  //cout << " HHHHHHHHHHHHEREE minPlot = " << minPlot << "  &  maxPlot = " << maxPlot << endl;

  c->Print(PATH+"test.pdf");
  
  meanPlotPos->Draw("colz");
  meanPlotPos->SetContour(nb);

  //Set Z axis centered in 1 so good points are in green
  minPlot= meanPlotPos->GetMinimum();
  maxPlot= meanPlotPos->GetMaximum();
  if (maxPlot-1 > 1-minPlot) meanPlotPos->SetMinimum(2-maxPlot);
  else meanPlotPos->SetMaximum(2-minPlot);

  c->Print(PATH+"test.pdf");
  
  meanPlotNeg->Draw("colz");
  meanPlotNeg->SetContour(nb);

  //Set Z axis centered in 1 so good points are in green
  minPlot= meanPlotNeg->GetMinimum();
  maxPlot= meanPlotNeg->GetMaximum();
  if (maxPlot-1 > 1-minPlot) meanPlotNeg->SetMinimum(2-maxPlot);
  else meanPlotNeg->SetMaximum(2-minPlot);

  c->Print(PATH+"test.pdf");

  TH1F* pull = new TH1F("pull","pull",50,-5,5);
  pull->GetXaxis()->SetTitle("Fit Pull");
  pull->GetYaxis()->SetTitle("Entries per 0.2");
  
  c->SetRightMargin(0.03);
  TF1 myGauss("myGaus","gaus");
  for(int xvarStep(0); xvarStep<nBinsX; ++xvarStep){
    for(int yvarStep(0); yvarStep<nBinsY; ++yvarStep){   
      double R = clone->GetBinContent(xvarStep+1,yvarStep+1);
      std::cout << "INFO : R "<< R << " Ratio delta " << (1-R)/(1+R) << " Diff Delta "<<  clone2->GetBinContent(xvarStep+1,yvarStep+1)/-2.  << " Diff Delta "<<  1-(clone2->GetBinContent(xvarStep+1,yvarStep+1)/-2.)/((1-R)/(1+R))  <<std::endl;
      pull->Fill( ( 1-clone->GetBinContent(xvarStep+1,yvarStep+1))/clone->GetBinError  (xvarStep+1,yvarStep+1) );
    } 
  }
  pull->Draw();
  pull->Fit(&myGauss);
  myMarkerText(0.7,0.85,kWhite,1,Form("Gaussian, #mu = %01.3f #pm %01.3f",myGauss.GetParameter(1), myGauss.GetParError(1) ),1.0,0.03);
  myMarkerText(0.7,0.80,kWhite,1,Form("Gaussian, #sigma = %01.3f #pm %01.3f",myGauss.GetParameter(2), myGauss.GetParError(2) ),1.0,0.03);
  myMarkerText(0.7,0.75,kWhite,1,Form("Mean,      = %01.3f #pm %01.3f",pull->GetMean(), pull->GetMeanError() ),1.0,0.03);
  myMarkerText(0.7,0.70,kWhite,1,Form("RMS,       = %01.3f #pm %01.3f",pull->GetRMS(), pull->GetRMSError() ),1.0,0.03);
  
  c->Print(PATH+"test.pdf");

  
  delete clone;
  delete clone2;
  
  
}



void makeSuperAwesomePlots( RooRealVar& xvar, 
                            RooRealVar& yvar,                                 
                            TH3F* dataPlotPos,
                            TH3F* dataPlotNeg,
                            TH2F* meanPlotPos,
                            TH2F* meanPlotNeg
                          )
{
  
  TCanvas* c = new TCanvas();
  SetAtlasStyle();



  RooRealVar eOverP("E/p","EoverP",0,5);
  RooRealVar weight("weight","weight",-1e50,1e50);

  // Set the fit ranges etc
  eOverP.setRange("final",0,3) ;
 

  // Signal function
  // -------------------------------------------------------------------------
  
  // Variables
  RooRealVar meanPos("<E/p>+","meanPos",1,0.2,2.8,"") ;
  RooRealVar meanNeg("<E/p>-","meanNeg",1,0.2,2.8,"") ;
  RooRealVar sigma("#sigma","sigma",0.1,0,5,"") ; 
  RooRealVar alpha("#alpha","alpha",-1,-100,-0.0001,"") ;
  RooRealVar n("n","n",1,0.1,1000,"") ;
  // define Landau parameters
  RooRealVar LandauMeanPos ("LandauM+", "LandauMeanPos", 1,0.2,2.8,""); LandauMeanPos.setConstant(kFALSE);
  RooRealVar LandauMeanNeg ("LandauM-", "LandauMeanNeg", 1,0.2,2.8,""); LandauMeanNeg.setConstant(kFALSE);
  RooRealVar LandauS ("Landau #sigma", "Landau Sigma", 0.1,0,5,""); LandauS.setConstant(kFALSE);
  

  // Functions
  RooGaussian gaussianPos("PosGauss", "initial Gauss", eOverP, meanPos, sigma);
  RooGaussian gaussianNeg("NegGauss", "initial Gauss", eOverP, meanNeg, sigma);
  RooCBShape  crystalBallPos("PosCB", "Crystal Ball shape", eOverP, meanPos, sigma, alpha, n);
  RooCBShape  crystalBallNeg("NegCB", "Crystal Ball shape", eOverP, meanNeg, sigma, alpha, n);
  
  RooLandau  LandauPos ("PosLandau", "initial Landau", eOverP, LandauMeanPos, LandauS);
  RooLandau  LandauNeg ("NegLandau", "initial Landau", eOverP, LandauMeanNeg, LandauS);
  
  //Landau-Gaussian convolution
  RooFFTConvPdf LcwGPos ("LcwG_Pos", "Landau (x) Gauss", eOverP, LandauPos, gaussianPos); 
  RooFFTConvPdf LcwGNeg ("LcwG_Neg", "Landau (x) Gauss", eOverP, LandauNeg, gaussianNeg);   


  RooArgSet   vars(eOverP,weight);

  RooDataSet* dataSet = new RooDataSet("DataSetHist","DataSetHist",vars,"weight");    
  dataSet->Print("v");
  
  int nBinsX = meanPlotPos->GetXaxis()->GetNbins();
  int nBinsY = meanPlotPos->GetYaxis()->GetNbins();
  
  for(int xvarStep(0); xvarStep<nBinsX; ++xvarStep){
    for(int yvarStep(0); yvarStep<nBinsY; ++yvarStep){
      TString rangeName = xvar.getTitle() + "Bin";
      rangeName += xvarStep;
      rangeName += "_"+yvar.getTitle()+"Bin";
      rangeName += yvarStep;
     
      
      //Make frame to plot everything on
      RooPlot* framePos = eOverP.frame(Title("E/P pos"),Range("final"));
      RooPlot* frameNeg = eOverP.frame(Title("E/P neg"),Range("final"));
 
      // Set the range of the xvar and yvar parameters
      eOverP.setRange(rangeName,0,3) ;
      
      TH1D* posProjection = dataPlotPos->ProjectionZ(rangeName+"_Pos",xvarStep+1,xvarStep+1,yvarStep+1,yvarStep+1);
      TH1D* negProjection = dataPlotNeg->ProjectionZ(rangeName+"_Neg",xvarStep+1,xvarStep+1,yvarStep+1,yvarStep+1);
      

      
      // Create the datasets within range specified
      RooDataSet* smallDataSetPos = (RooDataSet*)dataSet->emptyClone() ;
      for (int i=1 ; i<=posProjection->GetNbinsX() ; i++) {
        eOverP.setVal(posProjection->GetBinCenter(i));
        weight.setVal(posProjection->GetBinContent(i));    
        smallDataSetPos->add(vars,posProjection->GetBinContent(i)) ;
      } 
      smallDataSetPos->SetName(rangeName+"_Pos");
      
      
      RooDataSet* smallDataSetNeg = (RooDataSet*)dataSet->emptyClone() ;
      for (int i=1 ; i<=negProjection->GetNbinsX() ; i++) {
        eOverP.setVal(negProjection->GetBinCenter(i));
        weight.setVal(posProjection->GetBinContent(i));
        smallDataSetNeg->add(vars,negProjection->GetBinContent(i)) ;
      } 
      smallDataSetNeg->SetName(rangeName+"_Neg");
     
     
      delete posProjection;
      delete negProjection;
 
      
      // Plot the data on the frame
      smallDataSetPos->plotOn(framePos,LineColor(kBlue),MarkerColor(kBlue));
      smallDataSetNeg->plotOn(frameNeg,LineColor(kRed),MarkerColor(kRed));
      
      // Reset paramaters
      meanPos.setVal(1);
      meanNeg.setVal(1);
      sigma.setVal(0.1);
      alpha.setVal(-1);
      n.setVal(1);


      // C r e a t e   i n d e x   c a t e g o r y   a n d   j o i n   s a m p l e s
      // ---------------------------------------------------------------------------
    
      // Define category to distinguish physics and control samples events
      RooCategory sample("sample","sample") ;
      sample.defineType("Pos") ;
      sample.defineType("Neg") ;
    
      // Construct combined dataset in (x,sample)
      RooDataSet combData("combData","combined data",vars,Index(sample),Import("Pos",*smallDataSetPos),Import("Neg",*smallDataSetNeg)) ;
      std::cout << "************************************" <<std::endl;
      combData.Print("v");
    
 
     // C o n s t r u c t   a   s i m u l t a n e o u s   p d f   i n   ( x , s a m p l e )
      // -----------------------------------------------------------------------------------
    
      // *** Gaussian ***
      // Construct a simultaneous pdf using category sample as index
      RooSimultaneous simPdfGS("simPdfGS","simultaneous pdf",sample) ;
    
      // Associate model with the physics state and model_ctl with the control state
      simPdfGS.addPdf(gaussianPos,"Pos") ;
      simPdfGS.addPdf(gaussianNeg,"Neg") ;

      // *** CristalBall ***
      // Construct a simultaneous pdf using category sample as index
      RooSimultaneous simPdfCB("simPdfCB","simultaneous pdf",sample) ;
    
      // Associate model with the physics state and model_ctl with the control state
      simPdfCB.addPdf(crystalBallPos,"Pos") ;
      simPdfCB.addPdf(crystalBallNeg,"Neg") ;
    
      // *** Landau ***  
      // Construct a simultaneous pdf using category sample as index
      RooSimultaneous simPdfLnd("simPdfLnd","simultaneous pdf",sample) ;
    
      // Associate model with the physics state and model_ctl with the control state
      simPdfLnd.addPdf(LandauPos,"Pos") ;
      simPdfLnd.addPdf(LandauNeg,"Neg") ;

      // *** Landau-Gaussian Convolution ***
      // Construct a simultaneous pdf using category sample as index
      RooSimultaneous simPdfLcwG("simPdfLcwG","simultaneous pdf",sample) ;
    
      // Associate model with the physics state and model_ctl with the control state
      simPdfLcwG.addPdf(LcwGPos,"Pos") ;
      simPdfLcwG.addPdf(LcwGNeg,"Neg") ;

 
    
    
    
      // P e r f o r m   a   s i m u l t a n e o u s   f i t
      // ---------------------------------------------------
    
      double fitMeanPos, fitMeanNeg, fitSigma; 
    
      eOverP.setRange("fitGauss_Pos",0.5,1.5);
      eOverP.setRange("fitGauss_Neg",0.5,1.5);
      RooFitResult* fitResult = simPdfGS.fitTo(combData,Range("fitGauss"),SplitRange(),Save(),SumW2Error(kTRUE));
      if(fitResult){
        std::cout << "WARNING:: FIRST GAUSSIAN FIT PRODUCED A FIT RESULT" <<std::endl;        
      } else {
        std::cout << "WARNING:: FIRST GAUSSIAN FIT DID NOT PRODUCE A FIT RESULT" <<std::endl;        
      }

      fitMeanPos = meanPos.getVal();
      fitMeanNeg = meanNeg.getVal();
      fitSigma = sigma.getVal();
      
      eOverP.setRange("fitGauss_Pos",fitMeanPos-2*fitSigma,fitMeanPos+2*fitSigma);      
      eOverP.setRange("fitGauss_Neg",fitMeanNeg-2*fitSigma,fitMeanNeg+2*fitSigma);      

      if ( fitResult && fitResult->status() != 0){
        std::cout << "WARNING:: FIRST GAUSSIAN FIT DID NOT CONVERGE" <<std::endl;
        meanPos.setVal(1);
        meanNeg.setVal(1);
        sigma.setVal(0.2);
        eOverP.setRange("fitGauss_Pos",0.1,1.9);
        eOverP.setRange("fitGauss_Neg",0.1,1.9);
      }
      
      fitResult = simPdfGS.fitTo(combData,Range("fitGauss"),SplitRange(),Save(),SumW2Error(kTRUE) );
      if ( fitResult && fitResult->status() != 0){
        std::cout << "WARNING::SECOND GAUSSIAN FIT DID NOT CONVERGE" <<std::endl;
        meanPos.setVal(1);
        meanNeg.setVal(1);
        sigma.setVal(0.2);
      }
      
      if (fitResult) delete fitResult;

      fitMeanPos = meanPos.getVal();
      fitMeanNeg = meanNeg.getVal();
      fitSigma = sigma.getVal();
     
      eOverP.setRange("fitGauss_Pos",fitMeanPos-1.25*fitSigma,fitMeanPos+1*fitSigma);      
      eOverP.setRange("fitGauss_Neg",fitMeanNeg-1.25*fitSigma,fitMeanNeg+1*fitSigma);      
      fitResult = simPdfGS.fitTo(combData,Range("fitGauss"),SplitRange(), Save() );

      if ( fitResult && fitResult->status() != 0){
        std::cout << "WARNING::THIRD GAUSSIAN FIT DID NOT CONVERGE" <<std::endl;
        meanPos.setVal(1);
        meanNeg.setVal(1);
        sigma.setVal(0.2);
        fitSigma = 0.3;
      }
      if(fitResult) delete fitResult;
      
      
      //Perform simultaneous fit of model to data and model_ctl to data_ctl
      eOverP.setRange("finalFit_Pos",fitMeanPos-1.5*fitSigma,2.5);      
      eOverP.setRange("finalFit_Neg",fitMeanNeg-1.5*fitSigma,2.5);      
 
      if (fitMeanPos-1.5*fitSigma > 1.4 )
        eOverP.setRange("finalFit_Pos",1.2,2.5);      
      if (fitMeanNeg-1.5*fitSigma > 1.4 )
        eOverP.setRange("finalFit_Neg",1.2,2.5);      
 
     //***** Choose one, CB, Landau or LcwG
      //Fit with CrystalBall   
    /*  fitResult = simPdfCB.fitTo(combData, Range("finalFit"),SplitRange(), Save() ) ;
	crystalBallPos.plotOn(framePos,LineColor(kOrange+1),Range("finalFit_Pos"),NormRange("finalFit_Pos"));
        crystalBallNeg.plotOn(frameNeg,LineColor(kGreen+1),Range("finalFit_Neg"),NormRange("finalFit_Neg")); */


      //Fit with Landau
    /*  fitResult = simPdfLnd.fitTo(combData, Range("finalFit"),SplitRange(), Save() ) ;
	LandauPos.plotOn(framePos,LineColor(kOrange+1),Range("finalFit_Pos"),NormRange("finalFit_Pos"));
        LandauNeg.plotOn(frameNeg,LineColor(kGreen+1),Range("finalFit_Neg"),NormRange("finalFit_Neg")); */

      //Fit with Landau convoluted with Gaussian
      fitResult = simPdfLcwG.fitTo(combData, Range("finalFit"),SplitRange(), Save() ) ;
      LcwGPos.plotOn(framePos,LineColor(kOrange+1),Range("finalFit_Pos"),NormRange("finalFit_Pos"));
      LcwGNeg.plotOn(frameNeg,LineColor(kGreen+1),Range("finalFit_Neg"),NormRange("finalFit_Neg")); 
//*****

     
      meanPlotPos->SetBinContent(xvarStep+1,yvarStep+1,meanPos.getVal());
      meanPlotPos->SetBinError  (xvarStep+1,yvarStep+1,meanPos.getError());
      meanPlotNeg->SetBinContent(xvarStep+1,yvarStep+1,meanNeg.getVal());
      meanPlotNeg->SetBinError  (xvarStep+1,yvarStep+1,meanNeg.getError());

      if( fitResult && ( fitResult->status() != 0  || fitResult->covQual() != 3 )  ){
        std::cout << "WARNING::FINAL CB FIT DID NOT CONVERGE FULLY" <<std::endl;
        meanPlotPos->SetBinContent(xvarStep+1,yvarStep+1,1);
        meanPlotPos->SetBinError  (xvarStep+1,yvarStep+1,0.5);
        meanPlotNeg->SetBinContent(xvarStep+1,yvarStep+1,1);
        meanPlotNeg->SetBinError  (xvarStep+1,yvarStep+1,0.5);
      }
      
      if(fitResult) delete fitResult;
          
      c->Clear();    
      c->SetLogy(false);

      framePos->Draw();
      frameNeg->Draw("same");     
      
      //ATLASPRELIM_LABEL(0.25,0.88,0.03,1);
      TString* xvarTString = new TString(xvar.getTitle());
      TString* yvarTString = new TString(yvar.getTitle());
      
      const char *yvarChar = *yvarTString;
      const char *xvarChar = *xvarTString;
      
      
      myText(0.25,0.96,kBlack,Form("Slice %01.2f< %s <%01.2f, %01.2f< %s <%01.2f"
                                  ,meanPlotPos->GetXaxis()->GetBinLowEdge(xvarStep+1)
                                  ,xvarChar
                                  ,meanPlotPos->GetXaxis()->GetBinLowEdge(xvarStep+2)
                                  ,meanPlotPos->GetYaxis()->GetBinLowEdge(yvarStep+1)
                                  ,yvarChar
                                  ,meanPlotPos->GetYaxis()->GetBinLowEdge(yvarStep+2)) );
                                  
      myMarkerText(0.65,0.85,kBlue,20,Form("e^{+},  <E/p> %01.3f #pm %01.3f",meanPlotNeg->GetBinContent(xvarStep+1,yvarStep+1), meanPlotNeg->GetBinError(xvarStep+1,yvarStep+1) ),1.0,0.03);
      myMarkerText(0.65,0.80,kRed,20,Form("e^{-},  <E/p> %01.3f #pm %01.3f",meanPlotPos->GetBinContent(xvarStep+1,yvarStep+1), meanPlotPos->GetBinError(xvarStep+1,yvarStep+1) ),1.0,0.03);
      
      
      c->Print(PATH+"test.pdf");
      //c->SetLogy(true);
      //c->Print("test.pdf");
      
      delete framePos;
      delete frameNeg;
      delete smallDataSetPos;
      delete smallDataSetNeg;
    }
  }
  
  makeAwesomeSlicePlots(meanPlotPos ,meanPlotNeg, false);
  makeAwesomeSlicePlots(meanPlotPos ,meanPlotNeg, true);
  
  const UInt_t Number = 3;
  Double_t Green[Number]  = { 0.00, 1.00, 0.00};
  Double_t Red[Number]    = { 1.00, 0.10, 0.00};
  Double_t Blue[Number]   = { 0.10, 0.10, 1.00};
  Double_t Length[Number] = { 0.00, 0.50, 1.00 };
  Int_t nb=50;
  TColor::CreateGradientColorTable(Number,Length,Red,Green,Blue,nb);

  
  TH2F* clone =(TH2F*) meanPlotPos->Clone();
  clone->Divide(meanPlotNeg);
  clone->SetContour(nb);
  
  TH2F* clone2 =(TH2F*) meanPlotPos->Clone();
  clone2->Add( meanPlotNeg, -1);
  clone2->SetContour(nb);
  
  c->Clear();
  c->SetRightMargin(0.12);
  c->SetLogy(false);
  clone->Draw("colz");
  c->Print(PATH+"test.pdf");
  meanPlotPos->Draw("colz");
  meanPlotPos->SetContour(nb);

  c->Print(PATH+"test.pdf");
  meanPlotNeg->Draw("colz");
  meanPlotNeg->SetContour(nb);
  c->Print(PATH+"test.pdf");

  TH1F* pull = new TH1F("pull","pull",50,-5,5);
  pull->GetXaxis()->SetTitle("Fit Pull");
  pull->GetYaxis()->SetTitle("Entries per 0.2");
  
  c->SetRightMargin(0.03);
  TF1 myGauss("myGaus","gaus");
  for(int xvarStep(0); xvarStep<nBinsX; ++xvarStep){
    for(int yvarStep(0); yvarStep<nBinsY; ++yvarStep){   
      double R = clone->GetBinContent(xvarStep+1,yvarStep+1);
      std::cout << "INFO : R "<< R << " Ratio delta " << (1-R)/(1+R) << " Diff Delta "<<  clone2->GetBinContent(xvarStep+1,yvarStep+1)/-2.  << " Diff Delta "<<  1-(clone2->GetBinContent(xvarStep+1,yvarStep+1)/-2.)/((1-R)/(1+R))  <<std::endl;
      pull->Fill( ( 1-clone->GetBinContent(xvarStep+1,yvarStep+1))/clone->GetBinError  (xvarStep+1,yvarStep+1) );
    } 
  }
  pull->Draw();
  pull->Fit(&myGauss);
  myMarkerText(0.7,0.85,kWhite,1,Form("Gaussian, #mu = %01.3f #pm %01.3f",myGauss.GetParameter(1), myGauss.GetParError(1) ),1.0,0.03);
  myMarkerText(0.7,0.80,kWhite,1,Form("Gaussian, #sigma = %01.3f #pm %01.3f",myGauss.GetParameter(2), myGauss.GetParError(2) ),1.0,0.03);
  myMarkerText(0.7,0.75,kWhite,1,Form("Mean,      = %01.3f #pm %01.3f",pull->GetMean(), pull->GetMeanError() ),1.0,0.03);
  myMarkerText(0.7,0.70,kWhite,1,Form("RMS,       = %01.3f #pm %01.3f",pull->GetRMS(), pull->GetRMSError() ),1.0,0.03);
  
  c->Print(PATH+"test.pdf");

  
  delete clone;
  delete clone2;
  
  
}




void makeAwesomeSlicePlots(TH2F* pos, TH2F* neg, bool sliceInEta)
{
  std::cout << "INFO::makeAwesomeSlicePlots()" << std::endl;
  
  int maxStep(1);

  if(sliceInEta){
    maxStep = pos->GetNbinsX();
  } else {
    maxStep = pos->GetNbinsY();
  }
  
  TString name; 
  for (int i(0); i <maxStep; ++i ){
    TH1D* posProjection;
    TH1D* negProjection;
    name  =  pos->GetName();
    name += "_Slice";
  
    if (sliceInEta){
      posProjection = pos->ProjectionY( "_py", i+1 , i+1);
      negProjection = neg->ProjectionY( "_py", i+1 , i+1);
      name+="X";
      name+=i;
      
    } else {
      posProjection = pos->ProjectionX( "_px", i+1 , i+1);
      negProjection = neg->ProjectionX( "_px", i+1 , i+1);    
      name+="Y";
      name+=i;
      
    }
    
    
    
    ratioPlots(posProjection, negProjection, name );
    delete posProjection;
    delete negProjection;
  }  
  std::cout << "INFO::Finished makeAwesomeSlicePlots()" << std::endl;
}




void ratioPlots(TH1D* h1, TH1D* h2, TString name) {
   std::cout << "INFO::Starting ratioPlots()" << std::endl;
   TCanvas c1("C1","C1",600,600);
   TPad *pad1 = new TPad("pad1","pad1",0,0.5,1,1);
   pad1->SetTopMargin(0.05/0.5);
   pad1->SetRightMargin(0.05);
   pad1->SetBottomMargin(0.16);
   pad1->SetLeftMargin(0.16);

   pad1->SetBottomMargin(0);
   pad1->Draw();
   pad1->cd();
   TH1D* h1clone = (TH1D*) h1->Clone();
   h1->SetMarkerColor(kBlue);
   h1->SetMarkerStyle(20);
   h1->GetYaxis()->SetRangeUser(0.85,1.45);
   h1->GetYaxis()->SetLabelSize(0.05/0.5);
   h1->GetYaxis()->SetTitleSize(0.05/0.5);
   h1->GetYaxis()->SetTitle("<E/p>");
   h1->DrawCopy("pe");
   h2->SetMarkerColor(kRed);
   h2->SetMarkerStyle(22);
   h2->Draw("samepe");
   c1.cd();
   TPad *pad2 = new TPad("pad2","pad2",0,0,1,0.5);
   pad2->SetTopMargin(0.05);
   pad2->SetRightMargin(0.05);
   pad2->SetBottomMargin(0.16/0.5);
   pad2->SetLeftMargin(0.16);

   //pad2->SetTopMargin(0);
   pad2->Draw();
   pad2->cd();
   h1clone->Sumw2();
   h1clone->SetStats(0);
   h1clone->Divide(h2);
   h1clone->SetMarkerColor(kBlack);
   h1clone->SetMarkerStyle(20);
   h1clone->GetYaxis()->SetLabelSize(0.05/0.5);
   h1clone->GetXaxis()->SetLabelSize(0.05/0.5);
   h1clone->GetYaxis()->SetTitleSize(0.05/0.5);
   h1clone->GetXaxis()->SetTitleSize(0.05/0.5);
   h1clone->GetYaxis()->SetRangeUser(0.925,1.075);
   h1clone->GetYaxis()->SetTitle("<E/p>^{+}/<E/p>^{-}");
 
   h1clone->Draw("ep");
   //h1->GetYaxis()->SetRangeUser(0.95,1.05);
   c1.cd();
   c1.Print( PATH + name+".eps");
   delete h1clone;
   delete pad1;
   delete pad2;
   std::cout << "INFO::Finished ratioPlots()" << std::endl;

}


TH2F* getCorrections()
{
  TH2F *FinalCorrections = new TH2F("FinalCorrections","FinalCorrections",16,-2.5,2.5,16,-3.14159,3.14159);
  return FinalCorrections;
}


void MakeScaling(TString inputDir)
{

  TString dir = PATH;
  if(inputDir != "") dir  = inputDir;
  
  TFile *file0 = TFile::Open(dir+"fitEoverP.root");
  file0->ls();
  TH2F* meanPlotNeg = (TH2F*) file0->Get("meanPlotNeg");
  TH2F* meanPlotPos = (TH2F*) file0->Get("meanPlotPos");


  if(!meanPlotNeg || !meanPlotNeg){
    std::cout<< "ERROR  : inputfile does not contain the desired histograms "  << std::endl; 
  }
  
  meanPlotNeg->Draw("colz");
  meanPlotPos->Draw("colz");
  meanPlotNeg->Sumw2();
  meanPlotPos->Sumw2();


  //Need to obtain the mean pt either from a histogram or from hardcoded values 
/*  double pt[] = {39605.2,
  40822.5,
  41502.1,
  40948.9,
  40943.1,
  41343.5,
  40501.1,
  39274.9};
*/

double pt[100];

  TH2F* meanPtVsEta =  (TH2F*) file0->Get("meanPtVsEta");
  for(int i(1); i <= meanPtVsEta->GetNbinsX(); ++i){
    TH1D* projection =  meanPtVsEta->ProjectionY( "_py", i , i);
    pt[i-1] = projection->GetMean();
    delete projection;
  }  
  

  TH2F* a = (TH2F*) meanPlotPos->Clone(); 
  a->Add(meanPlotNeg,-1);
  a->Scale(-0.5);
  //a->Scale(1./38.);

  for(int i(1); i <= a->GetNbinsX(); ++i){
    for(int j(1); j <= a->GetNbinsY(); ++j){
      double temp  =  a->GetBinContent(i,j);
      a->SetBinContent(i,j, temp/pt[i-1]*1e3); 
      temp  =  a->GetBinError(i,j);
      a->SetBinError(i,j, temp/pt[i-1]*1e3);    
    }
  }
  
  a->Draw("colz");
  
  TFile f(dir+"Scaling.root","RECREATE");
  a->SetName("FinalCorrections");
  a->SetTitle("FinalCorrections");
  a->Write();
  f.ls();
  f.Close();
}

 

