/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TGraphErrors.h"
#include "TText.h"
#include "TLegend.h"
#include "TF1.h"
#include "TChain.h"
#include "TPaveStats.h"
#include "TProfile.h"

#include <vector>
#include <iostream>
#include <dirent.h>
#include <sstream>
#include <stdlib.h>
#include <cmath>

using namespace std;

const bool USETGCETA=true;

//const string outputdir=(USETGCETA)?"align_withTGCEta":"align_noTGCEta";
const string outputdir="align_tgcEtaOutlier";

const bool DOCSC=false;
const bool DOBEE=false;

const bool DOEC =true;

const int NVALBINS = 101;
const int NYX=(DOCSC)?2:1;

const int NMOM     =  16;
const double mommin=  4.;
const double mommax= 20.;
double* momMin;
double momCut=10.;

const int twocolors[2]={kRed,kBlue};

const int phicolor[16]={kRed+4,kRed+2,kRed,
			kMagenta+4,kMagenta+2,kMagenta,
			kBlue+4,kBlue+2,kBlue,
			kCyan+4,kCyan+2,kCyan,
			kGreen+4,kGreen+2,kGreen,
			kYellow+2};

// Declaration of leaf types
Int_t     nPixHits;
Int_t     nSCTHits;
Int_t     nTRTHits;
Int_t     nCSCHits;
Int_t     nMDTHits;
Int_t     nInnerMDTHits;
Int_t     nMiddleMDTHits;
Int_t     nOuterMDTHits;
Int_t     nTriggerPhiHits;
Int_t     author;
Int_t     hIndex;
Int_t     phiSector;
Int_t     isEndcap;
Int_t     identifier;
Int_t     isCSide;
Int_t     detType;
Int_t     layer;
Int_t     stName;
Int_t     stPhi;
Int_t     stEta;
Double_t  momentum;
Double_t  charge;
Double_t  d0;
Double_t  z0;
Double_t  phi0;
Double_t  theta0;
Double_t  eta0;
Double_t  calEnergyLoss;
Double_t  calEnergyLossErr;
Double_t  materialOnTrack;
Double_t  dR;
Double_t  dZlocal;
Double_t  dXlocal;
Double_t  dYlocal;
Double_t  dXlocalAngle;
Double_t  dYlocalAngle;
Double_t  dXglobal;
Double_t  dYglobal;
Double_t  dZglobal;
Int_t     nInnerChambers;
Int_t     nMiddleChambers;
Int_t     nOuterChambers;
Int_t     nOtherChambers;
Double_t  detTheta;
Int_t     innerStEta[10];
Int_t     middleStEta[10];
Int_t     outerStEta[10];
Int_t     otherStEta[10];

void    setBranchAddresses(TTree* tree);
void    getHistograms(bool histsDone);
void    makeLargeSmallWidthPlots(TFile* outputHists);
void    makePhiWidthPlots(TFile* outputHists);
void    makeWidthPlots(TFile* outputHists, const string& type);
void    makeCSCResPlots(TFile* outputHists);
void    makeCSCResVTanThetaPlots(TFile* outputHists);
void    makeMDTResPlots(TFile* outputHists);
void    makeMDTResVTanThetaPlots(TFile* outputHists);
string  mdtChamberNameNoPhi(int iecAC, int ilayer, int ieta);
string  mdtChamberNameNoEta(int iecAC, int ilayer);
string  mdtChamberName(int iecAC, int ilayer, int ieta, int iphi);
void    plotLgSmSummary(TPad* canv, TH1F** hists, const string& canvname);

TFile*     inputHistsFile;

TH2F****    lgsmHistsCSC;
TH2F****    ecHistsCSC;
TH1F****   phiSecHistsCSC;
TH1F***    phiSecHistsMDT;
TH2F****   resVTanThetaHistsCSC;
TH1F****** phiSecEtaHistsMDT;
TH2F****** resVTanThetaHistsMDT;

double**   tanThetaMin;
double**   tanThetaMax;

int allowedInnerLayerForMiddleFixed[6]={0,1,2,2,4,0};
int allowedOuterLayerForMiddleFixed[6]={2,3,5,6,6,0};
//int allowedOuterLayerForMiddleFixed[6]={1,3,5,6,6,0};

//________________________________________________________________________
int main() 
{
  // set up tan theta bins
  tanThetaMin=new double*[3];
  tanThetaMax=new double*[3];
  for (int ilayer=0;ilayer<3;ilayer++) {
    tanThetaMin[ilayer]=new double[6];
    tanThetaMax[ilayer]=new double[6];  
  }

  tanThetaMin[0][0]=0.24;  tanThetaMax[0][0]=0.44;
  tanThetaMin[0][1]=0.25; tanThetaMax[0][1]=0.75;
  tanThetaMin[0][2]=0.4;  tanThetaMax[0][2]=0.65;
  tanThetaMin[0][3]=0.4;  tanThetaMax[0][3]=1.3;
  tanThetaMin[0][4]=0.7;  tanThetaMax[0][4]=1.2;
  tanThetaMin[0][5]=0.;   tanThetaMax[0][5]=1.;

  tanThetaMin[1][0]=0.17; tanThetaMax[1][0]=0.26;
  tanThetaMin[1][1]=0.24; tanThetaMax[1][1]=0.36;
  tanThetaMin[1][2]=0.39;  tanThetaMax[1][2]=0.51;
  tanThetaMin[1][3]=0.51;  tanThetaMax[1][3]=0.58;
  tanThetaMin[1][4]=0.72; tanThetaMax[1][4]=0.82;
  tanThetaMin[1][5]=0.;   tanThetaMax[1][5]=1.;

  tanThetaMin[2][0]=0.05; tanThetaMax[2][0]=0.3;
  tanThetaMin[2][1]=0.12; tanThetaMax[2][1]=0.4;
  tanThetaMin[2][2]=0.2;  tanThetaMax[2][2]=0.5;
  tanThetaMin[2][3]=0.3;  tanThetaMax[2][3]=0.6;
  tanThetaMin[2][4]=0.35; tanThetaMax[2][4]=0.65;
  tanThetaMin[2][5]=0.4;  tanThetaMax[2][5]=0.75;

  // set up momentum bins
  momMin=new double[NMOM+1];
  for (int imom=0;imom<NMOM;imom++) {
    momMin[imom]=mommin + (mommax-mommin)/(double)NMOM*imom;
  }
  momMin[NMOM]=mommax;

  // fill histograms or get from input histogram file
  bool histsDone=true;
  getHistograms(histsDone);
  
  // make output histograms  
  TFile* outputHists=new TFile((outputdir+"outputhists.root").c_str(),"RECREATE");

  // make plots
  if (DOCSC) {
    makeWidthPlots(outputHists,"LargeSmall");
    makeWidthPlots(outputHists,"EndcapAC");
    makeCSCResPlots(outputHists);
    makeCSCResVTanThetaPlots(outputHists);
  }
  else {
    makeWidthPlots(outputHists,"LargeSmall");
    makeWidthPlots(outputHists,"EndcapAC");
    if (!DOBEE) {
      makePhiWidthPlots(outputHists);
      makeMDTResPlots(outputHists);
      makeMDTResVTanThetaPlots(outputHists);
    }
    
  }
  outputHists->Close();
}

