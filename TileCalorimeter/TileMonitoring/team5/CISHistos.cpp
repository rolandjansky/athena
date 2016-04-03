/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* Macro to produce L2 plots from athena based histograms

*/
#define cis_histos_ccp
#include <TROOT.h>
#include <TFile.h>
#include <TH1S.h>
#include <TH2S.h>
#include <TProfile.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TString.h>
#include <TLine.h>
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

Int_t amp_vs_q(TString runno, TString filename, TString runtype, TString module, bool draw_plots=false, TString outdir="")
{
  // Example : root [1] .L ene_charge_ratio.cc+
  //           root [2] ene_charge_ratio("61516","","CIS","LBA59",false,"/mydir/")

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
      outdir="/afs/cern.ch/user/t/tilebeam/scratch0/Commissioning/results/team5/CIS/"+module+"/";
    }
    w_outfile= outdir+"/r"+runno+"_"+module+"_amp_vs_q.ps"; // ps files with multiple pages need a special treatment
    o_outfile= w_outfile+"(";              // This is the way to open it
    c_outfile= w_outfile+")";		   // This is the way to close it
  }

  TString gain_str[2]= {"hi","lo"};
  TString cap_str[2]= {"5","100"};


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
    


  //Opening the file
  std::cout << "Entering directory Tile/RawChannel/"<< module <<"\n";
  file->cd( TString("Tile/RawChannel/"+module) );

  //Creating the canvases
  TCanvas* c_cis[8]; //4*gain + 2*capacitor +(0-23 chans,24-47 chans)
  for ( int i = 0;i<8;++i) c_cis[i]=new TCanvas;
  //AND->
  TH2S* histo[4*48];// = new TH2S*[4*48];  //(2*gain + 2*capacitor) *48 chans
  //AND<-
  for (Int_t gain=0; gain<2; ++gain) { //loop on gains: lo, hi

    for (Int_t cap=0; cap <2; ++cap) { //loop on capacitors: 5pF, 100pF

      c_cis[4*gain+2*cap]->Divide(6,4);
      c_cis[4*gain+2*cap+1]->Divide(6,4);

      for (Int_t i=0; i<24; ++i) { // loop on chans: 0-23

	Int_t idx = 96*gain+48*cap+i;
	std::ostringstream sStr("");
	std::ostringstream sChan("");
	sChan << std::setfill('0') << std::setw(2) << i << setfill(' ');
	sStr << "Tile/RawChannel/" << module << "/" << module << "_pmt_" << sChan.str() << "_" << gain_str[gain] << "_amp_vs_q_" << cap_str[cap];

	//	std::cout << "Getting histo:"<< sStr.str() <<"\n";
	histo[idx] =(TH2S*)file->Get( TString(sStr.str()) );

	c_cis[4*gain+2*cap]->cd(i+1);
	

	if (histo[idx]->GetMaximum()>0.) {
	  gPad->SetGrid(1,1);
	}	
	histo[idx]->Draw("");
	histo[idx]->GetXaxis()->SetTitleSize(0.05);
	histo[idx]->GetYaxis()->SetTitleSize(0.05);
	histo[idx]->GetXaxis()->SetTitle(module+" ch "+sChan.str()+" "+cap_str[cap]+" pF at "+gain_str[gain]+" gain     Charge");
	histo[idx]->GetYaxis()->SetTitle("Efit");
	//sys.Sleep(5000);
	//gStyle->SetOptStat(0);
	//delete histo;
      }
      c_cis[4*gain+2*cap]->Update(); // refresh Canvas once completely filled

      if (draw_plots) { // Filling ps output file

	if ((4*gain+2*cap)<1) { //first canvas
	  std::cout << "First canvas\n";
	  c_cis[4*gain+2*cap]->Print(o_outfile);
	  //	  c_cis[4*gain+2*cap].Print(w_outfile);
	}
	else {
	  c_cis[4*gain+2*cap]->Print(w_outfile);
	}

      }

      for (Int_t i=24; i<48; ++i) { // loop on chans: 24-47

	Int_t idx = 96*gain+48*cap+i;
	std::ostringstream sStr("");
	std::ostringstream sChan("");
	sChan << std::setfill('0') << std::setw(2) << i << setfill(' ');
	sStr << "Tile/RawChannel/" << module << "/" << module << "_pmt_" << sChan.str() << "_" << gain_str[gain] << "_amp_vs_q_" << cap_str[cap];

	//std::cout << "Getting histo:"<< sStr.str() <<"\n";
	histo[idx] =(TH2S*)file->Get( TString(sStr.str()) );
	if ( !histo[idx] )
	  {
	    std::cerr<<"Cannot find histogram named: "<<sStr.str()<<endl;
	  }
	
	c_cis[4*gain+2*cap+1]->cd(i-23);
	
	if ( histo[idx] ) {
	  if (histo[idx]->GetMaximum()>0.) {
	    gPad->SetGrid(1,1);
	  }	
	  histo[idx]->Draw("");
	  histo[idx]->GetXaxis()->SetTitleSize(0.05);
	  histo[idx]->GetYaxis()->SetTitleSize(0.05);
	  histo[idx]->GetXaxis()->SetTitle(module+" ch "+sChan.str()+" "+cap_str[cap]+" pF at "+gain_str[gain]+" gain     Charge");
	  histo[idx]->GetYaxis()->SetTitle("Efit");
	}
    
      }
      c_cis[4*gain+2*cap+1]->Update();

      if (draw_plots) {
	
	if ((4*gain+2*cap+1)>6) 	 { // last canvas

	  //	  c_cis[4*gain+2*cap+1].Print(w_outfile);
	  c_cis[4*gain+2*cap+1]->Print(c_outfile);
	  std::cout << "Last canvas\n";
	}
	else {
	  c_cis[4*gain+2*cap+1]->Print(w_outfile);
	}

      }

    }

  }
  return 0;
}


