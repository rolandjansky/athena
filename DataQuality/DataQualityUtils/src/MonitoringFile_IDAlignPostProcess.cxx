/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// **********************************************************************
// $Id: MonitoringFile_IDAlignPostProcess.cxx,v 1.1 2009-04-03 08:48:00 ponyisi Exp $
// **********************************************************************

#include "DataQualityUtils/MonitoringFile.h"

#include <cmath>
#include <vector>

#include <TCanvas.h>
#include <TF1.h>
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TKey.h>
#include <TMath.h>
#include <TProfile.h>

#include <iomanip>
#include <locale>
#include <sstream>
#include <string>


namespace dqutils {


void 
MonitoringFile::
fitMergedFile_IDAlignMonManager( std::string inFilename, bool /* isIncremental */ )
{
  /* std::cout << "\n";
  std::cout << "Running Inner-Detector alignment-monitoring analysis\n";
  std::cout << "\nWarning messages from fitting and histogram updating follow:\n\n"; */
  
  TFile* f = TFile::Open(inFilename.c_str(),"UPDATE");
  if (f == 0 || ! f->IsOpen()) {
      //std::cerr << "MonitoringFile::fitMergedFile_IDAlignMonManager(): "
      //<< "Input file not opened\n";
    delete f;
    return;
  }
  if(f->GetSize()<1000.) {
      //std::cerr << "MonitoringFile::fitMergedFile_IDAlignMonManager(): "
//	      << "MonitoringFile empty\n";
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
          if (perf_name.find("IDAlignMon")!=std::string::npos){
	    TObject* obj_perf = key_perf->ReadObj();
	    TDirectory* tdir_perf = dynamic_cast<TDirectory*>( obj_perf );
	    if (tdir_perf !=0) { 
	      run_dir=run_dir+'/';
	      TIter next_tracks(tdir_perf->GetListOfKeys());
	      TKey* key_tracks(0);
	      
	      while ((key_tracks=dynamic_cast<TKey*>(next_tracks() )) !=0) {
		std::string tracks_name(key_tracks->GetName());
		//std::cout<<"looking at tracks dir: "<<tracks_name<<std::endl;
		if (tracks_name.find("Track")!=std::string::npos) {
		    //std::cout<<"Found tracks name: "<<tracks_name<<std::endl;
		  TObject *obj = key_tracks->ReadObj();
		  TDirectory *tdir = dynamic_cast<TDirectory*>(obj);
		  if ( tdir !=0 ) {
		    TIter next_module(tdir->GetListOfKeys());
		    TKey* key_module(0);
		    while((key_module=dynamic_cast<TKey*>(next_module()))!=0) {
		      std::string module_name=key_module->GetName();
		      //std::cout<<"looking at module: "<<module_name<<std::endl;
		      if (module_name.find("Residuals")!=std::string::npos) {
			  //std::cout<<"Find Module: "<<module_name<<std::endl;
			fitMergedFile_IDAlignMonResiduals( f,run_dir,tracks_name );
		      }

		      if (module_name.find("TrackSegments")!=std::string::npos) {
			  //std::cout<<"Find Module: "<<module_name<<std::endl;
			fitMergedFile_IDAlignMonTrackSegments( f,run_dir,tracks_name );
		      }
                      if (module_name.find("GenericTracks")!=std::string::npos) {
			fitMergedFile_IDAlignMonPVbiases( f,run_dir,tracks_name );
		      }
		      //This is not needed. The plots are already in the monitoring file
		      //if (module_name.find("GenericTracks")!=std::string::npos) {
			  //std::cout<<"Find Module: "<<module_name<<std::endl;
		      //fitMergedFile_IDAlignMonGenericTracks( f,run_dir,tracks_name );
		      //}		      
		    }
		  }
		  else  // This was not a TDirectory, delete it!
		  {
		      delete obj;
		  }
		}
	      }
	    }
	    else // This was not a TDirectory, delete it!
	    {
		delete obj_perf;
	    }
	  }
	}
      }
      
      // if without top run_directory
      else if (tdir_run_name.find("IDAlignMon")!= std::string::npos ) {
	//std::cout<<"without run_directory"<<std::endl;
	TObject* obj_perf = key_run->ReadObj();
	TDirectory* tdir_perf = dynamic_cast<TDirectory*>( obj_perf );
	if (tdir_perf !=0) { 
	  run_dir='/';
	  TIter next_tracks(tdir_perf->GetListOfKeys());
	  TKey* key_tracks(0);

	  while ((key_tracks=dynamic_cast<TKey*>(next_tracks() )) !=0) {
	    std::string tracks_name(key_tracks->GetName());
	    //std::cout<<"looking at tracks dir: "<<tracks_name<<std::endl;
	    if (tracks_name.find("Tracks")!=std::string::npos) {
		//std::cout<<"Found tracks name: "<<tracks_name<<std::endl;
	      TObject *obj = key_tracks->ReadObj();
	      TDirectory *tdir = dynamic_cast<TDirectory*>(obj);
	      if ( tdir !=0 ) {
		TIter next_module(tdir->GetListOfKeys());
		TKey* key_module(0);
		while((key_module=dynamic_cast<TKey*>(next_module()))!=0) {
		  std::string module_name=key_module->GetName();
		  //std::cout<<"looking at module: "<<module_name<<std::endl;
		  if (module_name.find("Residuals")!=std::string::npos) {
		      //std::cout<<"Find Module: "<<module_name<<std::endl;
		    fitMergedFile_IDAlignMonResiduals( f,run_dir,tracks_name );
		  }
		  
		  if (module_name.find("TrackSegments")!=std::string::npos) {
		      //std::cout<<"Find Module: "<<module_name<<std::endl;
		    fitMergedFile_IDAlignMonTrackSegments( f,run_dir,tracks_name );
		  }
		  //This is not needed. The plots are already in the monitoring file
		  //if (module_name.find("GenericTracks")!=std::string::npos) {
		      //std::cout<<"Find Module: "<<module_name<<std::endl;
		  //  fitMergedFile_IDAlignMonGenericTracks( f,run_dir,tracks_name );
		  // }	
		}
	      }
	      else
	      {
		  delete obj;
	      }
	    }
	  }
	}
	else
	{
	    delete obj_perf;
	}
      }
      //else
      //std::cerr<<"directory IDAlignMon doesn't exist !"<<std::endl;
    }
    else
    {
	delete obj_run;
    }
  }
  
  f->Close();
  delete f;
  //std::cout << "\n";
  //std::cout << "Finish Inner-Detector alignment-monitoring analysis"<<std::endl;
}


void
MonitoringFile::
fitMergedFile_IDAlignMonTrackSegments( TFile* file, std::string run_dir, std::string tracksName )
{

  //std::cout << "in fitMergedFile_IDAlignMonTrackSegments " << std::endl;

  std::string path;
  path= run_dir + "IDAlignMon/" + tracksName + "/TrackSegments";
  if(  file->cd(path.c_str())==0 ) {
      //std::cerr << "MonitoringFile::fitMergedFile_IDAlignMonTrackSegments(): "
      //            << "No such directory \"" << path << "\"\n";
    return;
  }

  //The Charge Mis-Indentification Histograms
  //===========================================

  unsigned int nChargeHistograms = 4;
  
  //The input/source files 
  TH2F* m_oldChargeHists[4];
  
  //The output/destination files 
  TH1F* m_newChargeHists[4];
  m_newChargeHists[0]=NULL;
  m_newChargeHists[1]=NULL;
  m_newChargeHists[2]=NULL;
  m_newChargeHists[3]=NULL;
  
  //The names of the input files
  std::string chargeHistNames[4];
  chargeHistNames[0] = "delta_chargeVsPtLow";
  chargeHistNames[1] = "delta_chargeVsPtUp";
  chargeHistNames[2] = "delta_chargeVsD0";
  chargeHistNames[3] = "delta_chargeVsZ0";

  std::string chargeHistTitle[4];
  chargeHistTitle[0] = "Mis-Id ratio vs pT (Low)";
  chargeHistTitle[1] = "Mis-Id ratio vs pT (Up)";
  chargeHistTitle[2] = "Mis-Id ratio vs d_{0}";
  chargeHistTitle[3] = "Mis-Id ratio vs Z0";
  
  std::string chargeHistXAxisTitles[4];
  chargeHistXAxisTitles[0] = "p_{T} Low (GeV)";
  chargeHistXAxisTitles[1] = "p_{T} Up (GeV)";
  chargeHistXAxisTitles[2] = "d_{0} (mm)";
  chargeHistXAxisTitles[3] = "z_{0} (mm)";

  //loop over the number of histograms to be created
  for(unsigned int j = 0; j< nChargeHistograms; ++j){
    
    //We write the hist only if the input exist.
    if (!CheckHistogram(file,(path+"/"+chargeHistNames[j]).c_str())) continue;
    
    //get them
    m_oldChargeHists[j] = (TH2F*)(file->Get((path+"/"+chargeHistNames[j]).c_str()));
    
    //get binning
    unsigned int nbinsX = m_oldChargeHists[j]->GetNbinsX();
    float LE = m_oldChargeHists[j]->GetBinLowEdge(1);
    float UE = m_oldChargeHists[j]->GetBinLowEdge(nbinsX+1);
    
    //create the new files
    m_newChargeHists[j] = new TH1F(("ratio_"+chargeHistNames[j]).c_str(),"",nbinsX,LE,UE);
    
    //Fill the new files
    for(unsigned int xBin = 1; xBin!=nbinsX+1; xBin++){
      float ChargeMinus2 = m_oldChargeHists[j]->GetBinContent(xBin,2);
      float ChargePlus2 = m_oldChargeHists[j]->GetBinContent(xBin,6);
      float ChargeCorrect = m_oldChargeHists[j]->GetBinContent(xBin,4);
      float ChargeWrong = ChargeMinus2+ChargePlus2;
      if(ChargeCorrect > 0){
	m_newChargeHists[j]->SetBinContent(xBin,(ChargeWrong)/(ChargeCorrect+ChargeWrong));
	m_newChargeHists[j]->SetBinError(xBin,1.0/ChargeCorrect * pow(ChargeWrong * (1.0 + ChargeWrong/ChargeCorrect) ,0.5) );
      }else{
	if(ChargeWrong > 0)
	  m_newChargeHists[j]->SetBinContent(xBin,1);
	else
	  m_newChargeHists[j]->SetBinContent(xBin,0);
	m_newChargeHists[j]->SetBinError(xBin,0);
      }
    }//Fill the new histograms
    
    m_newChargeHists[j]->SetTitle((chargeHistTitle[j]).c_str());
    m_newChargeHists[j]->GetXaxis()->SetTitle((chargeHistXAxisTitles[j]).c_str());
    
  }//over charged hists
  
    
  //Write out the charge mis-id histograms
  file->Write();
  for(unsigned int j = 0; j< nChargeHistograms; ++j){
    
    //If the original hist dosn't exist, we didnt make a new one, 
    // so we dont write the new one
    if (!CheckHistogram(file,(path+"/"+chargeHistNames[j]).c_str())) continue;
    
    if (m_newChargeHists[j])
      m_newChargeHists[j]->Write("",TObject::kOverwrite);
  }

  
  //Delta Track Parameter Vs Track Parameter Profiles
  //==================================================

  //The input histograms
  TH2F* m_oldHists[30*3];
  
  //The output histograms
  TH1F* m_newHists[30*3*2];
  
  //The names of the input files
  std::string histNames[30];
  histNames[0] = "delta_d0VsD0";
  histNames[1] = "delta_d0VsZ0";
  histNames[2] = "delta_d0VsPhi0";
  histNames[3] = "delta_d0VsPt";
  histNames[4] = "delta_d0VsEta";
  
  histNames[5] = "delta_eta0VsD0";
  histNames[6] = "delta_eta0VsZ0";
  histNames[7] = "delta_eta0VsPhi0";
  histNames[8] = "delta_eta0VsPt";
  histNames[9] = "delta_eta0VsEta";
  
  histNames[10] = "delta_phi0VsD0";
  histNames[11] = "delta_phi0VsZ0";
  histNames[12] = "delta_phi0VsPhi0";
  histNames[13] = "delta_phi0VsPt";
  histNames[14] = "delta_phi0VsEta";

  histNames[15] = "delta_qOverPtVsD0";
  histNames[16] = "delta_qOverPtVsZ0";
  histNames[17] = "delta_qOverPtVsPhi0";
  histNames[18] = "delta_qOverPtVsPt";
  histNames[19] = "delta_qOverPtVsEta";
    
  histNames[20] = "delta_z0VsD0";
  histNames[21] = "delta_z0VsZ0";
  histNames[22] = "delta_z0VsPhi0";
  histNames[23] = "delta_z0VsPt";
  histNames[24] = "delta_z0VsEta";

  histNames[25] = "delta_nHitsVsD0";
  histNames[26] = "delta_nHitsVsZ0";
  histNames[27] = "delta_nHitsVsPhi0";
  histNames[28] = "delta_nHitsVsPt";
  histNames[29] = "delta_nHitsVsEta";

  std::string histTitles[30];
  std::string m_titlePrefix[2] = {"average_","sigma_"};
  std::string m_titlePostfix[3] = {"","(Positive Tracks)","(Negative Tracks)"};
  
  histTitles[0] = "#Delta d_{0} vs d_{0}^{Upper} ";
  histTitles[1] = "#Delta d_{0} vs z_{0}^{Upper} ";
  histTitles[2] = "#Delta d_{0} vs #phi_{0}^{Upper} ";
  histTitles[3] = "#Delta d_{0} vs p_{T}^{Upper} ";
  histTitles[4] = "#Delta d_{0} vs #eta^{Upper} ";

  histTitles[5] = "#Delta #eta vs d_{0}^{Upper} ";
  histTitles[6] = "#Delta #eta vs z_{0}^{Upper} ";
  histTitles[7] = "#Delta #eta vs #phi_{0}^{Upper} ";
  histTitles[8] = "#Delta #eta vs p_{T}^{Upper} ";
  histTitles[9] = "#Delta #eta vs #eta^{Upper} ";
  
  histTitles[10] = "#Delta phi vs d_{0}^{Upper} ";
  histTitles[11] = "#Delta phi vs z_{0}^{Upper} ";
  histTitles[12] = "#Delta phi vs phi0^{Upper} ";
  histTitles[13] = "#Delta phi vs p_{T}^{Upper} ";
  histTitles[14] = "#Delta phi vs #eta^{Upper} ";

  histTitles[15] = "#Delta q/p_{T} vs d_{0}^{Upper} ";
  histTitles[16] = "#Delta q/p_{T} vs z_{0}^{Upper} ";
  histTitles[17] = "#Delta q/p_{T} vs #phi_{0}^{Upper} ";
  histTitles[18] = "#Delta q/p_{T} vs p_{T}^{Upper} ";
  histTitles[19] = "#Delta q/p_{T} vs #eta^{Upper} ";

  histTitles[20] = "#Delta z_{0} vs d_{0}^{Upper} ";
  histTitles[21] = "#Delta z_{0} vs z_{0}^{Upper} ";
  histTitles[22] = "#Delta z_{0} vs #phi_{0}^{Upper} ";
  histTitles[23] = "#Delta z_{0} vs p_{T}^{Upper} ";
  histTitles[24] = "#Delta z_{0} vs #eta^{Upper} ";
  
  histTitles[25] = "#Delta nHits vs d_{0}^{Upper} ";
  histTitles[26] = "#Delta nHits vs z_{0}^{Upper} ";
  histTitles[27] = "#Delta nHits vs #phi_{0}^{Upper} ";
  histTitles[28] = "#Delta nHits vs p_{T}^{Upper} ";
  histTitles[29] = "#Delta nHits vs #eta^{Upper} ";
  
  std::string histXAxisTitles[5];
  histXAxisTitles[0] = "d_{0}^{Upper} (mm)";
  histXAxisTitles[1] = "z_{0}^{Upper} (mm)";
  histXAxisTitles[2] = "#phi_{0}^{Upper} (rad)";
  histXAxisTitles[3] = "p_{T}^{Upper} (GeV)";
  histXAxisTitles[4] = "#eta^{Upper}";
  
  std::string histYAxisTitles[6];
  histYAxisTitles[0] = "#Delta d_{0} (mm)";
  histYAxisTitles[1] = "#Delta #eta";
  histYAxisTitles[2] = "#Delta #phi (rad)";
  histYAxisTitles[3] = "#Delta q/p_{T} (1.0/GeV)";
  histYAxisTitles[4] = "#Delta z_{0} (mm)";
  histYAxisTitles[5] = "#Delta nHits";

  //loop over combined/positive/negative
  for(unsigned int i = 0; i < 3; ++i){
    //loop over the number of profiles to be created
    for(unsigned int j = 0; j< 30; ++j){
      
      std::string m_histNames;
      
      //Charged or Combo?
      if(i == 0)
	m_histNames += histNames[j];
      
      if(i == 1)
	m_histNames += histNames[j]+"_p";
      
      if(i == 2)
	m_histNames += histNames[j]+"_n";
      
      //We write the hist only if the input exist.
      if (!CheckHistogram(file,(path+"/"+m_histNames).c_str())){
	m_oldHists[30*i+j] = 0;
	continue;
      }	
      
      //get them
      m_oldHists[30*i+j] = (TH2F*)(file->Get((path+"/"+m_histNames).c_str()));
      
      //get binning
      unsigned int nbinsX = m_oldHists[30*i+j]->GetNbinsX();
      unsigned int nbinsY = m_oldHists[30*i+j]->GetNbinsY();
      float xLE = m_oldHists[30*i+j]->GetXaxis()->GetBinLowEdge(1);
      float xUE = m_oldHists[30*i+j]->GetXaxis()->GetBinLowEdge(nbinsX+1);
      float yLE = m_oldHists[30*i+j]->GetYaxis()->GetBinLowEdge(1);
      float yUE = m_oldHists[30*i+j]->GetYaxis()->GetBinLowEdge(nbinsY+1);
      
      //std::cout << "i = " << i << std::endl;
      //std::cout << "j = " << j << std::endl;
      //loop over Average/Sigma
      for(unsigned int k = 0; k < 2; ++k){
	//std::cout << "k = " << k << std::endl;
	
	//create the new files
	m_newHists[2*(30*i+j)+k] = new TH1F((m_titlePrefix[k]+m_histNames).c_str(),"",nbinsX,xLE,xUE);
	
	//Fill the new histograms
	fillGaussianMeanOrWidth(m_oldHists[30*i+j], m_newHists[2*(30*i+j)+k],yLE,yUE,k);
		
	m_newHists[2*(30*i+j)+k]->SetTitle((m_titlePrefix[k]+m_histNames+m_titlePostfix[i]).c_str());
	m_newHists[2*(30*i+j)+k]->GetXaxis()->SetTitle((histXAxisTitles[j%5]).c_str());
	if(j<5)
	  m_newHists[2*(30*i+j)+k]->GetYaxis()->SetTitle((histYAxisTitles[0]).c_str());
	else if(j<10)
	  m_newHists[2*(30*i+j)+k]->GetYaxis()->SetTitle((histYAxisTitles[1]).c_str());
	else if(j<15)
	  m_newHists[2*(30*i+j)+k]->GetYaxis()->SetTitle((histYAxisTitles[2]).c_str());
	else if(j<20)
	  m_newHists[2*(30*i+j)+k]->GetYaxis()->SetTitle((histYAxisTitles[3]).c_str());
	else if(j<25)
	  m_newHists[2*(30*i+j)+k]->GetYaxis()->SetTitle((histYAxisTitles[4]).c_str());
	else if(j<30)
	  m_newHists[2*(30*i+j)+k]->GetYaxis()->SetTitle((histYAxisTitles[5]).c_str());
      }//over the mean or the sigma
      
    }//over the histgrams (j)
  }//over the charge (i)
  
      
  //loop over combined/positive/negative
  for(unsigned int i = 0; i < 3; ++i){
    //loop over the number of profiles to be created
    for(unsigned int j = 0; j< 30; ++j){
      //loop over average/mean
      for(unsigned int k = 0; k < 2; ++k){
	std::string m_histNames;
	if(i == 0)
	  m_histNames = histNames[j];
	
	if(i == 1)
	  m_histNames = histNames[j]+"_p";
	
	if(i == 2)
	  m_histNames = histNames[j]+"_n";
	
	//If the original hist dosn't exist, we didnt make a new one, 
	// so we dont write the new one
	// if (!CheckHistogram(file,(path+"/"+m_histNames).c_str())){
	//   continue;
	// }
	if (m_oldHists[30*i+j] == 0) {
	    //std::cout << m_histNames << " not found" << std::endl;
	  continue;
	}
	
	m_newHists[2*(30*i+j)+k]->Write("",TObject::kOverwrite);
      }
    }
  }

  //Write out the delta trackP vs trackP profiles
  //20100217 - moved here to avoid file corruption - PUEO
  file->Write();
  return;
}

///

