/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// **********************************************************************
// $Id: MonitoringFile_IDPerfPostProcess.cxx,v 1.2 2009-04-16 17:49:23 weina Exp $
// **********************************************************************

#include "DataQualityUtils/MonitoringFile.h"

#include <cmath>
#include <vector>

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
#include "RooPlot.h"

namespace dqutils {

void 
MonitoringFile::
fitMergedFile_IDPerfMonManager( std::string inFilename, bool /* isIncremental */ )
{
  //std::cout << "\n";
  //std::cout << "Running Inner-Detector performance-monitoring analysis\n";
  //std::cout << "\nWarning messages from fitting and histogram updating follow:\n\n";
  
  TFile* f = TFile::Open(inFilename.c_str(),"UPDATE");
  if (f == 0 || ! f->IsOpen()) {
    //std::cerr << "MonitoringFile::fitMergedFile_IDPerfMonManager(): "
	      //<< "Input file not opened\n";
    delete f;
    return;
  }
  if(f->GetSize()<1000.) {
    //std::cerr << "MonitoringFile::fitMergedFile_IDPerfMonManager(): "
	      //<< "MonitoringFile empty\n";
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
	// std::cout<<"Run_directory: "<<run_dir<<std::endl;
        TIter next_perf(tdir_run->GetListOfKeys());
        TKey* key_perf(0);
        while ((key_perf=dynamic_cast<TKey*>( next_perf() ))!=0 ) {
          std::string perf_name(key_perf->GetName());
	  // for hist file with run_directory
          if (perf_name.find("IDPerfMon")!=std::string::npos){
	    TObject* obj_perf = key_perf->ReadObj();
	    TDirectory* tdir_perf = dynamic_cast<TDirectory*>( obj_perf );
	    if (tdir_perf !=0) { 
	      run_dir=run_dir+'/';
	      TIter next_module(tdir_perf->GetListOfKeys());
	      TKey* key_module(0);
	      while ((key_module=dynamic_cast<TKey*>(next_module() )) !=0) {
		std::string module_name(key_module->GetName());
		if (module_name.find("Kshort")!=std::string::npos) {
		  TObject *obj_Kshort = key_module->ReadObj();
		  TDirectory *tdir_Kshort = dynamic_cast<TDirectory*>(obj_Kshort);
		  if ( tdir_Kshort !=0 ) {
		    TIter next_trigger(tdir_Kshort->GetListOfKeys());
		    TKey* key_trigger(0);
		    while((key_trigger=dynamic_cast<TKey*>(next_trigger()))!=0) {
		      std::string TriggerName=key_trigger->GetName();
		      // std::cout<<"Find Module: "<<module_name<<" Trigger: "<<TriggerName<<std::endl;
		      fitMergedFile_IDPerfMonKshort( f,run_dir,TriggerName );
		    }
		  } else {
		    delete obj_Kshort;
		  }
		}

		if (module_name.find("Jpsi")!=std::string::npos) {
		  TObject *obj_Jpsi = key_module->ReadObj();
		  TDirectory *tdir_Jpsi = dynamic_cast<TDirectory*>(obj_Jpsi);
		  if ( tdir_Jpsi !=0 ) {
		    TIter next_trigger(tdir_Jpsi->GetListOfKeys());
		    TKey* key_trigger(0);
		    while((key_trigger=dynamic_cast<TKey*>(next_trigger()))!=0) {
		      std::string TriggerName=key_trigger->GetName();
		      // std::cout<<"Find Module: "<<module_name<<" Trigger: "<<TriggerName<<std::endl;
		      fitMergedFile_IDPerfMonJpsi( f,run_dir,TriggerName );
		    }
		  } else {
		    delete obj_Jpsi;
		  }
		}
		if (module_name.find("Upsilon")!=std::string::npos) {
		  TObject *obj_Upsilon = key_module->ReadObj();
		  TDirectory *tdir_Upsilon = dynamic_cast<TDirectory*>(obj_Upsilon);
		  if ( tdir_Upsilon !=0 ) {
		    TIter next_trigger(tdir_Upsilon->GetListOfKeys());
		    TKey* key_trigger(0);
		    while((key_trigger=dynamic_cast<TKey*>(next_trigger()))!=0) {
		      std::string TriggerName=key_trigger->GetName();
		      // std::cout<<"Find Module: "<<module_name<<" Trigger: "<<TriggerName<<std::endl;
		      fitMergedFile_IDPerfMonUpsilon( f,run_dir,TriggerName );
		    }
		  } else {
		    delete obj_Upsilon;
		  }
		}
		if (module_name.find("Zee")!=std::string::npos) {
		  TObject *obj_Zee = key_module->ReadObj();
		  TDirectory *tdir_Zee = dynamic_cast<TDirectory*>(obj_Zee);
		  if ( tdir_Zee !=0 ) {
		    TIter next_trigger(tdir_Zee->GetListOfKeys());
		    TKey* key_trigger(0);
		    while((key_trigger=dynamic_cast<TKey*>(next_trigger()))!=0) {
		      std::string TriggerName=key_trigger->GetName();
		      // std::cout<<"Find Module: "<<module_name<<" Trigger: "<<TriggerName<<std::endl;
		      fitMergedFile_IDPerfMonZee( f,run_dir,TriggerName );
		    }
		  } else {
		    delete obj_Zee;
		  }
		}
		if (module_name.find("Wenu")!=std::string::npos) {
		  TObject *obj_Wenu = key_module->ReadObj();
		  TDirectory *tdir_Wenu = dynamic_cast<TDirectory*>(obj_Wenu);
		  if ( tdir_Wenu !=0 ) {
		    TIter next_trigger(tdir_Wenu->GetListOfKeys());
		    TKey* key_trigger(0);
		    while((key_trigger=dynamic_cast<TKey*>(next_trigger()))!=0) {
		      std::string TriggerName=key_trigger->GetName();
		      // std::cout<<"Find Module: "<<module_name<<" Trigger: "<<TriggerName<<std::endl;
		      fitMergedFile_IDPerfMonWenu( f,run_dir,TriggerName );
		    }
		  } else {
		    delete obj_Wenu;
		  }
		}
		if (module_name.find("ZMM")!=std::string::npos) {
		  TObject *obj_ZMM = key_module->ReadObj();
		  TDirectory *tdir_ZMM = dynamic_cast<TDirectory*>(obj_ZMM);
		  if ( tdir_ZMM !=0 ) {
		    TIter next_trigger(tdir_ZMM->GetListOfKeys());
		    TKey* key_trigger(0);
		    while((key_trigger=dynamic_cast<TKey*>(next_trigger()))!=0) {
		      std::string TriggerName=key_trigger->GetName();
		      // std::cout<<"Find Module: "<<module_name<<" Trigger: "<<TriggerName<<std::endl;
		      fitMergedFile_IDPerfMonZMM( f,run_dir,TriggerName );
		    }
		  } else {
		    delete obj_ZMM;
		  }
		}
	      }
	    } else {
	      delete obj_perf;
	    }
	  }
	}
      }
      
      // if without top run_directory
      else if (tdir_run_name.find("IDPerfMon")!= std::string::npos ) {
 	//std::cout<<"without run_directory"<<std::endl;
	TObject* obj_perf = key_run->ReadObj();
	TDirectory* tdir_perf = dynamic_cast<TDirectory*>( obj_perf );
	if (tdir_perf !=0) { 
	  run_dir='/';
	  TIter next_module(tdir_perf->GetListOfKeys());
	  TKey* key_module(0);
	  while ((key_module=dynamic_cast<TKey*>(next_module() )) !=0) {
	    std::string module_name(key_module->GetName());
	    if (module_name.find("Kshort")!=std::string::npos) {
	      TObject *obj_Kshort = key_module->ReadObj();
	      TDirectory *tdir_Kshort = dynamic_cast<TDirectory*>(obj_Kshort);
	      if ( tdir_Kshort !=0 ) {
		TIter next_trigger(tdir_Kshort->GetListOfKeys());
		TKey* key_trigger(0);
		while((key_trigger=dynamic_cast<TKey*>(next_trigger()))!=0) {
		  std::string TriggerName=key_trigger->GetName();
		  // std::cout<<"Find Module: "<<module_name<<" Trigger: "<<TriggerName<<std::endl;
		  fitMergedFile_IDPerfMonKshort( f,run_dir,TriggerName );
		}
	      } else {
	      delete obj_Kshort;
	      }
	    }
	    if (module_name.find("Jpsi")!=std::string::npos) {
	      TObject *obj_Jpsi = key_module->ReadObj();
	      TDirectory *tdir_Jpsi = dynamic_cast<TDirectory*>(obj_Jpsi);
	      if ( tdir_Jpsi !=0 ) {
		TIter next_trigger(tdir_Jpsi->GetListOfKeys());
		TKey* key_trigger(0);
		while((key_trigger=dynamic_cast<TKey*>(next_trigger()))!=0) {
		  std::string TriggerName=key_trigger->GetName();
		  // std::cout<<"Find Module: "<<module_name<<" Trigger: "<<TriggerName<<std::endl;
		  fitMergedFile_IDPerfMonJpsi( f,run_dir,TriggerName );
		}
	      } else {
		delete obj_Jpsi;
	      }
	    }
	    if (module_name.find("Upsilon")!=std::string::npos) {
	      TObject *obj_Upsilon = key_module->ReadObj();
	      TDirectory *tdir_Upsilon = dynamic_cast<TDirectory*>(obj_Upsilon);
	      if ( tdir_Upsilon !=0 ) {
		TIter next_trigger(tdir_Upsilon->GetListOfKeys());
		TKey* key_trigger(0);
		while((key_trigger=dynamic_cast<TKey*>(next_trigger()))!=0) {
		  std::string TriggerName=key_trigger->GetName();
		  // std::cout<<"Find Module: "<<module_name<<" Trigger: "<<TriggerName<<std::endl;
		  fitMergedFile_IDPerfMonUpsilon( f,run_dir,TriggerName );
		}
	      } else {
		delete obj_Upsilon;
	      }
	    }
	    if (module_name.find("Zee")!=std::string::npos) {
	      TObject *obj_Zee = key_module->ReadObj();
	      TDirectory *tdir_Zee = dynamic_cast<TDirectory*>(obj_Zee);
	      if ( tdir_Zee !=0 ) {
		TIter next_trigger(tdir_Zee->GetListOfKeys());
		TKey* key_trigger(0);
		while((key_trigger=dynamic_cast<TKey*>(next_trigger()))!=0) {
		  std::string TriggerName=key_trigger->GetName();
		  // std::cout<<"Find Module: "<<module_name<<" Trigger: "<<TriggerName<<std::endl;
		  fitMergedFile_IDPerfMonZee( f,run_dir,TriggerName );
		}
	      } else {
		delete obj_Zee;
	      }
	    }
	    if (module_name.find("Wenu")!=std::string::npos) {
	      TObject *obj_Wenu = key_module->ReadObj();
	      TDirectory *tdir_Wenu = dynamic_cast<TDirectory*>(obj_Wenu);
	      if ( tdir_Wenu !=0 ) {
		TIter next_trigger(tdir_Wenu->GetListOfKeys());
		TKey* key_trigger(0);
		while((key_trigger=dynamic_cast<TKey*>(next_trigger()))!=0) {
		  std::string TriggerName=key_trigger->GetName();
		  // std::cout<<"Find Module: "<<module_name<<" Trigger: "<<TriggerName<<std::endl;
		  fitMergedFile_IDPerfMonWenu( f,run_dir,TriggerName );
		}
	      } else {
		delete obj_Wenu;
	      }
	    }
	    if (module_name.find("ZMM")!=std::string::npos) {
	      TObject *obj_ZMM = key_module->ReadObj();
	      TDirectory *tdir_ZMM = dynamic_cast<TDirectory*>(obj_ZMM);
	      if ( tdir_ZMM !=0 ) {
		TIter next_trigger(tdir_ZMM->GetListOfKeys());
		TKey* key_trigger(0);
		while((key_trigger=dynamic_cast<TKey*>(next_trigger()))!=0) {
		  std::string TriggerName=key_trigger->GetName();
		  // std::cout<<"Find Module: "<<module_name<<" Trigger: "<<TriggerName<<std::endl;
		  fitMergedFile_IDPerfMonZMM( f,run_dir,TriggerName );
		}
	      } else {
		delete obj_ZMM;
	      }
	    }
	  }
	} else {
	  delete obj_perf;
	}         
      }
      //      else
        //std::cerr<<"directory IDPerfMon doesn't exist !"<<std::endl;
      } else {
	delete obj_run;
      }
    }
  
  f->Close();
  delete f;
  //  std::cout << "\n";
  //std::cout << "Finish Inner-Detector performance-monitoring analysis"<<std::endl;
}