//________________________________________________________________________
void makePhiWidthPlots(TFile* outputHists)
{
  gStyle->SetOptStat(1);
  gStyle->SetOptFit(111);

  TCanvas* canv=new TCanvas("canv","canv");
  canv->SetFillColor(10);

  TCanvas* ctmp=new TCanvas("ctmp","ctmp");

  for (int iecAC=0;iecAC<2;iecAC++) {
    cout<<"iecAC: "<<iecAC<<endl;
    canv->Clear();
    canv->Divide(4,4);

    TF1** fits=new TF1*[16];
    TH1F** hMeans      =new TH1F*[2];
    TH1F** hWidths     =new TH1F*[2];
    TH1F** hOuterWidths=new TH1F*[2];
    TH1F** hEntries    =new TH1F*[2];

    hMeans[0]      =new TH1F("hMeanLg", "Unbiased Residual Mean vs. #phi", 16,0.,16.);
    hWidths[0]     =new TH1F("hWidthLg","Unbiased Residual Inner Width vs. #phi",16,0.,16.);
    hOuterWidths[0]=new TH1F("hOuterWidthLg","Unbiased Residual Outer Width vs. #phi",16,0.,16.);
    hEntries[0]    =new TH1F("hEntriesLg", "Number Residuals vs. #phi",16,0.,16.);

    hMeans[1]      =new TH1F("hMeanSm", "PhiMeansSm", 16,0.,16.);
    hWidths[1]     =new TH1F("hWidthSm","PhiWidthsSm",16,0.,16.);
    hOuterWidths[1]=new TH1F("hOuterWidthSm","PhiOuterWidthsSm",16,0.,16.);
    hEntries[1]    =new TH1F("hEntriesSm", "PhiEntriesSm",16,0.,16.);

    for (int iphi=0;iphi<16;iphi++) {
      cout<<"iphi: "<<iphi<<endl;
      stringstream histname;
      histname<<"Sector"<<iphi+1;
      if (iecAC==0) histname<<"A"; else histname<<"C";
      histname<<"_dY_MDT";

      cout<<"getting hist: "<<histname.str()<<endl;

      TH1F* hist=phiSecHistsMDT[iecAC][iphi];
      cout<<"hist has "<<hist->GetEntries()<<" entries"<<endl;
      stringstream fitname; fitname<<"fit_"<<iphi;

      ctmp->cd();
      //fits[iphi]=new TF1(fitname.str().c_str(),"gaus(0)+gaus(3)",-30.,30.);
      fits[iphi]=new TF1(fitname.str().c_str(),
			 "[0]*exp(-0.5*((x-[1])/[2])^2)+[3]*[0]*exp(-0.5*((x-[1])/[4])^2)",
			 -30.,30.);
      fits[iphi]->SetParameters(hist->GetMaximum(),0,5.5,.6,20.);
      fits[iphi]->SetParLimits(4,0.,999.);
      hist->Fit(fits[iphi],"RQ");

      int histindex=(iphi%2);

      stringstream binLabel; binLabel<<iphi+1;
      if (iecAC==0) binLabel<<"A"; else binLabel<<"C";

      // mean
      double mean   =fits[iphi]->GetParameter(1);
      double meanerr=fits[iphi]->GetParError(1);
      hMeans[histindex]->SetBinContent(iphi+1,mean);
      hMeans[histindex]->SetBinError(iphi+1,meanerr);
      hMeans[0]->GetXaxis()->SetBinLabel(iphi+1,binLabel.str().c_str());
      hMeans[0]->SetMinimum(-1.4);
      hMeans[0]->SetMaximum(1.4);

      // inner width
      double width   =fits[iphi]->GetParameter(2);
      double widtherr=fits[iphi]->GetParError(2);
      hWidths[histindex]->SetBinContent(iphi+1,width);
      hWidths[histindex]->SetBinError(iphi+1,widtherr);
      hWidths[0]->GetXaxis()->SetBinLabel(iphi+1,binLabel.str().c_str());
      hWidths[0]->SetMinimum(4.);
      hWidths[0]->SetMaximum(8.5);

      // outer width
      double owidth   =fits[iphi]->GetParameter(4);
      double owidtherr=fits[iphi]->GetParError(4);
      hOuterWidths[histindex]->SetBinContent(iphi+1,owidth);
      hOuterWidths[histindex]->SetBinError(iphi+1,owidtherr);
      hOuterWidths[0]->GetXaxis()->SetBinLabel(iphi+1,binLabel.str().c_str());
      hOuterWidths[0]->SetMinimum(14.);
      hOuterWidths[0]->SetMaximum(26.);

      // # entries
      double entries   =hist->GetEntries();
      hEntries[histindex]->SetBinContent(iphi+1,entries);
      hEntries[histindex]->SetBinError(iphi+1,sqrt(entries));
      hEntries[0]->GetXaxis()->SetBinLabel(iphi+1,binLabel.str().c_str());
      hEntries[0]->SetMinimum(0.);
      hEntries[0]->SetMaximum(35000.);

      canv->cd(iphi+1);
      hist->SetLineColor(phicolor[iphi]);
      fits[iphi]->SetLineColor(phicolor[iphi]);
      hist->Draw();
      fits[iphi]->Draw("same");
      canv->Modified();
    }
    
    stringstream canvname;
    canvname<<"MDTSectorPlots_";
    if (iecAC==0) canvname<<"Aside"; else canvname<<"Cside";
    canv->Print((outputdir+"/"+canvname.str()+".eps").c_str());

    plotLgSmSummary(canv,hMeans,outputdir+"/"+canvname.str()+"_Means.eps");
    plotLgSmSummary(canv,hWidths,outputdir+"/"+canvname.str()+"_Widths.eps");
    plotLgSmSummary(canv,hOuterWidths,outputdir+"/"+canvname.str()+"_OuterWidths.eps");
    plotLgSmSummary(canv,hEntries,outputdir+"/"+canvname.str()+"_Entries.eps");

    delete [] fits;
  }
  delete canv;
  delete ctmp;
}

//________________________________________________________________________
void makeCSCResPlots(TFile* outputHists)
{
  gStyle->SetOptStat(1);
  gStyle->SetOptFit(111);

  // tan((detTheta-11.59)/360.*6.28), theta<25.
  TH1F*** hMeans=new TH1F**[2];
  TF1**** fits=new TF1***[2];  
  for (int iecAC=0;iecAC<2;iecAC++) {
    hMeans[iecAC]=new TH1F*[NYX];
    fits[iecAC]=new TF1**[NYX];    
    int ixy=0;
    for (int iyx=NYX-1;iyx>=0;iyx--,ixy++) {
      string title="#Delta ";
      if (iyx==0) title+="y"; else title+="x";
      title+=" (mm)";

      string hmeanname=(DOCSC)?"CSC":"MDT";
      if (iecAC==0) hmeanname+="_EndcapA";
      else          hmeanname+="_EndcapC";
      if (iyx==0) hmeanname+="_dY"; else hmeanname+="_dX";
      hMeans[iecAC][iyx]=new TH1F(hmeanname.c_str(),hmeanname.c_str(),16,0.,16.);
      fits[iecAC][iyx]=new TF1*[16];

      TCanvas* canv=new TCanvas("canv","canv");
      canv->SetFillColor(10);
      canv->Divide(4,4);
      
      for (int iphi=0;iphi<16;iphi++) {
	canv->cd(iphi+1);
	stringstream fitname;
	fitname<<"fitRes_"<<iphi+1;
	if (iecAC==0) fitname<<"A"; else fitname<<"C";
	if (iyx==0) fitname<<"_dY"; else fitname<<"_dX";
	double fitrange=(iyx==0) ? 40. : 80.;
	fits[iecAC][iyx][iphi]=
	  new TF1(fitname.str().c_str(),
		  "[0]*exp(-0.5*((x-[1])/[2])^2)+[3]*[0]*exp(-0.5*((x-[1])/[4])^2)",
		  -fitrange,fitrange);
	TH1F* hist=phiSecHistsCSC[iecAC][iphi][iyx];
	TF1*  fit =fits[iecAC][iyx][iphi];
	
	double innerwidth=(iyx==0)?8.:16.;
	double outerwidth=(iyx==0)?20.:40.;
	fit->SetParameters(hist->GetMaximum(),hist->GetMean(),innerwidth,0.8,outerwidth);
	fit->SetParLimits(2,innerwidth/4.,outerwidth*1.5);
	hist->Fit(fit,"RQ");

	hist->GetXaxis()->SetTitle(title.c_str());

	hist->Draw();
	fit->Draw("same");

	stringstream binLabel; binLabel<<iphi+1;
	if (iecAC==0) binLabel<<"A"; else binLabel<<"C";
	hMeans[iecAC][iyx]->GetXaxis()->SetBinLabel(iphi+1,binLabel.str().c_str());
	if (fit->GetChisquare()/fit->GetNDF()<5. && hist->GetEntries()>1000.) {
	  hMeans[iecAC][iyx]->SetBinContent(iphi+1,fit->GetParameter(1));
	  hMeans[iecAC][iyx]->SetBinError  (iphi+1,fit->GetParError(1));
	  if (iyx==0) cout<<"dY: "; else cout<<"dX: ";
	  cout<<iphi+1;
	  if (iecAC==0) cout<<"A"; else cout<<"C";
	  cout<<" "<<fit->GetParameter(1)<<" +/- "<<fit->GetParError(1)<<endl;
	}
      }
      
      string canvname="CSC";
      if (iecAC==0) canvname+="_EndcapA";
      else          canvname+="_EndcapC";
      if (iyx==0) canvname+="_dY"; else canvname+="_dX";
      canv->Print((outputdir+"/"+canvname+".eps").c_str());

      canv->Clear();

      if (hMeans[iecAC][iyx]->GetMaximum()>20.) hMeans[iecAC][iyx]->SetMaximum(20.);
      if (hMeans[iecAC][iyx]->GetMinimum()<-20.) hMeans[iecAC][iyx]->SetMinimum(-20.);
      hMeans[iecAC][iyx]->Draw();
      hMeans[iecAC][iyx]->GetYaxis()->SetTitle(title.c_str());
      canv->Modified();

      canv->Print((outputdir+"/"+canvname+"_allPhi.eps").c_str());
      
      delete canv;
    }
  }
  
  return;
}