void
MonitoringFile::
fitMergedFile_IDAlignMonResiduals( TFile* f, std::string run_dir, std::string tracksName )
{

  const float minSiResMeanWindow = -0.1; 
  const float maxSiResMeanWindow = 0.1; 
  const float minSiResWidthWindow = 0.0; 
  const float maxSiResWidthWindow = 0.3; 
  const float minSiPullWidthWindow = 0.0; 
  const float maxSiPullWidthWindow = 2.0; 

  bool doOverlapResiduals = false;

  std::string path;
  path= run_dir + "IDAlignMon/" + tracksName + "/Residuals";
  if(  f->cd(path.c_str())==0 ) {
      //std::cerr << "MonitoringFile::fitMergedFile_IDAlignMonResiduals(): "
      //            << "No such directory \"" << path << "\"\n";
    return;
  }

  TObjArray* plots = new TObjArray[10];//used for performing operations on histos

  //2d histograms from which the fits/means will be taken - checking they exist
  if (!CheckHistogram(f,(path+"/si_barrel_pullX").c_str())) return;
  if (!CheckHistogram(f,(path+"/si_eca_pullX").c_str())) return;
  if (!CheckHistogram(f,(path+"/si_ecc_pullX").c_str())) return;
  if (!CheckHistogram(f,(path+"/si_barrel_pullY").c_str())) return;
  if (!CheckHistogram(f,(path+"/si_eca_pullY").c_str())) return;
  if (!CheckHistogram(f,(path+"/si_ecc_pullY").c_str())) return;

  if (!CheckHistogram(f,(path+"/si_barrel_resX").c_str())) return;
  if (!CheckHistogram(f,(path+"/si_eca_resX").c_str())) return;
  if (!CheckHistogram(f,(path+"/si_ecc_resX").c_str())) return;
  if (!CheckHistogram(f,(path+"/si_barrel_resY").c_str())) return;
  if (!CheckHistogram(f,(path+"/si_eca_resY").c_str())) return;
  if (!CheckHistogram(f,(path+"/si_ecc_resY").c_str())) return;

  if (!CheckHistogram(f,(path+"/pix_eca_xresvsmodphi_2d").c_str())) return;
  if (!CheckHistogram(f,(path+"/pix_eca_yresvsmodphi_2d").c_str())) return;
  if (!CheckHistogram(f,(path+"/pix_ecc_xresvsmodphi_2d").c_str())) return;
  if (!CheckHistogram(f,(path+"/pix_ecc_yresvsmodphi_2d").c_str())) return;
  if (!CheckHistogram(f,(path+"/sct_eca_xresvsmodphi_2d").c_str())) return;
  if (!CheckHistogram(f,(path+"/sct_ecc_xresvsmodphi_2d").c_str())) return;
    
  if (!CheckHistogram(f,(path+"/pix_b0_xresvsmodetaphi_3d").c_str())) return;
  if (!CheckHistogram(f,(path+"/pix_b1_xresvsmodetaphi_3d").c_str())) return;
  if (!CheckHistogram(f,(path+"/pix_b2_xresvsmodetaphi_3d").c_str())) return;
  if (!CheckHistogram(f,(path+"/pix_b3_xresvsmodetaphi_3d").c_str())) return;
  if (!CheckHistogram(f,(path+"/pix_b0_yresvsmodetaphi_3d").c_str())) return;
  if (!CheckHistogram(f,(path+"/pix_b1_yresvsmodetaphi_3d").c_str())) return;
  if (!CheckHistogram(f,(path+"/pix_b2_yresvsmodetaphi_3d").c_str())) return;
  if (!CheckHistogram(f,(path+"/pix_b3_yresvsmodetaphi_3d").c_str())) return;
  if (!CheckHistogram(f,(path+"/sct_b0_xresvsmodetaphi_3d").c_str())) return;
  if (!CheckHistogram(f,(path+"/sct_b1_xresvsmodetaphi_3d").c_str())) return;
  if (!CheckHistogram(f,(path+"/sct_b2_xresvsmodetaphi_3d").c_str())) return;
  if (!CheckHistogram(f,(path+"/sct_b3_xresvsmodetaphi_3d").c_str())) return;

  if (CheckHistogram(f,(path+"/pix_b0_Oxresxvsmodetaphi_3d").c_str())) {
   
    doOverlapResiduals = true;
    if (!CheckHistogram(f,(path+"/pix_b0_Oxresxvsmodetaphi_3d").c_str())) return;
    if (!CheckHistogram(f,(path+"/pix_b1_Oxresxvsmodetaphi_3d").c_str())) return;
    if (!CheckHistogram(f,(path+"/pix_b2_Oxresxvsmodetaphi_3d").c_str())) return;
    if (!CheckHistogram(f,(path+"/pix_b3_Oxresxvsmodetaphi_3d").c_str())) return;
    if (!CheckHistogram(f,(path+"/pix_b0_Oxresyvsmodetaphi_3d").c_str())) return;
    if (!CheckHistogram(f,(path+"/pix_b1_Oxresyvsmodetaphi_3d").c_str())) return;
    if (!CheckHistogram(f,(path+"/pix_b2_Oxresyvsmodetaphi_3d").c_str())) return;
    if (!CheckHistogram(f,(path+"/pix_b3_Oxresyvsmodetaphi_3d").c_str())) return;
    if (!CheckHistogram(f,(path+"/pix_b0_Oyresyvsmodetaphi_3d").c_str())) return;
    if (!CheckHistogram(f,(path+"/pix_b1_Oyresyvsmodetaphi_3d").c_str())) return;
    if (!CheckHistogram(f,(path+"/pix_b2_Oyresyvsmodetaphi_3d").c_str())) return;
    if (!CheckHistogram(f,(path+"/pix_b3_Oyresyvsmodetaphi_3d").c_str())) return;
    if (!CheckHistogram(f,(path+"/pix_b0_Oyresxvsmodetaphi_3d").c_str())) return;
    if (!CheckHistogram(f,(path+"/pix_b1_Oyresxvsmodetaphi_3d").c_str())) return;
    if (!CheckHistogram(f,(path+"/pix_b2_Oyresxvsmodetaphi_3d").c_str())) return;
    if (!CheckHistogram(f,(path+"/pix_b3_Oyresyvsmodetaphi_3d").c_str())) return;
   

    if (!CheckHistogram(f,(path+"/sct_b0_Oxresxvsmodetaphi_3d").c_str())) return;
    if (!CheckHistogram(f,(path+"/sct_b1_Oxresxvsmodetaphi_3d").c_str())) return;
    if (!CheckHistogram(f,(path+"/sct_b2_Oxresxvsmodetaphi_3d").c_str())) return;
    if (!CheckHistogram(f,(path+"/sct_b3_Oxresxvsmodetaphi_3d").c_str())) return;
    if (!CheckHistogram(f,(path+"/sct_b0_Oyresxvsmodetaphi_3d").c_str())) return;
    if (!CheckHistogram(f,(path+"/sct_b1_Oyresxvsmodetaphi_3d").c_str())) return;
    if (!CheckHistogram(f,(path+"/sct_b2_Oyresxvsmodetaphi_3d").c_str())) return;
    if (!CheckHistogram(f,(path+"/sct_b3_Oyresxvsmodetaphi_3d").c_str())) return;

    //Check if the bowing histograms exist.

    if (!CheckHistogram(f,(path+"/pix_b0_resXvsetaLumiBlock").c_str())) return;
    if (!CheckHistogram(f,(path+"/pix_b0_resXvsetaLumiBlock_planars").c_str())) return;
    
  }
  //else std::cout << "no overlap residual histograms found - will not process overlap residuals" << std::endl;

  //get the 2d histograms from the file
  TH2F* m_si_barrel_pullX = (TH2F*)(f->Get((path+"/si_barrel_pullX").c_str()));
  TH2F* m_si_eca_pullX = (TH2F*)(f->Get((path+"/si_eca_pullX").c_str()));
  TH2F* m_si_ecc_pullX = (TH2F*)(f->Get((path+"/si_ecc_pullX").c_str()));
  TH2F* m_si_barrel_pullY = (TH2F*)(f->Get((path+"/si_barrel_pullY").c_str()));
  TH2F* m_si_eca_pullY = (TH2F*)(f->Get((path+"/si_eca_pullY").c_str()));
  TH2F* m_si_ecc_pullY = (TH2F*)(f->Get((path+"/si_ecc_pullY").c_str()));

  TH2F* m_si_barrel_resX = (TH2F*)(f->Get((path+"/si_barrel_resX").c_str()));
  TH2F* m_si_eca_resX = (TH2F*)(f->Get((path+"/si_eca_resX").c_str()));
  TH2F* m_si_ecc_resX = (TH2F*)(f->Get((path+"/si_ecc_resX").c_str()));
  TH2F* m_si_barrel_resY = (TH2F*)(f->Get((path+"/si_barrel_resY").c_str()));
  TH2F* m_si_eca_resY = (TH2F*)(f->Get((path+"/si_eca_resY").c_str()));
  TH2F* m_si_ecc_resY = (TH2F*)(f->Get((path+"/si_ecc_resY").c_str()));

  TH2F* m_pix_eca_xresvsmodphi_2d = (TH2F*)(f->Get((path+"/pix_eca_xresvsmodphi_2d").c_str()));
  TH2F* m_pix_eca_yresvsmodphi_2d = (TH2F*)(f->Get((path+"/pix_eca_yresvsmodphi_2d").c_str()));
  TH2F* m_pix_ecc_xresvsmodphi_2d = (TH2F*)(f->Get((path+"/pix_ecc_xresvsmodphi_2d").c_str()));
  TH2F* m_pix_ecc_yresvsmodphi_2d = (TH2F*)(f->Get((path+"/pix_ecc_yresvsmodphi_2d").c_str()));
  TH2F* m_sct_eca_xresvsmodphi_2d = (TH2F*)(f->Get((path+"/sct_eca_xresvsmodphi_2d").c_str()));
  TH2F* m_sct_ecc_xresvsmodphi_2d = (TH2F*)(f->Get((path+"/sct_ecc_xresvsmodphi_2d").c_str()));

  //3-d histograms
  TH3F* m_pix_b0_xresvsmodetaphi_3d = (TH3F*)(f->Get((path+"/pix_b0_xresvsmodetaphi_3d").c_str()));
  TH3F* m_pix_b1_xresvsmodetaphi_3d = (TH3F*)(f->Get((path+"/pix_b1_xresvsmodetaphi_3d").c_str()));
  TH3F* m_pix_b2_xresvsmodetaphi_3d = (TH3F*)(f->Get((path+"/pix_b2_xresvsmodetaphi_3d").c_str()));
  TH3F* m_pix_b3_xresvsmodetaphi_3d = (TH3F*)(f->Get((path+"/pix_b3_xresvsmodetaphi_3d").c_str()));
  TH3F* m_pix_b0_yresvsmodetaphi_3d = (TH3F*)(f->Get((path+"/pix_b0_yresvsmodetaphi_3d").c_str()));
  TH3F* m_pix_b1_yresvsmodetaphi_3d = (TH3F*)(f->Get((path+"/pix_b1_yresvsmodetaphi_3d").c_str()));
  TH3F* m_pix_b2_yresvsmodetaphi_3d = (TH3F*)(f->Get((path+"/pix_b2_yresvsmodetaphi_3d").c_str()));
  TH3F* m_pix_b3_yresvsmodetaphi_3d = (TH3F*)(f->Get((path+"/pix_b3_yresvsmodetaphi_3d").c_str()));
  TH3F* m_sct_b0_xresvsmodetaphi_3d = (TH3F*)(f->Get((path+"/sct_b0_xresvsmodetaphi_3d").c_str()));
  TH3F* m_sct_b1_xresvsmodetaphi_3d = (TH3F*)(f->Get((path+"/sct_b1_xresvsmodetaphi_3d").c_str()));
  TH3F* m_sct_b2_xresvsmodetaphi_3d = (TH3F*)(f->Get((path+"/sct_b2_xresvsmodetaphi_3d").c_str()));
  TH3F* m_sct_b3_xresvsmodetaphi_3d = (TH3F*)(f->Get((path+"/sct_b3_xresvsmodetaphi_3d").c_str()));
  
  // Get the 2DProfiles

  //TProfile2D* m_pix_b0_resXvsetaLumiBlock         = (TProfile2D*) (f->Get((path+"/pix_b0_resXvsetaLumiBlock").c_str()));
  //TProfile2D* m_pix_b0_resXvsetaLumiBlock_planars = (TProfile2D*) (f->Get((path+"/pix_b0_resXvsetaLumiBlock_planars").c_str()));

  //TH1F* m_pix_b0_mag_vs_lb = new TH1F("pix_b0_mag_vs_lb","pix_b0_mag_vs_lb",1024,0,1024);
  //TH1F* m_pix_b0_base_vs_lb = new TH1F("pix_b0_base_vs_lb","pix_b0_base_vs_lb",1024,0,1024);

  //MakeBowingFit(m_pix_b0_resXvsetaLumiBlock,m_pix_b0_mag_vs_lb,m_pix_b0_base_vs_lb);

  // Ideally we would like to clone the existing histograms from the file instead of recreating them here
  // but there seems to be no way to then persistify the cloned histograms (i.e. write them back to the file)
  const Int_t nx = 21;
  TString siliconLayersBarrel[12] = {"Pix L0","Pix L1","Pix L2","Pix L3","SCT L0 S0","S1","SCT L1 S0","S1","SCT L2 S0","S1","SCT L3 S0","S1"};
  TString siliconLayers[nx] = {"Pix L0","Pix L1","Pix L2","SCT L0 S0","S1","SCT L1 S0","S1","SCT L2 S0","S1","SCT L3 S0","S1","SCT L4 S0","S1","SCT L5 S0","S1","SCT L6 S0","S1","SCT L7 S0","S1","SCT L8 S0","S1"};    

  //pull width for each layer in Silicon barrel and endcaps
  TH1F* m_si_barrel_pullX_width = new TH1F("si_barrel_pullX_width","Pull X Gaussian Width vs Silicon Barrel Layer",12,-0.5,11.5);
  for (int i=1;i<=12;i++) m_si_barrel_pullX_width->GetXaxis()->SetBinLabel(i,siliconLayersBarrel[i-1]);   
  m_si_barrel_pullX_width->GetYaxis()->SetTitle("Pull X Gaussian Width");
  TH1F* m_si_eca_pullX_width = new TH1F("si_eca_pullX_width","Pull X Gaussian Width vs Silicon ECA Layer",21,-0.5,20.5);  
  for (int i=1;i<=nx;i++) m_si_eca_pullX_width->GetXaxis()->SetBinLabel(i,siliconLayers[i-1]);    
  m_si_eca_pullX_width->GetYaxis()->SetTitle("Pull X Gaussian Width");
  TH1F* m_si_ecc_pullX_width = new TH1F("si_ecc_pullX_width","Pull X Gaussian Width vs Silicon ECC Layer",21,-0.5,20.5);  
  for (int i=1;i<=nx;i++) m_si_ecc_pullX_width->GetXaxis()->SetBinLabel(i,siliconLayers[i-1]);    
  m_si_ecc_pullX_width->GetYaxis()->SetTitle("Pull X Gaussian Width");
  TH1F* m_si_barrel_pullY_width = new TH1F("si_barrel_pullY_width","Pull Y Gaussian Width vs Silicon Barrel Layer",12,-0.5,11.5);  
  for (int i=1;i<=12;i++) m_si_barrel_pullY_width->GetXaxis()->SetBinLabel(i,siliconLayersBarrel[i-1]);    
  m_si_barrel_pullY_width->GetYaxis()->SetTitle("Pull Y Gaussian Width");
  TH1F* m_si_eca_pullY_width = new TH1F("si_eca_pullY_width","Pull Y Gaussian Width vs Silicon ECA Layer",21,-0.5,20.5);  
  for (int i=1;i<=nx;i++) m_si_eca_pullY_width->GetXaxis()->SetBinLabel(i,siliconLayers[i-1]);    
  m_si_eca_pullY_width->GetYaxis()->SetTitle("Pull Y Gaussian Width");
  TH1F* m_si_ecc_pullY_width = new TH1F("si_ecc_pullY_width","Pull Y Gaussian Width vs Silicon ECC Layer",21,-0.5,20.5);  
  for (int i=1;i<=nx;i++) m_si_ecc_pullY_width->GetXaxis()->SetBinLabel(i,siliconLayers[i-1]);    
  m_si_ecc_pullY_width->GetYaxis()->SetTitle("Pull Y Gaussian Width");
  fillGaussianMeanOrWidth(m_si_barrel_pullX,m_si_barrel_pullX_width,-6.0,6.0,1);
  fillGaussianMeanOrWidth(m_si_barrel_pullY,m_si_barrel_pullY_width,-6.0,6.0,1);
  fillGaussianMeanOrWidth(m_si_eca_pullX,m_si_eca_pullX_width,-6.0,6.0,1);
  fillGaussianMeanOrWidth(m_si_eca_pullY,m_si_eca_pullY_width,-6.0,6.0,1);
  fillGaussianMeanOrWidth(m_si_ecc_pullX,m_si_ecc_pullX_width,-6.0,6.0,1);
  fillGaussianMeanOrWidth(m_si_ecc_pullY,m_si_ecc_pullY_width,-6.0,6.0,1);

  //pull mean for each layer in Silicon barrel and endcaps
  TH1F* m_si_barrel_pullX_mean = new TH1F("si_barrel_pullX_mean","Pull X Gaussian Mean vs Silicon Barrel Layer",12,-0.5,11.5);
  for (int i=1;i<=12;i++) m_si_barrel_pullX_mean->GetXaxis()->SetBinLabel(i,siliconLayersBarrel[i-1]);   
  m_si_barrel_pullX_mean->GetYaxis()->SetTitle("Pull X Gaussian Mean");
  TH1F* m_si_eca_pullX_mean = new TH1F("si_eca_pullX_mean","Pull X Gaussian Mean vs Silicon ECA Layer",21,-0.5,20.5);  
  for (int i=1;i<=nx;i++) m_si_eca_pullX_mean->GetXaxis()->SetBinLabel(i,siliconLayers[i-1]);    
  m_si_eca_pullX_mean->GetYaxis()->SetTitle("Pull X Gaussian Mean");
  TH1F* m_si_ecc_pullX_mean = new TH1F("si_ecc_pullX_mean","Pull X Gaussian Mean vs Silicon ECC Layer",21,-0.5,20.5);  
  for (int i=1;i<=nx;i++) m_si_ecc_pullX_mean->GetXaxis()->SetBinLabel(i,siliconLayers[i-1]);    
  m_si_ecc_pullX_mean->GetYaxis()->SetTitle("Pull X Gaussian Mean");
  TH1F* m_si_barrel_pullY_mean = new TH1F("si_barrel_pullY_mean","Pull Y Gaussian Mean vs Silicon Barrel Layer",12,-0.5,11.5);  
  for (int i=1;i<=12;i++) m_si_barrel_pullY_mean->GetXaxis()->SetBinLabel(i,siliconLayersBarrel[i-1]);    
  m_si_barrel_pullY_mean->GetYaxis()->SetTitle("Pull Y Gaussian Mean");
  TH1F* m_si_eca_pullY_mean = new TH1F("si_eca_pullY_mean","Pull Y Gaussian Mean vs Silicon ECA Layer",21,-0.5,20.5);  
  for (int i=1;i<=nx;i++) m_si_eca_pullY_mean->GetXaxis()->SetBinLabel(i,siliconLayers[i-1]);    
  m_si_eca_pullY_mean->GetYaxis()->SetTitle("Pull Y Gaussian Mean");
  TH1F* m_si_ecc_pullY_mean = new TH1F("si_ecc_pullY_mean","Pull Y Gaussian Mean vs Silicon ECC Layer",21,-0.5,20.5);  
  for (int i=1;i<=nx;i++) m_si_ecc_pullY_mean->GetXaxis()->SetBinLabel(i,siliconLayers[i-1]);    
  m_si_ecc_pullY_mean->GetYaxis()->SetTitle("Pull Y Gaussian Mean");
  fillGaussianMeanOrWidth(m_si_barrel_pullX,m_si_barrel_pullX_mean,-6.0,6.0,0);
  fillGaussianMeanOrWidth(m_si_barrel_pullY,m_si_barrel_pullY_mean,-6.0,6.0,0);
  fillGaussianMeanOrWidth(m_si_eca_pullX,m_si_eca_pullX_mean,-6.0,6.0,0);
  fillGaussianMeanOrWidth(m_si_eca_pullY,m_si_eca_pullY_mean,-6.0,6.0,0);
  fillGaussianMeanOrWidth(m_si_ecc_pullX,m_si_ecc_pullX_mean,-6.0,6.0,0);
  fillGaussianMeanOrWidth(m_si_ecc_pullY,m_si_ecc_pullY_mean,-6.0,6.0,0);

  //residual mean for each layer in Silicon barrel and endcaps
  TH1F* m_si_barrel_resX_mean = new TH1F("si_barrel_resX_mean","Residual X Mean vs Silicon Barrel Layer",12,-0.5,11.5);
  for (int i=1;i<=12;i++) m_si_barrel_resX_mean->GetXaxis()->SetBinLabel(i,siliconLayersBarrel[i-1]);   
  m_si_barrel_resX_mean->GetYaxis()->SetTitle("Residual X Mean [mm]");
  TH1F* m_si_eca_resX_mean = new TH1F("si_eca_resX_mean","Residual X Mean vs Silicon ECA Layer",21,-0.5,20.5);  
  for (int i=1;i<=nx;i++) m_si_eca_resX_mean->GetXaxis()->SetBinLabel(i,siliconLayers[i-1]);    
  m_si_eca_resX_mean->GetYaxis()->SetTitle("Residual X Mean [mm]");
  TH1F* m_si_ecc_resX_mean = new TH1F("si_ecc_resX_mean","Residual X Mean vs Silicon ECC Layer",21,-0.5,20.5);  
  for (int i=1;i<=nx;i++) m_si_ecc_resX_mean->GetXaxis()->SetBinLabel(i,siliconLayers[i-1]);    
  m_si_ecc_resX_mean->GetYaxis()->SetTitle("Residual X Mean [mm]");
  TH1F* m_si_barrel_resY_mean = new TH1F("si_barrel_resY_mean","Residual Y Mean vs Silicon Barrel Layer",12,-0.5,11.5);  
  for (int i=1;i<=12;i++) m_si_barrel_resY_mean->GetXaxis()->SetBinLabel(i,siliconLayersBarrel[i-1]);    
  m_si_barrel_resY_mean->GetYaxis()->SetTitle("Residual Y Mean [mm]");
  TH1F* m_si_eca_resY_mean = new TH1F("si_eca_resY_mean","Residual Y Mean vs Silicon ECA Layer",21,-0.5,20.5);  
  for (int i=1;i<=nx;i++) m_si_eca_resY_mean->GetXaxis()->SetBinLabel(i,siliconLayers[i-1]);    
  m_si_eca_resY_mean->GetYaxis()->SetTitle("Residual Y Mean [mm]");
  TH1F* m_si_ecc_resY_mean = new TH1F("si_ecc_resY_mean","Residual Y Mean vs Silicon ECC Layer",21,-0.5,20.5);  
  for (int i=1;i<=nx;i++) m_si_ecc_resY_mean->GetXaxis()->SetBinLabel(i,siliconLayers[i-1]);    
  m_si_ecc_resY_mean->GetYaxis()->SetTitle("Residual Y Mean [mm]");
  meanRMSProjections2D(m_si_barrel_resX,m_si_barrel_resX_mean,2);
  meanRMSProjections2D(m_si_barrel_resY,m_si_barrel_resY_mean,2);
  meanRMSProjections2D(m_si_eca_resX,m_si_eca_resX_mean,2);
  meanRMSProjections2D(m_si_eca_resY,m_si_eca_resY_mean,2);
  meanRMSProjections2D(m_si_ecc_resX,m_si_ecc_resX_mean,2);
  meanRMSProjections2D(m_si_ecc_resY,m_si_ecc_resY_mean,2);

  //residual width for each layer in Silicon barrel and endcaps
  TH1F* m_si_barrel_resX_rms = new TH1F("si_barrel_resX_rms","Residual X Width vs Silicon Barrel Layer",12,-0.5,11.5);
  for (int i=1;i<=12;i++) m_si_barrel_resX_rms->GetXaxis()->SetBinLabel(i,siliconLayersBarrel[i-1]);   
  m_si_barrel_resX_rms->GetYaxis()->SetTitle("Residual X Width [mm]");
  TH1F* m_si_eca_resX_rms = new TH1F("si_eca_resX_rms","Residual X Width vs Silicon ECA Layer",21,-0.5,20.5);  
  for (int i=1;i<=nx;i++) m_si_eca_resX_rms->GetXaxis()->SetBinLabel(i,siliconLayers[i-1]);    
  m_si_eca_resX_rms->GetYaxis()->SetTitle("Residual X Width [mm]");
  TH1F* m_si_ecc_resX_rms = new TH1F("si_ecc_resX_rms","Residual X Width vs Silicon ECC Layer",21,-0.5,20.5);  
  for (int i=1;i<=nx;i++) m_si_ecc_resX_rms->GetXaxis()->SetBinLabel(i,siliconLayers[i-1]);    
  m_si_ecc_resX_rms->GetYaxis()->SetTitle("Residual X Width [mm]");
  TH1F* m_si_barrel_resY_rms = new TH1F("si_barrel_resY_rms","Residual Y Width vs Silicon Barrel Layer",12,-0.5,11.5);  
  for (int i=1;i<=12;i++) m_si_barrel_resY_rms->GetXaxis()->SetBinLabel(i,siliconLayersBarrel[i-1]);    
  m_si_barrel_resY_rms->GetYaxis()->SetTitle("Residual Y Width [mm]");
  TH1F* m_si_eca_resY_rms = new TH1F("si_eca_resY_rms","Residual Y Width vs Silicon ECA Layer",21,-0.5,20.5);  
  for (int i=1;i<=nx;i++) m_si_eca_resY_rms->GetXaxis()->SetBinLabel(i,siliconLayers[i-1]);    
  m_si_eca_resY_rms->GetYaxis()->SetTitle("Residual Y Width [mm]");
  TH1F* m_si_ecc_resY_rms = new TH1F("si_ecc_resY_rms","Residual Y Width vs Silicon ECC Layer",21,-0.5,20.5);  
  for (int i=1;i<=nx;i++) m_si_ecc_resY_rms->GetXaxis()->SetBinLabel(i,siliconLayers[i-1]);    
  m_si_ecc_resY_rms->GetYaxis()->SetTitle("Residual Y Width [mm]");
  meanRMSProjections2D(m_si_barrel_resX,m_si_barrel_resX_rms,3);
  meanRMSProjections2D(m_si_barrel_resY,m_si_barrel_resY_rms,3);
  meanRMSProjections2D(m_si_eca_resX,m_si_eca_resX_rms,3);
  meanRMSProjections2D(m_si_eca_resY,m_si_eca_resY_rms,3);
  meanRMSProjections2D(m_si_ecc_resX,m_si_ecc_resX_rms,3);
  meanRMSProjections2D(m_si_ecc_resY,m_si_ecc_resY_rms,3);

  //x residual mean as a function of ring in the Pixel barrel
  
  TH1F* m_pix_b0_xresvsmodeta = new TH1F("pix_b0_xresvsmodeta","X Residual Mean vs Eta-ID Pixel Barrel L0",20,-10.5,9.5);
  m_pix_b0_xresvsmodeta->GetYaxis()->SetTitle("Mean Residual X [mm]");
  m_pix_b0_xresvsmodeta->GetXaxis()->SetTitle("Module Eta-ID");
  TH1F* m_pix_b1_xresvsmodeta = new TH1F("pix_b1_xresvsmodeta","X Residual Mean vs Eta-ID Pixel Barrel L1",13,-6.5,6.5);
  m_pix_b1_xresvsmodeta->GetYaxis()->SetTitle("Mean Residual X [mm]");
  m_pix_b1_xresvsmodeta->GetXaxis()->SetTitle("Module Eta-ID");
  TH1F* m_pix_b2_xresvsmodeta = new TH1F("pix_b2_xresvsmodeta","X Residual Mean vs Eta-ID Pixel Barrel L2",13,-6.5,6.5);
  m_pix_b2_xresvsmodeta->GetYaxis()->SetTitle("Mean Residual X [mm]");
  m_pix_b2_xresvsmodeta->GetXaxis()->SetTitle("Module Eta-ID");
  TH1F* m_pix_b3_xresvsmodeta = new TH1F("pix_b3_xresvsmodeta","X Residual Mean vs Eta-ID Pixel Barrel L3",13,-6.5,6.5);
  m_pix_b3_xresvsmodeta->GetYaxis()->SetTitle("Mean Residual X [mm]");
  m_pix_b3_xresvsmodeta->GetXaxis()->SetTitle("Module Eta-ID");

  meanRMSProjections3D_XY(m_pix_b0_xresvsmodetaphi_3d,m_pix_b0_xresvsmodeta,0,2);
  meanRMSProjections3D_XY(m_pix_b1_xresvsmodetaphi_3d,m_pix_b1_xresvsmodeta,0,2);
  meanRMSProjections3D_XY(m_pix_b2_xresvsmodetaphi_3d,m_pix_b2_xresvsmodeta,0,2);
  meanRMSProjections3D_XY(m_pix_b3_xresvsmodetaphi_3d,m_pix_b3_xresvsmodeta,0,2);
  plots->Add(m_pix_b0_xresvsmodeta);
  plots->Add(m_pix_b1_xresvsmodeta);
  plots->Add(m_pix_b2_xresvsmodeta);
  plots->Add(m_pix_b3_xresvsmodeta);
  TH1F* m_pix_b_xresvsmodeta = combineHistos("pix_b_xresvsmodeta","X Residual Mean vs (Modified) Eta-ID Pixel Barrel",plots,10);
  m_pix_b_xresvsmodeta->GetYaxis()->SetTitle("Mean Residual X [mm]");
  m_pix_b_xresvsmodeta->GetXaxis()->SetTitle("(Modified) Eta-ID");
  plots->Clear();

  //x residual width as a function of ring in the Pixel barrel
  TH1F* m_pix_b0_xresvsmodeta_width = new TH1F("pix_b0_xresvsmodeta_width","X Residual Width vs Eta-ID Pixel Barrel L0",20,-10.5,9.5);
  m_pix_b0_xresvsmodeta_width->GetYaxis()->SetTitle("Width Residual X [mm]");
  m_pix_b0_xresvsmodeta_width->GetXaxis()->SetTitle("Module Eta-ID");
  TH1F* m_pix_b1_xresvsmodeta_width = new TH1F("pix_b1_xresvsmodeta_width","X Residual Width vs Eta-ID Pixel Barrel L1",13,-6.5,6.5);
  m_pix_b1_xresvsmodeta_width->GetYaxis()->SetTitle("Width Residual X [mm]");
  m_pix_b1_xresvsmodeta_width->GetXaxis()->SetTitle("Module Eta-ID");
  TH1F* m_pix_b2_xresvsmodeta_width = new TH1F("pix_b2_xresvsmodeta_width","X Residual Width vs Eta-ID Pixel Barrel L2",13,-6.5,6.5);
  m_pix_b2_xresvsmodeta_width->GetYaxis()->SetTitle("Width Residual X [mm]");
  m_pix_b2_xresvsmodeta_width->GetXaxis()->SetTitle("Module Eta-ID");
  TH1F* m_pix_b3_xresvsmodeta_width = new TH1F("pix_b3_xresvsmodeta_width","X Residual Width vs Eta-ID Pixel Barrel L3",13,-6.5,6.5);
  m_pix_b3_xresvsmodeta_width->GetYaxis()->SetTitle("Width Residual X [mm]");
  m_pix_b3_xresvsmodeta_width->GetXaxis()->SetTitle("Module Eta-ID");
  meanRMSProjections3D_XY(m_pix_b0_xresvsmodetaphi_3d,m_pix_b0_xresvsmodeta_width,0,3);
  meanRMSProjections3D_XY(m_pix_b1_xresvsmodetaphi_3d,m_pix_b1_xresvsmodeta_width,0,3);
  meanRMSProjections3D_XY(m_pix_b2_xresvsmodetaphi_3d,m_pix_b2_xresvsmodeta_width,0,3);
  meanRMSProjections3D_XY(m_pix_b3_xresvsmodetaphi_3d,m_pix_b3_xresvsmodeta_width,0,3);
  plots->Add(m_pix_b0_xresvsmodeta_width);
  plots->Add(m_pix_b1_xresvsmodeta_width);
  plots->Add(m_pix_b2_xresvsmodeta_width);
  plots->Add(m_pix_b3_xresvsmodeta_width);
  TH1F* m_pix_b_xresvsmodeta_width = combineHistos("pix_b_xresvsmodeta_width","X Residual Width vs (Modified) Eta-ID Pixel Barrel",plots,10);
  m_pix_b_xresvsmodeta_width->GetYaxis()->SetTitle("Width Residual X [mm]");
  m_pix_b_xresvsmodeta_width->GetXaxis()->SetTitle("(Modified) Eta-ID");
  plots->Clear();

  //y residual mean as a function of ring in the Pixel barrel
  TH1F* m_pix_b0_yresvsmodeta = new TH1F("pix_b0_yresvsmodeta","Y Residual Mean vs Eta-ID Pixel Barrel L0",20,-10.5,9.5);
  m_pix_b0_yresvsmodeta->GetYaxis()->SetTitle("Mean Residual Y [mm]");
  m_pix_b0_yresvsmodeta->GetXaxis()->SetTitle("Module Eta-ID");
  TH1F* m_pix_b1_yresvsmodeta = new TH1F("pix_b1_yresvsmodeta","Y Residual Mean vs Eta-ID Pixel Barrel L1",13,-6.5,6.5);
  m_pix_b1_yresvsmodeta->GetYaxis()->SetTitle("Mean Residual Y [mm]");
  m_pix_b1_yresvsmodeta->GetXaxis()->SetTitle("Module Eta-ID");
  TH1F* m_pix_b2_yresvsmodeta = new TH1F("pix_b2_yresvsmodeta","Y Residual Mean vs Eta-ID Pixel Barrel L2",13,-6.5,6.5);
  m_pix_b2_yresvsmodeta->GetYaxis()->SetTitle("Mean Residual Y [mm]");
  m_pix_b2_yresvsmodeta->GetXaxis()->SetTitle("Module Eta-ID");
  TH1F* m_pix_b3_yresvsmodeta = new TH1F("pix_b3_yresvsmodeta","Y Residual Mean vs Eta-ID Pixel Barrel L2",13,-6.5,6.5);
  m_pix_b3_yresvsmodeta->GetYaxis()->SetTitle("Mean Residual Y [mm]");
  m_pix_b3_yresvsmodeta->GetXaxis()->SetTitle("Module Eta-ID");
  meanRMSProjections3D_XY(m_pix_b0_yresvsmodetaphi_3d,m_pix_b0_yresvsmodeta,0,2);
  meanRMSProjections3D_XY(m_pix_b1_yresvsmodetaphi_3d,m_pix_b1_yresvsmodeta,0,2);
  meanRMSProjections3D_XY(m_pix_b2_yresvsmodetaphi_3d,m_pix_b2_yresvsmodeta,0,2);
  meanRMSProjections3D_XY(m_pix_b3_yresvsmodetaphi_3d,m_pix_b3_yresvsmodeta,0,2);
  plots->Add(m_pix_b0_yresvsmodeta);
  plots->Add(m_pix_b1_yresvsmodeta);
  plots->Add(m_pix_b2_yresvsmodeta);
  plots->Add(m_pix_b3_yresvsmodeta);
  TH1F* m_pix_b_yresvsmodeta = combineHistos("pix_b_yresvsmodeta","Y Residual Mean vs (Modified) Eta-ID Pixel Barrel",plots,10);
  m_pix_b_yresvsmodeta->GetYaxis()->SetTitle("Mean Residual Y [mm]");
  m_pix_b_yresvsmodeta->GetXaxis()->SetTitle("(Modified) Eta-ID");
  plots->Clear();

  //y residual width as a function of ring in the Pixel barrel
  TH1F* m_pix_b0_yresvsmodeta_width = new TH1F("pix_b0_yresvsmodeta_width","Y Residual Width vs Eta-ID Pixel Barrel L0",20,-10.5,9.5);
  m_pix_b0_yresvsmodeta_width->GetYaxis()->SetTitle("Width Residual Y [mm]");
  m_pix_b0_yresvsmodeta_width->GetXaxis()->SetTitle("Module Eta-ID");
  TH1F* m_pix_b1_yresvsmodeta_width = new TH1F("pix_b1_yresvsmodeta_width","Y Residual Width vs Eta-ID Pixel Barrel L1",13,-6.5,6.5);
  m_pix_b1_yresvsmodeta_width->GetYaxis()->SetTitle("Width Residual Y [mm]");
  m_pix_b1_yresvsmodeta_width->GetXaxis()->SetTitle("Module Eta-ID");
  TH1F* m_pix_b2_yresvsmodeta_width = new TH1F("pix_b2_yresvsmodeta_width","Y Residual Width vs Eta-ID Pixel Barrel L2",13,-6.5,6.5);
  m_pix_b2_yresvsmodeta_width->GetYaxis()->SetTitle("Width Residual Y [mm]");
  m_pix_b2_yresvsmodeta_width->GetXaxis()->SetTitle("Module Eta-ID");
   TH1F* m_pix_b3_yresvsmodeta_width = new TH1F("pix_b3_yresvsmodeta_width","Y Residual Width vs Eta-ID Pixel Barrel L2",13,-6.5,6.5);
  m_pix_b3_yresvsmodeta_width->GetYaxis()->SetTitle("Width Residual Y [mm]");
  m_pix_b3_yresvsmodeta_width->GetXaxis()->SetTitle("Module Eta-ID");

  meanRMSProjections3D_XY(m_pix_b0_yresvsmodetaphi_3d,m_pix_b0_yresvsmodeta_width,0,3);
  meanRMSProjections3D_XY(m_pix_b1_yresvsmodetaphi_3d,m_pix_b1_yresvsmodeta_width,0,3);
  meanRMSProjections3D_XY(m_pix_b2_yresvsmodetaphi_3d,m_pix_b2_yresvsmodeta_width,0,3);
  meanRMSProjections3D_XY(m_pix_b3_yresvsmodetaphi_3d,m_pix_b3_yresvsmodeta_width,0,3);
  plots->Add(m_pix_b0_yresvsmodeta_width);
  plots->Add(m_pix_b1_yresvsmodeta_width);
  plots->Add(m_pix_b2_yresvsmodeta_width);
  plots->Add(m_pix_b3_yresvsmodeta_width);
   
  TH1F* m_pix_b_yresvsmodeta_width = combineHistos("pix_b_yresvsmodeta_width","Y Residual Width vs (Modified) Eta-ID Pixel Barrel",plots,10);
  m_pix_b_yresvsmodeta_width->GetYaxis()->SetTitle("Width Residual Y [mm]");
  m_pix_b_yresvsmodeta_width->GetXaxis()->SetTitle("(Modified) Eta-ID");
  plots->Clear();

  //x residual mean as a function of stave in the Pixel barrel

  TH1F* m_pix_b0_xresvsmodphi = new TH1F("pix_b0_xresvsmodphi","X Residual Mean vs Phi-ID Pixel Barrel L0",14,0,14);
  m_pix_b0_xresvsmodphi->GetYaxis()->SetTitle("Mean Residual X [mm]");
  m_pix_b0_xresvsmodphi->GetXaxis()->SetTitle("Module Phi-ID");
  TH1F* m_pix_b1_xresvsmodphi = new TH1F("pix_b1_xresvsmodphi","X Residual Mean vs Phi-ID Pixel Barrel L0",22,-0,22);
  m_pix_b1_xresvsmodphi->GetYaxis()->SetTitle("Mean Residual X [mm]");
  m_pix_b1_xresvsmodphi->GetXaxis()->SetTitle("Module Phi-ID");
  TH1F* m_pix_b2_xresvsmodphi = new TH1F("pix_b2_xresvsmodphi","X Residual Mean vs Phi-ID Pixel Barrel L1",38,0,38);
  m_pix_b2_xresvsmodphi->GetYaxis()->SetTitle("Mean Residual X [mm]");
  m_pix_b2_xresvsmodphi->GetXaxis()->SetTitle("Module Phi-ID");
  TH1F* m_pix_b3_xresvsmodphi = new TH1F("pix_b3_xresvsmodphi","X Residual Mean vs Phi-ID Pixel Barrel L2",52,0,52);
  m_pix_b3_xresvsmodphi->GetYaxis()->SetTitle("Mean Residual X [mm]");
  m_pix_b3_xresvsmodphi->GetXaxis()->SetTitle("Module Phi-ID");
  
  
  meanRMSProjections3D_XY(m_pix_b0_xresvsmodetaphi_3d,m_pix_b0_xresvsmodphi,1,2);
  meanRMSProjections3D_XY(m_pix_b1_xresvsmodetaphi_3d,m_pix_b1_xresvsmodphi,1,2);
  meanRMSProjections3D_XY(m_pix_b2_xresvsmodetaphi_3d,m_pix_b2_xresvsmodphi,1,2);
  meanRMSProjections3D_XY(m_pix_b3_xresvsmodetaphi_3d,m_pix_b3_xresvsmodphi,1,2);
  
  plots->Add(m_pix_b0_xresvsmodphi);
  plots->Add(m_pix_b1_xresvsmodphi);
  plots->Add(m_pix_b2_xresvsmodphi);
  plots->Add(m_pix_b3_xresvsmodphi);
  TH1F* m_pix_b_xresvsmodphi = combineHistos("pix_b_xresvsmodphi","X Residual Mean vs (Modified) Phi-ID Pixel Barrel",plots,10);
  m_pix_b_xresvsmodphi->GetYaxis()->SetTitle("Mean Residual X [mm]");
  m_pix_b_xresvsmodphi->GetXaxis()->SetTitle("(Modified) Phi-ID");
  plots->Clear();

  //x residual width as a function of stave in the Pixel barrel
  TH1F* m_pix_b0_xresvsmodphi_width = new TH1F("pix_b0_xresvsmodphi_width","X Residual Width vs Phi-ID Pixel Barrel L0",14,0,14);
  m_pix_b0_xresvsmodphi_width->GetYaxis()->SetTitle("Width Residual X [mm]");
  m_pix_b0_xresvsmodphi_width->GetXaxis()->SetTitle("Module Phi-ID");
  TH1F* m_pix_b1_xresvsmodphi_width = new TH1F("pix_b1_xresvsmodphi_width","X Residual Width vs Phi-ID Pixel Barrel L0",22,0.,22);
  m_pix_b1_xresvsmodphi_width->GetYaxis()->SetTitle("Width Residual X [mm]");
  m_pix_b1_xresvsmodphi_width->GetXaxis()->SetTitle("Module Phi-ID");
  TH1F* m_pix_b2_xresvsmodphi_width = new TH1F("pix_b2_xresvsmodphi_width","X Residual Width vs Phi-ID Pixel Barrel L1",38,0,38);
  m_pix_b2_xresvsmodphi_width->GetYaxis()->SetTitle("Width Residual X [mm]");
  m_pix_b2_xresvsmodphi_width->GetXaxis()->SetTitle("Module Phi-ID");
  TH1F* m_pix_b3_xresvsmodphi_width = new TH1F("pix_b3_xresvsmodphi_width","X Residual Width vs Phi-ID Pixel Barrel L2",52,0,52);
  m_pix_b3_xresvsmodphi_width->GetYaxis()->SetTitle("Width Residual X [mm]");
  m_pix_b3_xresvsmodphi_width->GetXaxis()->SetTitle("Module Phi-ID");
  meanRMSProjections3D_XY(m_pix_b0_xresvsmodetaphi_3d,m_pix_b0_xresvsmodphi_width,1,3);
  meanRMSProjections3D_XY(m_pix_b1_xresvsmodetaphi_3d,m_pix_b1_xresvsmodphi_width,1,3);
  meanRMSProjections3D_XY(m_pix_b2_xresvsmodetaphi_3d,m_pix_b2_xresvsmodphi_width,1,3);
  meanRMSProjections3D_XY(m_pix_b3_xresvsmodetaphi_3d,m_pix_b3_xresvsmodphi_width,1,3);
  plots->Add(m_pix_b0_xresvsmodphi_width);
  plots->Add(m_pix_b1_xresvsmodphi_width);
  plots->Add(m_pix_b2_xresvsmodphi_width);
  plots->Add(m_pix_b3_xresvsmodphi_width);
  TH1F* m_pix_b_xresvsmodphi_width = combineHistos("pix_b_xresvsmodphi_width","X Residual Width vs (Modified) Phi-ID Pixel Barrel",plots,10);
  m_pix_b_xresvsmodphi_width->GetYaxis()->SetTitle("Width Residual X [mm]");
  m_pix_b_xresvsmodphi_width->GetXaxis()->SetTitle("(Modified) Phi-ID");
  plots->Clear();

  //y residual mean as a function of stave in the Pixel barrel
  TH1F* m_pix_b0_yresvsmodphi = new TH1F("pix_b0_yresvsmodphi","Y Residual Mean vs Phi-ID Pixel Barrel L0",14,0,14);
  m_pix_b0_yresvsmodphi->GetYaxis()->SetTitle("Mean Residual Y [mm]");
  m_pix_b0_yresvsmodphi->GetXaxis()->SetTitle("Module Phi-ID");
  TH1F* m_pix_b1_yresvsmodphi = new TH1F("pix_b1_yresvsmodphi","Y Residual Mean vs Phi-ID Pixel Barrel L0",22,0,22);
  m_pix_b1_yresvsmodphi->GetYaxis()->SetTitle("Mean Residual Y [mm]");
  m_pix_b1_yresvsmodphi->GetXaxis()->SetTitle("Module Phi-ID");
  TH1F* m_pix_b2_yresvsmodphi = new TH1F("pix_b2_yresvsmodphi","Y Residual Mean vs Phi-ID Pixel Barrel L1",38,0,38);
  m_pix_b2_yresvsmodphi->GetYaxis()->SetTitle("Mean Residual Y [mm]");
  m_pix_b2_yresvsmodphi->GetXaxis()->SetTitle("Module Phi-ID");
  TH1F* m_pix_b3_yresvsmodphi = new TH1F("pix_b3_yresvsmodphi","Y Residual Mean vs Phi-ID Pixel Barrel L2",52,0,52);
  m_pix_b3_yresvsmodphi->GetYaxis()->SetTitle("Mean Residual Y [mm]");
  m_pix_b3_yresvsmodphi->GetXaxis()->SetTitle("Module Phi-ID");
  meanRMSProjections3D_XY(m_pix_b0_yresvsmodetaphi_3d,m_pix_b0_yresvsmodphi,1,2);
  meanRMSProjections3D_XY(m_pix_b1_yresvsmodetaphi_3d,m_pix_b1_yresvsmodphi,1,2);
  meanRMSProjections3D_XY(m_pix_b2_yresvsmodetaphi_3d,m_pix_b2_yresvsmodphi,1,2);
  meanRMSProjections3D_XY(m_pix_b3_yresvsmodetaphi_3d,m_pix_b3_yresvsmodphi,1,2);
  plots->Add(m_pix_b0_yresvsmodphi);
  plots->Add(m_pix_b1_yresvsmodphi);
  plots->Add(m_pix_b2_yresvsmodphi);
  plots->Add(m_pix_b3_yresvsmodphi);
  
  TH1F* m_pix_b_yresvsmodphi = combineHistos("pix_b_yresvsmodphi","Y Residual Mean vs (Modified) Phi-ID Pixel Barrel",plots,10);
  m_pix_b_yresvsmodphi->GetYaxis()->SetTitle("Mean Residual Y [mm]");
  m_pix_b_yresvsmodphi->GetXaxis()->SetTitle("(Modified) Phi-ID");
  plots->Clear();

  //y residual width as a function of stave in the Pixel barrel
  TH1F* m_pix_b0_yresvsmodphi_width = new TH1F("pix_b0_yresvsmodphi_width","Y Residual Width vs Phi-ID Pixel Barrel L0",14,0,14);
  m_pix_b0_yresvsmodphi_width->GetYaxis()->SetTitle("Width Residual Y [mm]");
  m_pix_b0_yresvsmodphi_width->GetXaxis()->SetTitle("Module Phi-ID");
  TH1F* m_pix_b1_yresvsmodphi_width = new TH1F("pix_b1_yresvsmodphi_width","Y Residual Width vs Phi-ID Pixel Barrel L0",22,-0,22);
  m_pix_b1_yresvsmodphi_width->GetYaxis()->SetTitle("Width Residual Y [mm]");
  m_pix_b1_yresvsmodphi_width->GetXaxis()->SetTitle("Module Phi-ID");
  TH1F* m_pix_b2_yresvsmodphi_width = new TH1F("pix_b2_yresvsmodphi_width","Y Residual Width vs Phi-ID Pixel Barrel L1",38,0,38);
  m_pix_b2_yresvsmodphi_width->GetYaxis()->SetTitle("Width Residual Y [mm]");
  m_pix_b2_yresvsmodphi_width->GetXaxis()->SetTitle("Module Phi-ID");
  TH1F* m_pix_b3_yresvsmodphi_width = new TH1F("pix_b3_yresvsmodphi_width","Y Residual Width vs Phi-ID Pixel Barrel L2",52,0,52);
  m_pix_b3_yresvsmodphi_width->GetYaxis()->SetTitle("Width Residual Y [mm]");
  m_pix_b3_yresvsmodphi_width->GetXaxis()->SetTitle("Module Phi-ID");
  meanRMSProjections3D_XY(m_pix_b0_yresvsmodetaphi_3d,m_pix_b0_yresvsmodphi_width,1,3);
  meanRMSProjections3D_XY(m_pix_b1_yresvsmodetaphi_3d,m_pix_b1_yresvsmodphi_width,1,3);
  meanRMSProjections3D_XY(m_pix_b2_yresvsmodetaphi_3d,m_pix_b2_yresvsmodphi_width,1,3);
  meanRMSProjections3D_XY(m_pix_b3_yresvsmodetaphi_3d,m_pix_b3_yresvsmodphi_width,1,3);
  plots->Add(m_pix_b0_yresvsmodphi_width);
  plots->Add(m_pix_b1_yresvsmodphi_width);
  plots->Add(m_pix_b2_yresvsmodphi_width);
  plots->Add(m_pix_b3_yresvsmodphi_width);
  TH1F* m_pix_b_yresvsmodphi_width = combineHistos("pix_b_yresvsmodphi_width","Y Residual Width vs (Modified) Phi-ID Pixel Barrel",plots,10);
  m_pix_b_yresvsmodphi_width->GetYaxis()->SetTitle("Width Residual Y [mm]");
  m_pix_b_yresvsmodphi_width->GetXaxis()->SetTitle("(Modified) Phi-ID");
  plots->Clear();

  //x residual mean as a function of ring in the SCT barrel
  TH1F* m_sct_b0_xresvsmodeta = new TH1F("sct_b0_xresvsmodeta","X Residual Mean vs Eta-ID SCT Barrel L0",13,-0.5,12.5);
  m_pix_b0_xresvsmodeta->GetYaxis()->SetTitle("Mean Residual X [mm]");
  m_pix_b0_xresvsmodeta->GetXaxis()->SetTitle("Module Eta-ID");
  TH1F* m_sct_b1_xresvsmodeta = new TH1F("sct_b1_xresvsmodeta","X Residual Mean vs Eta-ID SCT Barrel L1",13,-0.5,12.5);
  m_sct_b1_xresvsmodeta->GetYaxis()->SetTitle("Mean Residual X [mm]");
  m_sct_b1_xresvsmodeta->GetXaxis()->SetTitle("Module Eta-ID");
  TH1F* m_sct_b2_xresvsmodeta = new TH1F("sct_b2_xresvsmodeta","X Residual Mean vs Eta-ID SCT Barrel L2",13,-0.5,12.5);
  m_sct_b2_xresvsmodeta->GetYaxis()->SetTitle("Mean Residual X [mm]");
  m_sct_b2_xresvsmodeta->GetXaxis()->SetTitle("Module Eta-ID");
  TH1F* m_sct_b3_xresvsmodeta = new TH1F("sct_b3_xresvsmodeta","X Residual Mean vs Eta-ID SCT Barrel L3",13,-0.5,12.5);
  m_sct_b3_xresvsmodeta->GetYaxis()->SetTitle("Mean Residual X [mm]");
  m_sct_b3_xresvsmodeta->GetXaxis()->SetTitle("Module Eta-ID");
  meanRMSProjections3D_XY(m_sct_b0_xresvsmodetaphi_3d,m_sct_b0_xresvsmodeta,0,2);
  meanRMSProjections3D_XY(m_sct_b1_xresvsmodetaphi_3d,m_sct_b1_xresvsmodeta,0,2);
  meanRMSProjections3D_XY(m_sct_b2_xresvsmodetaphi_3d,m_sct_b2_xresvsmodeta,0,2);
  meanRMSProjections3D_XY(m_sct_b3_xresvsmodetaphi_3d,m_sct_b3_xresvsmodeta,0,2);
  plots->Add(m_sct_b0_xresvsmodeta);
  plots->Add(m_sct_b1_xresvsmodeta);
  plots->Add(m_sct_b2_xresvsmodeta);
  plots->Add(m_sct_b3_xresvsmodeta);
  TH1F* m_sct_b_xresvsmodeta = combineHistos("sct_b_xresvsmodeta","X Residual Mean vs (Modified) Eta-ID SCT Barrel",plots,10);
  m_sct_b_xresvsmodeta->GetYaxis()->SetTitle("Mean Residual X [mm]");
  m_sct_b_xresvsmodeta->GetXaxis()->SetTitle("(Modified) Eta-ID");
  plots->Clear();

  //x residual width as a function of ring in the SCT barrel
  TH1F* m_sct_b0_xresvsmodeta_width = new TH1F("sct_b0_xresvsmodeta_width","X Residual Width vs Eta-ID SCT Barrel L0",13,-0.5,12.5);
  m_pix_b0_xresvsmodeta_width->GetYaxis()->SetTitle("Width Residual X [mm]");
  m_pix_b0_xresvsmodeta_width->GetXaxis()->SetTitle("Module Eta-ID");
  TH1F* m_sct_b1_xresvsmodeta_width = new TH1F("sct_b1_xresvsmodeta_width","X Residual Width vs Eta-ID SCT Barrel L1",13,-0.5,12.5);
  m_sct_b1_xresvsmodeta_width->GetYaxis()->SetTitle("Width Residual X [mm]");
  m_sct_b1_xresvsmodeta_width->GetXaxis()->SetTitle("Module Eta-ID");
  TH1F* m_sct_b2_xresvsmodeta_width = new TH1F("sct_b2_xresvsmodeta_width","X Residual Width vs Eta-ID SCT Barrel L2",13,-0.5,12.5);
  m_sct_b2_xresvsmodeta_width->GetYaxis()->SetTitle("Width Residual X [mm]");
  m_sct_b2_xresvsmodeta_width->GetXaxis()->SetTitle("Module Eta-ID");
  TH1F* m_sct_b3_xresvsmodeta_width = new TH1F("sct_b3_xresvsmodeta_width","X Residual Width vs Eta-ID SCT Barrel L3",13,-0.5,12.5);
  m_sct_b3_xresvsmodeta_width->GetYaxis()->SetTitle("Width Residual X [mm]");
  m_sct_b3_xresvsmodeta_width->GetXaxis()->SetTitle("Module Eta-ID");
  meanRMSProjections3D_XY(m_sct_b0_xresvsmodetaphi_3d,m_sct_b0_xresvsmodeta_width,0,3);
  meanRMSProjections3D_XY(m_sct_b1_xresvsmodetaphi_3d,m_sct_b1_xresvsmodeta_width,0,3);
  meanRMSProjections3D_XY(m_sct_b2_xresvsmodetaphi_3d,m_sct_b2_xresvsmodeta_width,0,3);
  meanRMSProjections3D_XY(m_sct_b3_xresvsmodetaphi_3d,m_sct_b3_xresvsmodeta_width,0,3);
  plots->Add(m_sct_b0_xresvsmodeta_width);
  plots->Add(m_sct_b1_xresvsmodeta_width);
  plots->Add(m_sct_b2_xresvsmodeta_width);
  plots->Add(m_sct_b3_xresvsmodeta_width);
  TH1F* m_sct_b_xresvsmodeta_width = combineHistos("sct_b_xresvsmodeta_width","X Residual Width vs (Modified) Eta-ID SCT Barrel",plots,10);
  m_sct_b_xresvsmodeta_width->GetYaxis()->SetTitle("Width Residual X [mm]");
  m_sct_b_xresvsmodeta_width->GetXaxis()->SetTitle("(Modified) Eta-ID");
  plots->Clear();

  //x residual mean as a function of stave in the SCT barrel
  TH1F* m_sct_b0_xresvsmodphi = new TH1F("sct_b0_xresvsmodphi","X Residual Mean vs Phi-ID SCT Barrel L0",32,-0.5,31.5);
  m_pix_b0_xresvsmodphi->GetYaxis()->SetTitle("Mean Residual X [mm]");
  m_pix_b0_xresvsmodphi->GetXaxis()->SetTitle("Module Phi-ID");
  TH1F* m_sct_b1_xresvsmodphi = new TH1F("sct_b1_xresvsmodphi","X Residual Mean vs Phi-ID SCT Barrel L1",40,-0.5,39.5);
  m_sct_b1_xresvsmodphi->GetYaxis()->SetTitle("Mean Residual X [mm]");
  m_sct_b1_xresvsmodphi->GetXaxis()->SetTitle("Module Phi-ID");
  TH1F* m_sct_b2_xresvsmodphi = new TH1F("sct_b2_xresvsmodphi","X Residual Mean vs Phi-ID SCT Barrel L2",48,-0.5,47.5);
  m_sct_b2_xresvsmodphi->GetYaxis()->SetTitle("Mean Residual X [mm]");
  m_sct_b2_xresvsmodphi->GetXaxis()->SetTitle("Module Phi-ID");
  TH1F* m_sct_b3_xresvsmodphi = new TH1F("sct_b3_xresvsmodphi","X Residual Mean vs Phi-ID SCT Barrel L3",56,-0.5,55.5);
  m_sct_b3_xresvsmodphi->GetYaxis()->SetTitle("Mean Residual X [mm]");
  m_sct_b3_xresvsmodphi->GetXaxis()->SetTitle("Module Phi-ID");
  meanRMSProjections3D_XY(m_sct_b0_xresvsmodetaphi_3d,m_sct_b0_xresvsmodphi,1,2);
  meanRMSProjections3D_XY(m_sct_b1_xresvsmodetaphi_3d,m_sct_b1_xresvsmodphi,1,2);
  meanRMSProjections3D_XY(m_sct_b2_xresvsmodetaphi_3d,m_sct_b2_xresvsmodphi,1,2);
  meanRMSProjections3D_XY(m_sct_b3_xresvsmodetaphi_3d,m_sct_b3_xresvsmodphi,1,2);
  plots->Add(m_sct_b0_xresvsmodphi);
  plots->Add(m_sct_b1_xresvsmodphi);
  plots->Add(m_sct_b2_xresvsmodphi);
  plots->Add(m_sct_b3_xresvsmodphi);
  TH1F* m_sct_b_xresvsmodphi = combineHistos("sct_b_xresvsmodphi","X Residual Mean vs (Modified) Phi-ID SCT Barrel",plots,10);
  m_sct_b_xresvsmodphi->GetYaxis()->SetTitle("Mean Residual X [mm]");
  m_sct_b_xresvsmodphi->GetXaxis()->SetTitle("(Modified) Phi-ID");
  plots->Clear();

  //x residual width as a function of stave in the SCT barrel
  TH1F* m_sct_b0_xresvsmodphi_width = new TH1F("sct_b0_xresvsmodphi_width","X Residual Width vs Phi-ID SCT Barrel L0",32,-0.5,31.5);
  m_pix_b0_xresvsmodphi_width->GetYaxis()->SetTitle("Width Residual X [mm]");
  m_pix_b0_xresvsmodphi_width->GetXaxis()->SetTitle("Module Phi-ID");
  TH1F* m_sct_b1_xresvsmodphi_width = new TH1F("sct_b1_xresvsmodphi_width","X Residual Width vs Phi-ID SCT Barrel L1",40,-0.5,39.5);
  m_sct_b1_xresvsmodphi_width->GetYaxis()->SetTitle("Width Residual X [mm]");
  m_sct_b1_xresvsmodphi_width->GetXaxis()->SetTitle("Module Phi-ID");
  TH1F* m_sct_b2_xresvsmodphi_width = new TH1F("sct_b2_xresvsmodphi_width","X Residual Width vs Phi-ID SCT Barrel L2",48,-0.5,47.5);
  m_sct_b2_xresvsmodphi_width->GetYaxis()->SetTitle("Width Residual X [mm]");
  m_sct_b2_xresvsmodphi_width->GetXaxis()->SetTitle("Module Phi-ID");
  TH1F* m_sct_b3_xresvsmodphi_width = new TH1F("sct_b3_xresvsmodphi_width","X Residual Width vs Phi-ID SCT Barrel L3",56,-0.5,55.5);
  m_sct_b3_xresvsmodphi_width->GetYaxis()->SetTitle("Width Residual X [mm]");
  m_sct_b3_xresvsmodphi_width->GetXaxis()->SetTitle("Module Phi-ID");
  meanRMSProjections3D_XY(m_sct_b0_xresvsmodetaphi_3d,m_sct_b0_xresvsmodphi_width,1,3);
  meanRMSProjections3D_XY(m_sct_b1_xresvsmodetaphi_3d,m_sct_b1_xresvsmodphi_width,1,3);
  meanRMSProjections3D_XY(m_sct_b2_xresvsmodetaphi_3d,m_sct_b2_xresvsmodphi_width,1,3);
  meanRMSProjections3D_XY(m_sct_b3_xresvsmodetaphi_3d,m_sct_b3_xresvsmodphi_width,1,3);
  plots->Add(m_sct_b0_xresvsmodphi_width);
  plots->Add(m_sct_b1_xresvsmodphi_width);
  plots->Add(m_sct_b2_xresvsmodphi_width);
  plots->Add(m_sct_b3_xresvsmodphi_width);
  TH1F* m_sct_b_xresvsmodphi_width = combineHistos("sct_b_xresvsmodphi_width","X Residual Width vs (Modified) Phi-ID SCT Barrel",plots,10);
  m_sct_b_xresvsmodphi_width->GetYaxis()->SetTitle("Width Residual X [mm]");
  m_sct_b_xresvsmodphi_width->GetXaxis()->SetTitle("(Modified) Phi-ID");
  plots->Clear();

  //residual mean as function of module eta/phi in Silicon endcaps
  TH1F* m_pix_eca_xresvsmodphi = new TH1F("pix_eca_xresvsmodphi","X Residual Mean vs (Modified) Module Phi Pixel Endcap A",165,0,165);
  m_pix_eca_xresvsmodphi->GetYaxis()->SetTitle("Mean Residual X [mm]");
  m_pix_eca_xresvsmodphi->GetXaxis()->SetTitle("(Modified) Module Phi-ID");
  meanRMSProjections2D(m_pix_eca_xresvsmodphi_2d,m_pix_eca_xresvsmodphi,2);
  TH1F* m_pix_eca_yresvsmodphi = new TH1F("pix_eca_yresvsmodphi","Y Residual Mean vs (Modified) Module Phi Pixel Endcap A",165,0,165);
  m_pix_eca_yresvsmodphi->GetYaxis()->SetTitle("Mean Residual Y [mm]");
  m_pix_eca_yresvsmodphi->GetXaxis()->SetTitle("(Modified) Module Phi-ID");
  meanRMSProjections2D(m_pix_eca_yresvsmodphi_2d,m_pix_eca_yresvsmodphi,2);
  TH1F* m_pix_ecc_xresvsmodphi = new TH1F("pix_ecc_xresvsmodphi","X Residual Mean vs (Modified) Module Phi Pixel Endcap C",165,0,165);
  m_pix_ecc_xresvsmodphi->GetYaxis()->SetTitle("Mean Residual X [mm]");
  m_pix_ecc_xresvsmodphi->GetXaxis()->SetTitle("(Modified) Module Phi-ID");    
  meanRMSProjections2D(m_pix_ecc_xresvsmodphi_2d,m_pix_ecc_xresvsmodphi,2);
  TH1F* m_pix_ecc_yresvsmodphi = new TH1F("pix_ecc_yresvsmodphi","Y Residual Mean vs (Modified) Module Phi Pixel Endcap C",165,0,165);
  m_pix_ecc_yresvsmodphi->GetYaxis()->SetTitle("Mean Residual Y [mm]");
  m_pix_ecc_yresvsmodphi->GetXaxis()->SetTitle("(Modified) Module Phi-ID");
  meanRMSProjections2D(m_pix_ecc_yresvsmodphi_2d,m_pix_ecc_yresvsmodphi,2);
  TH1F* m_sct_eca_xresvsmodphi = new TH1F("sct_eca_xresvsmodphi","X Residual Mean vs (Modified) Module Phi SCT Endcap A",558,0,558);
  m_sct_eca_xresvsmodphi->GetYaxis()->SetTitle("Mean Residual X [mm]");
  m_sct_eca_xresvsmodphi->GetXaxis()->SetTitle("(Modified) Module Phi-ID");
  meanRMSProjections2D(m_sct_eca_xresvsmodphi_2d,m_sct_eca_xresvsmodphi,2);
  TH1F* m_sct_ecc_xresvsmodphi = new TH1F("sct_ecc_xresvsmodphi","X Residual Mean vs (Modified) Module Phi SCT Endcap C",558,0,558);
  m_sct_ecc_xresvsmodphi->GetYaxis()->SetTitle("Mean Residual X [mm]");
  m_sct_ecc_xresvsmodphi->GetXaxis()->SetTitle("(Modified) Module Phi-ID");
  meanRMSProjections2D(m_sct_ecc_xresvsmodphi_2d,m_sct_ecc_xresvsmodphi,2);


  //2-d residual map histograms - pixel barrel residual mean
  TH2F* m_pix_b0_xresvsmodetaphi_mean = new TH2F("pix_b0_xresvsmodetaphi_mean","X Residual Mean vs Module Eta-Phi-ID Pixel Barrel L0",20,-10.5,9.5,14,0,14);
  m_pix_b0_xresvsmodetaphi_mean->GetXaxis()->SetTitle("Module Eta-ID");
  m_pix_b0_xresvsmodetaphi_mean->GetYaxis()->SetTitle("Module Phi-ID");
  TH2F* m_pix_b1_xresvsmodetaphi_mean = new TH2F("pix_b1_xresvsmodetaphi_mean","X Residual Mean vs Module Eta-Phi-ID Pixel Barrel L1",13,-6.5,6.5,22,-0.5,21.5);
  m_pix_b1_xresvsmodetaphi_mean->GetXaxis()->SetTitle("Module Eta-ID");
  m_pix_b1_xresvsmodetaphi_mean->GetYaxis()->SetTitle("Module Phi-ID");
  TH2F* m_pix_b2_xresvsmodetaphi_mean = new TH2F("pix_b2_xresvsmodetaphi_mean","X Residual Mean vs Module Eta-Phi-ID Pixel Barrel L2",13,-6.5,6.5,38,-0.5,37.5);
  m_pix_b2_xresvsmodetaphi_mean->GetXaxis()->SetTitle("Module Eta-ID");
  m_pix_b2_xresvsmodetaphi_mean->GetYaxis()->SetTitle("Module Phi-ID");
  TH2F* m_pix_b3_xresvsmodetaphi_mean = new TH2F("pix_b3_xresvsmodetaphi_mean","X Residual Mean vs Module Eta-Phi-ID Pixel Barrel L3",13,-6.5,6.5,52,-0.5,51.5);
  m_pix_b3_xresvsmodetaphi_mean->GetXaxis()->SetTitle("Module Eta-ID");
  m_pix_b3_xresvsmodetaphi_mean->GetYaxis()->SetTitle("Module Phi-ID");
  


  TH2F* m_pix_b0_yresvsmodetaphi_mean = new TH2F("pix_b0_yresvsmodetaphi_mean","Y Residual Mean vs Module Eta-Phi-ID Pixel Barrel L0",20,-10.5,9.5,14,0,14);
  m_pix_b0_yresvsmodetaphi_mean->GetXaxis()->SetTitle("Module Eta-ID");
  m_pix_b0_yresvsmodetaphi_mean->GetYaxis()->SetTitle("Module Phi-ID");
  TH2F* m_pix_b1_yresvsmodetaphi_mean = new TH2F("pix_b1_yresvsmodetaphi_mean","Y Residual Mean vs Module Eta-Phi-ID Pixel Barrel L1",13,-6.5,6.5,22,-0.5,21.5);
  m_pix_b1_yresvsmodetaphi_mean->GetXaxis()->SetTitle("Module Eta-ID");
  m_pix_b1_yresvsmodetaphi_mean->GetYaxis()->SetTitle("Module Phi-ID");
  TH2F* m_pix_b2_yresvsmodetaphi_mean = new TH2F("pix_b2_yresvsmodetaphi_mean","Y Residual Mean vs Module Eta-Phi-ID Pixel Barrel L2",13,-6.5,6.5,38,-0.5,37.5);
  m_pix_b2_yresvsmodetaphi_mean->GetXaxis()->SetTitle("Module Eta-ID");
  m_pix_b2_yresvsmodetaphi_mean->GetYaxis()->SetTitle("Module Phi-ID");
  TH2F* m_pix_b3_yresvsmodetaphi_mean = new TH2F("pix_b3_yresvsmodetaphi_mean","Y Residual Mean vs Module Eta-Phi-ID Pixel Barrel L3",13,-6.5,6.5,52,-0.5,51.5);
  m_pix_b3_yresvsmodetaphi_mean->GetXaxis()->SetTitle("Module Eta-ID");
  m_pix_b3_yresvsmodetaphi_mean->GetYaxis()->SetTitle("Module Phi-ID");
  meanRMSProjections3D(m_pix_b0_xresvsmodetaphi_3d,m_pix_b0_xresvsmodetaphi_mean,2);
  meanRMSProjections3D(m_pix_b1_xresvsmodetaphi_3d,m_pix_b1_xresvsmodetaphi_mean,2);
  meanRMSProjections3D(m_pix_b2_xresvsmodetaphi_3d,m_pix_b2_xresvsmodetaphi_mean,2);
  meanRMSProjections3D(m_pix_b3_xresvsmodetaphi_3d,m_pix_b3_xresvsmodetaphi_mean,2);
  meanRMSProjections3D(m_pix_b0_yresvsmodetaphi_3d,m_pix_b0_yresvsmodetaphi_mean,2);
  meanRMSProjections3D(m_pix_b1_yresvsmodetaphi_3d,m_pix_b1_yresvsmodetaphi_mean,2);
  meanRMSProjections3D(m_pix_b2_yresvsmodetaphi_3d,m_pix_b2_yresvsmodetaphi_mean,2);
  meanRMSProjections3D(m_pix_b3_yresvsmodetaphi_3d,m_pix_b3_yresvsmodetaphi_mean,2);

  //2-d residual map histograms - pixel barrel residual width
  TH2F* m_pix_b0_xresvsmodetaphi_rms = new TH2F("pix_b0_xresvsmodetaphi_rms","X Residual Width vs Module Eta-Phi-ID Pixel Barrel L0",20,-10.5,9.5,14,0,14);
  m_pix_b0_xresvsmodetaphi_rms->GetXaxis()->SetTitle("Module Eta-ID");
  m_pix_b0_xresvsmodetaphi_rms->GetYaxis()->SetTitle("Module Phi-ID");
  TH2F* m_pix_b1_xresvsmodetaphi_rms = new TH2F("pix_b1_xresvsmodetaphi_rms","X Residual Width vs Module Eta-Phi-ID Pixel Barrel L1",13,-6.5,6.5,22,-0.5,21.5);
  m_pix_b1_xresvsmodetaphi_rms->GetXaxis()->SetTitle("Module Eta-ID");
  m_pix_b1_xresvsmodetaphi_rms->GetYaxis()->SetTitle("Module Phi-ID");
  TH2F* m_pix_b2_xresvsmodetaphi_rms = new TH2F("pix_b2_xresvsmodetaphi_rms","X Residual Width vs Module Eta-Phi-ID Pixel Barrel L2",13,-6.5,6.5,38,-0.5,37.5);
  m_pix_b2_xresvsmodetaphi_rms->GetXaxis()->SetTitle("Module Eta-ID");
  m_pix_b2_xresvsmodetaphi_rms->GetYaxis()->SetTitle("Module Phi-ID");
  TH2F* m_pix_b3_xresvsmodetaphi_rms = new TH2F("pix_b3_xresvsmodetaphi_rms","X Residual Width vs Module Eta-Phi-ID Pixel Barrel L3",13,-6.5,6.5,52,-0.5,51.5);
  m_pix_b3_xresvsmodetaphi_rms->GetXaxis()->SetTitle("Module Eta-ID");
  m_pix_b3_xresvsmodetaphi_rms->GetYaxis()->SetTitle("Module Phi-ID");
  TH2F* m_pix_b0_yresvsmodetaphi_rms = new TH2F("pix_b0_yresvsmodetaphi_rms","Y Residual Width vs Module Eta-Phi-ID Pixel Barrel L0",20,-10.5,9.5,14,0,14);
  m_pix_b0_yresvsmodetaphi_rms->GetXaxis()->SetTitle("Module Eta-ID");
  m_pix_b0_yresvsmodetaphi_rms->GetYaxis()->SetTitle("Module Phi-ID");
  TH2F* m_pix_b1_yresvsmodetaphi_rms = new TH2F("pix_b1_yresvsmodetaphi_rms","Y Residual Width vs Module Eta-Phi-ID Pixel Barrel L1",13,-6.5,6.5,22,-0.5,21.5);
  m_pix_b1_yresvsmodetaphi_rms->GetXaxis()->SetTitle("Module Eta-ID");
  m_pix_b1_yresvsmodetaphi_rms->GetYaxis()->SetTitle("Module Phi-ID");
  TH2F* m_pix_b2_yresvsmodetaphi_rms = new TH2F("pix_b2_yresvsmodetaphi_rms","Y Residual Width vs Module Eta-Phi-ID Pixel Barrel L2",13,-6.5,6.5,38,-0.5,37.5);
  m_pix_b2_yresvsmodetaphi_rms->GetXaxis()->SetTitle("Module Eta-ID");
  m_pix_b2_yresvsmodetaphi_rms->GetYaxis()->SetTitle("Module Phi-ID");
  TH2F* m_pix_b3_yresvsmodetaphi_rms = new TH2F("pix_b3_yresvsmodetaphi_rms","Y Residual Width vs Module Eta-Phi-ID Pixel Barrel L3",13,-6.5,6.5,52,-0.5,51.5);
  m_pix_b3_yresvsmodetaphi_rms->GetXaxis()->SetTitle("Module Eta-ID");
  m_pix_b3_yresvsmodetaphi_rms->GetYaxis()->SetTitle("Module Phi-ID");
  meanRMSProjections3D(m_pix_b0_xresvsmodetaphi_3d,m_pix_b0_xresvsmodetaphi_rms,3);
  meanRMSProjections3D(m_pix_b1_xresvsmodetaphi_3d,m_pix_b1_xresvsmodetaphi_rms,3);
  meanRMSProjections3D(m_pix_b2_xresvsmodetaphi_3d,m_pix_b2_xresvsmodetaphi_rms,3);
  meanRMSProjections3D(m_pix_b3_xresvsmodetaphi_3d,m_pix_b3_xresvsmodetaphi_rms,3);
  meanRMSProjections3D(m_pix_b0_yresvsmodetaphi_3d,m_pix_b0_yresvsmodetaphi_rms,3);
  meanRMSProjections3D(m_pix_b1_yresvsmodetaphi_3d,m_pix_b1_yresvsmodetaphi_rms,3);
  meanRMSProjections3D(m_pix_b2_yresvsmodetaphi_3d,m_pix_b2_yresvsmodetaphi_rms,3);
  meanRMSProjections3D(m_pix_b3_yresvsmodetaphi_3d,m_pix_b3_xresvsmodetaphi_rms,3);

  //2-d residual map histograms - SCT barrel residual means
  TH2F* m_sct_b0_xresvsmodetaphi_mean = new TH2F("sct_b0_xresvsmodetaphi_mean","X Residual Mean vs Module Eta-Phi-ID SCT Barrel L0",13,-6.5,6.5,32,-0.5,31.5);
  m_sct_b0_xresvsmodetaphi_mean->GetXaxis()->SetTitle("Module Eta-ID");
  m_sct_b0_xresvsmodetaphi_mean->GetYaxis()->SetTitle("Module Phi-ID");
  TH2F* m_sct_b1_xresvsmodetaphi_mean = new TH2F("sct_b1_xresvsmodetaphi_mean","X Residual Mean vs Module Eta-Phi-ID SCT Barrel L1",13,-6.5,6.5,40,-0.5,39.5);
  m_sct_b1_xresvsmodetaphi_mean->GetXaxis()->SetTitle("Module Eta-ID");
  m_sct_b1_xresvsmodetaphi_mean->GetYaxis()->SetTitle("Module Phi-ID");
  TH2F* m_sct_b2_xresvsmodetaphi_mean = new TH2F("sct_b2_xresvsmodetaphi_mean","X Residual Mean vs Module Eta-Phi-ID SCT Barrel L2",13,-6.5,6.5,48,-0.5,47.5);
  m_sct_b2_xresvsmodetaphi_mean->GetXaxis()->SetTitle("Module Eta-ID");
  m_sct_b2_xresvsmodetaphi_mean->GetYaxis()->SetTitle("Module Phi-ID");
  TH2F* m_sct_b3_xresvsmodetaphi_mean = new TH2F("sct_b3_xresvsmodetaphi_mean","X Residual Mean vs Module Eta-Phi-ID SCT Barrel L3",13,-6.5,6.5,56,-0.5,55.5);
  m_sct_b3_xresvsmodetaphi_mean->GetXaxis()->SetTitle("Module Eta-ID");
  m_sct_b3_xresvsmodetaphi_mean->GetYaxis()->SetTitle("Module Phi-ID");
  meanRMSProjections3D(m_sct_b0_xresvsmodetaphi_3d,m_sct_b0_xresvsmodetaphi_mean,2);
  meanRMSProjections3D(m_sct_b1_xresvsmodetaphi_3d,m_sct_b1_xresvsmodetaphi_mean,2);
  meanRMSProjections3D(m_sct_b2_xresvsmodetaphi_3d,m_sct_b2_xresvsmodetaphi_mean,2);
  meanRMSProjections3D(m_sct_b3_xresvsmodetaphi_3d,m_sct_b3_xresvsmodetaphi_mean,2);

  //2-d residual map histograms - SCT barrel residual widths
  TH2F* m_sct_b0_xresvsmodetaphi_rms = new TH2F("sct_b0_xresvsmodetaphi_rms","X Residual Width vs Module Eta-Phi-ID SCT Barrel L0",13,-6.5,6.5,32,-0.5,31.5);
  m_sct_b0_xresvsmodetaphi_rms->GetXaxis()->SetTitle("Module Eta-ID");
  m_sct_b0_xresvsmodetaphi_rms->GetYaxis()->SetTitle("Module Phi-ID");
  TH2F* m_sct_b1_xresvsmodetaphi_rms = new TH2F("sct_b1_xresvsmodetaphi_rms","X Residual Width vs Module Eta-Phi-ID SCT Barrel L1",13,-6.5,6.5,40,-0.5,39.5);
  m_sct_b1_xresvsmodetaphi_rms->GetXaxis()->SetTitle("Module Eta-ID");
  m_sct_b1_xresvsmodetaphi_rms->GetYaxis()->SetTitle("Module Phi-ID");
  TH2F* m_sct_b2_xresvsmodetaphi_rms = new TH2F("sct_b2_xresvsmodetaphi_rms","X Residual Width vs Module Eta-Phi-ID SCT Barrel L2",13,-6.5,6.5,48,-0.5,47.5);
  m_sct_b2_xresvsmodetaphi_rms->GetXaxis()->SetTitle("Module Eta-ID");
  m_sct_b2_xresvsmodetaphi_rms->GetYaxis()->SetTitle("Module Phi-ID");
  TH2F* m_sct_b3_xresvsmodetaphi_rms = new TH2F("sct_b3_xresvsmodetaphi_rms","X Residual Width vs Module Eta-Phi-ID SCT Barrel L3",13,-6.5,6.5,56,-0.5,55.5);
  m_sct_b3_xresvsmodetaphi_rms->GetXaxis()->SetTitle("Module Eta-ID");
  m_sct_b3_xresvsmodetaphi_rms->GetYaxis()->SetTitle("Module Phi-ID");
  meanRMSProjections3D(m_sct_b0_xresvsmodetaphi_3d,m_sct_b0_xresvsmodetaphi_rms,3);
  meanRMSProjections3D(m_sct_b1_xresvsmodetaphi_3d,m_sct_b1_xresvsmodetaphi_rms,3);
  meanRMSProjections3D(m_sct_b2_xresvsmodetaphi_3d,m_sct_b2_xresvsmodetaphi_rms,3);
  meanRMSProjections3D(m_sct_b3_xresvsmodetaphi_3d,m_sct_b3_xresvsmodetaphi_rms,3);

  //histograms showing the distribution of Gaussian fitted residual means for pixel and SCT barrel (should add endcaps)
  //modules are required to have > 30 hits
  TH1F* m_pix_b0_residualmeans = new TH1F("pix_b0_residualmeans","Pixel Barrel Layer 0 Mean of Residual Distributions",100,-0.2,0.2);
  m_pix_b0_residualmeans->GetXaxis()->SetTitle("Fitted Residual Mean [mm]");
  m_pix_b0_residualmeans->GetYaxis()->SetTitle("Number of Modules/4#mum");
  TH1F* m_pix_b1_residualmeans = new TH1F("pix_b1_residualmeans","Pixel Barrel Layer 1 Mean of Residual Distributions",100,-0.2,0.2);
  m_pix_b1_residualmeans->GetXaxis()->SetTitle("Fitted Residual Mean [mm]");
  m_pix_b1_residualmeans->GetYaxis()->SetTitle("Number of Modules/4#mum");
  TH1F* m_pix_b2_residualmeans = new TH1F("pix_b2_residualmeans","Pixel Barrel Layer 2 Mean of Residual Distributions",100,-0.2,0.2);
  m_pix_b2_residualmeans->GetXaxis()->SetTitle("Fitted Residual Mean [mm]");
  m_pix_b2_residualmeans->GetYaxis()->SetTitle("Number of Modules/4#mum");
  TH1F* m_pix_b3_residualmeans = new TH1F("pix_b3_residualmeans","Pixel Barrel Layer 3 Mean of Residual Distributions",100,-0.2,0.2);
  m_pix_b3_residualmeans->GetXaxis()->SetTitle("Fitted Residual Mean [mm]");
  m_pix_b3_residualmeans->GetYaxis()->SetTitle("Number of Modules/4#mum");
  meanRMSProjections3D_DMRPlot(m_pix_b0_xresvsmodetaphi_3d,m_pix_b0_residualmeans,2);
  meanRMSProjections3D_DMRPlot(m_pix_b1_xresvsmodetaphi_3d,m_pix_b1_residualmeans,2);
  meanRMSProjections3D_DMRPlot(m_pix_b2_xresvsmodetaphi_3d,m_pix_b2_residualmeans,2);
  meanRMSProjections3D_DMRPlot(m_pix_b3_xresvsmodetaphi_3d,m_pix_b3_residualmeans,2);
  

  TH1F* m_pix_b_residualmeans = new TH1F("pix_b_residualmeans","Pixel Barrel Mean of Residual Distributions",100,-0.2,0.2);
  m_pix_b_residualmeans->GetXaxis()->SetTitle("Fitted Residual Mean [mm]");
  m_pix_b_residualmeans->GetYaxis()->SetTitle("Number of Modules/4#mum");
  m_pix_b_residualmeans->Add(m_pix_b0_residualmeans);
  m_pix_b_residualmeans->Add(m_pix_b1_residualmeans);
  m_pix_b_residualmeans->Add(m_pix_b2_residualmeans);
  m_pix_b_residualmeans->Add(m_pix_b3_residualmeans);

  TH1F* m_sct_b0_residualmeans = new TH1F("sct_b0_residualmeans","SCT Barrel Layer 0 Mean of Residual Distributions",100,-0.2,0.2);
  m_sct_b0_residualmeans->GetXaxis()->SetTitle("Fitted Residual Mean [mm]");
  m_sct_b0_residualmeans->GetYaxis()->SetTitle("Number of Modules/4#mum");
  TH1F* m_sct_b1_residualmeans = new TH1F("sct_b1_residualmeans","SCT Barrel Layer 1 Mean of Residual Distributions",100,-0.2,0.2);
  m_sct_b1_residualmeans->GetXaxis()->SetTitle("Fitted Residual Mean [mm]");
  m_sct_b1_residualmeans->GetYaxis()->SetTitle("Number of Modules/4#mum");
  TH1F* m_sct_b2_residualmeans = new TH1F("sct_b2_residualmeans","SCT Barrel Layer 2 Mean of Residual Distributions",100,-0.2,0.2);
  m_sct_b2_residualmeans->GetXaxis()->SetTitle("Fitted Residual Mean [mm]");
  m_sct_b2_residualmeans->GetYaxis()->SetTitle("Number of Modules/4#mum");
  TH1F* m_sct_b3_residualmeans = new TH1F("sct_b3_residualmeans","SCT Barrel Layer 3 Mean of Residual Distributions",100,-0.2,0.2);
  m_sct_b3_residualmeans->GetXaxis()->SetTitle("Fitted Residual Mean [mm]");
  m_sct_b3_residualmeans->GetYaxis()->SetTitle("Number of Modules/4#mum");
  meanRMSProjections3D_DMRPlot(m_sct_b0_xresvsmodetaphi_3d,m_sct_b0_residualmeans,2);
  meanRMSProjections3D_DMRPlot(m_sct_b1_xresvsmodetaphi_3d,m_sct_b1_residualmeans,2);
  meanRMSProjections3D_DMRPlot(m_sct_b2_xresvsmodetaphi_3d,m_sct_b2_residualmeans,2);
  meanRMSProjections3D_DMRPlot(m_sct_b3_xresvsmodetaphi_3d,m_sct_b3_residualmeans,2);

  TH1F* m_sct_b_residualmeans = new TH1F("sct_b_residualmeans","SCT Barrel Mean of Residual Distributions",100,-0.2,0.2);
  m_sct_b_residualmeans->GetXaxis()->SetTitle("Fitted Residual Mean [mm]");
  m_sct_b_residualmeans->GetYaxis()->SetTitle("Number of Modules/4#mum");
  m_sct_b_residualmeans->Add(m_sct_b0_residualmeans);
  m_sct_b_residualmeans->Add(m_sct_b1_residualmeans);
  m_sct_b_residualmeans->Add(m_sct_b2_residualmeans);
  m_sct_b_residualmeans->Add(m_sct_b3_residualmeans);


  //histograms showing the distribution of the errors on the Gaussian-fitted residual means for Pixel and SCT
  //i.e. the error on the residual mean entries for the plots above - this should not be larger than the bin size!
  //modules are required to have > 30 hits
  TH1F* m_pix_b0_residualfiterrors = new TH1F("pix_b0_residualfiterrors","Pixel Barrel Layer 0 Residual Gaussian Fit Error",200,0.0,0.02);
  m_pix_b0_residualfiterrors->GetXaxis()->SetTitle("Residual Gaus. Fit Error [mm]");
  m_pix_b0_residualfiterrors->GetYaxis()->SetTitle("Number of Modules");
  TH1F* m_pix_b1_residualfiterrors = new TH1F("pix_b1_residualfiterrors","Pixel Barrel Layer 1 Residual Gaussian Fit Error",200,0.0,0.02);
  m_pix_b1_residualfiterrors->GetXaxis()->SetTitle("Residual Gaus. Fit Error [mm]");
  m_pix_b1_residualfiterrors->GetYaxis()->SetTitle("Number of Modules");
  TH1F* m_pix_b2_residualfiterrors = new TH1F("pix_b2_residualfiterrors","Pixel Barrel Layer 2 Residual Gaussian Fit Error",200,0.0,0.02);
  m_pix_b2_residualfiterrors->GetXaxis()->SetTitle("Residual Gaus. Fit Error [mm]");
  m_pix_b2_residualfiterrors->GetYaxis()->SetTitle("Number of Modules");
  TH1F* m_pix_b3_residualfiterrors = new TH1F("pix_b3_residualfiterrors","Pixel Barrel Layer 3 Residual Gaussian Fit Error",200,0.0,0.02);
  m_pix_b3_residualfiterrors->GetXaxis()->SetTitle("Residual Gaus. Fit Error [mm]");
  m_pix_b3_residualfiterrors->GetYaxis()->SetTitle("Number of Modules");
  meanRMSProjections3D_DMRPlot(m_pix_b0_xresvsmodetaphi_3d,m_pix_b0_residualfiterrors,5);
  meanRMSProjections3D_DMRPlot(m_pix_b1_xresvsmodetaphi_3d,m_pix_b1_residualfiterrors,5);
  meanRMSProjections3D_DMRPlot(m_pix_b2_xresvsmodetaphi_3d,m_pix_b2_residualfiterrors,5);
  meanRMSProjections3D_DMRPlot(m_pix_b3_xresvsmodetaphi_3d,m_pix_b3_residualfiterrors,5);

  TH1F* m_pix_b_residualfiterrors = new TH1F("pix_b_residualfiterrors","Pixel Barrel Residual Gaussian Fit Error",200,-0.0,0.02);
  m_pix_b_residualfiterrors->GetXaxis()->SetTitle("Residual Gaus. Fit Error [mm]");
  m_pix_b_residualfiterrors->GetYaxis()->SetTitle("Number of Modules");
  m_pix_b_residualfiterrors->Add(m_pix_b0_residualfiterrors);
  m_pix_b_residualfiterrors->Add(m_pix_b1_residualfiterrors);
  m_pix_b_residualfiterrors->Add(m_pix_b2_residualfiterrors);
  m_pix_b_residualfiterrors->Add(m_pix_b3_residualfiterrors);
  


  TH1F* m_sct_b0_residualfiterrors = new TH1F("sct_b0_residualfiterrors","SCT Barrel Layer 0 Residual Gaussian Fit Error",200,0.0,0.02);
  m_sct_b0_residualfiterrors->GetXaxis()->SetTitle("Residual Gaus. Fit Error [mm]");
  m_sct_b0_residualfiterrors->GetYaxis()->SetTitle("Number of Modules");
  TH1F* m_sct_b1_residualfiterrors = new TH1F("sct_b1_residualfiterrors","SCT Barrel Layer 1 Residual Gaussian Fit Error",200,0.0,0.02);
  m_sct_b1_residualfiterrors->GetXaxis()->SetTitle("Residual Gaus. Fit Error [mm]");
  m_sct_b1_residualfiterrors->GetYaxis()->SetTitle("Number of Modules");
  TH1F* m_sct_b2_residualfiterrors = new TH1F("sct_b2_residualfiterrors","SCT Barrel Layer 2 Residual Gaussian Fit Error",200,0.0,0.02);
  m_sct_b2_residualfiterrors->GetXaxis()->SetTitle("Residual Gaus. Fit Error [mm]");
  m_sct_b2_residualfiterrors->GetYaxis()->SetTitle("Number of Modules");
  TH1F* m_sct_b3_residualfiterrors = new TH1F("sct_b3_residualfiterrors","SCT Barrel Layer 3 Residual Gaussian Fit Error",200,0.0,0.02);
  m_sct_b3_residualfiterrors->GetXaxis()->SetTitle("Residual Gaus. Fit Error [mm]");
  m_sct_b3_residualfiterrors->GetYaxis()->SetTitle("Number of Modules");
  meanRMSProjections3D_DMRPlot(m_sct_b0_xresvsmodetaphi_3d,m_sct_b0_residualfiterrors,5);
  meanRMSProjections3D_DMRPlot(m_sct_b1_xresvsmodetaphi_3d,m_sct_b1_residualfiterrors,5);
  meanRMSProjections3D_DMRPlot(m_sct_b2_xresvsmodetaphi_3d,m_sct_b2_residualfiterrors,5);
  meanRMSProjections3D_DMRPlot(m_sct_b3_xresvsmodetaphi_3d,m_sct_b3_residualfiterrors,5);

  TH1F* m_sct_b_residualfiterrors = new TH1F("sct_b_residualfiterrors","SCT Barrel Residual Gaussian Fit Error",200,-0.0,0.02);
  m_sct_b_residualfiterrors->GetXaxis()->SetTitle("Residual Gaus. Fit Error [mm]");
  m_sct_b_residualfiterrors->GetYaxis()->SetTitle("Number of Modules");
  m_sct_b_residualfiterrors->Add(m_sct_b0_residualfiterrors);
  m_sct_b_residualfiterrors->Add(m_sct_b1_residualfiterrors);
  m_sct_b_residualfiterrors->Add(m_sct_b2_residualfiterrors);
  m_sct_b_residualfiterrors->Add(m_sct_b3_residualfiterrors);


  // median suffers from 10micron bin size in residual distributions - means that it can only take discreet values at 5 micron intervals
  TH1F* m_pix_b0_residualmedians = new TH1F("pix_b0_residualmedians","Pixel Barrel Layer 0 Median of Residual Distributions",20,-0.1,0.1);
  m_pix_b0_residualmedians->GetXaxis()->SetTitle("Residual Median [mm]");
  m_pix_b0_residualmedians->GetYaxis()->SetTitle("Number of Modules/10#mum");
  TH1F* m_pix_b1_residualmedians = new TH1F("pix_b1_residualmedians","Pixel Barrel Layer 1 Median of Residual Distributions",20,-0.1,0.1);
  m_pix_b1_residualmedians->GetXaxis()->SetTitle("Residual Median [mm]");
  m_pix_b1_residualmedians->GetYaxis()->SetTitle("Number of Modules/10#mum");
  TH1F* m_pix_b2_residualmedians = new TH1F("pix_b2_residualmedians","Pixel Barrel Layer 2 Median of Residual Distributions",20,-0.1,0.1);
  m_pix_b2_residualmedians->GetXaxis()->SetTitle("Residual Median [mm]");
  m_pix_b2_residualmedians->GetYaxis()->SetTitle("Number of Modules/10#mum");
  TH1F* m_pix_b3_residualmedians = new TH1F("pix_b3_residualmedians","Pixel Barrel Layer 3 Median of Residual Distributions",20,-0.1,0.1);
  m_pix_b3_residualmedians->GetXaxis()->SetTitle("Residual Median [mm]");
  m_pix_b3_residualmedians->GetYaxis()->SetTitle("Number of Modules/10#mum");
  meanRMSProjections3D_DMRPlot(m_pix_b0_xresvsmodetaphi_3d,m_pix_b0_residualmedians,6);
  meanRMSProjections3D_DMRPlot(m_pix_b1_xresvsmodetaphi_3d,m_pix_b1_residualmedians,6);
  meanRMSProjections3D_DMRPlot(m_pix_b2_xresvsmodetaphi_3d,m_pix_b2_residualmedians,6);
  meanRMSProjections3D_DMRPlot(m_pix_b3_xresvsmodetaphi_3d,m_pix_b3_residualmedians,6);

  TH1F* m_pix_b_residualmedians = new TH1F("pix_b_residualmedians","Pixel Barrel Median of Residual Distributions",20,-0.1,0.1);
  m_pix_b_residualmedians->GetXaxis()->SetTitle("Residual Median [mm]");
  m_pix_b_residualmedians->GetYaxis()->SetTitle("Number of Modules/10#mum");
  m_pix_b_residualmedians->Add(m_pix_b0_residualmedians);
  m_pix_b_residualmedians->Add(m_pix_b1_residualmedians);
  m_pix_b_residualmedians->Add(m_pix_b2_residualmedians);
  m_pix_b_residualmedians->Add(m_pix_b3_residualmedians);
  


  TH1F* m_sct_b0_residualmedians = new TH1F("sct_b0_residualmedians","SCT Barrel Layer 0 Median of Residual Distributions",20,-0.1,0.1);
  m_sct_b0_residualmedians->GetXaxis()->SetTitle("Residual Median [mm]");
  m_sct_b0_residualmedians->GetYaxis()->SetTitle("Number of Modules/10#mum");
  TH1F* m_sct_b1_residualmedians = new TH1F("sct_b1_residualmedians","SCT Barrel Layer 1 Median of Residual Distributions",20,-0.1,0.1);
  m_sct_b1_residualmedians->GetXaxis()->SetTitle("Residual Median [mm]");
  m_sct_b1_residualmedians->GetYaxis()->SetTitle("Number of Modules/10#mum");
  TH1F* m_sct_b2_residualmedians = new TH1F("sct_b2_residualmedians","SCT Barrel Layer 2 Median of Residual Distributions",20,-0.1,0.1);
  m_sct_b2_residualmedians->GetXaxis()->SetTitle("Residual Median [mm]");
  m_sct_b2_residualmedians->GetYaxis()->SetTitle("Number of Modules/10#mum");
  TH1F* m_sct_b3_residualmedians = new TH1F("sct_b3_residualmedians","SCT Barrel Layer 3 Median of Residual Distributions",20,-0.1,0.1);
  m_sct_b3_residualmedians->GetXaxis()->SetTitle("Residual Median [mm]");
  m_sct_b3_residualmedians->GetYaxis()->SetTitle("Number of Modules/10#mum");
  meanRMSProjections3D_DMRPlot(m_sct_b0_xresvsmodetaphi_3d,m_sct_b0_residualmedians,6);
  meanRMSProjections3D_DMRPlot(m_sct_b1_xresvsmodetaphi_3d,m_sct_b1_residualmedians,6);
  meanRMSProjections3D_DMRPlot(m_sct_b2_xresvsmodetaphi_3d,m_sct_b2_residualmedians,6);
  meanRMSProjections3D_DMRPlot(m_sct_b3_xresvsmodetaphi_3d,m_sct_b3_residualmedians,6);

  TH1F* m_sct_b_residualmedians = new TH1F("sct_b_residualmedians","SCT Barrel Median of Residual Distributions",20,-0.1,0.1);
  m_sct_b_residualmedians->GetXaxis()->SetTitle("Residual Median [mm]");
  m_sct_b_residualmedians->GetYaxis()->SetTitle("Number of Modules/10#mum");
  m_sct_b_residualmedians->Add(m_sct_b0_residualmedians);
  m_sct_b_residualmedians->Add(m_sct_b1_residualmedians);
  m_sct_b_residualmedians->Add(m_sct_b2_residualmedians);
  m_sct_b_residualmedians->Add(m_sct_b3_residualmedians);


  //TH2F* m_pix_b0_xresvsmodetaphi_chi2 = new TH2F("pix_b0_xresvsmodetaphi_chi2","X Residual Fit Chi2/DOF vs Module Eta-Phi-ID Pixel Barrel L0",13,-6.5,6.5,22,-0.5,21.5);
  //m_pix_b0_xresvsmodetaphi_chi2->GetXaxis()->SetTitle("Module Eta-ID");
  //m_pix_b0_xresvsmodetaphi_chi2->GetYaxis()->SetTitle("Module Phi-ID");
  //meanRMSProjections3D(m_pix_b0_xresvsmodetaphi_3d,m_pix_b0_xresvsmodetaphi_chi2,4);


  //set y-axis minimum range 
  setMinWindow(m_si_barrel_pullX_width, minSiPullWidthWindow, maxSiPullWidthWindow);
  setMinWindow(m_si_barrel_pullY_width, minSiPullWidthWindow, maxSiPullWidthWindow);
  setMinWindow(m_si_eca_pullX_width, minSiPullWidthWindow, maxSiPullWidthWindow);
  setMinWindow(m_si_eca_pullY_width, minSiPullWidthWindow, maxSiPullWidthWindow);
  setMinWindow(m_si_ecc_pullX_width, minSiPullWidthWindow, maxSiPullWidthWindow);
  setMinWindow(m_si_ecc_pullY_width, minSiPullWidthWindow, maxSiPullWidthWindow);
  
  setMinWindow(m_si_barrel_pullX_mean, minSiResMeanWindow, maxSiResMeanWindow);
  setMinWindow(m_si_barrel_pullY_mean, minSiResMeanWindow, maxSiResMeanWindow);
  setMinWindow(m_si_eca_pullX_mean, minSiResMeanWindow, maxSiResMeanWindow);
  setMinWindow(m_si_eca_pullY_mean, minSiResMeanWindow, maxSiResMeanWindow);
  setMinWindow(m_si_ecc_pullX_mean, minSiResMeanWindow, maxSiResMeanWindow);
  setMinWindow(m_si_ecc_pullY_mean, minSiResMeanWindow, maxSiResMeanWindow);
  
  setMinWindow(m_si_barrel_resX_mean, minSiResMeanWindow, maxSiResMeanWindow);
  setMinWindow(m_si_barrel_resY_mean, minSiResMeanWindow, maxSiResMeanWindow);
  setMinWindow(m_si_eca_resX_mean, minSiResMeanWindow, maxSiResMeanWindow);
  setMinWindow(m_si_eca_resY_mean, minSiResMeanWindow, maxSiResMeanWindow);
  setMinWindow(m_si_ecc_resX_mean, minSiResMeanWindow, maxSiResMeanWindow);
  setMinWindow(m_si_ecc_resY_mean, minSiResMeanWindow, maxSiResMeanWindow);
  
  setMinWindow(m_si_barrel_resX_rms, minSiResWidthWindow, maxSiResWidthWindow);
  setMinWindow(m_si_barrel_resY_rms, minSiResWidthWindow, maxSiResWidthWindow);
  setMinWindow(m_si_eca_resX_rms, minSiResWidthWindow, maxSiResWidthWindow);
  setMinWindow(m_si_eca_resY_rms, minSiResWidthWindow, maxSiResWidthWindow);
  setMinWindow(m_si_ecc_resX_rms, minSiResWidthWindow, maxSiResWidthWindow);
  setMinWindow(m_si_ecc_resY_rms, minSiResWidthWindow, maxSiResWidthWindow);
  
  setMinWindow(m_pix_b_xresvsmodeta, minSiResMeanWindow, maxSiResMeanWindow);
  setMinWindow(m_pix_b_yresvsmodeta, minSiResMeanWindow, maxSiResMeanWindow);
  setMinWindow(m_pix_b_xresvsmodphi, minSiResMeanWindow, maxSiResMeanWindow);
  setMinWindow(m_pix_b_yresvsmodphi, minSiResMeanWindow, maxSiResMeanWindow);
  setMinWindow(m_sct_b_xresvsmodeta, minSiResMeanWindow, maxSiResMeanWindow);
  setMinWindow(m_sct_b_xresvsmodphi, minSiResMeanWindow, maxSiResMeanWindow);
  
  setMinWindow(m_pix_b_xresvsmodeta_width, minSiResWidthWindow, maxSiResWidthWindow);
  setMinWindow(m_pix_b_yresvsmodeta_width, minSiResWidthWindow, maxSiResWidthWindow);
  setMinWindow(m_pix_b_xresvsmodphi_width, minSiResWidthWindow, maxSiResWidthWindow);
  setMinWindow(m_pix_b_yresvsmodphi_width, minSiResWidthWindow, maxSiResWidthWindow);
  setMinWindow(m_sct_b_xresvsmodeta_width, minSiResWidthWindow, maxSiResWidthWindow);
  setMinWindow(m_sct_b_xresvsmodphi_width, minSiResWidthWindow, maxSiResWidthWindow);

  setMinWindow(m_pix_eca_xresvsmodphi, minSiResMeanWindow, maxSiResMeanWindow);
  setMinWindow(m_pix_eca_yresvsmodphi, minSiResMeanWindow, maxSiResMeanWindow);
  setMinWindow(m_pix_ecc_xresvsmodphi, minSiResMeanWindow, maxSiResMeanWindow);
  setMinWindow(m_pix_ecc_yresvsmodphi, minSiResMeanWindow, maxSiResMeanWindow);
  setMinWindow(m_sct_eca_xresvsmodphi, minSiResMeanWindow, maxSiResMeanWindow);
  setMinWindow(m_sct_ecc_xresvsmodphi, minSiResMeanWindow, maxSiResMeanWindow);


  if(  f->cd(path.c_str())==0 ) {
      //std::cerr << "MonitoringFile::fitMergedFile_IDAlignMonResiduals(): "
      //      << "No such directory \"" << path << "\"\n";
    return;
  }
  
  //delete the histograms already in the file with these keys so that 
  //they can be overwritten - we shouldn't have to do this, but seems to 
  //be necessary
  f->Delete("si_barrel_pullX_width;1");
  
  f->Delete("si_eca_pullX_width;1");
  f->Delete("si_ecc_pullX_width;1");
  f->Delete("si_barrel_pullY_width;1");
  f->Delete("si_eca_pullY_width;1");
  f->Delete("si_ecc_pullY_width;1");

  f->Delete("si_barrel_pullX_mean;1");
  f->Delete("si_eca_pullX_mean;1");
  f->Delete("si_ecc_pullX_mean;1");
  f->Delete("si_barrel_pullY_mean;1");
  f->Delete("si_eca_pullY_mean;1");
  f->Delete("si_ecc_pullY_mean;1");

  f->Delete("si_barrel_resX_mean;1");
  f->Delete("si_eca_resX_mean;1");
  f->Delete("si_ecc_resX_mean;1");
  f->Delete("si_barrel_resY_mean;1");
  f->Delete("si_eca_resY_mean;1");
  f->Delete("si_ecc_resY_mean;1");

  f->Delete("si_barrel_resX_rms;1");
  f->Delete("si_eca_resX_rms;1");
  f->Delete("si_ecc_resX_rms;1");
  f->Delete("si_barrel_resY_rms;1");
  f->Delete("si_eca_resY_rms;1");
  f->Delete("si_ecc_resY_rms;1");

  f->Delete("pix_b_xresvsmodeta;1");
  f->Delete("pix_b_xresvsmodphi;1");
  f->Delete("pix_b_yresvsmodeta;1");
  f->Delete("pix_b_yresvsmodphi;1");
  f->Delete("pix_eca_xresvsmodphi;1");
  f->Delete("pix_ecc_xresvsmodphi;1");
  f->Delete("pix_eca_yresvsmodphi;1");
  f->Delete("pix_ecc_yresvsmodphi;1");

  f->Delete("pix_b_xresvsmodeta_width;1");
  f->Delete("pix_b_yresvsmodeta_width;1");
  f->Delete("pix_b_xresvsmodphi_width;1");
  f->Delete("pix_b_yresvsmodphi_width;1");
  f->Delete("sct_b_xresvsmodeta_width;1");
  f->Delete("sct_b_xresvsmodphi_width;1");

  f->Delete("sct_b_xresvsmodeta;1");
  f->Delete("sct_b_xresvsmodphi;1");
  f->Delete("sct_eca_xresvsmodphi;1");
  f->Delete("sct_ecc_xresvsmodphi;1");

  f->Delete("pix_b0_xresvsmodetaphi_mean;1");
  f->Delete("pix_b1_xresvsmodetaphi_mean;1");
  f->Delete("pix_b2_xresvsmodetaphi_mean;1");
  f->Delete("pix_b3_xresvsmodetaphi_mean;1");
  f->Delete("pix_b0_yresvsmodetaphi_mean;1");
  f->Delete("pix_b1_yresvsmodetaphi_mean;1");
  f->Delete("pix_b2_yresvsmodetaphi_mean;1");
  f->Delete("pix_b3_yresvsmodetaphi_mean;1");

  f->Delete("pix_b0_xresvsmodetaphi_rms;1");
  f->Delete("pix_b1_xresvsmodetaphi_rms;1");
  f->Delete("pix_b2_xresvsmodetaphi_rms;1");
  f->Delete("pix_b3_xresvsmodetaphi_rms;1");
  f->Delete("pix_b0_yresvsmodetaphi_rms;1");
  f->Delete("pix_b1_yresvsmodetaphi_rms;1");
  f->Delete("pix_b2_yresvsmodetaphi_rms;1");
  f->Delete("pix_b3_yresvsmodetaphi_rms;1");

  f->Delete("sct_b0_xresvsmodetaphi_mean;1");
  f->Delete("sct_b1_xresvsmodetaphi_mean;1");
  f->Delete("sct_b2_xresvsmodetaphi_mean;1");
  f->Delete("sct_b3_xresvsmodetaphi_mean;1");

  f->Delete("sct_b0_xresvsmodetaphi_rms;1");
  f->Delete("sct_b1_xresvsmodetaphi_rms;1");
  f->Delete("sct_b2_xresvsmodetaphi_rms;1");
  f->Delete("sct_b3_xresvsmodetaphi_rms;1");
  
  //writing the histograms to the file
  m_si_barrel_pullX_width->Write("",TObject::kOverwrite);
  m_si_eca_pullX_width->Write("",TObject::kOverwrite);
  m_si_ecc_pullX_width->Write("",TObject::kOverwrite);
  m_si_barrel_pullY_width->Write("",TObject::kOverwrite);
  m_si_eca_pullY_width->Write("",TObject::kOverwrite);
  m_si_ecc_pullY_width->Write("",TObject::kOverwrite);

  m_si_barrel_pullX_mean->Write("",TObject::kOverwrite);
  m_si_eca_pullX_mean->Write("",TObject::kOverwrite);
  m_si_ecc_pullX_mean->Write("",TObject::kOverwrite);
  m_si_barrel_pullY_mean->Write("",TObject::kOverwrite);
  m_si_eca_pullY_mean->Write("",TObject::kOverwrite);
  m_si_ecc_pullY_mean->Write("",TObject::kOverwrite);

  f->Write();

  m_si_barrel_resX_mean->Write("",TObject::kOverwrite);
  m_si_eca_resX_mean->Write("",TObject::kOverwrite);
  m_si_ecc_resX_mean->Write("",TObject::kOverwrite);
  m_si_barrel_resY_mean->Write("",TObject::kOverwrite);
  m_si_eca_resY_mean->Write("",TObject::kOverwrite);
  m_si_ecc_resY_mean->Write("",TObject::kOverwrite);

  m_si_barrel_resX_rms->Write("",TObject::kOverwrite);
  m_si_eca_resX_rms->Write("",TObject::kOverwrite);
  m_si_ecc_resX_rms->Write("",TObject::kOverwrite);
  m_si_barrel_resY_rms->Write("",TObject::kOverwrite);
  m_si_eca_resY_rms->Write("",TObject::kOverwrite);
  m_si_ecc_resY_rms->Write("",TObject::kOverwrite);

  // Q: why do we need to call f->Write() more than once?
  // A: Because root is a pile of horse s**t.
  f->Write();

  m_pix_b_xresvsmodeta->Write("",TObject::kOverwrite);
  m_pix_b_xresvsmodphi->Write("",TObject::kOverwrite);
  m_pix_b_yresvsmodeta->Write("",TObject::kOverwrite);
  m_pix_b_yresvsmodphi->Write("",TObject::kOverwrite);
  m_pix_eca_xresvsmodphi->Write("",TObject::kOverwrite);
  m_pix_ecc_xresvsmodphi->Write("",TObject::kOverwrite);
  m_pix_eca_yresvsmodphi->Write("",TObject::kOverwrite);
  m_pix_ecc_yresvsmodphi->Write("",TObject::kOverwrite);
  
  f->Write();

  m_sct_b_xresvsmodeta->Write("",TObject::kOverwrite);
  m_sct_b_xresvsmodphi->Write("",TObject::kOverwrite);
  m_sct_eca_xresvsmodphi->Write("",TObject::kOverwrite);
  m_sct_ecc_xresvsmodphi->Write("",TObject::kOverwrite);

  m_pix_b_xresvsmodeta_width->Write("",TObject::kOverwrite);
  m_pix_b_yresvsmodeta_width->Write("",TObject::kOverwrite);
  m_pix_b_xresvsmodphi_width->Write("",TObject::kOverwrite);
  m_pix_b_yresvsmodphi_width->Write("",TObject::kOverwrite);
  m_sct_b_xresvsmodeta_width->Write("",TObject::kOverwrite);
  m_sct_b_xresvsmodphi_width->Write("",TObject::kOverwrite);
  
  f->Write();

  m_pix_b0_xresvsmodetaphi_mean->Write("",TObject::kOverwrite);
  m_pix_b1_xresvsmodetaphi_mean->Write("",TObject::kOverwrite);
  m_pix_b2_xresvsmodetaphi_mean->Write("",TObject::kOverwrite);
  m_pix_b3_xresvsmodetaphi_mean->Write("",TObject::kOverwrite);
  m_pix_b0_yresvsmodetaphi_mean->Write("",TObject::kOverwrite);
  m_pix_b1_yresvsmodetaphi_mean->Write("",TObject::kOverwrite);
  m_pix_b2_yresvsmodetaphi_mean->Write("",TObject::kOverwrite);
  m_pix_b3_yresvsmodetaphi_mean->Write("",TObject::kOverwrite);

  f->Write();

  m_pix_b0_xresvsmodetaphi_rms->Write("",TObject::kOverwrite);
  m_pix_b1_xresvsmodetaphi_rms->Write("",TObject::kOverwrite);
  m_pix_b2_xresvsmodetaphi_rms->Write("",TObject::kOverwrite);
  m_pix_b3_xresvsmodetaphi_rms->Write("",TObject::kOverwrite);
  m_pix_b0_yresvsmodetaphi_rms->Write("",TObject::kOverwrite);
  m_pix_b1_yresvsmodetaphi_rms->Write("",TObject::kOverwrite);
  m_pix_b2_yresvsmodetaphi_rms->Write("",TObject::kOverwrite);
  m_pix_b3_yresvsmodetaphi_rms->Write("",TObject::kOverwrite);

  //m_pix_b0_xresvsmodetaphi_chi2->Write("",TObject::kOverwrite);

  f->Write();

  m_sct_b0_xresvsmodetaphi_mean->Write("",TObject::kOverwrite);
  m_sct_b1_xresvsmodetaphi_mean->Write("",TObject::kOverwrite);
  m_sct_b2_xresvsmodetaphi_mean->Write("",TObject::kOverwrite);
  m_sct_b3_xresvsmodetaphi_mean->Write("",TObject::kOverwrite);

  m_sct_b0_xresvsmodetaphi_rms->Write("",TObject::kOverwrite);
  m_sct_b1_xresvsmodetaphi_rms->Write("",TObject::kOverwrite);
  m_sct_b2_xresvsmodetaphi_rms->Write("",TObject::kOverwrite);
  m_sct_b3_xresvsmodetaphi_rms->Write("",TObject::kOverwrite);

  f->Write();

  m_pix_b0_residualmeans->Write("",TObject::kOverwrite);
  m_pix_b1_residualmeans->Write("",TObject::kOverwrite);
  m_pix_b2_residualmeans->Write("",TObject::kOverwrite);
  m_pix_b3_residualmeans->Write("",TObject::kOverwrite);
  m_sct_b0_residualmeans->Write("",TObject::kOverwrite);
  m_sct_b1_residualmeans->Write("",TObject::kOverwrite);
  m_sct_b2_residualmeans->Write("",TObject::kOverwrite);
  m_sct_b3_residualmeans->Write("",TObject::kOverwrite);
  m_pix_b_residualmeans->Write("",TObject::kOverwrite);
  m_sct_b_residualmeans->Write("",TObject::kOverwrite);
  m_pix_b_residualmedians->Write("",TObject::kOverwrite);
  m_sct_b_residualmedians->Write("",TObject::kOverwrite);

  f->Write();

  m_pix_b0_residualfiterrors->Write("",TObject::kOverwrite);
  m_pix_b1_residualfiterrors->Write("",TObject::kOverwrite);
  m_pix_b2_residualfiterrors->Write("",TObject::kOverwrite);
  m_pix_b3_residualfiterrors->Write("",TObject::kOverwrite);
  m_sct_b0_residualfiterrors->Write("",TObject::kOverwrite);
  m_sct_b1_residualfiterrors->Write("",TObject::kOverwrite);
  m_sct_b2_residualfiterrors->Write("",TObject::kOverwrite);
  m_sct_b3_residualfiterrors->Write("",TObject::kOverwrite);
  m_pix_b_residualfiterrors->Write("",TObject::kOverwrite);
  m_sct_b_residualfiterrors->Write("",TObject::kOverwrite);

  f->Write();

  //m_pix_b0_mag_vs_lb->Write();
  //m_pix_b0_base_vs_lb->Write();
  
  //f->Write();

  
  


  if(doOverlapResiduals){

    TH3F* m_pix_b0_Oxresxvsmodetaphi_3d = (TH3F*)(f->Get((path+"/pix_b0_Oxresxvsmodetaphi_3d").c_str()));
    TH3F* m_pix_b1_Oxresxvsmodetaphi_3d = (TH3F*)(f->Get((path+"/pix_b1_Oxresxvsmodetaphi_3d").c_str()));
    TH3F* m_pix_b2_Oxresxvsmodetaphi_3d = (TH3F*)(f->Get((path+"/pix_b2_Oxresxvsmodetaphi_3d").c_str()));
    TH3F* m_pix_b3_Oxresxvsmodetaphi_3d = (TH3F*)(f->Get((path+"/pix_b3_Oxresxvsmodetaphi_3d").c_str()));

    TH3F* m_pix_b0_Oxresyvsmodetaphi_3d = (TH3F*)(f->Get((path+"/pix_b0_Oxresyvsmodetaphi_3d").c_str()));
    TH3F* m_pix_b1_Oxresyvsmodetaphi_3d = (TH3F*)(f->Get((path+"/pix_b1_Oxresyvsmodetaphi_3d").c_str()));
    TH3F* m_pix_b2_Oxresyvsmodetaphi_3d = (TH3F*)(f->Get((path+"/pix_b2_Oxresyvsmodetaphi_3d").c_str()));
    TH3F* m_pix_b3_Oxresyvsmodetaphi_3d = (TH3F*)(f->Get((path+"/pix_b3_Oxresyvsmodetaphi_3d").c_str()));

    TH3F* m_pix_b0_Oyresxvsmodetaphi_3d = (TH3F*)(f->Get((path+"/pix_b0_Oyresxvsmodetaphi_3d").c_str()));
    TH3F* m_pix_b1_Oyresxvsmodetaphi_3d = (TH3F*)(f->Get((path+"/pix_b1_Oyresxvsmodetaphi_3d").c_str()));
    TH3F* m_pix_b2_Oyresxvsmodetaphi_3d = (TH3F*)(f->Get((path+"/pix_b2_Oyresxvsmodetaphi_3d").c_str()));
    TH3F* m_pix_b3_Oyresxvsmodetaphi_3d = (TH3F*)(f->Get((path+"/pix_b3_Oyresxvsmodetaphi_3d").c_str()));

    TH3F* m_pix_b0_Oyresyvsmodetaphi_3d = (TH3F*)(f->Get((path+"/pix_b0_Oyresyvsmodetaphi_3d").c_str()));
    TH3F* m_pix_b1_Oyresyvsmodetaphi_3d = (TH3F*)(f->Get((path+"/pix_b1_Oyresyvsmodetaphi_3d").c_str()));
    TH3F* m_pix_b2_Oyresyvsmodetaphi_3d = (TH3F*)(f->Get((path+"/pix_b2_Oyresyvsmodetaphi_3d").c_str()));
    TH3F* m_pix_b3_Oyresyvsmodetaphi_3d = (TH3F*)(f->Get((path+"/pix_b3_Oyresyvsmodetaphi_3d").c_str()));

    TH3F* m_sct_b0_Oxresxvsmodetaphi_3d = (TH3F*)(f->Get((path+"/sct_b0_Oxresxvsmodetaphi_3d").c_str()));
    TH3F* m_sct_b1_Oxresxvsmodetaphi_3d = (TH3F*)(f->Get((path+"/sct_b1_Oxresxvsmodetaphi_3d").c_str()));
    TH3F* m_sct_b2_Oxresxvsmodetaphi_3d = (TH3F*)(f->Get((path+"/sct_b2_Oxresxvsmodetaphi_3d").c_str()));
    TH3F* m_sct_b3_Oxresxvsmodetaphi_3d = (TH3F*)(f->Get((path+"/sct_b3_Oxresxvsmodetaphi_3d").c_str()));

    TH3F* m_sct_b0_Oyresxvsmodetaphi_3d = (TH3F*)(f->Get((path+"/sct_b0_Oyresxvsmodetaphi_3d").c_str()));
    TH3F* m_sct_b1_Oyresxvsmodetaphi_3d = (TH3F*)(f->Get((path+"/sct_b1_Oyresxvsmodetaphi_3d").c_str()));
    TH3F* m_sct_b2_Oyresxvsmodetaphi_3d = (TH3F*)(f->Get((path+"/sct_b2_Oyresxvsmodetaphi_3d").c_str()));
    TH3F* m_sct_b3_Oyresxvsmodetaphi_3d = (TH3F*)(f->Get((path+"/sct_b3_Oyresxvsmodetaphi_3d").c_str()));
   

    //XOverlap residual X mean as a function of ring in the pixel barrel (sensitive to ring-expansion,contraction distortions)
    TH1F* m_pix_b0_Oxresxvsmodeta_mean = new TH1F("pix_b0_Oxresxvsmodeta_mean","X-Overlap X Residual Mean vs Eta-ID Pixel Barrel IBL",20,-10.5,9.5);
    TH1F* m_pix_b1_Oxresxvsmodeta_mean = new TH1F("pix_b1_Oxresxvsmodeta_mean","X-Overlap X Residual Mean vs Eta-ID Pixel Barrel L0",13,-6.5,6.5);
    TH1F* m_pix_b2_Oxresxvsmodeta_mean = new TH1F("pix_b2_Oxresxvsmodeta_mean","X-Overlap X Residual Mean vs Eta-ID Pixel Barrel L1",13,-6.5,6.5);
    TH1F* m_pix_b3_Oxresxvsmodeta_mean = new TH1F("pix_b3_Oxresxvsmodeta_mean","X-Overlap X Residual Mean vs Eta-ID Pixel Barrel L2",13,-6.5,6.5);
    m_pix_b0_Oxresxvsmodeta_mean->GetYaxis()->SetTitle("X-Overlap Mean Residual X");
    m_pix_b1_Oxresxvsmodeta_mean->GetYaxis()->SetTitle("X-Overlap Mean Residual X");
    m_pix_b2_Oxresxvsmodeta_mean->GetYaxis()->SetTitle("X-Overlap Mean Residual X");
    m_pix_b3_Oxresxvsmodeta_mean->GetYaxis()->SetTitle("X-Overlap Mean Residual X");
    m_pix_b0_Oxresxvsmodeta_mean->GetXaxis()->SetTitle("Module Eta-ID");
    m_pix_b1_Oxresxvsmodeta_mean->GetXaxis()->SetTitle("Module Eta-ID");
    m_pix_b2_Oxresxvsmodeta_mean->GetXaxis()->SetTitle("Module Eta-ID");
    m_pix_b3_Oxresxvsmodeta_mean->GetXaxis()->SetTitle("Module Eta-ID");
    meanRMSProjections3D_XY(m_pix_b0_Oxresxvsmodetaphi_3d,m_pix_b0_Oxresxvsmodeta_mean,0,2);
    meanRMSProjections3D_XY(m_pix_b1_Oxresxvsmodetaphi_3d,m_pix_b1_Oxresxvsmodeta_mean,0,2);
    meanRMSProjections3D_XY(m_pix_b2_Oxresxvsmodetaphi_3d,m_pix_b2_Oxresxvsmodeta_mean,0,2);
    meanRMSProjections3D_XY(m_pix_b3_Oxresxvsmodetaphi_3d,m_pix_b3_Oxresxvsmodeta_mean,0,2);
    plots->Add(m_pix_b0_Oxresxvsmodeta_mean);
    plots->Add(m_pix_b1_Oxresxvsmodeta_mean);
    plots->Add(m_pix_b2_Oxresxvsmodeta_mean);
    plots->Add(m_pix_b3_Oxresxvsmodeta_mean);
    TH1F* m_pix_b_Oxresxvsmodeta_mean = combineHistos("pix_b_Oxresxvsmodeta_mean","X-Overlap X Residual Mean vs (Modified) Eta-ID Pixel Barrel",plots,10);
    m_pix_b_Oxresxvsmodeta_mean->GetYaxis()->SetTitle("X-Overlap Mean Residual X");
    m_pix_b_Oxresxvsmodeta_mean->GetXaxis()->SetTitle("(Modified) Module Eta-ID");
    plots->Clear();

    //XOverlap residual X width as a function of ring in the pixel barrel (sensitive to ring-expansion,contraction distortions)
    TH1F* m_pix_b0_Oxresxvsmodeta_width = new TH1F("pix_b0_Oxresxvsmodeta_width","X-Overlap X Residual Width vs Eta-ID Pixel Barrel IBL",20,-10.5,9.5);
    TH1F* m_pix_b1_Oxresxvsmodeta_width = new TH1F("pix_b1_Oxresxvsmodeta_width","X-Overlap X Residual Width vs Eta-ID Pixel Barrel L0",13,-6.5,6.5);
    TH1F* m_pix_b2_Oxresxvsmodeta_width = new TH1F("pix_b2_Oxresxvsmodeta_width","X-Overlap X Residual Width vs Eta-ID Pixel Barrel L1",13,-6.5,6.5);
    TH1F* m_pix_b3_Oxresxvsmodeta_width = new TH1F("pix_b3_Oxresxvsmodeta_width","X-Overlap X Residual Width vs Eta-ID Pixel Barrel L2",13,-6.5,6.5);
    m_pix_b0_Oxresxvsmodeta_width->GetYaxis()->SetTitle("X-Overlap Width Residual X");
    m_pix_b1_Oxresxvsmodeta_width->GetYaxis()->SetTitle("X-Overlap Width Residual X");
    m_pix_b2_Oxresxvsmodeta_width->GetYaxis()->SetTitle("X-Overlap Width Residual X");
    m_pix_b2_Oxresxvsmodeta_width->GetYaxis()->SetTitle("X-Overlap Width Residual X");
    m_pix_b0_Oxresxvsmodeta_width->GetXaxis()->SetTitle("Module Eta-ID");
    m_pix_b1_Oxresxvsmodeta_width->GetXaxis()->SetTitle("Module Eta-ID");
    m_pix_b2_Oxresxvsmodeta_width->GetXaxis()->SetTitle("Module Eta-ID");
    m_pix_b2_Oxresxvsmodeta_width->GetXaxis()->SetTitle("Module Eta-ID");
    meanRMSProjections3D_XY(m_pix_b0_Oxresxvsmodetaphi_3d,m_pix_b0_Oxresxvsmodeta_width,0,3);
    meanRMSProjections3D_XY(m_pix_b1_Oxresxvsmodetaphi_3d,m_pix_b1_Oxresxvsmodeta_width,0,3);
    meanRMSProjections3D_XY(m_pix_b2_Oxresxvsmodetaphi_3d,m_pix_b2_Oxresxvsmodeta_width,0,3);
    meanRMSProjections3D_XY(m_pix_b3_Oxresxvsmodetaphi_3d,m_pix_b3_Oxresxvsmodeta_width,0,3);
    plots->Add(m_pix_b0_Oxresxvsmodeta_width);
    plots->Add(m_pix_b1_Oxresxvsmodeta_width);
    plots->Add(m_pix_b2_Oxresxvsmodeta_width);
    plots->Add(m_pix_b3_Oxresxvsmodeta_width);
    TH1F* m_pix_b_Oxresxvsmodeta_width = combineHistos("pix_b_Oxresxvsmodeta_width","X-Overlap X Residual Width vs (Modified) Eta-ID Pixel Barrel",plots,10);
    m_pix_b_Oxresxvsmodeta_width->GetYaxis()->SetTitle("X-Overlap Width Residual X");
    m_pix_b_Oxresxvsmodeta_width->GetXaxis()->SetTitle("(Modified) Module Eta-ID");
    plots->Clear();

    //XOverlap residual Y mean as a function of ring in the pixel barrel (sensitive to ring shear distortions)
    TH1F* m_pix_b0_Oxresyvsmodeta_mean = new TH1F("pix_b0_Oxresyvsmodeta_mean","X-Overlap Y Residual Mean vs Eta-ID Pixel Barrel IBL",20,-10.5,9.5);
    TH1F* m_pix_b1_Oxresyvsmodeta_mean = new TH1F("pix_b1_Oxresyvsmodeta_mean","X-Overlap Y Residual Mean vs Eta-ID Pixel Barrel L0",13,-6.5,6.5);
    TH1F* m_pix_b2_Oxresyvsmodeta_mean = new TH1F("pix_b2_Oxresyvsmodeta_mean","X-Overlap Y Residual Mean vs Eta-ID Pixel Barrel L1",13,-6.5,6.5);
    TH1F* m_pix_b3_Oxresyvsmodeta_mean = new TH1F("pix_b3_Oxresyvsmodeta_mean","X-Overlap Y Residual Mean vs Eta-ID Pixel Barrel L2",13,-6.5,6.5);
    m_pix_b0_Oxresyvsmodeta_mean->GetYaxis()->SetTitle("X-Overlap Mean Residual Y");
    m_pix_b1_Oxresyvsmodeta_mean->GetYaxis()->SetTitle("X-Overlap Mean Residual Y");
    m_pix_b2_Oxresyvsmodeta_mean->GetYaxis()->SetTitle("X-Overlap Mean Residual Y");
    m_pix_b3_Oxresyvsmodeta_mean->GetYaxis()->SetTitle("X-Overlap Mean Residual Y");
    m_pix_b0_Oxresyvsmodeta_mean->GetXaxis()->SetTitle("Module Eta-ID");
    m_pix_b1_Oxresyvsmodeta_mean->GetXaxis()->SetTitle("Module Eta-ID");
    m_pix_b2_Oxresyvsmodeta_mean->GetXaxis()->SetTitle("Module Eta-ID");
    m_pix_b3_Oxresyvsmodeta_mean->GetXaxis()->SetTitle("Module Eta-ID");
    meanRMSProjections3D_XY(m_pix_b0_Oxresyvsmodetaphi_3d,m_pix_b0_Oxresyvsmodeta_mean,0,2);
    meanRMSProjections3D_XY(m_pix_b1_Oxresyvsmodetaphi_3d,m_pix_b1_Oxresyvsmodeta_mean,0,2);
    meanRMSProjections3D_XY(m_pix_b2_Oxresyvsmodetaphi_3d,m_pix_b2_Oxresyvsmodeta_mean,0,2);
    meanRMSProjections3D_XY(m_pix_b3_Oxresyvsmodetaphi_3d,m_pix_b3_Oxresyvsmodeta_mean,0,2);
    plots->Add(m_pix_b0_Oxresyvsmodeta_mean);
    plots->Add(m_pix_b1_Oxresyvsmodeta_mean);
    plots->Add(m_pix_b2_Oxresyvsmodeta_mean);
    plots->Add(m_pix_b3_Oxresyvsmodeta_mean);
    
    TH1F* m_pix_b_Oxresyvsmodeta_mean = combineHistos("pix_b_Oxresyvsmodeta_mean","X-Overlap Y Residual Mean vs (Modified) Eta-ID Pixel Barrel",plots,10);
    m_pix_b_Oxresyvsmodeta_mean->GetYaxis()->SetTitle("X-Overlap Mean Residual Y");
    m_pix_b_Oxresyvsmodeta_mean->GetXaxis()->SetTitle("(Modified) Module Eta-ID");
    plots->Clear();

    //XOverlap residual X width as a function of ring in the pixel barrel (sensitive to ring shear distortions)
    TH1F* m_pix_b0_Oxresyvsmodeta_width = new TH1F("pix_b0_Oxresyvsmodeta_width","X-Overlap Y Residual Width vs Eta-ID Pixel Barrel IBL",20,-10.5,9.5);
    TH1F* m_pix_b1_Oxresyvsmodeta_width = new TH1F("pix_b1_Oxresyvsmodeta_width","X-Overlap Y Residual Width vs Eta-ID Pixel Barrel L0",13,-6.5,6.5);
    TH1F* m_pix_b2_Oxresyvsmodeta_width = new TH1F("pix_b2_Oxresyvsmodeta_width","X-Overlap Y Residual Width vs Eta-ID Pixel Barrel L1",13,-6.5,6.5);
    TH1F* m_pix_b3_Oxresyvsmodeta_width = new TH1F("pix_b3_Oxresyvsmodeta_width","X-Overlap Y Residual Width vs Eta-ID Pixel Barrel L2",13,-6.5,6.5);
    m_pix_b0_Oxresyvsmodeta_width->GetYaxis()->SetTitle("X-Overlap Width Residual Y");
    m_pix_b1_Oxresyvsmodeta_width->GetYaxis()->SetTitle("X-Overlap Width Residual Y");
    m_pix_b2_Oxresyvsmodeta_width->GetYaxis()->SetTitle("X-Overlap Width Residual Y");
    m_pix_b3_Oxresyvsmodeta_width->GetYaxis()->SetTitle("X-Overlap Width Residual Y");
    m_pix_b0_Oxresyvsmodeta_width->GetXaxis()->SetTitle("Module Eta-ID");
    m_pix_b1_Oxresyvsmodeta_width->GetXaxis()->SetTitle("Module Eta-ID");
    m_pix_b2_Oxresyvsmodeta_width->GetXaxis()->SetTitle("Module Eta-ID");
    m_pix_b3_Oxresyvsmodeta_width->GetXaxis()->SetTitle("Module Eta-ID");
    meanRMSProjections3D_XY(m_pix_b0_Oxresyvsmodetaphi_3d,m_pix_b0_Oxresyvsmodeta_width,0,3);
    meanRMSProjections3D_XY(m_pix_b1_Oxresyvsmodetaphi_3d,m_pix_b1_Oxresyvsmodeta_width,0,3);
    meanRMSProjections3D_XY(m_pix_b2_Oxresyvsmodetaphi_3d,m_pix_b2_Oxresyvsmodeta_width,0,3);
    meanRMSProjections3D_XY(m_pix_b3_Oxresyvsmodetaphi_3d,m_pix_b3_Oxresyvsmodeta_width,0,3);
    plots->Add(m_pix_b0_Oxresyvsmodeta_width);
    plots->Add(m_pix_b1_Oxresyvsmodeta_width);
    plots->Add(m_pix_b2_Oxresyvsmodeta_width);
    plots->Add(m_pix_b3_Oxresyvsmodeta_width);
    TH1F* m_pix_b_Oxresyvsmodeta_width = combineHistos("pix_b_Oxresyvsmodeta_width","X-Overlap Y Residual Width vs (Modified) Eta-ID Pixel Barrel",plots,10);
    m_pix_b_Oxresyvsmodeta_width->GetYaxis()->SetTitle("X-Overlap Width Residual Y");
    m_pix_b_Oxresyvsmodeta_width->GetXaxis()->SetTitle("(Modified) Module Eta-ID");
    plots->Clear();

    //YOverlap residual Y mean as a function of stave in the pixel barrel (sensitive to stave-stretch distortions)
    TH1F* m_pix_b0_Oyresyvsmodphi_mean = new TH1F("pix_b0_Oyresyvsmodphi_mean","Y-Overlap Y Residual Mean vs Phi-ID Pixel Barrel IBL",14,-0.5,13.5);
    TH1F* m_pix_b1_Oyresyvsmodphi_mean = new TH1F("pix_b1_Oyresyvsmodphi_mean","Y-Overlap Y Residual Mean vs Phi-ID Pixel Barrel L0",22,-0.5,21.5);
    TH1F* m_pix_b2_Oyresyvsmodphi_mean = new TH1F("pix_b2_Oyresyvsmodphi_mean","Y-Overlap Y Residual Mean vs Phi-ID Pixel Barrel L1",38,-0.5,37.5);
    TH1F* m_pix_b3_Oyresyvsmodphi_mean = new TH1F("pix_b3_Oyresyvsmodphi_mean","Y-Overlap Y Residual Mean vs Phi-ID Pixel Barrel L2",52,-0.5,51.5);
    m_pix_b0_Oyresyvsmodphi_mean->GetYaxis()->SetTitle("Y-Overlap Mean Residual Y");
    m_pix_b1_Oyresyvsmodphi_mean->GetYaxis()->SetTitle("Y-Overlap Mean Residual Y");
    m_pix_b2_Oyresyvsmodphi_mean->GetYaxis()->SetTitle("Y-Overlap Mean Residual Y");
    m_pix_b3_Oyresyvsmodphi_mean->GetYaxis()->SetTitle("Y-Overlap Mean Residual Y");
    m_pix_b0_Oyresyvsmodphi_mean->GetXaxis()->SetTitle("Module Phi-ID");
    m_pix_b1_Oyresyvsmodphi_mean->GetXaxis()->SetTitle("Module Phi-ID");
    m_pix_b2_Oyresyvsmodphi_mean->GetXaxis()->SetTitle("Module Phi-ID");
    m_pix_b3_Oyresyvsmodphi_mean->GetXaxis()->SetTitle("Module Phi-ID");
    meanRMSProjections3D_XY(m_pix_b0_Oyresyvsmodetaphi_3d,m_pix_b0_Oyresyvsmodphi_mean,1,2);
    meanRMSProjections3D_XY(m_pix_b1_Oyresyvsmodetaphi_3d,m_pix_b1_Oyresyvsmodphi_mean,1,2);
    meanRMSProjections3D_XY(m_pix_b2_Oyresyvsmodetaphi_3d,m_pix_b2_Oyresyvsmodphi_mean,1,2);
    meanRMSProjections3D_XY(m_pix_b3_Oyresyvsmodetaphi_3d,m_pix_b3_Oyresyvsmodphi_mean,1,2);
    plots->Add(m_pix_b0_Oyresyvsmodphi_mean);
    plots->Add(m_pix_b1_Oyresyvsmodphi_mean);
    plots->Add(m_pix_b2_Oyresyvsmodphi_mean);
    plots->Add(m_pix_b3_Oyresyvsmodphi_mean);
    TH1F* m_pix_b_Oyresyvsmodphi_mean = combineHistos("pix_b_Oyresyvsmodphi_mean","Y-Overlap Y Residual Mean vs (Modified) Eta-ID Pixel Barrel",plots,10);
    m_pix_b_Oyresyvsmodphi_mean->GetYaxis()->SetTitle("Y-Overlap Mean Residual Y");
    m_pix_b_Oyresyvsmodphi_mean->GetXaxis()->SetTitle("(Modified) Module Phi-ID");
    plots->Clear();

    //YOverlap residual Y width as a function of stave in the pixel barrel (sensitive to stave-stretch distortions)
    TH1F* m_pix_b0_Oyresyvsmodphi_width = new TH1F("pix_b0_Oyresyvsmodphi_width","Y-Overlap Y Residual Width vs Phi-ID Pixel Barrel IBL",14,-0.5,13.5);
    TH1F* m_pix_b1_Oyresyvsmodphi_width = new TH1F("pix_b1_Oyresyvsmodphi_width","Y-Overlap Y Residual Width vs Phi-ID Pixel Barrel L0",22,-0.5,21.5);
    TH1F* m_pix_b2_Oyresyvsmodphi_width = new TH1F("pix_b2_Oyresyvsmodphi_width","Y-Overlap Y Residual Width vs Phi-ID Pixel Barrel L1",38,-0.5,37.5);
    TH1F* m_pix_b3_Oyresyvsmodphi_width = new TH1F("pix_b3_Oyresyvsmodphi_width","Y-Overlap Y Residual Width vs Phi-ID Pixel Barrel L2",52,-0.5,51.5);
    m_pix_b0_Oyresyvsmodphi_width->GetYaxis()->SetTitle("Y-Overlap Width Residual Y");
    m_pix_b1_Oyresyvsmodphi_width->GetYaxis()->SetTitle("Y-Overlap Width Residual Y");
    m_pix_b2_Oyresyvsmodphi_width->GetYaxis()->SetTitle("Y-Overlap Width Residual Y");
    m_pix_b3_Oyresyvsmodphi_width->GetYaxis()->SetTitle("Y-Overlap Width Residual Y");
    m_pix_b0_Oyresyvsmodphi_width->GetXaxis()->SetTitle("Module Phi-ID");
    m_pix_b1_Oyresyvsmodphi_width->GetXaxis()->SetTitle("Module Phi-ID");
    m_pix_b2_Oyresyvsmodphi_width->GetXaxis()->SetTitle("Module Phi-ID");
    m_pix_b3_Oyresyvsmodphi_width->GetXaxis()->SetTitle("Module Phi-ID");
    meanRMSProjections3D_XY(m_pix_b0_Oyresyvsmodetaphi_3d,m_pix_b0_Oyresyvsmodphi_width,1,3);
    meanRMSProjections3D_XY(m_pix_b1_Oyresyvsmodetaphi_3d,m_pix_b1_Oyresyvsmodphi_width,1,3);
    meanRMSProjections3D_XY(m_pix_b2_Oyresyvsmodetaphi_3d,m_pix_b2_Oyresyvsmodphi_width,1,3);
    meanRMSProjections3D_XY(m_pix_b3_Oyresyvsmodetaphi_3d,m_pix_b3_Oyresyvsmodphi_width,1,3);
    plots->Add(m_pix_b0_Oyresyvsmodphi_width);
    plots->Add(m_pix_b1_Oyresyvsmodphi_width);
    plots->Add(m_pix_b2_Oyresyvsmodphi_width);
    plots->Add(m_pix_b3_Oyresyvsmodphi_width);
    TH1F* m_pix_b_Oyresyvsmodphi_width = combineHistos("pix_b_Oyresyvsmodphi_width","Y-Overlap Y Residual Width vs (Modified) Eta-ID Pixel Barrel",plots,10);
    m_pix_b_Oyresyvsmodphi_width->GetYaxis()->SetTitle("Y-Overlap Width Residual Y");
    m_pix_b_Oyresyvsmodphi_width->GetXaxis()->SetTitle("(Modified) Module Phi-ID");
    plots->Clear();

    //YOverlap residual X mean as a function of stave in the pixel barrel (sensitive to stave-shear distortions)
    TH1F* m_pix_b0_Oyresxvsmodphi_mean = new TH1F("pix_b0_Oyresxvsmodphi_mean","Y-Overlap X Residual Mean vs Phi-ID Pixel Barrel IBL",14,-0.5,13.5);
    TH1F* m_pix_b1_Oyresxvsmodphi_mean = new TH1F("pix_b1_Oyresxvsmodphi_mean","Y-Overlap X Residual Mean vs Phi-ID Pixel Barrel L0",22,-0.5,21.5);
    TH1F* m_pix_b2_Oyresxvsmodphi_mean = new TH1F("pix_b2_Oyresxvsmodphi_mean","Y-Overlap X Residual Mean vs Phi-ID Pixel Barrel L1",38,-0.5,37.5);
    TH1F* m_pix_b3_Oyresxvsmodphi_mean = new TH1F("pix_b3_Oyresxvsmodphi_mean","Y-Overlap X Residual Mean vs Phi-ID Pixel Barrel L2",52,-0.5,51.5);
    m_pix_b0_Oyresxvsmodphi_mean->GetYaxis()->SetTitle("Y-Overlap Mean Residual X");
    m_pix_b1_Oyresxvsmodphi_mean->GetYaxis()->SetTitle("Y-Overlap Mean Residual X");
    m_pix_b2_Oyresxvsmodphi_mean->GetYaxis()->SetTitle("Y-Overlap Mean Residual X");
    m_pix_b3_Oyresxvsmodphi_mean->GetYaxis()->SetTitle("Y-Overlap Mean Residual X");
    m_pix_b0_Oyresxvsmodphi_mean->GetXaxis()->SetTitle("Module Phi-ID");
    m_pix_b1_Oyresxvsmodphi_mean->GetXaxis()->SetTitle("Module Phi-ID");
    m_pix_b2_Oyresxvsmodphi_mean->GetXaxis()->SetTitle("Module Phi-ID");
    m_pix_b3_Oyresxvsmodphi_mean->GetXaxis()->SetTitle("Module Phi-ID");
    meanRMSProjections3D_XY(m_pix_b0_Oyresxvsmodetaphi_3d,m_pix_b0_Oyresxvsmodphi_mean,1,2);
    meanRMSProjections3D_XY(m_pix_b1_Oyresxvsmodetaphi_3d,m_pix_b1_Oyresxvsmodphi_mean,1,2);
    meanRMSProjections3D_XY(m_pix_b2_Oyresxvsmodetaphi_3d,m_pix_b2_Oyresxvsmodphi_mean,1,2);
    meanRMSProjections3D_XY(m_pix_b3_Oyresxvsmodetaphi_3d,m_pix_b3_Oyresxvsmodphi_mean,1,2);
    plots->Add(m_pix_b0_Oyresxvsmodphi_mean);
    plots->Add(m_pix_b1_Oyresxvsmodphi_mean);
    plots->Add(m_pix_b2_Oyresxvsmodphi_mean);
    plots->Add(m_pix_b3_Oyresxvsmodphi_mean);
    TH1F* m_pix_b_Oyresxvsmodphi_mean = combineHistos("pix_b_Oyresxvsmodphi_mean","Y-Overlap X Residual Mean vs (Modified) Eta-ID Pixel Barrel",plots,10);
    m_pix_b_Oyresxvsmodphi_mean->GetYaxis()->SetTitle("Y-Overlap Mean Residual X");
    m_pix_b_Oyresxvsmodphi_mean->GetXaxis()->SetTitle("(Modified) Module Phi-ID");
    plots->Clear();

    //YOverlap residual X width as a function of stave in the pixel barrel (sensitive to stave-shear distortions)
    
    TH1F* m_pix_b0_Oyresxvsmodphi_width = new TH1F("pix_b0_Oyresxvsmodphi_width","Y-Overlap X Residual Width vs Phi-ID Pixel Barrel IBL",14,-0.5,13.5);
    TH1F* m_pix_b1_Oyresxvsmodphi_width = new TH1F("pix_b1_Oyresxvsmodphi_width","Y-Overlap X Residual Width vs Phi-ID Pixel Barrel L0",22,-0.5,21.5);
    TH1F* m_pix_b2_Oyresxvsmodphi_width = new TH1F("pix_b2_Oyresxvsmodphi_width","Y-Overlap X Residual Width vs Phi-ID Pixel Barrel L1",38,-0.5,37.5);
    TH1F* m_pix_b3_Oyresxvsmodphi_width = new TH1F("pix_b3_Oyresxvsmodphi_width","Y-Overlap X Residual Width vs Phi-ID Pixel Barrel L2",52,-0.5,51.5);
    m_pix_b0_Oyresxvsmodphi_width->GetYaxis()->SetTitle("Y-Overlap Width Residual X");
    m_pix_b1_Oyresxvsmodphi_width->GetYaxis()->SetTitle("Y-Overlap Width Residual X");
    m_pix_b2_Oyresxvsmodphi_width->GetYaxis()->SetTitle("Y-Overlap Width Residual X");
    m_pix_b3_Oyresxvsmodphi_width->GetYaxis()->SetTitle("Y-Overlap Width Residual X");
    m_pix_b0_Oyresxvsmodphi_width->GetXaxis()->SetTitle("Module Phi-ID");
    m_pix_b1_Oyresxvsmodphi_width->GetXaxis()->SetTitle("Module Phi-ID");
    m_pix_b2_Oyresxvsmodphi_width->GetXaxis()->SetTitle("Module Phi-ID");
    m_pix_b3_Oyresxvsmodphi_width->GetXaxis()->SetTitle("Module Phi-ID");
    meanRMSProjections3D_XY(m_pix_b0_Oyresxvsmodetaphi_3d,m_pix_b0_Oyresxvsmodphi_width,1,3);
    meanRMSProjections3D_XY(m_pix_b1_Oyresxvsmodetaphi_3d,m_pix_b1_Oyresxvsmodphi_width,1,3);
    meanRMSProjections3D_XY(m_pix_b2_Oyresxvsmodetaphi_3d,m_pix_b2_Oyresxvsmodphi_width,1,3);
    meanRMSProjections3D_XY(m_pix_b3_Oyresxvsmodetaphi_3d,m_pix_b3_Oyresxvsmodphi_width,1,3);
    plots->Add(m_pix_b0_Oyresxvsmodphi_width);
    plots->Add(m_pix_b1_Oyresxvsmodphi_width);
    plots->Add(m_pix_b2_Oyresxvsmodphi_width);
    plots->Add(m_pix_b3_Oyresxvsmodphi_width);
    TH1F* m_pix_b_Oyresxvsmodphi_width = combineHistos("pix_b_Oyresxvsmodphi_width","Y-Overlap X Residual Width vs (Modified) Eta-ID Pixel Barrel",plots,10);
    m_pix_b_Oyresxvsmodphi_width->GetYaxis()->SetTitle("Y-Overlap Width Residual X");
    m_pix_b_Oyresxvsmodphi_width->GetXaxis()->SetTitle("(Modified) Module Phi-ID");
    plots->Clear();

    //XOverlap residual X mean as a function of ring in the SCT barrel (sensitive to ring-expansion,contraction distortions)
    TH1F* m_sct_b0_Oxresxvsmodeta_mean = new TH1F("sct_b0_Oxresxvsmodeta_mean","X-Overlap X Residual Mean vs Eta-ID SCT Barrel L0",13,-6.5,6.5);
    TH1F* m_sct_b1_Oxresxvsmodeta_mean = new TH1F("sct_b1_Oxresxvsmodeta_mean","X-Overlap X Residual Mean vs Eta-ID SCT Barrel L1",13,-6.5,6.5);
    TH1F* m_sct_b2_Oxresxvsmodeta_mean = new TH1F("sct_b2_Oxresxvsmodeta_mean","X-Overlap X Residual Mean vs Eta-ID SCT Barrel L2",13,-6.5,6.5);
    TH1F* m_sct_b3_Oxresxvsmodeta_mean = new TH1F("sct_b3_Oxresxvsmodeta_mean","X-Overlap X Residual Mean vs Eta-ID SCT Barrel L3",13,-6.5,6.5);
    m_sct_b0_Oxresxvsmodeta_mean->GetYaxis()->SetTitle("X-Overlap Mean Residual X");
    m_sct_b1_Oxresxvsmodeta_mean->GetYaxis()->SetTitle("X-Overlap Mean Residual X");
    m_sct_b2_Oxresxvsmodeta_mean->GetYaxis()->SetTitle("X-Overlap Mean Residual X");
    m_sct_b3_Oxresxvsmodeta_mean->GetYaxis()->SetTitle("X-Overlap Mean Residual X");
    m_sct_b0_Oxresxvsmodeta_mean->GetXaxis()->SetTitle("Module Eta-ID");
    m_sct_b1_Oxresxvsmodeta_mean->GetXaxis()->SetTitle("Module Eta-ID");
    m_sct_b2_Oxresxvsmodeta_mean->GetXaxis()->SetTitle("Module Eta-ID");
    m_sct_b3_Oxresxvsmodeta_mean->GetXaxis()->SetTitle("Module Eta-ID");
    meanRMSProjections3D_XY(m_sct_b0_Oxresxvsmodetaphi_3d,m_sct_b0_Oxresxvsmodeta_mean,0,2);
    meanRMSProjections3D_XY(m_sct_b1_Oxresxvsmodetaphi_3d,m_sct_b1_Oxresxvsmodeta_mean,0,2);
    meanRMSProjections3D_XY(m_sct_b2_Oxresxvsmodetaphi_3d,m_sct_b2_Oxresxvsmodeta_mean,0,2);
    meanRMSProjections3D_XY(m_sct_b3_Oxresxvsmodetaphi_3d,m_sct_b3_Oxresxvsmodeta_mean,0,2);
    plots->Add(m_sct_b0_Oxresxvsmodeta_mean);
    plots->Add(m_sct_b1_Oxresxvsmodeta_mean);
    plots->Add(m_sct_b2_Oxresxvsmodeta_mean);
    plots->Add(m_sct_b3_Oxresxvsmodeta_mean);
    TH1F* m_sct_b_Oxresxvsmodeta_mean = combineHistos("sct_b_Oxresxvsmodeta_mean","X-Overlap X Residual Mean vs (Modified) Eta-ID SCT Barrel",plots,10);
    m_sct_b_Oxresxvsmodeta_mean->GetYaxis()->SetTitle("X-Overlap Mean Residual X");
    m_sct_b_Oxresxvsmodeta_mean->GetXaxis()->SetTitle("(Modified) Module Eta-ID");
    plots->Clear();


    //XOverlap residual X width as a function of ring in the SCT barrel (sensitive to ring-expansion,contraction distortions)
    TH1F* m_sct_b0_Oxresxvsmodeta_width = new TH1F("sct_b0_Oxresxvsmodeta_width","X-Overlap X Residual Width vs Eta-ID SCT Barrel L0",13,-6.5,6.5);
    TH1F* m_sct_b1_Oxresxvsmodeta_width = new TH1F("sct_b1_Oxresxvsmodeta_width","X-Overlap X Residual Width vs Eta-ID SCT Barrel L1",13,-6.5,6.5);
    TH1F* m_sct_b2_Oxresxvsmodeta_width = new TH1F("sct_b2_Oxresxvsmodeta_width","X-Overlap X Residual Width vs Eta-ID SCT Barrel L2",13,-6.5,6.5);
    TH1F* m_sct_b3_Oxresxvsmodeta_width = new TH1F("sct_b3_Oxresxvsmodeta_width","X-Overlap X Residual Width vs Eta-ID SCT Barrel L3",13,-6.5,6.5);
    m_sct_b0_Oxresxvsmodeta_width->GetYaxis()->SetTitle("X-Overlap Width Residual X");
    m_sct_b1_Oxresxvsmodeta_width->GetYaxis()->SetTitle("X-Overlap Width Residual X");
    m_sct_b2_Oxresxvsmodeta_width->GetYaxis()->SetTitle("X-Overlap Width Residual X");
    m_sct_b3_Oxresxvsmodeta_width->GetYaxis()->SetTitle("X-Overlap Width Residual X");
    m_sct_b0_Oxresxvsmodeta_width->GetXaxis()->SetTitle("Module Eta-ID");
    m_sct_b1_Oxresxvsmodeta_width->GetXaxis()->SetTitle("Module Eta-ID");
    m_sct_b2_Oxresxvsmodeta_width->GetXaxis()->SetTitle("Module Eta-ID");
    m_sct_b3_Oxresxvsmodeta_width->GetXaxis()->SetTitle("Module Eta-ID");
    meanRMSProjections3D_XY(m_sct_b0_Oxresxvsmodetaphi_3d,m_sct_b0_Oxresxvsmodeta_width,0,3);
    meanRMSProjections3D_XY(m_sct_b1_Oxresxvsmodetaphi_3d,m_sct_b1_Oxresxvsmodeta_width,0,3);
    meanRMSProjections3D_XY(m_sct_b2_Oxresxvsmodetaphi_3d,m_sct_b2_Oxresxvsmodeta_width,0,3);
    meanRMSProjections3D_XY(m_sct_b3_Oxresxvsmodetaphi_3d,m_sct_b3_Oxresxvsmodeta_width,0,3);
    plots->Add(m_sct_b0_Oxresxvsmodeta_width);
    plots->Add(m_sct_b1_Oxresxvsmodeta_width);
    plots->Add(m_sct_b2_Oxresxvsmodeta_width);
    plots->Add(m_sct_b3_Oxresxvsmodeta_width);
    TH1F* m_sct_b_Oxresxvsmodeta_width = combineHistos("sct_b_Oxresxvsmodeta_width","X-Overlap X Residual Width vs (Modified) Eta-ID SCT Barrel",plots,10);
    m_sct_b_Oxresxvsmodeta_width->GetYaxis()->SetTitle("X-Overlap Width Residual X");
    m_sct_b_Oxresxvsmodeta_width->GetXaxis()->SetTitle("(Modified) Module Eta-ID");
    plots->Clear();


    //YOverlap residual X mean as a function of stave in the SCT barrel (sensitive to stave shear distortions)
    TH1F* m_sct_b0_Oyresxvsmodphi_mean = new TH1F("sct_b0_Oyresxvsmodphi_mean","Y-Overlap X Residual Mean vs Phi-ID SCT Barrel L0",32,-0.5,31.5);
    TH1F* m_sct_b1_Oyresxvsmodphi_mean = new TH1F("sct_b1_Oyresxvsmodphi_mean","Y-Overlap X Residual Mean vs Phi-ID SCT Barrel L1",40,-0.5,39.5);
    TH1F* m_sct_b2_Oyresxvsmodphi_mean = new TH1F("sct_b2_Oyresxvsmodphi_mean","Y-Overlap X Residual Mean vs Phi-ID SCT Barrel L2",48,-0.5,47.5);
    TH1F* m_sct_b3_Oyresxvsmodphi_mean = new TH1F("sct_b3_Oyresxvsmodphi_mean","Y-Overlap X Residual Mean vs Phi-ID SCT Barrel L3",56,-0.5,55.5);
    m_sct_b0_Oyresxvsmodphi_mean->GetYaxis()->SetTitle("Y-Overlap Mean Residual X");
    m_sct_b1_Oyresxvsmodphi_mean->GetYaxis()->SetTitle("Y-Overlap Mean Residual X");
    m_sct_b2_Oyresxvsmodphi_mean->GetYaxis()->SetTitle("Y-Overlap Mean Residual X");
    m_sct_b3_Oyresxvsmodphi_mean->GetYaxis()->SetTitle("Y-Overlap Mean Residual X");
    m_sct_b0_Oyresxvsmodphi_mean->GetXaxis()->SetTitle("Module Phi-ID");
    m_sct_b1_Oyresxvsmodphi_mean->GetXaxis()->SetTitle("Module Phi-ID");
    m_sct_b2_Oyresxvsmodphi_mean->GetXaxis()->SetTitle("Module Phi-ID");
    m_sct_b3_Oyresxvsmodphi_mean->GetXaxis()->SetTitle("Module Phi-ID");
    meanRMSProjections3D_XY(m_sct_b0_Oyresxvsmodetaphi_3d,m_sct_b0_Oyresxvsmodphi_mean,1,2);
    meanRMSProjections3D_XY(m_sct_b1_Oyresxvsmodetaphi_3d,m_sct_b1_Oyresxvsmodphi_mean,1,2);
    meanRMSProjections3D_XY(m_sct_b2_Oyresxvsmodetaphi_3d,m_sct_b2_Oyresxvsmodphi_mean,1,2);
    meanRMSProjections3D_XY(m_sct_b3_Oyresxvsmodetaphi_3d,m_sct_b3_Oyresxvsmodphi_mean,1,2);
    plots->Add(m_sct_b0_Oyresxvsmodphi_mean);
    plots->Add(m_sct_b1_Oyresxvsmodphi_mean);
    plots->Add(m_sct_b2_Oyresxvsmodphi_mean);
    plots->Add(m_sct_b3_Oyresxvsmodphi_mean);
    TH1F* m_sct_b_Oyresxvsmodphi_mean = combineHistos("sct_b_Oyresxvsmodphi_mean","Y-Overlap X Residual Mean vs (Modified) Phi-ID SCT Barrel",plots,10);
    m_sct_b_Oyresxvsmodphi_mean->GetYaxis()->SetTitle("Y-Overlap Mean Residual X");
    m_sct_b_Oyresxvsmodphi_mean->GetXaxis()->SetTitle("(Modified) Module Phi-ID");
    plots->Clear();
    
    //YOverlap residual X width as a function of stave in the SCT barrel (sensitive to stave shear distortions)
    TH1F* m_sct_b0_Oyresxvsmodphi_width = new TH1F("sct_b0_Oyresxvsmodphi_width","Y-Overlap X Residual Width vs Phi-ID SCT Barrel L0",32,-0.5,31.5);
    TH1F* m_sct_b1_Oyresxvsmodphi_width = new TH1F("sct_b1_Oyresxvsmodphi_width","Y-Overlap X Residual Width vs Phi-ID SCT Barrel L1",40,-0.5,39.5);
    TH1F* m_sct_b2_Oyresxvsmodphi_width = new TH1F("sct_b2_Oyresxvsmodphi_width","Y-Overlap X Residual Width vs Phi-ID SCT Barrel L2",48,-0.5,47.5);
    TH1F* m_sct_b3_Oyresxvsmodphi_width = new TH1F("sct_b3_Oyresxvsmodphi_width","Y-Overlap X Residual Width vs Phi-ID SCT Barrel L3",56,-0.5,55.5);
    m_sct_b0_Oyresxvsmodphi_width->GetYaxis()->SetTitle("Y-Overlap Width Residual X");
    m_sct_b1_Oyresxvsmodphi_width->GetYaxis()->SetTitle("Y-Overlap Width Residual X");
    m_sct_b2_Oyresxvsmodphi_width->GetYaxis()->SetTitle("Y-Overlap Width Residual X");
    m_sct_b3_Oyresxvsmodphi_width->GetYaxis()->SetTitle("Y-Overlap Width Residual X");
    m_sct_b0_Oyresxvsmodphi_width->GetXaxis()->SetTitle("Module Phi-ID");
    m_sct_b1_Oyresxvsmodphi_width->GetXaxis()->SetTitle("Module Phi-ID");
    m_sct_b2_Oyresxvsmodphi_width->GetXaxis()->SetTitle("Module Phi-ID");
    m_sct_b3_Oyresxvsmodphi_width->GetXaxis()->SetTitle("Module Phi-ID");
    meanRMSProjections3D_XY(m_sct_b0_Oyresxvsmodetaphi_3d,m_sct_b0_Oyresxvsmodphi_width,1,3);
    meanRMSProjections3D_XY(m_sct_b1_Oyresxvsmodetaphi_3d,m_sct_b1_Oyresxvsmodphi_width,1,3);
    meanRMSProjections3D_XY(m_sct_b2_Oyresxvsmodetaphi_3d,m_sct_b2_Oyresxvsmodphi_width,1,3);
    meanRMSProjections3D_XY(m_sct_b3_Oyresxvsmodetaphi_3d,m_sct_b3_Oyresxvsmodphi_width,1,3);
    plots->Add(m_sct_b0_Oyresxvsmodphi_width);
    plots->Add(m_sct_b1_Oyresxvsmodphi_width);
    plots->Add(m_sct_b2_Oyresxvsmodphi_width);
    plots->Add(m_sct_b3_Oyresxvsmodphi_width);
    TH1F* m_sct_b_Oyresxvsmodphi_width = combineHistos("sct_b_Oyresxvsmodphi_width","Y-Overlap X Residual Width vs (Modified) Phi-ID SCT Barrel",plots,10);
    m_sct_b_Oyresxvsmodphi_width->GetYaxis()->SetTitle("Y-Overlap Width Residual X");
    m_sct_b_Oyresxvsmodphi_width->GetXaxis()->SetTitle("(Modified) Module Phi-ID");
    plots->Clear();
    

    //write the overlap histograms to file
    m_pix_b_Oxresxvsmodeta_mean->Write("",TObject::kOverwrite);
    m_pix_b_Oxresxvsmodeta_width->Write("",TObject::kOverwrite);
    m_pix_b_Oxresyvsmodeta_mean->Write("",TObject::kOverwrite);
    m_pix_b_Oxresyvsmodeta_width->Write("",TObject::kOverwrite);
    m_pix_b_Oyresyvsmodphi_mean->Write("",TObject::kOverwrite);
    m_pix_b_Oyresyvsmodphi_width->Write("",TObject::kOverwrite);

    f->Write();

    m_pix_b_Oyresxvsmodphi_mean->Write("",TObject::kOverwrite);
    m_pix_b_Oyresxvsmodphi_width->Write("",TObject::kOverwrite);
    m_sct_b_Oxresxvsmodeta_mean->Write("",TObject::kOverwrite);
    m_sct_b_Oxresxvsmodeta_width->Write("",TObject::kOverwrite);
    m_sct_b_Oyresxvsmodphi_mean->Write("",TObject::kOverwrite);
    m_sct_b_Oyresxvsmodphi_width->Write("",TObject::kOverwrite);

    f->Write(); 
  }

  return;
  
}
void 
MonitoringFile::
fitMergedFile_IDAlignMonGenericTracks (TFile* file, std::string run_dir, std::string tracksName) 
{
  std::string path;
  path= run_dir + "IDAlignMon/" + tracksName + "/GenericTracks";
  if(  file->cd(path.c_str())==0 ) {
      //std::cerr << "MonitoringFile::fitMergedFile_IDAlignMonGenericTracks(): "
      //      << "No such directory \"" << path << "\"\n";
    return;
  }
  //charge Asym vs pt
  if (CheckHistogram(file,(path+"/trk_pT_asym_barrel").c_str()) && CheckHistogram(file,(path+"/trk_pT_neg_barrel").c_str()) && CheckHistogram(file,(path+"/trk_pT_pos_barrel").c_str())  ) {
    TH1F* m_trk_pT_neg_barrel=(TH1F*)(file->Get((path+"/trk_pT_neg_barrel").c_str())->Clone());
    TH1F* m_trk_pT_pos_barrel=(TH1F*)(file->Get((path+"/trk_pT_pos_barrel").c_str())->Clone());
    TH1F* m_trk_pT_asym_barrel=(TH1F*)(file->Get((path+"/trk_pT_asym_barrel").c_str())->Clone());
    ProcessAsymHistograms(m_trk_pT_neg_barrel,m_trk_pT_pos_barrel,m_trk_pT_asym_barrel);
    m_trk_pT_asym_barrel->SetXTitle("p_{T} [GeV]");
    m_trk_pT_asym_barrel->Write("",TObject::kOverwrite);
  }
  if (CheckHistogram(file,(path+"/trk_pT_asym_ecc").c_str()) && CheckHistogram(file,(path+"/trk_pT_neg_ecc").c_str()) && CheckHistogram(file,(path+"/trk_pT_pos_ecc").c_str())  ) {
    TH1F* m_trk_pT_neg_ecc=(TH1F*)(file->Get((path+"/trk_pT_neg_ecc").c_str())->Clone());
    TH1F* m_trk_pT_pos_ecc=(TH1F*)(file->Get((path+"/trk_pT_pos_ecc").c_str())->Clone());
    TH1F* m_trk_pT_asym_ecc=(TH1F*)(file->Get((path+"/trk_pT_asym_ecc").c_str())->Clone());
    ProcessAsymHistograms(m_trk_pT_neg_ecc,m_trk_pT_pos_ecc,m_trk_pT_asym_ecc);
    m_trk_pT_asym_ecc->SetXTitle("p_{T} [GeV]");
    m_trk_pT_asym_ecc->Write("",TObject::kOverwrite);
  }
  if (CheckHistogram(file,(path+"/trk_pT_asym_eca").c_str()) && CheckHistogram(file,(path+"/trk_pT_neg_eca").c_str()) && CheckHistogram(file,(path+"/trk_pT_pos_eca").c_str())  ) {
    TH1F* m_trk_pT_neg_eca=(TH1F*)(file->Get((path+"/trk_pT_neg_eca").c_str())->Clone());
    TH1F* m_trk_pT_pos_eca=(TH1F*)(file->Get((path+"/trk_pT_pos_eca").c_str())->Clone());
    TH1F* m_trk_pT_asym_eca=(TH1F*)(file->Get((path+"/trk_pT_asym_eca").c_str())->Clone());
    ProcessAsymHistograms(m_trk_pT_neg_eca,m_trk_pT_pos_eca,m_trk_pT_asym_eca);
    m_trk_pT_asym_eca->SetXTitle("p_{T} [GeV]");
    m_trk_pT_asym_eca->Write("",TObject::kOverwrite);
  }
  if (CheckHistogram(file,(path+"/trk_pT_asym").c_str()) && CheckHistogram(file,(path+"/pT_n").c_str()) && CheckHistogram(file,(path+"/pT_p").c_str())  ) {
    TH1F* m_trk_pT_neg=(TH1F*)(file->Get((path+"/pT_n").c_str())->Clone());
    TH1F* m_trk_pT_pos=(TH1F*)(file->Get((path+"/pT_p").c_str())->Clone());
    TH1F* m_trk_pT_asym=(TH1F*)(file->Get((path+"/trk_pT_asym").c_str())->Clone());
    ProcessAsymHistograms(m_trk_pT_neg,m_trk_pT_pos,m_trk_pT_asym);
    m_trk_pT_asym->SetXTitle("p_{T} [GeV]");
    m_trk_pT_asym->Write("",TObject::kOverwrite);
  }

  //charge Asym vs phi
  if (CheckHistogram(file,(path+"/trk_phi0_asym_barrel").c_str()) && CheckHistogram(file,(path+"/trk_phi0_neg_barrel").c_str()) && CheckHistogram(file,(path+"/trk_phi0_pos_barrel").c_str())  ) {
    TH1F* m_trk_phi0_neg_barrel=(TH1F*)(file->Get((path+"/trk_phi0_neg_barrel").c_str())->Clone());
    TH1F* m_trk_phi0_pos_barrel=(TH1F*)(file->Get((path+"/trk_phi0_pos_barrel").c_str())->Clone());
    TH1F* m_trk_phi0_asym_barrel=(TH1F*)(file->Get((path+"/trk_phi0_asym_barrel").c_str())->Clone());
    ProcessAsymHistograms(m_trk_phi0_neg_barrel,m_trk_phi0_pos_barrel,m_trk_phi0_asym_barrel);
    m_trk_phi0_asym_barrel->SetXTitle("track #phi [rad]");
    m_trk_phi0_asym_barrel->Write("",TObject::kOverwrite);
  }
  if (CheckHistogram(file,(path+"/trk_phi0_asym_ecc").c_str()) && CheckHistogram(file,(path+"/trk_phi0_neg_ecc").c_str()) && CheckHistogram(file,(path+"/trk_phi0_pos_ecc").c_str())  ) {
    TH1F* m_trk_phi0_neg_ecc=(TH1F*)(file->Get((path+"/trk_phi0_neg_ecc").c_str())->Clone());
    TH1F* m_trk_phi0_pos_ecc=(TH1F*)(file->Get((path+"/trk_phi0_pos_ecc").c_str())->Clone());
    TH1F* m_trk_phi0_asym_ecc=(TH1F*)(file->Get((path+"/trk_phi0_asym_ecc").c_str())->Clone());
    ProcessAsymHistograms(m_trk_phi0_neg_ecc,m_trk_phi0_pos_ecc,m_trk_phi0_asym_ecc);
    m_trk_phi0_asym_ecc->SetXTitle("track #phi [rad]");
    m_trk_phi0_asym_ecc->Write("",TObject::kOverwrite);
  }
  if (CheckHistogram(file,(path+"/trk_phi0_asym_eca").c_str()) && CheckHistogram(file,(path+"/trk_phi0_neg_eca").c_str()) && CheckHistogram(file,(path+"/trk_phi0_pos_eca").c_str())  ) {
    TH1F* m_trk_phi0_neg_eca=(TH1F*)(file->Get((path+"/trk_phi0_neg_eca").c_str())->Clone());
    TH1F* m_trk_phi0_pos_eca=(TH1F*)(file->Get((path+"/trk_phi0_pos_eca").c_str())->Clone());
    TH1F* m_trk_phi0_asym_eca=(TH1F*)(file->Get((path+"/trk_phi0_asym_eca").c_str())->Clone());
    ProcessAsymHistograms(m_trk_phi0_neg_eca,m_trk_phi0_pos_eca,m_trk_phi0_asym_eca);
    m_trk_phi0_asym_eca->SetXTitle("track #phi [rad]");
    m_trk_phi0_asym_eca->Write("",TObject::kOverwrite);
  }

  //charge Asym vs eta
  if (CheckHistogram(file,(path+"/eta_asym").c_str()) && CheckHistogram(file,(path+"/eta_neg").c_str()) && CheckHistogram(file,(path+"/eta_pos").c_str())  ) {
    TH1F* m_eta_neg=(TH1F*)(file->Get((path+"/eta_neg").c_str())->Clone());
    TH1F* m_eta_pos=(TH1F*)(file->Get((path+"/eta_pos").c_str())->Clone());
    TH1F* m_eta_asym=(TH1F*)(file->Get((path+"/eta_asym").c_str())->Clone());
    ProcessAsymHistograms(m_eta_neg,m_eta_pos,m_eta_asym);
    m_eta_asym->SetXTitle("track #eta");
    m_eta_asym->Write("",TObject::kOverwrite);
  }

  //charge Asym vs d0 (corrected for vertex)
  if (CheckHistogram(file,(path+"/trk_d0c_asym_barrel").c_str()) && CheckHistogram(file,(path+"/trk_d0c_neg_barrel").c_str()) && CheckHistogram(file,(path+"/trk_d0c_pos_barrel").c_str())  ) {
    TH1F* m_trk_d0c_neg_barrel=(TH1F*)(file->Get((path+"/trk_d0c_neg_barrel").c_str())->Clone());
    TH1F* m_trk_d0c_pos_barrel=(TH1F*)(file->Get((path+"/trk_d0c_pos_barrel").c_str())->Clone());
    TH1F* m_trk_d0c_asym_barrel=(TH1F*)(file->Get((path+"/trk_d0c_asym_barrel").c_str())->Clone());
    ProcessAsymHistograms(m_trk_d0c_neg_barrel,m_trk_d0c_pos_barrel,m_trk_d0c_asym_barrel);
    m_trk_d0c_asym_barrel->SetXTitle("track d_{0} [mm]");
    m_trk_d0c_asym_barrel->Write("",TObject::kOverwrite);
  }
  if (CheckHistogram(file,(path+"/trk_d0c_asym_ecc").c_str()) && CheckHistogram(file,(path+"/trk_d0c_neg_ecc").c_str()) && CheckHistogram(file,(path+"/trk_d0c_pos_ecc").c_str())  ) {
    TH1F* m_trk_d0c_neg_ecc=(TH1F*)(file->Get((path+"/trk_d0c_neg_ecc").c_str())->Clone());
    TH1F* m_trk_d0c_pos_ecc=(TH1F*)(file->Get((path+"/trk_d0c_pos_ecc").c_str())->Clone());
    TH1F* m_trk_d0c_asym_ecc=(TH1F*)(file->Get((path+"/trk_d0c_asym_ecc").c_str())->Clone());
    ProcessAsymHistograms(m_trk_d0c_neg_ecc,m_trk_d0c_pos_ecc,m_trk_d0c_asym_ecc);
    m_trk_d0c_asym_ecc->SetXTitle("track d_{0} [mm]");
    m_trk_d0c_asym_ecc->Write("",TObject::kOverwrite);
  }
  if (CheckHistogram(file,(path+"/trk_d0c_asym_eca").c_str()) && CheckHistogram(file,(path+"/trk_d0c_neg_eca").c_str()) && CheckHistogram(file,(path+"/trk_d0c_pos_eca").c_str())  ) {
    TH1F* m_trk_d0c_neg_eca=(TH1F*)(file->Get((path+"/trk_d0c_neg_eca").c_str())->Clone());
    TH1F* m_trk_d0c_pos_eca=(TH1F*)(file->Get((path+"/trk_d0c_pos_eca").c_str())->Clone());
    TH1F* m_trk_d0c_asym_eca=(TH1F*)(file->Get((path+"/trk_d0c_asym_eca").c_str())->Clone());
    ProcessAsymHistograms(m_trk_d0c_neg_eca,m_trk_d0c_pos_eca,m_trk_d0c_asym_eca);
    m_trk_d0c_asym_eca->SetXTitle("track d_{0} [mm]");
    m_trk_d0c_asym_eca->Write("",TObject::kOverwrite);
  }
  if (CheckHistogram(file,(path+"/trk_d0c_asym").c_str()) && CheckHistogram(file,(path+"/trk_d0c_neg").c_str()) && CheckHistogram(file,(path+"/trk_d0c_pos").c_str())  ) {
    TH1F* m_trk_d0c_neg=(TH1F*)(file->Get((path+"/trk_d0c_neg").c_str())->Clone());
    TH1F* m_trk_d0c_pos=(TH1F*)(file->Get((path+"/trk_d0c_pos").c_str())->Clone());
    TH1F* m_trk_d0c_asym=(TH1F*)(file->Get((path+"/trk_d0c_asym").c_str())->Clone());
    ProcessAsymHistograms(m_trk_d0c_neg,m_trk_d0c_pos,m_trk_d0c_asym);
    m_trk_d0c_asym->SetXTitle("track d_{0} [mm]");
    m_trk_d0c_asym->Write("",TObject::kOverwrite);
  }
  //charge Asym vs z0 (corrected for vertex)
  if (CheckHistogram(file,(path+"/trk_z0c_asym_barrel").c_str()) && CheckHistogram(file,(path+"/trk_z0c_neg_barrel").c_str()) && CheckHistogram(file,(path+"/trk_z0c_pos_barrel").c_str())  ) {
    TH1F* m_trk_z0c_neg_barrel=(TH1F*)(file->Get((path+"/trk_z0c_neg_barrel").c_str())->Clone());
    TH1F* m_trk_z0c_pos_barrel=(TH1F*)(file->Get((path+"/trk_z0c_pos_barrel").c_str())->Clone());
    TH1F* m_trk_z0c_asym_barrel=(TH1F*)(file->Get((path+"/trk_z0c_asym_barrel").c_str())->Clone());
    ProcessAsymHistograms(m_trk_z0c_neg_barrel,m_trk_z0c_pos_barrel,m_trk_z0c_asym_barrel);
    m_trk_z0c_asym_barrel->SetXTitle("track z_{0} [mm]");
    m_trk_z0c_asym_barrel->Write("",TObject::kOverwrite);
  }
  if (CheckHistogram(file,(path+"/trk_z0c_asym_ecc").c_str()) && CheckHistogram(file,(path+"/trk_z0c_neg_ecc").c_str()) && CheckHistogram(file,(path+"/trk_z0c_pos_ecc").c_str())  ) {
    TH1F* m_trk_z0c_neg_ecc=(TH1F*)(file->Get((path+"/trk_z0c_neg_ecc").c_str())->Clone());
    TH1F* m_trk_z0c_pos_ecc=(TH1F*)(file->Get((path+"/trk_z0c_pos_ecc").c_str())->Clone());
    TH1F* m_trk_z0c_asym_ecc=(TH1F*)(file->Get((path+"/trk_z0c_asym_ecc").c_str())->Clone());
    ProcessAsymHistograms(m_trk_z0c_neg_ecc,m_trk_z0c_pos_ecc,m_trk_z0c_asym_ecc);
    m_trk_z0c_asym_ecc->SetXTitle("track z_{0} [mm]");
    m_trk_z0c_asym_ecc->Write("",TObject::kOverwrite);
  }
  if (CheckHistogram(file,(path+"/trk_z0c_asym_eca").c_str()) && CheckHistogram(file,(path+"/trk_z0c_neg_eca").c_str()) && CheckHistogram(file,(path+"/trk_z0c_pos_eca").c_str())  ) {
    TH1F* m_trk_z0c_neg_eca=(TH1F*)(file->Get((path+"/trk_z0c_neg_eca").c_str())->Clone());
    TH1F* m_trk_z0c_pos_eca=(TH1F*)(file->Get((path+"/trk_z0c_pos_eca").c_str())->Clone());
    TH1F* m_trk_z0c_asym_eca=(TH1F*)(file->Get((path+"/trk_z0c_asym_eca").c_str())->Clone());
    ProcessAsymHistograms(m_trk_z0c_neg_eca,m_trk_z0c_pos_eca,m_trk_z0c_asym_eca);
    m_trk_z0c_asym_eca->SetXTitle("track z_{0} [mm]");
    m_trk_z0c_asym_eca->Write("",TObject::kOverwrite);
  }
  if (CheckHistogram(file,(path+"/trk_z0c_asym").c_str()) && CheckHistogram(file,(path+"/trk_z0c_neg").c_str()) && CheckHistogram(file,(path+"/trk_z0c_pos").c_str())  ) {
    TH1F* m_trk_z0c_neg=(TH1F*)(file->Get((path+"/trk_z0c_neg").c_str())->Clone());
    TH1F* m_trk_z0c_pos=(TH1F*)(file->Get((path+"/trk_z0c_pos").c_str())->Clone());
    TH1F* m_trk_z0c_asym=(TH1F*)(file->Get((path+"/trk_z0c_asym").c_str())->Clone());
    ProcessAsymHistograms(m_trk_z0c_neg,m_trk_z0c_pos,m_trk_z0c_asym);
    m_trk_z0c_asym->SetXTitle("track z_{0} [mm]");
    m_trk_z0c_asym->Write("",TObject::kOverwrite);
  }
  file->Write();
  
}