void
MonitoringFile::
fitMergedFile_IDPerfMonKshort( TFile* f, std::string run_dir, std::string TriggerName )
{
  std::string path;
  path= run_dir + "IDPerfMon/Kshort/" + TriggerName;
  if(  f->cd(path.c_str())==0 ) {
    //std::cerr << "MonitoringFile::fitMergedFile_IDPerfMonKshort(): "
	            //<< "No such directory \"" << path << "\"\n";
    return;
  }
  
  Double_t myPi = TMath::Pi();

  TH1F* h_massVPtBinFittedHistos[5];
  TH1F* h_massVRadiusBinFittedHistos[7];
  TH1F* h_massVEtaBinHistos[10];
  TH1F* h_massVPhiBinHistos[10];
  TH1F* h_massVCurvatureDiffBinHistos[6];

  if (!CheckHistogram(f,(path+"/ks_mass").c_str())) return;
  if (!CheckHistogram(f,(path+"/MassVptBinFitted0").c_str())) return;
  if (!CheckHistogram(f,(path+"/MassVptBinFitted1").c_str())) return;
  if (!CheckHistogram(f,(path+"/MassVptBinFitted2").c_str())) return;
  if (!CheckHistogram(f,(path+"/MassVptBinFitted3").c_str())) return;
  if (!CheckHistogram(f,(path+"/MassVptBinFitted4").c_str())) return;
  if (!CheckHistogram(f,(path+"/MassVradiusBinFitted0").c_str())) return;
  if (!CheckHistogram(f,(path+"/MassVradiusBinFitted1").c_str())) return;
  if (!CheckHistogram(f,(path+"/MassVradiusBinFitted2").c_str())) return;
  if (!CheckHistogram(f,(path+"/MassVradiusBinFitted3").c_str())) return;
  if (!CheckHistogram(f,(path+"/MassVradiusBinFitted4").c_str())) return;
  if (!CheckHistogram(f,(path+"/MassVradiusBinFitted5").c_str())) return;
  if (!CheckHistogram(f,(path+"/MassVradiusBinFitted6").c_str())) return;
  if (!CheckHistogram(f,(path+"/MassVEtaBin0").c_str())) return;
  if (!CheckHistogram(f,(path+"/MassVEtaBin1").c_str())) return;
  if (!CheckHistogram(f,(path+"/MassVEtaBin2").c_str())) return;
  if (!CheckHistogram(f,(path+"/MassVEtaBin3").c_str())) return;
  if (!CheckHistogram(f,(path+"/MassVEtaBin4").c_str())) return;
  if (!CheckHistogram(f,(path+"/MassVEtaBin5").c_str())) return;
  if (!CheckHistogram(f,(path+"/MassVEtaBin6").c_str())) return;
  if (!CheckHistogram(f,(path+"/MassVEtaBin7").c_str())) return;
  if (!CheckHistogram(f,(path+"/MassVEtaBin8").c_str())) return;
  if (!CheckHistogram(f,(path+"/MassVEtaBin9").c_str())) return;
  if (!CheckHistogram(f,(path+"/MassVPhiBin0").c_str())) return;
  if (!CheckHistogram(f,(path+"/MassVPhiBin1").c_str())) return;
  if (!CheckHistogram(f,(path+"/MassVPhiBin2").c_str())) return;
  if (!CheckHistogram(f,(path+"/MassVPhiBin3").c_str())) return;
  if (!CheckHistogram(f,(path+"/MassVPhiBin4").c_str())) return;
  if (!CheckHistogram(f,(path+"/MassVPhiBin5").c_str())) return;
  if (!CheckHistogram(f,(path+"/MassVPhiBin6").c_str())) return;
  if (!CheckHistogram(f,(path+"/MassVPhiBin7").c_str())) return;
  if (!CheckHistogram(f,(path+"/MassVPhiBin8").c_str())) return;
  if (!CheckHistogram(f,(path+"/MassVPhiBin9").c_str())) return;
  if (!CheckHistogram(f,(path+"/MassVCurvatureDiffBin0").c_str())) return;
  if (!CheckHistogram(f,(path+"/MassVCurvatureDiffBin1").c_str())) return;
  if (!CheckHistogram(f,(path+"/MassVCurvatureDiffBin2").c_str())) return;
  if (!CheckHistogram(f,(path+"/MassVCurvatureDiffBin3").c_str())) return;
  if (!CheckHistogram(f,(path+"/MassVCurvatureDiffBin4").c_str())) return;
  if (!CheckHistogram(f,(path+"/MassVCurvatureDiffBin5").c_str())) return;

  TH1F* h_mass = (TH1F*)(f->Get((path+"/ks_mass").c_str())->Clone());
  if (CheckHistogram(f,(path+"/Nevents").c_str())) {
    TH1F* h_Nevents=(TH1F*)f->Get((path+"/Nevents").c_str());
    TH1F* h_rate = new TH1F("rate","production rate",1,0,1);
    double Ntot =h_Nevents->GetEntries();
    double yield = h_mass->Integral(h_mass->FindBin(0.487), h_mass->FindBin(0.507));
    double rate=yield/Ntot;
    double rate_error=sqrt(1/yield+1/Ntot)*yield/Ntot;
    h_rate->SetBinContent(1,rate);
    h_rate->SetBinError(1,rate_error);
    h_rate->Write("",TObject::kOverwrite);
  }
//   TH1F* h_mass_scaled = (TH1F*)(f->Get((path+"/ks_mass").c_str())->Clone("ks_mass_scaled_copy"));
//   TString title(h_mass_scaled->GetTitle());
//   if (CheckHistogram(f,(path+"/ks_mass_scaled").c_str())) {
//     if (CheckHistogram(f,(path+"/Nevents").c_str())) {
//       TH1F* h_Nevents=(TH1F*)f->Get((path+"/Nevents").c_str());
//       double Ntot =h_Nevents->GetEntries();
//       if (Ntot!=0.) h_mass_scaled->Scale(1./Ntot);
//       h_mass_scaled->SetTitle(title+" (per event)");
//       h_mass_scaled->Write("ks_mass_scaled",TObject::kOverwrite);
//     }  
//   }
//   delete h_mass_scaled;

  h_massVPtBinFittedHistos[0] = (TH1F*)(f->Get((path+"/MassVptBinFitted0").c_str())->Clone());
  h_massVPtBinFittedHistos[1] = (TH1F*)(f->Get((path+"/MassVptBinFitted1").c_str())->Clone());
  h_massVPtBinFittedHistos[2] = (TH1F*)(f->Get((path+"/MassVptBinFitted2").c_str())->Clone());
  h_massVPtBinFittedHistos[3] = (TH1F*)(f->Get((path+"/MassVptBinFitted3").c_str())->Clone());
  h_massVPtBinFittedHistos[4] = (TH1F*)(f->Get((path+"/MassVptBinFitted4").c_str())->Clone());
  h_massVRadiusBinFittedHistos[0] = (TH1F*)(f->Get((path+"/MassVradiusBinFitted0").c_str())->Clone());
  h_massVRadiusBinFittedHistos[1] = (TH1F*)(f->Get((path+"/MassVradiusBinFitted1").c_str())->Clone());
  h_massVRadiusBinFittedHistos[2] = (TH1F*)(f->Get((path+"/MassVradiusBinFitted2").c_str())->Clone());
  h_massVRadiusBinFittedHistos[3] = (TH1F*)(f->Get((path+"/MassVradiusBinFitted3").c_str())->Clone());
  h_massVRadiusBinFittedHistos[4] = (TH1F*)(f->Get((path+"/MassVradiusBinFitted4").c_str())->Clone());
  h_massVRadiusBinFittedHistos[5] = (TH1F*)(f->Get((path+"/MassVradiusBinFitted5").c_str())->Clone());
  h_massVRadiusBinFittedHistos[6] = (TH1F*)(f->Get((path+"/MassVradiusBinFitted6").c_str())->Clone());
  h_massVEtaBinHistos[0] = (TH1F*)(f->Get((path+"/MassVEtaBin0").c_str())->Clone());
  h_massVEtaBinHistos[1] = (TH1F*)(f->Get((path+"/MassVEtaBin1").c_str())->Clone());
  h_massVEtaBinHistos[2] = (TH1F*)(f->Get((path+"/MassVEtaBin2").c_str())->Clone());
  h_massVEtaBinHistos[3] = (TH1F*)(f->Get((path+"/MassVEtaBin3").c_str())->Clone());
  h_massVEtaBinHistos[4] = (TH1F*)(f->Get((path+"/MassVEtaBin4").c_str())->Clone());
  h_massVEtaBinHistos[5] = (TH1F*)(f->Get((path+"/MassVEtaBin5").c_str())->Clone());
  h_massVEtaBinHistos[6] = (TH1F*)(f->Get((path+"/MassVEtaBin6").c_str())->Clone());
  h_massVEtaBinHistos[7] = (TH1F*)(f->Get((path+"/MassVEtaBin7").c_str())->Clone());
  h_massVEtaBinHistos[8] = (TH1F*)(f->Get((path+"/MassVEtaBin8").c_str())->Clone());
  h_massVEtaBinHistos[9] = (TH1F*)(f->Get((path+"/MassVEtaBin9").c_str())->Clone());
  h_massVPhiBinHistos[0] = (TH1F*)(f->Get((path+"/MassVPhiBin0").c_str())->Clone());
  h_massVPhiBinHistos[1] = (TH1F*)(f->Get((path+"/MassVPhiBin1").c_str())->Clone());
  h_massVPhiBinHistos[2] = (TH1F*)(f->Get((path+"/MassVPhiBin2").c_str())->Clone());
  h_massVPhiBinHistos[3] = (TH1F*)(f->Get((path+"/MassVPhiBin3").c_str())->Clone());
  h_massVPhiBinHistos[4] = (TH1F*)(f->Get((path+"/MassVPhiBin4").c_str())->Clone());
  h_massVPhiBinHistos[5] = (TH1F*)(f->Get((path+"/MassVPhiBin5").c_str())->Clone());
  h_massVPhiBinHistos[6] = (TH1F*)(f->Get((path+"/MassVPhiBin6").c_str())->Clone());
  h_massVPhiBinHistos[7] = (TH1F*)(f->Get((path+"/MassVPhiBin7").c_str())->Clone());
  h_massVPhiBinHistos[8] = (TH1F*)(f->Get((path+"/MassVPhiBin8").c_str())->Clone());
  h_massVPhiBinHistos[9] = (TH1F*)(f->Get((path+"/MassVPhiBin9").c_str())->Clone());
  h_massVCurvatureDiffBinHistos[0] = (TH1F*)(f->Get((path+"/MassVCurvatureDiffBin0").c_str())->Clone());
  h_massVCurvatureDiffBinHistos[1] = (TH1F*)(f->Get((path+"/MassVCurvatureDiffBin1").c_str())->Clone());
  h_massVCurvatureDiffBinHistos[2] = (TH1F*)(f->Get((path+"/MassVCurvatureDiffBin2").c_str())->Clone());
  h_massVCurvatureDiffBinHistos[3] = (TH1F*)(f->Get((path+"/MassVCurvatureDiffBin3").c_str())->Clone());
  h_massVCurvatureDiffBinHistos[4] = (TH1F*)(f->Get((path+"/MassVCurvatureDiffBin4").c_str())->Clone());
  h_massVCurvatureDiffBinHistos[5] = (TH1F*)(f->Get((path+"/MassVCurvatureDiffBin5").c_str())->Clone());
 
  TF1 *func1 = new TF1("func1","gaus",0.460,0.540);
  TF1 *func2 = new TF1("func2","expo",0.460,0.540);
  TF1 *func = new TF1("func","func1+func2",0.460,0.540);

  func->SetLineColor(4);
  func->SetParameters(10.,0.500,0.010,2.,-.001);
  // func->SetParLimits(0,0.,10000.);
  func->SetParLimits(1,0.460,0.540);
  func->SetParLimits(2,0.,0.100);
  func->SetParLimits(3,0.,10000.);
  func->SetParLimits(4,-1000.,0.);
  func->SetParNames("Constant","Mean","Sigma","Constant","Slope");

  TCanvas *myCanvas = new TCanvas("MyCanvas");
  myCanvas->cd();

  h_mass->Fit(func,"lmhqnr");

  Double_t massBins[5], massErrorBins[5], widthBins[5], widthErrorBins[5];
  const Int_t nPtBinsHisto = 5+1;
  Double_t ptBins[nPtBinsHisto] = {0.5,1.6,2.1,2.8,3.9,5.1};
  for(int binFill=0;binFill<5;binFill++) {
    double max= h_massVPtBinFittedHistos[binFill]->GetBinContent(h_massVPtBinFittedHistos[binFill]->GetMaximumBin());
    func->SetParLimits(0,0,2*max);
    h_massVPtBinFittedHistos[binFill]->Fit(func,"lmhqr");
    massBins[binFill] = func->GetParameter(1);
    massErrorBins[binFill] = func->GetParError(1);
    widthBins[binFill] = func->GetParameter(2);
    widthErrorBins[binFill] = func->GetParError(2);
  }

  const Double_t* ptBinsFinal = ptBins;
  const Double_t* massBinsFinal = massBins;
  const Double_t* massErrorBinsFinal = massErrorBins;
  const Double_t* widthBinsFinal = widthBins;
  const Double_t* widthErrorBinsFinal = widthErrorBins;

  TH1F* h_massVersusPt = new TH1F("KsMassVersusPt_Merged","",5,ptBinsFinal);
  h_massVersusPt->SetXTitle("p_{T} (Gev / c)");
  h_massVersusPt->SetYTitle("Mass (Gev / c^{2})");
  h_massVersusPt->SetMarkerStyle(20);
  for(int binFill=0;binFill<5;binFill++) {
    Double_t binContent = massBinsFinal[binFill];
    Double_t binError = massErrorBinsFinal[binFill];
    h_massVersusPt->SetBinContent(binFill+1,binContent);
    h_massVersusPt->SetBinError(binFill+1,binError);
  }
  
  TH1F* h_widthVersusPt = new TH1F("KsWidthVersusPt_Merged","",5,ptBinsFinal);
  h_widthVersusPt->SetXTitle("p_{T} (Gev / c)");
  h_widthVersusPt->SetYTitle("Width (Gev / c^{2})");
  h_widthVersusPt->SetMarkerStyle(20);
  for(int binFill=0;binFill<5;binFill++) {
    Double_t binContent = widthBinsFinal[binFill];
    Double_t binError = widthErrorBinsFinal[binFill];
    h_widthVersusPt->SetBinContent(binFill+1,binContent);
    h_widthVersusPt->SetBinError(binFill+1,binError);
  }

  Double_t massVradiusBins[7], massVradiusErrorBins[7], widthVradiusBins[7], widthVradiusErrorBins[7];
  const Int_t nRadiusBinsHisto = 7+1;
  Double_t radiusBins[nRadiusBinsHisto] = {0.,30.,40.,60.,80.,100.,140.,230};
  for(int binFill=0;binFill<7;binFill++) {
    double max= h_massVRadiusBinFittedHistos[binFill]->GetBinContent(h_massVRadiusBinFittedHistos[binFill]->GetMaximumBin());
    func->SetParLimits(0,0,2*max);
    h_massVRadiusBinFittedHistos[binFill]->Fit(func,"lmhqr");
    massVradiusBins[binFill] = func->GetParameter(1);
    massVradiusErrorBins[binFill] = func->GetParError(1);
    widthVradiusBins[binFill] = func->GetParameter(2);
    widthVradiusErrorBins[binFill] = func->GetParError(2);
  }

  const Double_t* radiusBinsFinal = radiusBins;
  const Double_t* massVradiusBinsFinal = massVradiusBins;
  const Double_t* massVradiusErrorBinsFinal = massVradiusErrorBins;
  const Double_t* widthVradiusBinsFinal = widthVradiusBins;
  const Double_t* widthVradiusErrorBinsFinal = widthVradiusErrorBins;

  TH1F* h_massVersusRadius = new TH1F("KsMassVersusRadius_Merged","",7,radiusBinsFinal);
  h_massVersusRadius->SetXTitle("Decay Radius (mm)");
  h_massVersusRadius->SetYTitle("Mass (Gev / c^{2})");
  h_massVersusRadius->SetMarkerStyle(20);
  for(int binFill=0;binFill<7;binFill++) {
    Double_t binContent = massVradiusBinsFinal[binFill];
    Double_t binError = massVradiusErrorBinsFinal[binFill];
    h_massVersusRadius->SetBinContent(binFill+1,binContent);
    h_massVersusRadius->SetBinError(binFill+1,binError);
  }
  TH1F* h_widthVersusRadius = new TH1F("KsWidthVersusRadius_Merged","",7,radiusBinsFinal);
  h_widthVersusRadius->SetXTitle("Decay Radius (mm)");
  h_widthVersusRadius->SetYTitle("Width (Gev / c^{2})");
  h_widthVersusRadius->SetMarkerStyle(20);
  for(int binFill=0;binFill<7;binFill++) {
    Double_t binContent = widthVradiusBinsFinal[binFill];
    Double_t binError = widthVradiusErrorBinsFinal[binFill];
    h_widthVersusRadius->SetBinContent(binFill+1,binContent);
    h_widthVersusRadius->SetBinError(binFill+1,binError);
  }

  Double_t etaBins[11] = {-2.5,-2.0,-1.5,-1.0,-0.5,0.0,0.5,1.0,1.5,2.0,2.5};
  Double_t massVetaBins[10], massVetaErrorBins[10], widthVetaBins[10], widthVetaErrorBins[10];
  for(int binFill=0;binFill<10;binFill++) {
    double max= h_massVEtaBinHistos[binFill]->GetBinContent(h_massVEtaBinHistos[binFill]->GetMaximumBin());
    func->SetParLimits(0,0,2*max);
    h_massVEtaBinHistos[binFill]->Fit(func,"lmhqr");
    massVetaBins[binFill] = func->GetParameter(1);
    massVetaErrorBins[binFill] = func->GetParError(1);
    widthVetaBins[binFill] = func->GetParameter(2);
    widthVetaErrorBins[binFill] = func->GetParError(2);
  }
  TH1F* h_massVersusEta = new TH1F("KsMassVersusEta_Merged","",10,etaBins);
  h_massVersusEta->SetXTitle("#eta");
  h_massVersusEta->SetYTitle("Mass (Gev / c^{2})");
  h_massVersusEta->SetMarkerStyle(20);
  for(int binFill=0;binFill<10;binFill++) {
    Double_t binContent = massVetaBins[binFill];
    Double_t binError = massVetaErrorBins[binFill];
    h_massVersusEta->SetBinContent(binFill+1,binContent);
    h_massVersusEta->SetBinError(binFill+1,binError);
  }
  TH1F* h_widthVersusEta = new TH1F("KsWidthVersusEta_Merged","",10,etaBins);
  h_widthVersusEta->SetXTitle("#eta");
  h_widthVersusEta->SetYTitle("Width (Gev / c^{2})");
  h_widthVersusEta->SetMarkerStyle(20);
  for(int binFill=0;binFill<10;binFill++) {
    Double_t binContent = widthVetaBins[binFill];
    Double_t binError = widthVetaErrorBins[binFill];
    h_widthVersusEta->SetBinContent(binFill+1,binContent);
    h_widthVersusEta->SetBinError(binFill+1,binError);
  }

  Double_t phiBins[11] = { (-5.0*myPi / 5) ,
		     (-4.0*myPi / 5) ,
		     (-3.0*myPi / 5) ,
		     (-2.0*myPi / 5) ,
		     (-1.0*myPi / 5) ,
		     (0.0*myPi / 5) ,
		     (1.0*myPi / 5) ,
		     (2.0*myPi / 5) ,
		     (3.0*myPi / 5) ,
		     (4.0*myPi / 5) ,
		     (5.0*myPi / 5) };
  Double_t massVphiBins[10], massVphiErrorBins[10], widthVphiBins[10], widthVphiErrorBins[10];
  for(int binFill=0;binFill<10;binFill++) {
   double max= h_massVPhiBinHistos[binFill]->GetBinContent(h_massVPhiBinHistos[binFill]->GetMaximumBin());
   func->SetParLimits(0,0,2*max);
    h_massVPhiBinHistos[binFill]->Fit(func,"lmhqr");
    massVphiBins[binFill] = func->GetParameter(1);
    massVphiErrorBins[binFill] = func->GetParError(1);
    widthVphiBins[binFill] = func->GetParameter(2);
    widthVphiErrorBins[binFill] = func->GetParError(2);
  }
  TH1F* h_massVersusPhi = new TH1F("KsMassVersusPhi_Merged","",10,phiBins);
  h_massVersusPhi->SetXTitle("#phi");
  h_massVersusPhi->SetYTitle("Mass (Gev / c^{2})");
  h_massVersusPhi->SetMarkerStyle(20);
  for(int binFill=0;binFill<10;binFill++) {
    Double_t binContent = massVphiBins[binFill];
    Double_t binError = massVphiErrorBins[binFill];
    h_massVersusPhi->SetBinContent(binFill+1,binContent);
    h_massVersusPhi->SetBinError(binFill+1,binError);
  }
  TH1F* h_widthVersusPhi = new TH1F("KsWidthVersusPhi_Merged","",10,phiBins);
  h_widthVersusPhi->SetXTitle("#phi");
  h_widthVersusPhi->SetYTitle("Width (Gev / c^{2})");
  h_widthVersusPhi->SetMarkerStyle(20);
  for(int binFill=0;binFill<10;binFill++) {
    Double_t binContent = widthVphiBins[binFill];
    Double_t binError = widthVphiErrorBins[binFill];
    h_widthVersusPhi->SetBinContent(binFill+1,binContent);
    h_widthVersusPhi->SetBinError(binFill+1,binError);
  }

  Double_t curvatureDiffBins[7] = {-0.0012,-0.0008,-0.0004,0.0000,0.0004,0.0008,0.0012};
  Double_t massVcurvatureDiffBins[6], massVcurvatureDiffErrorBins[6], widthVcurvatureDiffBins[6], widthVcurvatureDiffErrorBins[6];
  for(int binFill=0;binFill<6;binFill++) {
   double max= h_massVCurvatureDiffBinHistos[binFill]->GetBinContent(h_massVCurvatureDiffBinHistos[binFill]->GetMaximumBin());
   func->SetParLimits(0,0,2*max);
    h_massVCurvatureDiffBinHistos[binFill]->Fit(func,"lmhqr");
    massVcurvatureDiffBins[binFill] = func->GetParameter(1);
    massVcurvatureDiffErrorBins[binFill] = func->GetParError(1);
    widthVcurvatureDiffBins[binFill] = func->GetParameter(2);
    widthVcurvatureDiffErrorBins[binFill] = func->GetParError(2);
  }
  TH1F* h_massVersusCurvatureDiff = new TH1F("KsMassVersusCurvatureDiff_Merged","",6,curvatureDiffBins);
  h_massVersusCurvatureDiff->SetXTitle("1/p_{T}(#pi^{+}) - 1/p_{T}(#pi^{-}) [GeV^{-1}]");
  h_massVersusCurvatureDiff->SetYTitle("Mass (Gev / c^{2})");
  h_massVersusCurvatureDiff->SetMarkerStyle(20);
  for(int binFill=0;binFill<6;binFill++) {
    Double_t binContent = massVcurvatureDiffBins[binFill];
    Double_t binError = massVcurvatureDiffErrorBins[binFill];
    h_massVersusCurvatureDiff->SetBinContent(binFill+1,binContent);
    h_massVersusCurvatureDiff->SetBinError(binFill+1,binError);
  }
  TH1F* h_widthVersusCurvatureDiff = new TH1F("KsWidthVersusCurvatureDiff_Merged","",6,curvatureDiffBins);
  h_widthVersusCurvatureDiff->SetXTitle("1/p_{T}(#pi^{+}) - 1/p_{T}(#pi^{-}) [GeV^{-1}]");
  h_widthVersusCurvatureDiff->SetYTitle("Width (Gev / c^{2})");
  h_widthVersusCurvatureDiff->SetMarkerStyle(20);
  for(int binFill=0;binFill<6;binFill++) {
    Double_t binContent = widthVcurvatureDiffBins[binFill];
    Double_t binError = widthVcurvatureDiffErrorBins[binFill];
    h_widthVersusCurvatureDiff->SetBinContent(binFill+1,binContent);
    h_widthVersusCurvatureDiff->SetBinError(binFill+1,binError);
  }

  if(  f->cd(path.c_str())==0 ) {
    //std::cerr << "MonitoringFile::fitMergedFile_IDPerfMonKshort(): "
	            //<< "No such directory \"" << path << "\"\n";
    return;
  }
  
  // f->cd(path.c_str());

  h_mass->Write("",TObject::kOverwrite);

  h_massVPtBinFittedHistos[0]->Write("",TObject::kOverwrite);
  h_massVPtBinFittedHistos[1]->Write("",TObject::kOverwrite);
  h_massVPtBinFittedHistos[2]->Write("",TObject::kOverwrite);
  h_massVPtBinFittedHistos[3]->Write("",TObject::kOverwrite);
  h_massVPtBinFittedHistos[4]->Write("",TObject::kOverwrite);
  h_massVRadiusBinFittedHistos[0]->Write("",TObject::kOverwrite);
  h_massVRadiusBinFittedHistos[1]->Write("",TObject::kOverwrite);
  h_massVRadiusBinFittedHistos[2]->Write("",TObject::kOverwrite);
  h_massVRadiusBinFittedHistos[3]->Write("",TObject::kOverwrite);
  h_massVRadiusBinFittedHistos[4]->Write("",TObject::kOverwrite);
  h_massVRadiusBinFittedHistos[5]->Write("",TObject::kOverwrite);
  h_massVRadiusBinFittedHistos[6]->Write("",TObject::kOverwrite);
  h_massVEtaBinHistos[0]->Write("",TObject::kOverwrite);
  h_massVEtaBinHistos[1]->Write("",TObject::kOverwrite);
  h_massVEtaBinHistos[2]->Write("",TObject::kOverwrite);
  h_massVEtaBinHistos[3]->Write("",TObject::kOverwrite);
  h_massVEtaBinHistos[4]->Write("",TObject::kOverwrite);
  h_massVEtaBinHistos[5]->Write("",TObject::kOverwrite);
  h_massVEtaBinHistos[6]->Write("",TObject::kOverwrite);
  h_massVEtaBinHistos[7]->Write("",TObject::kOverwrite);
  h_massVEtaBinHistos[8]->Write("",TObject::kOverwrite);
  h_massVEtaBinHistos[9]->Write("",TObject::kOverwrite);
  h_massVPhiBinHistos[0]->Write("",TObject::kOverwrite);
  h_massVPhiBinHistos[1]->Write("",TObject::kOverwrite);
  h_massVPhiBinHistos[2]->Write("",TObject::kOverwrite);
  h_massVPhiBinHistos[3]->Write("",TObject::kOverwrite);
  h_massVPhiBinHistos[4]->Write("",TObject::kOverwrite);
  h_massVPhiBinHistos[5]->Write("",TObject::kOverwrite);
  h_massVPhiBinHistos[6]->Write("",TObject::kOverwrite);
  h_massVPhiBinHistos[7]->Write("",TObject::kOverwrite);
  h_massVPhiBinHistos[8]->Write("",TObject::kOverwrite);
  h_massVPhiBinHistos[9]->Write("",TObject::kOverwrite);
  h_massVCurvatureDiffBinHistos[0]->Write("",TObject::kOverwrite);
  h_massVCurvatureDiffBinHistos[1]->Write("",TObject::kOverwrite);
  h_massVCurvatureDiffBinHistos[2]->Write("",TObject::kOverwrite);
  h_massVCurvatureDiffBinHistos[3]->Write("",TObject::kOverwrite);
  h_massVCurvatureDiffBinHistos[4]->Write("",TObject::kOverwrite);
  h_massVCurvatureDiffBinHistos[5]->Write("",TObject::kOverwrite);

  h_massVersusPt->Write("",TObject::kOverwrite);
  h_widthVersusPt->Write("",TObject::kOverwrite);
  h_massVersusRadius->Write("",TObject::kOverwrite);
  h_widthVersusRadius->Write("",TObject::kOverwrite);
  h_massVersusEta->Write("",TObject::kOverwrite);
  h_widthVersusEta->Write("",TObject::kOverwrite);
  h_massVersusPhi->Write("",TObject::kOverwrite);
  h_widthVersusPhi->Write("",TObject::kOverwrite);
  h_massVersusCurvatureDiff->Write("",TObject::kOverwrite);
  h_widthVersusCurvatureDiff->Write("",TObject::kOverwrite);

  myCanvas->Close();
  delete myCanvas;
  
  f->Write();
  
  delete func;
  delete func1;
  delete func2;
}