//________________________________________________________________________
void makeCSCResVTanThetaPlots(TFile* outputHists)
{
  gStyle->SetOptStat(1);
  gStyle->SetOptFit(111);
  gStyle->SetPalette(1);

  TH1F*** hMeans       =new TH1F**[2];
  TF1**** fits         =new TF1***[2];
  TH1F**** resVTanTheta=new TH1F***[2];
  for (int iecAC=0;iecAC<2;iecAC++) {
    hMeans      [iecAC]=new TH1F*[NYX];
    fits        [iecAC]=new TF1**[NYX];    
    resVTanTheta[iecAC]=new TH1F**[NYX];
    int ixy=NYX-1;
    for (int iyx=0;iyx<NYX;iyx++,ixy--) {
      string title="#Delta ";
      if (iyx==0) title+="y"; else title+="x";
      title+=" (mm)";

      string hmeanname=(iecAC==0)?"EndcapA":"EndcapC";
      if (iyx==0) hmeanname+="_dY"; else hmeanname+="_dX";
      hMeans      [iecAC][iyx]=new TH1F(hmeanname.c_str(),hmeanname.c_str(),16,0.,16.);
      fits        [iecAC][iyx]=new TF1*[16];
      resVTanTheta[iecAC][iyx]=new TH1F*[16];

      TCanvas* canv=new TCanvas("canv","canv");
      canv->SetFillColor(10);
      canv->Divide(4,4);
      
      for (int iphi=0;iphi<16;iphi++) {
	canv->cd(iphi+1);
		
	TH2F* hist=resVTanThetaHistsCSC[iecAC][iphi][iyx];

	double fitrange=(iyx==0)?20.:40.;
	string resHistName=string(hist->GetName())+"Fits";
	resVTanTheta[iecAC][iyx][iphi]=new TH1F(resHistName.c_str(),resHistName.c_str(),
						5,-.04,.06);
	TH1F** slices=new TH1F*[5];	
	for (int i=0;i<5;i++) {
	  stringstream tmphistname; tmphistname<<hist->GetName()<<"_slice"<<i;
	  slices[i]=(TH1F*)hist->ProjectionY(tmphistname.str().c_str(),i+1,i+2);
	  slices[i]->Draw();
	  TF1* fit=
	    new TF1("fit", "gaus",-fitrange,fitrange);
	  slices[i]->Fit(fit,"QR");
	  resVTanTheta[iecAC][iyx][iphi]->SetBinContent(i+1,fit->GetParameter(1));
	  resVTanTheta[iecAC][iyx][iphi]->SetBinError(i+1,fit->GetParError(1));
	  delete fit;
	}
	string fitname=string(hist->GetName())+"_fit";
	fits[iecAC][iyx][iphi]=new TF1(fitname.c_str(),"pol1",-fitrange,fitrange);
	resVTanTheta[iecAC][iyx][iphi]->Fit(fits[iecAC][iyx][iphi],"RQ");

	//canv->cd(iphi+1);
	hist->GetYaxis()->SetRangeUser(-fitrange,fitrange);
	//hist->Draw("colz");
	resVTanTheta[iecAC][iyx][iphi]->Draw("same");
	fits[iecAC][iyx][iphi]->Draw("same");
      }

      string canvname="CSC";
      if (iecAC==0) canvname+="_EndcapA";
      else          canvname+="_EndcapC";
      if (iyx==0) canvname+="_dY"; else canvname+="_dX";
      canv->Print((outputdir+"/"+canvname+"_tantheta.eps").c_str());

      canv->Clear();

      hMeans[iecAC][iyx]->Draw();
      hMeans[iecAC][iyx]->GetYaxis()->SetTitle(title.c_str());
      canv->Modified();

      canv->Print((outputdir+"/"+canvname+"_allPhi_tantheta.eps").c_str());
      
      delete canv;
    }
  }
  
  return;
}

//________________________________________________________________________
void makeMDTResVTanThetaPlots(TFile* outputHists)
{
  gStyle->SetOptStat(1);
  gStyle->SetOptFit(111);
  gStyle->SetPalette(1);

  TH1F***** hMeans       =new TH1F****[2];
  TF1****** fits         =new TF1*****[2];
  TH1F****** resVTanTheta=new TH1F*****[2];
  for (int iecAC=0;iecAC<2;iecAC++) {
    hMeans      [iecAC]=new TH1F***[3];
    fits        [iecAC]=new TF1****[3];    
    resVTanTheta[iecAC]=new TH1F****[3];
    for (int ilayer=0;ilayer<3;ilayer++) {
      hMeans      [iecAC][ilayer]=new TH1F**[6];
      fits        [iecAC][ilayer]=new TF1***[6];
      resVTanTheta[iecAC][ilayer]=new TH1F***[6];
      for (int ieta=0;ieta<6;ieta++) {
	hMeans      [iecAC][ilayer][ieta]=new TH1F*[NYX];
	fits        [iecAC][ilayer][ieta]=new TF1**[NYX];
	resVTanTheta[iecAC][ilayer][ieta]=new TH1F**[NYX];	
	
	int ixy=NYX-1;
	for (int iyx=0;iyx<NYX;iyx++,ixy--) {
	  string title="#Delta ";
	  if (iyx==0) title+="y"; else title+="x";
	  title+=" (mm)";
	  
	  string hmeanname="mean_resVTanTheta"+mdtChamberNameNoPhi(iecAC,ilayer,ieta);
	  if (iyx==0) hmeanname+="_dY"; else hmeanname+="_dX";
	  hMeans      [iecAC][ilayer][ieta][iyx]=new TH1F(hmeanname.c_str(),hmeanname.c_str(),16,0.,16.);
	  fits        [iecAC][ilayer][ieta][iyx]=new TF1*[16];
	  resVTanTheta[iecAC][ilayer][ieta][iyx]=new TH1F*[16];
	  
	  TCanvas* canv=new TCanvas("canv","canv");
	  canv->SetFillColor(10);
	  canv->Divide(4,4);
	  
	  for (int iphi=0;iphi<16;iphi++) {
	    canv->cd(iphi+1);
	    string fitname="fitRes_"+mdtChamberName(iecAC,ilayer,ieta,iphi)+"_dY";
	    double fitrange=(iyx==0)?20.:40.;
	    if (ilayer==1) fitrange*=0.5;

	    TH2F* hist=resVTanThetaHistsMDT[iecAC][ilayer][ieta][iphi][iyx];
	    cout<<"hist has "<<hist->GetEntries()<<" entries"<<endl;

	    string resHistName=string(hist->GetName())+"Fits";
	    resVTanTheta[iecAC][ilayer][ieta][iyx][iphi]=
	      new TH1F(resHistName.c_str(),resHistName.c_str(),5,tanThetaMin[ilayer][ieta],tanThetaMax[ilayer][ieta]);
	    TH1F** slices=new TH1F*[5];	
	    for (int i=0;i<5;i++) {
	      stringstream tmphistname; tmphistname<<hist->GetName()<<"_slice"<<i;
	      slices[i]=(TH1F*)hist->ProjectionY(tmphistname.str().c_str(),i+1,i+2);
	      slices[i]->Draw();
	      TF1* fit=
		new TF1("fit", "gaus",-fitrange,fitrange);
	      slices[i]->Fit(fit,"QR");
	      cout<<"mean["<<i<<"]: "<<fit->GetParameter(1)<<" +/- "<<fit->GetParError(1)<<endl;

	      resVTanTheta[iecAC][ilayer][ieta][iyx][iphi]->SetBinContent(i+1,fit->GetParameter(1));
	      resVTanTheta[iecAC][ilayer][ieta][iyx][iphi]->SetBinError(i+1,fit->GetParError(1));
	      delete fit;
	    }
	    fits[iecAC][ilayer][ieta][iyx][iphi]=new TF1(fitname.c_str(),"pol1",-fitrange,fitrange);
	    TF1* fitpol1=fits[iecAC][ilayer][ieta][iyx][iphi];
	    resVTanTheta[iecAC][ilayer][ieta][iyx][iphi]->
	      Fit(fitpol1,"RQ");
	    
	    hMeans[iecAC][ilayer][ieta][iyx]->SetBinContent(iphi+1,fitpol1->GetParameter(0));
	    hMeans[iecAC][ilayer][ieta][iyx]->SetBinError(iphi+1,fitpol1->GetParError(0));
	    
	    //canv->cd(iphi+1);
	    hist->GetYaxis()->SetRangeUser(-fitrange,fitrange);
	    //hist->Draw("colz");
	    resVTanTheta[iecAC][ilayer][ieta][iyx][iphi]->Draw("same");
	    fits[iecAC][ilayer][ieta][iyx][iphi]->Draw("same");
	  }
	  
	  string canvname=mdtChamberNameNoPhi(iecAC,ilayer,ieta);
	  if (iyx==0) canvname+="_dY"; else canvname+="_dX";
	  canv->Print((outputdir+"/"+canvname+"_tantheta.eps").c_str());
	  //exit(3);

	  canv->Clear();
	  
	  hMeans[iecAC][ilayer][ieta][iyx]->Draw();
	  hMeans[iecAC][ilayer][ieta][iyx]->GetYaxis()->SetTitle(title.c_str());
	  canv->Modified();

	  hMeans[iecAC][ilayer][ieta][iyx]->Write();
	  
	  canv->Print((outputdir+"/"+canvname+"_allPhi_tantheta.eps").c_str());
	  
	  delete canv;
	}
      }
    }
  }
  
  return;
}

