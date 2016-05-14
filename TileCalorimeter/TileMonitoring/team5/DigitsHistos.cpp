/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Shows the pulse shape
// author: andrea.dotti@pi.infn.it

#define pulse_shape_cpp
#include <TFile.h>
#include <TString.h>
#include <TProfile.h>
#include <TH1F.h>
#include <TH1D.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <iostream>
#include <sstream>
#include <iomanip>



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
  if (!file) {
    std::cout << "Opening file " << filename << "\n";
    file= TFile::Open(filename);
  }
  return file;
}


Int_t pulse_shape(TString runno, TString filename, TString runtype, TString module, TString chan, bool save_plots=false, TString outdir="")
{
  // Example: root[1] .L PulseShape.cpp+
  //          root[2] pulse_shape("61919","","CIS",LBA12",12,false,"/tmp")
 
  TFile *file = (TFile*)gROOT->GetListOfFiles()->FindObject(filename);
  if (!file) {
    file = openFile(runno,filename,runtype);
  }
  
  TString w_outfile;
  if (save_plots) {
    if (outdir=="") {
      outdir="/afs/cern.ch/user/t/tilebeam/scratch0/Commissioning/results/team5/CIS/"+module+"/";
    }
    
    w_outfile= outdir+"/r"+runno+"_"+module+"_pulseshape_chan_"+chan+".ps";
  }
  gStyle->SetStatFontSize(0.12);
  
  if (chan.Length()!=2) {
    std::cerr << "Error with the channel number: " << chan << "\n" ;
    std::cerr << "Please you chan number from 01 to 47 in two digits format\n" ;
    return 3; 
  }
  
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
  
  //Creating the canvases
  TCanvas* c_pulse = new TCanvas; 
  TProfile* profs[2];           // 2 gains
  c_pulse->Divide(1,2);
  TString gain_str[2]= {"hi","lo"};
  if ( !file->cd("Tile/Digits/"+module) )
    {
      std::cerr<<"Cannot find directory: Tile/Digits/"<<module<<endl;
      return 3;
    }
  for (Int_t gain = 0; gain<2;++gain)
    {
      std::ostringstream sStr("");
      std::ostringstream sChan("");
      sChan<<std::setfill('0')<<std::setw(2)<<chan<<setfill(' ');
      sStr<<"Tile/Digits/"<<module<<"/"<<module<<"_pmt_"<<sChan.str()<<"_"<<gain_str[gain]<<"_prof";
      profs[gain]=(TProfile*)file->Get(sStr.str().c_str());
      if ( !profs[gain] )
	std::cerr<<"Cannot get histogram named: "<<sStr.str()<<std::endl;
      c_pulse->cd(gain+1);
      if ( profs[gain] ) {
	profs[gain]->GetXaxis()->SetTitleSize(0.05);
	profs[gain]->GetXaxis()->SetTitle(module+" ch "+sChan.str()+" "+" Samples");
	profs[gain]->GetYaxis()->SetTitleSize(0.05);
	profs[gain]->GetYaxis()->SetTitle("ADC");
	profs[gain]->Draw();
      }
    }
  c_pulse->Update();
  if (save_plots)
    {
      c_pulse->Print(w_outfile);
    }
  return 0;
}