Int_t amp_vs_q(TString runno, TString filename, TString runtype, TString module, TString chan, bool draw_plots=false, TString outdir="")
{
  // Example : root [1] .L ene_charge_ratio.cc+
  //           root [2] ene_charge_ratio("61516","","CIS","LBA59","12",false,"/mydir/")

  ////////////////////////////////////////////////////////////////


  TFile *file = (TFile*)gROOT->GetListOfFiles()->FindObject(filename);

  if (!file) {
    file = openFile(runno,filename,runtype);
  }


  TString w_outfile;


  if (draw_plots) {
    if (outdir=="") {
      outdir="/afs/cern.ch/user/t/tilebeam/scratch0/Commissioning/results/team5/CIS/"+module+"/";
    }

    w_outfile= outdir+"/r"+runno+"_"+module+"_amp_vs_q_chan_"+chan+".ps";
  }

  TString gain_str[2]= {"hi","lo"};
  TString cap_str[2]= {"5","100"};


  //gStyle->SetTitleFontSize(0.15);
  gStyle->SetStatFontSize(0.12);
  //  gStyle->SetLabelSize(0.14);

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
    


  //Opening the file
  std::cout << "Entering directory Tile/RawChannel/"<< module <<"\n";
  file->cd( TString("Tile/RawChannel/"+module) );

  //Creating the canvases
  TCanvas* c_cis = new TCanvas; //4*gain + 2*capacitor +(0-23 chans,24-47 chans)
  TH2S** histo = new TH2S*[4];  //(2*gain + 2*capacitor) *48 chans

      c_cis->Divide(2,2);

  for (Int_t gain=0; gain<2; ++gain) { //loop on gains: lo, hi

    for (Int_t cap=0; cap <2; ++cap) { //loop on capacitors: 5pF, 100pF


      Int_t idx = 2*gain+cap;
      std::ostringstream sStr("");
      std::ostringstream sChan("");
      sChan << std::setfill('0') << std::setw(2) << chan << setfill(' ');
      sStr << "Tile/RawChannel/" << module << "/" << module << "_pmt_" << sChan.str() << "_" << gain_str[gain] << "_amp_vs_q_" << cap_str[cap];

      //	std::cout << "Getting histo:"<< sStr.str() <<"\n";
      histo[idx] =(TH2S*)file->Get( TString(sStr.str()) );
      if ( !histo[idx] )
	{
	  std::cerr<<"Cannot find histogram named: "<<sStr.str()<<endl;
	}
      c_cis->cd(idx+1);
	
      if ( histo[idx] ) {
	if (histo[idx]->GetMaximum()>0.) {
	  gPad->SetGrid(1,1);
	}	
	histo[idx]->Draw("");
	histo[idx]->GetXaxis()->SetTitleSize(0.05);
	histo[idx]->GetYaxis()->SetTitleSize(0.05);
	histo[idx]->GetXaxis()->SetTitle(module+" ch "+sChan.str()+" "+cap_str[cap]+" pF at "+gain_str[gain]+" gain     Charge");
	histo[idx]->GetYaxis()->SetTitle("Efit");
      }
      //sys.Sleep(5000);
      //gStyle->SetOptStat(0);
      //delete histo;
    }
    
  }

  c_cis->Update(); // refresh Canvas once completely filled
  
  if (draw_plots) { // Filling ps output file
    
    c_cis->Print(w_outfile);
  }

  delete[] histo;
  return 0;
}