void 
MonitoringFile::
fitMergedFile_IDAlignMonPVbiases (TFile* file, std::string run_dir, std::string tracksName) 
{
  std::string path;
  path= run_dir + "IDAlignMon/" + tracksName + "/GenericTracks/PVbiases";
  //path= run_dir + "IDAlignMon/InDetTrackParticles_all/GenericTracks";
  if(  file->cd(path.c_str())==0 ) {
      //std::cerr << "MonitoringFile::fitMergedFile_IDAlignMonGenericTracks(): "
      //      << "No such directory \"" << path << "\"\n";
    return;
  }


  //Maps vs phi_vs_eta vs eta
  if(CheckHistogram(file,(path+"/trk_d0_wrtPV_vs_phi_vs_eta_400MeV_600MeV_positive").c_str()) && CheckHistogram(file,(path+"/trk_d0_wrtPV_vs_phi_vs_eta_400MeV_600MeV_negative").c_str()) ) {
	TH3F* m_trk_d0_wrtPV_vs_phi_vs_eta_400MeV_600MeV_positive=(TH3F*)(file->Get((path+"/trk_d0_wrtPV_vs_phi_vs_eta_400MeV_600MeV_positive").c_str())->Clone());
	TH3F* m_trk_d0_wrtPV_vs_phi_vs_eta_400MeV_600MeV_negative=(TH3F*)(file->Get((path+"/trk_d0_wrtPV_vs_phi_vs_eta_400MeV_600MeV_negative").c_str())->Clone());
	
	TH2F* m_trk_d0_wrtPV_map_vs_phi_vs_eta_400MeV_600MeV_positive = new TH2F("trk_d0_wrtPV_map_vs_phi_vs_eta_400MeV_600MeV_positive", "map d0 vs phi_vs_eta 400MeV-600MeV positive; #phi; #eta" , 20, -3.1, 3.1, 20, -2.5, 2.5 );
	TH2F* m_trk_d0_wrtPV_map_vs_phi_vs_eta_400MeV_600MeV_negative = new TH2F("trk_d0_wrtPV_map_vs_phi_vs_eta_400MeV_600MeV_negative", "map d0 vs phi_vs_eta 400MeV-600MeV negative; #phi; #eta" , 20, -3.1, 3.1, 20, -2.5, 2.5 );
	
	MakeMap(m_trk_d0_wrtPV_map_vs_phi_vs_eta_400MeV_600MeV_positive,m_trk_d0_wrtPV_vs_phi_vs_eta_400MeV_600MeV_positive);
	MakeMap(m_trk_d0_wrtPV_map_vs_phi_vs_eta_400MeV_600MeV_negative,m_trk_d0_wrtPV_vs_phi_vs_eta_400MeV_600MeV_negative);

 	m_trk_d0_wrtPV_map_vs_phi_vs_eta_400MeV_600MeV_positive->Write("",TObject::kOverwrite);
	m_trk_d0_wrtPV_map_vs_phi_vs_eta_400MeV_600MeV_negative->Write("",TObject::kOverwrite);
  }

  if(CheckHistogram(file,(path+"/trk_d0_wrtPV_vs_phi_vs_eta_600MeV_1GeV_positive").c_str()) && CheckHistogram(file,(path+"/trk_d0_wrtPV_vs_phi_vs_eta_600MeV_1GeV_negative").c_str()) ) {
	TH3F* m_trk_d0_wrtPV_vs_phi_vs_eta_600MeV_1GeV_positive=(TH3F*)(file->Get((path+"/trk_d0_wrtPV_vs_phi_vs_eta_600MeV_1GeV_positive").c_str())->Clone());
	TH3F* m_trk_d0_wrtPV_vs_phi_vs_eta_600MeV_1GeV_negative=(TH3F*)(file->Get((path+"/trk_d0_wrtPV_vs_phi_vs_eta_600MeV_1GeV_negative").c_str())->Clone());
	
	TH2F* m_trk_d0_wrtPV_map_vs_phi_vs_eta_600MeV_1GeV_positive = new TH2F("trk_d0_wrtPV_map_vs_phi_vs_eta_600MeV_1GeV_positive", "map d0 vs phi_vs_eta 600MeV-1GeV positive; #phi; #eta" , 20, -3.1, 3.1, 20, -2.5, 2.5 );
	TH2F* m_trk_d0_wrtPV_map_vs_phi_vs_eta_600MeV_1GeV_negative = new TH2F("trk_d0_wrtPV_map_vs_phi_vs_eta_600MeV_1GeV_negative", "map d0 vs phi_vs_eta 600MeV-1GeV negative; #phi; #eta" , 20, -3.1, 3.1, 20, -2.5, 2.5 );
	
	MakeMap(m_trk_d0_wrtPV_map_vs_phi_vs_eta_600MeV_1GeV_positive,m_trk_d0_wrtPV_vs_phi_vs_eta_600MeV_1GeV_positive);
	MakeMap(m_trk_d0_wrtPV_map_vs_phi_vs_eta_600MeV_1GeV_negative,m_trk_d0_wrtPV_vs_phi_vs_eta_600MeV_1GeV_negative);

 	m_trk_d0_wrtPV_map_vs_phi_vs_eta_600MeV_1GeV_positive->Write("",TObject::kOverwrite);
	m_trk_d0_wrtPV_map_vs_phi_vs_eta_600MeV_1GeV_negative->Write("",TObject::kOverwrite);
  }

  if(CheckHistogram(file,(path+"/trk_d0_wrtPV_vs_phi_vs_eta_1GeV_2GeV_positive").c_str()) && CheckHistogram(file,(path+"/trk_d0_wrtPV_vs_phi_vs_eta_1GeV_2GeV_negative").c_str()) ) {
	TH3F* m_trk_d0_wrtPV_vs_phi_vs_eta_1GeV_2GeV_positive=(TH3F*)(file->Get((path+"/trk_d0_wrtPV_vs_phi_vs_eta_1GeV_2GeV_positive").c_str())->Clone());
	TH3F* m_trk_d0_wrtPV_vs_phi_vs_eta_1GeV_2GeV_negative=(TH3F*)(file->Get((path+"/trk_d0_wrtPV_vs_phi_vs_eta_1GeV_2GeV_negative").c_str())->Clone());
	
	TH2F* m_trk_d0_wrtPV_map_vs_phi_vs_eta_1GeV_2GeV_positive = new TH2F("trk_d0_wrtPV_map_vs_phi_vs_eta_1GeV_2GeV_positive", "map d0 vs phi_vs_eta 1GeV-2GeV positive; #phi; #eta" , 20, -3.1, 3.1, 20, -2.5, 2.5 );
	TH2F* m_trk_d0_wrtPV_map_vs_phi_vs_eta_1GeV_2GeV_negative = new TH2F("trk_d0_wrtPV_map_vs_phi_vs_eta_1GeV_2GeV_negative", "map d0 vs phi_vs_eta 1GeV-2GeV negative; #phi; #eta" , 20, -3.1, 3.1, 20, -2.5, 2.5 );
	
	MakeMap(m_trk_d0_wrtPV_map_vs_phi_vs_eta_1GeV_2GeV_positive,m_trk_d0_wrtPV_vs_phi_vs_eta_1GeV_2GeV_positive);
	MakeMap(m_trk_d0_wrtPV_map_vs_phi_vs_eta_1GeV_2GeV_negative,m_trk_d0_wrtPV_vs_phi_vs_eta_1GeV_2GeV_negative);

 	m_trk_d0_wrtPV_map_vs_phi_vs_eta_1GeV_2GeV_positive->Write("",TObject::kOverwrite);
	m_trk_d0_wrtPV_map_vs_phi_vs_eta_1GeV_2GeV_negative->Write("",TObject::kOverwrite);
  }

  if(CheckHistogram(file,(path+"/trk_d0_wrtPV_vs_phi_vs_eta_2GeV_5GeV_positive").c_str()) && CheckHistogram(file,(path+"/trk_d0_wrtPV_vs_phi_vs_eta_2GeV_5GeV_negative").c_str()) ) {
	TH3F* m_trk_d0_wrtPV_vs_phi_vs_eta_2GeV_5GeV_positive=(TH3F*)(file->Get((path+"/trk_d0_wrtPV_vs_phi_vs_eta_2GeV_5GeV_positive").c_str())->Clone());
	TH3F* m_trk_d0_wrtPV_vs_phi_vs_eta_2GeV_5GeV_negative=(TH3F*)(file->Get((path+"/trk_d0_wrtPV_vs_phi_vs_eta_2GeV_5GeV_negative").c_str())->Clone());
	
	TH2F* m_trk_d0_wrtPV_map_vs_phi_vs_eta_2GeV_5GeV_positive = new TH2F("trk_d0_wrtPV_map_vs_phi_vs_eta_2GeV_5GeV_positive", "map d0 vs phi_vs_eta 2GeV-5GeV positive; #phi; #eta" , 20, -3.1, 3.1, 20, -2.5, 2.5 );
	TH2F* m_trk_d0_wrtPV_map_vs_phi_vs_eta_2GeV_5GeV_negative = new TH2F("trk_d0_wrtPV_map_vs_phi_vs_eta_2GeV_5GeV_negative", "map d0 vs phi_vs_eta 2GeV-5GeV negative; #phi; #eta" , 20, -3.1, 3.1, 20, -2.5, 2.5 );
	
	MakeMap(m_trk_d0_wrtPV_map_vs_phi_vs_eta_2GeV_5GeV_positive,m_trk_d0_wrtPV_vs_phi_vs_eta_2GeV_5GeV_positive);
	MakeMap(m_trk_d0_wrtPV_map_vs_phi_vs_eta_2GeV_5GeV_negative,m_trk_d0_wrtPV_vs_phi_vs_eta_2GeV_5GeV_negative);

 	m_trk_d0_wrtPV_map_vs_phi_vs_eta_2GeV_5GeV_positive->Write("",TObject::kOverwrite);
	m_trk_d0_wrtPV_map_vs_phi_vs_eta_2GeV_5GeV_negative->Write("",TObject::kOverwrite);
  }

  if(CheckHistogram(file,(path+"/trk_d0_wrtPV_vs_phi_vs_eta_5GeV_10GeV_positive").c_str()) && CheckHistogram(file,(path+"/trk_d0_wrtPV_vs_phi_vs_eta_5GeV_10GeV_negative").c_str()) ) {
	TH3F* m_trk_d0_wrtPV_vs_phi_vs_eta_5GeV_10GeV_positive=(TH3F*)(file->Get((path+"/trk_d0_wrtPV_vs_phi_vs_eta_5GeV_10GeV_positive").c_str())->Clone());
	TH3F* m_trk_d0_wrtPV_vs_phi_vs_eta_5GeV_10GeV_negative=(TH3F*)(file->Get((path+"/trk_d0_wrtPV_vs_phi_vs_eta_5GeV_10GeV_negative").c_str())->Clone());
	
	TH2F* m_trk_d0_wrtPV_map_vs_phi_vs_eta_5GeV_10GeV_positive = new TH2F("trk_d0_wrtPV_map_vs_phi_vs_eta_5GeV_10GeV_positive", "map d0 vs phi_vs_eta 5GeV-10GeV positive; #phi; #eta" , 20, -3.1, 3.1, 20, -2.5, 2.5 );
	TH2F* m_trk_d0_wrtPV_map_vs_phi_vs_eta_5GeV_10GeV_negative = new TH2F("trk_d0_wrtPV_map_vs_phi_vs_eta_5GeV_10GeV_negative", "map d0 vs phi_vs_eta 5GeV-10GeV negative; #phi; #eta" , 20, -3.1, 3.1, 20, -2.5, 2.5 );
	
	MakeMap(m_trk_d0_wrtPV_map_vs_phi_vs_eta_5GeV_10GeV_positive,m_trk_d0_wrtPV_vs_phi_vs_eta_5GeV_10GeV_positive);
	MakeMap(m_trk_d0_wrtPV_map_vs_phi_vs_eta_5GeV_10GeV_negative,m_trk_d0_wrtPV_vs_phi_vs_eta_5GeV_10GeV_negative);

 	m_trk_d0_wrtPV_map_vs_phi_vs_eta_5GeV_10GeV_positive->Write("",TObject::kOverwrite);
	m_trk_d0_wrtPV_map_vs_phi_vs_eta_5GeV_10GeV_negative->Write("",TObject::kOverwrite);
  }

  if(CheckHistogram(file,(path+"/trk_d0_wrtPV_vs_phi_vs_eta_10GeV_positive").c_str()) && CheckHistogram(file,(path+"/trk_d0_wrtPV_vs_phi_vs_eta_10GeV_negative").c_str()) ) {
	TH3F* m_trk_d0_wrtPV_vs_phi_vs_eta_10GeV_positive=(TH3F*)(file->Get((path+"/trk_d0_wrtPV_vs_phi_vs_eta_10GeV_positive").c_str())->Clone());
	TH3F* m_trk_d0_wrtPV_vs_phi_vs_eta_10GeV_negative=(TH3F*)(file->Get((path+"/trk_d0_wrtPV_vs_phi_vs_eta_10GeV_negative").c_str())->Clone());
	
	TH2F* m_trk_d0_wrtPV_map_vs_phi_vs_eta_10GeV_positive = new TH2F("trk_d0_wrtPV_map_vs_phi_vs_eta_10GeV_positive", "map d0 vs phi_vs_eta >10GeV positive; #phi; #eta" , 20, -3.1, 3.1, 20, -2.5, 2.5 );
	TH2F* m_trk_d0_wrtPV_map_vs_phi_vs_eta_10GeV_negative = new TH2F("trk_d0_wrtPV_map_vs_phi_vs_eta_10GeV_negative", "map d0 vs phi_vs_eta >10GeV negative; #phi; #eta" , 20, -3.1, 3.1, 20, -2.5, 2.5 );
	
	MakeMap(m_trk_d0_wrtPV_map_vs_phi_vs_eta_10GeV_positive,m_trk_d0_wrtPV_vs_phi_vs_eta_10GeV_positive);
	MakeMap(m_trk_d0_wrtPV_map_vs_phi_vs_eta_10GeV_negative,m_trk_d0_wrtPV_vs_phi_vs_eta_10GeV_negative);

 	m_trk_d0_wrtPV_map_vs_phi_vs_eta_10GeV_positive->Write("",TObject::kOverwrite);
	m_trk_d0_wrtPV_map_vs_phi_vs_eta_10GeV_negative->Write("",TObject::kOverwrite);
  }

  if(CheckHistogram(file,(path+"/trk_d0_wrtPV_vs_phi_vs_eta_2GeV").c_str()) ) {
	TH3F* m_trk_d0_wrtPV_vs_phi_vs_eta_2GeV=(TH3F*)(file->Get((path+"/trk_d0_wrtPV_vs_phi_vs_eta_2GeV").c_str())->Clone());
	
	TH2F* m_trk_d0_wrtPV_map_vs_phi_vs_eta_2GeV = new TH2F("trk_d0_wrtPV_map_vs_phi_vs_eta_2GeV", "map d0 vs phi_vs_eta >2GeV; #phi; #eta" , 20, -3.1, 3.1, 20, -2.5, 2.5 );
	
	MakeMap(m_trk_d0_wrtPV_map_vs_phi_vs_eta_2GeV,m_trk_d0_wrtPV_vs_phi_vs_eta_2GeV);

 	m_trk_d0_wrtPV_map_vs_phi_vs_eta_2GeV->Write("",TObject::kOverwrite);
  }

  
  //Profiles vs phi
  if(CheckHistogram(file,(path+"/trk_d0_wrtPV_vs_phi_400MeV_600MeV_positive").c_str()) && CheckHistogram(file,(path+"/trk_d0_wrtPV_vs_phi_400MeV_600MeV_negative").c_str()) ) {
	TH2F* m_trk_d0_wrtPV_vs_phi_400MeV_600MeV_positive=(TH2F*)(file->Get((path+"/trk_d0_wrtPV_vs_phi_400MeV_600MeV_positive").c_str())->Clone());
	TH2F* m_trk_d0_wrtPV_vs_phi_400MeV_600MeV_negative=(TH2F*)(file->Get((path+"/trk_d0_wrtPV_vs_phi_400MeV_600MeV_negative").c_str())->Clone());
	
	TH1F* m_trk_d0_wrtPV_profile_vs_phi_400MeV_600MeV_positive = new TH1F("trk_d0_wrtPV_profile_vs_phi_400MeV_600MeV_positive", "profile d0 vs phi 400MeV-600MeV positive; #phi; d0 [mm]" , 50, -3.1, 3.1 );
	TH1F* m_trk_d0_wrtPV_profile_vs_phi_400MeV_600MeV_negative = new TH1F("trk_d0_wrtPV_profile_vs_phi_400MeV_600MeV_negative", "profile d0 vs phi 400MeV-600MeV negative; #phi; d0 [mm]" , 50, -3.1, 3.1 );
	
	Make1DProfile(m_trk_d0_wrtPV_profile_vs_phi_400MeV_600MeV_positive,m_trk_d0_wrtPV_vs_phi_400MeV_600MeV_positive);
	Make1DProfile(m_trk_d0_wrtPV_profile_vs_phi_400MeV_600MeV_negative,m_trk_d0_wrtPV_vs_phi_400MeV_600MeV_negative);

 	m_trk_d0_wrtPV_profile_vs_phi_400MeV_600MeV_positive->Write("",TObject::kOverwrite);
	m_trk_d0_wrtPV_profile_vs_phi_400MeV_600MeV_negative->Write("",TObject::kOverwrite);
  }

  if(CheckHistogram(file,(path+"/trk_d0_wrtPV_vs_phi_600MeV_1GeV_positive").c_str()) && CheckHistogram(file,(path+"/trk_d0_wrtPV_vs_phi_600MeV_1GeV_negative").c_str()) ) {
	TH2F* m_trk_d0_wrtPV_vs_phi_600MeV_1GeV_positive=(TH2F*)(file->Get((path+"/trk_d0_wrtPV_vs_phi_600MeV_1GeV_positive").c_str())->Clone());
	TH2F* m_trk_d0_wrtPV_vs_phi_600MeV_1GeV_negative=(TH2F*)(file->Get((path+"/trk_d0_wrtPV_vs_phi_600MeV_1GeV_negative").c_str())->Clone());
	
	TH1F* m_trk_d0_wrtPV_profile_vs_phi_600MeV_1GeV_positive = new TH1F("trk_d0_wrtPV_profile_vs_phi_600MeV_1GeV_positive", "profile d0 vs phi 600MeV-1GeV positive; #phi; d0 [mm]" , 50, -3.1, 3.1 );
	TH1F* m_trk_d0_wrtPV_profile_vs_phi_600MeV_1GeV_negative = new TH1F("trk_d0_wrtPV_profile_vs_phi_600MeV_1GeV_negative", "profile d0 vs phi 600MeV-1GeV negative; #phi; d0 [mm]" , 50, -3.1, 3.1 );
	
	Make1DProfile(m_trk_d0_wrtPV_profile_vs_phi_600MeV_1GeV_positive,m_trk_d0_wrtPV_vs_phi_600MeV_1GeV_positive);
	Make1DProfile(m_trk_d0_wrtPV_profile_vs_phi_600MeV_1GeV_negative,m_trk_d0_wrtPV_vs_phi_600MeV_1GeV_negative);

 	m_trk_d0_wrtPV_profile_vs_phi_600MeV_1GeV_positive->Write("",TObject::kOverwrite);
	m_trk_d0_wrtPV_profile_vs_phi_600MeV_1GeV_negative->Write("",TObject::kOverwrite);
  }

  if(CheckHistogram(file,(path+"/trk_d0_wrtPV_vs_phi_1GeV_2GeV_positive").c_str()) && CheckHistogram(file,(path+"/trk_d0_wrtPV_vs_phi_1GeV_2GeV_negative").c_str()) ) {
	TH2F* m_trk_d0_wrtPV_vs_phi_1GeV_2GeV_positive=(TH2F*)(file->Get((path+"/trk_d0_wrtPV_vs_phi_1GeV_2GeV_positive").c_str())->Clone());
	TH2F* m_trk_d0_wrtPV_vs_phi_1GeV_2GeV_negative=(TH2F*)(file->Get((path+"/trk_d0_wrtPV_vs_phi_1GeV_2GeV_negative").c_str())->Clone());
	
	TH1F* m_trk_d0_wrtPV_profile_vs_phi_1GeV_2GeV_positive = new TH1F("trk_d0_wrtPV_profile_vs_phi_1GeV_2GeV_positive", "profile d0 vs phi 1GeV-2GeV positive; #phi; d0 [mm]" , 50, -3.1, 3.1 );
	TH1F* m_trk_d0_wrtPV_profile_vs_phi_1GeV_2GeV_negative = new TH1F("trk_d0_wrtPV_profile_vs_phi_1GeV_2GeV_negative", "profile d0 vs phi 1GeV-2GeV negative; #phi; d0 [mm]" , 50, -3.1, 3.1 );
	
	Make1DProfile(m_trk_d0_wrtPV_profile_vs_phi_1GeV_2GeV_positive,m_trk_d0_wrtPV_vs_phi_1GeV_2GeV_positive);
	Make1DProfile(m_trk_d0_wrtPV_profile_vs_phi_1GeV_2GeV_negative,m_trk_d0_wrtPV_vs_phi_1GeV_2GeV_negative);

 	m_trk_d0_wrtPV_profile_vs_phi_1GeV_2GeV_positive->Write("",TObject::kOverwrite);
	m_trk_d0_wrtPV_profile_vs_phi_1GeV_2GeV_negative->Write("",TObject::kOverwrite);
  }

  if(CheckHistogram(file,(path+"/trk_d0_wrtPV_vs_phi_2GeV_5GeV_positive").c_str()) && CheckHistogram(file,(path+"/trk_d0_wrtPV_vs_phi_2GeV_5GeV_negative").c_str()) ) {
	TH2F* m_trk_d0_wrtPV_vs_phi_2GeV_5GeV_positive=(TH2F*)(file->Get((path+"/trk_d0_wrtPV_vs_phi_2GeV_5GeV_positive").c_str())->Clone());
	TH2F* m_trk_d0_wrtPV_vs_phi_2GeV_5GeV_negative=(TH2F*)(file->Get((path+"/trk_d0_wrtPV_vs_phi_2GeV_5GeV_negative").c_str())->Clone());
	
	TH1F* m_trk_d0_wrtPV_profile_vs_phi_2GeV_5GeV_positive = new TH1F("trk_d0_wrtPV_profile_vs_phi_2GeV_5GeV_positive", "profile d0 vs phi 2GeV-5GeV positive; #phi; d0 [mm]" , 50, -3.1, 3.1 );
	TH1F* m_trk_d0_wrtPV_profile_vs_phi_2GeV_5GeV_negative = new TH1F("trk_d0_wrtPV_profile_vs_phi_2GeV_5GeV_negative", "profile d0 vs phi 2GeV-5GeV negative; #phi; d0 [mm]" , 50, -3.1, 3.1 );
	
	Make1DProfile(m_trk_d0_wrtPV_profile_vs_phi_2GeV_5GeV_positive,m_trk_d0_wrtPV_vs_phi_2GeV_5GeV_positive);
	Make1DProfile(m_trk_d0_wrtPV_profile_vs_phi_2GeV_5GeV_negative,m_trk_d0_wrtPV_vs_phi_2GeV_5GeV_negative);

 	m_trk_d0_wrtPV_profile_vs_phi_2GeV_5GeV_positive->Write("",TObject::kOverwrite);
	m_trk_d0_wrtPV_profile_vs_phi_2GeV_5GeV_negative->Write("",TObject::kOverwrite);
  }

  if(CheckHistogram(file,(path+"/trk_d0_wrtPV_vs_phi_5GeV_10GeV_positive").c_str()) && CheckHistogram(file,(path+"/trk_d0_wrtPV_vs_phi_5GeV_10GeV_negative").c_str()) ) {
	TH2F* m_trk_d0_wrtPV_vs_phi_5GeV_10GeV_positive=(TH2F*)(file->Get((path+"/trk_d0_wrtPV_vs_phi_5GeV_10GeV_positive").c_str())->Clone());
	TH2F* m_trk_d0_wrtPV_vs_phi_5GeV_10GeV_negative=(TH2F*)(file->Get((path+"/trk_d0_wrtPV_vs_phi_5GeV_10GeV_negative").c_str())->Clone());
	
	TH1F* m_trk_d0_wrtPV_profile_vs_phi_5GeV_10GeV_positive = new TH1F("trk_d0_wrtPV_profile_vs_phi_5GeV_10GeV_positive", "profile d0 vs phi 5GeV-10GeV positive; #phi; d0 [mm]" , 50, -3.1, 3.1 );
	TH1F* m_trk_d0_wrtPV_profile_vs_phi_5GeV_10GeV_negative = new TH1F("trk_d0_wrtPV_profile_vs_phi_5GeV_10GeV_negative", "profile d0 vs phi 5GeV-10GeV negative; #phi; d0 [mm]" , 50, -3.1, 3.1 );
	
	Make1DProfile(m_trk_d0_wrtPV_profile_vs_phi_5GeV_10GeV_positive,m_trk_d0_wrtPV_vs_phi_5GeV_10GeV_positive);
	Make1DProfile(m_trk_d0_wrtPV_profile_vs_phi_5GeV_10GeV_negative,m_trk_d0_wrtPV_vs_phi_5GeV_10GeV_negative);

 	m_trk_d0_wrtPV_profile_vs_phi_5GeV_10GeV_positive->Write("",TObject::kOverwrite);
	m_trk_d0_wrtPV_profile_vs_phi_5GeV_10GeV_negative->Write("",TObject::kOverwrite);
  }

  if(CheckHistogram(file,(path+"/trk_d0_wrtPV_vs_phi_10GeV_positive").c_str()) && CheckHistogram(file,(path+"/trk_d0_wrtPV_vs_phi_10GeV_negative").c_str()) ) {
	TH2F* m_trk_d0_wrtPV_vs_phi_10GeV_positive=(TH2F*)(file->Get((path+"/trk_d0_wrtPV_vs_phi_10GeV_positive").c_str())->Clone());
	TH2F* m_trk_d0_wrtPV_vs_phi_10GeV_negative=(TH2F*)(file->Get((path+"/trk_d0_wrtPV_vs_phi_10GeV_negative").c_str())->Clone());
	
	TH1F* m_trk_d0_wrtPV_profile_vs_phi_10GeV_positive = new TH1F("trk_d0_wrtPV_profile_vs_phi_10GeV_positive", "profile d0 vs phi >10GeV positive; #phi; d0 [mm]" , 50, -3.1, 3.1 );
	TH1F* m_trk_d0_wrtPV_profile_vs_phi_10GeV_negative = new TH1F("trk_d0_wrtPV_profile_vs_phi_10GeV_negative", "profile d0 vs phi >10GeV negative; #phi; d0 [mm]" , 50, -3.1, 3.1 );
	
	Make1DProfile(m_trk_d0_wrtPV_profile_vs_phi_10GeV_positive,m_trk_d0_wrtPV_vs_phi_10GeV_positive);
	Make1DProfile(m_trk_d0_wrtPV_profile_vs_phi_10GeV_negative,m_trk_d0_wrtPV_vs_phi_10GeV_negative);

 	m_trk_d0_wrtPV_profile_vs_phi_10GeV_positive->Write("",TObject::kOverwrite);
	m_trk_d0_wrtPV_profile_vs_phi_10GeV_negative->Write("",TObject::kOverwrite);
  }

  //Profiles vs eta
  if(CheckHistogram(file,(path+"/trk_d0_wrtPV_vs_eta_400MeV_600MeV_positive").c_str()) && CheckHistogram(file,(path+"/trk_d0_wrtPV_vs_eta_400MeV_600MeV_negative").c_str()) ) {
	TH2F* m_trk_d0_wrtPV_vs_eta_400MeV_600MeV_positive=(TH2F*)(file->Get((path+"/trk_d0_wrtPV_vs_eta_400MeV_600MeV_positive").c_str())->Clone());
	TH2F* m_trk_d0_wrtPV_vs_eta_400MeV_600MeV_negative=(TH2F*)(file->Get((path+"/trk_d0_wrtPV_vs_eta_400MeV_600MeV_negative").c_str())->Clone());
	
	TH1F* m_trk_d0_wrtPV_profile_vs_eta_400MeV_600MeV_positive = new TH1F("trk_d0_wrtPV_profile_vs_eta_400MeV_600MeV_positive", "profile d0 vs eta 400MeV-600MeV positive; #eta; d0 [mm]" , 50, -2.5, 2.5 );
	TH1F* m_trk_d0_wrtPV_profile_vs_eta_400MeV_600MeV_negative = new TH1F("trk_d0_wrtPV_profile_vs_eta_400MeV_600MeV_negative", "profile d0 vs eta 400MeV-600MeV negative; #eta; d0 [mm]" , 50, -2.5, 2.5 );
	
	Make1DProfile(m_trk_d0_wrtPV_profile_vs_eta_400MeV_600MeV_positive,m_trk_d0_wrtPV_vs_eta_400MeV_600MeV_positive);
	Make1DProfile(m_trk_d0_wrtPV_profile_vs_eta_400MeV_600MeV_negative,m_trk_d0_wrtPV_vs_eta_400MeV_600MeV_negative);

 	m_trk_d0_wrtPV_profile_vs_eta_400MeV_600MeV_positive->Write("",TObject::kOverwrite);
	m_trk_d0_wrtPV_profile_vs_eta_400MeV_600MeV_negative->Write("",TObject::kOverwrite);
  }

  if(CheckHistogram(file,(path+"/trk_d0_wrtPV_vs_eta_600MeV_1GeV_positive").c_str()) && CheckHistogram(file,(path+"/trk_d0_wrtPV_vs_eta_600MeV_1GeV_negative").c_str()) ) {
	TH2F* m_trk_d0_wrtPV_vs_eta_600MeV_1GeV_positive=(TH2F*)(file->Get((path+"/trk_d0_wrtPV_vs_eta_600MeV_1GeV_positive").c_str())->Clone());
	TH2F* m_trk_d0_wrtPV_vs_eta_600MeV_1GeV_negative=(TH2F*)(file->Get((path+"/trk_d0_wrtPV_vs_eta_600MeV_1GeV_negative").c_str())->Clone());
	
	TH1F* m_trk_d0_wrtPV_profile_vs_eta_600MeV_1GeV_positive = new TH1F("trk_d0_wrtPV_profile_vs_eta_600MeV_1GeV_positive", "profile d0 vs eta 600MeV-1GeV positive; #eta; d0 [mm]" , 50, -2.5, 2.5 );
	TH1F* m_trk_d0_wrtPV_profile_vs_eta_600MeV_1GeV_negative = new TH1F("trk_d0_wrtPV_profile_vs_eta_600MeV_1GeV_negative", "profile d0 vs eta 600MeV-1GeV negative; #eta; d0 [mm]" , 50, -2.5, 2.5 );
	
	Make1DProfile(m_trk_d0_wrtPV_profile_vs_eta_600MeV_1GeV_positive,m_trk_d0_wrtPV_vs_eta_600MeV_1GeV_positive);
	Make1DProfile(m_trk_d0_wrtPV_profile_vs_eta_600MeV_1GeV_negative,m_trk_d0_wrtPV_vs_eta_600MeV_1GeV_negative);

 	m_trk_d0_wrtPV_profile_vs_eta_600MeV_1GeV_positive->Write("",TObject::kOverwrite);
	m_trk_d0_wrtPV_profile_vs_eta_600MeV_1GeV_negative->Write("",TObject::kOverwrite);
  }

  if(CheckHistogram(file,(path+"/trk_d0_wrtPV_vs_eta_1GeV_2GeV_positive").c_str()) && CheckHistogram(file,(path+"/trk_d0_wrtPV_vs_eta_1GeV_2GeV_negative").c_str()) ) {
	TH2F* m_trk_d0_wrtPV_vs_eta_1GeV_2GeV_positive=(TH2F*)(file->Get((path+"/trk_d0_wrtPV_vs_eta_1GeV_2GeV_positive").c_str())->Clone());
	TH2F* m_trk_d0_wrtPV_vs_eta_1GeV_2GeV_negative=(TH2F*)(file->Get((path+"/trk_d0_wrtPV_vs_eta_1GeV_2GeV_negative").c_str())->Clone());
	
	TH1F* m_trk_d0_wrtPV_profile_vs_eta_1GeV_2GeV_positive = new TH1F("trk_d0_wrtPV_profile_vs_eta_1GeV_2GeV_positive", "profile d0 vs eta 1GeV-2GeV positive; #eta; d0 [mm]" , 50, -2.5, 2.5 );
	TH1F* m_trk_d0_wrtPV_profile_vs_eta_1GeV_2GeV_negative = new TH1F("trk_d0_wrtPV_profile_vs_eta_1GeV_2GeV_negative", "profile d0 vs eta 1GeV-2GeV negative; #eta; d0 [mm]" , 50, -2.5, 2.5 );
	
	Make1DProfile(m_trk_d0_wrtPV_profile_vs_eta_1GeV_2GeV_positive,m_trk_d0_wrtPV_vs_eta_1GeV_2GeV_positive);
	Make1DProfile(m_trk_d0_wrtPV_profile_vs_eta_1GeV_2GeV_negative,m_trk_d0_wrtPV_vs_eta_1GeV_2GeV_negative);

 	m_trk_d0_wrtPV_profile_vs_eta_1GeV_2GeV_positive->Write("",TObject::kOverwrite);
	m_trk_d0_wrtPV_profile_vs_eta_1GeV_2GeV_negative->Write("",TObject::kOverwrite);
  }

  if(CheckHistogram(file,(path+"/trk_d0_wrtPV_vs_eta_2GeV_5GeV_positive").c_str()) && CheckHistogram(file,(path+"/trk_d0_wrtPV_vs_eta_2GeV_5GeV_negative").c_str()) ) {
	TH2F* m_trk_d0_wrtPV_vs_eta_2GeV_5GeV_positive=(TH2F*)(file->Get((path+"/trk_d0_wrtPV_vs_eta_2GeV_5GeV_positive").c_str())->Clone());
	TH2F* m_trk_d0_wrtPV_vs_eta_2GeV_5GeV_negative=(TH2F*)(file->Get((path+"/trk_d0_wrtPV_vs_eta_2GeV_5GeV_negative").c_str())->Clone());
	
	TH1F* m_trk_d0_wrtPV_profile_vs_eta_2GeV_5GeV_positive = new TH1F("trk_d0_wrtPV_profile_vs_eta_2GeV_5GeV_positive", "profile d0 vs eta 2GeV-5GeV positive; #eta; d0 [mm]" , 50, -2.5, 2.5 );
	TH1F* m_trk_d0_wrtPV_profile_vs_eta_2GeV_5GeV_negative = new TH1F("trk_d0_wrtPV_profile_vs_eta_2GeV_5GeV_negative", "profile d0 vs eta 2GeV-5GeV negative; #eta; d0 [mm]" , 50, -2.5, 2.5 );
	
	Make1DProfile(m_trk_d0_wrtPV_profile_vs_eta_2GeV_5GeV_positive,m_trk_d0_wrtPV_vs_eta_2GeV_5GeV_positive);
	Make1DProfile(m_trk_d0_wrtPV_profile_vs_eta_2GeV_5GeV_negative,m_trk_d0_wrtPV_vs_eta_2GeV_5GeV_negative);

 	m_trk_d0_wrtPV_profile_vs_eta_2GeV_5GeV_positive->Write("",TObject::kOverwrite);
	m_trk_d0_wrtPV_profile_vs_eta_2GeV_5GeV_negative->Write("",TObject::kOverwrite);
  }

  if(CheckHistogram(file,(path+"/trk_d0_wrtPV_vs_eta_5GeV_10GeV_positive").c_str()) && CheckHistogram(file,(path+"/trk_d0_wrtPV_vs_eta_5GeV_10GeV_negative").c_str()) ) {
	TH2F* m_trk_d0_wrtPV_vs_eta_5GeV_10GeV_positive=(TH2F*)(file->Get((path+"/trk_d0_wrtPV_vs_eta_5GeV_10GeV_positive").c_str())->Clone());
	TH2F* m_trk_d0_wrtPV_vs_eta_5GeV_10GeV_negative=(TH2F*)(file->Get((path+"/trk_d0_wrtPV_vs_eta_5GeV_10GeV_negative").c_str())->Clone());
	
	TH1F* m_trk_d0_wrtPV_profile_vs_eta_5GeV_10GeV_positive = new TH1F("trk_d0_wrtPV_profile_vs_eta_5GeV_10GeV_positive", "profile d0 vs eta 5GeV-10GeV positive; #eta; d0 [mm]" , 50, -2.5, 2.5 );
	TH1F* m_trk_d0_wrtPV_profile_vs_eta_5GeV_10GeV_negative = new TH1F("trk_d0_wrtPV_profile_vs_eta_5GeV_10GeV_negative", "profile d0 vs eta 5GeV-10GeV negative; #eta; d0 [mm]" , 50, -2.5, 2.5 );
	
	Make1DProfile(m_trk_d0_wrtPV_profile_vs_eta_5GeV_10GeV_positive,m_trk_d0_wrtPV_vs_eta_5GeV_10GeV_positive);
	Make1DProfile(m_trk_d0_wrtPV_profile_vs_eta_5GeV_10GeV_negative,m_trk_d0_wrtPV_vs_eta_5GeV_10GeV_negative);

 	m_trk_d0_wrtPV_profile_vs_eta_5GeV_10GeV_positive->Write("",TObject::kOverwrite);
	m_trk_d0_wrtPV_profile_vs_eta_5GeV_10GeV_negative->Write("",TObject::kOverwrite);
  }

  if(CheckHistogram(file,(path+"/trk_d0_wrtPV_vs_eta_10GeV_positive").c_str()) && CheckHistogram(file,(path+"/trk_d0_wrtPV_vs_eta_10GeV_negative").c_str()) ) {
	TH2F* m_trk_d0_wrtPV_vs_eta_10GeV_positive=(TH2F*)(file->Get((path+"/trk_d0_wrtPV_vs_eta_10GeV_positive").c_str())->Clone());
	TH2F* m_trk_d0_wrtPV_vs_eta_10GeV_negative=(TH2F*)(file->Get((path+"/trk_d0_wrtPV_vs_eta_10GeV_negative").c_str())->Clone());
	
	TH1F* m_trk_d0_wrtPV_profile_vs_eta_10GeV_positive = new TH1F("trk_d0_wrtPV_profile_vs_eta_10GeV_positive", "profile d0 vs eta >10GeV positive; #eta; d0 [mm]" , 50, -2.5, 2.5 );
	TH1F* m_trk_d0_wrtPV_profile_vs_eta_10GeV_negative = new TH1F("trk_d0_wrtPV_profile_vs_eta_10GeV_negative", "profile d0 vs eta >10GeV negative; #eta; d0 [mm]" , 50, -2.5, 2.5 );
	
	Make1DProfile(m_trk_d0_wrtPV_profile_vs_eta_10GeV_positive,m_trk_d0_wrtPV_vs_eta_10GeV_positive);
	Make1DProfile(m_trk_d0_wrtPV_profile_vs_eta_10GeV_negative,m_trk_d0_wrtPV_vs_eta_10GeV_negative);

 	m_trk_d0_wrtPV_profile_vs_eta_10GeV_positive->Write("",TObject::kOverwrite);
	m_trk_d0_wrtPV_profile_vs_eta_10GeV_negative->Write("",TObject::kOverwrite);
  }

  file->Write();
}