//________________________________________________________________________
void makeMDTResPlots(TFile* outputHists)
{
  gStyle->SetOptStat("uon");
  gStyle->SetOptFit(111);

  TH1F**** hPull  =new TH1F***[2];
  TH1F***** hMeans=new TH1F****[2];
  TF1****** fits  =new TF1*****[2];  
  for (int iecAC=0;iecAC<2;iecAC++) {
    hMeans[iecAC]=new TH1F***[3];
    fits  [iecAC]=new TF1****[3];
    hPull  [iecAC]=new TH1F**[3];
    for (int ilayer=0;ilayer<3;ilayer++) {
      hMeans[iecAC][ilayer]=new TH1F**[6];
      fits  [iecAC][ilayer]=new TF1***[6];
      hPull  [iecAC][ilayer]=new TH1F*[6];
      for (int ieta=0;ieta<6;ieta++) {
	hMeans[iecAC][ilayer][ieta]=new TH1F*[NYX];
	fits  [iecAC][ilayer][ieta]=new TF1**[NYX];
	int ixy=0;
	for (int iyx=NYX-1;iyx>=0;iyx--,ixy++) {
	  string title="#Delta ";
	  if (iyx==0) title+="y"; else title+="x";
	  title+=" (mm)";
	  
	  string hmeanname="mean_"+mdtChamberNameNoPhi(iecAC,ilayer,ieta);
	  if (iyx==0) hmeanname+="_dY"; else hmeanname+="_dX";
	  hMeans[iecAC][ilayer][ieta][iyx]=new TH1F(hmeanname.c_str(),hmeanname.c_str(),16,0.,16.);
	  fits  [iecAC][ilayer][ieta][iyx]=new TF1*[16];
	  if (ieta==0) {
	    string histname="mean_"+mdtChamberNameNoEta(iecAC,ilayer)+"_dY_all";
	    hPull  [iecAC][ilayer][iyx]=new TH1F(histname.c_str(),
						histname.c_str(),
						40,-10.,10.);
	  }
	  TCanvas* canv=new TCanvas("canv","canv");
	  canv->SetFillColor(10);
	  canv->Divide(4,4);
	  
	  for (int iphi=0;iphi<16;iphi++) {
	    canv->cd(iphi+1);
	    string fitname="fitRes_"+mdtChamberName(iecAC,ilayer,ieta,iphi);
	    if (iyx==0) fitname+="_dY"; else fitname+="_dX";
	    double fitrange=(iyx==0) ? 25. : 40.;
	    if (ilayer==1) fitrange*=0.5;

	    TH1F* hist=phiSecEtaHistsMDT[iecAC][ilayer][ieta][iphi][iyx];

	    double innerwidth=(iyx==0)?4.:8.;
	    double outerwidth=(iyx==0)?10.:20.;
	    //if (ilayer<2) {
	    //innerwidth*=2;
	    //outerwidth*=2;
	    //}
	    if (hist->GetEntries()>1000&&ilayer==2) {
	      fits[iecAC][ilayer][ieta][iyx][iphi]=
		new TF1(fitname.c_str(),
			"[0]*exp(-0.5*((x-[1])/[2])^2)+[3]*[0]*exp(-0.5*((x-[1])/[4])^2)",
			-fitrange,fitrange);
	      fits[iecAC][ilayer][ieta][iyx][iphi]
		->SetParameters(hist->GetMaximum(),hist->GetMean(),innerwidth,0.8,outerwidth);
	      fits[iecAC][ilayer][ieta][iyx][iphi]
		->SetParLimits(4,innerwidth*2.,outerwidth*5.);
	    }
	    else {
	      fits[iecAC][ilayer][ieta][iyx][iphi]=
		new TF1(fitname.c_str(),"gaus",-fitrange,fitrange);
	      fits[iecAC][ilayer][ieta][iyx][iphi]
		->SetParameters(hist->GetMaximum(),hist->GetMean(),innerwidth);
	    }
	    
	    TF1*  fit =fits[iecAC][ilayer][ieta][iyx][iphi];
	    fit->SetParLimits(2,innerwidth/4.,innerwidth*2.);
	    
	    hist->Fit(fit,"RQL");
	    
	    hist->GetXaxis()->SetTitle(title.c_str());
	    
	    hist->Draw();
	    fit->Draw("same");
	    
	    stringstream binLabel; binLabel<<iphi+1;
	    if (iecAC==0) binLabel<<"A"; else binLabel<<"C";
	    hMeans[iecAC][ilayer][ieta][iyx]->GetXaxis()->SetBinLabel(iphi+1,binLabel.str().c_str());
	    //if (fit->GetChisquare()/fit->GetNDF()<5. && hist->GetEntries()>1000.) {
	      hMeans[iecAC][ilayer][ieta][iyx]->SetBinContent(iphi+1,fit->GetParameter(1));
	      hMeans[iecAC][ilayer][ieta][iyx]->SetBinError  (iphi+1,fit->GetParError(1));
	      if (iyx==0) cout<<"dY: "; else cout<<"dX: ";
	      cout<<iphi+1;
	      if (iecAC==0) cout<<"A"; else cout<<"C";
	      cout<<" "<<fit->GetParameter(1)<<" +/- "<<fit->GetParError(1)<<endl;
	      
	      if ((ilayer==0&&ieta<2) ||
		  (ilayer==1&&ieta<3) ||
		  (ilayer==2))
		hPull[iecAC][ilayer][iyx]->Fill(fit->GetParameter(1)/fit->GetParError(1));
	      //}
	  }
	  
	  
	  string canvname=mdtChamberNameNoPhi(iecAC,ilayer,ieta);
	  if (iyx==0) canvname+="_dY"; else canvname+="_dX";
	  canv->Print((outputdir+"/"+canvname+".eps").c_str());
	  
	  canv->Clear();
	  
	  if (hMeans[iecAC][ilayer][ieta][iyx]->GetMaximum()>20. ||
	      hMeans[iecAC][ilayer][ieta][iyx]->GetMaximum()<-20.) {
	    hMeans[iecAC][ilayer][ieta][iyx]->SetMaximum(20.);
	    hMeans[iecAC][ilayer][ieta][iyx]->SetMinimum(-20.);
	  }

	  hMeans[iecAC][ilayer][ieta][iyx]->Draw();
	  hMeans[iecAC][ilayer][ieta][iyx]->GetYaxis()->SetTitle(title.c_str());
	  canv->Modified();

	  hMeans[iecAC][ilayer][ieta][iyx]->Write();
	  
	  canv->Print((outputdir+"/"+canvname+"_allPhi.eps").c_str());
      
	  canv->Clear();
	  if (ieta==5) {
	    hPull[iecAC][ilayer][iyx]->Draw();
	    hPull[iecAC][ilayer][iyx]->Fit("gaus","LQ");
	    hPull[iecAC][ilayer][iyx]->GetXaxis()->SetTitle("pull");
	    canv->Modified();
	    
	    hPull[iecAC][ilayer][iyx]->Write();
	    string canvname2=mdtChamberNameNoEta(iecAC,ilayer)+"_dY";
	    canv->Print((outputdir+"/"+canvname2+"_all.eps").c_str());
	  }
	  delete canv;
	}
      }
    }
  }
  
  return;

}

