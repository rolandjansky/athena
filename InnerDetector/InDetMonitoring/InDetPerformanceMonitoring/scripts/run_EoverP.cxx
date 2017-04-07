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
#include<TString.h>
#include<iostream>
#include<TLegend.h>
#include <iomanip>
#include <cmath>

#include "TROOT.h"
#include "THStack.h"
#include "AtlasStyle.C"

using namespace RooFit ;

// general functions
void initialize();
int main(){
  return 0;
}

// plotting functions
void makePlots( RooAbsData* originalDataSet, 
		RooRealVar& eOverP, 
		RooRealVar& eta, 
		int nBinsEta,  
		RooRealVar& phi, 
		int nBinsPhi,
		RooRealVar& Et,
		TString extraCuts = "");
void  makeFits( RooAbsData* originalDataSet, 
	        RooRealVar& eOverP,
	        RooRealVar& eta, 
	        RooRealVar& phi, 
	        TH2F* meanPlotPos,
	        TH2F* meanPlotNeg,
	        TString additionalCut = "" );
void makeSlicePlots(TH2F* pos, TH2F* neg, bool sliceInEta);
void makeRatioPlots(TH1D* h1, TH1D* h2, TString name);

// corrections functions
TH2F*   makeCorrectionHist();
double  getCorrections(TH2F* corrections, double eta, double phi, double Et, double charge);
//void    makeEtMeans(RooDataSet* dataSet,RooRealVar& Et,RooRealVar& ClusterEta,int nBinsEats);
void    makeScalingHist(TString inputDir);

// derpy global variable
TString PATH = "./Results/";