void 
MonitoringFile::
Make1DProfile(TH1* output, TH2* histo) 
{   
   int nXbins = histo->GetXaxis()->GetNbins(); //NEED TO CHANGE THIS 
   
   double current_mu, current_err_mu, current_sigma, current_err_sigma;

   for(int i=0;i<nXbins;i++) {
   	TH1D * projection = histo->ProjectionY("projection",i+1,i+1);
   	IterativeGaussFit(projection, current_mu, current_err_mu, current_sigma, current_err_sigma);
        
 	output->SetBinContent(i,current_mu);
        output->SetBinError(i,current_err_mu);
   }
}

void
MonitoringFile::
MakeMap(TH2* outputhist, TH3* hist) 
{
  int num_bins=1; //WHAT IS THIS AGAIN
 
  int num_bins_x = hist->GetXaxis()->GetNbins();
  int num_bins_y = hist->GetYaxis()->GetNbins();

  TH1D* current_proj;

  for (int i = 1; i < num_bins_x+(num_bins==1); i+=num_bins) {
  	for (int j = 1; j < num_bins_y+(num_bins==1); j+=num_bins) {

    		int index = i/num_bins;
    		int index_y = j/num_bins;

    		current_proj = hist->ProjectionZ(Form("%s_GaussProjection_%i_%i",hist->GetName(),index, index_y),i,i+num_bins-1,j,j+num_bins-1);
    		current_proj->SetTitle(Form("%s - Bin %i x %i",hist->GetName(), index,index_y));

		//Fit the gauss
     		double current_mu,current_err_mu, current_sigma, current_err_sigma;
 		IterativeGaussFit(current_proj, current_mu, current_err_mu, current_sigma, current_err_sigma);

               //Fill the maps
	     	float x_coord = (hist->GetXaxis()->GetBinLowEdge(i) + hist->GetXaxis()->GetBinUpEdge(i+num_bins-1))/2;
    		float y_coord = (hist->GetYaxis()->GetBinLowEdge(j) + hist->GetYaxis()->GetBinUpEdge(j+num_bins-1))/2;

    		outputhist->Fill(x_coord,y_coord,current_mu);
                
                delete current_proj;
    
  	} //end loop on j (y)
  } //end loop on i (x)
}