Int_t pulse_shape(TString runno, TString filename, TString runtype, TString module, bool save_plots=false, TString outdir="")
{
  // Example: root[1] .L PulseShape.cpp+
  //          root[2] pulse_shape("61919","","CIS",LBA12",12,false,"/tmp")
 
  TFile *file = (TFile*)gROOT->GetListOfFiles()->FindObject(filename);
  if (!file) {
    file = openFile(runno,filename,runtype);
  }
  
  TString w_outfile;
  if (save_plots) {
    if (outdir=="") {
      outdir="/afs/cern.ch/user/t/tilebeam/scratch0/Commissioning/results/team5/CIS/"+module+"/";
    }
    
    w_outfile= outdir+"/r"+runno+"_"+module+"_pulseshape.ps";
  }
  gStyle->SetStatFontSize(0.12);
  
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
  if ( !file->cd("Tile/Digits/"+module) )
    {
      std::cerr<<"Cannot find directory: Tile/Digits/"<<module<<endl;
      return 3;
    }
  //Creating the canvases
  TCanvas* c_pulse[4];
  TProfile* profs[2*48];           // 2 gains
  for ( int i=0;i<4;++i)
    {
      c_pulse[i] = new TCanvas;
      c_pulse[i]->Divide(6,4);
    }
  TString gain_str[2]= {"hi","lo"};
  c_pulse[0]->SetTitle("HG_PMT00-PMT23");
  c_pulse[1]->SetTitle("HG_PMT24-PMT47");
  c_pulse[2]->SetTitle("LG_PMT24-PMT47");
  c_pulse[3]->SetTitle("LG_PMT24-PMT47");

  for (Int_t chan = 0; chan<47;++chan) {
    for (Int_t gain = 0; gain<2;++gain)
      {
	Int_t cidx = (gain*48)+chan;
	std::ostringstream sStr("");
	std::ostringstream sChan("");
	sChan<<std::setfill('0')<<std::setw(2)<<chan<<setfill(' ');
	sStr<<"Tile/Digits/"<<module<<"/"<<module<<"_pmt_"<<sChan.str()<<"_"<<gain_str[gain]<<"_prof";
	profs[cidx]=(TProfile*)file->Get(sStr.str().c_str());
	if ( !profs[cidx] )
	  std::cerr<<"Cannot get histogram named: "<<sStr.str()<<std::endl;
	c_pulse[(chan/24)+(2*gain)]->cd((chan%24)+1);
	if ( profs[cidx] ) {
	  profs[cidx]->GetXaxis()->SetTitleSize(0.05);
	  profs[cidx]->GetXaxis()->SetTitle(module+" ch "+sChan.str()+" "+" Samples");
	  profs[cidx]->GetYaxis()->SetTitleSize(0.05);
	  profs[cidx]->GetYaxis()->SetTitle("ADC");
	  profs[cidx]->Draw();
	}
      }
  }
  for ( int i = 0 ;i<4;++i) c_pulse[i]->Update();
    //  c_pulse->Update();
  if (save_plots)
    {
      c_pulse[0]->Print(w_outfile+"(");
      c_pulse[1]->Print(w_outfile);
      c_pulse[2]->Print(w_outfile);
      c_pulse[3]->Print(w_outfile+")");
    }
  return 0;
}


Int_t adc_dist(TString runno, TString filename, TString runtype, TString module, TString chan, bool save_plots=false, TString outdir="")
{
  // Example: root[1] .L PulseShape.cpp+
  //          root[2] pulse_shape("61919","","CIS",LBA12",12,false,"/tmp")
 
  TFile *file = (TFile*)gROOT->GetListOfFiles()->FindObject(filename);
  if (!file) {
    file = openFile(runno,filename,runtype);
  }
  
  TString w_outfile;
  if (save_plots) {
    if (outdir=="") {
      outdir="/afs/cern.ch/user/t/tilebeam/scratch0/Commissioning/results/team5/CIS/"+module+"/";
    }
    
    w_outfile= outdir+"/r"+runno+"_"+module+"_adcdist_chan_"+chan+".ps";
  }
  gStyle->SetStatFontSize(0.12);
  
  if (chan.Length()!=2) {
    std::cerr << "Error with the channel number: " << chan << "\n" ;
    std::cerr << "Please you chan number from 01 to 47 in two digits format\n" ;
    return 3; 
  }
  
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
  
  //Creating the canvases
  TCanvas* c_pulse = new TCanvas; 
  TH1* profs[2];           // 2 gains
  c_pulse->Divide(1,2);
  TString gain_str[2]= {"hi","lo"};
  if ( !file->cd("Tile/Digits/"+module) )
    {
      std::cerr<<"Cannot find directory: Tile/Digits/"<<module<<endl;
      return 3;
    }
  for (Int_t gain = 0; gain<2;++gain)
    {
      std::ostringstream sStr("");
      std::ostringstream sChan("");
      sChan<<std::setfill('0')<<std::setw(2)<<chan<<setfill(' ');
      sStr<<"Tile/Digits/"<<module<<"/"<<module<<"_pmt_"<<sChan.str()<<"_"<<gain_str[gain]<<"_samples";
      profs[gain]=(TH1*)file->Get(sStr.str().c_str());
      if ( !profs[gain] )
	std::cerr<<"Cannot get histogram named: "<<sStr.str()<<std::endl;
      c_pulse->cd(gain+1);
      if ( profs[gain] ) {
	profs[gain]->GetXaxis()->SetTitleSize(0.05);
	profs[gain]->GetXaxis()->SetTitle(module+" ch "+sChan.str()+" "+" ADC");
	profs[gain]->GetYaxis()->SetTitleSize(0.05);
	profs[gain]->GetYaxis()->SetTitle("entries");
	profs[gain]->Draw();
	if ( profs[gain]->GetMaximum() > 0 )
	  gPad->SetLogy(true);
      }
    }
  c_pulse->Update();
  if (save_plots)
    {
      c_pulse->Print(w_outfile);
    }
  return 0;
}