void run_EoverP(TString fileName = "../share/eoverpValidationOut.root", int refit=2, int netabins=12, int nphibins=12){

  // ----------
  // initialize
  // ----------
  initialize();
   
  // ----------
  // check validity of input vars
  // ----------
  if( !((refit >= 0) && (refit <= 2)) ){
    std::cout << "ERROR :: Input parameter 'refit' must be equal to 0, 1, or 2!" << std::endl;
    std::exit(0);
  }
  if( (netabins < 1) || (nphibins < 1) ){
    std::cout << "ERROR :: Can't have less than one bin in eta/phi!" << std::endl;
    std::exit(0);
  }

  // ----------
  // set up our RooFit variables
  // ----------
  RooRealVar ClusterEnergy("ClusterEnergy","ClusterEnergy",0,1e10,"MeV") ;
  RooRealVar ClusterPhi("ClusterPhi","#phi",0);
  RooRealVar ClusterEta("ClusterEta","#eta",0);
  RooRealVar charge("charge","charge",0);
  RooRealVar TrackTheta("TrackTheta","TrackTheta",0);
  RooRealVar TrackQoverP("QoverP","QoverP",-1e10,1e10,"MeV^{-1}");
  RooRealVar EoverP("EoverP","E/p",0,5,"");
  RooRealVar Et("Et","E_{T}",0,1e10,"GeV");
  RooRealVar Eta("Eta","#eta",-3,3,"");
  
  RooArgSet arg;
  arg.add(ClusterEnergy);
  arg.add(ClusterPhi);
  arg.add(ClusterEta);
  arg.add(charge);
  arg.add(TrackTheta);
  arg.add(TrackQoverP);
  arg.add(EoverP);
  arg.add(Et);
  arg.add(Eta);
 
  // ---------
  // Open our TFile(s) and set up our event variables
  //  (in this format, each "event" is one electron)
  // ---------
  TChain *eoverpTree = new TChain("EGrefitterSmall");
  eoverpTree->Add(fileName);
  
  Double_t         el_ClusterEnergy;
  Double_t         el_ClusterPhi;
  Double_t         el_ClusterEta;
  Double_t         el_charge;
  Double_t         el_TrackTheta;
  Double_t         el_QoverP;
  Double_t         el_QoverP1;
  Double_t         el_QoverP2;
  eoverpTree->SetBranchAddress("ClusterEnergy", &el_ClusterEnergy);
  eoverpTree->SetBranchAddress("ClusterPhi", &el_ClusterPhi);
  eoverpTree->SetBranchAddress("ClusterEta", &el_ClusterEta);
  eoverpTree->SetBranchAddress("charge", &el_charge);
  eoverpTree->SetBranchAddress("TrackTheta", &el_TrackTheta);  
  eoverpTree->SetBranchAddress("Default_QoverP", &el_QoverP);
  eoverpTree->SetBranchAddress("Refitted1_QoverP", &el_QoverP1);
  eoverpTree->SetBranchAddress("Refitted2_QoverP", &el_QoverP2);
  
  RooDataSet* dataSet = new RooDataSet("DataSet","DataSet",arg);
  
  // which refit QoverP do we want?  -- Hardcoded for now...
  //Int_t refit = 2 // 0 = original; 1 = refit1; 2 = refit2

  // ----------
  // Loop over the events and fill the variables
  // ----------
  Int_t nEl = eoverpTree->GetEntries();
  for (Int_t i=0; i<nEl; i++){
    if( (i%10000) == 0 )
      std::cout << "Filling variables for electron #" << i << std::endl;
    eoverpTree->GetEntry(i);

    charge.setVal(el_charge);
    Et.setVal(el_ClusterEnergy*sin(el_TrackTheta)*1e-3);
    Eta.setVal(-log(tan(el_TrackTheta/2)) );
    ClusterPhi.setVal(el_ClusterPhi);
    ClusterEnergy.setVal(el_ClusterEnergy);
    ClusterEta.setVal(el_ClusterEta);
    //std::cout << "default: " << el_QoverP*el_ClusterEnergy << " refit1: " << el_QoverP1*el_ClusterEnergy << " refit2: " << el_QoverP2*el_ClusterEnergy << std::endl;
    if(refit == 0){
      EoverP.setVal(fabs(el_QoverP)*el_ClusterEnergy);
      //std::cout << "refit = " << refit << "   Using " << 1./el_QoverP << std::endl;
    }
    else if(refit == 1){
      EoverP.setVal(fabs(el_QoverP1)*el_ClusterEnergy);
      //std::cout << "refit = " << refit << "   Using " << 1./el_QoverP1 << std::endl;
    }
    else{
      EoverP.setVal(fabs(el_QoverP2)*el_ClusterEnergy);
      //std::cout << "refit = " << refit << "   Using " << 1./el_QoverP2 << std::endl;
    }
    
    dataSet->add(arg);

  }

  TCanvas c; // put a blank page at the start of the output. because reasons.
  c.Print(PATH+"new_eoverp_plots.pdf(");

  // ----------
  // Pass the data off to the plotting functions
  // ----------
  makePlots(dataSet, 
	    EoverP, 
	    ClusterEta, 
	    netabins, 
	    ClusterPhi, 
	    nphibins,
	    Et); 
  
  c.Print(PATH+"new_eoverp_plots.pdf)"); // put a blank page at the end of the output. yup.

  makeScalingHist(PATH);

  return;

}