int
MonitoringFile::
IterativeGaussFit(TH1* hist, double &mu, double &mu_err, double &sigma, double &sigma_err) 
{
  //constants for fitting algorithm
  const int iteration_limit = 20;
  const float percent_limit = 0.01;
  const float fit_range_multiplier = 1.5;
  const int fDebug = 0;
  
  double last_mu;
  double last_sigma;
  double current_mu;
  double current_sigma;
  double mu_percent_diff;
  double sigma_percent_diff;
  
  if (!hist) {
    if (fDebug) std::cout<< "Error in  Anp::IterativeGaussFit(): Histogram to be fit is missing" <<std::endl;
    return 1;
  }  
  
  TF1* fit_func = new TF1("fit_func","gaus");

  int bad_fit = hist->Fit(fit_func,"QN");

  if (fDebug && bad_fit) std::cout <<"BAD INITIAL FIT: "<< hist->GetTitle() << std::endl;

  last_mu = fit_func->GetParameter(1); 
  last_sigma = fit_func->GetParameter(2);

  if (bad_fit) last_mu = hist->GetMean();

  // check as well that the last_mu is reasonable
  if (fabs(last_mu - hist->GetMean()) > 5*hist->GetBinWidth(1)) last_mu =  hist->GetMean();  

  fit_func->SetRange(last_mu-fit_range_multiplier*last_sigma,last_mu+fit_range_multiplier*last_sigma);

  int iteration = 0;

  while ( iteration < iteration_limit ) {

    iteration++;

    double FitRangeLower = last_mu-fit_range_multiplier*last_sigma;
    double FitRangeUpper = last_mu+fit_range_multiplier*last_sigma;

    // if range is to narrow --> broaden it
    if ((FitRangeUpper-FitRangeLower)/hist->GetBinWidth(1) < 4) {
      FitRangeLower -= hist->GetBinWidth(1);
      FitRangeUpper += hist->GetBinWidth(1);
    }

    fit_func->SetRange(FitRangeLower, FitRangeUpper);

    bad_fit = hist->Fit(fit_func, "RQN");

    if (fDebug && bad_fit) std::cout<<" ** BAD FIT ** : bin "<< hist->GetTitle() <<"  iteration "<<iteration<<std::endl;

    current_mu = fit_func->GetParameter(1); 
    current_sigma = fit_func->GetParameter(2);

    float average_mu = (last_mu+current_mu)/2;
    float average_sigma = (last_sigma+current_sigma)/2;

    if (average_mu == 0) {
      if ( fDebug ) std::cout<<" Average mu = 0 in bin "<< hist->GetTitle() <<std::endl;
      average_mu = current_mu;
    } 

    if (average_sigma == 0) {
      if ( fDebug ) std::cout<<"Average sigma = 0; Fit Problem in "<< hist->GetTitle() <<". "<<last_sigma<<" "<<current_sigma<<std::endl;
      average_sigma = current_sigma;
    }

    mu_percent_diff = fabs((last_mu-current_mu)/average_mu);
    sigma_percent_diff = fabs((last_sigma-current_sigma)/average_sigma);

    if ( mu_percent_diff < percent_limit && sigma_percent_diff < percent_limit ) break;

    if (iteration != iteration_limit) { //necessary?
      last_mu = current_mu;
      last_sigma = current_sigma;
    }    
    // check as well that the last_mu is reasonable
    if (fabs(last_mu - hist->GetMean()) > 5*hist->GetBinWidth(1)) {
      last_mu =  hist->GetMean();  
    } 
  }

  if (iteration == iteration_limit) {
    if (fDebug ) std::cout<<"WARNING: Fit did not converge to < "<<percent_limit*100<<"% in "<< hist->GetTitle() <<" in "<<iteration_limit<<" iterations. Percent Diffs: "<<mu_percent_diff*100<<"% (Mu),"<<" "<<sigma_percent_diff*100<<"% (Sigma)"<<std::endl;
    //possibly return a value other than 0 to indicate not converged?
  }

  mu = current_mu;
  mu_err = fit_func->GetParError(1);
  sigma = current_sigma;
  sigma_err = fit_func->GetParError(2);
  fit_func->SetLineColor(kRed);
 
  hist->GetListOfFunctions()->Add(fit_func);

  return 0;
}