//Jpsi plots merging 
void
MonitoringFile::
fitMergedFile_IDPerfMonJpsi( TFile* f, std::string run_dir, std::string TriggerName )
{ 
  std::string path;
  path= run_dir+"IDPerfMon/Jpsi/"+TriggerName;

  if( f->cd(path.c_str())==0 ) {
    //std::cerr//<< "MonitoringFile::fitMergedFile_IDPerfMonJpsi(): "
	            //<<"No such directory  \""<< path << "\"\n";
    return;
  }

  TH1F* h_jpsi_invmass_vs_pt = new TH1F("Jpsi_invmass_vs_pt","Jpsi invmass-PDGmass vs. Jpsi Pt; p_{T}(J/#psi) [GeV/c]; Mass Shift [GeV/c^{2}]", 6 ,10.,20.);
  TH1F* h_jpsi_invmass_vs_z0 = new TH1F("Jpsi_invmass_vs_z0","Jpsi invmass-PDGmass vs. muon z0; z0(#mu) [mm]; Mass Shift [GeV/c^{2}]", 6,-100.,100.);
  TH1F* h_jpsi_invmass_vs_phi = new TH1F("Jpsi_invmass_vs_phi","Jpsi invmass-PDGmass vs. Jpsi phi; #phi(J/#psi) [rad]; Mass Shift [GeV/c^{2}]", 6, -3.15, 3.15);
  TH1F* h_jpsi_invmass_vs_phiplus = new TH1F("Jpsi_invmass_vs_phiplus","Jpsi invmass-PDGmass vs. Muon+ phi; #phi(#mu^{+}) [rad]; Mass Shift [GeV/c^{2}]", 6, -3.15, 3.15);
  TH1F* h_jpsi_invmass_vs_phiminus = new TH1F("Jpsi_invmass_vs_phiminus","Jpsi invmass-PDGmass vs. Muon- phi; #phi(#mu^{-}) [rad]; Mass Shift [GeV/c^{2}]", 6, -3.15, 3.15);
  TH1F* h_jpsi_invmass_vs_phidiff=new TH1F("Jpsi_invmass_vs_phidiff","Jpsi invmass-PDGmass vs. |phi(+)-phi(-)|; |#phi(#mu^{+})-#phi(#mu^{-})| [rad]; Mass Shift [GeV/c^{2}]", 6, 0., 0.4);
  TH1F* h_jpsi_invmass_vs_phisum = new TH1F("Jpsi_invmass_vs_phisum","Jpsi invmass-PDGmass vs. phi(+)+phi(-);#phi(#mu^{+})+#phi(#mu^{-}) [rad];Mass Shift [GeV/c^{2}]", 6, -3.15, 3.15);
  TH1F* h_jpsi_invmass_vs_eta = new TH1F("Jpsi_invmass_vs_eta","Jpsi invmass-PDGmass vs. Jpsi eta; #eta(J/#psi); Mass Shift [GeV/c^{2}]", 6, -2.4, 2.4);
  TH1F* h_jpsi_invmass_vs_eta_barrel = new TH1F("Jpsi_invmass_vs_eta_barrel","Jpsi invmass-PDGmass vs. Jpsi eta (barrel); #eta(J/#psi); Mass Shift [GeV/c^{2}]", 2, -0.8, 0.8);
  TH1F* h_jpsi_invmass_vs_eta_ecc = new TH1F("Jpsi_invmass_vs_eta_ecc","Jpsi invmass-PDGmass vs. Jpsi eta (endcap C); #eta(J/#psi); Mass Shift [GeV/c^{2}]", 2, -2.4, -0.8);
  TH1F* h_jpsi_invmass_vs_eta_eca = new TH1F("Jpsi_invmass_vs_eta_eca","Jpsi invmass-PDGmass vs. Jpsi eta (endcap A); #eta(J/#psi); Mass Shift [GeV/c^{2}]", 2, 0.8, 2.4);
  TH1F* h_jpsi_invmass_vs_etaplus= new TH1F("Jpsi_invmass_vs_etaplus","Jpsi invmass-PDGmass vs. Muon+ eta; #eta(#mu^{+}); Mass Shift [GeV/c^{2}]", 6,-2.4,2.4);
  TH1F* h_jpsi_invmass_vs_etaminus= new TH1F("Jpsi_invmass_vs_etaminus","Jpsi invmass-PDGmass vs. Muon- eta; #eta(#mu^{-}); Mass Shift [GeV/c^{2}]", 6,-2.4,2.4);
  TH1F* h_jpsi_invmass_vs_maxeta= new TH1F("Jpsi_invmass_vs_maxeta","Jpsi invmass-PDGmass vs. max Muon |eta|; max|#eta(#mu)|; Mass Shift [GeV/c^{2}]", 6,-2.4,2.4);
  TH1F* h_jpsi_invmass_vs_etadiff = new TH1F("Jpsi_invmass_vs_etadiff","Jpsi invmass-PDGmass vs. eta(+)-eta(-); #eta(#mu^{+})-#eta(#mu^{-}); Mass Shift [GeV/c^{2}]", 6,-0.5,0.5);
  TH1F* h_jpsi_invmass_vs_sumeta= new TH1F("Jpsi_invmass_vs_sumeta","Jpsi invmass-PDGmass vs. eta(+)+eta(-); #eta(#mu^{+})+#eta(#mu^{-});Mass Shift [GeV/c^{2}]", 6,-4.8,4.8);
  TH1F* h_jpsi_invmass_vs_sumabseta= new TH1F("Jpsi_invmass_vs_sumabseta","Jpsi invmass-PDGmass vs. |eta(+)|+|eta(-)|;|#eta(#mu^{+})|+|#eta(#mu^{-})|;Mass Shift [GeV/c^{2}]", 6,0.,4.8);
  TH1F* h_jpsi_invmass_vs_crt= new TH1F("Jpsi_invmass_vs_curvature","Jpsi invmass-PDGmass vs. crt(+)-|crt(-)|; 1/p_{T}(#mu^{+})-1/p_{T}(#mu^{-}) [GeV^{-1}]; Mass Shift [GeV/c^{2}]",6, -0.15, 0.15);
  TH1F* h_jpsi_invmass_vs_invpt=new TH1F("Jpsi_invmass_vs_invpt","Jpsi invmass-PDGmass vs. muon  1/Pt; 1/p_{T}(#mu) [(GeV/c)^{-1}]; Mass Shift [GeV/c^{2}]",6,0.05,0.25);
  TH1F* h_jpsi_invmass_vs_angle_eta=new TH1F("Jpsi_invmass_vs_angle_eta","Jpsi invmass-PDGmass vs. arccos(#Delta#eta/#DeltaR);arccos(#Delta#eta/#DeltaR); Mass Shift [GeV/c^{2}]",6,0.,90.);

  TH1F* h_jpsi_width_vs_pt = new TH1F("Jpsi_width_vs_pt","Jpsi width vs. Jpsi Pt; p_{T}(J/#psi) [GeV/c]; Width [GeV/c^{2}]", 6 ,10.,20.);
  TH1F* h_jpsi_width_vs_z0 = new TH1F("Jpsi_width_vs_z0","Jpsi width vs. muon z0; z0(#mu) [mm]; Width [GeV/c^{2}]", 6,-100.,100.);
  TH1F* h_jpsi_width_vs_phi = new TH1F("Jpsi_width_vs_phi","Jpsi width vs. Jpsi phi; #phi(J/#psi) [rad]; Width [GeV/c^{2}]", 6, -3.15, 3.15);
  TH1F* h_jpsi_width_vs_phiplus = new TH1F("Jpsi_width_vs_phiplus","Jpsi width vs. Muon+ phi; #phi(#mu^{+}) [rad]; Width [GeV/c^{2}]", 6, -3.15, 3.15);
  TH1F* h_jpsi_width_vs_phiminus = new TH1F("Jpsi_width_vs_phiminus","Jpsi width vs. Muon- phi; #phi(#mu^{-}) [rad]; Width [GeV/c^{2}]", 6, -3.15, 3.15);
  TH1F* h_jpsi_width_vs_phidiff=new TH1F("Jpsi_width_vs_phidiff","Jpsi width vs. |phi(+)-phi(-)|; |#phi(#mu^{+})-#phi(#mu^{-})| [rad]; Width [GeV/c^{2}]", 6, 0., 0.4);
  TH1F* h_jpsi_width_vs_phisum = new TH1F("Jpsi_width_vs_phisum","Jpsi width vs. phi(+)+phi(-);#phi(#mu^{+})+#phi(#mu^{-}) [rad];Width [GeV/c^{2}]", 6, -3.15, 3.15);
  TH1F* h_jpsi_width_vs_eta = new TH1F("Jpsi_width_vs_eta","Jpsi width vs. Jpsi eta; #eta(J/#psi); Width[GeV/c^{2}]", 6, -2.4, 2.4);
  TH1F* h_jpsi_width_vs_eta_barrel = new TH1F("Jpsi_width_vs_eta_barrel","Jpsi width vs. Jpsi eta (barrel); #eta(J/#psi); Width [GeV/c^{2}]", 2, -0.8, 0.8);
  TH1F* h_jpsi_width_vs_eta_ecc = new TH1F("Jpsi_width_vs_eta_ecc","Jpsi width vs. Jpsi eta (endcap C); #eta(J/#psi); Width [GeV/c^{2}]", 2, -2.4, -0.8);
  TH1F* h_jpsi_width_vs_eta_eca = new TH1F("Jpsi_width_vs_eta_eca","Jpsi width vs. Jpsi eta (endcap A); #eta(J/#psi); Width [GeV/c^{2}]", 2, 0.8, 2.4);
  TH1F* h_jpsi_width_vs_etaplus= new TH1F("Jpsi_width_vs_etaplus","Jpsi width vs. Muon+ eta; #eta(#mu^{+}); Width [GeV/c^{2}]", 6,-2.4,2.4);
  TH1F* h_jpsi_width_vs_etaminus= new TH1F("Jpsi_width_vs_etaminus","Jpsi width vs. Muon- eta; #eta(#mu^{-}); Width [GeV/c^{2}]", 6,-2.4,2.4);
  TH1F* h_jpsi_width_vs_maxeta= new TH1F("Jpsi_width_vs_maxeta","Jpsi width vs. max Muon |eta|; max|#eta(#mu)|; Width[GeV/c^{2}]", 6,-2.4,2.4);
  TH1F* h_jpsi_width_vs_etadiff = new TH1F("Jpsi_width_vs_etadiff","Jpsi width vs. eta(+)-eta(-); #eta(#mu^{+})-#eta(#mu^{-}); Width [GeV/c^{2}]", 6,-0.5,0.5);
  TH1F* h_jpsi_width_vs_sumeta= new TH1F("Jpsi_width_vs_sumeta","Jpsi width vs. eta(+)+eta(-); #eta(#mu^{+})+#eta(#mu^{-});Width [GeV/c^{2}]", 6,-4.8,4.8);
  TH1F* h_jpsi_width_vs_sumabseta= new TH1F("Jpsi_width_vs_sumabseta","Jpsi width vs. |eta(+)|+|eta(-)|;|#eta(#mu^{+})|+|#eta(#mu^{-})|;Width [GeV/c^{2}]", 6,0.,4.8);
  TH1F* h_jpsi_width_vs_crt= new TH1F("Jpsi_width_vs_curvature","Jpsi width vs. crt(+)-|crt(-)|; 1/p_{T}(#mu^{+})-1/p_{T}(#mu^{-}) [GeV^{-1}]; Width [GeV/c^{2}]",6, -0.15, 0.15);
  TH1F* h_jpsi_width_vs_invpt=new TH1F("Jpsi_width_vs_invpt","Jpsi width vs. muon  1/Pt; 1/p_{T}(#mu) [(GeV/c)^{-1}]; Width [GeV/c^{2}]",6,0.05,0.25);
  TH1F* h_jpsi_width_vs_angle_eta=new TH1F("Jpsi_width_vs_angle_eta","Jpsi width vs. arccos(#Delta#eta/#DeltaR);arccos(#Delta#eta/#DeltaR); Width [GeV/c^{2}]",6,0.,90.);

  TH1F* h_eff_pt = new TH1F("eff_pt","ID tracking efficiency vs p_{T} (tag-and-probe);p_{T} [GeV]",9, 2., 20.);
  TH1F* h_eff_eta = new TH1F("eff_eta","ID tracking efficiency vs #eta (tag-and-probe);#eta",10, -2.5, 2.5);
  TH1F* h_eff_phi = new TH1F("eff_phi","ID tracking efficiency vs #phi (tag-and-probe);#phi [rad]",8, -3.15, 3.15);

  TH1F* h_chargeasym_vs_pt = new TH1F("chargeAsym_vs_pt","Charge asymmetry vs. p_{T};p_{T}(#mu) [GeV];#frac{N^{-}-N^{+}}{N^{-}+N^{+}}",10,0.,20.);

  const int nbins=6;
  TH1F* hpt[nbins];
  TH1F* hz0[nbins];
  TH1F* hphi[nbins];
  TH1F* hphiplus[nbins];
  TH1F* hphiminus[nbins];
  TH1F* hphidiff[nbins];
  TH1F* heta[nbins];
  TH1F* heta_b[2];
  TH1F* heta_ecc[2];
  TH1F* heta_eca[2];
  TH1F* hetadiff[nbins];
  TH1F* hmaxeta[nbins];
  TH1F* hinvpt[nbins];
  TH1F* hcrt[nbins];
  TH1F* hetaplus[nbins];
  TH1F* hetaminus[nbins];
  TH1F* hangle_eta[nbins];
  TH1F* hsumphi[nbins];
  TH1F* hsumeta[nbins];
  TH1F* hsumabseta[nbins];
 
  if (!CheckHistogram( f,(path+"/Jpsi_invmass").c_str())) return;
  TH1F* h_mass = (TH1F*)(f->Get((path+"/Jpsi_invmass").c_str())->Clone());
  if (CheckHistogram(f,(path+"/Nevents").c_str())) {
    TH1F* h_Nevents=(TH1F*)f->Get((path+"/Nevents").c_str());
    TH1F* h_rate = new TH1F("rate","production rate",1,0,1);
    double Ntot =h_Nevents->GetEntries();
    double yield = h_mass->Integral(h_mass->FindBin(2.95), h_mass->FindBin(3.25));
    double rate=yield/Ntot;
    double rate_error=sqrt(1/yield+1/Ntot)*yield/Ntot;
    h_rate->SetBinContent(1,rate);
    h_rate->SetBinError(1,rate_error);
    h_rate->Write("",TObject::kOverwrite);
  }
//   TH1F* h_mass_scaled = (TH1F*)(f->Get((path+"/Jpsi_invmass").c_str())->Clone("Jpsi_invmass_scaled_copy"));
//   h_mass_scaled->SetMarkerStyle(21);
//   TString title(h_mass_scaled->GetTitle());
//   if (CheckHistogram(f,(path+"/Jpsi_invmass_scaled").c_str())) {
//     if (CheckHistogram(f,(path+"/Nevents").c_str())) {
//       TH1F* h_Nevents=(TH1F*)f->Get((path+"/Nevents").c_str());
//       double Ntot =h_Nevents->GetEntries();
//       if (Ntot!=0.) h_mass_scaled->Scale(1./Ntot);
//       h_mass_scaled->SetTitle(title+" (per event)");
//       h_mass_scaled->Write("Jpsi_invmass_scaled",TObject::kOverwrite);
//     }  
//   }
//   delete h_mass_scaled;
  if (CheckHistogram( f,(path+"/Jpsi_invmass").c_str())) {
    TH1F* h_mass_rebin = (TH1F*)(f->Get((path+"/Jpsi_invmass").c_str())->Clone("Jpsi_invmass_rebin"));
    TString title = h_mass_rebin->GetTitle();
    h_mass_rebin ->SetMarkerStyle(21);
    h_mass_rebin->Rebin(2);
    h_mass_rebin->SetTitle(title+" (larger binning for low Stats.)");
    h_mass_rebin->Write("Jpsi_invmass_rebin",TObject::kOverwrite);
  }

  if (!CheckHistogram( f,(path+"/probe_all_pt").c_str())) return;
  TH1F* h_probe_all_pt = (TH1F*)(f->Get((path+"/probe_all_pt").c_str())->Clone());
  if (!CheckHistogram( f,(path+"/probe_match_pt").c_str())) return;
  TH1F* h_probe_match_pt = (TH1F*)(f->Get((path+"/probe_match_pt").c_str())->Clone());
  if (!CheckHistogram( f,(path+"/probe_all_eta").c_str())) return;
  TH1F* h_probe_all_eta = (TH1F*)(f->Get((path+"/probe_all_eta").c_str())->Clone());
  if (!CheckHistogram( f,(path+"/probe_match_eta").c_str())) return;
  TH1F* h_probe_match_eta = (TH1F*)(f->Get((path+"/probe_match_eta").c_str())->Clone());
  if (!CheckHistogram( f,(path+"/probe_all_phi").c_str())) return;
  TH1F* h_probe_all_phi = (TH1F*)(f->Get((path+"/probe_all_phi").c_str())->Clone());
  if (!CheckHistogram( f,(path+"/probe_match_phi").c_str())) return;
  TH1F* h_probe_match_phi = (TH1F*)(f->Get((path+"/probe_match_phi").c_str())->Clone());
  if (!CheckHistogram( f,(path+"/Muonplus_pt").c_str())) return;
  TH1F* h_ptplus = (TH1F*)(f->Get((path+"/Muonplus_pt").c_str())->Clone());
  if (!CheckHistogram( f,(path+"/Muonminus_pt").c_str())) return;
  TH1F* h_ptminus = (TH1F*)(f->Get((path+"/Muonminus_pt").c_str())->Clone());

  char cpt[100],cz0[100],cphi[100],cphidiff[100],ceta[100],cetadiff[100],cmaxeta[100],ccrt[100],cinvpt[100],cphiplus[100],cphiminus[100],cetaplus[100],cetaminus[100],ceta_b[100],ceta_ecc[100],ceta_eca[100],cangle_eta[100],csumeta[100],csumabseta[100],csumphi[100];
  for (int i=0;i<nbins;i++)
    { 
     
      sprintf(cpt,(path+"/pt_bin%d").c_str(),i);
      sprintf(cz0,(path+"/z0_bin%d").c_str(),i);
      sprintf(cphi,(path+"/phi_bin%d").c_str(),i);
      sprintf(cphiplus,(path+"/phiplus_bin%d").c_str(),i);
      sprintf(cphiminus,(path+"/phiminus_bin%d").c_str(),i);
      sprintf(cphidiff,(path+"/phidiff_bin%d").c_str(),i);
      sprintf(ceta,(path+"/eta_bin%d").c_str(),i);
      sprintf(cetaplus,(path+"/etaplus_bin%d").c_str(),i);
      sprintf(cetaminus,(path+"/etaminus_bin%d").c_str(),i);
      sprintf(cmaxeta,(path+"/maxeta_bin%d").c_str(),i);
      sprintf(cetadiff,(path+"/etadiff_bin%d").c_str(),i);
      sprintf(ccrt,(path+"/crt_bin%d").c_str(),i);
      sprintf(cinvpt,(path+"/invpt_bin%d").c_str(),i);
      sprintf(cangle_eta,(path+"/angle_eta_bin%d").c_str(),i);
      sprintf(csumphi,(path+"/phi_sum%d").c_str(),i);
      sprintf(csumeta,(path+"/eta_sum%d").c_str(),i);
      sprintf(csumabseta,(path+"/eta_AbsSum%d").c_str(),i);
     
      if (!CheckHistogram(f,cpt)) return;     
      if (!CheckHistogram(f,cz0)) return;
      if (!CheckHistogram(f,cphi)) return;
      if (!CheckHistogram(f,cphiplus)) return;
      if (!CheckHistogram(f,cphiminus)) return;
      if (!CheckHistogram(f,cphidiff)) return;
      if (!CheckHistogram(f,ceta)) return;
      if (!CheckHistogram(f,cetaplus)) return;
      if (!CheckHistogram(f,cetaminus)) return;
      if (!CheckHistogram(f,cmaxeta)) return;
      if (!CheckHistogram(f,cetadiff)) return;
      if (!CheckHistogram(f,ccrt)) return;
      if (!CheckHistogram(f,cinvpt)) return;
      if (!CheckHistogram(f,cangle_eta)) return;
      if (!CheckHistogram(f,csumphi)) return;
      if (!CheckHistogram(f,csumeta)) return;
      if (!CheckHistogram(f,csumabseta)) return;

      hpt[i] =(TH1F*)(f->Get(cpt)->Clone());
      hz0[i] =(TH1F*)(f->Get(cz0)->Clone());
      hphi[i]=(TH1F*)(f->Get(cphi)->Clone());
      hphiplus[i]=(TH1F*)(f->Get(cphiplus)->Clone());
      hphiminus[i]=(TH1F*)(f->Get(cphiminus)->Clone());
      hphidiff[i]=(TH1F*)(f->Get(cphidiff)->Clone());
      heta[i]=(TH1F*)(f->Get(ceta)->Clone());
      hetaplus[i]=(TH1F*)(f->Get(cetaplus)->Clone());
      hetaminus[i]=(TH1F*)(f->Get(cetaminus)->Clone());
      hetadiff[i]=(TH1F*)(f->Get(cetadiff)->Clone());
      hmaxeta[i]=(TH1F*)(f->Get(cmaxeta)->Clone());
      hcrt[i] = (TH1F*)(f->Get(ccrt)->Clone());
      hinvpt[i] =(TH1F*)(f->Get(cinvpt)->Clone());
      hangle_eta[i]=(TH1F*)(f->Get(cangle_eta)->Clone());
      hsumphi[i]=(TH1F*)(f->Get(csumphi)->Clone());
      hsumeta[i]=(TH1F*)(f->Get(csumeta)->Clone());
      hsumabseta[i]=(TH1F*)(f->Get(csumabseta)->Clone());

      if (i<2) {
	sprintf(ceta_b,(path+"/eta_b_bin%d").c_str(),i);
	sprintf(ceta_eca,(path+"/eta_eca_bin%d").c_str(),i);
	sprintf(ceta_ecc,(path+"/eta_ecc_bin%d").c_str(),i);
	if (!CheckHistogram(f,ceta_b)) return;
	if (!CheckHistogram(f,ceta_eca)) return;
	if (!CheckHistogram(f,ceta_ecc)) return;  
	heta_b[i]=(TH1F*)(f->Get(ceta_b)->Clone());
	heta_ecc[i]=(TH1F*)(f->Get(ceta_ecc)->Clone());
	heta_eca[i]=(TH1F*)(f->Get(ceta_eca)->Clone());
      }
    }
  h_mass->SetMarkerStyle(21);
  
  TCanvas *myCanvas = new TCanvas("MyCanvas");
  myCanvas->cd();
  
//   h_mass->Fit(f1,"RQMN");

  fitJpsiHistograms(h_jpsi_invmass_vs_pt,h_jpsi_width_vs_pt,hpt,nbins);
  fitJpsiHistograms(h_jpsi_invmass_vs_z0,h_jpsi_width_vs_z0,hz0,nbins);
  fitJpsiHistograms(h_jpsi_invmass_vs_phi,h_jpsi_width_vs_phi,hphi,nbins);
  fitJpsiHistograms(h_jpsi_invmass_vs_phiplus,h_jpsi_width_vs_phiplus,hphiplus,nbins);
  fitJpsiHistograms(h_jpsi_invmass_vs_phiminus,h_jpsi_width_vs_phiminus,hphiminus,nbins);
  fitJpsiHistograms(h_jpsi_invmass_vs_phidiff,h_jpsi_width_vs_phidiff,hphidiff,nbins);
  fitJpsiHistograms(h_jpsi_invmass_vs_eta,h_jpsi_width_vs_eta,heta,nbins);
  fitJpsiHistograms(h_jpsi_invmass_vs_etaplus,h_jpsi_width_vs_etaplus,hetaplus,nbins);
  fitJpsiHistograms(h_jpsi_invmass_vs_etaminus,h_jpsi_width_vs_etaminus,hetaminus,nbins);  
  fitJpsiHistograms(h_jpsi_invmass_vs_maxeta,h_jpsi_width_vs_maxeta,hmaxeta,nbins);
  fitJpsiHistograms(h_jpsi_invmass_vs_etadiff,h_jpsi_width_vs_etadiff,hetadiff,nbins);
  fitJpsiHistograms(h_jpsi_invmass_vs_crt,h_jpsi_width_vs_crt,hcrt,nbins);
  fitJpsiHistograms(h_jpsi_invmass_vs_invpt,h_jpsi_width_vs_invpt,hinvpt,nbins);
  fitJpsiHistograms(h_jpsi_invmass_vs_eta_barrel,h_jpsi_width_vs_eta_barrel,heta_b,2);
  fitJpsiHistograms(h_jpsi_invmass_vs_eta_eca,h_jpsi_width_vs_eta_eca,heta_eca,2);
  fitJpsiHistograms(h_jpsi_invmass_vs_eta_ecc,h_jpsi_width_vs_eta_ecc,heta_ecc,2);
  fitJpsiHistograms(h_jpsi_invmass_vs_angle_eta,h_jpsi_width_vs_angle_eta,hangle_eta,nbins);
  fitJpsiHistograms(h_jpsi_invmass_vs_phisum,h_jpsi_width_vs_phisum,hsumphi,nbins);
  fitJpsiHistograms(h_jpsi_invmass_vs_sumeta,h_jpsi_width_vs_sumeta,hsumeta,nbins);
  fitJpsiHistograms(h_jpsi_invmass_vs_sumabseta,h_jpsi_width_vs_sumabseta,hsumabseta,nbins); 
  if (h_eff_pt->GetNbinsX()==h_probe_match_pt->GetNbinsX()) 
    {
      h_eff_pt->SetMarkerStyle(21);
      h_eff_eta->SetMarkerStyle(21);
      h_eff_phi->SetMarkerStyle(21);
      h_eff_pt->Divide(h_probe_match_pt,h_probe_all_pt,1,1,"B");
      h_eff_eta->Divide(h_probe_match_eta,h_probe_all_eta,1,1,"B");
      h_eff_phi->Divide(h_probe_match_phi,h_probe_all_phi,1,1,"B");
    }
  ProcessAsymHistograms(h_ptminus,h_ptplus,h_chargeasym_vs_pt);
  h_chargeasym_vs_pt->Write("",TObject::kOverwrite);

  //  f->cd(path.c_str());
  if( f->cd(path.c_str())==0 ) {
    //std::cerr<< "MonitoringFile::fitMergedFile_IDPerfMonJpsi(): "
    //<<"No such directory  \""<< path << "\"\n";
    return;
  }

  h_mass->Write("",TObject::kOverwrite);
  for (int i=0; i<nbins;i++)
    {
      hpt[i]->Write("",TObject::kOverwrite);
      hz0[i]->Write("",TObject::kOverwrite);
      hphi[i]->Write("",TObject::kOverwrite);
      hphiplus[i]->Write("",TObject::kOverwrite);
      hphiminus[i]->Write("",TObject::kOverwrite);
      hphidiff[i]->Write("",TObject::kOverwrite);
      heta[i]->Write("",TObject::kOverwrite);
      hetadiff[i]->Write("",TObject::kOverwrite);
      hmaxeta[i]->Write("",TObject::kOverwrite);
      hinvpt[i]->Write("",TObject::kOverwrite);
      hcrt[i]->Write("",TObject::kOverwrite);
      hetaplus[i]->Write("",TObject::kOverwrite);
      hetaminus[i]->Write("",TObject::kOverwrite);
      hangle_eta[i]->Write("",TObject::kOverwrite);
      hsumphi[i]->Write("",TObject::kOverwrite);
      hsumeta[i]->Write("",TObject::kOverwrite);
      hsumabseta[i]->Write("",TObject::kOverwrite);
      if (i<2) {
	heta_b[i]->Write("",TObject::kOverwrite);
	heta_ecc[i]->Write("",TObject::kOverwrite);
	heta_eca[i]->Write("",TObject::kOverwrite);
      }
    }

  h_jpsi_invmass_vs_pt->Write("",TObject::kOverwrite);
  h_jpsi_invmass_vs_z0->Write("",TObject::kOverwrite);
  h_jpsi_invmass_vs_phi->Write("",TObject::kOverwrite);
  h_jpsi_invmass_vs_phiplus->Write("",TObject::kOverwrite);
  h_jpsi_invmass_vs_phiminus->Write("",TObject::kOverwrite);
  h_jpsi_invmass_vs_phidiff->Write("",TObject::kOverwrite);
  h_jpsi_invmass_vs_eta->Write("",TObject::kOverwrite);
  h_jpsi_invmass_vs_eta_barrel->Write("",TObject::kOverwrite);
  h_jpsi_invmass_vs_eta_ecc->Write("",TObject::kOverwrite);
  h_jpsi_invmass_vs_eta_eca->Write("",TObject::kOverwrite);
  h_jpsi_invmass_vs_etadiff->Write("",TObject::kOverwrite);
  h_jpsi_invmass_vs_etaplus->Write("",TObject::kOverwrite);
  h_jpsi_invmass_vs_etaminus->Write("",TObject::kOverwrite);
  h_jpsi_invmass_vs_maxeta->Write("",TObject::kOverwrite);
  h_jpsi_invmass_vs_crt->Write("",TObject::kOverwrite);
  h_jpsi_invmass_vs_invpt->Write("",TObject::kOverwrite);
  h_jpsi_invmass_vs_angle_eta->Write("",TObject::kOverwrite);
  h_jpsi_invmass_vs_phisum->Write("",TObject::kOverwrite);
  h_jpsi_invmass_vs_sumeta->Write("",TObject::kOverwrite);
  h_jpsi_invmass_vs_sumabseta->Write("",TObject::kOverwrite);

  h_jpsi_width_vs_pt->Write("",TObject::kOverwrite);
  h_jpsi_width_vs_z0->Write("",TObject::kOverwrite);
  h_jpsi_width_vs_phi->Write("",TObject::kOverwrite);
  h_jpsi_width_vs_phiplus->Write("",TObject::kOverwrite);
  h_jpsi_width_vs_phiminus->Write("",TObject::kOverwrite);
  h_jpsi_width_vs_phidiff->Write("",TObject::kOverwrite);
  h_jpsi_width_vs_eta->Write("",TObject::kOverwrite);
  h_jpsi_width_vs_eta_barrel->Write("",TObject::kOverwrite);
  h_jpsi_width_vs_eta_ecc->Write("",TObject::kOverwrite);
  h_jpsi_width_vs_eta_eca->Write("",TObject::kOverwrite);
  h_jpsi_width_vs_etadiff->Write("",TObject::kOverwrite);
  h_jpsi_width_vs_etaplus->Write("",TObject::kOverwrite);
  h_jpsi_width_vs_etaminus->Write("",TObject::kOverwrite);
  h_jpsi_width_vs_maxeta->Write("",TObject::kOverwrite);
  h_jpsi_width_vs_crt->Write("",TObject::kOverwrite);
  h_jpsi_width_vs_invpt->Write("",TObject::kOverwrite);
  h_jpsi_width_vs_angle_eta->Write("",TObject::kOverwrite);
  h_jpsi_width_vs_phisum->Write("",TObject::kOverwrite);
  h_jpsi_width_vs_sumeta->Write("",TObject::kOverwrite);
  h_jpsi_width_vs_sumabseta->Write("",TObject::kOverwrite);

  h_eff_pt->Write("",TObject::kOverwrite);
  h_eff_eta->Write("",TObject::kOverwrite);
  h_eff_phi->Write("",TObject::kOverwrite);



  myCanvas->Close();
  delete myCanvas;
  
  f->Write();
  
//   delete f1;
}


