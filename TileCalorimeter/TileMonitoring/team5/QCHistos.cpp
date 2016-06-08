/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* Macro to produce L2 Quality Control  plots from athena based histograms

*/
#define qchistos_ccp
#include <TROOT.h>
#include <TFile.h>
#include <TH1S.h>
#include <TH2S.h>
#include <TProfile.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TString.h>
#include <TLine.h>
#include <TPaveText.h>
#include <TText.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>


TFile* openFile(TString runno, TString filename, TString runtype) {

  if (filename=="") {
      TString defdir = "rfio:/castor/cern.ch/user/t/tilebeam/commissioning/";
      TString deffile = "tilemon_"+runno+"_"+runtype+".0.root";
      filename = defdir+deffile;
  }
  else 
    {
      filename = "rfio:"+filename;
    }
  TFile *file = (TFile*)gROOT->GetListOfFiles()->FindObject(filename);
  if ( !file ) {
    std::cout << "Opening file " << filename << "\n";
    file = TFile::Open(filename);
  }
  return file;
}

Int_t quality(TString runno, TString filename, TString runtype, TString module, bool draw_plots=false, TString outdir="")
{
  // Example : root [1] .L QCHistos.ccp+
  //           root [2] quality("61516","","CIS","LBA59",false,"/mydir/")

  ////////////////////////////////////////////////////////////////


  TFile *file = (TFile*)gROOT->GetListOfFiles()->FindObject(filename);

  if (!file) {
    file = openFile(runno,filename,runtype);
  }


  TString w_outfile;
  TString o_outfile;
  TString c_outfile;

  if (draw_plots) {
    if (outdir=="") {
      outdir="/afs/cern.ch/user/t/tilebeam/scratch0/Commissioning/results/team5/"+runtype+"/"+module+"/";
    }
    w_outfile= outdir+"/r"+runno+"_"+module+"_quality.ps"; // ps files with multiple pages need a special treatment
    o_outfile= w_outfile+"(";              // This is the way to open it
    c_outfile= w_outfile+")";		   // This is the way to close it
  }

  TString gain_str[2]= {"hi","lo"};
  //  TString check_str[2]= {"BCID","CRC"};

  //gStyle->SetTitleFontSize(0.15);
  gStyle->SetStatFontSize(0.12);
  //  gStyle->SetLabelSize(0.14);


  if (file->IsZombie()) 
    {
      std::cerr << "Error opening file: " << filename << "\n" ;
      return 1;
    }
  
  std::cout << "File " << filename << " opened\n";

  if (module=="all") 
    {
      std::cout << "Sorry, I don't know how to handle all modules this at the moment \n";
      return 2;
    }

  std::cout << "Entering directory Tile/Digits/"<< module <<"\n";
  file->cd( TString("Tile/Digits/"+module) );

  TH1S** histo = new TH1S*[16];
  //  TPaveText** ptext_crc = new TPaveText*[16]; 


  TCanvas* c_crc = new TCanvas;
  c_crc->Divide(4,4);

  //  gStyle->SetOptStat(0);

  std::cout <<"\n\n";
  for (Int_t i=0; i<16; ++i) {
    
    std::ostringstream sStr("");
    std::ostringstream sChan("");
    sChan << std::setfill('0') << std::setw(2) << i << setfill(' ');
    sStr << "Tile/Digits/" << module << "/" << module << "_dmu_" << sChan.str() << "_CRC_err";
    
    
    //std::cout << "Getting histo:"<< sStr.str() <<"\n";
    histo[i] =(TH1S*)file->Get( TString(sStr.str()) );
    
    c_crc->cd(i+1);
    
    if (histo[i]->GetMaximum()>0.) {
      gPad->SetGrid(1,1);
      gPad->SetLogy(1);
    }	
      
    if ((histo[i]->GetMean()==1.)&&(histo[i]->GetRMS()==0.)) {
      histo[i]->SetFillColor(30);
    }
    else {
      histo[i]->SetFillColor(2);
    }
    histo[i]->Draw("histo");
    histo[i]->GetXaxis()->SetTitleSize(0.05);
    histo[i]->GetYaxis()->SetTitleSize(0.05);
    histo[i]->GetXaxis()->SetTitle(module+" DMU "+sChan.str()+"    CRC");
    //ptext_crc[i] = new TPaveText(3.75,1.,5.05,histo[i]->GetMaximum());

    //ptext_crc[i]->SetLabel("Values %");
    //TText* text1= ptext_crc[i]->AddText("CRC ok: ");
    //TText* text2= ptext_crc[i]->AddText("CRC==0: ");
    //TText* text3= ptext_crc[i]->AddText("FE CRC err: ");
    //TText* text4= ptext_crc[i]->AddText("ROD CRC err:");
    //TText* text5= ptext_crc[i]->AddText("FE+ROD CRC err:");
    //ptext_crc[i]->Draw();
    Double_t nentries=  histo[i]->GetEntries();
    std::cout <<module<< " TileDMU"<<sChan.str()<<":\tCRC ok: "<<  histo[i]->GetBinContent(2)/nentries <<"\tCRC==0: "<<histo[i]->GetBinContent(1)/nentries<<"\tFE CRC err: "<<histo[i]->GetBinContent(3)/nentries<<"\tROD CRC err: "<<histo[i]->GetBinContent(4)/nentries<<"\tFE+ROD CRC err: "<<histo[i]->GetBinContent(5)/nentries<<"\n";

  }	

  c_crc->Update(); // refresh Canvas once completely filled

  if (draw_plots) { // Filling ps output file
	
    std::cout << "First canvas\n";
    c_crc->Print(o_outfile);
	  //	  c_cis[4*gain+2*cap].Print(w_outfile);
  }


  if (true) { //now all runs have bigain plots, 
              //but I keep the if in case we revert 
              //to the old situation at some point
  //  if (runtype=="CIS"|| runtype=="Ped") { // double gain runs
    TCanvas* c_bcid = new TCanvas[2];
    TH1S** histo_bcid = new TH1S*[2*16];
    for (Int_t gain=0; gain<2; ++gain) { //loop on gains: lo, hi  
    
      c_bcid[gain].Divide(4,4);
    
      std::cout <<"\n\n";
      for (Int_t i=0; i<16; ++i) {
	
	Int_t idx = 16*gain+i;
	std::ostringstream sStr("");
	std::ostringstream sChan("");
	sChan << std::setfill('0') << std::setw(2) << i << setfill(' ');
	sStr << "Tile/Digits/" << module << "/" << module << "_dmu_" << sChan.str() << "_" << gain_str[gain] << "_BCID_err";


	//	  std::cout << "Getting histo: "<< sStr.str() <<"\n";
	histo_bcid[idx] =(TH1S*)file->Get( TString(sStr.str()) );

	c_bcid[gain].cd(i+1);

	if (histo_bcid[idx]->GetMaximum()>0.) {
	  gPad->SetGrid(1,1);
	  gPad->SetLogy(1);
	}	
	
	if ((histo_bcid[idx]->GetMean()==1.)&&(histo_bcid[idx]->GetRMS()==0.)) {
	  histo_bcid[idx]->SetFillColor(30);
	}
	else {
	  histo_bcid[idx]->SetFillColor(2);
	}
	histo_bcid[idx]->Draw("histo");
	histo_bcid[idx]->GetXaxis()->SetTitleSize(0.05);
	histo_bcid[idx]->GetYaxis()->SetTitleSize(0.05);
	histo_bcid[idx]->GetXaxis()->SetTitle(module+" DMU "+sChan.str()+" at "+gain_str[gain]+" gain   BCID");

	Double_t nentries=  histo_bcid[idx]->GetEntries();
	std::cout <<module<<" TileDMU"<<sChan.str()<<" gain "<<gain<<":\tBCID ok: "<<  histo_bcid[idx]->GetBinContent(2)/nentries <<"\tBCID==0: "<<histo_bcid[idx]->GetBinContent(1)/nentries<<"\tBCID err: "<<histo_bcid[idx]->GetBinContent(3)/nentries<<"\n";
      }

      c_bcid[gain].Update(); // refresh Canvas once completely filled

      if (draw_plots) { // Filling ps output file
	if (gain==0) {
	  c_bcid[gain].Print(w_outfile);
	}
	else {
	  c_bcid[gain].Print(c_outfile);
	}

      }


    }	

    delete[] c_bcid;
    delete[] histo_bcid;

  }
  else {// single gain runs
    TCanvas* c_bcid = new TCanvas;
    TH1S** histo_bcid = new TH1S*[16];
    
    c_bcid->Divide(4,4);
    
    for (Int_t i=0; i<16; ++i) {
      
      Int_t idx = i;
      std::ostringstream sStr("");
      std::ostringstream sChan("");
      sChan << std::setfill('0') << std::setw(2) << i << setfill(' ');
      sStr << "Tile/Digits/" << module << "/" << module << "_dmu_" << sChan.str() << "_BCID_err";


      //	  std::cout << "Getting histo: "<< sStr.str() <<"\n";
      histo_bcid[idx] =(TH1S*)file->Get( TString(sStr.str()) );

      c_bcid->cd(i+1);
      
      if (histo_bcid[idx]->GetMaximum()>0.) {
	gPad->SetGrid(1,1);
	gPad->SetLogy(1);
      }	
	  
      if ((histo_bcid[idx]->GetMean()==1.)&&(histo_bcid[idx]->GetRMS()==0.)) {
	histo_bcid[idx]->SetFillColor(30);
      }
      else {
	histo_bcid[idx]->SetFillColor(2);
      }
      histo_bcid[idx]->Draw("histo");
      histo_bcid[idx]->GetXaxis()->SetTitleSize(0.05);
      histo_bcid[idx]->GetYaxis()->SetTitleSize(0.05);
      histo_bcid[idx]->GetXaxis()->SetTitle(module+" DMU "+sChan.str()+"     BCID");
    }

    c_bcid->Update(); // refresh Canvas once completely filled

    if (draw_plots) { // Filling ps output file
	
      c_bcid->Print(w_outfile);
	  //	  c_cis[4*gain+2*cap].Print(w_outfile);
    }

    delete[] histo_bcid;
  }	



  delete[] histo;
  return 0;
}