void 
MonitoringFile::
ProcessAsymHistograms(TH1F* m_neg, TH1F* m_pos, TH1F* m_asym)
{
  if (m_neg->GetNbinsX()!=m_pos->GetNbinsX()||m_neg->GetNbinsX()!=m_asym->GetNbinsX()) return;
  for (int i=1;i<=m_neg->GetNbinsX();i++) {
    float asym=0;
    float err=0;
    float a=m_neg->GetBinContent(i);
    float b=m_pos->GetBinContent(i);
    float e1=m_neg->GetBinError(i);
    float e2=m_pos->GetBinError(i);
    if ((a+b)>0)
      { asym=(a-b)/(a+b);
      //error propagation
      err=sqrt((4*b*b*e1*e1+4*a*a*e2*e2)/pow(a+b,4));   }       
    m_asym->SetBinContent(i,asym);
    m_asym->SetBinError(i,err);    
  }
}


void 
MonitoringFile::
meanRMSProjections2D(TH2F* h2d, TH1F* h, int iopt)
{

  //fills a 1-D histogram with mean or width of the distribution for each bin in a 2D histogram
  //the way in which the mean or width is determined is specified by the iopt option
  //can be from a gaussian fit for example
  // iopt = 0; fills with statistical mean
  // iopt = 1; fills with statistical RMS
  // iopt = 2; fills with mean of a two-stage Gaussian fit
  // iopt = 3; fills with width of a two-stage Gaussian fit
  // iopt = 4; fills with Chi2/DOF of a two-stage Gaussian fit
  // iopt = 5; fills with nentries in the bin


  //std::cout << "In fillGaussianMeanOrWidth for " << h->GetName() << std::endl;

  //calling this means that the histogram bin content is flagged 
  //as being an average and so adding histos from different jobs 
  //will produce weighted mean
  h->SetBit(TH1::kIsAverage);

  int nbins_2d = h2d->GetNbinsX();

  // Removed by Alex
  //int nbins_h = h->GetNbinsX();
  //if(nbins_2d!=nbins_h) std::cout << "Mean/Width Histograms not set up correctly - nbins mismatch for " << h->GetName() << std::endl;

  for(int i = 1; i!=nbins_2d+1; i++){

    //std::cout << "Gaussian: processing bin " << i << std::endl;

    TH1F* hproj = (TH1F*)h2d->ProjectionY("proj",i,i,"e");

    //std::cout << "Gaussian: made projection for bin " << i << std::endl;

    //do not fill if there are few entries in the bin
    if(iopt<5 && (hproj->GetEntries()<=30 || hproj->Integral()<=30)) {
	delete hproj;
	//h->SetBinContent(i,j,0);
	//h->SetBinError(i,j,0);
	continue;
      }

    if(iopt==0){
	h->SetBinContent(i,hproj->GetMean());
	h->SetBinError(i,hproj->GetMeanError());
      }
      else if(iopt==1){
	h->SetBinContent(i,hproj->GetRMS());
	h->SetBinError(i,hproj->GetRMSError());
      }
      else if(iopt>=2 && iopt<5){

	//we make a 2-stage Gaussian fit
	//first fit is in the range [MEAN + 2*RMS, MEAN - 2*RMS]
	//(restricting the range of the first fit has little effect)
	//second fit is in the range [mean + 2*width, mean - 2*width]
	//using the mean and width of the first fit

	//first fit
	float _mean = hproj->GetMean();
	float _rms  = hproj->GetRMS();
	float _min = _mean - 2*_rms;
	float _max = _mean + 2*_rms;
	TF1 *fW = new TF1("fW","gaus",_min,_max);
	hproj -> Fit("fW", "RQN");
	
	//second fit
	_mean = fW->GetParameter(1);
	_rms  = fW->GetParameter(2);
	_min = _mean - 2*_rms;
	_max = _mean + 2*_rms;
	//if bins are large relative to distribution then 
	//the above determined range could only be two bins
	//or even just fractions of bins
	//in this case a gauus fit is meaningless, and can have strange
	//chi2 behaviour and large errors on the mean
	//therefore make sure we are including full bins in fit and 
	//have a minimum of 3 bins
	int _minBin = hproj->FindBin(_min);
	int _maxBin = hproj->FindBin(_max);
	if(_maxBin - _minBin < 2) {
	  //require at least 3 bins for a meaningful gauss fit
	  //std::cout << "WARNING core had too few bins for fit - expanding core for " << h->GetName() << std::endl;
	  _maxBin = _maxBin + 1;
	  _minBin = _minBin - 1;
	}
	_min = hproj->GetBinLowEdge(_minBin);
	_max = (hproj->GetBinLowEdge(_maxBin))+(hproj->GetBinWidth(_maxBin));

	TF1 *fW2 = new TF1("fW2","gaus",_min,_max);
	hproj -> Fit("fW2", "RQN");   
	if(iopt==2){
	  h->SetBinContent(i,fW2->GetParameter(1));
	  h->SetBinError(i,fW2->GetParError(1));
	} else if (iopt==3){
	  h->SetBinContent(i,fW2->GetParameter(2));
	  h->SetBinError(i,fW2->GetParError(2));
	} else if (iopt==4){
	  float _chi2 = fW2->GetChisquare();
	  int _ndf = fW2->GetNDF();
	  if(_ndf) h->SetBinContent(i,_chi2/_ndf);
	  else h->SetBinContent(i,0);
	}
	delete fW;
	delete fW2;
      }
      else if(iopt==5){
	h->SetBinContent(i,hproj->GetEntries());
      }
    //else std::cerr << "Incorrect iopt switch in meanRMSProjections2D()" << std::endl;

    delete hproj;
  }


  return;

}

