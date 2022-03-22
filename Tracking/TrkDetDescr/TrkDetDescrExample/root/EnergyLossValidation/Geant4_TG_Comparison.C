/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "EnergyLossComparisonTree.h"
#include "TApplication.h"
#include "TPostScript.h"
#include "TProfile.h"
#include "TCanvas.h"
#include "TString.h"
#include "TStyle.h"
#include "TLatex.h"

#include <vector>
#include <iostream>

int main(int argc, char **argv);

int main(int argc, char **argv)
{
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
   //plain->SetStatColor(1);
   plain->SetOptStat(0);
   // Labelling
   plain->SetLabelFont(132, "XY");
   plain->SetLabelSize(0.05, "XY");

   gROOT->SetStyle("Plain");

// =======================================================/


		int bins = 0;
		
    if (argc<2) {

      std::cout << " [e] Not engough arguments given ! " << std::endl;
      std::cout << " [i] Sample usage (for files: file.root) : " << std::endl;
      std::cout << "        Geant4_TG_Comparison file " << std::endl;

    } else {

      std::cout << " [m] Creating the ElossValidation classes ... " << std::endl;
		
      TString file       =  argv[1];
      file      += ".root"; 

      std::cout << " [m] Input file 1:    " << file << std::endl;
		
      TString saveExtension = "none";
      int mode = 0;
      if (argc>2) mode = std::abs(strcmp(argv[2], "0"));
	


      TFile* File = TFile::Open(file);
      TTree* Tree = File ? (TTree*)File->Get("EnergyLossValidation") : 0;

      //TFile* outputFile = new TFile("TG_Geant4.root", "recreate");
      
      if (Tree){
 
        std::cout << " [m] Creating the energy loss tree ... " << std::endl; 

        EnergyLossComparisonTree eloss(Tree);

	std::cout << " [m] Creating vectors of histograms " << std::endl;
				
	// get the histograms
	std::vector<TH1D*>* hists = eloss.histograms(bins, mode);

	std::cout << " [m] Drawing the histograms ... " << std::endl;

		
       TString xAxisTitle    = "#eta";
       TString yAxisTitle = (mode == 0) ? "MPV (MeV)" : "#sigma (MeV)";

       int tempInd = 0;
       std::cout << "Retrieved histos " << hists->size() << std::endl;
       TPostScript ps((mode == 0) ? "TG_Geant4.ps" : "TG_Geant4Sigma.ps",112);
       TCanvas* currCanv = new TCanvas();//, 100+icanv*20, 100+icanv*20, 600, 920);
       for (int icanv=0; icanv<hists->size()/2; ++icanv){	
	 //tempInd = std::string(hists->at(2*icanv)->GetName()).find(':');
	 //std::string finalName = std::string("Compare")+std::string(hists->at(2*icanv)->GetName()).substr(tempInd);
	 //currCanv->Divide(2,3);
	 hists->at(2*icanv)->GetXaxis()->SetTitle(xAxisTitle);
	 hists->at(2*icanv)->GetYaxis()->SetTitle(yAxisTitle);
	 hists->at(2*icanv)->Draw("hist");
	 hists->at(2*icanv+1)->SetMarkerStyle(8);
	 hists->at(2*icanv+1)->SetMarkerSize(0.4);
	 hists->at(2*icanv+1)->Draw("e1psame");
	 currCanv->Update();

	     // draw the histograms
	 //		   for (int ihist=0; ihist<6; ++ihist){
	 //	    currCanv->cd(ihist+1);
         
	 /*if (ihist<6){
            // draw the histogram
	           (*g4Profiles)[icanv*7+ihist]->SetMinimum(0.);
             (*g4Profiles)[icanv*7+ihist]->SetYTitle(yAxisTitle[ihist]);
             (*tgProfiles)[icanv*7+ihist]->SetMarkerStyle(8);		      
             (*tgProfiles)[icanv*7+ihist]->SetMarkerSize(0.4);
             (*tgProfiles)[icanv*7+ihist]->SetXTitle("#eta");
             (*g4Profiles)[icanv*7+ihist]->Draw("hist");
             (*tgProfiles)[icanv*7+ihist]->Draw("same");
		    
          // do the normalization for the last to plots (rho/t) !
          } else {
             TH1F* g4Histo = (TH1F*)(*g4Profiles)[icanv*7+ihist];
             TH1F* g4Path  = (TH1F*)(*g4Profiles)[icanv*7+6];
             
             TH1F* tgHisto = (TH1F*)(*tgProfiles)[icanv*7+ihist];
             TH1F* tgPath  = (TH1F*)(*tgProfiles)[icanv*7+6];
             // get the number of bins
             Int_t numBins = g4Histo->GetNbinsX();


             TString drawTgName = "tg_hist_";
                     drawTgName += icanv;
                     drawTgName += "_";
                     drawTgName += ihist;

  
             TH1F* drawTgHisto = new TH1F(drawTgName, "Renormalized", numBins, -3.,3.);

             for (Int_t ibin=0; ibin < numBins; ++ibin)
             {
                 Double_t g4_pathBin = g4Path->GetBinContent(ibin);
                 Double_t tg_pathBin = tgPath->GetBinContent(ibin);

                 Double_t tg_histBin = tgHisto->GetBinContent(ibin);

                 Double_t tg_fillBin = (tg_pathBin > 10e-6) ? tg_histBin*tg_pathBin/g4_pathBin : 0;

                 drawTgHisto->SetBinContent(ibin,tg_fillBin);

             } 

             g4Histo->SetMinimum(0.);
             g4Histo->SetYTitle(yAxisTitle[ihist]);
             drawTgHisto->SetMarkerStyle(8);         
             drawTgHisto->SetMarkerSize(0.4);
             drawTgHisto->SetXTitle("#eta");
             g4Histo->Draw("hist");
             drawTgHisto->Draw("same, p");
        
            }

        
	    }*/

			 	 
         if (saveExtension == "gif" || saveExtension == "eps" || saveExtension == "pdf" ){
           TString saveName  = "Output/";
	   //saveName += finalName;
                   saveName += ".";
                   saveName += saveExtension;
           currCanv->SaveAs(saveName);
         }
       } 
       ps.Close();

       TCanvas* cTruth = new TCanvas("cTruth","cTruth", 800,700);
       cTruth->Divide(2,2);


       TCanvas* cReco = new TCanvas("cReco","cReco",800,700);
       cReco->Divide(2,2);

       std::cout << " [m]  Created new canvases... " << std::endl;

       TH2D* true2D = eloss.combinedTrueHistos();
       cTruth->cd(1);
       true2D->Draw();
       cTruth->cd(4);
       std::vector<TH1D*>* trueFits = eloss.doTrueFits(true2D);
       cTruth->cd(2);
       trueFits->at(0)->Draw("e1p");
       cTruth->cd(3);
       trueFits->at(1)->Draw("e1p");
       cTruth->cd(4);

       TH2D* reco2D = eloss.combinedRecoHistoMPV();
       cReco->cd(1);
       reco2D->Draw();
       TH2D* reco2DSigma = eloss.combinedRecoHistoSigma();
       cReco->cd(2);
       reco2DSigma->Draw();

       TH1D* reco1D = eloss.doRecoFits(reco2D,0);
       cReco->cd(3);
       reco1D->Draw("e1p");
       TH1D* reco1DSigma = eloss.doRecoFits(reco2DSigma,1);
       cReco->cd(4);
       reco1DSigma->Draw("e1p");

       cTruth->cd(4);
       trueFits->at(0)->SetMinimum(0);
       trueFits->at(0)->Draw("hist");
       trueFits->at(1)->Draw("samehist");
       reco1D->Draw("e1psame");
       reco1DSigma->Draw("e1psame");
       
       /*       true2D = eloss.combinedTrueHistos();
       cTruth->cd(1);
       true2D->Draw();
       */
       /*std::vector<TH2D*>* trueHistos = eloss.trueHistos();
       for(int i = 0; i<trueHistos->size();i++){
	 cTruth->cd();
	 cTruth->cd(i+1);
	 trueHistos->at(i)->Draw();
       }
       std::vector<TH2D*>* recoHistos = eloss.recoHistos(); 
       //std::cout << trueHistos->size() << std::endl;
       //std::cout << recoHistos->size() << std::endl;
       for(int i = 0; i<recoHistos->size();i++){
	 cReco->cd();
	 cReco->cd(i+1);
	 recoHistos->at(i)->Draw();
	 }*/
       
       std::cout << " [m]  Saving new canvases... " << std::endl;
       
       cTruth->SaveAs("TruthHistos.root");
       cReco->SaveAs("RecoHistos.root");
       
       std::cout << " [m]  Saved new canvases... " << std::endl;
       
      }

      std::cout << " [m] Writing the output file ... " << std::endl;
      //outputFile->Write();

    } // not enough arguments given
 
   theApp.Run();
   return(0);
}
