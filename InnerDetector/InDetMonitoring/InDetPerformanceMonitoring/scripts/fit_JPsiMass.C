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
#include "RooArgusBG.h"



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

TCanvas* cJPsiMassFit_Pos;
TCanvas* cJPsiMassFit_Neg;
TCanvas* cJPsiMassFitzoom;
TCanvas* cJPsiMassFitIndividual;
TCanvas* cJPsiMassVsEtaPhi_PosMinusNeg;
TCanvas* cJPsiSigmaVsEtaPhi_Pos;
TCanvas* cJPsiSigmaVsEtaPhi_Neg;
TCanvas* cJPsiMassVsEtaPhi_Pos;
TCanvas* cJPsiMassVsEtaPhi_Neg;
TCanvas* cFitComparative_Neg;
TCanvas* cJPsiMassVsEta_Pos;

TH2D* MassFitResult_Pos;
TH2D* MassFitResult_Neg;
TH2D* MassFitSigma_Pos;
TH2D* MassFitSigma_Neg;

TH2D* SmallMassFitResult_Pos;
TH2D* SmallMassFitResult_Neg;

TH1D* MassFitResultVsEta_Pos;
TH1D* MassFitResultVsEta_Neg;
	
TH1D* FitComparative_Pos;
TH1D* FitComparative_Neg;
	
TH2D* MassFitAloneResult_Pos;
TH2D* MassFitAloneResult_Neg;

TH1D* MassFitAloneResultVsEta_Pos;
TH1D* MassFitAloneResultVsEta_Neg;
	
TF1* constant;


