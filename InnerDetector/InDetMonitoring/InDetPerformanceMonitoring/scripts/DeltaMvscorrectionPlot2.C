#include "atlasstyle/AtlasStyle.C"
#include "TFile.h"
#include "TH2F.h"
#include "TMath.h"
#include "TGraphErrors.h"
#include "TGraph.h"
#include "TLine.h"
#include "TSpline.h"
#include "TCanvas.h"
#include "TLegend.h"
#include <TROOT.h>
#include <iostream>

#include "PlotEvolution2.h"
#include "PlotEvolution2.C"


void DeltaMvscorrectionPlot2()
{ 
  
  SetAtlasStyle(); 
  //I use this option with macros of AtlasStyle.C which I have also in the scripts directory.
  //one can copy them fom my public /afs/cern.ch/work/o/oestrada/public/alignment2/Jpsi_20.7.3.8/20.7.3.8/InnerDetector/InDetMonitoring/InDetPerformanceMonitoring/scripts/atlasstyle

  const int nfiles=1; //choose nfiles = 1 if one wants to study corrections with epsilon = 0; for studying artificial radial distortions nfiles = 5

/* char fname[nfiles][10000]={"/afs/cern.ch/work/o/oestrada/public/alignment2/Jpsi_20.7.3.8/20.7.3.8/InnerDetector/InDetMonitoring/InDetPerformanceMonitoring/scripts/ZmumuValidationExampleOutput.Data.stableperiodPeriodE.newmodelepsilonm002_6_eta_3_phiRefit2Params.root",
                             "/afs/cern.ch/work/o/oestrada/public/alignment2/Jpsi_20.7.3.8/20.7.3.8/InnerDetector/InDetMonitoring/InDetPerformanceMonitoring/scripts/ZmumuValidationExampleOutput.Data.stableperiodPeriodE.newmodelepsilonm001_6_eta_3_phiRefit2Params.root",
                             "/afs/cern.ch/work/o/oestrada/public/alignment2/Jpsi_20.7.3.8/20.7.3.8/InnerDetector/InDetMonitoring/InDetPerformanceMonitoring/scripts/ZmumuValidationExampleOutput.Data.stableperiodPeriodE.newmodelepsilon0_6_eta_3_phiRefit2Params.root",
                             "/afs/cern.ch/work/o/oestrada/public/alignment2/Jpsi_20.7.3.8/20.7.3.8/InnerDetector/InDetMonitoring/InDetPerformanceMonitoring/scripts/ZmumuValidationExampleOutput.Data.stableperiodPeriodE.newmodelepsilonp001_6_eta_3_phiRefit2Params.root",
                             "/afs/cern.ch/work/o/oestrada/public/alignment2/Jpsi_20.7.3.8/20.7.3.8/InnerDetector/InDetMonitoring/InDetPerformanceMonitoring/scripts/ZmumuValidationExampleOutput.Data.stableperiodPeriodE.newmodelepsilonp002_6_eta_3_phiRefit2Params.root "};*/

//char fname[nfiles][10000] = {"ZmumuValidationExampleOutput.Data.stableperiodPeriodE.newmodelv2epsilon0_3_eta_6_phi_30iterRefit2Params.root"};
//char fname[nfiles][10000] = {"ZmumuValidationExampleOutput.Data.stableperiodPeriodE.newmodelv3epsilon0_3_eta_6_phi_30iterRefit2Params.root"};
//char fname[nfiles][10000] = {"ZmumuValidationExampleOutput.Data.stableperiodPeriodE.PROVAIFICSINFORMULALnewmodelv2epsilon0_3_eta_6_phi_30iter_mcbiasRefit1Params.root"};
char fname[nfiles][10000] = {"ZmumuValidationExampleOutput.Data.stableperiodPeriodE.ROOTerrorstest2Refit2Params.root"};

/*char fname[nfiles][10000]={"/afs/cern.ch/work/o/oestrada/public/alignment2/Jpsi_20.7.3.8/20.7.3.8/InnerDetector/InDetMonitoring/InDetPerformanceMonitoring/scripts/ZmumuValidationExampleOutput.Data.stableperiodPeriodE.PROVAIFICSINFORMULALnewmodelv2epsilonm002_3_eta_1_phi_30iter_mcbiasRefit1Params.root",
                             "/afs/cern.ch/work/o/oestrada/public/alignment2/Jpsi_20.7.3.8/20.7.3.8/InnerDetector/InDetMonitoring/InDetPerformanceMonitoring/scripts/ZmumuValidationExampleOutput.Data.stableperiodPeriodE.PROVAIFICSINFORMULALnewmodelv2epsilonm001_3_eta_1_phi_30iter_mcbiasRefit1Params.root",
                             "/afs/cern.ch/work/o/oestrada/public/alignment2/Jpsi_20.7.3.8/20.7.3.8/InnerDetector/InDetMonitoring/InDetPerformanceMonitoring/scripts/ZmumuValidationExampleOutput.Data.stableperiodPeriodE.PROVAIFICSINFORMULALnewmodelv2epsilon0_3_eta_1_phi_30iter_mcbiasRefit1Params.root",
                             "/afs/cern.ch/work/o/oestrada/public/alignment2/Jpsi_20.7.3.8/20.7.3.8/InnerDetector/InDetMonitoring/InDetPerformanceMonitoring/scripts/ZmumuValidationExampleOutput.Data.stableperiodPeriodE.PROVAIFICSINFORMULALnewmodelv2epsilonp001_3_eta_1_phi_30iter_mcbiasRefit1Params.root",
                             "/afs/cern.ch/work/o/oestrada/public/alignment2/Jpsi_20.7.3.8/20.7.3.8/InnerDetector/InDetMonitoring/InDetPerformanceMonitoring/scripts/ZmumuValidationExampleOutput.Data.stableperiodPeriodE.PROVAIFICSINFORMULALnewmodelv2epsilonp002_3_eta_1_phi_30iter_mcbiasRefit1Params.root "};*/
  

 /*char fname[nfiles][10000]={"/afs/cern.ch/work/o/oestrada/public/alignment2/Jpsi_20.7.3.8/20.7.3.8/InnerDetector/InDetMonitoring/InDetPerformanceMonitoring/scripts/ZmumuValidationExampleOutput.MC.stableperiodPeriodE.ROOTepsilonm002Refit2Params.root",
                             "/afs/cern.ch/work/o/oestrada/public/alignment2/Jpsi_20.7.3.8/20.7.3.8/InnerDetector/InDetMonitoring/InDetPerformanceMonitoring/scripts/ZmumuValidationExampleOutput.MC.stableperiodPeriodE.ROOTepsilonm001Refit2Params.root",
                             "/afs/cern.ch/work/o/oestrada/public/alignment2/Jpsi_20.7.3.8/20.7.3.8/InnerDetector/InDetMonitoring/InDetPerformanceMonitoring/scripts/ZmumuValidationExampleOutput.MC.stableperiodPeriodE.ROOTepsilon0Refit2Params.root",
                             "/afs/cern.ch/work/o/oestrada/public/alignment2/Jpsi_20.7.3.8/20.7.3.8/InnerDetector/InDetMonitoring/InDetPerformanceMonitoring/scripts/ZmumuValidationExampleOutput.MC.stableperiodPeriodE.ROOTepsilonp001Refit2Params.root",
                             "/afs/cern.ch/work/o/oestrada/public/alignment2/Jpsi_20.7.3.8/20.7.3.8/InnerDetector/InDetMonitoring/InDetPerformanceMonitoring/scripts/ZmumuValidationExampleOutput.MC.stableperiodPeriodE.ROOTepsilonp002Refit2Params.root"};*/
  
  TFile *f[nfiles];
 
  const char *IdHisDeltaM = "h_dimuon_mass_vs_etaphi_fitresultVsPDG";
  const char *IdHisDeltaM_Iteration1 = "Iteration1/h_dimuon_mass_vs_etaphi_fitresultVsPDG";
  const char *IdHisEpsilon = "h_radialdistortion_vs_etaphi_Integrated";
 
  TH2F* hDeltaM[nfiles];
  TH2F* hDeltaM_Iteration1[nfiles];
  TH2F* hEpsilon[nfiles];

 
  std::vector<double> v_deltaM;
  std::vector<double> v_deltaM_Iteration1;
  std::vector<double> v_Epsilon;

 
  //double x[nfiles]  = {-0.002,-0.001,0.,0.001,0.002}; // for artificial radial distortions
  double x[nfiles] = {0.};
  double y[nfiles] = {0};
  double y_Iteration1[nfiles] = {0};
  double y_Epsilon[nfiles] = {0};
  double Y[nfiles] = {0}; 
  double Y_Iteration1[nfiles] = {0};

  fit_results thisfit[nfiles];
  TH2F* h_deltaM[nfiles];
  TH2F* h_epsilon[nfiles];

  TH1F* h_deltaM_barrel[nfiles];
  TH1F* h_epsilon_barrel[nfiles];

  int max_etabin = 3;
  int max_phibin = 6;
  
  TH2F* hc_deltaM[nfiles];
  TH2F* hc_epsilon[nfiles];
   
   
  for (int l=0; l<nfiles; ++l) {
    h_deltaM[l] = new TH2F(Form("h_deltaM%d",l),"#DeltaM; #eta; #phi", max_etabin, -2.5, 2.5, max_phibin, 0., 2*3.14159);
    h_epsilon[l] = new TH2F(Form("h_epsilon%d",l),"#Epsilon; #eta; #phi", max_etabin, -2.5, 2.5, max_phibin, 0., 2*3.14159);

    h_deltaM_barrel[l] = new TH1F(Form("h_deltaM_barrel%d",l),"#DeltaM; #phi; #DeltaM[MeV]", max_phibin, 0.+0.5, max_phibin+0.5);
    h_epsilon_barrel[l] = new TH1F(Form("h_epsilon_barrel%d",l),"#Epsilon; #phi; #epsilon", max_phibin, 0.+0.5, max_phibin+0.5);

    printf("%d =>  %s\n",l,fname[l]);
    f[l] = new TFile(fname[l],"READ");
    f[l]->cd();
    for (int etabin=1; etabin <= max_etabin; etabin++) {
      for (int phibin=1; phibin <= max_phibin; phibin++) {
	thisfit[l] = PlotEvolution2(etabin, phibin, f[l]);
   
        std::cout << " etabin  = " << etabin << "  phibin = " << phibin  << std::endl; 
	std::cout << " deltaM  = " << thisfit[l].deltaM_val << " +- " << thisfit[l].deltaM_err << " MeV" << std::endl;
	std::cout << " epsilon = " << thisfit[l].epsilon_val << " +- " << thisfit[l].epsilon_err << std::endl << std::endl;

        h_deltaM[l]->SetBinContent( etabin, phibin, thisfit[l].deltaM_val);
	h_deltaM[l]->SetBinError(etabin, phibin, thisfit[l].deltaM_err);
	hc_deltaM[l] = (TH2F*) h_deltaM[l]->Clone();

	h_epsilon[l]->SetBinContent( etabin, phibin, thisfit[l].epsilon_val);
        h_epsilon[l]->SetBinError( etabin, phibin, thisfit[l].epsilon_err);
        h_epsilon[l]->GetYaxis()->SetLimits(1,max_phibin);
	hc_epsilon[l] = (TH2F*) h_epsilon[l]->Clone(); 
        
        if (etabin == 2) {
          h_deltaM_barrel[l]->SetBinContent(phibin, thisfit[l].deltaM_val);
          h_deltaM_barrel[l]->SetBinError( phibin, thisfit[l].deltaM_err);
          h_epsilon_barrel[l]->SetBinContent(phibin, thisfit[l].epsilon_val);
          h_epsilon_barrel[l]->SetBinError( phibin, thisfit[l].epsilon_err);
        }
      }
    }

  TCanvas *cEpsilonMap = new TCanvas("cEpsilonMap"," ",1200,980);
  gStyle->SetPalette(1);
  h_epsilon[l]->Draw("COLZ TEXT");
  cEpsilonMap->Print("EpsilonMap.png"); //depending on the needs, avoid printing figures makes faster to run the program

  TCanvas *cDeltaMMap = new TCanvas("cDeltaMMap"," ",1200,980);
  gStyle->SetPalette(1);
  h_deltaM[l]->Draw("COLZ TEXT");
  cEpsilonMap->Print("DeltaMMap.png");


  TCanvas *cDeltaMbarrel = new TCanvas("cDeltaMbarrel"," ",1200,980);
  h_deltaM_barrel[l]->SetMarkerStyle(20);
  double number = 5.0;
  h_deltaM_barrel[l]->SetMaximum(number);
  h_deltaM_barrel[l]->SetMinimum(-number);
  gPad->SetGridx(1);
  gPad->SetGridy(1);
  h_deltaM_barrel[l]->DrawCopy("EP");
  cDeltaMbarrel->Print("DeltaMbarrel.png");
                       
  TCanvas *cEpsilonbarrel = new TCanvas("cEpsilonbarrel"," ",1200,980);
  h_epsilon_barrel[l]->SetMarkerStyle(20);
  double numbere = 0.002;
  h_epsilon_barrel[l]->SetMaximum(numbere);
  h_epsilon_barrel[l]->SetMinimum(-numbere);
  gPad->SetGridx(1);
  gPad->SetGridy(1);
  h_epsilon_barrel[l]->DrawCopy("EP");
  cEpsilonbarrel->Print("Epsilonbarrel.png");

  }

  bool debug = true;
 
  for (int l=0; l<nfiles; ++l) {
    printf("%d =>  %s\n",l,fname[l]);
    f[l] = new TFile(fname[l],"READ");
    f[l]->cd();
    hDeltaM[l] = (TH2F*) gDirectory->Get(IdHisDeltaM);
    hDeltaM_Iteration1[l]= (TH2F*) gDirectory->Get(IdHisDeltaM_Iteration1);
    hEpsilon[l] = (TH2F*) gDirectory->Get(IdHisEpsilon);
 
    }

  double DeltaM = 0.;
  double DeltaM_Iteration1 = 0.;
  double Epsilon = 0.;

  if (debug == true) cout << "------------------  Total Eta Bins:" << hDeltaM[0]->GetNbinsX() << endl;
  if (debug == true) cout << "------------------  Total Phi Bins:" << hDeltaM[0]->GetNbinsY() << endl;
 
  int totaletabins = hDeltaM[0]->GetNbinsX();
  int totalphibins = hDeltaM[0]->GetNbinsY();

  //**********The following loop is useful in order to debug the values of every eta and phi bin for every file**************************
 
  for (int l=0; l<nfiles; ++l) { 
    for (int etabin=1; etabin<= hDeltaM[l]->GetNbinsX() ; etabin++) {
      for (int phibin=1; phibin<= hDeltaM[l]->GetNbinsY() ; phibin++) {
       
	DeltaM = hDeltaM[l]->GetBinContent(etabin,phibin);
	DeltaM_Iteration1 = hDeltaM_Iteration1[l]->GetBinContent(etabin,phibin);
	Epsilon = hEpsilon[l]->GetBinContent(etabin, phibin);
      
	if (debug == true) cout << "------------------  Current Eta Bin:" << etabin << endl;
	if (debug == true) cout << "------------------  Current Phi Bin:" << phibin << endl;
        
	printf("%d =>  Delta M 1st Iteration %f\n",l,DeltaM_Iteration1);
	printf("%d =>  Delta M last Iteration %f\n",l,DeltaM);
	printf("%d =>  Epsilon last Iteration %f\n",l,Epsilon);
       
      }
    } 
  }
 
 
  //****************here test of artificial radial distortions graphs using the histograms as matrices*******************************
 
  for (int etabin=1; etabin<= totaletabins ; etabin++) {
    for (int phibin=1; phibin<= totalphibins ; phibin++) { 

      TCanvas *cDeltaM = new TCanvas("cDeltaM"," ",950,900);

      for (int l=0; l<nfiles; ++l) { 
	y[l] = hc_deltaM[l]->GetBinContent(etabin,phibin);
	Y[l] =  hDeltaM[l]->GetBinContent(etabin,phibin);
	Y_Iteration1[l] =  hDeltaM_Iteration1[l]->GetBinContent(etabin,phibin);
   
	if ( sizeof(Y)/sizeof( Y[l] ) == nfiles ) {
	  if ( debug == true ) printf("Region of **** BIN ETA %d *** PHI BIN %d *** of FILE %d => Value of corrected DeltaM: %f \n",etabin,phibin,l,y[l]);

	  TGraph *graphfin = new TGraph(nfiles, x, y);
	  TGraph *graphini = new TGraph(nfiles, x, Y_Iteration1);
     
	  gPad->SetGridx(1);
	  gPad->SetGridy(1);
	  graphfin->GetXaxis()->SetTitle("#epsilon_{input}");
	  graphfin->GetYaxis()->SetTitle("M_{fit} - M_{PDG} (MeV)");
	  graphfin->SetMarkerStyle(20); //filled circle
	  graphfin->SetMarkerSize(1.5);
	  if( etabin == 1) graphfin->SetMarkerColor(kBlue);
	  if( etabin == 2) graphfin->SetMarkerColor(kGreen);
	  if( etabin == 3) graphfin->SetMarkerColor(kRed);
          //if( phibin == 1) graphfin->SetMarkerColor(kBlue);
	  //if( phibin == 2) graphfin->SetMarkerColor(kGreen);
	  //if( phibin == 3) graphfin->SetMarkerColor(kRed);
	  graphfin->SetMaximum(20.0);// was 20
	  graphfin->SetMinimum(-20.0); // was -20
	  graphfin->Draw("AP"); 

	  graphini->GetXaxis()->SetTitle("#epsilon_{input}");
	  graphini->GetYaxis()->SetTitle("M_{fit} - M_{PDG} (MeV)");
	  graphini->SetMarkerStyle(24); //empty circle
	  graphini->SetMarkerSize(1.5);
          if( etabin == 1) graphfin->SetMarkerColor(kBlue);
	  if( etabin == 2) graphfin->SetMarkerColor(kGreen);
	  if( etabin == 3) graphfin->SetMarkerColor(kRed);
	  //if ( phibin == 1 ) graphini->SetMarkerColor(kBlue);
	  //if ( phibin == 2 ) graphini->SetMarkerColor(kGreen);
	  //if ( phibin == 3 ) graphini->SetMarkerColor (kRed);
	  graphini->Draw("P"); 
     
	  TLegend *mylegend = new TLegend(0.6,0.75,0.9,0.9);
	  mylegend->SetTextFont(42);
	  mylegend->SetTextSize(0.025);
	  mylegend->AddEntry(graphini,"initial","P");
	  mylegend->AddEntry(graphfin,"corrected ","P");
	  mylegend->Draw("");
           
	}
 
      }
      char plotname[50];
      //sprintf(plotname,"DeltaM_eta%d_phi_%d.png",etabin,phibin);
      //cDeltaM->Print(plotname);  //printing or not can make the program to go faster
    } 
  }

  //****************same for epsilon********************************************
  for (int etabin=1; etabin<= totaletabins ; etabin++) {
    for (int phibin=1; phibin<= totalphibins ; phibin++) { 

      TCanvas *cEpsilon = new TCanvas("cEpsilon"," ",950,900);

      for (int l=0; l<nfiles; ++l) { 
	y[l] = hc_epsilon[l]->GetBinContent(etabin,phibin);
	Y[l] =  hDeltaM[l]->GetBinContent(etabin,phibin);
	Y_Iteration1[l] =  hDeltaM_Iteration1[l]->GetBinContent(etabin,phibin);
        /*if (etabin == 1 && phibin == 1) y[4] = -0.00200429;
        if (etabin == 2 && phibin == 1) y[4] = -0.00200203;
        if (etabin == 3 && phibin == 1) y[3] = 0.00100433;
        if (etabin == 3 && phibin == 1) y[4] = -0.00201069;*/
	if ( sizeof(Y)/sizeof( Y[l] ) == nfiles ) {
	  if ( debug == true ) printf("Region of **** BIN ETA %d *** PHI BIN %d *** of FILE %d => Value of corrected Epsilon: %f \n",etabin,phibin,l,y[l]);
          y[l] = -y[l];
	  TGraph *graphfine = new TGraph(nfiles, x, y);
	  gPad->SetGridx(1);
	  gPad->SetGridy(1);
	  graphfine->GetXaxis()->SetTitle("#epsilon_{input}");
	  graphfine->GetYaxis()->SetTitle("- #epsilon_{result}");
	  //graphfine->GetYaxis()->SetTitleOffset(0.8);
	  graphfine->SetMarkerStyle(20); //filled circle
	  graphfine->SetMarkerSize(1.5);
	  if( etabin == 1) graphfine->SetMarkerColor(kBlue);
	  if( etabin == 2) graphfine->SetMarkerColor(kGreen);
	  if( etabin == 3) graphfine->SetMarkerColor(kRed);
          //if( phibin == 1) graphfine->SetMarkerColor(kBlue);
	  //if( phibin == 2) graphfine->SetMarkerColor(kGreen);
	  //if( phibin == 3) graphfine->SetMarkerColor(kRed);
	  graphfine->SetMaximum(0.004);
	  graphfine->SetMinimum(-0.004);
	  graphfine->Draw("AP"); 
     
	  TLegend *mylegende = new TLegend(0.599366,0.800915,0.792812,0.900458);
	  mylegende->SetTextFont(42);
	  mylegende->SetTextSize(0.025);
	  mylegende->AddEntry(graphfine,"corrected","P");
	  mylegende->Draw("");
           
	}
 
      }
      char plotname[50];
      //sprintf(plotname,"Epsilon_eta%d_phi_%d.png",etabin,phibin);
      //cEpsilon->Print(plotname);  //depending on the needs printing or not can make the program to go faster
    } 
  }

 
}


