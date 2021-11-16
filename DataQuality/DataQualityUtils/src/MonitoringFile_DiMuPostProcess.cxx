/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "DataQualityUtils/MonitoringFile.h"

#include <cmath>
#include <vector>
#include <sstream>

#include <TCanvas.h>
#include <TF1.h>
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TKey.h>
#include <TMath.h>
#include <TProfile.h>
#include <TTree.h>
#include <TROOT.h>

#include "RooRealVar.h"
#include "RooDataHist.h"
#include "RooArgList.h"
#include "RooPlot.h"
#include "RooBreitWigner.h"
#include "RooCBShape.h"
#include "RooFFTConvPdf.h"
#include "RooGlobalFunc.h"

namespace dqutils {

void 
MonitoringFile::
fitMergedFile_DiMuMonManager( const std::string & inFilename, bool isIncremental)
{ //adapted from MonitoringFile_IDPerfPostProcess.cxx

  if (isIncremental==true) return;

  TFile* f = TFile::Open(inFilename.c_str(),"UPDATE");
  if (f == 0 || ! f->IsOpen()) {
    //std::cout<<"Failed to load or open file!"<<endl;
    delete f;
    return;
  }
  if(f->GetSize()<1000.) {
    //std::cout<<"File is empty!"<<endl;
    delete f;
    return; 
  }
  std::string run_dir;
  TIter next_run ( f->GetListOfKeys() );
  TKey* key_run(0);
  while ((key_run = dynamic_cast<TKey*>( next_run() )) !=0 ) {
    TObject* obj_run = key_run->ReadObj();
    TDirectory* tdir_run = dynamic_cast<TDirectory*>( obj_run );
    if (tdir_run !=0 ) {
      std::string tdir_run_name( tdir_run->GetName() );
      if (tdir_run_name.find("run")!= std::string::npos ) {
        run_dir=tdir_run_name;
        TIter next_perf(tdir_run->GetListOfKeys());
        TKey* key_perf(0);
        while ((key_perf=dynamic_cast<TKey*>( next_perf() ))!=0 ) {
          std::string perf_name(key_perf->GetName());
          if (perf_name.find("DiMuMon")!=std::string::npos){
	    TObject* obj_perf = key_perf->ReadObj();
	    TDirectory* tdir_perf = dynamic_cast<TDirectory*>( obj_perf );
	    if (tdir_perf !=0) { 
	      run_dir=run_dir+'/';
	      TIter next_module(tdir_perf->GetListOfKeys());
	      TKey* key_module(0);
	      while ((key_module=dynamic_cast<TKey*>(next_module() )) !=0) {
		std::string module_name(key_module->GetName());
		if (module_name.find("Jpsi")!=std::string::npos) {
		  processModule(f, run_dir, key_module,"Jpsi");
		} else if (module_name.find("Zmumu")!=std::string::npos) {
		  processModule(f, run_dir, key_module,"Zmumu");
		}
	      }
	    } else {
	      delete obj_perf;
	    }
	  }
	}
      }
      
      // if without top run_directory
      else if (tdir_run_name.find("DiMuMon")!= std::string::npos ) {
	TObject* obj_perf = key_run->ReadObj();
	TDirectory* tdir_perf = dynamic_cast<TDirectory*>( obj_perf );
	if (tdir_perf !=0) { 
	  run_dir='/';
	  TIter next_module(tdir_perf->GetListOfKeys());
	  TKey* key_module(0);
	  while ((key_module=dynamic_cast<TKey*>(next_module() )) !=0) {
	    std::string module_name(key_module->GetName());
	    if (module_name.find("Jpsi")!=std::string::npos) {
	      processModule(f, run_dir, key_module,"Jpsi");
	    } else if (module_name.find("Zmumu")!=std::string::npos) {
	      processModule(f, run_dir, key_module,"Zmumu");
	    }
	  }
	} else {
	  delete obj_perf;
	}         
      }
    } else {
      delete obj_run;
    }
  }
  
  f->Close();
  delete f;
  //  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "DiMuPostProcessing done!" <<endmsg;
}
  
void MonitoringFile::processModule( TFile* f, const std::string & run_dir, TKey* key_module, const std::string & moduleName){
  TObject *obj_mod = key_module->ReadObj();
  TDirectory *tdir_mod = dynamic_cast<TDirectory*>(obj_mod);
  if ( tdir_mod !=0 ) {
      TIter next_trigger(tdir_mod->GetListOfKeys());
      TKey* key_trigger(0);
      while((key_trigger=dynamic_cast<TKey*>(next_trigger()))!=0) {
	std::string triggerName=key_trigger->GetName();
	fitMergedFile_DiMuMonAll( f,run_dir,moduleName,triggerName );
      }
  } else {
    delete obj_mod;
  }  
}


void
MonitoringFile::fitMergedFile_DiMuMonAll( TFile* f, const std::string & run_dir, const std::string & resonName, const std::string & triggerName )
{ 
  //std::cout<<"fitMergedFile_DiMuMon has been called"<<endl;
  std::string path;
  path= run_dir+"DiMuMon/"+resonName+"/"+triggerName;

  if( f->cd(path.c_str())==0 ) {
    return;
  }
  std::vector<std::string> regions;
  regions.push_back("All");
  regions.push_back("BB");
  regions.push_back("EAEA");
  regions.push_back("ECEC");
  std::vector<std::string> vars;
  vars.push_back("eta");
  vars.push_back("etaAll");
  vars.push_back("etaPos");
  vars.push_back("etaNeg");
  vars.push_back("phi");
  vars.push_back("phiAll");
  vars.push_back("phiPos");
  vars.push_back("phiNeg");
  vars.push_back("pt");
  vars.push_back("ptAll");
  vars.push_back("ptPos");
  vars.push_back("ptNeg");
  vars.push_back("etaDiff");
  vars.push_back("etaSumm");
  vars.push_back("phiDiff");
  vars.push_back("phiSumm");
  vars.push_back("crtDiff");
  
  std::map< std::string, TH1F* > h_invmass;
  std::map< std::string, std::map< std::string, TH2F*> > h_2DinvmassVSx;
  std::map< std::string, std::map< std::string, TH1F*> > h_invmassVSx;
  std::map< std::string, std::map< std::string, TH1F*> > h_widthVSx;
  TH1F* h_chi2;

  //loop over all possible 2D histos
  //check if 2D histo has been filled
  //if found the 2D histo, then see whether the mean or width or both 1D histos were also made.-->Decide what to refit `
  if (CheckHistogram(f,(path+"_detail/chi2").c_str())) {
    h_chi2 = (TH1F*)(f->Get((path+"_detail/chi2").c_str())->Clone());
    std::vector<std::string> ::iterator ivar = vars.begin();
    std::vector<std::string> ::iterator ireg = regions.begin();
    for (ireg=regions.begin(); ireg!=regions.end(); ++ireg) {
      for (ivar=vars.begin(); ivar!=vars.end(); ++ivar) {
	std::string hname2D = resonName + "_2DinvmassVS" + *ivar + "_" + *ireg;
	if (CheckHistogram(f,(path+"/"+hname2D).c_str())) {
	  h_2DinvmassVSx[*ireg][*ivar] = (TH2F*)(f->Get((path+"/"+hname2D).c_str())->Clone());
	  std::string hnameMean = resonName + "_invmassVS" + *ivar + "_" + *ireg;
	  std::string hnameWidth = resonName + "_widthVS" + *ivar + "_" + *ireg;
	  std::vector<TH1F*> hfitted;
	  if (CheckHistogram(f,(path+"/"+hnameMean).c_str())) {
	    h_invmassVSx[*ireg][*ivar] = (TH1F*)(f->Get((path+"/"+hnameMean).c_str())->Clone());
	    hfitted.push_back(h_invmassVSx[*ireg][*ivar] );
	    if (CheckHistogram(f,(path+"_detail/"+hnameWidth).c_str())) {
	      h_widthVSx[*ireg][*ivar] = (TH1F*)(f->Get((path+"_detail/"+hnameWidth).c_str())->Clone());
	      hfitted.push_back(h_widthVSx[*ireg][*ivar] );
	      fitHistos(h_2DinvmassVSx[*ireg][*ivar], hfitted, 0, triggerName, resonName, h_chi2);// 0 means to fill both mean and width results from the fit
	      f->cd((path+"/").c_str());
	      h_invmassVSx[*ireg][*ivar]->Write("",TObject::kOverwrite);
	      f->cd((path+"_detail/").c_str());
	      h_widthVSx[*ireg][*ivar]->Write("",TObject::kOverwrite);	 	 
	    } else {
	      fitHistos(h_2DinvmassVSx[*ireg][*ivar], hfitted, 1, triggerName, resonName, h_chi2);// 1 means to fill only mean results from the fit	 
	      f->cd((path+"/").c_str());
	      h_invmassVSx[*ireg][*ivar]->Write("",TObject::kOverwrite);	 
	    } 
	  } else {
	    if (CheckHistogram(f,(path+"_detail/"+hnameWidth).c_str())) {
	      h_widthVSx[*ireg][*ivar] = (TH1F*)(f->Get((path+"_detail/"+hnameWidth).c_str())->Clone());
	      hfitted.push_back(h_widthVSx[*ireg][*ivar] );
	      fitHistos(h_2DinvmassVSx[*ireg][*ivar], hfitted, 2, triggerName, resonName, h_chi2);// 2 means to fill only width results from the fit	 
	      f->cd((path+"_detail/").c_str());
	      h_widthVSx[*ireg][*ivar]->Write((path+"_detail/"+hnameWidth).c_str(),TObject::kOverwrite);	 	 
	    }
	  }
	  
	  
	}
	
      }
    }
    h_chi2->Write("",TObject::kOverwrite);	 	 
  }
  f->Write();
}

void MonitoringFile::fitHistos (TH2F* hin, std::vector<TH1F*> hout, int mode, const std::string & triggerName, const std::string & resonName, TH1F* h_chi2){
  bool saveHistos = false;
  // a canvas may be needed when implmenting this into the post-processing file 
  //std::cout<<"The fitHistos method is called"<<endl;
  std::string hname =  hin->GetName();
  TCanvas* ctemp;
  char num2str[50];
  int nbins=hin->GetNbinsX();
  for (int i=0; i<nbins;i++){
    snprintf(num2str,50,"%s_%i",(hname).c_str(),i);
    TH1F* htemp = (TH1F*) (hin->ProjectionY(num2str,i+1,i+1));
    //htemp->SetTitle(projName);
    htemp->Sumw2();
    if (htemp->GetEntries()>50){
      double mean = 999., meanErr = 999., sigma = 999., sigmaErr = 999., chi2 = 0.;
      if (resonName == "Jpsi" || resonName == "Upsi"){
	mean = htemp->GetMean();
	sigma= htemp->GetRMS();
	TF1* fn = new TF1("fn","gaus",mean-2*sigma,mean+2*sigma);
	fn->SetParameters(htemp->GetEntries()/4,mean,sigma);
	htemp->Fit("fn","RQMN");
	mean = fn->GetParameter(1);
	sigma= fn->GetParameter(2);
	fn->SetRange(mean-1.2*sigma,mean+1.2*sigma);
     
	if (saveHistos==false) {
	  htemp->Fit("fn","RQMN");
	} else {
	  ctemp= new TCanvas("ctemp","ctemp",500,500);
	  TString psName = num2str + triggerName + ".ps";
	  htemp->Fit("fn","RQM");
	  ctemp->Print(psName);
	  delete ctemp;
	}
	double frange = 2.4*sigma;
	double hrange = htemp->GetXaxis()->GetXmax()-htemp->GetXaxis()->GetXmin();
	double ndf = frange/hrange*(htemp->GetNbinsX()) - 3;//subtract number of fit parameters
	//fill results ;
	mean = fn->GetParameter(1);
	meanErr = fn->GetParError(1);
	sigma = fn->GetParameter(2);
	sigmaErr = fn->GetParError(2);
	chi2 = (fn->GetChisquare())/ndf;
	delete fn;
      } else {
	//fit Z peak with a convolution of BreitWigner and Crystal Ball fns, fit by Louise, implementation by Jike taken from IDPerfMon
	RooMsgService::instance().setGlobalKillBelow(RooFit::FATAL);
	RooAbsPdf::verboseEval(-100); //sami in order to make roofit quiet
	RooRealVar  m("mass", "dimuon invariable mass", 91.2, 71., 111., "GeV");
	RooDataHist *data = 0;
	data = new RooDataHist("data", "data", m, htemp);
	RooRealVar bwm0("bw_#mu",   "bw_#mu",   91.2, 85.2, 97.2) ;
	RooRealVar bwsg("bw_#sigma","bw_#sigma", 2.4952) ;
	RooBreitWigner bw("bw","bw", m, bwm0, bwsg);
	
	RooRealVar cbm0("cb_#mu",    "cb_#mu",  0  ) ;
	RooRealVar cbsg("cb_#sigma", "cb_#sigma", 3.,  1.,  10.) ;
	RooRealVar cbal("cb_#alpha", "cb_#alpha",2.0) ;
	RooRealVar cbn( "cb_n",      "cb_n",      1., 0.05, 3.) ;
	RooCBShape cb(  "cb",        "cb", m, cbm0, cbsg, cbal, cbn);
	
	m.setBins(5000);
	RooFFTConvPdf bxc("bxc", "BW (X) CB", m, bw, cb) ;
	bxc.fitTo(*data, RooFit::PrintLevel(-1), RooFit::FitOptions("QN"), RooFit::PrintEvalErrors(-1), RooFit::Warnings(kFALSE));
        RooPlot* frame;
	if (saveHistos==true){
	  frame= m.frame();
	  data->plotOn(frame, RooFit::MarkerSize(0.9));
	  bxc.paramOn(frame,  RooFit::Format("NELU", RooFit::AutoPrecision(2)), RooFit::Layout(0.1,0.4,0.9));
	  bxc.plotOn (frame,  RooFit::LineColor(kBlue));
	  delete frame;
	}
	mean = bwm0.getVal();
	meanErr = bwm0.getError();
	sigma = cbsg.getVal();
	sigmaErr = cbsg.getError();
	delete data;	
      }
      //fill results
      if (mode == 0){//plot both invmass and width vs variable
	hout.at(0)->SetBinContent(i+1,mean);
	hout.at(0)->SetBinError(i+1,meanErr);
	hout.at(1)->SetBinContent(i+1,sigma);
	hout.at(1)->SetBinError(i+1,sigmaErr);
      } else if (mode==1){//plot only invmass vs variable
	hout.at(0)->SetBinContent(i+1,mean);
	hout.at(0)->SetBinError(i+1,meanErr);
      } else if (mode==2){//plot only width vs variable
	hout.at(0)->SetBinContent(i+1,sigma);
	hout.at(0)->SetBinError(i+1,sigmaErr);
      }
      h_chi2->Fill(chi2);
    }// more than 50 events
      
    delete htemp;
  }
}


}