void makePlots( RooAbsData* originalDataSet, 
		RooRealVar& eOverP, 
		RooRealVar& eta, 
		int nBinsEta,  
		RooRealVar& phi, 
		int nBinsPhi,
		RooRealVar& Et,
		TString extraCuts )
{
  // Set up ranges
  double etaRange[nBinsEta+1];
  double stepSize = 5./nBinsEta;
  for (int  i(0); i<= nBinsEta; ++i){
    etaRange[i] = -2.5+stepSize*(double)i;
  }

  double phiRange[nBinsPhi+1];
  stepSize = 2 * TMath::Pi()/nBinsPhi;
  for (int  i(0); i<= nBinsPhi; ++i){
    phiRange[i] = -TMath::Pi()+stepSize*(double)i;
  }
  
  
  TH2F *meanPlotPos = new TH2F("meanPlotPos"+extraCuts,"meanPlotPos"+extraCuts,nBinsEta,etaRange,nBinsPhi,phiRange ); 
  meanPlotPos->SetTitle("Mean Plot Positive "+extraCuts);
  meanPlotPos->GetXaxis()->SetTitle("#eta");
  meanPlotPos->GetYaxis()->SetTitle("#phi [rad]");
  TH2F *meanPlotNeg = new TH2F("meanPlotNeg"+extraCuts,"meanPlotNeg"+extraCuts,nBinsEta,etaRange,nBinsPhi,phiRange );
  meanPlotNeg->SetTitle("Mean Plot Negative "+extraCuts);
  meanPlotNeg->GetXaxis()->SetTitle("#eta");
  meanPlotNeg->GetYaxis()->SetTitle("#phi [rad]");

  makeFits(originalDataSet, 
	   eOverP,
	   eta,
	   phi,
	   meanPlotPos,
	   meanPlotNeg,
	   extraCuts);

  TFile f(PATH+"fitEoverP.root","Recreate");
  meanPlotPos->Write();
  meanPlotNeg->Write();
  
  
  //if(Et != NULL ){
  double ptRange[102];
  stepSize = 1500;
  for (int  i=0; i<= 100; i++){
    ptRange[i] = (20e3+stepSize*(double)i)/1000.; // GeV
    //std::cout << ptRange[i] << std::endl;
  }
  
  double eopRange[102];
  stepSize = 0.05;
  for (int  i=0; i<= 100; i++){
    eopRange[i] = 0.8+stepSize*(double)i;
  }
  
  TH2F *ptvsEta =  new TH2F("meanPtVsEta"+extraCuts,"meanPtVsEta"+extraCuts,nBinsEta,etaRange,100,ptRange);
  TH2F *eoverPvsEta =  new TH2F("eoverPvsEta"+extraCuts,"eoverPvsEta"+extraCuts,nBinsEta,etaRange,100,eopRange);
  
  RooArgSet vars(Et,eOverP,eta);
  for(int i=0; i < originalDataSet->numEntries(); i++){
    // wkd2 -- this is giving the same value of all the parameters for all i, need to fix it!
    const RooArgSet* args = originalDataSet->get(i);
    vars = *args;
    //originalDataSet->get(i);         
    double valEt     = Et.getVal();
    double valeOverP = eOverP.getVal(); 
    double valEta    = eta.getVal();
    
    //std::cout << "valEta: " << valEta << "  valEt: " << valEt << std::endl;

    ptvsEta->Fill(valEta,valEt);
    eoverPvsEta->Fill(valEta,valeOverP);
  }
  
  ptvsEta->Write();
  eoverPvsEta->Write();
  //} // end if(Et != NULL)

  //f.Write();
  f.Close();   
}