void MonitoringFile::
meanRMSProjections3D(TH3F* h3d, TH2F* h,int iopt)
{

  //fills a 2-D histogram with mean or width of the distribution for each bin in a 3D histogram
  //the way in which the mean or width is determined is specified by the iopt option
  //can be from a gaussian fit for example
  // iopt = 0; fills with statistical mean
  // iopt = 1; fills with statistical RMS
  // iopt = 2; fills with mean of a two-stage Gaussian fit
  // iopt = 3; fills with width of a two-stage Gaussian fit
  // iopt = 4; fills with Chi2/DOF of a two-stage Gaussian fit
  // iopt = 5; fills with nentries in the bin

  int nbins_3dX = h3d->GetNbinsX();
  int nbins_3dY = h3d->GetNbinsY();
  int nbins_hX = h->GetNbinsX();
  int nbins_hY = h->GetNbinsY();

  if(nbins_3dX!=nbins_hX) {
      std::cout << "meanRMSProjections3D: Mean/RMS Histograms not set up correctly - nbins mismatch for " << h->GetName() << std::endl;
    return;
  }
  if(nbins_3dY!=nbins_hY) {
      std::cout << "meanRMSProjections3D: Mean/RMS Histograms not set up correctly - nbins mismatch for " << h->GetName() << std::endl;
    return;
  }

  for(int i = 1; i!=nbins_3dX+1; i++){
    for(int j = 1; j!=nbins_3dY+1; j++){

      
      TH1F* hproj = (TH1F*)h3d->ProjectionZ("proj",i,i,j,j,"e");
      
      //You can use this piece of code as snippet in order to produce fancy single modules residuals.
      //TCanvas Canvas;
      //std::string eta_mod="",phi_mod="";
      //std::string histoName = h3d->GetName();
      //if (histoName.find("pix_b0") == std::string::npos)
      //continue;
      //std::ostringstream convert_eta,convert_phi;   // stream used for the conversion
      //convert_eta << i;     
      //convert_phi << j;
      //eta_mod = convert_eta.str();
      //phi_mod = convert_phi.str();
      //Canvas.cd();
      //hproj->DrawClone();
      
      //Canvas.SaveAs((histoName+"_"+eta_mod+"_"+phi_mod+".pdf").c_str());

      //do not fill if there are 30 or less entries in the bin
      if(iopt<5 && (hproj->GetEntries()<=30 || hproj->Integral()<=30)) {
	delete hproj;
	//h->SetBinContent(i,j,0);
	//h->SetBinError(i,j,0);
	continue;
      }
      if(iopt==0){
	h->SetBinContent(i,j,hproj->GetMean());
	h->SetBinError(i,j,hproj->GetMeanError());
      }
      else if(iopt==1){
	h->SetBinContent(i,j,hproj->GetRMS());
	h->SetBinError(i,j,hproj->GetRMSError());
      }
      else if(iopt>=2 && iopt<5){

	//we make a 2-stage Gaussian fit
	//first fit is in the range [MEAN + 2*RMS, MEAN - 2*RMS]
	//(restricting the range of the first fit has little effect)
	//second fit is in the range [mean + 2*width, mean - 2*width]
	//using the mean and width of the first fit

	//first fit
	float _mean = hproj->GetMean();
	float _rms  = hproj->GetRMS();
	float _min = _mean - 2*_rms;
	float _max = _mean + 2*_rms;
	TF1 *fW = new TF1("fW","gaus",_min,_max);
	hproj -> Fit("fW", "RQN");
	
	//second fit
	_mean = fW->GetParameter(1);
	_rms  = fW->GetParameter(2);
	_min = _mean - 2*_rms;
	_max = _mean + 2*_rms;
	//if bins are large relative to distribution then 
	//the above determined range could only be two bins
	//or even just fractions of bins
	//in this case a gauus fit is meaningless, and can have strange
	//chi2 behaviour and large errors on the mean
	//therefore make sure we are including full bins in fit and 
	//have a minimum of 3 bins
	int _minBin = hproj->FindBin(_min);
	int _maxBin = hproj->FindBin(_max);
	if(_maxBin - _minBin < 2) {
	  //require at least 3 bins for a meaningful gauss fit
	  //std::cout << "WARNING core had too few bins for fit - expanding core for " << h->GetName() << std::endl;
	  _maxBin = _maxBin + 1;
	  _minBin = _minBin - 1;
	}
	_min = hproj->GetBinLowEdge(_minBin);
	_max = (hproj->GetBinLowEdge(_maxBin))+(hproj->GetBinWidth(_maxBin));

	TF1 *fW2 = new TF1("fW2","gaus",_min,_max);
	hproj -> Fit("fW2", "RQN");   
	if(iopt==2){
	  h->SetBinContent(i,j,fW2->GetParameter(1));
	  h->SetBinError(i,j,fW2->GetParError(1));
	} else if (iopt==3){
	  h->SetBinContent(i,j,fW2->GetParameter(2));
	  h->SetBinError(i,j,fW2->GetParError(2));
	} else if (iopt==4){
	  float _chi2 = fW2->GetChisquare();
	  int _ndf = fW2->GetNDF();
	  if(_ndf) h->SetBinContent(i,j,_chi2/_ndf);
	  else h->SetBinContent(i,j,0);
	}
	
	delete fW;
	delete fW2;
      }
      else if(iopt==5){
	h->SetBinContent(i,j,hproj->GetEntries());
      }
      //else std::cerr << "Incorrect iopt switch in meanRMSProjections3D()" << std::endl;
      delete hproj;
    }
  }

  return;
}

