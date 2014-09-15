/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TH1F.h"
#include "TTree.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TROOT.h"
#include "TLegend.h"

#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <cmath>

using namespace std;
int main() {

  const int NFILES=3;
  int* colors=new int[NFILES];
  colors[0]=kRed;
  colors[1]=kBlue;
  colors[2]=kGreen;

  TFile** f=new TFile*[NFILES];
  f[0]=new TFile("/afs/cern.ch/user/r/roberth/scratch0/L1_1file_noscat_results/MuonAlign_L1_1file_noscat_iter0.root","READ");
  f[1]=new TFile("/afs/cern.ch/user/r/roberth/scratch0/L1_1file_noscat_results/MuonAlign_L1_1file_noscat_iter1.root","READ");
  f[2]=new TFile("/afs/cern.ch/user/r/roberth/scratch0/L1_1file_noscat_results/MuonAlign_L1_1file_noscat_iter2.root","READ");

  
  TTree** trees=new TTree*[NFILES];
  for (int i=0;i<NFILES;i++) {
    trees[i]=(TTree*)f[i]->Get("MuonTrackPreProcessor");
  }

  TH1F** hIDres=new TH1F*[NFILES];
  TH1F** hMSres=new TH1F*[NFILES];
  TH1F** hCalRes=new TH1F*[NFILES];
  TH1F** hIDphiscatres=new TH1F*[NFILES];
  TH1F** hIDthetascatres=new TH1F*[NFILES];
  TH1F** hMSphiscatres=new TH1F*[NFILES];
  TH1F** hMSthetascatres=new TH1F*[NFILES];

  TH1F** hIDpull=new TH1F*[NFILES];
  TH1F** hMSpull=new TH1F*[NFILES];
  TH1F** hCalPull=new TH1F*[NFILES];
  TH1F** hIDphiscatpull=new TH1F*[NFILES];
  TH1F** hIDthetascatpull=new TH1F*[NFILES];
  TH1F** hMSphiscatpull=new TH1F*[NFILES];
  TH1F** hMSthetascatpull=new TH1F*[NFILES];

  TH1F** hIDchi2=new TH1F*[NFILES];
  TH1F** hMSchi2=new TH1F*[NFILES];
  TH1F** hCalChi2=new TH1F*[NFILES];
  TH1F** hIDphiscatchi2=new TH1F*[NFILES];
  TH1F** hIDthetascatchi2=new TH1F*[NFILES];
  TH1F** hMSphiscatchi2=new TH1F*[NFILES];
  TH1F** hMSthetascatchi2=new TH1F*[NFILES];
						
  TH1F** hTrackChi2=new TH1F*[NFILES];
  TH1F** hMyTrackChi2=new TH1F*[NFILES];

  int*     run   =new int[NFILES];
  int*     evt   =new int[NFILES];

  double*  trackChi2=new double[NFILES];
  int*     trackNDoF=new int[NFILES];

  double*  totalTrackChi2=new double[NFILES];
  int   *  totalTrackNDoF=new int[NFILES];

  int*     nIDres=new int[NFILES];
  double** IDres =new double*[NFILES];
  double** IDerr =new double*[NFILES];

  int*     nIDscatRes     =new int[NFILES];
  double** IDscatPhiRes   =new double*[NFILES];
  double** IDscatPhiErr   =new double*[NFILES];
  double** IDscatThetaRes =new double*[NFILES];
  double** IDscatThetaErr =new double*[NFILES];

  int*     nMSres         =new int[NFILES];
  double** MSres          =new double*[NFILES];
  double** MSerr          =new double*[NFILES];
  int**    MSstatLayer    =new int*[NFILES];
  int**    MSisEndcap     =new int*[NFILES];

  int*     nMStrigRes     =new int[NFILES];
  double** MStrigRes      =new double*[NFILES];
  double** MStrigErr      =new double*[NFILES];
  int   ** MStrigStatLayer=new int*[NFILES];
  int   ** MStrigIsEndcap =new int*[NFILES];

  int*     nMSscatRes     =new int[NFILES];
  double** MSscatPhiRes   =new double*[NFILES];
  double** MSscatPhiErr   =new double*[NFILES];
  double** MSscatThetaRes =new double*[NFILES];
  double** MSscatThetaErr =new double*[NFILES];

  int*     nCalRes        =new int[NFILES];
  double** CalRes         =new double*[NFILES];
  double** CalErr         =new double*[NFILES];

  for (int i=0;i<NFILES;i++) {

    totalTrackChi2[i]=0.;
    totalTrackNDoF[i]=0;

    IDres[i]=new double[100]; 
    IDerr[i]=new double[100];
    IDscatPhiRes[i]=new double[100];
    IDscatPhiErr[i]=new double[100];
    IDscatThetaRes[i]=new double[100];
    IDscatThetaErr[i]=new double[100];
    MSres[i]=new double[150];
    MSerr[i]=new double[150];
    MSstatLayer[i]=new int[150];
    MSisEndcap[i]=new int[150];
    MStrigRes[i]=new double[50];
    MStrigErr[i]=new double[50];
    MStrigStatLayer[i]=new int[50];
    MStrigIsEndcap[i]=new int[50];
    MSscatPhiRes[i]=new double[100];
    MSscatPhiErr[i]=new double[100];
    MSscatThetaRes[i]=new double[100];
    MSscatThetaErr[i]=new double[100];
    CalRes[i]=new double[10];
    CalErr[i]=new double[10];

    TTree* t=trees[i];
    t->SetBranchAddress("run",    &run[i]);
    t->SetBranchAddress("evt",    &evt[i]);
						
    t->SetBranchAddress("trackChi2", &trackChi2[i]);
    t->SetBranchAddress("trackNDoF", &trackNDoF[i]);

    t->SetBranchAddress("nIDres", &nIDres[i]);
    t->SetBranchAddress("IDres",   IDres[i]);
    t->SetBranchAddress("IDerr",   IDerr[i]);

    t->SetBranchAddress("nIDscatRes",      &nIDscatRes[i]);
    t->SetBranchAddress("IDscatPhiRes",     IDscatPhiRes[i]);
    t->SetBranchAddress("IDscatPhiErr",     IDscatPhiErr[i]);
    t->SetBranchAddress("IDscatThetaRes",   IDscatThetaRes[i]);
    t->SetBranchAddress("IDscatThetaErr",   IDscatThetaErr[i]);
    
    t->SetBranchAddress("nMSres", &nMSres[i]);
    t->SetBranchAddress("MSres",   MSres[i]);
    t->SetBranchAddress("MSerr",   MSerr[i]);
    t->SetBranchAddress("MSstatLayer",  MSstatLayer[i]);
    t->SetBranchAddress("MSisEndcap",   MSisEndcap[i]);

    t->SetBranchAddress("nMStrigRes",      &nMStrigRes[i]);
    t->SetBranchAddress("MStrigRes",       MStrigRes[i]);
    t->SetBranchAddress("MStrigErr",       MStrigErr[i]);
    t->SetBranchAddress("MStrigStatLayer", MStrigStatLayer[i]);
    t->SetBranchAddress("MStrigIsEndcap",  MStrigIsEndcap[i]);

    t->SetBranchAddress("nMSscatRes",      &nMSscatRes[i]);
    t->SetBranchAddress("MSscatPhiRes",    MSscatPhiRes[i]);
    t->SetBranchAddress("MSscatPhiErr",    MSscatPhiErr[i]);
    t->SetBranchAddress("MSscatThetaRes",  MSscatThetaRes[i]);
    t->SetBranchAddress("MSscatThetaErr",  MSscatThetaErr[i]);

    t->SetBranchAddress("nCalRes", &nCalRes[i]);
    t->SetBranchAddress("CalRes",  CalRes[i]);
    t->SetBranchAddress("CalErr",  CalErr[i]);    

    stringstream ifilestr; ifilestr<<i;

    // ID res
    hIDres[i]=new TH1F(("IDres"+ifilestr.str()).c_str(),
			"ID res",100,-.2,.2);
    hIDres[i]->SetLineColor(colors[i]);
    hIDres[i]->GetXaxis()->SetTitle("residuals for ID hits on combined muon track (mm)");

    // ID pull
    hIDpull[i]=new TH1F(("IDpull"+ifilestr.str()).c_str(),
			"ID pulls",100,-5.,5.);
    hIDpull[i]->SetLineColor(colors[i]);
    hIDpull[i]->GetXaxis()->SetTitle("pulls for ID hits on combined muon track");

    // ID chi2
    hIDchi2[i]=new TH1F(("IDchi2"+ifilestr.str()).c_str(),
			//"ID log(chi2)",100,-4.,2.5);
			"ID log(chi2)",100,0.,300.);
    hIDchi2[i]->SetLineColor(colors[i]);
    hIDchi2[i]->GetXaxis()->SetTitle("log_{10}(#chi^{2}) for ID hits on combined muon track");

    // MS res
    hMSres[i]=new TH1F(("MSres"+ifilestr.str()).c_str(),
		       "MS res",100,-.5,.5);
    hMSres[i]->SetLineColor(colors[i]);
    hMSres[i]->GetXaxis()->SetTitle("residuals for MS hits on combined muon track (mm)");

    // MS pull
    hMSpull[i]=new TH1F(("MSpull"+ifilestr.str()).c_str(),
		       "MS pulls",100,-5.,5.);
    hMSpull[i]->SetLineColor(colors[i]);
    hMSpull[i]->GetXaxis()->SetTitle("pulls for MS hits on combined muon track");

    // MS chi2
    hMSchi2[i]=new TH1F(("MSchi2"+ifilestr.str()).c_str(),
			//"MS log(chi2)",100,-4.,2.5);
			"MS log(chi2)",100,0.,300.);
    hMSchi2[i]->SetLineColor(colors[i]);
    hMSchi2[i]->GetXaxis()->SetTitle("log_{10}(#chi^{2}) for MS hits on combined muon track");

    // cal eloss
    hCalRes[i]=new TH1F(("CalRes"+ifilestr.str()).c_str(),
			"calorimeter energy loss",100,-.5,.5);
    hCalRes[i]->SetLineColor(colors[i]);
    hCalRes[i]->GetXaxis()->SetTitle("calorimeter energy loss for combined muon track");

    // cal pull
    hCalPull[i]=new TH1F(("CalPull"+ifilestr.str()).c_str(),
			"calorimeter pulls",100,-5.,5.);
    hCalPull[i]->SetLineColor(colors[i]);
    hCalPull[i]->GetXaxis()->SetTitle("calorimeter energy loss/expected energy loss for combined muon track");

    // cal chi2
    hCalChi2[i]=new TH1F(("CalChi2"+ifilestr.str()).c_str(),
			 //"Cal log(chi2)",100,-4.,2.5);
			 "Cal log(chi2)",100,0.,300.);
    hCalChi2[i]->SetLineColor(colors[i]);
    hCalChi2[i]->GetXaxis()->SetTitle("log_{10}(#chi^{2}) for cal. energy loss on combined muon track");

    // ID phi scat res
    hIDphiscatres[i]=new TH1F(("IDphiscatres"+ifilestr.str()).c_str(),
			       "ID scat. phi res",100,-.005,.005);
    hIDphiscatres[i]->SetLineColor(colors[i]);
    hIDphiscatres[i]->GetXaxis()->SetTitle("residuals for scat. phi on combined muon track in ID (mrad)");

    // ID phi scat pull
    hIDphiscatpull[i]=new TH1F(("IDphiscatpull"+ifilestr.str()).c_str(),
			       "ID scat. phi pulls",100,-.00001,.00001);
    hIDphiscatpull[i]->SetLineColor(colors[i]);
    hIDphiscatpull[i]->GetXaxis()->SetTitle("pulls for scat. phi on combined muon track in ID");

    // ID phi scat chi2
    hIDphiscatchi2[i]=new TH1F(("IDphiscatchi2"+ifilestr.str()).c_str(),
			       "ID log(scat.phi chi2)",100,-4.,2.5);
    hIDphiscatchi2[i]->SetLineColor(colors[i]);
    hIDphiscatchi2[i]->GetXaxis()->SetTitle("log_{10}(#chi^{2}) from scat. phi on combined muon track in ID (mrad)");


    // ID theta scat res
    hIDthetascatres[i]=new TH1F(("IDthetascatres"+ifilestr.str()).c_str(),
			       "ID scat. theta res",100,-.005,.005);
    hIDthetascatres[i]->SetLineColor(colors[i]);
    hIDthetascatres[i]->GetXaxis()->SetTitle("residuals for scat. theta on combined muon track in ID");

    // ID theta scat pull
    hIDthetascatpull[i]=new TH1F(("IDthetascatpull"+ifilestr.str()).c_str(),
			       "ID scat. theta pulls",100,-.00001,.00001);
    hIDthetascatpull[i]->SetLineColor(colors[i]);
    hIDthetascatpull[i]->GetXaxis()->SetTitle("pulls for scat. theta on combined muon track in ID (mrad)");

    // ID theta scat chi2
    hIDthetascatchi2[i]=new TH1F(("IDthetascatchi2"+ifilestr.str()).c_str(),
			       "ID log (scat.theta chi2)",100,-4.,2.5);
    hIDthetascatchi2[i]->SetLineColor(colors[i]);
    hIDthetascatchi2[i]->GetXaxis()->SetTitle("log_{10}(#chi^{2}) from scat. theta on combined muon track in ID");

    // MS phi scat res
    hMSphiscatres[i]=new TH1F(("MSphiscatres"+ifilestr.str()).c_str(),
			       "MS scat. phi res",100,-.005,.005);
    hMSphiscatres[i]->SetLineColor(colors[i]);
    hMSphiscatres[i]->GetXaxis()->SetTitle("residuals for scat. phi on combined muon track in MS (mrad)");

    // MS phi scat pull
    hMSphiscatpull[i]=new TH1F(("MSphiscatpull"+ifilestr.str()).c_str(),
			       "MS scat. phi pulls",100,-.00001,.00001);
    hMSphiscatpull[i]->SetLineColor(colors[i]);
    hMSphiscatpull[i]->GetXaxis()->SetTitle("pulls for scat. phi on combined muon track in MS");

    // MS phi scat chi2
    hMSphiscatchi2[i]=new TH1F(("MSphiscatchi2"+ifilestr.str()).c_str(),
			       "MS log(scat.phi chi2)",100,-4.,2.5);
    hMSphiscatchi2[i]->SetLineColor(colors[i]);
    hMSphiscatchi2[i]->GetXaxis()->SetTitle("log_{10}(#chi^{2}) from scat. phi on combined muon track in MS");

    // MS theta scat res
    hMSthetascatres[i]=new TH1F(("MSthetascatres"+ifilestr.str()).c_str(),
			       "MS scat. theta res",100,-.02,.02);
    hMSthetascatres[i]->SetLineColor(colors[i]);
    hMSthetascatres[i]->GetXaxis()->SetTitle("residuals for scat. theta on combined muon track in MS (mrad)");

    // MS theta scat pull
    hMSthetascatpull[i]=new TH1F(("MSthetascatpull"+ifilestr.str()).c_str(),
			       "MS scat. theta pulls",100,-.001,.001);
    hMSthetascatpull[i]->SetLineColor(colors[i]);
    hMSthetascatpull[i]->GetXaxis()->SetTitle("pulls for scat. theta on combined muon track in MS");

    // MS theta scat chi2
    hMSthetascatchi2[i]=new TH1F(("MSthetascatchi2"+ifilestr.str()).c_str(),
			       "MS log(scat.theta chi2)",100,-4.,2.5);
    hMSthetascatchi2[i]->SetLineColor(colors[i]);
    hMSthetascatchi2[i]->GetXaxis()->SetTitle("log_{10}(#chi^{2}) from scat. theta on combined muon track in MS");

    hTrackChi2[i]=new TH1F(("trackChi2"+ifilestr.str()).c_str(),
			   //"ID log(trackChi2)",100,0.,2.5);
			   "ID log(trackChi2)",100,0.,300.);
    hTrackChi2[i]->SetLineColor(colors[i]);
    hTrackChi2[i]->GetXaxis()->SetTitle("log_{10}(#chi^{2}) for combined muon track");
    
    hMyTrackChi2[i]=new TH1F(("myTrackChi2"+ifilestr.str()).c_str(),
			     "ID log(myTrackChi2)",100,0.,2.5);
    hMyTrackChi2[i]->SetLineColor(colors[i]);
    hMyTrackChi2[i]->GetXaxis()->SetTitle("log_{10}(my #chi^{2}) for combined muon track");
    
    
  }

  TLegend* leg=new TLegend(.78,.70,.98,.83);
  for (int i=0;i<NFILES;i++)  {
    stringstream iterstr; iterstr<<"iter "<<i;
    leg->AddEntry(hIDpull[i],iterstr.str().c_str(),"l");
  }

  int* lastgoodentry=new int[NFILES];
  for (int i=0;i<NFILES;i++) lastgoodentry[i]=0;

  for (int i=0;i<NFILES;i++) {
    cout<<"tree "<<i<<" has "<<trees[i]->GetEntries()<<endl;
  }

  for (int ientry0=0;ientry0<trees[0]->GetEntries();ientry0++) {

    //cout<<"ientry0: "<<ientry0<<endl;

    trees[0]->GetEntry(ientry0);

    //cout<<endl;
    //cout<<"run0 "<<run[0]<<"/evt "<<evt[0]<<endl;

    double idChi20(0.),msChi20(0.),calChi20(0.),idphiscatChi20(0.),idthetascatChi20(0.),msphiscatChi20(0.),msthetascatChi20(0.);
      

    int nidres0=nIDres[0];
    int nmsres0=nMSres[0];
    int ncalres0=nCalRes[0];
    int nidscatres0=nIDscatRes[0];
    int nmsscatres0=nMSscatRes[0];

    double* idres0 =new double[nidres0];
    double* msres0 =new double[nmsres0];
    double* calres0=new double[ncalres0];
    double* idphiscatres0=new double[nidscatres0];
    double* idthetascatres0=new double[nidscatres0];
    double* msphiscatres0=new double[nmsscatres0];
    double* msthetascatres0=new double[nmsscatres0];

    double* idpull0 =new double[nidres0];
    double* mspull0 =new double[nmsres0];
    double* calpull0=new double[ncalres0];
    double* idphiscatpull0=new double[nidscatres0];
    double* idthetascatpull0=new double[nidscatres0];
    double* msphiscatpull0=new double[nmsscatres0];
    double* msthetascatpull0=new double[nmsscatres0];

    for (int i=0;i<nIDres[0];i++) {
      double pull=IDres[0][i]; 
      idres0[i]=pull;
      hIDres[0]->Fill(pull);
      pull/=IDerr[0][i];       
      idpull0[i]=pull;
      idChi20 += pull*pull;
      hIDpull[0]->Fill(pull);
    }
    for (int i=0;i<nMSres[0];i++) {
      double pull=MSres[0][i];
      msres0[i]=pull;
      hMSres[0]->Fill(pull);
      pull/=MSerr[0][i];
      mspull0[i]=pull;
      msChi20 +=pull*pull;
      hMSpull[0]->Fill(pull);
    }
    for (int i=0;i<nCalRes[0];i++) {
      double pull=CalRes[0][i];
      calres0[i]=pull;
      hCalRes[0]->Fill(pull);
      pull/=CalErr[0][i];
      calpull0[i]=pull;
      calChi20+=pull*pull;
      hCalPull[0]->Fill(pull);
    }
    for (int i=0;i<nIDscatRes[0];i++) {
      double pull=IDscatPhiRes[0][i];
      idphiscatres0[i]=pull;
      hIDphiscatres[0]->Fill(1000.*pull);
      pull/=IDscatPhiErr[0][i];
      idphiscatpull0[i]=pull;
      idphiscatChi20 += pull*pull;
      hIDphiscatpull[0]->Fill(pull);

      pull=IDscatThetaRes[0][i];
      idthetascatres0[i]=pull;
      hIDthetascatres[0]->Fill(1000.*pull);
      pull/=IDscatThetaErr[0][i];
      idthetascatpull0[i]=pull;
      idthetascatChi20 += pull*pull;
      hIDthetascatpull[0]->Fill(pull);
    }
    for (int i=0;i<nMSscatRes[0];i++) {
      double pull=MSscatPhiRes[0][i];
      msphiscatres0[i]=pull;
      hMSphiscatres[0]->Fill(1000.*pull);
      pull/=MSscatPhiErr[0][i];
      msphiscatpull0[i]=pull;
      msphiscatChi20 += pull*pull;
      hMSphiscatpull[0]->Fill(pull);

      pull=MSscatThetaRes[0][i];
      msthetascatres0[i]=pull;
      hMSthetascatres[0]->Fill(1000.*pull);
      pull/=MSscatThetaErr[0][i];
      msthetascatpull0[i]=pull;
      msthetascatChi20 += pull*pull;
      hMSthetascatpull[0]->Fill(pull);
    }
    
    //hIDchi2[0]->Fill(std::log10(idChi20));//(double)nidres0));
    //hMSchi2[0]->Fill(std::log10(msChi20));///(double)nmsres0));
    //hCalChi2[0]->Fill(std::log10(calChi20));///(double)ncalres0));

    hIDchi2[0]->Fill(idChi20);//(double)nidres0));
    hMSchi2[0]->Fill(msChi20);///(double)nmsres0));
    hCalChi2[0]->Fill(calChi20);///(double)ncalres0));

    hIDphiscatchi2[0]->Fill(std::log10(idphiscatChi20));///(double)nidscatres0));
    hIDthetascatchi2[0]->Fill(std::log10(idthetascatChi20));///(double)nidscatres0));
    hMSphiscatchi2[0]->Fill(std::log10(msphiscatChi20));///(double)nmsscatres0));
    hMSthetascatchi2[0]->Fill(std::log10(msthetascatChi20));///(double)nmsscatres0));
						
    //hTrackChi2[0]->Fill(std::log10(trackChi2[0]*(double)trackNDoF[0]));
    hTrackChi2[0]->Fill(trackChi2[0]*(double)trackNDoF[0]);
    double myChi2=idChi20+msChi20+calChi20+
      idphiscatChi20+idthetascatChi20+
      msphiscatChi20+msthetascatChi20;
    hMyTrackChi2[0]->Fill(std::log10(myChi2));
    //cout<<"delta chi2(0): "<<trackChi2[0]*(double)trackNDoF[0]-myChi2<<endl;
    totalTrackChi2[0]+=trackChi2[0];
    totalTrackNDoF[0]+=trackNDoF[0];

    for (int ifile=1;ifile<NFILES;ifile++) {
      
      for (int ientry=lastgoodentry[ifile];ientry<trees[ifile]->GetEntries();ientry++) {

	trees[ifile]->GetEntry(ientry);
	
	// check matching run/evt
	if (run[ifile]!=run[0] || evt[ifile]!=evt[0]) {
	  continue;
	}
	lastgoodentry[ifile]=ientry;

	// get residuals and add chi2
	double idChi2(0.),msChi2(0.),calChi2(0.),
	  idphiscatChi2(0.),msphiscatChi2(0.),
	  idthetascatChi2(0.),msthetascatChi2(0.);	  
	
	for (int i=0;i<nIDres[ifile];i++) {
	  double pull=IDres[ifile][i];
	  hIDres[ifile]->Fill(pull);	  
	  pull/=IDerr[ifile][i];
	  idChi2 += pull*pull;
	  hIDpull[ifile]->Fill(pull);
	}
	for (int i=0;i<nMSres[ifile];i++) {
	  double pull=MSres[ifile][i];
	  hMSres[ifile]->Fill(pull);
	  pull/=MSerr[ifile][i];
	  msChi2 += pull*pull;
	  hMSpull[ifile]->Fill(pull);
	}
	for (int i=0;i<nCalRes[ifile];i++) {
	  double pull=CalRes[ifile][i];
	  hCalRes[ifile]->Fill(pull);
	  pull/=CalErr[ifile][i];
	  calChi2+= pull*pull;
	  hCalPull[ifile]->Fill(pull);
	}
	for (int i=0;i<nIDscatRes[ifile];i++) {
	  double pull=IDscatPhiRes[ifile][i];
	  hIDphiscatres[ifile]->Fill(1000.*pull);
	  pull/=IDscatPhiErr[ifile][i];
	  idphiscatChi2 += pull*pull;
	  hIDphiscatpull[ifile]->Fill(pull);
	  
	  pull=IDscatThetaRes[ifile][i];
	  hIDthetascatres[ifile]->Fill(1000.*pull);
	  pull/=IDscatThetaErr[ifile][i];
	  idthetascatChi2 += pull*pull;
	  hIDthetascatpull[ifile]->Fill(pull);
	}
	for (int i=0;i<nMSscatRes[ifile];i++) {
	  double pull=MSscatPhiRes[ifile][i];
	  hMSphiscatres[ifile]->Fill(1000.*pull);
	  pull/=MSscatPhiErr[ifile][i];
	  msphiscatChi2 += pull*pull;
	  hMSphiscatpull[ifile]->Fill(pull);
	  
	  pull=MSscatThetaRes[ifile][i];
	  hMSthetascatres[ifile]->Fill(1000.*pull);
	  pull/=MSscatThetaErr[ifile][i];
	  msthetascatChi2 += pull*pull;
	  hMSthetascatpull[ifile]->Fill(pull);
	}
	
	//hIDchi2[ifile]->Fill(std::log10(idChi2));///(double)nIDres[ifile]));
	//hMSchi2[ifile]->Fill(std::log10(msChi2));///(double)nMSres[ifile]));
	//hCalChi2[ifile]->Fill(std::log10(calChi2));///(double)nCalRes[ifile]));

	hIDchi2[ifile]->Fill(idChi2);///(double)nIDres[ifile]));
	hMSchi2[ifile]->Fill(msChi2);///(double)nMSres[ifile]));
	hCalChi2[ifile]->Fill(calChi2);///(double)nCalRes[ifile]));

	hIDphiscatchi2[ifile]->Fill(std::log10(idphiscatChi2));///(double)nIDscatRes[ifile]));
	hIDthetascatchi2[ifile]->Fill(std::log10(idthetascatChi2));///(double)nIDscatRes[ifile]));
	hMSphiscatchi2[ifile]->Fill(std::log10(msphiscatChi2));///(double)nMSscatRes[ifile]));
	hMSthetascatchi2[ifile]->Fill(std::log10(msthetascatChi2));///(double)nMSscatRes[ifile]));

	//hTrackChi2[ifile]->Fill(std::log10(trackChi2[ifile]*(double)trackNDoF[ifile]));
	hTrackChi2[ifile]->Fill(trackChi2[ifile]*(double)trackNDoF[ifile]);

	double myChi2=idChi2+msChi2+calChi2+
	  idphiscatChi2+idthetascatChi2+
	  msphiscatChi2+msthetascatChi2;
	hMyTrackChi2[ifile]->Fill(std::log10(myChi2));
									
	totalTrackChi2[ifile]+=trackChi2[ifile];
	totalTrackNDoF[ifile]+=trackNDoF[ifile];

	//cout<<"delta chi2: "<<trackChi2[ifile]*(double)trackNDoF[ifile]-myChi2<<endl;

	break;
      }
    }
    delete [] idthetascatpull0;
    delete [] msthetascatpull0;
    delete [] idphiscatpull0;
    delete [] msphiscatpull0;
    delete [] calpull0;
    delete [] idpull0;
    delete [] mspull0;
    delete [] idthetascatres0;
    delete [] msthetascatres0;
    delete [] idphiscatres0;
    delete [] msphiscatres0;
    delete [] calres0;
    delete [] idres0;
    delete [] msres0;

  }

  for (int i=0;i<NFILES;i++) std::cout<<"total track chi2 ["<<i<<"]="<<totalTrackChi2[i]/(double)totalTrackNDoF[i]<<endl;

  //gStyle->SetOptStat("111111");

  TCanvas* canv=new TCanvas("canv","canv");
  canv->SetFillColor(10);
  
  const int NHISTS=21;
  TH1F** hists[NHISTS]={hIDchi2,          hIDres,         hIDpull,         
			hIDphiscatchi2,   hIDphiscatres,  hIDphiscatpull,
			hIDthetascatchi2, hIDthetascatres,hIDthetascatpull,
			hCalChi2,         hCalRes,        hCalPull,
			hMSchi2,          hMSres,         hMSpull,
			hMSphiscatchi2,   hMSphiscatres,  hMSphiscatpull,
			hMSthetascatchi2, hMSthetascatres,hMSthetascatpull};

  
  // ID measurement chi2
  int ipad=0;
  canv->Divide(3,4);

  for (int ihist=0;ihist<NHISTS;ihist++) {
    
    canv->cd(++ipad);
    double max(0.);
    for (int ifile=0;ifile<NFILES;ifile++) {
      double thismax=hists[ihist][ifile]->GetMaximum();
      if (thismax>max) max=thismax;
    }
    hists[ihist][0]->SetMaximum(max*1.2);
    hists[ihist][0]->Draw();
    for (int ifile=1;ifile<NFILES;ifile++) hists[ihist][ifile]->Draw("same");
    leg->Draw();

    if (ihist==11) {
      canv->Print("level1hists_ID.eps");
      ipad=0;
      canv->Clear();
      canv->Divide(3,4);
    }
  }

  canv->cd(++ipad);
  double max=0.;
  for (int ifile=0;ifile<NFILES;ifile++) {
    double thismax=hTrackChi2[ifile]->GetMaximum();
    if (thismax>max) max=thismax;
  }
  hTrackChi2[0]->SetMaximum(max*1.2);
  hTrackChi2[0]->Draw();
  for (int ifile=1;ifile<NFILES;ifile++) hTrackChi2[ifile]->Draw("same");
  leg->Draw();
						
  /*
  canv->cd(++ipad);
  max=0.;
  for (int ifile=0;ifile<NFILES;ifile++) {
    double thismax=hMyTrackChi2[ifile]->GetMaximum();
    if (thismax>max) max=thismax;
  }
  hMyTrackChi2[0]->SetMaximum(max*1.2);
  hMyTrackChi2[0]->Draw();
  for (int ifile=1;ifile<NFILES;ifile++) hMyTrackChi2[ifile]->Draw("same");
  leg->Draw();
  */
  
  canv->Print("level1hists_MS.eps");

  delete canv;

  delete [] lastgoodentry;
}