/*
//________________________________________________________________________
void makeWidthPlots(TFile* outputHists, const string& type)
{
  gStyle->SetOptStat(1);
  gStyle->SetOptFit(111);

  TF1***   fits=new TF1**[2];
  TH1F**** slices=new TH1F***[2];
  TH1F***  hSigmas=new TH1F**[2];

  TH2F*** hists;
  if      (type=="EndcapAC")   hists=ecHistsCSC;
  else if (type=="LargeSmall") hists=lgsmHistsCSC;
  else cout<<"bad type"<<endl;

  int nplots=2;  
  for (int iplot=0;iplot<nplots;iplot++) {
    
    fits  [iplot]=new TF1*  [NYX];
    slices[iplot]=new TH1F**[NYX];
    hSigmas[iplot]=new TH1F*[NYX];
    
    for (int iyx=NYX-1;iyx>=0;iyx--) {
      double fitrange=(iyx==0)?40.:80.;
      stringstream fitname; fitname<<"fit_"<<iplot<<"_"<<iyx;
      //fits[iplot][iyx]=new TF1(fitname.str().c_str(),"[0]*exp(-0.5*((x-[1])/[2])^2)+[3]*[0]*exp(-0.5*((x-[4])/[5])^2)",-fitrange,fitrange);
      fits[iplot][iyx]=new TF1(fitname.str().c_str(),"gaus",-fitrange,fitrange);
      TObjArray aSlices;
      hists[iplot][iyx]->FitSlicesY(fits[iplot][iyx],0,NMOM,0,"QNR",&aSlices);
      const int nSlices=aSlices.GetEntriesFast();
      slices[iplot][iyx]=new TH1F*[nSlices];
      for (int islice=0;islice<nSlices;islice++) {
	slices[iplot][iyx][islice]=(TH1F*)aSlices[islice]->Clone();
      }
      string hname=hists[iplot][iyx]->GetName()+string("_2");      
      hSigmas[iplot][iyx]=(TH1F*)gDirectory->Get(hname.c_str())->Clone((hists[iplot][iyx]->GetName()+string("_sigma")).c_str());
      
    }
  }

  TCanvas* ctmp=new TCanvas("ctmp","ctmp");
  TF1*** fits1OverP=new TF1**[NYX];
  TPaveStats*** paves=new TPaveStats**[NYX];
  double min[2]={999.,999.};
  double max[2]={0.,  0.};
  for (int iyx=NYX-1;iyx>=0;iyx--) {
    fits1OverP[iyx]=new TF1*[2];
    paves[iyx]=new TPaveStats*[2];    
    
    // fit to 1/p and get min/max
    for (int iplot=0;iplot<nplots;iplot++) {
      for (int ibin=0;ibin<(int)hSigmas[iplot][iyx]->GetNbinsX();ibin++) {
	double bc=hSigmas[iplot][iyx]->GetBinContent(ibin+1);
	if (bc>max[iyx])        max[iyx]=bc;
	if (bc>0.&&bc<min[iyx]) min[iyx]=bc;	
      }
      stringstream fitname; fitname<<"fit1OverP_"<<iyx<<"_ecAC"<<iplot;
      //fits1OverP[iyx][iplot]=new TF1(fitname.str().c_str(),"[0]+[1]/(x-3.)",9.,mommax);
      fits1OverP[iyx][iplot]=new TF1(fitname.str().c_str(),"TMath::Sqrt([0]*[0]+[1]*[1]/(x-3.5)/(x-3.5))",9.,mommax);

      hSigmas[iplot][iyx]->Fit(fits1OverP[iyx][iplot],"RQ");

      ctmp->Update();
      paves[iyx][iplot]=(TPaveStats*)hSigmas[iplot][iyx]->FindObject("stats");
      if (iplot==0) {
	paves[iyx][iplot]->SetY1NDC(0.8);
	paves[iyx][iplot]->SetY2NDC(1.0);
      }
      if (iplot==1) {
	paves[iyx][iplot]->SetY1NDC(0.6);
	paves[iyx][iplot]->SetY2NDC(0.8);
      }
      paves[iyx][iplot]->SetTextColor(twocolors[iplot]);
      ctmp->Modified();    
    }
    if (max[iyx]>0.) max[iyx]*=1.1; else max[iyx]*=0.8;
    if (min[iyx]<0.) min[iyx]*=1.1; else min[iyx]*=0.8;
  }
  delete ctmp;

  TCanvas* canv=new TCanvas("canv","canv");
  canv->SetFillColor(10);
  canv->Divide(NYX,1);
  int ixy=0;
  for (int iyx=NYX-1;iyx>=0;iyx--,ixy++) {
    canv->cd(ixy+1);
    for (int iplot=0;iplot<nplots;iplot++) {
      hSigmas[iplot][iyx]->SetMinimum(min[iyx]);
      hSigmas[iplot][iyx]->SetMaximum(1.1*max[iyx]);
      hSigmas[iplot][iyx]->SetLineColor(twocolors[iplot]);
      
      fits1OverP[iyx][iplot]->SetLineWidth(2);
      fits1OverP[iyx][iplot]->SetLineColor(twocolors[iplot]);
      
      if (iplot==0) hSigmas[iplot][iyx]->Draw();
      else          hSigmas[iplot][iyx]->Draw("same");
      fits1OverP[iyx][iplot]->Draw("same");
    }
  }
  string detStr=(DOCSC)?"CSC":"MDT";
  if (DOBEE) detStr="BEE";
  if (type=="EndcapAC")
    canv->Print((outputdir+"/"+detStr+"_EndcapACWidth.eps").c_str());
  else if (type=="LargeSmall")
    canv->Print((outputdir+"/"+detStr+"_LgSmWidth.eps").c_str());
  else
    cout<<"bad type"<<endl;
    
  delete canv;
  delete [] hSigmas;

  return;
}
*/