Int_t ene_charge_ratio(TString runno, TString filename, TString runtype, TString module, bool draw_plots=false, TString outdir="")
{
  // Example : root [1] .L ene_charge_ratio.cc+
  //           root [2] ene_charge_ratio("61516","","CIS","LBA59",false,"/mydir/")

  ////////////////////////////////////////////////////////////////

  /*   TControlBar* barMain = new TControlBar("vertical","CIS L2 plots",600,50);
   //  barMain->AddButton("Energy_Charge","ene_charge(runno,runtype,filename,module,draw_plots,outdir)","Energy/Charge Ratio");
  barMain->AddButton("&Quit","quit()","Stop and Quit");
  barMain->Show();
  gROOT->SaveContext();
  //}
  */
  //Int_t ene_charge(TString runno, TString runtype, TString filename, TString module, bool draw_plots=false, TString outdir="")
  //{
  //  TSystem sys("Gen","Gen");

  TFile *file = (TFile*)gROOT->GetListOfFiles()->FindObject(filename);

  if (!file) {
    file = openFile(runno,filename,runtype);
  }

  TString w_outfile;
  TString o_outfile;
  TString c_outfile;

  if (draw_plots) {
    if (outdir=="") {
      outdir="/afs/cern.ch/user/t/tilebeam/scratch0/Commissioning/results/team5/CIS/"+module+"/";
    }
    w_outfile= outdir+"/r"+runno+"_"+module+"_ene_charge_ratio.ps"; // ps files with multiple pages need a special treatment
    o_outfile= w_outfile+"(";              // This is the way to open it
    c_outfile= w_outfile+")";		   // This is the way to close it
  }

  TString gain_str[2]= {"hi","lo"};
  TString cap_str[2]= {"5","100"};

  TLine* ll = new TLine(0.7,0.,0.7,100.);
  TLine* rl = new TLine(1.3,0.,1.3,100.);

  ll->SetLineColor(2);
  ll->SetLineWidth(2);
  rl->SetLineColor(2);
  rl->SetLineWidth(2);

  gStyle->SetTitleFontSize(0.15);
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
    


  //Opening the file
  std::cout << "Entering directory Tile/RawChannel/"<< module <<"\n";
  file->cd( TString("Tile/RawChannel/"+module) );

  //Creating the canvases
  //AND->
  TCanvas* c_cis[8];// = new TCanvas*[8]; //4*gain + 2*capacitor +(0-23 chans,24-47 chans)
  for (int i=0;i<8;++i) c_cis[i]=new TCanvas;
  TH1S** histo = new TH1S*[4*48]; //4*gain + 2*capacitor *48 chans

  for (Int_t gain=0; gain<2; ++gain) { //loop on gains: lo, hi

    for (Int_t cap=0; cap <2; ++cap) { //loop on capacitors: 5pF, 100pF

      c_cis[4*gain+2*cap]->Divide(6,4);
      c_cis[4*gain+2*cap+1]->Divide(6,4);

      for (Int_t i=0; i<24; ++i) { // loop on chans: 0-23

	Int_t idx = 96*gain+48*cap+i;
	std::ostringstream sStr("");
	std::ostringstream sChan("");
	sChan << std::setfill('0') << std::setw(2) << i << setfill(' ');
	sStr << "Tile/RawChannel/" << module << "/" << module << "_pmt_" << sChan.str() << "_" << gain_str[gain] << "_amp_ratio_" << cap_str[cap];

	//	std::cout << "Getting histo:"<< sStr.str() <<"\n";
	histo[idx] =(TH1S*)file->Get( TString(sStr.str()) );
	if ( !histo[idx] )
	  std::cerr<<"Cannot find histogram named: "<<sStr.str()<<endl;
	c_cis[4*gain+2*cap]->cd(i+1);
	
	if ( histo[idx] ) {
	  if((histo[idx]->GetMean()+histo[idx]->GetRMS()>1.3) ||
	     (histo[idx]->GetMean()+histo[idx]->GetRMS()<0.7)) 
	    {
	      histo[idx]->SetFillColor(2);
	    }
	  else {	
	    histo[idx]->SetFillColor(30);
	  }
	  
	  if (histo[idx]->GetMaximum()>0.) {
	    gPad->SetLogy();
	    gPad->SetGrid(1,1);
	  }	
	  histo[idx]->Draw("histo");
	  histo[idx]->GetXaxis()->SetTitleSize(0.05);
	  histo[idx]->GetXaxis()->SetTitle(module+" ch "+sChan.str()+" "+cap_str[cap]+" pF at "+gain_str[gain]+" gain (Efit/Q)");
	}
	ll->Draw();
	rl->Draw();

	//sys.Sleep(5000);
	//gStyle->SetOptStat(0);
	//delete histo;
      }
      c_cis[4*gain+2*cap]->Update(); // refresh Canvas once completely filled

      if (draw_plots) { // Filling ps output file
	
	if ((4*gain+2*cap)<1) { //first canvas
	  std::cout << "First canvas\n";
	  c_cis[4*gain+2*cap]->Print(o_outfile);
	  //	  c_cis[4*gain+2*cap].Print(w_outfile);
	}
	else {
	  c_cis[4*gain+2*cap]->Print(w_outfile);
	}
      }
      for (Int_t i=24; i<48; ++i) { // loop on chans: 24-47

	Int_t idx = 96*gain+48*cap+i;
	std::ostringstream sStr("");
	std::ostringstream sChan("");
	sChan << std::setfill('0') << std::setw(2) << i << setfill(' ');
	sStr << "Tile/RawChannel/" << module << "/" << module << "_pmt_" << sChan.str() << "_" << gain_str[gain] << "_amp_ratio_" << cap_str[cap];

	//std::cout << "Getting histo:"<< sStr.str() <<"\n";
	histo[idx] =(TH1S*)file->Get( TString(sStr.str()) );
	if ( !histo[idx] )
	  std::cerr<<"Cannot find histogram named: "<<sStr.str()<<endl;
	c_cis[4*gain+2*cap+1]->cd(i-23);
	
	if ( histo[idx] ) {
	  if((histo[idx]->GetMean()+histo[idx]->GetRMS()>1.3) ||
	     (histo[idx]->GetMean()-histo[idx]->GetRMS()<0.7)) 
	    {
	      histo[idx]->SetFillColor(2);
	    }
	  else {	
	    histo[idx]->SetFillColor(30);
	  }
	  
	  if (histo[idx]->GetMaximum()>0.) {
	    gPad->SetLogy();
	    gPad->SetGrid(1,1);
	  }	
	  histo[idx]->Draw("histo");
	  histo[idx]->GetXaxis()->SetTitleSize(0.05);
	  histo[idx]->GetXaxis()->SetTitle(module+" ch "+sChan.str()+" "+cap_str[cap]+" pF at "+gain_str[gain]+" gain (Efit/Q)");
	}
	ll->Draw();
	rl->Draw();
    
      }
      c_cis[4*gain+2*cap+1]->Update();

      if (draw_plots) {

	if ((4*gain+2*cap+1)>6) 	 
	  { // last canvas

	    c_cis[4*gain+2*cap+1]->Print(c_outfile);
	    std::cout << "Last canvas\n";
	  }
	else {
	  c_cis[4*gain+2*cap+1]->Print(w_outfile);
	}	
      }
    } 	  

  }
  delete[] histo;
  return 0;
}