bool fit_JPsiMass()
{

//  	SetAtlasStyle();
   
	Double_t JPsiPDGMass = 3.096916;
	Double_t JPsiPDGSmallMass = 3.686109;

	cJPsiMassFit_Pos = new TCanvas("cJPsiMassFit_Pos","JPsi Mass Fit",1100, 750);
	cJPsiMassFit_Neg = new TCanvas("cJPsiMassFit_Neg","JPsi Mass Fit",1100, 750);
	cJPsiMassFitzoom = new TCanvas("cJPsiMassFitzoom","JPsi Mass Fit",1400, 600);
	   					 
	cout << "Opening file" << endl;
	TString FileName = "/afs/cern.ch/work/j/jajimene/public/Athena/20.1.9.2/InnerDetector/InDetMonitoring/InDetPerformanceMonitoring/scripts/JpsimumuValidationOutputFiles/Pruebas2.root";
	TFile *file;
	file = new TFile(FileName);
	
	if (file==NULL) {
		cout << "Error opening file!!" << endl;
		return false;     
	}
	
	cout << "File opened successfully." << endl;

	TString histnamePos = "Iteration1/JpsiMassVsEtaPhi_Pos";
	TString histnameNeg = "Iteration1/JpsiMassVsEtaPhi_Neg";
/*
	gStyle->SetPadRightMargin(0.15);
	gStyle->SetPadLeftMargin(0.08);
	gStyle->SetPadTopMargin(0.12);
	gROOT->ForceStyle();*/

	TH3F *h_JPsiMassVsEtaPhi_Pos;
	TH3F *h_JPsiMassVsEtaPhi_Neg;

	RooRealVar Mass_Pos("Mass_Pos","Mass [GeV]",2.52,4.28,"");
	RooRealVar Mass_Neg("Mass_Neg","Mass [GeV]",2.52,4.28,"");
	RooRealVar Eta("Eta","#eta",-2.5,2.5,"");
	RooRealVar Phi("Phi","#phi",-3.1416,3.1416);

	RooArgSet myarg;

	myarg.add(Mass_Pos);
	myarg.add(Mass_Neg);
	myarg.add(Eta);
	myarg.add(Phi);
	
	h_JPsiMassVsEtaPhi_Pos = (TH3F*)file->Get(histnamePos); 
	h_JPsiMassVsEtaPhi_Neg = (TH3F*)file->Get(histnameNeg); 
	cout << "Aqyui?." << endl;

	int EtaPoints = h_JPsiMassVsEtaPhi_Pos->GetXaxis()->GetNbins();
	int PhiPoints = h_JPsiMassVsEtaPhi_Pos->GetYaxis()->GetNbins();
	cout << "EtaPoints = " << EtaPoints << endl;
	cout << "PhiPoints = " << PhiPoints << endl;

	cJPsiMassFit_Pos->Divide(EtaPoints,PhiPoints);
	cJPsiMassFit_Neg->Divide(EtaPoints,PhiPoints);
	cout << "Aqyui?." << endl;
	cJPsiMassFitzoom->Divide(EtaPoints/2,PhiPoints/4);


	MassFitResult_Pos = new TH2D ("MassFitResult_Pos","(M_{Exp}-M_{PDG})[MeV] (Positive muons)", EtaPoints, -2.5,2.5, PhiPoints, -3.1416,3.1416);
	MassFitResult_Neg = new TH2D ("MassFitResult_Neg","(M_{Exp}-M_{PDG})[MeV] (Negative muons)", EtaPoints, -2.5,2.5, PhiPoints, -3.1416,3.1416);

	MassFitSigma_Pos = new TH2D ("MassFitSigma_Pos","#sigma [MeV]", EtaPoints, -2.5,2.5, PhiPoints, -3.1416,3.1416);
	MassFitSigma_Neg = new TH2D ("MassFitSigma_Neg","#sigma [MeV] (Negative muons)", EtaPoints, -2.5,2.5, PhiPoints, -3.1416,3.1416);

	SmallMassFitResult_Pos = new TH2D ("SmallMassFitResult_Pos","(SmallM_{Exp}-SmallM_{PDG})[MeV] (Positive muons)", EtaPoints, -2.5,2.5, PhiPoints, -3.1416,3.1416);
	SmallMassFitResult_Neg = new TH2D ("SmallMassFitResult_Neg","(SmallM_{Exp}-SmallM_{PDG})[MeV] (Negative muons)", EtaPoints, -2.5,2.5, PhiPoints, -3.1416,3.1416);

	MassFitResultVsEta_Pos = new TH1D ("MassFitResultVsEta_Pos","(M_{Exp}-M_{PDG})[MeV]", EtaPoints, -2.5,2.5);
	MassFitResultVsEta_Neg = new TH1D ("MassFitResultVsEta_Neg","(M_{Exp}-M_{PDG})[MeV]", EtaPoints, -2.5,2.5);
	
	FitComparative_Pos = new TH1D ("FitComparative_Pos","FitComparative_Pos", 15, -0.002,0.005);
	FitComparative_Neg = new TH1D ("FitComparative_Neg","FitComparative_Neg", 15, -0.002,0.005);
	
	MassFitAloneResult_Pos = new TH2D ("MassFitAloneResult_Pos","(M_{Exp}-M_{PDG})[MeV] (Positive muons)", EtaPoints, -2.5,2.5, PhiPoints, -3.1416,3.1416);
	MassFitAloneResult_Neg = new TH2D ("MassFitAloneResult_Neg","(M_{Exp}-M_{PDG})[MeV] (Negative muons)", EtaPoints, -2.5,2.5, PhiPoints, -3.1416,3.1416);

	MassFitAloneResultVsEta_Pos = new TH1D ("MassFitAloneResultVsEta_Pos","(M_{Exp}-M_{PDG})[MeV]", EtaPoints, -2.5,2.5);
	MassFitAloneResultVsEta_Neg = new TH1D ("MassFitAloneResultVsEta_Neg","(M_{Exp}-M_{PDG})[MeV]", EtaPoints, -2.5,2.5);
	cout << "Aqyui?." << endl;
	
	TLatex l;
	l.SetTextSize(0.035);
	TLatex l2;
	l2.SetTextSize(0.028);
	TLatex l3;
	l3.SetTextSize(0.038);
	
	float fittingWidth=0.2;
	//h_JPsiMassVsEtaPhi_Pos->Draw();

	int Etabin=1;
	int Phibin=1;

	for (int i = 1; i<=EtaPoints; i++){
		for (int j =1; j<=PhiPoints; j++){

						


			//Get the Mass spectrum for each eta and phi bins.
			//TH1D *h_JPsiMassPos= h_JPsiMassVsEtaPhi_Pos->ProjectionZ("Mass_Pos",0,-1,0,-1);
			TH1D *h_JPsiMassPos= h_JPsiMassVsEtaPhi_Pos->ProjectionZ("Mass_Pos",i,i,j,j);
			
			// Create a binned dataset that imports contents of TH1 and associates its contents to observable 'x'
			RooDataHist dh_Mass_Pos("dh_Mass_Pos","dh JPsi Mass Pos",Mass_Pos,Import(*h_JPsiMassPos)) ;
			
			// Make plot of binned dataset showing Poisson error bars (RooFit default)
			RooPlot* Mass_Posframe = Mass_Pos.frame(Title("Imported TH1 with Poisson error bars")) ;
			dh_Mass_Pos.plotOn(Mass_Posframe,MarkerColor(kRed+1),LineColor(kRed+1),MarkerStyle(7)) ;


			// Define "smallPeak" range in Mass:
			Mass_Pos.setRange("SmallPeak_Pos",3.55,3.8) ;

			
			// Define "JPsiPeak" range in Mass:
			//Mass_Pos.setRange("JPsiPeak_Pos",3.01,3.18) ;

			Mass_Pos.setRange("JPsiPeak_Pos",JPsiPDGMass-0.1,JPsiPDGMass+0.1) ;

			// Define "Background" range in Mass:
			Mass_Pos.setRange("Background_Hi_Pos",3.85,4.28) ;
			Mass_Pos.setRange("Background_fi_Pos",2.52,2.84) ;
			Mass_Pos.setRange("Background_Lo_Pos",3.35,3.55) ;

			// Define "Argus Background" range in Mass:
			Mass_Pos.setRange("signal_Pos",2.52,3.43) ;
			
			Mass_Pos.setRange("WholeRange_Pos",2.52,4.28) ;
						
			//Fit the mass spectrum

			//Gauss Peak
			RooRealVar mean_Pos("mean_Pos","mean_Pos",3.0,3.2) ;
			RooRealVar sigma_Pos("sigma_Pos","sigma_Pos",0,0.4) ;
			RooGaussian gauss_Pos("gauss","gauss",Mass_Pos,mean_Pos,sigma_Pos) ;
			gauss_Pos.fitTo(dh_Mass_Pos,Range("JPsiPeak_Pos")) ;

			//obtain the num of events due to the peak for the whole range-> NormGauss_Pos
			RooRealVar NormGauss_Pos("NormGauss_Pos","NormGauss_Pos",3000,0,1000000);

			RooExtendPdf NormGaussFunc_Pos("NormGaussFunc_Pos","NormGaussFunc_Pos",gauss_Pos, NormGauss_Pos,"WholeRange_Pos");
			NormGaussFunc_Pos.fitTo(dh_Mass_Pos,Range("JPsiPeak_Pos"));

			
			//Gauss for second Peak 
			RooRealVar smallmean_Pos("smallmean_Pos","small_mean_Pos",3.68,3.55,3.8) ;
			RooRealVar smallsigma_Pos("smallsigma_Pos","small_sigma_Pos",0,0.4) ;
			RooGaussian smallgauss_Pos("smallgauss","smallgauss",Mass_Pos,smallmean_Pos,smallsigma_Pos) ;
			smallgauss_Pos.fitTo(dh_Mass_Pos,Range("SmallPeak_Pos")) ;
			//smallgauss_Pos.plotOn(Mass_Posframe,LineColor(kCyan),LineWidth(2));
			
			//linear background
			//First, fit background at the right part of the plot wit a linear function			
			RooRealVar aa_Pos("aa_Pos","aa_Pos",0.,-500.,500.);
			RooRealVar bb_Pos("bb_Pos","bb_Pos",0.,-100.,100.);
			RooRealVar cc_Pos("cc_Pos","cc_Pos",0.,-100.,100.);
			RooPolynomial bkgdpoly_Pos("poly_Pos","2th order poly",Mass_Pos,RooArgList(aa_Pos,bb_Pos));
			//RooPolynomial bkgdpoly_Pos("poly_Pos","3th order poly",Mass_Pos,RooArgList(aa_Pos,bb_Pos,cc_Pos));
			bkgdpoly_Pos.fitTo(dh_Mass_Pos,Range("Background_Lo_Pos,Background_Hi_Pos,Background_fi_Pos"));
			//bkgdpoly_Pos.plotOn(Mass_Posframe,LineColor(kGreen),LineWidth(2));
			
			
			//obtain the num of events due to the background for the whole range-> Norm_Pos
			RooRealVar Norm_Pos("Norm_Pos","Norm_Pos",3000,0,1000000);

			RooExtendPdf NormPol_Pos("NormPol_Pos","NormPol_Pos",bkgdpoly_Pos, Norm_Pos,"WholeRange_Pos");
			NormPol_Pos.fitTo(dh_Mass_Pos,Range("Background_Lo_Pos,Background_Hi_Pos"));
	
			//bb_Pos.setConstant(kTRUE);
			//aa_Pos.setConstant(kTRUE);
			
			// gaussian + background **

			//Argus function

			RooRealVar argpar_Pos("argpar_Pos","argus_Pos shape parameter",-1,-Norm_Pos.getVal()/1500.,-1.) ;
			RooRealVar argpeak_Pos("argpeak_Pos","argus_Pos peak parameter",3.23,3.05,3.3) ;
			RooArgusBG argusBackground_Pos("Argus_Pos_background","Argus_Pos PDF",Mass_Pos,argpeak_Pos,argpar_Pos) ;
			
			//different weights of gauss, poly and argus:
			RooRealVar nbkg_Pos("nbkg_Pos","bkg_Pos Integral",Norm_Pos.getVal(),Norm_Pos.getVal()*0.60,Norm_Pos.getVal()*1.4) ;
			RooRealVar nsig_Pos("nsig_Pos","#signal events",10000,0.,3000000) ;
			RooRealVar nsmallsig_Pos("nsmallsig_Pos","#small signal events",200,0.,1000000) ;
			RooRealVar nargus_Pos("nargus_Pos","#background events",0,0.,Norm_Pos.getVal()/5) ;
			
			//Construct model = gauss + linear bkg + argus bkg
			RooAddPdf model_Pos("model_Pos","g+a+b+g",RooArgList(gauss_Pos,bkgdpoly_Pos,argusBackground_Pos,smallgauss_Pos),RooArgList(nsig_Pos,nbkg_Pos,nargus_Pos,nsmallsig_Pos));
			//RooAddPdf model_Pos("model_Pos","g+a+b+g",RooArgList(gauss_Pos,bkgdpoly_Pos,argusBackground_Pos),RooArgList(nsig_Pos,nbkg_Pos,nargus_Pos));
			model_Pos.fitTo(dh_Mass_Pos,Range("WholeRange_Pos"));
			model_Pos.plotOn(Mass_Posframe,LineColor(kGreen),LineWidth(2));
			model_Pos.plotOn(Mass_Posframe,LineColor(kOrange),LineWidth(1),Components(argusBackground_Pos),LineStyle(kDashed));
			model_Pos.plotOn(Mass_Posframe,LineColor(kRed),LineWidth(1),Components(bkgdpoly_Pos),LineStyle(kDashed));
			model_Pos.plotOn(Mass_Posframe,LineColor(kViolet),LineWidth(1),Components(gauss_Pos),LineStyle(kDashed));
			model_Pos.plotOn(Mass_Posframe,LineColor(kCyan),LineWidth(1),Components(smallgauss_Pos),LineStyle(kDashed));
			
			
			//alternative model based in a crystall ball to cross-check 
/*
			RooRealVar alpha_Pos("#alpha_{Pos}","alpha_Pos",-1,-100,-0.0001,"") ;
			RooRealVar n_Pos("n_{Pos}","n_Pos",1,0.1,1000,"") ;
  
			RooCBShape  crystalBall_Pos("PosCB", "Crystal Ball shape", Mass_Pos, mean_Pos, sigma_Pos, alpha_Pos, n_Pos);
			crystalBall_Pos.fitTo(dh_Mass_Pos,Range("JPsiPeak_Pos")) ;

			RooAddPdf CBmodel_Pos("CBmodel_Pos","CB+b+g",RooArgList(crystalBall_Pos,bkgdpoly_Pos,smallgauss_Pos),RooArgList(nsig_Pos,nbkg_Pos,nsmallsig_Pos));
			CBmodel_Pos.fitTo(dh_Mass_Pos,Range("WholeRange_Pos"));
			CBmodel_Pos.plotOn(Mass_Posframe,LineColor(kGreen),LineWidth(2));
			CBmodel_Pos.plotOn(Mass_Posframe,LineColor(kRed),LineWidth(1),Components(bkgdpoly_Pos),LineStyle(kDashed));
			CBmodel_Pos.plotOn(Mass_Posframe,LineColor(kViolet),LineWidth(1),Components(crystalBall_Pos),LineStyle(kDashed));
			CBmodel_Pos.plotOn(Mass_Posframe,LineColor(kCyan),LineWidth(1),Components(smallgauss_Pos),LineStyle(kDashed));
			//end of crystall ball
*/

			//Construct model = gauss + linear bkg (FIT only the peak)
			fittingWidth=2.0* sigma_Pos.getVal();
			//if (fittingWidth >= 0.05) fittingWidth=0.05;
			Mass_Pos.setRange("JPsiAlonePeak_Pos",JPsiPDGMass-fittingWidth,JPsiPDGMass+fittingWidth) ;

			bb_Pos.setConstant(kTRUE);
			aa_Pos.setConstant(kTRUE);

			RooRealVar Alonemean_Pos("Alonemean_Pos","Alone mean_Pos",mean_Pos.getVal()*0.9,mean_Pos.getVal()*1.1) ;
			RooRealVar Alonesigma_Pos("Alone sigma_Pos","Alone sigma_Pos",sigma_Pos.getVal()*0.9,sigma_Pos.getVal()*1.1) ;
			RooGaussian Alonegauss_Pos("Alonegauss","Alone gauss",Mass_Pos,Alonemean_Pos,Alonesigma_Pos) ;
			Alonegauss_Pos.fitTo(dh_Mass_Pos,Range("JPsiAlonePeak_Pos")) ;

			RooRealVar nbkgPeak_Pos("nbkgPeak_Pos","bkgPeak_Pos Integral",nbkg_Pos.getVal());
			RooRealVar nsigAlone_Pos("nsigAlone_Pos","#signal events",10000,0.,3000000) ;

			RooAddPdf Peakmodel_Pos("Peakmodel_Pos","g+b",RooArgList(Alonegauss_Pos,bkgdpoly_Pos),RooArgList(nsigAlone_Pos,nbkgPeak_Pos));
			Peakmodel_Pos.fitTo(dh_Mass_Pos,Range("JPsiAlonePeak_Pos"));
			Peakmodel_Pos.plotOn(Mass_Posframe,LineColor(kViolet),LineWidth(2));
			//Peakmodel_Pos.plotOn(Mass_Posframe,LineColor(kGreen),LineWidth(2),Components(bkgdpoly_Pos),LineStyle(kDashed));
			//Peakmodel_Pos.plotOn(Mass_Posframe,LineColor(kViolet),LineWidth(1),Components(gauss_Pos),LineStyle(kDashed));

			FitComparative_Pos->Fill(mean_Pos.getVal()-Alonemean_Pos.getVal());


			//Fill the different histograms with the result of the fits
			cJPsiMassFit_Pos->cd(i+(j-1)*(EtaPoints)); 
			Mass_Posframe->Draw();
			
			if (((i+(j-1)*(EtaPoints))>2*(EtaPoints))&&((i+(j-1)*(EtaPoints))<=3*(EtaPoints)))
			{	
				cJPsiMassFitzoom->cd(i+(j-1)*(EtaPoints)-2*(EtaPoints)); 
				Mass_Posframe->Draw();
			}

			
			if ((i==Etabin)&&(j==Phibin))
			{
				cJPsiMassFitIndividual = new TCanvas("cJPsiMassFitIndividual","JPsi Mass Fit",900, 700);
				Mass_Posframe->Draw();
			}	

			MassFitResult_Pos->SetBinContent(i,j,(mean_Pos.getVal()-JPsiPDGMass)*1000);
			MassFitResult_Pos->SetBinError(i,j,(mean_Pos.getError())*1000);
			
			MassFitAloneResult_Pos->SetBinContent(i,j,(Alonemean_Pos.getVal()-JPsiPDGMass)*1000);
			MassFitAloneResult_Pos->SetBinError(i,j,(Alonemean_Pos.getError())*1000);

			MassFitSigma_Pos->SetBinContent(i,j,(sigma_Pos.getVal())*1000);
			MassFitSigma_Pos->SetBinError(i,j,(sigma_Pos.getError())*1000);

			SmallMassFitResult_Pos->SetBinContent(i,j,(smallmean_Pos.getVal()-JPsiPDGSmallMass)*1000);
			SmallMassFitResult_Pos->SetBinError(i,j,(smallmean_Pos.getError())*1000);
	
		}
	}


	for (int i = 1; i<=EtaPoints; i++){
		for (int j =1; j<=PhiPoints; j++){

			
			//Get the Mass spectrum for each eta and phi bins.
			//TH1D *h_JPsiMassPos= h_JPsiMassVsEtaPhi_Neg->ProjectionZ("Mass_Neg",0,-1,0,-1);
			TH1D *h_JPsiMassPos= h_JPsiMassVsEtaPhi_Neg->ProjectionZ("Mass_Neg",i,i,j,j);
			
			// Create a binned dataset that imports contents of TH1 and associates its contents to observable 'x'
			RooDataHist dh_Mass_Neg("dh_Mass_Neg","dh JPsi Mass Pos",Mass_Neg,Import(*h_JPsiMassPos)) ;
			
			// Make plot of binned dataset showing Poisson error bars (RooFit default)
			RooPlot* Mass_Negframe = Mass_Neg.frame(Title("Imported TH1 with Poisson error bars")) ;
			dh_Mass_Neg.plotOn(Mass_Negframe,MarkerColor(kBlue+1),LineColor(kBlue+1),MarkerStyle(7)) ;


			// Define "smallPeak" range in Mass:
			Mass_Neg.setRange("SmallPeak_Neg",3.55,3.8) ;

			
			// Define "JPsiPeak" range in Mass:
			//Mass_Neg.setRange("JPsiPeak_Neg",3.01,3.18) ;

			Mass_Neg.setRange("JPsiPeak_Neg",JPsiPDGMass-0.1,JPsiPDGMass+0.1) ;

			// Define "Background" range in Mass:
			Mass_Neg.setRange("Background_Hi_Neg",3.85,4.28) ;
			Mass_Neg.setRange("Background_fi_Neg",2.52,2.84) ;
			Mass_Neg.setRange("Background_Lo_Neg",3.35,3.55) ;

			// Define "Argus Background" range in Mass:
			Mass_Neg.setRange("signal_Neg",2.52,3.43) ;
			
			Mass_Neg.setRange("WholeRange_Neg",2.52,4.28) ;
						
			//Fit the mass spectrum

			//Gauss Peak
			RooRealVar mean_Neg("mean_Neg","mean_Neg",3.0,3.2) ;
			RooRealVar sigma_Neg("sigma_Neg","sigma_Neg",0,0.4) ;
			RooGaussian gauss_Neg("gauss","gauss",Mass_Neg,mean_Neg,sigma_Neg) ;
			gauss_Neg.fitTo(dh_Mass_Neg,Range("JPsiPeak_Neg")) ;

			//obtain the num of events due to the peak for the whole range-> NormGauss_Neg
			RooRealVar NormGauss_Neg("NormGauss_Neg","NormGauss_Neg",3000,0,1000000);

			RooExtendPdf NormGaussFunc_Neg("NormGaussFunc_Neg","NormGaussFunc_Neg",gauss_Neg, NormGauss_Neg,"WholeRange_Neg");
			NormGaussFunc_Neg.fitTo(dh_Mass_Neg,Range("JPsiPeak_Neg"));

			
			//Gauss for second Peak 
			RooRealVar smallmean_Neg("smallmean_Neg","small_mean_Neg",3.68,3.55,3.8) ;
			RooRealVar smallsigma_Neg("smallsigma_Neg","small_sigma_Neg",0,0.4) ;
			RooGaussian smallgauss_Neg("smallgauss","smallgauss",Mass_Neg,smallmean_Neg,smallsigma_Neg) ;
			smallgauss_Neg.fitTo(dh_Mass_Neg,Range("SmallPeak_Neg")) ;
			//smallgauss_Neg.plotOn(Mass_Negframe,LineColor(kCyan),LineWidth(2));
			
			//linear background
			//First, fit background at the right part of the plot wit a linear function			
			RooRealVar aa_Neg("aa_Neg","aa_Neg",0.,-500.,500.);
			RooRealVar bb_Neg("bb_Neg","bb_Neg",0.,-100.,100.);
			RooRealVar cc_Neg("cc_Neg","cc_Neg",0.,-100.,100.);
			RooPolynomial bkgdpoly_Neg("poly_Neg","2th order poly",Mass_Neg,RooArgList(aa_Neg,bb_Neg));
			//RooPolynomial bkgdpoly_Neg("poly_Neg","3th order poly",Mass_Neg,RooArgList(aa_Neg,bb_Neg,cc_Neg));
			bkgdpoly_Neg.fitTo(dh_Mass_Neg,Range("Background_Lo_Neg,Background_Hi_Neg,Background_fi_Neg"));
			//bkgdpoly_Neg.plotOn(Mass_Negframe,LineColor(kGreen),LineWidth(2));
			
			
			//obtain the num of events due to the background for the whole range-> Norm_Neg
			RooRealVar Norm_Neg("Norm_Neg","Norm_Neg",3000,0,1000000);

			RooExtendPdf NormPol_Neg("NormPol_Neg","NormPol_Neg",bkgdpoly_Neg, Norm_Neg,"WholeRange_Neg");
			NormPol_Neg.fitTo(dh_Mass_Neg,Range("Background_Lo_Neg,Background_Hi_Neg"));
	
			//bb_Neg.setConstant(kTRUE);
			//aa_Neg.setConstant(kTRUE);
			
			// gaussian + background **

			//Argus function

			RooRealVar argpar_Neg("argpar_Neg","argus_Neg shape parameter",-1,-Norm_Neg.getVal()/1500.,-1.) ;
			RooRealVar argpeak_Neg("argpeak_Neg","argus_Neg peak parameter",3.23,3.05,3.3) ;
			RooArgusBG argusBackground_Neg("Argus_Neg_background","Argus_Neg PDF",Mass_Neg,argpeak_Neg,argpar_Neg) ;
			
			//different weights of gauss, poly and argus:
			RooRealVar nbkg_Neg("nbkg_Neg","bkg_Neg Integral",Norm_Neg.getVal(),Norm_Neg.getVal()*0.60,Norm_Neg.getVal()*1.4) ;
			RooRealVar nsig_Neg("nsig_Neg","#signal events",10000,0.,3000000) ;
			RooRealVar nsmallsig_Neg("nsmallsig_Neg","#small signal events",200,0.,1000000) ;
			RooRealVar nargus_Neg("nargus_Neg","#background events",0,0.,Norm_Neg.getVal()/5) ;
			
			//Construct model = gauss + linear bkg + argus bkg
			RooAddPdf model_Neg("model_Neg","g+a+b+g",RooArgList(gauss_Neg,bkgdpoly_Neg,argusBackground_Neg,smallgauss_Neg),RooArgList(nsig_Neg,nbkg_Neg,nargus_Neg,nsmallsig_Neg));
			//RooAddPdf model_Neg("model_Neg","g+a+b+g",RooArgList(gauss_Neg,bkgdpoly_Neg,argusBackground_Neg),RooArgList(nsig_Neg,nbkg_Neg,nargus_Neg));
			model_Neg.fitTo(dh_Mass_Neg,Range("WholeRange_Neg"));
			model_Neg.plotOn(Mass_Negframe,LineColor(kOrange),LineWidth(2));
			model_Neg.plotOn(Mass_Negframe,LineColor(kOrange),LineWidth(1),Components(argusBackground_Neg),LineStyle(7));
			model_Neg.plotOn(Mass_Negframe,LineColor(kRed),LineWidth(1),Components(bkgdpoly_Neg),LineStyle(7));
			model_Neg.plotOn(Mass_Negframe,LineColor(kViolet),LineWidth(1),Components(gauss_Neg),LineStyle(7));
			model_Neg.plotOn(Mass_Negframe,LineColor(kCyan),LineWidth(1),Components(smallgauss_Neg),LineStyle(7));
	
			/*//alternative model based in a crystall ball to cross-check 

			RooRealVar alpha_Neg("#alpha_{Pos}","alpha_Neg",-1,-100,-0.0001,"") ;
			RooRealVar n_Neg("n_{Pos}","n_Neg",1,0.1,1000,"") ;
  
			RooCBShape  crystalBall_Neg("PosCB", "Crystal Ball shape", Mass_Neg, mean_Neg, sigma_Neg, alpha_Neg, n_Neg);
			crystalBall_Neg.fitTo(dh_Mass_Neg,Range("JPsiPeak_Neg")) ;

			RooAddPdf CBmodel_Neg("CBmodel_Neg","CB+b+g",RooArgList(crystalBall_Neg,bkgdpoly_Neg,smallgauss_Neg),RooArgList(nsig_Neg,nbkg_Neg,nsmallsig_Neg));
			CBmodel_Neg.fitTo(dh_Mass_Neg,Range("WholeRange_Neg"));
			CBmodel_Neg.plotOn(Mass_Negframe,LineColor(kOrange),LineWidth(2));
			CBmodel_Neg.plotOn(Mass_Negframe,LineColor(kRed),LineWidth(1),Components(bkgdpoly_Neg),LineStyle(kDashed));
			CBmodel_Neg.plotOn(Mass_Negframe,LineColor(kViolet),LineWidth(1),Components(crystalBall_Neg),LineStyle(kDashed));
			CBmodel_Neg.plotOn(Mass_Negframe,LineColor(kCyan),LineWidth(1),Components(smallgauss_Neg),LineStyle(kDashed));
			//end of crystall ball*/


			fittingWidth=2.0*sigma_Neg.getVal();
			//if (fittingWidth >= 0.075) fittingWidth=0.075;
			Mass_Neg.setRange("JPsiAlonePeak_Neg",JPsiPDGMass-fittingWidth,JPsiPDGMass+fittingWidth) ;
			//Construct model = gauss + linear bkg (FIT only the peak)

			bb_Neg.setConstant(kTRUE);
			aa_Neg.setConstant(kTRUE);

			RooRealVar Alonemean_Neg("Alonemean_Neg","Alone mean_Neg",mean_Neg.getVal()*0.9,mean_Neg.getVal()*1.1) ;
			RooRealVar Alonesigma_Neg("Alone sigma_Neg","Alone sigma_Neg",sigma_Neg.getVal()*0.9,sigma_Neg.getVal()*1.1) ;
			RooGaussian Alonegauss_Neg("Alonegauss","Alone gauss",Mass_Neg,Alonemean_Neg,Alonesigma_Neg) ;
			Alonegauss_Neg.fitTo(dh_Mass_Neg,Range("JPsiAlonePeak_Neg")) ;

			RooRealVar nbkgPeak_Neg("nbkgPeak_Neg","bkgPeak_Neg Integral",nbkg_Neg.getVal());
			RooRealVar nsigAlone_Neg("nsigAlone_Neg","#signal events",10000,0.,3000000) ;

			RooAddPdf Peakmodel_Neg("Peakmodel_Neg","g+b",RooArgList(Alonegauss_Neg,bkgdpoly_Neg),RooArgList(nsigAlone_Neg,nbkgPeak_Neg));
			Peakmodel_Neg.fitTo(dh_Mass_Neg,Range("JPsiAlonePeak_Neg"));
			Peakmodel_Neg.plotOn(Mass_Negframe,LineColor(kRed),LineWidth(2));
			Peakmodel_Neg.plotOn(Mass_Negframe,LineColor(kRed),LineWidth(2),Components(bkgdpoly_Neg),LineStyle(kDashed));
			//Peakmodel_Neg.plotOn(Mass_Negframe,LineColor(kViolet),LineWidth(1),Components(gauss_Neg),LineStyle(kDashed));
			//Peakmodel_Neg.plotOn(Mass_Negframe,LineColor(kGreen),LineWidth(2));
			//Peakmodel_Neg.plotOn(Mass_Negframe,LineColor(kGreen),LineWidth(2),Components(bkgdpoly_Neg),LineStyle(kDashed));
			//Peakmodel_Neg.plotOn(Mass_Negframe,LineColor(kViolet),LineWidth(1),Components(gauss_Neg),LineStyle(kDashed));*/

			FitComparative_Neg->Fill(mean_Neg.getVal()-Alonemean_Neg.getVal());



			cJPsiMassFit_Neg->cd(i+(j-1)*(EtaPoints)); 
			Mass_Negframe->Draw();
			Mass_Negframe->GetYaxis()->SetLabelSize(0.025);
			Mass_Negframe->GetYaxis()->SetLabelOffset(0.001);
			Mass_Negframe->GetYaxis()->SetTitleOffset(1.35);
			Mass_Negframe->SetTitle("");
			
			if (((i+(j-1)*(EtaPoints))>0*(EtaPoints))&&((i+(j-1)*(EtaPoints))<=1*(EtaPoints)))
			{	
				cJPsiMassFitzoom->cd(i+(j-1)*(EtaPoints)-0*(EtaPoints)); 
				Mass_Negframe->Draw();
				if (i+(j-1)*(EtaPoints)==1) l3.DrawLatex(3.7,Mass_Negframe->GetMaximum()*0.96,"#eta #in [-2.5, -1.66]");
				if (i+(j-1)*(EtaPoints)==3) l3.DrawLatex(3.8,Mass_Negframe->GetMaximum()*0.96,"#eta #in [-0.83, 0]");
				if (i+(j-1)*(EtaPoints)==2)
					{ 
					l.DrawLatex(3.5,2500,"    #psi (2S)     ");
					l2.DrawLatex(3.5,1700,"m = 3.686109(14)");
					
					l.DrawLatex(3.17,22000,"   J/#psi (1S)     ");
					l2.DrawLatex(3.15,21200,"m = 3.096916(11)");

					l3.DrawLatex(3.66,Mass_Negframe->GetMaximum()*0.96,"#eta #in [-1.66, -0.83]");
					}
			}

			MassFitResult_Neg->SetBinContent(i,j,(mean_Neg.getVal()-JPsiPDGMass)*1000);
			MassFitResult_Neg->SetBinError(i,j,(mean_Neg.getError())*1000);

			MassFitAloneResult_Neg->SetBinContent(i,j,(Alonemean_Neg.getVal()-JPsiPDGMass)*1000);
			MassFitAloneResult_Neg->SetBinError(i,j,(Alonemean_Neg.getError())*1000);

			MassFitSigma_Neg->SetBinContent(i,j,(sigma_Neg.getVal())*1000);
			MassFitSigma_Neg->SetBinError(i,j,(sigma_Neg.getError())*1000);

			if ((i==Etabin)&&(j==Phibin))
			{
				cJPsiMassFitIndividual->cd();
				Mass_Negframe->Draw("same");
			}	
		}
	}


	gStyle->SetOptStat(0000); 

	cJPsiMassVsEtaPhi_PosMinusNeg = new TCanvas("cJPsiMassVsEtaPhi_PosMinusNeg","Mass_{Pos} - Mass_{Neg}",20,220,800, 600);
	
	TH2F* h1 = (TH2F*)MassFitResult_Pos->Clone("Mass_{Pos} - Mass_{Neg}");
	h1->Add(MassFitResult_Neg,-1);
	h1->Draw("Lego2");
	h1->SetTitle("Mass_{Pos} - Mass_{Neg}");
	h1->SetXTitle("#eta");
	h1->SetYTitle("#phi");
         
	
	cJPsiSigmaVsEtaPhi_Pos = new TCanvas("cJPsiSigmaVsEtaPhi_Pos","JPsi Sigma vs Eta Phi",0,200,720, 600);
		
	MassFitSigma_Pos->Draw("Lego2");
	MassFitSigma_Pos->SetXTitle("#eta");
	MassFitSigma_Pos->SetYTitle("#phi");
	MassFitSigma_Pos->SetMinimum(0);
	
	

	TH1F* h11 = (TH1F*)MassFitResultVsEta_Pos->Clone("Mass_{Pos} - Mass_{Neg}");
	h11->Add(MassFitResultVsEta_Neg,-1);
	//h11->Draw("same");
	h11->SetLineColor(kGreen);
	
	cJPsiMassVsEtaPhi_Pos = new TCanvas("cJPsiMassVsEtaPhi_Pos","JPsi Mass vs Eta Phi",0,200,720, 600);
		
	MassFitResult_Pos->Draw("Lego2");
	MassFitResult_Pos->SetXTitle("#eta");
	MassFitResult_Pos->SetYTitle("#phi");

	cJPsiSigmaVsEtaPhi_Neg = new TCanvas("cJPsiSigmaVsEtaPhi_Neg","JPsi Sigma vs Eta Phi",720,200,720, 600);
		
	MassFitSigma_Neg->Draw("Lego2");
	MassFitSigma_Neg->SetXTitle("#eta");
	MassFitSigma_Neg->SetYTitle("#phi");
	MassFitSigma_Neg->SetMinimum(0);
        
	cJPsiMassVsEtaPhi_Neg = new TCanvas("cJPsiMassVsEtaPhi_Neg","JPsi Mass vs Eta Phi",720,200,720, 600);
		
	MassFitResult_Neg->Draw("Lego2");
	MassFitResult_Neg->SetXTitle("#eta");
	MassFitResult_Neg->SetYTitle("#phi");      
	
	/*cJPsiSmallMassVsEtaPhi_Pos = new TCanvas("cJPsiSmallMassVsEtaPhi_Pos","Resonance Mass vs Eta Phi",0,200,720, 600);
		
	SmallMassFitResult_Pos->Draw("Colz");
	SmallMassFitResult_Pos->SetXTitle("#eta");
	SmallMassFitResult_Pos->SetYTitle("#phi");*/
       
	cFitComparative_Neg = new TCanvas("cFitComparative_Neg","cFitComparative_Neg",720,200,720, 600);
	FitComparative_Pos->Draw();
	FitComparative_Pos->SetLineColor(kRed);	
	FitComparative_Neg->Draw("same");
	FitComparative_Neg->SetLineColor(kBlue);

	double maximum_Pos = MassFitResult_Pos->GetMaximum();
	double maximum_Neg = MassFitResult_Neg->GetMaximum(); 
	double minimum_Pos = MassFitResult_Pos->GetMinimum(); 
	double minimum_Neg = MassFitResult_Neg->GetMinimum(); 
	if (maximum_Pos>maximum_Neg) {MassFitResult_Neg->SetMaximum(int(maximum_Pos)+1); MassFitResult_Pos->SetMaximum(int(maximum_Pos)+1); MassFitResultVsEta_Pos->SetMaximum(int(maximum_Pos)+2);}
	else {MassFitResult_Pos->SetMaximum(int(maximum_Neg)+1); MassFitResultVsEta_Pos->SetMaximum(int(maximum_Neg)+2); MassFitResult_Neg->SetMaximum(int(maximum_Neg)+1);}
	
	if (minimum_Pos<minimum_Neg) {MassFitResult_Neg->SetMinimum(int(minimum_Pos)-1); MassFitResult_Pos->SetMinimum(int(minimum_Pos)-1); MassFitResultVsEta_Pos->SetMinimum(int(minimum_Pos)-2);}
	else {MassFitResult_Pos->SetMinimum(int(minimum_Neg)-1); MassFitResultVsEta_Pos->SetMinimum(int(minimum_Neg)-2); MassFitResult_Neg->SetMinimum(int(minimum_Neg)-1);}
	

	cJPsiMassVsEta_Pos = new TCanvas("cJPsiMassVsEta_Pos","JPsi Mass vs Eta",0,0,720, 550);
		
	constant = new TF1("Constant", "[0]+x*[1]", -2.5,2.5);
	constant->SetParameter(0,PhiPoints);
	constant->SetParameter(1,0);
	MassFitResultVsEta_Pos= MassFitResult_Pos->ProjectionX("a",0,-1,"e");
	MassFitResultVsEta_Neg= MassFitResult_Neg->ProjectionX("b",0,-1,"e");
        //MassFitResultVsEta_Pos->Add(MassFitResultVsEta_Neg,+1);
	//MassFitResultVsEta_Pos->Divide(constant,2);
	MassFitResultVsEta_Pos->Divide(constant,1);
	MassFitResultVsEta_Neg->Divide(constant,1);

	MassFitAloneResultVsEta_Pos= MassFitAloneResult_Pos->ProjectionX("c",0,-1,"e");
	MassFitAloneResultVsEta_Neg= MassFitAloneResult_Neg->ProjectionX("d",0,-1,"e");
	//MassFitAloneResultVsEta_Pos->Add(MassFitAloneResultVsEta_Neg,+1);
	//MassFitAloneResultVsEta_Pos->Divide(constant,2);
	MassFitAloneResultVsEta_Pos->Divide(constant,1);
	MassFitAloneResultVsEta_Neg->Divide(constant,1);
	
	MassFitResultVsEta_Pos->SetXTitle("#eta");
	MassFitResultVsEta_Pos->SetYTitle("(M_{Exp}-M_{PDG}) [MeV]");
	MassFitResultVsEta_Pos->SetTitle("");
	MassFitResultVsEta_Pos->Draw();
	//MassFitResultVsEta_Neg->Draw("same");
	MassFitResultVsEta_Pos->SetLineColor(kRed);
	MassFitResultVsEta_Neg->SetLineColor(kBlue);

	MassFitAloneResultVsEta_Pos->Draw("");
	MassFitAloneResultVsEta_Neg->Draw("same");
	MassFitAloneResultVsEta_Pos->SetLineColor(kRed);
	MassFitAloneResultVsEta_Neg->SetLineColor(kBlue);

	MassFitAloneResultVsEta_Pos->SetTitle("");
	MassFitAloneResultVsEta_Pos->SetXTitle("#eta");
	MassFitAloneResultVsEta_Pos->SetYTitle("(M_{Exp}-M_{PDG}) [MeV]");

	MassFitResultVsEta_Pos->SetLineStyle(1);
	MassFitResultVsEta_Neg->SetLineStyle(1);
	MassFitAloneResultVsEta_Pos->SetLineStyle(2);
	MassFitAloneResultVsEta_Neg->SetLineStyle(2);

	MassFitAloneResultVsEta_Pos->SetMaximum(15);
	MassFitAloneResultVsEta_Pos->SetMinimum(-10);

	return true;
}

 