void MonitoringFile::
meanRMSProjections3D_DMRPlot(TH3F* h3d, TH1F* h,int iopt)
{

  //fills a 1-D histogram with the mean or width of the distribution for each bin in a 3D histogram
  //the way in which the mean or width is determined is specified by the iopt option
  //can be from a gaussian fit for example
  // iopt = 0; fills with statistical mean
  // iopt = 1; fills with statistical RMS
  // iopt = 2; fills with mean of a two-stage Gaussian fit
  // iopt = 3; fills with width of a two-stage Gaussian fit
  // iopt = 4; fills with Chi2/DOF of a two-stage Gaussian fit
  // iopt = 5; fills with the error on the fitted mean
  // iopt = 6; fills with statistical median
  // iopt = 7; fills with nentries in the bin

  int nbins_3dX = h3d->GetNbinsX();
  int nbins_3dY = h3d->GetNbinsY();

  for(int i = 1; i!=nbins_3dX+1; i++){
    for(int j = 1; j!=nbins_3dY+1; j++){

      TH1F* hproj = (TH1F*)h3d->ProjectionZ("proj",i,i,j,j,"e");

      //do not fill if there are 30 or less entries in the bin
      if(iopt<7 && (hproj->GetEntries()<=30 || hproj->Integral()<=30)) {
        delete hproj;
        continue;
      }
      if(iopt==0){
	h->Fill(hproj->GetMean());
      }
      else if(iopt==1){
	h->Fill(hproj->GetRMS());
      }
      else if(iopt>=2 && iopt<6){

	//we make a 2-stage Gaussian fit
	//first fit is in the range [MEAN + 2*RMS, MEAN - 2*RMS]
	//(restricting the range of the first fit has little effect)
	//second fit is in the range [mean + 2*width, mean - 2*width]
	//using the mean and width of the first fit

	//first fit
	float _mean = hproj->GetMean();
	float _rms  = hproj->GetRMS();
	float _min = _mean - 2*_rms;
	float _max = _mean + 2*_rms;
	TF1 *fW = new TF1("fW","gaus",_min,_max);
	hproj -> Fit("fW", "RQN");
	
	//second fit
	_mean = fW->GetParameter(1);
	_rms  = fW->GetParameter(2);
	_min = _mean - 2*_rms;
	_max = _mean + 2*_rms;
	//if bins are large relative to distribution then 
	//the above determined range could only be two bins
	//or even just fractions of bins
	//in this case a gauus fit is meaningless, and can have strange
	//chi2 behaviour and large errors on the mean
	//therefore make sure we are including full bins in fit and 
	//have a minimum of 3 bins
	int _minBin = hproj->FindBin(_min);
	int _maxBin = hproj->FindBin(_max);
	if(_maxBin - _minBin < 2) {
	  //require at least 3 bins for a meaningful gauss fit
	  //std::cout << "WARNING core had too few bins for fit - expanding core for " << h->GetName() << std::endl;
	  _maxBin = _maxBin + 1;
	  _minBin = _minBin - 1;
	}
	_min = hproj->GetBinLowEdge(_minBin);
	_max = (hproj->GetBinLowEdge(_maxBin))+(hproj->GetBinWidth(_maxBin));

	TF1 *fW2 = new TF1("fW2","gaus",_min,_max);
	hproj -> Fit("fW2", "RQN");   
	if(iopt==2){
	  h->Fill(fW2->GetParameter(1));
	} else if (iopt==3){
	  h->Fill(fW2->GetParameter(2));
	} else if (iopt==4){
	  float _chi2 = fW2->GetChisquare();
	  int _ndf = fW2->GetNDF();
	  if(_ndf) h->Fill(_chi2/_ndf);
	  else h->Fill(0.0);
	}else if (iopt==5){
	  h->Fill(fW2->GetParError(1));
	}
	
	delete fW;
	delete fW2;
      }
      else if(iopt==6){
	h->Fill(getMedian(hproj));
      }
      else if(iopt==7){
	h->Fill(hproj->GetEntries());
      }
      //else std::cerr << "Incorrect iopt switch in meanRMSProjections3D_DMRPlot()" << std::endl;
      delete hproj;
    }
  }

  return;
}

void MonitoringFile::
meanRMSProjections3D_XY(TH3F* h3d, TH1F* h,int iXY,int iopt)
{

  //fills a 1-D histogram with mean or width of the projected rows/columns of a 3-D histogram
  // iXY = 0; project columns (i.e. for each x of the 3D histogram form the distribution of all y-bins)
  // iXY = 1; project rows (i.e. for each y of the 3D histogram form the distribution of all x-bins)
  //the way in which the mean or width is determined is specified by the iopt option
  //can be from a gaussian fit for example
  // iopt = 0; fills with statistical mean
  // iopt = 1; fills with statistical RMS
  // iopt = 2; fills with mean of a two-stage Gaussian fit
  // iopt = 3; fills with width of a two-stage Gaussian fit
  // iopt = 4; fills with Chi2/DOF of a two-stage Gaussian fit
  // iopt = 5; fills with nentries in the bin

  int nbins_3dX = h3d->GetNbinsX();
  int nbins_3dY = h3d->GetNbinsY();
  int nbins_h = h->GetNbinsX();
  int nbins_3d = 0;//will be overridden

  if(iXY==0) {
    if(nbins_3dX!=nbins_h) {
	std::cerr << "meanRMSProjections3D_XY: Mean/RMS Histograms not set up correctly - nbins mismatch for " << h->GetName() << std::endl;
      return;
    }
    nbins_3d = nbins_3dX;
  }
  else if(iXY==1) {
    if(nbins_3dY!=nbins_h) {
	std::cerr << "meanRMSProjections3D_XY: Mean/RMS Histograms not set up correctly - nbins mismatch for" << h->GetName() << std::endl;
	std::cout << "nbins_3dY = " << nbins_3dY << ", nbins_h = " << nbins_h << std::endl;
      return;
    }
    nbins_3d = nbins_3dY;
  }
  else {
      //std::cerr << "meanRMSProjections3D_XY: iXY != 1 or 0 exiting" << std::endl;
    return;
  }

  for(int i = 1; i!=nbins_3d+1; i++){

    TH1F* hproj;
    if(iXY==0){
      h3d->GetXaxis()->SetRange(i,i);
      h3d->GetYaxis()->SetRange(1,nbins_3dY);
      hproj = (TH1F*)h3d->Project3D("ze");
    }
    else { // (iXY == 1), as checked above
      h3d->GetYaxis()->SetRange(i,i);
      h3d->GetXaxis()->SetRange(1,nbins_3dX);
      hproj = (TH1F*)h3d->Project3D("ze");
    }
        
    //do not fill if there are 30 or less entries in the bin
    if(iopt<5 && (hproj->GetEntries()<=30 || hproj->Integral()<=30)) {
      delete hproj;
      //h->SetBinContent(i,0);
      //h->SetBinError(i,0);
      continue;
    }
    if(iopt==0){
      h->SetBinContent(i,hproj->GetMean());
      h->SetBinError(i,hproj->GetMeanError());
    }
    else if(iopt==1){
      h->SetBinContent(i,hproj->GetRMS());
      h->SetBinError(i,hproj->GetRMSError());
    }
    else if(iopt>=2 && iopt<5){

      //we make a 2-stage Gaussian fit
      //first fit is in the range [MEAN + 2*RMS, MEAN - 2*RMS]
      //(restricting the range of the first fit has little effect)
      //second fit is in the range [mean + 2*width, mean - 2*width]
      //using the mean and width of the first fit

      //first fit
      float _mean = hproj->GetMean();
      float _rms  = hproj->GetRMS();
      float _min = _mean - 2*_rms;
      float _max = _mean + 2*_rms;
      TF1 *fW = new TF1("fW","gaus",_min,_max);
      hproj -> Fit("fW", "RQN");

      //second fit
      _mean = fW->GetParameter(1);
      _rms  = fW->GetParameter(2);
      _min = _mean - 2*_rms;
      _max = _mean + 2*_rms;
      //if bins are large relative to distribution then 
      //the above determined range could only be two bins
      //or even just fractions of bins
      //in this case a gauus fit is meaningless, and can have strange
      //chi2 behaviour and large errors on the mean
      //therefore make sure we are including full bins in fit and 
      //have a minimum of 3 bins
      int _minBin = hproj->FindBin(_min);
      int _maxBin = hproj->FindBin(_max);
      if(_maxBin - _minBin < 2) {
	//require at least 3 bins for a meaningful gauss fit
	//std::cout << "WARNING core had too few bins for fit - expanding core for " << h->GetName() << std::endl;
	_maxBin = _maxBin + 1;
	_minBin = _minBin - 1;
      }
      _min = hproj->GetBinLowEdge(_minBin);
      _max = (hproj->GetBinLowEdge(_maxBin))+(hproj->GetBinWidth(_maxBin));

      TF1 *fW2 = new TF1("fW2","gaus",_min,_max);
      hproj -> Fit("fW2", "RQN");   
      if(iopt==2){
	h->SetBinContent(i,fW2->GetParameter(1));
	h->SetBinError(i,fW2->GetParError(1));
      } else if (iopt==3){
	h->SetBinContent(i,fW2->GetParameter(2));
	h->SetBinError(i,fW2->GetParError(2));
      } else if (iopt==4){
	float _chi2 = fW2->GetChisquare();
	int _ndf = fW2->GetNDF();
	if(_ndf) h->SetBinContent(i,_chi2/_ndf);
	else h->SetBinContent(i,0);
      }
      delete fW;
      delete fW2;
    }
    else if(iopt==5){
      h->SetBinContent(i,hproj->GetEntries());
    }
    //else std::cerr << "Incorrect iopt switch in meanRMSProjections3D_XY()" << std::endl;
    delete hproj;
  }


  return;

}


TH1F* MonitoringFile::
combineHistos(const char* name, const char* title, TObjArray* plots,int gap)
{

  //makes one histogram out of all the histograms in the TObjArray
  //this histogram is built such that the histograms are placed side-by-side 
  //on the same x-axis

  int n_plots = (plots->GetLast())+1;//add one since begins at zero

  //std::cout << "nplots = " << n_plots << std::endl;

  //count total bins
  int totalBins = 0;
  for(int i=0;i!=n_plots;++i){

    int nbins = ((TH1F*)(plots->At(i)))->GetNbinsX();
    totalBins = totalBins + nbins;
  }
  //take account of the gap we want to leave between plots
  totalBins = totalBins + ((n_plots-1)*gap);

  TH1F* hnew = new TH1F(name,title,totalBins,0,totalBins);

  int bCount = 1;
  //loop over the plots and fill the new one
  for(int i=0;i!=n_plots;++i){

    TH1F* h = (TH1F*)(plots->At(i));

    int nbins_h = h->GetNbinsX();
    //fill the bins with the content of h
    for(int j=1;j!=nbins_h+1;++j){
      hnew->SetBinContent(bCount,h->GetBinContent(j));
      hnew->SetBinError(bCount,h->GetBinError(j));
      bCount++;
    }
    
    //skip the next bins to produce a gap
    //only if this isn't the last plot
    //(don't want a gap at the end)
    if(i<(n_plots-1))bCount = bCount + gap;
  }

  return hnew;
  
}


void
MonitoringFile::
setMinWindow(TH1* h1, float windowMin, float windowMax){
  
  //establishes a minimum window size for TH1 histogram

  float min=h1->GetMinimum();
  float max=h1->GetMaximum();
  float margin=0.05*(max-min);
  if(min > windowMin) min=windowMin-margin;
  if(max < windowMax) max=windowMax+margin;
  h1->SetMinimum(min);
  h1->SetMaximum(max);

  return;

}

float
MonitoringFile::
getMedian(TH1* h1){
  
  //calculates median for a TH1

  int nBins = h1->GetXaxis()->GetNbins();

  //double x[nBins]; double y[nBins];
  std::vector<double> x;
  std::vector<double> y;
  // double x[10000]; double y[10000];
  for (int i = 1; i!=nBins+1; i++){

    //x[i] = h1->GetBinCenter(i);
    //y[i] = h1->GetBinContent(i);
    x.push_back(h1->GetBinCenter(i));
    y.push_back(h1->GetBinContent(i));
  }

  //float median = TMath::Median(nBins,x,y);
  float median = TMath::Median(nBins,&x[0],&y[0]);

  return median;

}

void 
MonitoringFile::
fillMeanOrWidth(TH2F* h2d, TH1F* h,int iopt)
{

  //fills a 1-D histogram with either the mean or RMS of the 2d histogram in each bin. 
  //binning of 1d and 2d histograms must match
  // iopt=0; Mean
  // iopt=1; Width

  //std::cout << "In fillMeanOrWidth" << std::endl;

  int nbins_2d = h2d->GetNbinsX();
  int nbins_h = h->GetNbinsX();

  if(nbins_2d!=nbins_h) //std::cout << "Mean/RMS Histograms not set up correctly - nbins mismatch for " << h->GetName() << std::endl;

  for(int i = 1; i!=nbins_2d+1; i++){

    //std::cout << "Processing bin " << i << std::endl;

    TH1F* hproj = (TH1F*)h2d->ProjectionY("proj",i,i,"e");

    //std::cout << "Made projection for bin " << i << std::endl;

    //do not fill if there are no entries in the bin
    if(hproj->GetEntries()<=0) {
      delete hproj;
      continue;
    }

    if(iopt==0){
      h->SetBinContent(i,hproj->GetMean());
      h->SetBinError(i,hproj->GetMeanError());
      //std::cout << "Filled mean results for bin " << i << std::endl;
    }
    else if(iopt==1){
      h->SetBinContent(i,hproj->GetRMS());
      h->SetBinError(i,hproj->GetRMSError());
    }
    //else std::cout << "Incorrect switch in MeanRMSProjectionsBarrel()" << std::endl;

    delete hproj;
  }

  //std::cout << "Finished fillMeanOrWidth" << std::endl;

  return;

}


void 
MonitoringFile::
fillGaussianMeanOrWidth(TH2F* h2d, TH1F* h, float fitMin, float fitMax, int iopt)
{

  //Makes a Gaussian fit to each bin of a TH2F and fills a TH1F with 
  //the either the mean or width of this Gaussian
  // iopt=0; Mean
  // iopt=1; Width

  //std::cout << "In fillGaussianMeanOrWidth for " << h->GetName() << std::endl;

  //calling this means that the histogram bin content is flagged 
  //as being an average and so adding histos from different jobs 
  //will produce weighted mean
  h->SetBit(TH1::kIsAverage);

  int nbins_2d = h2d->GetNbinsX();

  // Removed by Alex
  //int nbins_h = h->GetNbinsX();
  //if(nbins_2d!=nbins_h) std::cout << "Mean/Width Histograms not set up correctly - nbins mismatch for " << h->GetName() << std::endl;

  for(int i = 1; i!=nbins_2d+1; i++){

    //std::cout << "Gaussian: processing bin " << i << std::endl;

    TH1F* hproj = (TH1F*)h2d->ProjectionY("proj",i,i,"e");

    //std::cout << "Gaussian: made projection for bin " << i << std::endl;

    //do not fill if there are few entries in the bin
    if(hproj->GetEntries()<=30 || hproj->Integral()<=30) {
      delete hproj;
      continue;
    }
    
    TF1 *fit = new TF1("fit","gaus",fitMin,fitMax);

    hproj->Fit("fit","RQN");
    float mean = fit->GetParameter(1);
    float meanErr = fit->GetParError(1);
    float width = fit->GetParameter(2);
    float widthErr = fit->GetParError(2);

    //std::cout << "Gaussian: fitted bin " << i << std::endl;

    if(iopt==0){
      h->SetBinContent(i,mean);
      h->SetBinError(i,meanErr);
    }
    else if(iopt==1){
      h->SetBinContent(i,width);
      h->SetBinError(i,widthErr);
    }
    //else  std::cout << "Incorrect switch in fillGaussianMeanOrWidth" << std::endl;

    delete hproj;
    delete fit;
  }

  //std::cout << "leaving fillGaussianMeanOrWidth for " << h->GetName() << std::endl;

  return;

}
/*
void MonitoringFile::MakeStaveShapeFit(float& mag, float& mag_er, float& base, float& base_er, TH1D* projection)
{
  TGraphErrors* g = ConvertHistoInGraph(projection);
  TF1 *fit;
  fit = new TF1("fit", "[0]-[1]*(x+[2]) * (4.0*[2]*(x+[2])**2 - (x+[2])**3 - (2.0*[2])**3)", -m_z_fix, m_z_fix );
  fit->FixParameter(2, m_z_fix);
  fit->SetParameter(1, 5.e-12);
  TFitResultPtr r = g->Fit(fit,"EX0SQ");
  mag       =  5.0 * fit->GetParameter(1) * (m_z_fix*m_z_fix*m_z_fix*m_z_fix);
  mag_er    =  5.0 * fit->GetParError(1)  * (m_z_fix*m_z_fix*m_z_fix*m_z_fix);
  base     =fit->GetParameter(0);
  base_er  =fit->GetParError(0);
  
  delete g;
  return;
}

TGraphErrors* MonitoringFile::ConvertHistoInGraph(TH1D* histo)
{
  TGraphErrors* graph = new TGraphErrors();
  std::vector<int> filled_bins;
  for (int etabin=1;etabin < 21;++etabin)
    {
      if (histo->GetBinContent(etabin) != 0.)
	{
	  filled_bins.push_back(etabin);
	}
    }
  
  for (int ibin=0;ibin < (int) filled_bins.size();++ibin)
    {
      graph->Set(ibin+1);
      graph->SetPoint(ibin,z_axis[filled_bins.at(ibin)-1],histo->GetBinContent(filled_bins.at(ibin)));
      graph->SetPointError(ibin,0,histo->GetBinError(filled_bins.at(ibin)));
    }
  
  graph->GetXaxis()->SetRangeUser(-m_z_fix,m_z_fix);
  return graph;
  
}
*/
/*
void MonitoringFile::MakeBowingFit(TProfile2D* p2d, TH1F* hm, TH1F* hb)
{
  for (int lumibin=1;lumibin <= p2d->GetXaxis()->GetNbins();++lumibin)
    {
      TH1D* projection_lumiblock = p2d->ProjectionY(("iblBowingProjection_lumiblock_"+intToString(lumibin-1)).c_str(),lumibin,lumibin);
      MakeStaveShapeFit(mag,mag_er,base,base_er,projection_lumiblock);
      m_pix_b0_mag_vs_lb->SetBinContent(lumibin,mag);
      m_pix_b0_mag_vs_lb->SetBinError(lumibin,mag_er);
      m_pix_b0_base_vs_lb->SetBinContent(lumibin,base);
      m_pix_b0_base_vs_lb->SetBinError(lumibin,base_er);
      delete projection_lumiblock;
    }
  
  return;
}
*/

void 
MonitoringFile::
fillDetPaperMeanRMS(TH2F* h2d, TH1F* h, int iopt)
{

  //this finds the statistical mean/rms using the detector paper definition
  //i.e. the mean and RMS defined using bins which contain 99.7% of entries (symmetrically) around the mean bin
  // iopt=0; Mean
  // iopt=1; RMS (resolution)
  //Note that this method is not reliable if there is large underflow/overflow in the histogram.

  //calling this means that the histogram bin content is flagged 
  //as being an average and so adding histos from different jobs 
  //will produce weighted mean
  h->SetBit(TH1::kIsAverage);

  int nbins_2d = h2d->GetNbinsX();

  // Removed by Alex
  //int nbins_h = h->GetNbinsX();
  //if(nbins_2d!=nbins_h) std::cout << "Mean/Width Histograms not set up correctly - nbins mismatch for " << h->GetName() << std::endl;

  for(int i = 1; i!=nbins_2d+1; i++){

    //std::cout << "fillDetPaperMeanRMS: processing bin " << i << std::endl;

    TH1F* hproj = (TH1F*)h2d->ProjectionY("proj",i,i,"e");

    //std::cout << "fillDetPaperMeanRMS: made projection for bin " << i << std::endl;

    //do not fill if there are few entries in the bin
    if(hproj->GetEntries()<=30 || hproj->Integral()<=30) {
      delete hproj;
      continue;
    }

    float hmean = hproj->GetMean();
    //    float hRMS = hproj->GetRMS();
    float hFullIntegral = hproj->Integral(1,hproj->GetNbinsX());

    //std::cout << "fillDetPaperMeanRMS: Original Mean, RMS = " << hmean << ", " << hRMS << std::endl;

    float hTargetIntegral = 0.997*hFullIntegral;
    int hMeanBin = hproj->FindBin(hmean);

    //std::cout << "fillDetPaperMeanRMS: NEntries = " << hproj->GetEntries() << ",  fullIntegral = " << hFullIntegral << std::endl;

    int lowBin = hMeanBin;
    int highBin = hMeanBin;
    float hIntegral = 0.0;

    while (hIntegral < hTargetIntegral) {
      // find the bin which contains the mean
      // then integrate symmetrically around this bin, increasing the range 
      // until the integral is larger than the target
      // this defines your upper and lower bounds for mean/rms.

      lowBin = lowBin - 1;
      highBin = highBin + 1;
      hIntegral = hproj->Integral(lowBin,highBin);
    }

    //std::cout << "fillDetPaperMeanRMS: NBins = " <<  hproj->GetNbinsX() << " minBin =  " << lowBin << " highBin = " << highBin << std::endl;
    
    TH1F* hclone = (TH1F*)hproj->Clone("clone"); 

    hclone->GetXaxis()->SetRange(lowBin,highBin);
    float hNewMean = hclone->GetMean();
    float hNewMeanErr = hclone->GetMeanError();
    float hNewRMS = hclone->GetRMS();
    float hNewRMSErr = hclone->GetRMSError();

    //std::cout << "fillDetPaperMeanRMS: New Mean, RMS = " << hNewMean << ", " << hNewRMS << std::endl;

    if(iopt==0){
      h->SetBinContent(i,hNewMean);
      h->SetBinError(i,hNewMeanErr);
    }
    else if(iopt==1){
      h->SetBinContent(i,hNewRMS );
      h->SetBinError(i,hNewRMSErr);
    }
    //else  std::cout << "Incorrect switch in fillDetPaperMeanRMS" << std::endl;

    delete hproj;
    delete hclone;
  }

  return;

}
}