Int_t ene_charge_ratio(TString runno, TString filename, TString runtype, TString module, TString chan, bool draw_plots=false, TString outdir="")
{
  // Example : root [1] .L ene_charge_ratio.cc+
  //           root [2] ene_charge_ratio("61516","","CIS", "LBA59",false,"/mydir/")

  ////////////////////////////////////////////////////////////////


  TFile *file = (TFile*)gROOT->GetListOfFiles()->FindObject(filename);

  if (!file) {
    file = openFile(runno,filename,runtype);
  }


  TString w_outfile;

  if (draw_plots) {
    if (outdir=="") {
      outdir="/afs/cern.ch/user/t/tilebeam/scratch0/Commissioning/results/team5/CIS/"+module+"/";
    }
    w_outfile= outdir+"/r"+runno+"_"+module+"_ene_charge_ratio_chan_"+chan+".ps"; // ps files with multiple pages need a special treatment
  }

  TString gain_str[2]= {"hi","lo"};
  TString cap_str[2]= {"5","100"};

  TLine* ll = new TLine(0.7,0.,0.7,100.);
  TLine* rl = new TLine(1.3,0.,1.3,100.);

  ll->SetLineColor(2);
  ll->SetLineWidth(2);
  rl->SetLineColor(2);
  rl->SetLineWidth(2);

  gStyle->SetTitleFontSize(0.15);
  gStyle->SetStatFontSize(0.12);
  //  gStyle->SetLabelSize(0.14);

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
    


  //Opening the file
  std::cout << "Entering directory Tile/RawChannel/"<< module <<"\n";
  file->cd( TString("Tile/RawChannel/"+module) );

  //Creating the canvases
  TCanvas* c_cis = new TCanvas; //4*gain + 2*capacitor +(0-23 chans,24-47 chans)
  TH1S** histo = new TH1S*[4]; //4*gain + 2*capacitor *48 chans

      c_cis->Divide(2,2);

  for (Int_t gain=0; gain<2; ++gain) { //loop on gains: lo, hi

    for (Int_t cap=0; cap <2; ++cap) { //loop on capacitors: 5pF, 100pF

	Int_t idx = 2*gain+cap;
	std::ostringstream sStr("");
	std::ostringstream sChan("");
	sChan << std::setfill('0') << std::setw(2) << chan << setfill(' ');
	sStr << "Tile/RawChannel/" << module << "/" << module << "_pmt_" << sChan.str() << "_" << gain_str[gain] << "_amp_ratio_" << cap_str[cap];

	//	std::cout << "Getting histo:"<< sStr.str() <<"\n";
	histo[idx] =(TH1S*)file->Get( TString(sStr.str()) );
	if ( !histo[idx] )
	  std::cerr<<"Cannot find histogram named: "<<sStr.str()<<endl;
	c_cis->cd(idx+1);
	
	if ( histo[idx] ) {
	  if((histo[idx]->GetMean()+histo[idx]->GetRMS()>1.3) ||
	     (histo[idx]->GetMean()+histo[idx]->GetRMS()<0.7)) 
	    {
	      histo[idx]->SetFillColor(2);
	    }
	  else {	
	    histo[idx]->SetFillColor(30);
	  }
	  
	  if (histo[idx]->GetMaximum()>0.) {
	    gPad->SetLogy();
	    gPad->SetGrid(1,1);
	  }	
	  histo[idx]->Draw("histo");
	  histo[idx]->GetXaxis()->SetTitleSize(0.05);
	  histo[idx]->GetXaxis()->SetTitle(module+" ch "+sChan.str()+" "+cap_str[cap]+" pF at "+gain_str[gain]+" gain (Efit/Q)");
	}
	ll->Draw();
	rl->Draw();
    }
  }
	//sys.Sleep(5000);
	//gStyle->SetOptStat(0);
	//delete histo;
  c_cis->Update(); // refresh Canvas once completely filled

  if (draw_plots) { // Filling ps output file
    
    c_cis->Print(w_outfile);
  }

  delete[] histo;
  return 0;
}