//Upsilon
void
MonitoringFile::
fitMergedFile_IDPerfMonUpsilon( TFile* f, std::string run_dir, std::string TriggerName )
{
  std::string path;
  path=run_dir+"IDPerfMon/Upsilon/"+TriggerName;
  
  if( f->cd(path.c_str())==0 ) {
    //std::cerr<< "MonitoringFile::fitMergedFile_IDPerfMonUpsilon(): "
	            //<<"No such directory \""<< path << "\"\n";
    return;
  }
  
  TH1F* h_upsilon_invmass_vs_pt = new TH1F("Upsilon_invmass_vs_pt","Upsilon invmass-PDGmass vs. Upsilon Pt; p_{T}(#Upsilon) [GeV/c]; Mass Shift [GeV/c^{2}]", 6, 4.,20.);
  TH1F* h_upsilon_invmass_vs_z0 = new TH1F("Upsilon_invmass_vs_z0","Upsilon invmass-PDGmass vs. muon z0; z0(#mu) [mm]; Mass Shift [GeV/c^{2}]", 6,-100.,100.);
  TH1F* h_upsilon_invmass_vs_phi = new TH1F("Upsilon_invmass_vs_phi","Upsilon invmass-PDGmass vs. Upsilon phi; #phi(#Upsilon) [rad]; Mass Shift [GeV/c^{2}]", 6, -3.15, 3.15);
  TH1F* h_upsilon_invmass_vs_phiplus = new TH1F("Upsilon_invmass_vs_phiplus","Upsilon invmass-PDGmass vs. Muon+ phi; #phi(#mu^{+}) [rad]; Mass Shift [GeV/c^{2}]", 6, -3.15, 3.15);
  TH1F* h_upsilon_invmass_vs_phiminus = new TH1F("Upsilon_invmass_vs_phiminus","Upsilon invmass-PDGmass vs. Muon- phi; #phi(#mu^{-}) [rad]; Mass Shift [GeV/c^{2}]", 6, -3.15, 3.15);
  TH1F* h_upsilon_invmass_vs_phidiff = new TH1F("Upsilon_invmass_vs_phidiff","Upsilon invmass-PDGmass vs. |phi(+)-phi(-)|; |#phi(#mu^{+})-#phi(#mu^{-})| [rad]; Mass Shift [GeV/c^{2}]", 6, 0., 2.5);
  TH1F* h_upsilon_invmass_vs_eta = new TH1F("Upsilon_invmass_vs_eta","Upsilon invmass-PDGmass vs. Upsilon eta; #eta(#Upsilon); Mass Shift [GeV/c^{2}]", 8, -3.2, 3.2);
  TH1F* h_upsilon_invmass_vs_eta_barrel = new TH1F("Upsilon_invmass_vs_eta_barrel","Upsilon invmass-PDGmass vs. Upsilon eta (barrel); #eta(#Upsilon); Mass Shift [GeV/c^{2}]", 2, -0.8, 0.8);
  TH1F* h_upsilon_invmass_vs_eta_eca = new TH1F("Upsilon_invmass_vs_eta_eca","Upsilon invmass-PDGmass vs. Upsilon eta (endcap A); #eta(#Upsilon); Mass Shift [GeV/c^{2}]", 3, 0.8, 3.2);
  TH1F* h_upsilon_invmass_vs_eta_ecc = new TH1F("Upsilon_invmass_vs_eta_ecc","Upsilon invmass-PDGmass vs. Upsilon eta (endcap C); #eta(#Upsilon); Mass Shift [GeV/c^{2}]", 3,-3.2, -0.8);
  TH1F* h_upsilon_invmass_vs_etaplus= new TH1F("Upsilon_invmass_vs_etaplus","Upsilon invmass-PDGmass vs. Muon+ eta; #eta(#mu^{+}); Mass Shift [GeV/c^{2}]", 6,-2.4,2.4); 
  TH1F* h_upsilon_invmass_vs_etaminus= new TH1F("Upsilon_invmass_vs_etaminus","Upsilon invmass-PDGmass vs. Muon- eta; #eta(#mu^{-}); Mass Shift [GeV/c^{2}]", 6,-2.4,2.4); 
  TH1F* h_upsilon_invmass_vs_maxeta= new TH1F("Upsilon_invmass_vs_maxeta","Upsilon invmass-PDGmass vs. max Muon |eta|; max|#eta(#mu)|; Mass Shift [GeV/c^{2}]", 6,-2.4,2.4);
  TH1F* h_upsilon_invmass_vs_etadiff = new TH1F("Upsilon_invmass_vs_etadiff","Upsilon invmass-PDGmass vs. eta(+)-eta(-); #eta(#mu^{+})-#eta(#mu^{-}); Mass Shift [GeV/c^{2}]", 6 ,-1.5,1.5);
  TH1F* h_upsilon_invmass_vs_crt= new TH1F("Upsilon_invmass_vs_curvature","Upsilon invmass-PDGmass vs. crt(+)-|crt(-)|; 1/p_{T}(#mu^{+})-1/p_{T}(#mu^{-}) [GeV^{-1}]; Mass Shift [GeV/c^{2}]",6, -0.15, 0.15);
  TH1F* h_upsilon_invmass_vs_invpt=new TH1F("Upsilon_invmass_vs_invpt","Upsilon invmass-PDGmass vs. muon  1/Pt; 1/p_{T}(#mu) [(GeV/c)^{-1}]; Mass Shift [GeV/c^{2}]",6,0.05,0.25);
  TH1F* h_upsilon_invmass_vs_angle_eta=new TH1F("Upsilon_invmass_vs_angle_eta","Upsilon invmass-PDGmass vs. arccos(#Delta#eta/#DeltaR);arccos(#Delta#eta/#DeltaR); Mass Shift [GeV/c^{2}]",6,0.,90.);

  TH1F* h_upsilon_width_vs_pt = new TH1F("Upsilon_width_vs_pt","Upsilon width vs. Upsilon Pt; p_{T}(#Upsilon) [GeV/c]; Width [GeV/c^{2}]", 6, 4.,20.);
  TH1F* h_upsilon_width_vs_z0 = new TH1F("Upsilon_width_vs_z0","Upsilon width vs. muon z0; z0(#mu) [mm]; Width [GeV/c^{2}]", 6,-100.,100.);
  TH1F* h_upsilon_width_vs_phi = new TH1F("Upsilon_width_vs_phi","Upsilon width vs. Upsilon phi; #phi(#Upsilon) [rad]; Width [GeV/c^{2}]", 6, -3.15, 3.15);
  TH1F* h_upsilon_width_vs_phiplus = new TH1F("Upsilon_width_vs_phiplus","Upsilon width vs. Muon+ phi; #phi(#mu^{+}) [rad]; Width [GeV/c^{2}]", 6, -3.15, 3.15);
  TH1F* h_upsilon_width_vs_phiminus = new TH1F("Upsilon_width_vs_phiminus","Upsilon width vs. Muon- phi; #phi(#mu^{-}) [rad]; Width [GeV/c^{2}]", 6, -3.15, 3.15);
  TH1F* h_upsilon_width_vs_phidiff = new TH1F("Upsilon_width_vs_phidiff","Upsilon width vs. |phi(+)-phi(-)|; |#phi(#mu^{+})-#phi(#mu^{-})| [rad]; Width [GeV/c^{2}]", 6, 0., 2.5);
  TH1F* h_upsilon_width_vs_eta = new TH1F("Upsilon_width_vs_eta","Upsilon width vs. Upsilon eta; #eta(#Upsilon); Width [GeV/c^{2}]", 8, -3.2, 3.2);
  TH1F* h_upsilon_width_vs_eta_barrel = new TH1F("Upsilon_width_vs_eta_barrel","Upsilon width vs. Upsilon eta (barrel); #eta(#Upsilon); Width [GeV/c^{2}]", 2, -0.8, 0.8);
  TH1F* h_upsilon_width_vs_eta_eca = new TH1F("Upsilon_width_vs_eta_eca","Upsilon width vs. Upsilon eta (endcap A); #eta(#Upsilon); Width [GeV/c^{2}]", 3, 0.8, 3.2);
  TH1F* h_upsilon_width_vs_eta_ecc = new TH1F("Upsilon_width_vs_eta_ecc","Upsilon width vs. Upsilon eta (endcap C); #eta(#Upsilon); Width [GeV/c^{2}]", 3, -3.2, -0.8);
  TH1F* h_upsilon_width_vs_etaplus= new TH1F("Upsilon_width_vs_etaplus","Upsilon width vs. Muon+ eta; #eta(#mu^{+}); Width [GeV/c^{2}]", 6,-2.4,2.4); 
  TH1F* h_upsilon_width_vs_etaminus= new TH1F("Upsilon_width_vs_etaminus","Upsilon width vs. Muon- eta; #eta(#mu^{-}); Width [GeV/c^{2}]", 6,-2.4,2.4); 
  TH1F* h_upsilon_width_vs_maxeta= new TH1F("Upsilon_width_vs_maxeta","Upsilon width vs. max Muon |eta|; max|#eta(#mu)|; Width [GeV/c^{2}]", 6,-2.4,2.4);
  TH1F* h_upsilon_width_vs_etadiff = new TH1F("Upsilon_width_vs_etadiff","Upsilon width vs. eta(+)-eta(-); #eta(#mu^{+})-#eta(#mu^{-}); Width [GeV/c^{2}]", 6 ,-1.5,1.5);
  TH1F* h_upsilon_width_vs_crt= new TH1F("Upsilon_width_vs_curvature","Upsilon width vs. crt(+)-|crt(-)|; 1/p_{T}(#mu^{+})-1/p_{T}(#mu^{-}) [GeV^{-1}]; Width [GeV/c^{2}]",6, -0.15, 0.15);
  TH1F* h_upsilon_width_vs_invpt=new TH1F("Upsilon_width_vs_invpt","Upsilon width vs. muon  1/Pt; 1/p_{T}(#mu) [(GeV/c)^{-1}]; Width [GeV/c^{2}]",6,0.05,0.25);
  TH1F* h_upsilon_width_vs_angle_eta=new TH1F("Upsilon_width_vs_angle_eta","Upsilon width vs. arccos(#Delta#eta/#DeltaR);arccos(#Delta#eta/#DeltaR); width [GeV/c^{2}]",6,0.,90.);

  const int nbins=6;
  TH1F* hpt[nbins];
  TH1F* hz0[nbins];
  TH1F* hphi[nbins];
  TH1F* hphiplus[nbins];
  TH1F* hphiminus[nbins];
  TH1F* hphidiff[nbins];
  TH1F* heta[8];
  TH1F* heta_b[2];
  TH1F* heta_ecc[3];
  TH1F* heta_eca[3];
  TH1F* hetadiff[nbins];
  TH1F* hmaxeta[nbins];
  TH1F* hinvpt[nbins];
  TH1F* hcrt[nbins];
  TH1F* hetaplus[nbins];
  TH1F* hetaminus[nbins];
  TH1F* hangle_eta[nbins];

  if (!CheckHistogram(f,(path+"/Upsilon_invmass").c_str())) return; 
  TH1F* h_mass = (TH1F*)(f->Get((path+"/Upsilon_invmass").c_str())->Clone());
  if (CheckHistogram(f,(path+"/Nevents").c_str())) {
    TH1F* h_Nevents=(TH1F*)f->Get((path+"/Nevents").c_str());
    TH1F* h_rate = new TH1F("rate","production rate",1,0,1);
    double Ntot =h_Nevents->GetEntries();
    double yield = h_mass->Integral(h_mass->FindBin(9.1), h_mass->FindBin(9.8));
    double rate=yield/Ntot;
    double rate_error=sqrt(1/yield+1/Ntot)*yield/Ntot;
    h_rate->SetBinContent(1,rate);
    h_rate->SetBinError(1,rate_error);
    h_rate->Write("",TObject::kOverwrite);
  }
//   TH1F* h_mass_scaled = (TH1F*)(f->Get((path+"/Upsilon_invmass").c_str())->Clone("Upsilon_invmass_scaled_copy"));
//   h_mass_scaled->SetMarkerStyle(21);
//   TString title(h_mass_scaled->GetTitle());
//   if (CheckHistogram(f,(path+"/Upsilon_invmass_scaled").c_str())) {
//     if (CheckHistogram(f,(path+"/Nevents").c_str())) {
//       TH1F* h_Nevents=(TH1F*)f->Get((path+"/Nevents").c_str());
//       double Ntot =h_Nevents->GetEntries();
//       if (Ntot!=0.) h_mass_scaled->Scale(1./Ntot);
//       h_mass_scaled->SetTitle(title+" (per event)");
//       h_mass_scaled->Write("Upsilon_invmass_scaled",TObject::kOverwrite);
//     }  
//   }
//   delete h_mass_scaled;
  if (CheckHistogram( f,(path+"/Upsilon_invmass").c_str())) {
    TH1F* h_mass_rebin = (TH1F*)(f->Get((path+"/Upsilon_invmass").c_str())->Clone("Upsilon_invmass_rebin"));
    TString title = h_mass_rebin->GetTitle();
    h_mass_rebin ->SetMarkerStyle(21);
    h_mass_rebin->Rebin(8);
    h_mass_rebin->SetTitle(title+" (larger binning for low Stats.)");
    h_mass_rebin->Write("Upsilon_invmass_rebin",TObject::kOverwrite);
  }

  char cpt[100],cz0[100],cphi[100],cphidiff[100],ceta[100],cetadiff[100],cmaxeta[100],ccrt[100],cinvpt[100],cphiplus[100],cphiminus[100],cetaplus[100],cetaminus[100],ceta_b[100],ceta_ecc[100],ceta_eca[100],cangle_eta[100];

  for (int i=0;i<nbins;i++)
    {
      sprintf(cpt,(path+"/pt_bin%d").c_str(),i);
      sprintf(cz0,(path+"/z0_bin%d").c_str(),i);
      sprintf(cphi,(path+"/phi_bin%d").c_str(),i);
      sprintf(cphiplus,(path+"/phiplus_bin%d").c_str(),i);
      sprintf(cphiminus,(path+"/phiminus_bin%d").c_str(),i);
      sprintf(cphidiff,(path+"/phidiff_bin%d").c_str(),i);
      sprintf(cetaplus,(path+"/etaplus_bin%d").c_str(),i);
      sprintf(cetaminus,(path+"/etaminus_bin%d").c_str(),i);
      sprintf(cmaxeta,(path+"/maxeta_bin%d").c_str(),i);
      sprintf(cetadiff,(path+"/etadiff_bin%d").c_str(),i);
      sprintf(ccrt,(path+"/crt_bin%d").c_str(),i);
      sprintf(cinvpt,(path+"/invpt_bin%d").c_str(),i);
      sprintf(cangle_eta,(path+"/angle_eta_bin%d").c_str(),i);

      if (!CheckHistogram(f,cpt)) return;     
      if (!CheckHistogram(f,cz0)) return;
      if (!CheckHistogram(f,cphi)) return;
      if (!CheckHistogram(f,cphiplus)) return;
      if (!CheckHistogram(f,cphiminus)) return;
      if (!CheckHistogram(f,cphidiff)) return;
      if (!CheckHistogram(f,cetaplus)) return;
      if (!CheckHistogram(f,cetaminus)) return;
      if (!CheckHistogram(f,cmaxeta)) return;
      if (!CheckHistogram(f,cetadiff)) return;
      if (!CheckHistogram(f,ccrt)) return;
      if (!CheckHistogram(f,cinvpt)) return;
      if (!CheckHistogram(f,cangle_eta)) return;

      hpt[i] =(TH1F*)(f->Get(cpt)->Clone());
      hz0[i] =(TH1F*)(f->Get(cz0)->Clone());
      hphi[i]=(TH1F*)(f->Get(cphi)->Clone());
      hphiplus[i]=(TH1F*)(f->Get(cphiplus)->Clone());
      hphiminus[i]=(TH1F*)(f->Get(cphiminus)->Clone());
      hphidiff[i]=(TH1F*)(f->Get(cphidiff)->Clone());
      hetaplus[i]=(TH1F*)(f->Get(cetaplus)->Clone());
      hetaminus[i]=(TH1F*)(f->Get(cetaminus)->Clone());
      hetadiff[i]=(TH1F*)(f->Get(cetadiff)->Clone());
      hmaxeta[i]=(TH1F*)(f->Get(cmaxeta)->Clone());
      hcrt[i] = (TH1F*)(f->Get(ccrt)->Clone());
      hinvpt[i] =(TH1F*)(f->Get(cinvpt)->Clone());
      hangle_eta[i]=(TH1F*)(f->Get(cangle_eta)->Clone());

      if (i<2) {
	sprintf(ceta_b,(path+"/eta_b_bin%d").c_str(),i);
        if (!CheckHistogram(f,ceta_b)) return;
	heta_b[i] = (TH1F*)(f->Get(ceta_b)->Clone());
      }
      if(i<3) {
	sprintf(ceta_ecc,(path+"/eta_ecc_bin%d").c_str(),i);
	sprintf(ceta_eca,(path+"/eta_eca_bin%d").c_str(),i);
	if (!CheckHistogram(f,ceta_ecc)) return;
        if (!CheckHistogram(f,ceta_eca)) return;
	heta_ecc[i] =(TH1F*)(f->Get(ceta_ecc)->Clone());
	heta_eca[i] =(TH1F*)(f->Get(ceta_eca)->Clone());
      }
    }
  for (int i=0; i<8;i++)
    { 	
      sprintf(ceta,(path+"/eta_bin%d").c_str(),i);
      if (!CheckHistogram(f,ceta)) return;
      heta[i] =(TH1F*)(f->Get(ceta)->Clone());
    }
   
//   TF1 *f1 = new TF1("f1","gaus",9.,10.);
//   f1->SetParameter(1,9.046);
//   f1->SetParameter(2,0.15);
//   f1->SetLineColor(2);
//   f1->SetParNames("Constant","Mean","Width");
  h_mass->SetMarkerStyle(21);
  
  TCanvas *myCanvas = new TCanvas("MyCanvas");
  myCanvas->cd();
  
//   h_mass->Fit(f1,"RQMN");


  fitUpsilonHistograms(h_upsilon_invmass_vs_pt,h_upsilon_width_vs_pt,hpt,nbins);
  fitUpsilonHistograms(h_upsilon_invmass_vs_z0,h_upsilon_width_vs_z0,hz0,nbins);
  fitUpsilonHistograms(h_upsilon_invmass_vs_phi,h_upsilon_width_vs_phi,hphi,nbins);
  fitUpsilonHistograms(h_upsilon_invmass_vs_phiplus,h_upsilon_width_vs_phiplus,hphiplus,nbins);
  fitUpsilonHistograms(h_upsilon_invmass_vs_phiminus,h_upsilon_width_vs_phiminus,hphiminus,nbins);
  fitUpsilonHistograms(h_upsilon_invmass_vs_phidiff,h_upsilon_width_vs_phidiff,hphidiff,nbins);
  fitUpsilonHistograms(h_upsilon_invmass_vs_eta,h_upsilon_width_vs_eta,heta,8);
  fitUpsilonHistograms(h_upsilon_invmass_vs_eta_barrel,h_upsilon_width_vs_eta_barrel,heta_b,2);
  fitUpsilonHistograms(h_upsilon_invmass_vs_eta_eca,h_upsilon_width_vs_eta_eca,heta_eca,3);
  fitUpsilonHistograms(h_upsilon_invmass_vs_eta_ecc,h_upsilon_width_vs_eta_ecc,heta_ecc,3);
  fitUpsilonHistograms(h_upsilon_invmass_vs_etaplus,h_upsilon_width_vs_etaplus,hetaplus,nbins);
  fitUpsilonHistograms(h_upsilon_invmass_vs_etaminus,h_upsilon_width_vs_etaminus,hetaminus,nbins);  
  fitUpsilonHistograms(h_upsilon_invmass_vs_maxeta,h_upsilon_width_vs_maxeta,hmaxeta,nbins);
  fitUpsilonHistograms(h_upsilon_invmass_vs_etadiff,h_upsilon_width_vs_etadiff,hetadiff,nbins);
  fitUpsilonHistograms(h_upsilon_invmass_vs_crt,h_upsilon_width_vs_crt,hcrt,nbins);
  fitUpsilonHistograms(h_upsilon_invmass_vs_invpt,h_upsilon_width_vs_invpt,hinvpt,nbins);
  fitUpsilonHistograms(h_upsilon_invmass_vs_angle_eta,h_upsilon_width_vs_angle_eta,hangle_eta,nbins);
 

  // f->cd(path.c_str());
  if( f->cd(path.c_str())==0 ) {
    //std::cerr<< "MonitoringFile::fitMergedFile_IDPerfMonUpsilon(): "
	     //<<"No such directory \""<< path << "\"\n";
    return;
  }

  h_mass->Write("",TObject::kOverwrite);
  for (int i=0; i<nbins;i++)
    {
      hpt[i]->Write("",TObject::kOverwrite);
      hz0[i]->Write("",TObject::kOverwrite);
      hphi[i]->Write("",TObject::kOverwrite);
      hphiplus[i]->Write("",TObject::kOverwrite);
      hphiminus[i]->Write("",TObject::kOverwrite);
      hphidiff[i]->Write("",TObject::kOverwrite);
      hetadiff[i]->Write("",TObject::kOverwrite);
      hmaxeta[i]->Write("",TObject::kOverwrite);
      hinvpt[i]->Write("",TObject::kOverwrite);
      hcrt[i]->Write("",TObject::kOverwrite);
      hetaplus[i]->Write("",TObject::kOverwrite);
      hetaminus[i]->Write("",TObject::kOverwrite);
      hangle_eta[i]->Write("",TObject::kOverwrite);
      if (i<2) {
	heta_b[i]->Write("",TObject::kOverwrite);
      }
      if (i<3) {
	heta_ecc[i]->Write("",TObject::kOverwrite);
	heta_eca[i]->Write("",TObject::kOverwrite);
      }
    }
  for (int i=0;i<8;i++) 
    {
      heta[i]->Write("",TObject::kOverwrite);
    } 
  h_upsilon_invmass_vs_pt->Write("",TObject::kOverwrite);
  h_upsilon_invmass_vs_z0->Write("",TObject::kOverwrite);
  h_upsilon_invmass_vs_phi->Write("",TObject::kOverwrite);
  h_upsilon_invmass_vs_phiplus->Write("",TObject::kOverwrite);
  h_upsilon_invmass_vs_phiminus->Write("",TObject::kOverwrite);
  h_upsilon_invmass_vs_phidiff->Write("",TObject::kOverwrite);
  h_upsilon_invmass_vs_eta->Write("",TObject::kOverwrite);
  h_upsilon_invmass_vs_eta_barrel->Write("",TObject::kOverwrite);
  h_upsilon_invmass_vs_eta_ecc->Write("",TObject::kOverwrite);
  h_upsilon_invmass_vs_eta_eca->Write("",TObject::kOverwrite);
  h_upsilon_invmass_vs_etadiff->Write("",TObject::kOverwrite);
  h_upsilon_invmass_vs_etaplus->Write("",TObject::kOverwrite);
  h_upsilon_invmass_vs_etaminus->Write("",TObject::kOverwrite);
  h_upsilon_invmass_vs_maxeta->Write("",TObject::kOverwrite);
  h_upsilon_invmass_vs_crt->Write("",TObject::kOverwrite);
  h_upsilon_invmass_vs_invpt->Write("",TObject::kOverwrite);
  h_upsilon_invmass_vs_angle_eta->Write("",TObject::kOverwrite);

  h_upsilon_width_vs_pt->Write("",TObject::kOverwrite);
  h_upsilon_width_vs_z0->Write("",TObject::kOverwrite);
  h_upsilon_width_vs_phi->Write("",TObject::kOverwrite);
  h_upsilon_width_vs_phiplus->Write("",TObject::kOverwrite);
  h_upsilon_width_vs_phiminus->Write("",TObject::kOverwrite);
  h_upsilon_width_vs_phidiff->Write("",TObject::kOverwrite);
  h_upsilon_width_vs_eta->Write("",TObject::kOverwrite);
  h_upsilon_width_vs_eta_barrel->Write("",TObject::kOverwrite);
  h_upsilon_width_vs_eta_ecc->Write("",TObject::kOverwrite);
  h_upsilon_width_vs_eta_eca->Write("",TObject::kOverwrite);
  h_upsilon_width_vs_etadiff->Write("",TObject::kOverwrite);
  h_upsilon_width_vs_etaplus->Write("",TObject::kOverwrite);
  h_upsilon_width_vs_etaminus->Write("",TObject::kOverwrite);
  h_upsilon_width_vs_maxeta->Write("",TObject::kOverwrite);
  h_upsilon_width_vs_crt->Write("",TObject::kOverwrite);
  h_upsilon_width_vs_invpt->Write("",TObject::kOverwrite);
  h_upsilon_width_vs_angle_eta->Write("",TObject::kOverwrite);
 
  myCanvas->Close();
  delete myCanvas;
  
  f->Write();
  
//   delete f1;

}