//________________________________________________________________________
void makeWidthPlots(TFile* outputHists, const string& type)
{
  gStyle->SetOptStat(1);
  gStyle->SetOptFit(111);

  int nplots=2;  

  TF1****   fits=new TF1***[nplots];
  TH1F***** slices=new TH1F****[nplots];
  TH1F****  hSigmas=new TH1F***[nplots];

  TH2F**** hists;
  if      (type=="EndcapAC")   hists=ecHistsCSC;
  else if (type=="LargeSmall") hists=lgsmHistsCSC;
  else cout<<"bad type"<<endl;

  for (int iplot=0;iplot<nplots;iplot++) {
    
    fits  [iplot]=new TF1**  [3];
    slices[iplot]=new TH1F***[3];
    hSigmas[iplot]=new TH1F**[3];
    
    for (int ilayer=0;ilayer<3;ilayer++) {

      fits  [iplot][ilayer]=new TF1*  [3];
      slices[iplot][ilayer]=new TH1F**[3];
      hSigmas[iplot][ilayer]=new TH1F*[3];
      
      
      for (int iyx=NYX-1;iyx>=0;iyx--) {
	double fitrange=(iyx==0)?40.:80.;
	stringstream fitname; fitname<<"fit_"<<iplot<<"_"<<ilayer<<"_"<<iyx;
	//fits[iplot][iyx]=new TF1(fitname.str().c_str(),"[0]*exp(-0.5*((x-[1])/[2])^2)+[3]*[0]*exp(-0.5*((x-[4])/[5])^2)",-fitrange,fitrange);
	fits[iplot][ilayer][iyx]=new TF1(fitname.str().c_str(),"gaus",-fitrange,fitrange);
	TObjArray aSlices;
	hists[iplot][ilayer][iyx]->FitSlicesY(fits[iplot][ilayer][iyx],0,NMOM,0,"QNR",&aSlices);
	const int nSlices=aSlices.GetEntriesFast();
	slices[iplot][ilayer][iyx]=new TH1F*[nSlices];
	for (int islice=0;islice<nSlices;islice++) {
	  slices[iplot][ilayer][iyx][islice]=(TH1F*)aSlices[islice]->Clone();
	}
	string hname=hists[iplot][ilayer][iyx]->GetName()+string("_2");      
	hSigmas[iplot][ilayer][iyx]=(TH1F*)gDirectory->Get(hname.c_str())->Clone((hists[iplot][ilayer][iyx]->GetName()+string("_sigma")).c_str());
	
      }
    }
  }
  
  TCanvas* ctmp=new TCanvas("ctmp","ctmp");
  TF1**** fits1OverP=new TF1***[3];
  TPaveStats**** paves=new TPaveStats***[3];
  double min[2]={999.,999.};
  double max[2]={0.,  0.};
  for (int ilayer=0;ilayer<3;ilayer++) {
    fits1OverP[ilayer]=new TF1**[NYX];
    paves      [ilayer]=new TPaveStats**[NYX];

    for (int iyx=NYX-1;iyx>=0;iyx--) {
      fits1OverP[ilayer][iyx]=new TF1*[2];
      paves[ilayer][iyx]=new TPaveStats*[2];    
      
      // fit to 1/p and get min/max
      for (int iplot=0;iplot<nplots;iplot++) {
	for (int ibin=0;ibin<(int)hSigmas[iplot][ilayer][iyx]->GetNbinsX();ibin++) {
	  double bc=hSigmas[iplot][ilayer][iyx]->GetBinContent(ibin+1);
	  if (bc>max[iyx])        max[iyx]=bc;
	  if (bc>0.&&bc<min[iyx]) min[iyx]=bc;	
	}
	stringstream fitname; fitname<<"fit1OverP_"<<ilayer<<"_"<<iyx<<"_ecAC"<<iplot;
	fits1OverP[ilayer][iyx][iplot]=
	  new TF1(fitname.str().c_str(),"TMath::Sqrt([0]*[0]+[1]*[1]/(x-3.5)/(x-3.5))",9.,mommax);
	
	hSigmas[iplot][ilayer][iyx]->Fit(fits1OverP[ilayer][iyx][iplot],"RQ");
	
	ctmp->Update();
	paves[ilayer][iyx][iplot]=(TPaveStats*)hSigmas[iplot][ilayer][iyx]->FindObject("stats");
	if (iplot==0) {
	  paves[ilayer][iyx][iplot]->SetY1NDC(0.8);
	  paves[ilayer][iyx][iplot]->SetY2NDC(1.0);
	}
	if (iplot==1) {
	  paves[ilayer][iyx][iplot]->SetY1NDC(0.6);
	  paves[ilayer][iyx][iplot]->SetY2NDC(0.8);
	}
	paves[ilayer][iyx][iplot]->SetTextColor(twocolors[iplot]);
	ctmp->Modified();    
      }
      if (max[iyx]>0.) max[iyx]*=1.1; else max[iyx]*=0.8;
      if (min[iyx]<0.) min[iyx]*=1.1; else min[iyx]*=0.8;
    }
  }
  delete ctmp;
  
  TCanvas* canv=new TCanvas("canv","canv");
  canv->SetFillColor(10);
  canv->Divide(NYX,1);
  for (int ilayer=0;ilayer<3;ilayer++) {
    int ixy=0;
    for (int iyx=NYX-1;iyx>=0;iyx--,ixy++) {
      canv->cd(ixy+1);
      for (int iplot=0;iplot<nplots;iplot++) {
	hSigmas[iplot][ilayer][iyx]->SetMinimum(min[iyx]);
	hSigmas[iplot][ilayer][iyx]->SetMaximum(1.1*max[iyx]);
	hSigmas[iplot][ilayer][iyx]->SetLineColor(twocolors[iplot]);
	
	fits1OverP[ilayer][iyx][iplot]->SetLineWidth(2);
	fits1OverP[ilayer][iyx][iplot]->SetLineColor(twocolors[iplot]);
	
	if (iplot==0) hSigmas[iplot][ilayer][iyx]->Draw();
	else          hSigmas[iplot][ilayer][iyx]->Draw("same");
	fits1OverP[ilayer][iyx][iplot]->Draw("same");
      }
    }
    string detStr=(DOCSC)?"CSC":"MDT";
    if (DOBEE) detStr="BEE";
    string layerStr;
    switch (ilayer) {
    case 0: layerStr="inner"; break;
    case 1: layerStr="middle"; break;
    case 2: layerStr="outer"; break;
    default: layerStr=""; break;
    }
    if (type=="EndcapAC")
      canv->Print((outputdir+"/"+detStr+"_"+layerStr+"_EndcapACWidth.eps").c_str());
    else if (type=="LargeSmall")
      canv->Print((outputdir+"/"+detStr+"_"+layerStr+"_LgSmWidth.eps").c_str());
    else
      cout<<"bad type"<<endl;
  }
  delete canv;
  delete [] hSigmas;
  
  return;
}

//________________________________________________________________________
void setBranchAddresses(TTree* tree)
{
  tree->SetBranchAddress("nPixHits", &nPixHits);
  tree->SetBranchAddress("nSCTHits", &nSCTHits);
  tree->SetBranchAddress("nTRTHits", &nTRTHits);
  tree->SetBranchAddress("nCSCHits", &nCSCHits);
  tree->SetBranchAddress("nMDTHits", &nMDTHits);
  tree->SetBranchAddress("nInnerMDTHits", &nInnerMDTHits);
  tree->SetBranchAddress("nMiddleMDTHits", &nMiddleMDTHits);
  tree->SetBranchAddress("nOuterMDTHits", &nOuterMDTHits);
  tree->SetBranchAddress("nTriggerPhiHits", &nTriggerPhiHits);
  tree->SetBranchAddress("author", &author);
  tree->SetBranchAddress("hIndex", &hIndex);
  tree->SetBranchAddress("phiSector", &phiSector);
  tree->SetBranchAddress("isEndcap", &isEndcap);
  tree->SetBranchAddress("identifier", &identifier);
  tree->SetBranchAddress("isCSide", &isCSide);
  tree->SetBranchAddress("detType", &detType);
  tree->SetBranchAddress("layer", &layer);
  tree->SetBranchAddress("stName", &stName);
  tree->SetBranchAddress("stPhi", &stPhi);
  tree->SetBranchAddress("stEta", &stEta);
  tree->SetBranchAddress("momentum", &momentum);
  tree->SetBranchAddress("charge", &charge);
  tree->SetBranchAddress("d0", &d0);
  tree->SetBranchAddress("z0", &z0);
  tree->SetBranchAddress("phi0", &phi0);
  tree->SetBranchAddress("theta0", &theta0);
  tree->SetBranchAddress("eta0", &eta0);
  tree->SetBranchAddress("calEnergyLoss", &calEnergyLoss);
  tree->SetBranchAddress("calEnergyLossErr", &calEnergyLossErr);
  tree->SetBranchAddress("materialOnTrack", &materialOnTrack);
  tree->SetBranchAddress("dR", &dR);
  tree->SetBranchAddress("dZlocal", &dZlocal);
  tree->SetBranchAddress("dXlocal", &dXlocal);
  tree->SetBranchAddress("dYlocal", &dYlocal);
  tree->SetBranchAddress("dXlocalAngle", &dXlocalAngle);
  tree->SetBranchAddress("dYlocalAngle", &dYlocalAngle);
  tree->SetBranchAddress("dXglobal", &dXglobal);
  tree->SetBranchAddress("dYglobal", &dYglobal);
  tree->SetBranchAddress("dZglobal", &dZglobal);
  tree->SetBranchAddress("nInnerChambers", &nInnerChambers);
  tree->SetBranchAddress("nMiddleChambers", &nMiddleChambers);
  tree->SetBranchAddress("nOuterChambers", &nOuterChambers);
  tree->SetBranchAddress("nOtherChambers", &nOtherChambers);
  tree->SetBranchAddress("detTheta", &detTheta);
  tree->SetBranchAddress("innerStEta", innerStEta);
  tree->SetBranchAddress("middleStEta", middleStEta);
  tree->SetBranchAddress("outerStEta", outerStEta);
  tree->SetBranchAddress("otherStEta", otherStEta);

  return;
}