Int_t time_vs_phase(TString runno,  TString filename, TString runtype, TString module, bool draw_plots=false, TString outdir="")
{
  // Example : root [1] .L ene_charge_ratio.cc+
  //           root [2] ene_charge_ratio("61516","","CIS","LBA59",false,"/mydir/")

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
      outdir="/afs/cern.ch/user/t/tilebeam/scratch0/Commissioning/results/team5/CIS/"+module+"/";
    }
    w_outfile= outdir+"/r"+runno+"_"+module+"_time_vs_phase.ps"; // ps files with multiple pages need a special treatment
    o_outfile= w_outfile+"(";              // This is the way to open it
    c_outfile= w_outfile+")";		   // This is the way to close it
  }

  TString gain_str[2]= {"hi","lo"}; // string to get high or low gain histograms
  TString cap_str[2]= {"5","100"};  // string to get 5 or 100 pF capacitor


  //gStyle->SetTitleFontSize(0.15);
  //  gStyle->SetStatFontSize(0.12);
  //  gStyle->SetLabelSize(0.14);
  gStyle->SetOptStat(0);


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
    


  //Opening the file
  std::cout << "Entering directory Tile/RawChannel/"<< module <<"\n";
  file->cd( TString("Tile/RawChannel/"+module) );

  //Creating the canvases
  //AND->
  TCanvas* c_cis[8];// = new TCanvas[8]; //4*gain + 2*capacitor +(0-23 chans,24-47 chans)
  for (int i = 0 ;i<8;i++) c_cis[i]=new TCanvas;
  TH2S** histo = new TH2S*[4*48];  //(2*gain + 2*capacitor) *48 chans


  for (Int_t gain=0; gain<2; ++gain) { //loop on gains: lo, hi

    for (Int_t cap=0; cap <2; ++cap) { //loop on capacitors: 5pF, 100pF

      c_cis[4*gain+2*cap]->Divide(6,4); //24 histogrmas for each canvas
      c_cis[4*gain+2*cap+1]->Divide(6,4);

      for (Int_t i=0; i<24; ++i) { // loop on chans: 0-23

	Int_t idx = 96*gain+48*cap+i; // histogram index
	std::ostringstream sStr("");
	std::ostringstream sChan("");
	sChan << std::setfill('0') << std::setw(2) << i << setfill(' ');
	sStr << "Tile/RawChannel/" << module << "/" << module << "_pmt_" << sChan.str() << "_" << gain_str[gain] << "_time_vs_time_" << cap_str[cap]; // histogram name

	//	std::cout << "Getting histo:"<< sStr.str() <<"\n";
	histo[idx] =(TH2S*)file->Get( TString(sStr.str()) );
	if ( !histo[idx] )
	  std::cerr<<"Cannot find histogram named: "<<sStr.str()<<endl;
	c_cis[4*gain+2*cap]->cd(i+1);  // entering the canvas
	
	if ( histo[idx] ) {
	  if (histo[idx]->GetMaximum()>0.) {
	    gPad->SetGrid(1,1);
	  }	
	  histo[idx]->Draw("");
	  histo[idx]->GetXaxis()->SetTitleSize(0.05);
	  histo[idx]->GetYaxis()->SetTitleSize(0.05);
	  histo[idx]->GetXaxis()->SetTitle(module+" ch "+sChan.str()+" "+cap_str[cap]+" pF at "+gain_str[gain]+" gain   CIS Phase");
	  histo[idx]->GetYaxis()->SetTitle("Time (ns)");
	}
	//sys.Sleep(5000);
	//gStyle->SetOptStat(0);
	//delete histo;
      }
      c_cis[4*gain+2*cap]->Update(); // refresh Canvas once completely filled

      if (draw_plots) { // Filling ps output file

	if ((4*gain+2*cap)<1) { //first canvas
	  std::cout << "First canvas\n";
	  c_cis[4*gain+2*cap]->Print(o_outfile);
	  //	  c_cis[4*gain+2*cap].Print(w_outfile);
	}
	else {
	  c_cis[4*gain+2*cap]->Print(w_outfile);
	}

      }


      for (Int_t i=24; i<48; ++i) { // loop on chans: 24-47

	Int_t idx = 96*gain+48*cap+i; // histogram index
	std::ostringstream sStr("");
	std::ostringstream sChan("");
	sChan << std::setfill('0') << std::setw(2) << i << setfill(' ');
	sStr << "Tile/RawChannel/" << module << "/" << module << "_pmt_" << sChan.str() << "_" << gain_str[gain] << "_time_vs_time_" << cap_str[cap]; // histogram name

	//std::cout << "Getting histo:"<< sStr.str() <<"\n";
	histo[idx] =(TH2S*)file->Get( TString(sStr.str()) );
	if ( !histo[idx] )
	  std::cerr<<"Cannot find histogram named: "<<sStr.str()<<endl;
	c_cis[4*gain+2*cap+1]->cd(i-23);
	
	if ( histo[idx] ) {
	  if (histo[idx]->GetMaximum()>0.) {
	    gPad->SetGrid(1,1);
	  }	
	  histo[idx]->Draw("");
	  histo[idx]->GetXaxis()->SetTitleSize(0.05);
	  histo[idx]->GetYaxis()->SetTitleSize(0.05);
	  histo[idx]->GetXaxis()->SetTitle(module+" ch "+sChan.str()+" "+cap_str[cap]+" pF at "+gain_str[gain]+" gain   CIS Phase");
	  histo[idx]->GetYaxis()->SetTitle("Time (ns)");
	}
      }
      c_cis[4*gain+2*cap+1]->Update(); // refresh the canvas once completely filled

      if (draw_plots) {

	if ((4*gain+2*cap+1)>6) 	 { // last canvas

	  //	  c_cis[4*gain+2*cap+1].Print(w_outfile);
	  c_cis[4*gain+2*cap+1]->Print(c_outfile);
	  std::cout << "Last canvas\n";
	}
	else {
	  c_cis[4*gain+2*cap+1]->Print(w_outfile);
	}

      }

    }

  }
  delete[] histo;
  return 0;
}