void
MonitoringFile::
fitMergedFile_IDPerfMonZee ( TFile* f, std::string run_dir,std::string TriggerName )
{

  std::string path;
  path= run_dir + "IDPerfMon/Zee/" + TriggerName;
  if(  f->cd(path.c_str())==0 ) {
    //std::cerr << "MonitoringFile::fitMergedFile_IDPerfMonZee(): "
	      //<< "No such directory \"" << path << "\"\n";
    return;
  }
  
  f->cd(path.c_str());

  if (!CheckHistogram(f,(path+"/Zee_invmass").c_str())) return; 
  TH1F* h_mass = (TH1F*)(f->Get((path+"/Zee_invmass").c_str())->Clone());
  if (CheckHistogram(f,(path+"/Nevents").c_str())) {
    TH1F* h_Nevents=(TH1F*)f->Get((path+"/Nevents").c_str());
    TH1F* h_rate = new TH1F("rate","production rate",1,0,1);
    double Ntot =h_Nevents->GetEntries();
    double yield = h_mass->Integral(h_mass->FindBin(80), h_mass->FindBin(105));
    double rate=yield/Ntot;
    double rate_error=sqrt(1/yield+1/Ntot)*yield/Ntot;
    h_rate->SetBinContent(1,rate);
    h_rate->SetBinError(1,rate_error);
    h_rate->Write("",TObject::kOverwrite);
  }
//   TH1F* h_mass_scaled = (TH1F*)(f->Get((path+"/Zee_trk_invmass").c_str())->Clone("Zee_trk_invmass_scaled_copy"));
//   TString title(h_mass_scaled->GetTitle());
//   if (CheckHistogram(f,(path+"/Zee_trk_invmass_scaled").c_str())) {
//     if (CheckHistogram(f,(path+"/Nevents").c_str())) {
//       TH1F* h_Nevents=(TH1F*)f->Get((path+"/Nevents").c_str());
//       double Ntot =h_Nevents->GetEntries();
//       if (Ntot!=0.) h_mass_scaled->Scale(1./Ntot);
//       h_mass_scaled->SetTitle(title+" (per event)");
//       h_mass_scaled->Write("Zee_trk_invmass_scaled",TObject::kOverwrite);
//     }  
//   }
//   delete h_mass_scaled;

  enum eta_region { incl, barrel, eca, ecc, Nregions };
  std::vector<std::string> region_strings;
  region_strings.push_back("incl");
  region_strings.push_back("barrel");
  region_strings.push_back("eca");
  region_strings.push_back("ecc");

  std::vector<TH1F*> h_Zee_Eopdiff;
  std::vector<TH1F*> h_Zee_Eop_plus;
  std::vector<TH1F*> h_Zee_Eop_minus;

  std::vector<TProfile*> h_Zee_Eopdiff_vs_p;
  std::vector<TProfile*> h_Zee_meanEop_vs_p_plus;
  std::vector<TProfile*> h_Zee_meanEop_vs_p_minus;

  std::vector<TProfile*> h_Zee_Eopdiff_vs_invp;
  std::vector<TProfile*> h_Zee_meanEop_vs_invp_plus;
  std::vector<TProfile*> h_Zee_meanEop_vs_invp_minus;

  std::vector<TProfile*> h_Zee_Eopdiff_vs_E;
  std::vector<TProfile*> h_Zee_meanEop_vs_E_plus;
  std::vector<TProfile*> h_Zee_meanEop_vs_E_minus;

  std::vector<TProfile*> h_Zee_Eopdiff_vs_phi;
  std::vector<TProfile*> h_Zee_meanEop_vs_phi_plus;
  std::vector<TProfile*> h_Zee_meanEop_vs_phi_minus;

  std::vector<TProfile*> h_Zee_Eopdiff_vs_eta;
  std::vector<TProfile*> h_Zee_meanEop_vs_eta_plus;
  std::vector<TProfile*> h_Zee_meanEop_vs_eta_minus;

  std::string str;
  
  for (int region=0; region!=Nregions; ++region) {

    str = path + "/Zee_Eop_plus_" + region_strings[region];
    if (!CheckHistogram(f,str.c_str())) return;     
    h_Zee_Eop_plus.push_back((TH1F*)(f->Get(str.c_str())));
    h_Zee_Eop_plus[region]->SetTitle(h_Zee_Eop_plus[region]->GetTitle());
    str = path + "/Zee_Eop_minus_" + region_strings[region];
    if (!CheckHistogram(f,str.c_str())) return;     
    h_Zee_Eop_minus.push_back((TH1F*)(f->Get(str.c_str())));
    h_Zee_Eop_minus[region]->SetTitle(h_Zee_Eop_minus[region]->GetTitle());
    str = path + "/Zee_Eopdiff_" + region_strings[region];
    if (!CheckHistogram(f,str.c_str())) return;     
    h_Zee_Eopdiff.push_back((TH1F*)(f->Get(str.c_str())));
    h_Zee_Eopdiff[region]->SetTitle(h_Zee_Eopdiff[region]->GetTitle());

    h_Zee_Eopdiff[region]->Add(h_Zee_Eop_plus[region],h_Zee_Eop_minus[region],1.,-1);

    if (region == incl) {

      str = path + "/Zee_meanEop_vs_p_plus";
      if (!CheckHistogram(f,str.c_str())) return;     
      h_Zee_meanEop_vs_p_plus.push_back((TProfile*)(f->Get(str.c_str())));
      h_Zee_meanEop_vs_p_plus[region]->SetTitle(h_Zee_meanEop_vs_p_plus[region]->GetTitle());
      str = path + "/Zee_meanEop_vs_p_minus";
      if (!CheckHistogram(f,str.c_str())) return;     
      h_Zee_meanEop_vs_p_minus.push_back((TProfile*)(f->Get(str.c_str())));
      h_Zee_meanEop_vs_p_minus[region]->SetTitle(h_Zee_meanEop_vs_p_minus[region]->GetTitle());
      str = path + "/Zee_Eopdiff_vs_p";
      if (!CheckHistogram(f,str.c_str())) return;     
      h_Zee_Eopdiff_vs_p.push_back((TProfile*)(f->Get(str.c_str())));
      h_Zee_Eopdiff_vs_p[region]->SetTitle(h_Zee_Eopdiff_vs_p[region]->GetTitle());
      
      str = path + "/Zee_meanEop_vs_invp_plus";
      if (!CheckHistogram(f,str.c_str())) return;     
      h_Zee_meanEop_vs_invp_plus.push_back((TProfile*)(f->Get(str.c_str())));
      h_Zee_meanEop_vs_invp_plus[region]->SetTitle(h_Zee_meanEop_vs_invp_plus[region]->GetTitle());
      str = path + "/Zee_meanEop_vs_invp_minus";
      if (!CheckHistogram(f,str.c_str())) return;     
      h_Zee_meanEop_vs_invp_minus.push_back((TProfile*)(f->Get(str.c_str())));
      h_Zee_meanEop_vs_invp_minus[region]->SetTitle(h_Zee_meanEop_vs_invp_minus[region]->GetTitle());
      str = path + "/Zee_Eopdiff_vs_invp";
      if (!CheckHistogram(f,str.c_str())) return;     
      h_Zee_Eopdiff_vs_invp.push_back((TProfile*)(f->Get(str.c_str())));
      h_Zee_Eopdiff_vs_invp[region]->SetTitle(h_Zee_Eopdiff_vs_invp[region]->GetTitle());

      str = path + "/Zee_meanEop_vs_E_plus";
      if (!CheckHistogram(f,str.c_str())) return;     
      h_Zee_meanEop_vs_E_plus.push_back((TProfile*)(f->Get(str.c_str())));
      h_Zee_meanEop_vs_E_plus[region]->SetTitle(h_Zee_meanEop_vs_E_plus[region]->GetTitle());
      str = path + "/Zee_meanEop_vs_E_minus";
      if (!CheckHistogram(f,str.c_str())) return;     
      h_Zee_meanEop_vs_E_minus.push_back((TProfile*)(f->Get(str.c_str())));
      h_Zee_meanEop_vs_E_minus[region]->SetTitle(h_Zee_meanEop_vs_E_minus[region]->GetTitle());
      str = path + "/Zee_Eopdiff_vs_E";
      if (!CheckHistogram(f,str.c_str())) return;     
      h_Zee_Eopdiff_vs_E.push_back((TProfile*)(f->Get(str.c_str())));
      h_Zee_Eopdiff_vs_E[region]->SetTitle(h_Zee_Eopdiff_vs_E[region]->GetTitle());
      
      str = path + "/Zee_meanEop_vs_phi_plus";
      if (!CheckHistogram(f,str.c_str())) return;     
      h_Zee_meanEop_vs_phi_plus.push_back((TProfile*)(f->Get(str.c_str())));
      h_Zee_meanEop_vs_phi_plus[region]->SetTitle(h_Zee_meanEop_vs_phi_plus[region]->GetTitle());
      str = path + "/Zee_meanEop_vs_phi_minus";
      if (!CheckHistogram(f,str.c_str())) return;     
      h_Zee_meanEop_vs_phi_minus.push_back((TProfile*)(f->Get(str.c_str())));
      h_Zee_meanEop_vs_phi_minus[region]->SetTitle(h_Zee_meanEop_vs_phi_minus[region]->GetTitle());
      str = path + "/Zee_Eopdiff_vs_phi";
      if (!CheckHistogram(f,str.c_str())) return;     
      h_Zee_Eopdiff_vs_phi.push_back((TProfile*)(f->Get(str.c_str())));
      h_Zee_Eopdiff_vs_phi[region]->SetTitle(h_Zee_Eopdiff_vs_phi[region]->GetTitle());
            
      str = path + "/Zee_meanEop_vs_eta_plus";
      if (!CheckHistogram(f,str.c_str())) return;     
      h_Zee_meanEop_vs_eta_plus.push_back((TProfile*)(f->Get(str.c_str())));
      h_Zee_meanEop_vs_eta_plus[region]->SetTitle(h_Zee_meanEop_vs_eta_plus[region]->GetTitle());
      str = path + "/Zee_meanEop_vs_eta_minus";
      if (!CheckHistogram(f,str.c_str())) return;     
      h_Zee_meanEop_vs_eta_minus.push_back((TProfile*)(f->Get(str.c_str())));
      h_Zee_meanEop_vs_eta_minus[region]->SetTitle(h_Zee_meanEop_vs_eta_minus[region]->GetTitle());
      str = path + "/Zee_Eopdiff_vs_eta";
      if (!CheckHistogram(f,str.c_str())) return;     
      h_Zee_Eopdiff_vs_eta.push_back((TProfile*)(f->Get(str.c_str())));
      h_Zee_Eopdiff_vs_eta[region]->SetTitle(h_Zee_Eopdiff_vs_eta[region]->GetTitle());
       
      h_Zee_Eopdiff_vs_p[region]->Add(h_Zee_meanEop_vs_p_plus[region],h_Zee_meanEop_vs_p_minus[region],1.,-1);
      h_Zee_Eopdiff_vs_invp[region]->Add(h_Zee_meanEop_vs_invp_plus[region],h_Zee_meanEop_vs_invp_minus[region],1.,-1);
      h_Zee_Eopdiff_vs_E[region]->Add(h_Zee_meanEop_vs_E_plus[region],h_Zee_meanEop_vs_E_minus[region],1.,-1);
      h_Zee_Eopdiff_vs_phi[region]->Add(h_Zee_meanEop_vs_phi_plus[region],h_Zee_meanEop_vs_phi_minus[region],1.,-1);
      h_Zee_Eopdiff_vs_eta[region]->Add(h_Zee_meanEop_vs_eta_plus[region],h_Zee_meanEop_vs_eta_minus[region],1.,-1);
       
    }
    
  }

  for (int region=0; region!=Nregions; ++region) {
    h_Zee_Eopdiff[region]->Write("",TObject::kOverwrite);
    if (region == incl) {
      h_Zee_Eopdiff_vs_p[region]->Write("",TObject::kOverwrite);
      h_Zee_Eopdiff_vs_invp[region]->Write("",TObject::kOverwrite);
      h_Zee_Eopdiff_vs_E[region]->Write("",TObject::kOverwrite);
      h_Zee_Eopdiff_vs_phi[region]->Write("",TObject::kOverwrite);
      h_Zee_Eopdiff_vs_eta[region]->Write("",TObject::kOverwrite); 
    }
  }
  
  f->Write();

}