//________________________________________________________________________
void getHistograms(bool histsDone)
{
  string histname=outputdir+"/inputHists";
  string detStr=(DOCSC)?"CSC":"MDT";

  if (!DOCSC && DOBEE) detStr="BEE";

  if (DOCSC)       histname+="_CSC.root";
  else if (!DOBEE) histname+="_MDT.root";
  else             histname+="_MDT_BEE.root";

  if (histsDone) inputHistsFile=new TFile(histname.c_str(),"READ");

  // large/small hists
  lgsmHistsCSC=new TH2F***[2]; 
  for (int ilgsm=0;ilgsm<2;ilgsm++) {
    lgsmHistsCSC[ilgsm]=new TH2F**[3];
    for (int ilayer=0;ilayer<3;ilayer++) {
      lgsmHistsCSC[ilgsm][ilayer]=new TH2F*[2];
      for (int iyx=0;iyx<NYX;iyx++) {
	double valmax=(iyx==0) ? 40.4: 80.8;
	double valmin=-valmax;
	string name=(ilgsm==0)?"LargeHists":"SmallHists";
	if (ilayer==0)      name+="_inner";
	else if (ilayer==1) name+="_middle";
	else                name+="_outer";

	if (iyx==0) name+="_dY"; else name+="_dX";
	name+="_"+detStr;
	if (!histsDone) {
	  cout<<"writing "<<name<<endl;
	  lgsmHistsCSC[ilgsm][ilayer][iyx]=new TH2F(name.c_str(), name.c_str(),
						    NMOM,mommin,mommax,NVALBINS,valmin,valmax);    
	}
	else {
	  cout<<"getting "<<name<<endl;
	  lgsmHistsCSC[ilgsm][ilayer][iyx]=(TH2F*)inputHistsFile->Get(name.c_str());
	  cout<<"hist has "<<lgsmHistsCSC[ilgsm][ilayer][iyx]->GetEntries()<<" entries"<<endl;
	}
      }
    }
  }

  // endcap AC hists
  ecHistsCSC=new TH2F***[2];
  for (int iecAC=0;iecAC<2;iecAC++) {
    ecHistsCSC[iecAC]=new TH2F**[3];
    for (int ilayer=0;ilayer<3;ilayer++) {
      ecHistsCSC[iecAC][ilayer]=new TH2F*[NYX];
      for (int iyx=0;iyx<NYX;iyx++) {
	double valmax=(iyx==0) ? 40.4: 80.8;
	double valmin=-valmax;
	string name=(iecAC==0)?"EndcapA":"EndcapC";
	if (ilayer==0)      name+="_inner";
	else if (ilayer==1) name+="_middle";
	else                name+="_outer";
	if (iyx==0) name+="_dY"; else name+="_dX";
	name+="_"+detStr;
	if (!histsDone) {
	cout<<"writing "<<name<<endl;
	ecHistsCSC[iecAC][ilayer][iyx]=new TH2F(name.c_str(), name.c_str(),
						NMOM,mommin,mommax,NVALBINS,valmin,valmax);    
	}
	else {
	  cout<<"getting "<<name<<endl;
	  ecHistsCSC[iecAC][ilayer][iyx]=(TH2F*)inputHistsFile->Get(name.c_str());
	  cout<<"hist has "<<ecHistsCSC[iecAC][ilayer][iyx]->GetEntries()<<" entries"<<endl;
	}
      }
    }    
  }

  // CSC chamber hists
  phiSecHistsCSC      =new TH1F***[2];
  resVTanThetaHistsCSC=new TH2F***[2];
  for (int iecAC=0;iecAC<2;iecAC++) {
    phiSecHistsCSC      [iecAC]=new TH1F**[16];
    resVTanThetaHistsCSC[iecAC]=new TH2F**[16];
    for (int iphi=0;iphi<16;iphi++) {
      phiSecHistsCSC      [iecAC][iphi]=new TH1F*[2];
      resVTanThetaHistsCSC[iecAC][iphi]=new TH2F*[2];
      for (int iyx=0;iyx<NYX;iyx++) {
	double valmax=(iyx==0) ? 40.4 : 80.8;
	double valmin=-valmax;
	stringstream name; name<<"Sector"<<iphi+1;
	if (iecAC==0) name<<"A"; else name<<"C";
	if (iyx==0) name<<"_dY"; else name<<"_dX";
	name<<"_CSC";
	if (!histsDone) {
	  phiSecHistsCSC[iecAC][iphi][iyx]=new TH1F(name.str().c_str(),name.str().c_str(),
						    NVALBINS,valmin,valmax);
	  double min=std::tan(-2.5*3.14159/180.);
	  double max=std::tan( 3.5*3.14159/180.);
			      
	  resVTanThetaHistsCSC[iecAC][iphi][iyx]=
	    new TH2F((name.str()+"_resVTanTheta").c_str(),
		     (name.str()+"_resVTanTheta").c_str(),
		     5,min,max,50,valmin,valmax);
	}
	else {
	  phiSecHistsCSC[iecAC][iphi][iyx]=(TH1F*)inputHistsFile->Get(name.str().c_str());
	  resVTanThetaHistsCSC[iecAC][iphi][iyx]=(TH2F*)inputHistsFile->Get((name.str()+"_resVTanTheta").c_str());
	}
      }
    }
  }

  // MDT phi-sector hists
  phiSecHistsMDT=new TH1F**[2];
  for (int iecAC=0;iecAC<2;iecAC++) {
    phiSecHistsMDT[iecAC]=new TH1F*[16];
    for (int iphi=0;iphi<16;iphi++) {
      double valmax=40.4;
      double valmin=-valmax;
      stringstream name; name<<"Sector"<<iphi+1;
      if (iecAC==0) name<<"A"; else name<<"C";
      name<<"_dY_MDT";
      if (!histsDone) {
	phiSecHistsMDT[iecAC][iphi]=new TH1F(name.str().c_str(),name.str().c_str(),
					     NVALBINS,valmin,valmax);
	double min=std::tan(-2.5*3.14159/180.);
	double max=std::tan( 3.5*3.14159/180.);
	
      }
      else {
	phiSecHistsMDT[iecAC][iphi]=(TH1F*)inputHistsFile->Get(name.str().c_str());
      }
    }
  }  

  // EC MDT chamber hists
  phiSecEtaHistsMDT      =new TH1F*****[2];
  resVTanThetaHistsMDT=new TH2F*****[2];
  for (int iecAC=0;iecAC<2;iecAC++) {
    phiSecEtaHistsMDT      [iecAC]=new TH1F****[3];
    resVTanThetaHistsMDT[iecAC]=new TH2F****[3];
    for (int ilayer=0;ilayer<3;ilayer++) {
      phiSecEtaHistsMDT      [iecAC][ilayer]=new TH1F***[6];
      resVTanThetaHistsMDT[iecAC][ilayer]=new TH2F***[6];
      for (int ieta=0;ieta<6;ieta++) {
	phiSecEtaHistsMDT      [iecAC][ilayer][ieta]=new TH1F**[16];
	resVTanThetaHistsMDT[iecAC][ilayer][ieta]=new TH2F**[16];
	for (int iphi=0;iphi<16;iphi++) {
	  phiSecEtaHistsMDT      [iecAC][ilayer][ieta][iphi]=new TH1F*[2];
	  resVTanThetaHistsMDT[iecAC][ilayer][ieta][iphi]=new TH2F*[2];
	  for (int iyx=0;iyx<NYX;iyx++) {
	    double valmax=(iyx==0) ? 40.4 : 80.8;
	    if (ilayer==1) valmax*=.5;

	    double valmin=-valmax;
	    stringstream name; 
	    name<<mdtChamberName(iecAC,ilayer,ieta,iphi);
	    /*
	    name<<"E";
	    if (ilayer==0) name<<"I";
	    else if (ilayer==1) name<<"M";
	    else if (ilayer==2) name<<"O";
	    if (iphi%2==0) name<<"L"; else name<<"S";
	    name<<ieta+1;
	    if (iecAC==0) name<<"A"; else name<<"C";
	    if (iphi%2==0) name<<iphi/2+1;
	    else           name<<(iphi-1)/2+1;
	    */
	    if (iyx==0) name<<"_dY"; else name<<"_dX";
	    if (!histsDone) {
	      phiSecEtaHistsMDT[iecAC][ilayer][ieta][iphi][iyx]=
		new TH1F(name.str().c_str(),name.str().c_str(),
			 NVALBINS,valmin,valmax);
	      resVTanThetaHistsMDT[iecAC][ilayer][ieta][iphi][iyx]=
		new TH2F((name.str()+"VTanTheta").c_str(),
			 (name.str()+"VTanTheta").c_str(),
			 5,tanThetaMin[ilayer][ieta],tanThetaMax[ilayer][ieta],
			 50,valmin,valmax);
	    }
	    else {
	      phiSecEtaHistsMDT[iecAC][ilayer][ieta][iphi][iyx]=
		(TH1F*)inputHistsFile->Get(name.str().c_str());
	      resVTanThetaHistsMDT[iecAC][ilayer][ieta][iphi][iyx]=
		(TH2F*)inputHistsFile->Get((name.str()+"VTanTheta").c_str());
	    }
	  }
	}
      }
    }
  }
  
  if (!histsDone) {
    //string filename=(!USETGCETA)?"/data/roberth/bfieldoff_V6_noTGC/UnbiasedMuonResiduals.root":"/data/roberth/bfieldoff_V6/UnbiasedMuonResiduals.root";
    //string filename=(!USETGCETA)?"/data/roberth/bfieldoff_V6_noTGC/UnbiasedMuonResiduals.root":"/data/roberth/bfieldoff_V6/UnbiasedMuonResiduals.root";
    string filename="/data/roberth/bfieldoff_V10/UnbiasedMuonResiduals.root";

    /*
    TFile* inputfile=new TFile(filename.c_str(),"READ");
    TTree* tree=(TTree*)inputfile->Get("unbiasedMuonResiduals");
    */
    TChain* tree=new TChain("unbiasedMuonResiduals");
    tree->Add("/data/roberth/bfieldoff_V10/UnbiasedMuonResiduals*root");

    setBranchAddresses(tree);
    
    cout<<"tree has "<<tree->GetEntries()<<" entries"<<endl;
    for (int ientry=0;ientry<(int)tree->GetEntries();ientry++) {
      tree->GetEntry(ientry);

      // CSC hists     
      // single inner layer hit
      if (DOCSC && nInnerChambers>1) continue;

      if (DOBEE) {
	if (nOtherChambers<1) continue;
      }
      else {
	if (nOtherChambers>0) continue;
      }

      // CSC/MDT selection
      if (DOCSC && detType==0) continue;
      if (!DOCSC) {
	if (detType==1) continue;
	//if (layer!=0 || (stEta!=1&&stEta!=-1)) continue;
      }
      
      // select Muid/Moore track
      if (hIndex!=0) continue;
      
      // select endcap
      if (DOEC && isEndcap==0) continue;

      // I don't know what's going on with these guys
      //if ((stEta>0&&detTheta>25.) || (stEta<0&&detTheta<(180.-25.))) continue;

      // fill histogram
      int ilgsm=(phiSector%2==1)?0:1;
      int iecAC=(stEta>0)?0:1; 
      int iphi =phiSector-1;
      for (int iyx=0;iyx<NYX;iyx++) {
	double val=(iyx==0)?dYlocal:dXlocal;
	int theta=(stEta>0)?detTheta : 180.-detTheta;

	if (layer<3) {
	  lgsmHistsCSC[ilgsm][layer][iyx]->Fill(momentum,val);
	  ecHistsCSC  [iecAC][layer][iyx]->Fill(momentum,val);
	}

	if (DOCSC) {
	  if (momentum>=momCut) {
	    phiSecHistsCSC[iecAC][iphi][iyx]->Fill(val);
	    double tanTheta=std::tan((theta-11.59)*3.14159/180.);
	    resVTanThetaHistsCSC[iecAC][iphi][iyx]->Fill(tanTheta,val);
	  }
	}
	else {
	  if (layer>2) continue;
	  
	  /*
	  if (layer==0&&nInnerChambers>1) continue;
	  if (layer==1&&nMiddleChambers>1) continue;
	  if (layer==2&&nOuterChambers>1) continue;
	  */
	  // require only 1 chamber in each layer
	  if (nInnerChambers!=1||nMiddleChambers!=1||nOuterChambers!=1) continue;
	  //cout<<"stEta: "<<stEta<<", innerStEta[0]="<<innerStEta[0]<<", "<<"middleStEta[0]="<<middleStEta[0]<<", "<<"outerStEta[0]="<<outerStEta[0]<<endl;
 
	  /*
	  if (layer==1) {
	    int ieta=(stEta>0)?stEta-1:-stEta-1;
	    int thisInnerEta=(stEta>0)?innerStEta[0]:-innerStEta[0];
	    int thisOuterEta=(stEta>0)?outerStEta[0]:-outerStEta[0];

	    if (thisInnerEta!=allowedInnerLayerForMiddleFixed[ieta]) continue;
	    if (thisOuterEta!=allowedOuterLayerForMiddleFixed[ieta]) continue;
	  }
	  */

	  int ieta=abs(stEta)-1;
	  if (momentum>=momCut) {
	    phiSecEtaHistsMDT[iecAC][layer][ieta][iphi][iyx]->Fill(val);
	    phiSecHistsMDT[iecAC][iphi]->Fill(val);
	    double tanTheta=std::tan(theta*3.14159/180.);
	    resVTanThetaHistsMDT[iecAC][layer][ieta][iphi][iyx]->Fill(tanTheta,val);	  
	  }
	}
      }
    }
    //inputfile->Close();
  
    inputHistsFile=new TFile(histname.c_str(),"RECREATE");
    for (int ilgsm=0;ilgsm<2;ilgsm++) {
      for (int ilayer=0;ilayer<3;ilayer++) {
	for (int iyx=0;iyx<NYX;iyx++) {
	  lgsmHistsCSC[ilgsm][ilayer][iyx]->Write();
	}
      }
    }
    for (int iecAC=0;iecAC<2;iecAC++) {
      for (int ilayer=0;ilayer<3;ilayer++) {	
	for (int iyx=0;iyx<NYX;iyx++) {
	  ecHistsCSC[iecAC][ilayer][iyx]->Write();
	}
      }
    }
    if (DOCSC) {
      for (int iecAC=0;iecAC<2;iecAC++) {
	for (int iphi=0;iphi<16;iphi++) {
	  for (int iyx=0;iyx<NYX;iyx++) {
	    phiSecHistsCSC[iecAC][iphi][iyx]->Write();
	    resVTanThetaHistsCSC[iecAC][iphi][iyx]->Write();
	  }
	}
      }
    }
    else { // MDT hists
      for (int iecAC=0;iecAC<2;iecAC++) {
	for (int iphi=0;iphi<16;iphi++)
	  phiSecHistsMDT[iecAC][iphi]->Write();
	for (int ilayer=0;ilayer<3;ilayer++) {
	  for (int ieta=0;ieta<6;ieta++) {
	    for (int iphi=0;iphi<16;iphi++) {
	      for (int iyx=0;iyx<NYX;iyx++) {
		phiSecEtaHistsMDT[iecAC][ilayer][ieta][iphi][iyx]->Write();
		resVTanThetaHistsMDT[iecAC][ilayer][ieta][iphi][iyx]->Write();
	      }
	    }
	  }
	}
      }      
    }
  }
  
  return;

}