Int_t adc_dist(TString runno, TString filename, TString runtype, TString module, bool save_plots=false, TString outdir="")
{
  // Example: root[1] .L PulseShape.cpp+
  //          root[2] adc_dist("61919","","CIS",LBA12",12,false,"/tmp")
 
  TFile *file = (TFile*)gROOT->GetListOfFiles()->FindObject(filename);
  if (!file) {
    file = openFile(runno,filename,runtype);
  }
  
  TString w_outfile;
  if (save_plots) {
    if (outdir=="") {
      outdir="/afs/cern.ch/user/t/tilebeam/scratch0/Commissioning/results/team5/CIS/"+module+"/";
    }
    
    w_outfile= outdir+"/r"+runno+"_"+module+"_adcdist.ps";
  }
  gStyle->SetStatFontSize(0.12);
  
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
  if ( !file->cd("Tile/Digits/"+module) )
    {
      std::cerr<<"Cannot find directory: Tile/Digits/"<<module<<endl;
      return 3;
    }
  //Creating the canvases
  TCanvas* c_pulse[4];
  TH1* profs[2*48];           // 2 gains
  for ( int i=0;i<4;++i)
    {
      c_pulse[i] = new TCanvas;
      c_pulse[i]->Divide(6,4);
    }
  TString gain_str[2]= {"hi","lo"};
  c_pulse[0]->SetTitle("HG_PMT00-PMT23");
  c_pulse[1]->SetTitle("HG_PMT24-PMT47");
  c_pulse[2]->SetTitle("LG_PMT24-PMT47");
  c_pulse[3]->SetTitle("LG_PMT24-PMT47");

  for (Int_t chan = 0; chan<47;++chan) {
    for (Int_t gain = 0; gain<2;++gain)
      {
	Int_t cidx = (gain*48)+chan;
	std::ostringstream sStr("");
	std::ostringstream sChan("");
	sChan<<std::setfill('0')<<std::setw(2)<<chan<<setfill(' ');
	sStr<<"Tile/Digits/"<<module<<"/"<<module<<"_pmt_"<<sChan.str()<<"_"<<gain_str[gain]<<"_samples";
	profs[cidx]=(TH1*)file->Get(sStr.str().c_str());
	if ( !profs[cidx] )
	  std::cerr<<"Cannot get histogram named: "<<sStr.str()<<std::endl;
	c_pulse[(chan/24)+(2*gain)]->cd((chan%24)+1);
	if ( profs[cidx] ) {
	  profs[cidx]->GetXaxis()->SetTitleSize(0.05);
	  profs[cidx]->GetXaxis()->SetTitle(module+" ch "+sChan.str()+" "+" Samples");
	  profs[cidx]->GetYaxis()->SetTitleSize(0.05);
	  profs[cidx]->GetYaxis()->SetTitle("ADC");
	  if ( profs[cidx]->GetMaximum() > 0 )
	    gPad->SetLogy(true);
	  profs[cidx]->Draw();
	}
      }
  }
  for ( int i = 0 ;i<4;++i) c_pulse[i]->Update();
    //  c_pulse->Update();
  if (save_plots)
    {
      c_pulse[0]->Print(w_outfile+"(");
      c_pulse[1]->Print(w_outfile);
      c_pulse[2]->Print(w_outfile);
      c_pulse[3]->Print(w_outfile+")");
    }
  return 0;
}