void
MonitoringFile::
fitMergedFile_IDPerfMonWenu ( TFile* f, std::string run_dir,std::string TriggerName )
{

  std::string path;
  path= run_dir + "IDPerfMon/Wenu/" + TriggerName;
  if(  f->cd(path.c_str())==0 ) {
    //std::cerr << "MonitoringFile::fitMergedFile_IDPerfMonWenu(): "
	      //<< "No such directory \"" << path << "\"\n";
    return;
  }
  
  f->cd(path.c_str());

  if (!CheckHistogram(f,(path+"/Wenu_trk_transmass_sel").c_str())) return; 
  TH1F* h_mass = (TH1F*)(f->Get((path+"/Wenu_trk_transmass_sel").c_str())->Clone());
  if (CheckHistogram(f,(path+"/Nevents").c_str())) {
    TH1F* h_Nevents=(TH1F*)f->Get((path+"/Nevents").c_str());
    TH1F* h_rate = new TH1F("rate","production rate",1,0,1);
    double Ntot =h_Nevents->GetEntries();
    double yield = h_mass->Integral(h_mass->FindBin(50), h_mass->FindBin(80));
    double rate=yield/Ntot;
    double rate_error=sqrt(1/yield+1/Ntot)*yield/Ntot;
    h_rate->SetBinContent(1,rate);
    h_rate->SetBinError(1,rate_error);
    h_rate->Write("",TObject::kOverwrite);
  }
//   TH1F* h_mass_scaled = (TH1F*)(f->Get((path+"/Wenu_trk_transmass_sel").c_str())->Clone("Wenu_trk_transmass_sel_scaled_copy"));
//   TString title(h_mass_scaled->GetTitle());
//   if (CheckHistogram(f,(path+"/Wenu_trk_transmass_sel_scaled").c_str())) {
//     if (CheckHistogram(f,(path+"/Nevents").c_str())) {
//       TH1F* h_Nevents=(TH1F*)f->Get((path+"/Nevents").c_str());
//       double Ntot =h_Nevents->GetEntries();
//       if (Ntot!=0.) h_mass_scaled->Scale(1./Ntot);
//       h_mass_scaled->SetTitle(title+" (per event)");
//       h_mass_scaled->Write("Wenu_trk_transmass_sel_scaled",TObject::kOverwrite);
//     }  
//   }
//   delete h_mass_scaled;

  enum eta_region { incl, barrel, eca, ecc, Nregions };
  std::vector<std::string> region_strings;
  region_strings.push_back("incl");
  region_strings.push_back("barrel");
  region_strings.push_back("eca");
  region_strings.push_back("ecc");

  std::vector<TH1F*> h_Wenu_Eopdiff;
  std::vector<TH1F*> h_Wenu_Eop_plus;
  std::vector<TH1F*> h_Wenu_Eop_minus;

  std::vector<TProfile*> h_Wenu_Eopdiff_vs_p;
  std::vector<TProfile*> h_Wenu_meanEop_vs_p_plus;
  std::vector<TProfile*> h_Wenu_meanEop_vs_p_minus;

  std::vector<TProfile*> h_Wenu_Eopdiff_vs_invp;
  std::vector<TProfile*> h_Wenu_meanEop_vs_invp_plus;
  std::vector<TProfile*> h_Wenu_meanEop_vs_invp_minus;

  std::vector<TProfile*> h_Wenu_Eopdiff_vs_E;
  std::vector<TProfile*> h_Wenu_meanEop_vs_E_plus;
  std::vector<TProfile*> h_Wenu_meanEop_vs_E_minus;

  std::vector<TProfile*> h_Wenu_Eopdiff_vs_phi;
  std::vector<TProfile*> h_Wenu_meanEop_vs_phi_plus;
  std::vector<TProfile*> h_Wenu_meanEop_vs_phi_minus;

  std::vector<TProfile*> h_Wenu_Eopdiff_vs_eta;
  std::vector<TProfile*> h_Wenu_meanEop_vs_eta_plus;
  std::vector<TProfile*> h_Wenu_meanEop_vs_eta_minus;

  std::string str;

  for (int region=0; region!=Nregions; ++region) {

    str = path + "/Wenu_Eop_plus_" + region_strings[region];
    if (!CheckHistogram(f,str.c_str())) return;     
    h_Wenu_Eop_plus.push_back((TH1F*)(f->Get(str.c_str())));
    h_Wenu_Eop_plus[region]->SetTitle(h_Wenu_Eop_plus[region]->GetTitle());
    str = path + "/Wenu_Eop_minus_" + region_strings[region];
    if (!CheckHistogram(f,str.c_str())) return;     
    h_Wenu_Eop_minus.push_back((TH1F*)(f->Get(str.c_str())));
    h_Wenu_Eop_minus[region]->SetTitle(h_Wenu_Eop_minus[region]->GetTitle());
    str = path + "/Wenu_Eopdiff_" + region_strings[region];
    if (!CheckHistogram(f,str.c_str())) return;     
    h_Wenu_Eopdiff.push_back((TH1F*)(f->Get(str.c_str())));
    h_Wenu_Eopdiff[region]->SetTitle(h_Wenu_Eopdiff[region]->GetTitle());

    h_Wenu_Eopdiff[region]->Add(h_Wenu_Eop_plus[region],h_Wenu_Eop_minus[region],1.,-1);
    
    if (region == incl) {
      
      str = path + "/Wenu_meanEop_vs_p_plus";
      if (!CheckHistogram(f,str.c_str())) return;     
      h_Wenu_meanEop_vs_p_plus.push_back((TProfile*)(f->Get(str.c_str())));
      h_Wenu_meanEop_vs_p_plus[region]->SetTitle(h_Wenu_meanEop_vs_p_plus[region]->GetTitle());
      str = path + "/Wenu_meanEop_vs_p_minus";
      if (!CheckHistogram(f,str.c_str())) return;     
      h_Wenu_meanEop_vs_p_minus.push_back((TProfile*)(f->Get(str.c_str())));
      h_Wenu_meanEop_vs_p_minus[region]->SetTitle(h_Wenu_meanEop_vs_p_minus[region]->GetTitle());
      str = path + "/Wenu_Eopdiff_vs_p";
      if (!CheckHistogram(f,str.c_str())) return;     
      h_Wenu_Eopdiff_vs_p.push_back((TProfile*)(f->Get(str.c_str())));
      h_Wenu_Eopdiff_vs_p[region]->SetTitle(h_Wenu_Eopdiff_vs_p[region]->GetTitle());
      
      str = path + "/Wenu_meanEop_vs_invp_plus";
      if (!CheckHistogram(f,str.c_str())) return;     
      h_Wenu_meanEop_vs_invp_plus.push_back((TProfile*)(f->Get(str.c_str())));
      h_Wenu_meanEop_vs_invp_plus[region]->SetTitle(h_Wenu_meanEop_vs_invp_plus[region]->GetTitle());
      str = path + "/Wenu_meanEop_vs_invp_minus";
      if (!CheckHistogram(f,str.c_str())) return;     
      h_Wenu_meanEop_vs_invp_minus.push_back((TProfile*)(f->Get(str.c_str())));
      h_Wenu_meanEop_vs_invp_minus[region]->SetTitle(h_Wenu_meanEop_vs_invp_minus[region]->GetTitle());
      str = path + "/Wenu_Eopdiff_vs_invp";
      if (!CheckHistogram(f,str.c_str())) return;     
      h_Wenu_Eopdiff_vs_invp.push_back((TProfile*)(f->Get(str.c_str())));
      h_Wenu_Eopdiff_vs_invp[region]->SetTitle(h_Wenu_Eopdiff_vs_invp[region]->GetTitle());
            
      str = path + "/Wenu_meanEop_vs_E_plus";
      if (!CheckHistogram(f,str.c_str())) return;     
      h_Wenu_meanEop_vs_E_plus.push_back((TProfile*)(f->Get(str.c_str())));
      h_Wenu_meanEop_vs_E_plus[region]->SetTitle(h_Wenu_meanEop_vs_E_plus[region]->GetTitle());
      str = path + "/Wenu_meanEop_vs_E_minus";
      if (!CheckHistogram(f,str.c_str())) return;     
      h_Wenu_meanEop_vs_E_minus.push_back((TProfile*)(f->Get(str.c_str())));
      h_Wenu_meanEop_vs_E_minus[region]->SetTitle(h_Wenu_meanEop_vs_E_minus[region]->GetTitle());
      str = path + "/Wenu_Eopdiff_vs_E";
      if (!CheckHistogram(f,str.c_str())) return;     
      h_Wenu_Eopdiff_vs_E.push_back((TProfile*)(f->Get(str.c_str())));
      h_Wenu_Eopdiff_vs_E[region]->SetTitle(h_Wenu_Eopdiff_vs_E[region]->GetTitle());
      
      str = path + "/Wenu_meanEop_vs_phi_plus";
      if (!CheckHistogram(f,str.c_str())) return;     
      h_Wenu_meanEop_vs_phi_plus.push_back((TProfile*)(f->Get(str.c_str())));
      h_Wenu_meanEop_vs_phi_plus[region]->SetTitle(h_Wenu_meanEop_vs_phi_plus[region]->GetTitle());
      str = path + "/Wenu_meanEop_vs_phi_minus";
      if (!CheckHistogram(f,str.c_str())) return;     
      h_Wenu_meanEop_vs_phi_minus.push_back((TProfile*)(f->Get(str.c_str())));
      h_Wenu_meanEop_vs_phi_minus[region]->SetTitle(h_Wenu_meanEop_vs_phi_minus[region]->GetTitle());
      str = path + "/Wenu_Eopdiff_vs_phi";
      if (!CheckHistogram(f,str.c_str())) return;     
      h_Wenu_Eopdiff_vs_phi.push_back((TProfile*)(f->Get(str.c_str())));
      h_Wenu_Eopdiff_vs_phi[region]->SetTitle(h_Wenu_Eopdiff_vs_phi[region]->GetTitle());
            
      str = path + "/Wenu_meanEop_vs_eta_plus";
      if (!CheckHistogram(f,str.c_str())) return;     
      h_Wenu_meanEop_vs_eta_plus.push_back((TProfile*)(f->Get(str.c_str())));
      h_Wenu_meanEop_vs_eta_plus[region]->SetTitle(h_Wenu_meanEop_vs_eta_plus[region]->GetTitle());
      str = path + "/Wenu_meanEop_vs_eta_minus";
      if (!CheckHistogram(f,str.c_str())) return;     
      h_Wenu_meanEop_vs_eta_minus.push_back((TProfile*)(f->Get(str.c_str())));
      h_Wenu_meanEop_vs_eta_minus[region]->SetTitle(h_Wenu_meanEop_vs_eta_minus[region]->GetTitle());
      str = path + "/Wenu_Eopdiff_vs_eta";
      if (!CheckHistogram(f,str.c_str())) return;     
      h_Wenu_Eopdiff_vs_eta.push_back((TProfile*)(f->Get(str.c_str())));
      h_Wenu_Eopdiff_vs_eta[region]->SetTitle(h_Wenu_Eopdiff_vs_eta[region]->GetTitle());
            
      h_Wenu_Eopdiff_vs_p[region]->Add(h_Wenu_meanEop_vs_p_plus[region],h_Wenu_meanEop_vs_p_minus[region],1.,-1);
      h_Wenu_Eopdiff_vs_invp[region]->Add(h_Wenu_meanEop_vs_invp_plus[region],h_Wenu_meanEop_vs_invp_minus[region],1.,-1);
      h_Wenu_Eopdiff_vs_E[region]->Add(h_Wenu_meanEop_vs_E_plus[region],h_Wenu_meanEop_vs_E_minus[region],1.,-1);
      h_Wenu_Eopdiff_vs_phi[region]->Add(h_Wenu_meanEop_vs_phi_plus[region],h_Wenu_meanEop_vs_phi_minus[region],1.,-1);
      h_Wenu_Eopdiff_vs_eta[region]->Add(h_Wenu_meanEop_vs_eta_plus[region],h_Wenu_meanEop_vs_eta_minus[region],1.,-1);
    
    }
    
  }

  for (int region=0; region!=Nregions; ++region) {
    h_Wenu_Eopdiff[region]->Write("",TObject::kOverwrite);
    if (region == incl) {
      h_Wenu_Eopdiff_vs_p[region]->Write("",TObject::kOverwrite);
      h_Wenu_Eopdiff_vs_invp[region]->Write("",TObject::kOverwrite);
      h_Wenu_Eopdiff_vs_E[region]->Write("",TObject::kOverwrite);
      h_Wenu_Eopdiff_vs_phi[region]->Write("",TObject::kOverwrite);
      h_Wenu_Eopdiff_vs_eta[region]->Write("",TObject::kOverwrite); 
    }
  }

  f->Write();

}