void makeFits( RooAbsData* originalDataSet, 
	       RooRealVar& eOverP, 
	       RooRealVar& xvar, 
	       RooRealVar& yvar,                                 
	       TH2F* meanPlotPos,
	       TH2F* meanPlotNeg,
	       TString additionalCut )
{
 
  //----------
  // Initialize canvas
  //----------
  TCanvas *c = new TCanvas();
  SetAtlasStyle();

  //----------
  // Set up variables for the fits
  //----------
  // Variables
  RooRealVar meanPos("<E/p>+","meanPos",1,0.,2.8,"") ;
  RooRealVar meanNeg("<E/p>-","meanNeg",1,0.,2.8,"") ;
  RooRealVar sigma("#sigma","sigma",0.1,0,5,"") ; 
  RooRealVar alpha("#alpha","alpha",-1,-100,-0.0001,"") ;
  RooRealVar n("n","n",1,0.1,1000,"") ;

  // Landau parameters
  RooRealVar LandauMeanPos ("LandauM+", "LandauMeanPos", 1,0.7,1.5,""); 
  LandauMeanPos.setConstant(kFALSE);
  RooRealVar LandauMeanNeg ("LandauM-", "LandauMeanNeg", 1,0.7,1.5,""); 
  LandauMeanNeg.setConstant(kFALSE);
  RooRealVar LandauS ("Landau #sigma", "Landau Sigma", 0.1,0,5,""); 
  LandauS.setConstant(kFALSE);

  // Functions
  RooGaussian gaussianPos("PosGauss", "Initial Gauss", eOverP, meanPos, sigma);
  RooGaussian gaussianNeg("NegGauss", "Initial Gauss", eOverP, meanNeg, sigma);
  RooCBShape  crystalBallPos("PosCB", "Crystal Ball shape", eOverP, meanPos, sigma, alpha, n);
  RooCBShape  crystalBallNeg("NegCB", "Crystal Ball shape", eOverP, meanNeg, sigma, alpha, n);
  RooLandau  LandauPos ("PosLandau", "Initial Landau", eOverP, LandauMeanPos, LandauS);
  RooLandau  LandauNeg ("NegLandau", "Initial Landau", eOverP, LandauMeanNeg, LandauS);

  // Landau-Gaussian convolution
  RooFFTConvPdf LcwGPos ("LcwG_Pos", "Landau (x) Gauss", eOverP, LandauPos, gaussianPos); 
  RooFFTConvPdf LcwGNeg ("LcwG_Neg", "Landau (x) Gauss", eOverP, LandauNeg, gaussianNeg);

  // Setup fit ranges for eOverP variable we filled earlier
  eOverP.setBins(50);
  eOverP.setRange("fitGauss",0.5,1.5);
  eOverP.setRange("fitLandau",0.5,1.5);
  eOverP.setRange("final",0.0,3.0);
  eOverP.setRange("finalFit",0.8,2.5);

  //----------
  // Set up separate eta/phi binned data sets for positive/negative electrons
  //----------
  // get number of bins in eta(x)/phi(y)
  static const int nBinsX = meanPlotPos->GetXaxis()->GetNbins();
  static const int nBinsY = meanPlotPos->GetYaxis()->GetNbins();

  RooAbsData* absDataSetPos = originalDataSet->reduce( Cut("charge > 0"+ additionalCut) );
  RooAbsData* absDataSetNeg = originalDataSet->reduce( Cut("charge < 0"+ additionalCut) );
  RooDataSet* dataSetPos = dynamic_cast<RooDataSet*>( absDataSetPos ); // positron data set
  RooDataSet* dataSetNeg = dynamic_cast<RooDataSet*>( absDataSetNeg ); // electron data set
  RooArgSet   vars(xvar,yvar,eOverP);
  
  RooDataSet* binnedDataPos[nBinsX][nBinsY]; // positron data set broken up by eta/phi bin
  RooDataSet* binnedDataNeg[nBinsX][nBinsY]; // electron data set broken up by eta/phi bin

  // name each of our eta/phi bins in binnedDataPos/Neg
  for(int xvarStep = 0; xvarStep<nBinsX; xvarStep++){ // eta
    for(int yvarStep = 0; yvarStep<nBinsY; yvarStep++){ // phi
    
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

  //----------
  // Fill pos/neg binned data sets
  //----------
  // positive
  for (int i=0; i<dataSetPos->numEntries(); i++) {
    const RooArgSet* args = dataSetPos->get(i);
    vars= *args;
    int globalbin = meanPlotPos->FindBin(xvar.getVal(),yvar.getVal());
    int binx,biny,binz;
    meanPlotPos->GetBinXYZ(globalbin,binx,biny,binz);
    binnedDataPos[binx-1][biny-1]->add(*args);
  }
  // negative
  for (int i=0; i<dataSetNeg->numEntries(); i++) {
    const RooArgSet* args = dataSetNeg->get(i);
    vars= *args;
    int globalbin = meanPlotNeg->FindBin(xvar.getVal(),yvar.getVal());
    int binx,biny,binz;
    meanPlotNeg->GetBinXYZ(globalbin,binx,biny,binz);
    binnedDataNeg[binx-1][biny-1]->add(*args);
  }

  //----------
  // Time to make the donuts
  //----------
  for(int xvarStep = 0; xvarStep<nBinsX; xvarStep++){ // eta
    for(int yvarStep = 0; yvarStep<nBinsY; yvarStep++){ // phi
      // some naming like we did before
      TString rangeName = xvar.getTitle() + "Bin";
      rangeName += xvarStep;
      rangeName += "_"+yvar.getTitle()+"Bin";
      rangeName += yvarStep;

      // set up the framse for pos/neg plots
      RooPlot* framePos = eOverP.frame(Title("E/P pos"),Range("final"));
      RooPlot* frameNeg = eOverP.frame(Title("E/P neg"),Range("final"));

      // make data sets from the relevent bin of the binned sets to be easier to work with
      RooDataSet* smallDataSetPos = binnedDataPos[xvarStep][yvarStep];
      RooDataSet* smallDataSetNeg = binnedDataNeg[xvarStep][yvarStep];

      // plot the smallDataSet on the corresponding frame: -- Pos(Blue) & Neg(Red)
      smallDataSetPos->plotOn(framePos,LineColor(kBlue),MarkerColor(kBlue));
      smallDataSetNeg->plotOn(frameNeg,LineColor(kRed),MarkerColor(kRed));

      // reset the fit paramaters
      meanPos.setVal(1);
      meanNeg.setVal(1);
      sigma.setVal(0.1);
      alpha.setVal(-1);
      n.setVal(1);

      //----------
      // Set up samples
      //----------
      RooCategory sample("sample","sample"); // separate positive and negative
      sample.defineType("Pos");
      sample.defineType("Neg");
      RooDataSet combData("combData","combined data",eOverP,Index(sample),Import("Pos",*smallDataSetPos),Import("Neg",*smallDataSetNeg)); // combined data set in (x, sample)

      //----------
      // Set up simultaneous pdfs in (x,sample)
      //----------
      // gaussian
      RooSimultaneous simPdfGS("simPdfGS","simultaneous pdf",sample);
      simPdfGS.addPdf(gaussianPos,"Pos");
      simPdfGS.addPdf(gaussianNeg,"Neg");
      // cristal ball
      RooSimultaneous simPdfCB("simPdfCB","simultaneous pdf",sample);
      simPdfCB.addPdf(crystalBallPos,"Pos");
      simPdfCB.addPdf(crystalBallNeg,"Neg");
      // landau
      RooSimultaneous simPdfLnd("simPdfLnd","simultaneous pdf",sample);
      simPdfLnd.addPdf(LandauPos,"Pos");
      simPdfLnd.addPdf(LandauNeg,"Neg");
      // landau-gaussian convolution
      RooSimultaneous simPdfLcwG("simPdfLcwG","simultaneous pdf",sample);
      simPdfLcwG.addPdf(LcwGPos,"Pos");
      simPdfLcwG.addPdf(LcwGNeg,"Neg");

      //----------
      // Perform simultaneous fit
      //----------
      // some fit vars
      double fitMeanPos = -999.;
      double fitMeanNeg = -999.;
      double fitSigma   = -999.;

      // set up ranges for first fit
      eOverP.setRange("fitGauss_Pos",0.5,1.5);
      eOverP.setRange("fitGauss_Neg",0.5,1.5);
      // fit the gaussian & check if it produced a result
      RooFitResult* fitResult = simPdfGS.fitTo(combData,Range("fitGauss"),SplitRange(),Save());
      if(fitResult)
        std::cout << "WARNING:: FIRST GAUSSIAN FIT PRODUCED A FIT RESULT" <<std::endl;        
      else
        std::cout << "WARNING:: FIRST GAUSSIAN FIT DID NOT PRODUCE A FIT RESULT" <<std::endl;        
      // update fit vars
      fitMeanPos = meanPos.getVal();
      fitMeanNeg = meanNeg.getVal();
      fitSigma = sigma.getVal();
      // set up for 2nd fit
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
      // will ---
      
      fitResult = simPdfGS.fitTo(combData,Range("fitGauss"),SplitRange(),Save() );
      if ( fitResult && fitResult->status() != 0){
        std::cout << "WARNING::SECOND GAUSSIAN FIT DID NOT CONVERGE" <<std::endl;
        meanPos.setVal(1);
        meanNeg.setVal(1);
        sigma.setVal(0.2);
      }
      
      //if (fitResult) delete fitResult;
      
      /*
      // do some more fits 
      int counter = 0;
      do{
	if(counter > 5){
	  std::cout<< "WARNING:: REACHED MAX FIT ATTEMPTS WITH NO CONVERGENCE" << std::endl;
	  break;
	}
	std::cout << "INFO:: TRYING GAUSSIAN FIT #"<< counter+2  << std::endl;

	fitResult = simPdfGS.fitTo(combData,Range("fitGauss"),SplitRange(),Save() );
	if ( fitResult && fitResult->status() != 0){
	  std::cout << "WARNING:: GAUSSIAN FIT #"<< counter+2 << " DID NOT CONVERGE" <<std::endl;
	  meanPos.setVal(1.);
	  meanNeg.setVal(1.);
	  sigma.setVal(0.2+0.2*counter);
	}
	counter++;
	} while(fitResult && fitResult->status() != 0);*/
      // --- will

      if (fitResult) delete fitResult;
      fitMeanPos = meanPos.getVal();
      fitMeanNeg = meanNeg.getVal();
      fitSigma = sigma.getVal();
     
      eOverP.setRange("fitGauss_Pos",fitMeanPos-1.25*fitSigma,fitMeanPos+1*fitSigma);      
      eOverP.setRange("fitGauss_Neg",fitMeanNeg-1.25*fitSigma,fitMeanNeg+1*fitSigma);      

      // do final fit
      fitResult = simPdfGS.fitTo(combData,Range("fitGauss"),SplitRange(), Save() );
      if ( fitResult && fitResult->status() != 0){
        std::cout << "WARNING::FINAL GAUSSIAN FIT DID NOT CONVERGE" <<std::endl;
        meanPos.setVal(1);
        meanNeg.setVal(1);
        sigma.setVal(0.2);
        fitSigma = 0.3;
      }
      if(fitResult) delete fitResult;
      
      //std::cout << "INFO: meanPos = " << meanPos.getVal() << " meanNeg = " << meanNeg.getVal() << " sigma = " << sigma.getVal() << std::endl;

      /*
      //Fit with Landau
      fitResult = simPdfLnd.fitTo(combData, Range("fitLandau"),SplitRange(), Save() ) ;
       if ( fitResult && fitResult->status() != 0){
        std::cout << "WARNING::TEST LANDAU FIT DID NOT CONVERGE" <<std::endl;
        LandauMeanPos.setVal(1);
        LandauMeanNeg.setVal(1);
        LandauS.setVal(0.2);
        //fitSigma = 0.3;
      }
      if(fitResult) delete fitResult;
      
      fitMeanPos = (meanPos.getVal()+LandauMeanPos.getVal())/2.;
      fitMeanNeg = (meanNeg.getVal()+LandauMeanNeg.getVal())/2.;
      fitSigma = sigma.getVal() > LandauS.getVal() ? sigma.getVal() : LandauS.getVal();
      
      std::cout << "INFO: meanPos = " << meanPos.getVal() << " meanNeg = " << meanNeg.getVal() << " sigma = " << sigma.getVal() << std::endl;
      std::cout << "INFO: LandauMeanPos = " << LandauMeanPos.getVal() << " LandauMeanNeg = " << LandauMeanNeg.getVal() << " LandauS = " << LandauS.getVal() << std::endl;
      std::cout << "INFO: Fit Mean Pos: " << fitMeanPos << " Fit Mean Pos: " << fitMeanNeg << "Fit Sigma: " << fitSigma << std::endl;
      */

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
      //LandauMeanPos.setVal(1.);//LandauMeanPos.setConstant(kTRUE);
      //LandauMeanNeg.setVal(1.);//LandauMeanNeg.setConstant(kTRUE);     
      //meanPos.setVal(0.);meanPos.setConstant(kTRUE);
      //meanNeg.setVal(0.);meanNeg.setConstant(kTRUE);
      //sigma.setVal(0.2);    
      

//***** Choose one, CB, Landau or LcwG
      //Fit with CrystalBall   
      alpha.setVal(1.2);
      meanPos.setVal(1.);
      meanNeg.setVal(1.);
      fitResult = simPdfCB.fitTo(combData, Range("finalFit"),SplitRange(), Save() ) ;
      crystalBallPos.plotOn(framePos,LineColor(kAzure-2),Range("finalFit_Pos"),NormRange("finalFit_Pos"));
      crystalBallNeg.plotOn(frameNeg,LineColor(kRed-7),Range("finalFit_Neg"),NormRange("finalFit_Neg")); 


      //Fit with Landau
    /*  fitResult = simPdfLnd.fitTo(combData, Range("finalFit"),SplitRange(), Save() ) ;
	LandauPos.plotOn(framePos,LineColor(kOrange+1),Range("finalFit_Pos"),NormRange("finalFit_Pos"));
        LandauNeg.plotOn(frameNeg,LineColor(kGreen+1),Range("finalFit_Neg"),NormRange("finalFit_Neg")); */

      
      //Fit with Landau convoluted with Gaussian
      if(false){
	int simCount = 0;
	do{
	  if(simCount > 5) break;
	  fitResult = simPdfLcwG.fitTo(combData, Range("finalFit"),SplitRange(), Save() ) ;
	  simCount++;
	} while(fitResult && ( fitResult->status() != 0  || fitResult->covQual() != 3 ));
	
	LcwGPos.plotOn(framePos,LineColor(kAzure-2),Range("finalFit_Pos"),NormRange("finalFit_Pos"));
	LcwGNeg.plotOn(frameNeg,LineColor(kRed-7),Range("finalFit_Neg"),NormRange("finalFit_Neg")); 
      }

      // hold on
      if (false) {
	string input = "";
	cout << " ** fit_EoverP ** Please type RETURN to continue:\n>";
	getline(cin, input);
      }

//*****
      
      meanPlotPos->SetBinContent(xvarStep+1,yvarStep+1,meanPos.getVal());
      meanPlotPos->SetBinError  (xvarStep+1,yvarStep+1,meanPos.getError());
      meanPlotNeg->SetBinContent(xvarStep+1,yvarStep+1,meanNeg.getVal());
      meanPlotNeg->SetBinError  (xvarStep+1,yvarStep+1,meanNeg.getError());

      if( fitResult && ( fitResult->status() != 0  || fitResult->covQual() != 3 )  ){
        std::cout << "WARNING::FINAL SIM FIT DID NOT CONVERGE FULLY" <<std::endl;
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
                                  
      myMarkerText(0.65,0.85,kBlue,20,Form("e^{+},  <E/p> %01.3f #pm %01.3f",meanPlotPos->GetBinContent(xvarStep+1,yvarStep+1), meanPlotNeg->GetBinError(xvarStep+1,yvarStep+1) ),1.0,0.03);
      myMarkerText(0.65,0.80,kRed,20,Form("e^{-},  <E/p> %01.3f #pm %01.3f",meanPlotNeg->GetBinContent(xvarStep+1,yvarStep+1), meanPlotPos->GetBinError(xvarStep+1,yvarStep+1) ),1.0,0.03);
      
      
      c->Print(PATH+"new_eoverp_plots.pdf");
      
      delete framePos;
      delete frameNeg;
      delete smallDataSetPos;
      delete smallDataSetNeg;
    }
  }
  
  makeSlicePlots(meanPlotPos ,meanPlotNeg, false);
  makeSlicePlots(meanPlotPos ,meanPlotNeg, true);
  
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
  
  c->Print(PATH+"new_eoverp_plots.pdf");
  
  meanPlotPos->Draw("colz");
  meanPlotPos->SetContour(nb);

  //Set Z axis centered in 1 so good points are in green
  minPlot= meanPlotPos->GetMinimum();
  maxPlot= meanPlotPos->GetMaximum();
  if (maxPlot-1 > 1-minPlot) meanPlotPos->SetMinimum(2-maxPlot);
  else meanPlotPos->SetMaximum(2-minPlot);

  c->Print(PATH+"new_eoverp_plots.pdf");
  
  meanPlotNeg->Draw("colz");
  meanPlotNeg->SetContour(nb);

  //Set Z axis centered in 1 so good points are in green
  minPlot= meanPlotNeg->GetMinimum();
  maxPlot= meanPlotNeg->GetMaximum();
  if (maxPlot-1 > 1-minPlot) meanPlotNeg->SetMinimum(2-maxPlot);
  else meanPlotNeg->SetMaximum(2-minPlot);

  c->Print(PATH+"new_eoverp_plots.pdf");

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
  
  c->Print(PATH+"new_eoverp_plots.pdf");

  
  delete clone;
  delete clone2;
  
  
}


void makeSlicePlots(TH2F* pos, TH2F* neg, bool sliceInEta)
{
  std::cout << "INFO::makeSlicePlots()" << std::endl;

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
    
    
    
    makeRatioPlots(posProjection, negProjection, name );
    delete posProjection;
    delete negProjection;
  }  
  std::cout << "INFO::Finished makeSlicePlots()" << std::endl;
}




void makeRatioPlots(TH1D* h1, TH1D* h2, TString name) {
   std::cout << "INFO::Starting makeRatioPlots()" << std::endl;
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
   h1clone->GetYaxis()->SetRangeUser(0.8,1.2);
   h1clone->GetYaxis()->SetTitle("<E/p>^{+}/<E/p>^{-}");
 
   h1clone->Draw("ep");
   //h1->GetYaxis()->SetRangeUser(0.95,1.05);
   c1.cd();

   // legend title setup
   TString title;
   if(name.Contains("Y",TString::kExact))
     title += "Phi slice ";
   else
     title += "Eta slice ";
   title += name[name.Length()-1];

   TLegend *leg1 = new TLegend(0.75,0.82,0.9,0.92);
   leg1->SetBorderSize(0);
   leg1->SetFillColor(0);
   leg1->AddEntry("",title,"");
   leg1->AddEntry(h1,"Positive","p");
   leg1->AddEntry(h2,"Negative","p");
   leg1->Draw();

   //c1.Print( PATH + name+".pdf");
   c1.Print(PATH+"new_eoverp_plots.pdf");
   delete h1clone;
   delete pad1;
   delete pad2;
   std::cout << "INFO::Finished makeRatioPlots()" << std::endl;

}

void initialize(){
  // Set up ATLAS style and other things before we get started
  gROOT->LoadMacro("AtlasStyle.C");
  SetAtlasStyle();
  gROOT->SetBatch();
  return;
}

TH2F* makeCorrectionHist(){
  TH2F *FinalCorrections = new TH2F("FinalCorrections","FinalCorrections",16,-2.5,2.5,16,-3.14159,3.14159);
  return FinalCorrections;
}

double getCorrections(TH2F* corrections, double eta, double phi, double Et, double charge){
  //Q/p_constr=Q/p_recon*(1 + pT_recon*Q*delta)
  if(!corrections)
    return 1;

  
  std::cout << "INFO :: GetCorrection -> Bin number "; //<< std::endl;
  
  int binNumber = corrections->FindBin(eta, phi);

  //std::cout<< binNumber << std::endl;
  

  std::cout << "INFO :: GetCorrection -> delta" << std::endl;
  double delta = corrections->GetBinContent(binNumber); 
  
  return 1+charge*Et*delta;  
}

void makeScalingHist(TString inputDir)
{
  std::cout << "INFO :: In makeScalingHist()" << std::endl;

  TString dir = PATH;
  if(inputDir != "") dir  = inputDir;
  else{
    std::cout << "ERROR :: makeScalingHist(): No input directory specified!" << std::endl;
    return;
  }
  
  TFile *file0 = TFile::Open(dir+"fitEoverP.root");
  file0->ls();
  TH2F* meanPlotNeg = (TH2F*) file0->Get("meanPlotNeg");
  TH2F* meanPlotPos = (TH2F*) file0->Get("meanPlotPos");


  if(!meanPlotNeg || !meanPlotNeg){
    std::cout<< "ERROR :: makeScalingHist(): inputfile does not contain the desired histograms "  << std::endl; 
    return;
  }
  
  meanPlotNeg->Draw("colz");
  meanPlotPos->Draw("colz");
  meanPlotNeg->Sumw2();
  meanPlotPos->Sumw2();


  //Need to obtain the mean pt either from a histogram or from hardcoded values 
  double pt[1000];

  TH2F* meanPtVsEta =  (TH2F*) file0->Get("meanPtVsEta");
  if(!meanPtVsEta){
    std::cout << "ERROR :: makeScalingHist(): inputfile does not contain the meanPtVsEta histogram "  << std::endl; 
    return;
  }
  for(int i=1; i<=meanPtVsEta->GetNbinsX(); i++){
    TH1D* projection =  meanPtVsEta->ProjectionY( "_py", i , i);
    pt[i-1] = (projection->GetMean()*1000.); //MeV
    delete projection;
  }  
  

  TH2F* a = (TH2F*) meanPlotPos->Clone(); 
  a->Add(meanPlotNeg,-1);
  a->Scale(-0.5); // divide by 2

  for(int i=1; i <= a->GetNbinsX(); i++){
    for(int j=1; j <= a->GetNbinsY(); j++){
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
