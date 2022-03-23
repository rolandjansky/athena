/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MaterialMapperTree.h"
#include "TApplication.h"
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
		
    if (argc<3) {

      std::cout << " [e] Not engough arguments given ! " << std::endl;
      std::cout << " [i] Sample usage (for files: TG_file.root, G4_file.root) : " << std::endl;
      std::cout << "        Geant4_TG_Comparison G4_file TG_file <65> <0> <1> <gif>" << std::endl;

    } else {

     	std::cout << " [m] Creating the MaterialMapping classes ... " << std::endl;
		
	      TString g4file       =  argv[1];
		      g4file      += ".root"; 
              TString tgfile       =  argv[2];
                      tgfile      += ".root";    	

	     std::cout << " [m] Input file 1:    " << g4file << std::endl;
	     std::cout << " [m] Input file 2:    " << tgfile << std::endl;
		
	     if (argc<4) {
		    std::cout << " WARNING: not enough arguments given, bin number will be set to 60" << std::endl;
		    bins = 60;
	    } else 
		    bins = atoi(argv[3]);
		  
		
      int doCalo = 0;
      if (argc>4)
          doCalo = atoi(argv[4]);
      
      bool doEta = false;
      if (argc>5)
          doEta = bool(atoi(argv[5]));
            
      TString saveExtension = "none";
      if (argc>6)
            saveExtension = argv[6];


      TFile* g4File = TFile::Open(g4file);
      TTree* g4Tree = g4File ? (TTree*)g4File->Get("MaterialTree") : 0;

      TFile* tgFile = TFile::Open(tgfile);
      TTree* tgTree = tgFile ? (TTree*)tgFile->Get("MaterialTree") : 0;

      TFile* outputFile = new TFile("TG_Geant4.root", "recreate");

      const int numCanvas = 12;

      if (g4Tree && tgTree){
 
        std::cout << " [m] Creating the material trees ... " << std::endl; 

        MaterialMapperTree g4Material(g4Tree);
	       MaterialMapperTree tgMaterial(tgTree);

     	  std::cout << " [m] Creating vectors of Profile histograms with #bins " << bins << std::endl;
				
	      
     	  std::cout << " [m] Drawing the histograms ... " << std::endl;

				
	       TString canvNames[numCanvas] 
                              = { "BeamPipe",
		            "Pixel",
		                  "SCT",
                                  "TRT",
				  "IDS",
                                  "ID",
                                  "Solenoid",
                                  "LAr",
                                  "Tile",
                                  "CaloServ",
                                  "Calo",
                                  "Total" 
                                 };
																
        TString canvTitles[numCanvas] 
                              = { "Material: BeamPipe",
			          "Material: Pixel",
				  "Material: SCT",
				  "Material: TRT",
                                  "Material: ID Services",
                                  "Material: Inner Detector",
                                  "Material: Solenoid",
                                  "Material: LAr",
                                  "Material: Tile",
                                  "Material: Calo Services",
                                  "Material: Calorimeter",
                                  "Material: Total"
		                            };
		
		
       TString xAxisTitle    = "#eta";
       TString yAxisTitle[9] = { "t [X_{0}]", "t [L_{0}]", "A", "Z", "Z/A", "#rho [g/cm^{3}]", "t * A/Z * #rho [g/cm^{3}]", "", "" }; 

      int loopCanvas = doCalo ? numCanvas : 5;
											
      std::cout << " [m] Configured to loop over " << loopCanvas << " Canvases " << std::endl;


       // get the profile histograms
       std::vector<TProfile*>* g4Profiles = g4Material.profileHistograms(bins, "Geant4", doEta);
       std::vector<TProfile*>* tgProfiles = tgMaterial.profileHistograms(bins, "Tracking Geometry", doEta);
         	
       for (int icanv=0; icanv<loopCanvas; ++icanv)
	{	
		      
	 TCanvas* currCanv = new TCanvas(canvNames[icanv], canvTitles[icanv], 100+icanv*20, 100+icanv*20, 900, 920);
	 currCanv->Divide(3,3);
			 

          int numhists = 9;

	     // draw the histograms
		   for (int ihist=0; ihist<numhists; ++ihist){
			    currCanv->cd(ihist+1);
         
          if (ihist<numhists-1){
              // draw the histogram
	           (*g4Profiles)[icanv*numhists+ihist]->SetMinimum(0.);
	           //cppcheck-suppress arrayIndexOutOfBoundsCond
             (*g4Profiles)[icanv*numhists+ihist]->SetYTitle(yAxisTitle[ihist]);
             (*tgProfiles)[icanv*numhists+ihist]->SetMarkerStyle(8);		      
             (*tgProfiles)[icanv*numhists+ihist]->SetMarkerSize(0.4);
              if (doEta)   
                 (*tgProfiles)[icanv*numhists+ihist]->SetXTitle("#eta");
              else
	         (*tgProfiles)[icanv*numhists+ihist]->SetXTitle("#phi");              
             (*g4Profiles)[icanv*numhists+ihist]->Draw("hist");
             (*tgProfiles)[icanv*numhists+ihist]->Draw("same");
		    
          // do the normalization for the last to plots (rho/t) !
          } else {
             TH1F* g4Histo = (TH1F*)(*g4Profiles)[icanv*numhists+ihist];
             TH1F* g4Path  = (TH1F*)(*g4Profiles)[icanv*numhists+numhists-1];
             
             TH1F* tgHisto = (TH1F*)(*tgProfiles)[icanv*numhists+ihist];
             TH1F* tgPath  = (TH1F*)(*tgProfiles)[icanv*numhists+numhists-1];
             // get the number of bins
             Int_t numBins = g4Histo->GetNbinsX();


             TString drawTgName = "tg_hist_";
                     drawTgName += icanv;
                     drawTgName += "_";
                     drawTgName += ihist;

  
             TH1F* drawTgHisto = new TH1F(drawTgName, "Renormalized", numBins, -4.,4.);

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

        
         }
			     
         if (saveExtension == "gif" || saveExtension == "eps" || saveExtension == "pdf" ){
           TString saveName  = "Output/";
                   saveName += canvNames[icanv];
                   saveName += ".";
                   saveName += saveExtension;
           currCanv->SaveAs(saveName);
          }
       }
     }
     std::cout << " [m] Writing the output file ... " << std::endl;
     outputFile->Write();
    } // not enough arguments given
 
   theApp.Run();
   return(0);
}