void
MonitoringFile::
fitMergedFile_IDPerfMonZMM ( TFile* f, std::string run_dir,std::string TriggerName ){

  std::string path;
  std::vector<TH1F*> hvec;
  gROOT->SetStyle("Plain");

  path = run_dir + "IDPerfMon/ZMM/" + TriggerName;
  //std::cout <<" ZMM path: "<< path <<  std::endl;
  if(  f->cd(path.c_str())==0 ) {
       //std::cerr << "MonitoringFile::fitMergedFile_IDPerfMonZMM(): "
  	         //<< "No such directory \"" << path << "\"\n";
       return;
  }  
  f->cd(path.c_str());



  // CB ---------
  TH1F* h_ZMass_CB_Shift_vs_Regions = new TH1F("Z_Mass_CB_Shift_vs_Regions", "Z_Mass_CB Shift vs. Regions",  4, 0.5, 4.5 );
  TH1F* h_ZMass_CB_Width_vs_Regions = new TH1F("Z_Mass_CB_Width_vs_Regions", "Z_Mass_CB Width vs. Regions",  4, 0.5, 4.5 );

   if ( !CheckHistogram( f,(path+"/Z_Mass_CB_All").c_str()) ||
        !CheckHistogram( f,(path+"/Z_Mass_CB_BB").c_str())  ||
        !CheckHistogram( f,(path+"/Z_Mass_CB_BE").c_str())  ||
        !CheckHistogram( f,(path+"/Z_Mass_CB_EE").c_str())   ) return;

   hvec.push_back( (TH1F*)(f->Get((path+"/Z_Mass_CB_All").c_str())->Clone()) );
   hvec.push_back( (TH1F*)(f->Get((path+"/Z_Mass_CB_BB").c_str())->Clone()) );
   hvec.push_back( (TH1F*)(f->Get((path+"/Z_Mass_CB_BE").c_str())->Clone()) );
   hvec.push_back( (TH1F*)(f->Get((path+"/Z_Mass_CB_EE").c_str())->Clone()) );
   fitZmumuHistograms(h_ZMass_CB_Shift_vs_Regions, h_ZMass_CB_Width_vs_Regions, hvec);
   hvec.clear();
   h_ZMass_CB_Shift_vs_Regions->SetStats(0);
   h_ZMass_CB_Width_vs_Regions->SetStats(0);
   h_ZMass_CB_Shift_vs_Regions->Write("",TObject::kOverwrite);
   h_ZMass_CB_Width_vs_Regions->Write("",TObject::kOverwrite);

   // ID ----------
   TH1F* h_ZMass_ID_Shift_vs_Regions = new TH1F("Z_Mass_ID_Shift_vs_Regions", "Z_Mass_ID Shift vs. Regions",  4, 0.5, 4.5 );
   TH1F* h_ZMass_ID_Width_vs_Regions = new TH1F("Z_Mass_ID_Width_vs_Regions", "Z_Mass_ID Width vs. Regions",  4, 0.5, 4.5 );

   if ( !CheckHistogram( f,(path+"/Z_Mass_ID_All").c_str()) ||
        !CheckHistogram( f,(path+"/Z_Mass_ID_BB").c_str())  ||
	!CheckHistogram( f,(path+"/Z_Mass_ID_BE").c_str())  ||
	!CheckHistogram( f,(path+"/Z_Mass_ID_EE").c_str())   ) return;

   hvec.push_back( (TH1F*)(f->Get((path+"/Z_Mass_ID_All").c_str())->Clone()) );
   hvec.push_back( (TH1F*)(f->Get((path+"/Z_Mass_ID_BB").c_str())->Clone()) );
   hvec.push_back( (TH1F*)(f->Get((path+"/Z_Mass_ID_BE").c_str())->Clone()) );
   hvec.push_back( (TH1F*)(f->Get((path+"/Z_Mass_ID_EE").c_str())->Clone()) );
   fitZmumuHistograms(h_ZMass_ID_Shift_vs_Regions, h_ZMass_ID_Width_vs_Regions, hvec);
   hvec.clear();
   h_ZMass_ID_Shift_vs_Regions->SetStats(0);
   h_ZMass_ID_Width_vs_Regions->SetStats(0);
   h_ZMass_ID_Shift_vs_Regions->Write("",TObject::kOverwrite);
   h_ZMass_ID_Width_vs_Regions->Write("",TObject::kOverwrite);

   // ME -----------
   TH1F* h_ZMass_ME_Shift_vs_Regions = new TH1F("Z_Mass_ME_Shift_vs_Regions", "Z_Mass_ME Shift vs. Regions",  4, 0.5, 4.5 );
   TH1F* h_ZMass_ME_Width_vs_Regions = new TH1F("Z_Mass_ME_Width_vs_Regions", "Z_Mass_ME Width vs. Regions",  4, 0.5, 4.5 );
    
   if ( !CheckHistogram( f,(path+"/Z_Mass_ME_All").c_str()) ||
        !CheckHistogram( f,(path+"/Z_Mass_ME_BB").c_str())  ||
	!CheckHistogram( f,(path+"/Z_Mass_ME_BE").c_str())  ||
	!CheckHistogram( f,(path+"/Z_Mass_ME_EE").c_str())   ) return;

   hvec.push_back( (TH1F*)(f->Get((path+"/Z_Mass_ME_All").c_str())->Clone()) );
   hvec.push_back( (TH1F*)(f->Get((path+"/Z_Mass_ME_BB").c_str())->Clone()) );
   hvec.push_back( (TH1F*)(f->Get((path+"/Z_Mass_ME_BE").c_str())->Clone()) );
   hvec.push_back( (TH1F*)(f->Get((path+"/Z_Mass_ME_EE").c_str())->Clone()) );
   fitZmumuHistograms(h_ZMass_ME_Shift_vs_Regions, h_ZMass_ME_Width_vs_Regions, hvec);
   hvec.clear();
   h_ZMass_ME_Shift_vs_Regions->SetStats(0);
   h_ZMass_ME_Width_vs_Regions->SetStats(0);
   h_ZMass_ME_Shift_vs_Regions->Write("",TObject::kOverwrite);
   h_ZMass_ME_Width_vs_Regions->Write("",TObject::kOverwrite);

   // MS -----------
   TH1F* h_ZMass_MS_Shift_vs_Regions = new TH1F("Z_Mass_MS_Shift_vs_Regions", "Z_Mass_MS Shift vs. Regions",  4, 0.5, 4.5 );
   TH1F* h_ZMass_MS_Width_vs_Regions = new TH1F("Z_Mass_MS_Width_vs_Regions", "Z_Mass_MS Width vs. Regions",  4, 0.5, 4.5 );

     
   if ( !CheckHistogram( f,(path+"/Z_Mass_MS_All").c_str()) ||
        !CheckHistogram( f,(path+"/Z_Mass_MS_BB").c_str())  ||
        !CheckHistogram( f,(path+"/Z_Mass_MS_BE").c_str())  ||
	!CheckHistogram( f,(path+"/Z_Mass_MS_EE").c_str())   ) return;

   hvec.push_back( (TH1F*)(f->Get((path+"/Z_Mass_MS_All").c_str())->Clone()) );
   hvec.push_back( (TH1F*)(f->Get((path+"/Z_Mass_MS_BB").c_str())->Clone()) );
   hvec.push_back( (TH1F*)(f->Get((path+"/Z_Mass_MS_BE").c_str())->Clone()) );
   hvec.push_back( (TH1F*)(f->Get((path+"/Z_Mass_MS_EE").c_str())->Clone()) );
   fitZmumuHistograms(h_ZMass_MS_Shift_vs_Regions, h_ZMass_MS_Width_vs_Regions, hvec);
   hvec.clear();
   h_ZMass_MS_Shift_vs_Regions->SetStats(0);
   h_ZMass_MS_Width_vs_Regions->SetStats(0);
   h_ZMass_MS_Shift_vs_Regions->Write("",TObject::kOverwrite);
   h_ZMass_MS_Width_vs_Regions->Write("",TObject::kOverwrite);



  // OLD naming style plots
  if (!CheckHistogram(f, (path+"/z-mass_trks").c_str())) return;
  TH1F* h_ZMass_ID   = (TH1F*)(f->Get((path+"/z-mass_trks").c_str())->Clone());
  h_ZMass_ID->SetTitle("Z Mass ID Tracks");   
  //fitZmumuMassPlot(h_ZMass_ID);
  //h_ZMass_ID->Write("",TObject::kOverwrite);

  if (!CheckHistogram(f, (path+"/z-mass_comb").c_str())) return;
  TH1F* h_ZMass_CB   = (TH1F*)(f->Get((path+"/z-mass_comb").c_str())->Clone());
  h_ZMass_CB->SetTitle("Z Mass CB Tracks");
  //fitZmumuMassPlot(h_ZMass_CB);
  //h_ZMass_CB->Write("",TObject::kOverwrite);


  if ( CheckHistogram(f,(path+"/Nevents").c_str()) ) {
       TH1F* h_Nevents	 = (TH1F*)f->Get((path+"/Nevents").c_str());
       TH1F* h_rate 	 = new TH1F("rate","production rate",1,0,1);
       double Ntot  	 = h_Nevents->GetEntries();
       double yield 	 = h_ZMass_ID->Integral(h_ZMass_ID->FindBin(80), h_ZMass_ID->FindBin(105));
       double rate 	 = yield/Ntot;
       double rate_error = sqrt(1/yield+1/Ntot)*yield/Ntot;
       //std::cout <<" rate: "<< rate <<" rate_error: "<< rate_error <<std::endl;
       h_rate->SetBinContent(1,rate);
       h_rate->SetBinError(1,rate_error);
       h_rate->Write("rate",TObject::kOverwrite);
  }

  f->Write();
}