//////////////////////////////////////////////////////////////////
// Plots for a single channel
Int_t time_vs_phase(TString runno, TString filename, TString runtype, TString module, TString chan, bool draw_plots=false, TString outdir="")
{
  // Example : root [1] .L time_vs_phase_ratio.cc+
  //           root [2] time_vs_phase("61516","","CIS","LBA59","12",false,"/mydir/")

//////////////////////////////////////////////////////////////////


  TFile *file = (TFile*)gROOT->GetListOfFiles()->FindObject(filename);

  if (!file) {
    file = openFile(runno,filename,runtype);
  }


  TString w_outfile;
   
  if (draw_plots) {
     if (outdir=="") {
      outdir="/afs/cern.ch/user/t/tilebeam/scratch0/Commissioning/results/team5/CIS/"+module+"/";
     }
    TString w_outfile= outdir+"/r"+runno+"_"+module+"_time_vs_phase_chan_"+chan+".ps";
  }

  TString gain_str[2]= {"hi","lo"}; // string to get high or low gain histograms
  TString cap_str[2]= {"5","100"};  // string to get 5 or 100 pF capacitor


  //gStyle->SetTitleFontSize(0.15);
  //gStyle->SetStatFontSize(0.12);
  //  gStyle->SetLabelSize(0.14);
  gStyle->SetOptStat(0);

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
    


  //Opening the file
  std::cout << "Entering directory Tile/RawChannel/"<< module <<"\n";
  file->cd( TString("Tile/RawChannel/"+module) );

  //Creating the canvases
  TCanvas* c_cis = new TCanvas; 
  TH2S** histo = new TH2S*[4]; //2*gain + 2*capacitor

  c_cis->Divide(2,2); //4 histograms 

  for (Int_t gain=0; gain<2; ++gain) { //loop on gains: lo, hi

    for (Int_t cap=0; cap <2; ++cap) { //loop on capacitors: 5pF, 100pF

	Int_t idx = 2*gain+cap; // histogram index
	std::ostringstream sStr("");
	std::ostringstream sChan("");
	sChan << std::setfill('0') << std::setw(2) << chan << setfill(' ');
	sStr << "Tile/RawChannel/" << module << "/" << module << "_pmt_" << sChan.str() << "_" << gain_str[gain] << "_time_vs_time_" << cap_str[cap]; // histogram name

	//	std::cout << "Getting histo:"<< sStr.str() <<"\n";
	histo[idx] =(TH2S*)file->Get( TString(sStr.str()) );
	if ( !histo[idx] )
	  std::cerr<<"Cannot find histogram named: "<<sStr.str()<<endl;
	c_cis->cd(idx+1);  // entering the canvas
	
	if ( histo[idx] ) {
	  if (histo[idx]->GetMaximum()>0.) {
	    gPad->SetGrid(1,1);
	  }	
	  histo[idx]->Draw("");
	  histo[idx]->GetXaxis()->SetTitleSize(0.05);
	  histo[idx]->GetYaxis()->SetTitleSize(0.05);
	  histo[idx]->GetXaxis()->SetTitle(module+" ch "+sChan.str()+" "+cap_str[cap]+" pF at "+gain_str[gain]+" gain   CIS Phase");
	  histo[idx]->GetYaxis()->SetTitle("Time (ns)");
	}
	  //sys.Sleep(5000);
	//gStyle->SetOptStat(0);
	//delete histo;
    }

  }

  c_cis->Update(); // refresh Canvas once completely filled
  
  if (draw_plots) { // Filling ps output file
    c_cis->Print(w_outfile);
  }

  delete[] histo;
  return 0;
}