//________________________________________________________________________
string mdtChamberName(int iecAC, int ilayer, int ieta, int iphi)
{
 
  stringstream name; name<<"E";
  if (ilayer==0)      name<<"I";
  else if (ilayer==1) name<<"M";
  else if (ilayer==2) name<<"O";
  else cout<<"bad layer"<<endl;

  if (iphi%2==0) name<<"L";
  else           name<<"S";
  
  name<<ieta+1;
  
  if (iecAC==0) name<<"A";
  else          name<<"C";

  if (iphi%2==0) name<<iphi/2+1;
  else           name<<(iphi-1)/2+1;
    
  return name.str();
}

//________________________________________________________________________
string mdtChamberNameNoEta(int iecAC, int ilayer)
{
 
  stringstream name; name<<"E";
  if (ilayer==0)      name<<"I";
  else if (ilayer==1) name<<"M";
  else if (ilayer==2) name<<"O";
  else cout<<"bad layer"<<endl;

  name<<"X";
  
  if (iecAC==0) name<<"A";
  else          name<<"C";

  return name.str();
}

//________________________________________________________________________
string mdtChamberNameNoPhi(int iecAC, int ilayer, int ieta)
{
 
  stringstream name; name<<"E";
  if (ilayer==0)      name<<"I";
  else if (ilayer==1) name<<"M";
  else if (ilayer==2) name<<"O";
  else cout<<"bad layer"<<endl;

  name<<"X";
  
  name<<ieta+1;
  
  if (iecAC==0) name<<"A";
  else          name<<"C";

  return name.str();

}

//________________________________________________________________________
void plotLgSmSummary(TPad* canv, TH1F** hists, const string& canvname)
{
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(111);

  canv->Clear();
  /*
  double max=-999.;
  double min=999.;
  for (int i=0;i<2;i++) {
    for (int j=0;j<16;j++) {
      double val=hists[i]->GetBinContent(j+1);
      double err=hists[i]->GetBinError(j+1);
      if (val>0) val+=err;
      else val-=err;
      if (val>max && val!=0.) max=val;
      if (val<min && val!=0.) min=val;
    }
  }
  if (max>0) max*=1.1; else max*=0.9;
  if (min<0) min*=1.1; else min*=0.9;
  
  for (int i=0;i<2;i++) {
    hists[i]->SetMaximum(max);
    hists[i]->SetMinimum(min);
  }
  */

  hists[0]->SetLineColor(twocolors[0]);
  hists[0]->Draw();

  TCanvas* ctmp=new TCanvas("ctmpLgSmSummary","ctmpLgSmSummary");

  TF1** fits=new TF1*[2];
  TPaveStats** paves=new TPaveStats*[2];
  fits[0]=new TF1("fitLg","pol0",0.,16.);
  fits[1]=new TF1("fitSm","pol0",0.,16.);
  for (int i=0;i<2;i++) {
    hists[i]->SetLineColor(twocolors[i]);
    ctmp->cd();
    hists[i]->Fit(fits[i],"RQ");
    ctmp->Update();
    paves[i]=(TPaveStats*)hists[i]->FindObject("stats");
    if (i==0) {
      paves[i]->SetY1NDC(0.92);
      paves[i]->SetY2NDC(1.0);
    }
    else {
      paves[i]->SetY1NDC(0.84);
      paves[i]->SetY2NDC(0.92);
    }
    paves[i]->SetTextColor(twocolors[i]);
    ctmp->Modified();

    canv->cd();
    fits[i]->SetLineColor(twocolors[i]);
    if (i==0) hists[i]->Draw();
    else      hists[i]->Draw("same");
    fits[i]->Draw("same");
  }
  canv->Update();
  canv->Print(canvname.c_str());
  delete [] fits;
  delete ctmp;

  return;
}