void
MonitoringFile::
fitJpsiHistograms(TH1F* hmass, TH1F* hwidth, TH1F* h1[], int n)
{
  double jpsiPDGmass=3.097;
  double mass=0.;
  double mass_error=0;
  double width=0;
  double width_error=0;
  double maxmass=0.;
  double maxwidth=0.;
  double minwidth=999;
  hmass->SetMarkerStyle(21); 
  hwidth->SetMarkerStyle(21);
//   TF1 *fitjpsi = new TF1("fitjpsi","gaus",2.9,3.3);
//   fitjpsi->SetParNames("Constant","Mean","Width");
  TF1 *fitjpsi = new TF1("fitjpsi","gaus",2.6,3.6);
  //  fitjpsi->SetParNames("Constant","Mean","Sigma","Constant","Slope");
  fitjpsi->SetLineColor(2);
  for (int i=0; i<n; i++) {
    int h1entries = h1[i]->GetEntries(); 
    if (h1entries>50) {
      double mean = h1[i]->GetMean();
      double spread = 2*(h1[i]->GetRMS());
      double constantGuess = double(h1entries)/10.;
      fitjpsi->SetParameters(constantGuess,jpsiPDGmass,0.05);
      fitjpsi->SetRange(mean-spread,mean+spread);
      h1[i]->Fit("fitjpsi","RQM");
      mean = fitjpsi->GetParameter(1);
      spread = 1.2*(fitjpsi->GetParameter(2)); 
      fitjpsi->SetRange(mean-spread,mean+spread);
      h1[i]->Fit("fitjpsi","RQM");
      h1[i]->SetMarkerStyle(21);
      mass=fitjpsi->GetParameter(1)-jpsiPDGmass;
      mass_error=fitjpsi->GetParError(1);
      width=fitjpsi->GetParameter(2);
      width_error=fitjpsi->GetParError(2);
      hmass->SetBinContent(i+1,mass);
      hmass->SetBinError(i+1,mass_error); 
      hwidth->SetBinContent(i+1,width);
      hwidth->SetBinError(i+1,width_error); 
      if( (fabs(mass)+mass_error)>maxmass ) maxmass=fabs(mass)+mass_error;
      if( (fabs(width)+width_error)>maxwidth ) maxwidth=fabs(width)+width_error;
      if( (fabs(width)-width_error)<minwidth ) minwidth=fabs(width)-width_error;
    }
  }
  if (maxmass>0.1) maxmass=0.1;
  if (maxwidth>0.2) maxwidth=0.2;
  if (minwidth<0.) minwidth=0.;
  hmass->SetAxisRange(-maxmass*1.05,maxmass*1.05,"Y");
//   hwidth->SetAxisRange(0.,maxwidth*1.05,"Y");
  if (minwidth<maxwidth) hwidth->SetAxisRange(minwidth*0.9,maxwidth*1.05,"Y");

  delete fitjpsi;
}


void 
MonitoringFile::
fitUpsilonHistograms(TH1F* hmass, TH1F* hwidth, TH1F* h1[], int n)
{
  double upsilonPDGmass=9.46;
  double mass=0;
  double mass_error=0;
  double width=0;
  double width_error=0;
  double maxmass=0.;
  double maxwidth=0.;
  double minwidth=999;
  hmass->SetMarkerStyle(21); 
  hwidth->SetMarkerStyle(21);
//   TF1 *fitupsi = new TF1("fitupsi","gaus",9.,10.);
//   fitupsi->SetParNames("Constant","Mean","Width");
  TF1 *fitupsi = new TF1("fitupsi","gaus(0)+pol1(3)",8.5,10.5);
  fitupsi->SetParNames("Constant","Mean","Sigma","Constant","Slope");
  fitupsi->SetLineColor(2);
  for (int i=0; i<n; i++) {
    if (h1[i]->GetEntries()>20) {
      double max=h1[i]->GetBinContent(h1[i]->GetMaximumBin());  
      fitupsi->SetParameter(1,9.46);
      fitupsi->SetParameter(2,0.15); 
      fitupsi->SetParLimits(1,8.,11.);
      fitupsi->SetParLimits(2,0.,0.5);
      fitupsi->SetParameter(0,max);
      h1[i]->SetMarkerStyle(21);
      h1[i]->Fit("fitupsi","RQM");
      mass=fitupsi->GetParameter(1)-upsilonPDGmass;
      mass_error=fitupsi->GetParError(1);
      width=fitupsi->GetParameter(2);
      width_error=fitupsi->GetParError(2);
      hmass->SetBinContent(i+1,mass);
      hmass->SetBinError(i+1,mass_error);
      hwidth->SetBinContent(i+1,width);
      hwidth->SetBinError(i+1,width_error); 
      if( (fabs(mass)+mass_error)>maxmass ) maxmass=fabs(mass)+mass_error;
      if( (fabs(width)+width_error)>maxwidth ) maxwidth=fabs(width)+width_error;
      if( (fabs(width)-width_error)<minwidth ) minwidth=fabs(width)-width_error;
    }
  }
  if (maxmass>0.1) maxmass=0.1;
  if (maxwidth>0.4) maxwidth=0.4;
  if (minwidth<0.) minwidth=0.;
  hmass->SetAxisRange(-maxmass*1.05,maxmass*1.05,"Y");
  if (minwidth<maxwidth) hwidth->SetAxisRange(minwidth*0.9,maxwidth*1.05,"Y");

  delete fitupsi;
}


void MonitoringFile::fitZmumuHistograms(TH1F* hmass, TH1F* hwidth, std::vector<TH1F*> hvec){

  const double ZPDGmass = 91.1876;
  const int    size = (int)hvec.size();
  for(int i = 0; i < size; i++){
      double mass      = fitZmumuMass(hvec[i]).first.getVal()  - ZPDGmass;     
      double mass_err  = fitZmumuMass(hvec[i]).first.getError();      
      double width     = fitZmumuMass(hvec[i]).second.getVal();
      double width_err = fitZmumuMass(hvec[i]).second.getError();
      
      hmass->SetBinContent(i+1, mass);
      hmass->SetBinError(i+1,mass_err); 
      hmass->GetXaxis()->SetBinLabel(i+1, hvec[i]->GetName());
      hmass->GetXaxis()->SetTitle("Different Detector Regions");
      hmass->SetAxisRange(-3.0, 3.0,"Y");

      hwidth->SetBinContent(i+1, width);
      hwidth->SetBinError(i+1, width_err);
      hwidth->GetXaxis()->SetBinLabel(i+1, hvec[i]->GetName());
      hwidth->GetXaxis()->SetTitle("Different Detector Regions");
      hwidth->SetAxisRange(0, 8, "Y");
  }
}



std::pair<RooRealVar, RooRealVar> MonitoringFile::fitZmumuMass( TH1F* hmass ){
    
  //  gROOT->ProcessLine("gErrorIgnoreLevel=1001;");
  RooAbsPdf::verboseEval(-100); //sami in order to make roofit quiet
  RooRealVar  m("mass", "dimuon invariable mass", 91.2, 60., 120., "GeV");
  RooDataHist *data = 0;
  data = new RooDataHist("data", "data", m, hmass);
  RooRealVar bwm0("bw_#mu",    "bw_#mu",   91.2, 85., 95.) ;
  RooRealVar bwsg("bw_#sigma", "bw_#sigma", 2.4952) ;
  RooBreitWigner bw("bw","bw",  m, bwm0, bwsg);
    
  RooRealVar cbm0("cb_#mu",    "cb_#mu",  0  ) ;
  RooRealVar cbsg("cb_#sigma", "cb_#sigma", 5.,  0.1,  10.) ;
  RooRealVar cbal("cb_#alpha", "cb_#alpha", 1.5, 0.1,  10.) ;
  RooRealVar cbn( "cb_n",      "cb_n",      1.5, 0.01, 10.) ;
  RooCBShape cb(  "cb",        "cb", m, cbm0, cbsg, cbal, cbn);
    
  m.setBins(5000);
  RooFFTConvPdf bxc("bxc", "BW (X) CB", m, bw, cb) ;
  bxc.fitTo(*data,RooFit::PrintLevel(-1), RooFit::FitOptions("QN"), RooFit::PrintEvalErrors(-1),RooFit::Warnings(kFALSE));
  
  RooArgSet* params = bxc.getVariables() ;
  //params->Print("v");
  RooRealVar* fit_bwm0 = (RooRealVar*) params->find("bw_#mu");
  RooRealVar* fit_cbsg = (RooRealVar*) params->find("cb_#sigma");
  //double     vfit_bwm0 = fit_bwm0->getVal();
  //double     vfit_cbsg = fit_cbsg->getVal();
  //std::cout <<" vfit_bwm0: "<<  vfit_bwm0 << " vfit_cbsg: "  << vfit_cbsg << std::endl;
  delete data;
  return std::make_pair(*fit_bwm0 , *fit_cbsg);
}



//void MonitoringFile::fitZmumuMass(TH1F* hmass , RooPlot* frame ){
void MonitoringFile::fitZmumuMassPlot(TH1F* hmass){

  //  gROOT->ProcessLine("gErrorIgnoreLevel=1001;");
  RooAbsPdf::verboseEval(-100); //sami in order to make roofit quiet
  RooRealVar  m("mass", "dimuon invariable mass", 91.2, 60., 120., "GeV");
  RooDataHist *data = 0;
  data = new RooDataHist("data", "data", m, hmass);
  RooRealVar bwm0("bw_#mu",   "bw_#mu",   91.2, 85., 95.) ;
  RooRealVar bwsg("bw_#sigma","bw_#sigma", 2.4952) ;
  RooBreitWigner bw("bw","bw", m, bwm0, bwsg);

  RooRealVar cbm0("cb_#mu",    "cb_#mu",  0  ) ;
  RooRealVar cbsg("cb_#sigma", "cb_#sigma", 5.,  0.1,  10.) ;
  RooRealVar cbal("cb_#alpha", "cb_#alpha", 1.5, 0.1,  10.) ;
  RooRealVar cbn( "cb_n",      "cb_n",      1.5, 0.01, 10.) ;
  RooCBShape cb(  "cb",        "cb", m, cbm0, cbsg, cbal, cbn);

  m.setBins(5000);
  RooFFTConvPdf bxc("bxc", "BW (X) CB", m, bw, cb) ;
  bxc.fitTo(*data, RooFit::PrintLevel(-1), RooFit::FitOptions("QN"), RooFit::PrintEvalErrors(-1),RooFit::Warnings(kFALSE));
  RooPlot* frame = m.frame();
  data->plotOn(frame, RooFit::MarkerSize(0.9));
  bxc.paramOn(frame,  RooFit::Format("NELU", RooFit::AutoPrecision(2)), RooFit::Layout(0.1,0.4,0.9));
  bxc.plotOn (frame,  RooFit::LineColor(kYellow));
  cb.plotOn(frame,    RooFit::LineColor(kRed));
  bw.plotOn(frame,    RooFit::LineStyle(kDashed)) ;
  delete data;
}


}
