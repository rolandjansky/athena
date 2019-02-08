/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*************************************************
// Class for the RPC merge histogram and fill the COOL DB
// author Michele Bianco michele.bianco@le.infn.it, Gabriele Chiodini gabriele.chiodini@le.infn.it                          
// and Angelo Guida angelo.guida@le.infn.it
// 08/April/2009
//************************************************

#include "DataQualityUtils/MonitoringFile.h"
#include "DataQualityUtils/CoolRpc.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <cstdlib>

#include "TH1F.h"
#include "TFile.h"
#include "TClass.h"
#include "TKey.h"

namespace dqutils {

void 
MonitoringFile::RPCPostProcess( std::string inFilename, bool /* isIncremental */ ) 
{
 // std::cout << "Running RPC post processing \n" ;
  
  bool  applyEffThreshold  =  true;
  bool  EffThreshold       = false;
  bool  printout           = true ;
  float Minimum_efficiency =  0.5;
  
  
  
  TFile* f = TFile::Open(inFilename.c_str(),"UPDATE");
  if (f == 0) {
    std::cerr << "MonitoringFile::RPCPostProcess(): "
	      << "Input file not opened \n";
    return;
  }
  if(f->GetSize()<1000.) {
    std::cerr << "MonitoringFile::RPCPostProcess(): "
	      << "Input file empty \n";
    return; 
  }
  CoolRpc coolrpc;												    		 
 
  // get run directory name
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
	
	int run_number;
	run_number = atoi( (run_dir.substr(4, run_dir.size()-4 )).c_str() );
        std::cout << "run_number rpc monitoring " << run_number <<std::endl;
	
	std::string pathRawMon     = run_dir + "/Muon/MuonRawDataMonitoring/RPC/"                           ;
	//std::string pathTrackMon   = run_dir + "/Muon/MuonTrackMonitoring/NoTrigger/RPCStandAloneTrackMon/" ;
	std::string pathTrackMon   = run_dir + "/Muon/MuonRawDataMonitoring/RPCStandAloneTrackMon/" ;
	
	std::string dir_ov_raw     = pathRawMon   + "Overview/" ;
	std::string dir_sum_raw    = pathRawMon   + "Summary/"  ;
	std::string dir_dqmf_raw   = pathRawMon   + "Dqmf/"     ;
	
  	std::string dir_sideA_track    = pathTrackMon + "RPCBA/"             ;
  	std::string dir_sideC_track    = pathTrackMon + "RPCBC/"             ;
  	std::string dir_glob_track     = pathTrackMon + "GLOBAL/"            ;
	std::string dir_sum_track      = pathTrackMon + "Summary/"           ;
	std::string dir_dqmf_track     = pathTrackMon + "Dqmf/"              ;
	std::string dir_trigger_track  = pathTrackMon + "TriggerEfficiency/" ;
	
	double n_hit_f, n_tr_p, panel_eff, panel_err_eff;
	double nEta, nPhi, nEtaPhi, gapEff=0.0, gapErrEff=0.0;
	double res_mean, res2_mean, res_RMS;
	double panel_occ; 
	double panelCS, panelCS2, panelCS_entries , panelCS_mean, panelCS2_mean, panelCS_RMS;
	double Time_mean, Time2_mean,  Time_RMS;
	double noiseCorr, noiseCorrErr ;
	double noiseTot,  noiseTotErr  ;
	double noiseErrNorm ;
	
	// trigger efficiency
	std::string METracks_name   = dir_trigger_track   + "hMEtracks"            ; 
	std::string MuctpiThr0_name = dir_trigger_track   + "hRPCMuctpiThr0"       ; 
	std::string MuctpiThr1_name = dir_trigger_track   + "hRPCMuctpiThr1"       ; 
	std::string MuctpiThr2_name = dir_trigger_track   + "hRPCMuctpiThr2"       ; 
	std::string MuctpiThr3_name = dir_trigger_track   + "hRPCMuctpiThr3"       ; 
	std::string MuctpiThr4_name = dir_trigger_track   + "hRPCMuctpiThr4"       ; 
	std::string MuctpiThr5_name = dir_trigger_track   + "hRPCMuctpiThr5"       ; 
	std::string PadThr0_name = dir_trigger_track   + "hRPCPadThr0"       ; 
	std::string PadThr1_name = dir_trigger_track   + "hRPCPadThr1"       ; 
	std::string PadThr2_name = dir_trigger_track   + "hRPCPadThr2"       ; 
	std::string PadThr3_name = dir_trigger_track   + "hRPCPadThr3"       ; 
	std::string PadThr4_name = dir_trigger_track   + "hRPCPadThr4"       ; 
	std::string PadThr5_name = dir_trigger_track   + "hRPCPadThr5"       ; 
	std::string PhiEtaCoinThr0_name = dir_trigger_track   + "hRPCPhiEtaCoinThr0"       ; 
	std::string PhiEtaCoinThr1_name = dir_trigger_track   + "hRPCPhiEtaCoinThr1"       ; 
	std::string PhiEtaCoinThr2_name = dir_trigger_track   + "hRPCPhiEtaCoinThr2"       ; 
	std::string PhiEtaCoinThr3_name = dir_trigger_track   + "hRPCPhiEtaCoinThr3"       ; 
	std::string PhiEtaCoinThr4_name = dir_trigger_track   + "hRPCPhiEtaCoinThr4"       ; 
	std::string PhiEtaCoinThr5_name = dir_trigger_track   + "hRPCPhiEtaCoinThr5"       ; 
	
	std::string MuctpiThr_eff0_name = dir_trigger_track   + "hRPCMuctpiThr_eff0"       ; 
	std::string MuctpiThr_eff1_name = dir_trigger_track   + "hRPCMuctpiThr_eff1"       ; 
	std::string MuctpiThr_eff2_name = dir_trigger_track   + "hRPCMuctpiThr_eff2"       ; 
	std::string MuctpiThr_eff3_name = dir_trigger_track   + "hRPCMuctpiThr_eff3"       ; 
	std::string MuctpiThr_eff4_name = dir_trigger_track   + "hRPCMuctpiThr_eff4"       ; 
	std::string MuctpiThr_eff5_name = dir_trigger_track   + "hRPCMuctpiThr_eff5"       ; 
	std::string PadThr_eff0_name = dir_trigger_track   + "hRPCPadThr_eff0"       ; 
	std::string PadThr_eff1_name = dir_trigger_track   + "hRPCPadThr_eff1"       ; 
	std::string PadThr_eff2_name = dir_trigger_track   + "hRPCPadThr_eff2"       ; 
	std::string PadThr_eff3_name = dir_trigger_track   + "hRPCPadThr_eff3"       ; 
	std::string PadThr_eff4_name = dir_trigger_track   + "hRPCPadThr_eff4"       ; 
	std::string PadThr_eff5_name = dir_trigger_track   + "hRPCPadThr_eff5"       ; 
	std::string PhiEtaCoinThr_eff0_name = dir_trigger_track   + "hRPCPhiEtaCoinThr_eff0"       ; 
	std::string PhiEtaCoinThr_eff1_name = dir_trigger_track   + "hRPCPhiEtaCoinThr_eff1"       ; 
	std::string PhiEtaCoinThr_eff2_name = dir_trigger_track   + "hRPCPhiEtaCoinThr_eff2"       ; 
	std::string PhiEtaCoinThr_eff3_name = dir_trigger_track   + "hRPCPhiEtaCoinThr_eff3"       ; 
	std::string PhiEtaCoinThr_eff4_name = dir_trigger_track   + "hRPCPhiEtaCoinThr_eff4"       ; 
	std::string PhiEtaCoinThr_eff5_name = dir_trigger_track   + "hRPCPhiEtaCoinThr_eff5"       ; 
 
 	
	if ( RPCCheckHistogram(f,METracks_name.c_str()) && RPCCheckHistogram(f,MuctpiThr0_name.c_str()) && RPCCheckHistogram(f,MuctpiThr_eff0_name.c_str())) {
	
 	 
	  TH1I* hist_METracks   = (TH1I*)(f->Get( METracks_name.c_str()));
	  TH1I* hist_MuctpiThr0 = (TH1I*)(f->Get( MuctpiThr0_name.c_str()));
	  TH1I* hist_MuctpiThr1 = (TH1I*)(f->Get( MuctpiThr1_name.c_str()));
	  TH1I* hist_MuctpiThr2 = (TH1I*)(f->Get( MuctpiThr2_name.c_str()));
	  TH1I* hist_MuctpiThr3 = (TH1I*)(f->Get( MuctpiThr3_name.c_str()));
	  TH1I* hist_MuctpiThr4 = (TH1I*)(f->Get( MuctpiThr4_name.c_str()));
	  TH1I* hist_MuctpiThr5 = (TH1I*)(f->Get( MuctpiThr5_name.c_str()));
	  TH1I* hist_PadThr0 = (TH1I*)(f->Get( PadThr0_name.c_str()));
	  TH1I* hist_PadThr1 = (TH1I*)(f->Get( PadThr1_name.c_str()));
	  TH1I* hist_PadThr2 = (TH1I*)(f->Get( PadThr2_name.c_str()));
	  TH1I* hist_PadThr3 = (TH1I*)(f->Get( PadThr3_name.c_str()));
	  TH1I* hist_PadThr4 = (TH1I*)(f->Get( PadThr4_name.c_str()));
	  TH1I* hist_PadThr5 = (TH1I*)(f->Get( PadThr5_name.c_str()));
	  TH1I* hist_PhiEtaCoinThr0 = (TH1I*)(f->Get( PhiEtaCoinThr0_name.c_str()));
	  TH1I* hist_PhiEtaCoinThr1 = (TH1I*)(f->Get( PhiEtaCoinThr1_name.c_str()));
	  TH1I* hist_PhiEtaCoinThr2 = (TH1I*)(f->Get( PhiEtaCoinThr2_name.c_str()));
	  TH1I* hist_PhiEtaCoinThr3 = (TH1I*)(f->Get( PhiEtaCoinThr3_name.c_str()));
	  TH1I* hist_PhiEtaCoinThr4 = (TH1I*)(f->Get( PhiEtaCoinThr4_name.c_str()));
	  TH1I* hist_PhiEtaCoinThr5 = (TH1I*)(f->Get( PhiEtaCoinThr5_name.c_str()));
	  TH1I* hist_MuctpiThr_eff0 = (TH1I*)(f->Get( MuctpiThr_eff0_name.c_str()));
	  TH1I* hist_MuctpiThr_eff1 = (TH1I*)(f->Get( MuctpiThr_eff1_name.c_str()));
	  TH1I* hist_MuctpiThr_eff2 = (TH1I*)(f->Get( MuctpiThr_eff2_name.c_str()));
	  TH1I* hist_MuctpiThr_eff3 = (TH1I*)(f->Get( MuctpiThr_eff3_name.c_str()));
	  TH1I* hist_MuctpiThr_eff4 = (TH1I*)(f->Get( MuctpiThr_eff4_name.c_str()));
	  TH1I* hist_MuctpiThr_eff5 = (TH1I*)(f->Get( MuctpiThr_eff5_name.c_str()));
	  TH1I* hist_PadThr_eff0 = (TH1I*)(f->Get( PadThr_eff0_name.c_str()));
	  TH1I* hist_PadThr_eff1 = (TH1I*)(f->Get( PadThr_eff1_name.c_str()));
	  TH1I* hist_PadThr_eff2 = (TH1I*)(f->Get( PadThr_eff2_name.c_str()));
	  TH1I* hist_PadThr_eff3 = (TH1I*)(f->Get( PadThr_eff3_name.c_str()));
	  TH1I* hist_PadThr_eff4 = (TH1I*)(f->Get( PadThr_eff4_name.c_str()));
	  TH1I* hist_PadThr_eff5 = (TH1I*)(f->Get( PadThr_eff5_name.c_str()));
	  TH1I* hist_PhiEtaCoinThr_eff0 = (TH1I*)(f->Get( PhiEtaCoinThr_eff0_name.c_str()));
	  TH1I* hist_PhiEtaCoinThr_eff1 = (TH1I*)(f->Get( PhiEtaCoinThr_eff1_name.c_str()));
	  TH1I* hist_PhiEtaCoinThr_eff2 = (TH1I*)(f->Get( PhiEtaCoinThr_eff2_name.c_str()));
	  TH1I* hist_PhiEtaCoinThr_eff3 = (TH1I*)(f->Get( PhiEtaCoinThr_eff3_name.c_str()));
	  TH1I* hist_PhiEtaCoinThr_eff4 = (TH1I*)(f->Get( PhiEtaCoinThr_eff4_name.c_str()));
	  TH1I* hist_PhiEtaCoinThr_eff5 = (TH1I*)(f->Get( PhiEtaCoinThr_eff5_name.c_str()));
	
	  int nb = hist_METracks->GetNbinsX() ;
	  double  Ly_eff, Ly_effErr ;
	  for ( int ib=0; ib!=nb; ib++ ) {
	     float n_Ly_hitOn = hist_MuctpiThr0->GetBinContent(ib+1) ;  
	     float n_Ly_TrPrj = hist_METracks  ->GetBinContent(ib+1) ;
	     
	     
	     if ( n_Ly_TrPrj >0 ) {
	       //MuctpiThr0
	       Ly_eff    = float(n_Ly_hitOn)/float(n_Ly_TrPrj);
	       
	       Ly_effErr = sqrt( fabs( n_Ly_hitOn) / n_Ly_TrPrj ) *
	  		   sqrt( 1. - fabs( n_Ly_hitOn) / n_Ly_TrPrj ) /
	  		   sqrt( n_Ly_TrPrj ) ;
	       hist_MuctpiThr_eff0->SetBinContent(ib+1, Ly_eff)    ;
	       hist_MuctpiThr_eff0->SetBinError  (ib+1, Ly_effErr) ;
	    
	       //MuctpiThr1
	       n_Ly_hitOn = hist_MuctpiThr1->GetBinContent(ib+1) ;  
	       Ly_eff    = float(n_Ly_hitOn)/float(n_Ly_TrPrj);
	       
	       Ly_effErr = sqrt( fabs( n_Ly_hitOn) / n_Ly_TrPrj ) *
	  	 	    sqrt( 1. - fabs( n_Ly_hitOn) / n_Ly_TrPrj ) /
	  		    sqrt( n_Ly_TrPrj ) ;
	       hist_MuctpiThr_eff1->SetBinContent(ib+1, Ly_eff)    ;
	       hist_MuctpiThr_eff1->SetBinError  (ib+1, Ly_effErr) ;
	       //MuctpiThr2
	       n_Ly_hitOn = hist_MuctpiThr2->GetBinContent(ib+1) ;  
	       Ly_eff    = float(n_Ly_hitOn)/float(n_Ly_TrPrj);
	       
	       Ly_effErr = sqrt( fabs( n_Ly_hitOn) / n_Ly_TrPrj ) *
	  	 	    sqrt( 1. - fabs( n_Ly_hitOn) / n_Ly_TrPrj ) /
	  		    sqrt( n_Ly_TrPrj ) ;
	       hist_MuctpiThr_eff2->SetBinContent(ib+1, Ly_eff)    ;
	       hist_MuctpiThr_eff2->SetBinError  (ib+1, Ly_effErr) ;
	       //MuctpiThr3
	       n_Ly_hitOn = hist_MuctpiThr3->GetBinContent(ib+1) ;  
	       Ly_eff    = float(n_Ly_hitOn)/float(n_Ly_TrPrj);
	       
	       Ly_effErr = sqrt( fabs( n_Ly_hitOn) / n_Ly_TrPrj ) *
	  	 	    sqrt( 1. - fabs( n_Ly_hitOn) / n_Ly_TrPrj ) /
	  		    sqrt( n_Ly_TrPrj ) ;
	       hist_MuctpiThr_eff3->SetBinContent(ib+1, Ly_eff)    ;
	       hist_MuctpiThr_eff3->SetBinError  (ib+1, Ly_effErr) ;
	       //MuctpiThr4
	       n_Ly_hitOn = hist_MuctpiThr4->GetBinContent(ib+1) ;  
	       Ly_eff    = float(n_Ly_hitOn)/float(n_Ly_TrPrj);
	       
	       Ly_effErr = sqrt( fabs( n_Ly_hitOn) / n_Ly_TrPrj ) *
	  	 	    sqrt( 1. - fabs( n_Ly_hitOn) / n_Ly_TrPrj ) /
	  		    sqrt( n_Ly_TrPrj ) ;
	       hist_MuctpiThr_eff4->SetBinContent(ib+1, Ly_eff)    ;
	       hist_MuctpiThr_eff4->SetBinError  (ib+1, Ly_effErr) ;
	       //MuctpiThr5
	       n_Ly_hitOn = hist_MuctpiThr5->GetBinContent(ib+1) ;  
	       Ly_eff    = float(n_Ly_hitOn)/float(n_Ly_TrPrj);
	       
	       Ly_effErr = sqrt( fabs( n_Ly_hitOn) / n_Ly_TrPrj ) *
	  	 	    sqrt( 1. - fabs( n_Ly_hitOn) / n_Ly_TrPrj ) /
	  		    sqrt( n_Ly_TrPrj ) ;
	       hist_MuctpiThr_eff5->SetBinContent(ib+1, Ly_eff)    ;
	       hist_MuctpiThr_eff5->SetBinError  (ib+1, Ly_effErr) ;
	       //PadThr0
	       n_Ly_hitOn = hist_PadThr0->GetBinContent(ib+1) ;
	       Ly_eff    = float(n_Ly_hitOn)/float(n_Ly_TrPrj);
	       
	       Ly_effErr = sqrt( fabs( n_Ly_hitOn) / n_Ly_TrPrj ) *
	  		   sqrt( 1. - fabs( n_Ly_hitOn) / n_Ly_TrPrj ) /
	  		   sqrt( n_Ly_TrPrj ) ;
	       hist_PadThr_eff0->SetBinContent(ib+1, Ly_eff)    ;
	       hist_PadThr_eff0->SetBinError  (ib+1, Ly_effErr) ;
	       //PadThr1
	       n_Ly_hitOn = hist_PadThr1->GetBinContent(ib+1) ;  
	       Ly_eff    = float(n_Ly_hitOn)/float(n_Ly_TrPrj);
	       
	       Ly_effErr = sqrt( fabs( n_Ly_hitOn) / n_Ly_TrPrj ) *
	  	 	    sqrt( 1. - fabs( n_Ly_hitOn) / n_Ly_TrPrj ) /
	  		    sqrt( n_Ly_TrPrj ) ;
	       hist_PadThr_eff1->SetBinContent(ib+1, Ly_eff)    ;
	       hist_PadThr_eff1->SetBinError  (ib+1, Ly_effErr) ;
	       //PadThr2
	       n_Ly_hitOn = hist_PadThr2->GetBinContent(ib+1) ;  
	       Ly_eff    = float(n_Ly_hitOn)/float(n_Ly_TrPrj);
	       
	       Ly_effErr = sqrt( fabs( n_Ly_hitOn) / n_Ly_TrPrj ) *
	  	 	    sqrt( 1. - fabs( n_Ly_hitOn) / n_Ly_TrPrj ) /
	  		    sqrt( n_Ly_TrPrj ) ;
	       hist_PadThr_eff2->SetBinContent(ib+1, Ly_eff)    ;
	       hist_PadThr_eff2->SetBinError  (ib+1, Ly_effErr) ;
	       //PadThr3
	       n_Ly_hitOn = hist_PadThr3->GetBinContent(ib+1) ;  
	       Ly_eff    = float(n_Ly_hitOn)/float(n_Ly_TrPrj);
	       
	       Ly_effErr = sqrt( fabs( n_Ly_hitOn) / n_Ly_TrPrj ) *
	  	 	    sqrt( 1. - fabs( n_Ly_hitOn) / n_Ly_TrPrj ) /
	  		    sqrt( n_Ly_TrPrj ) ;
	       hist_PadThr_eff3->SetBinContent(ib+1, Ly_eff)    ;
	       hist_PadThr_eff3->SetBinError  (ib+1, Ly_effErr) ;
	       //PadThr4
	       n_Ly_hitOn = hist_PadThr4->GetBinContent(ib+1) ;  
	       Ly_eff    = float(n_Ly_hitOn)/float(n_Ly_TrPrj);
	       
	       Ly_effErr = sqrt( fabs( n_Ly_hitOn) / n_Ly_TrPrj ) *
	  	 	    sqrt( 1. - fabs( n_Ly_hitOn) / n_Ly_TrPrj ) /
	  		    sqrt( n_Ly_TrPrj ) ;
	       hist_PadThr_eff4->SetBinContent(ib+1, Ly_eff)    ;
	       hist_PadThr_eff4->SetBinError  (ib+1, Ly_effErr) ;
	       //PadThr5
	       n_Ly_hitOn = hist_PadThr5->GetBinContent(ib+1) ;  
	       Ly_eff    = float(n_Ly_hitOn)/float(n_Ly_TrPrj);
	       
	       Ly_effErr = sqrt( fabs( n_Ly_hitOn) / n_Ly_TrPrj ) *
	  	 	    sqrt( 1. - fabs( n_Ly_hitOn) / n_Ly_TrPrj ) /
	  		    sqrt( n_Ly_TrPrj ) ;
	       hist_PadThr_eff5->SetBinContent(ib+1, Ly_eff)    ;
	       hist_PadThr_eff5->SetBinError  (ib+1, Ly_effErr) ;
	       //PhiEtaCoinThr0
	       n_Ly_hitOn = hist_PhiEtaCoinThr0->GetBinContent(ib+1) ;  
	       Ly_eff    = float(n_Ly_hitOn)/float(n_Ly_TrPrj);
	       
	       Ly_effErr = sqrt( fabs( n_Ly_hitOn) / n_Ly_TrPrj ) *
	  		   sqrt( 1. - fabs( n_Ly_hitOn) / n_Ly_TrPrj ) /
	  		   sqrt( n_Ly_TrPrj ) ;
	       hist_PhiEtaCoinThr_eff0->SetBinContent(ib+1, Ly_eff)    ;
	       hist_PhiEtaCoinThr_eff0->SetBinError  (ib+1, Ly_effErr) ;
	       //PhiEtaCoinThr1
	       n_Ly_hitOn = hist_PhiEtaCoinThr1->GetBinContent(ib+1) ;  
	       Ly_eff    = float(n_Ly_hitOn)/float(n_Ly_TrPrj);
	       
	       Ly_effErr = sqrt( fabs( n_Ly_hitOn) / n_Ly_TrPrj ) *
	  	 	    sqrt( 1. - fabs( n_Ly_hitOn) / n_Ly_TrPrj ) /
	  		    sqrt( n_Ly_TrPrj ) ;
	       hist_PhiEtaCoinThr_eff1->SetBinContent(ib+1, Ly_eff)    ;
	       hist_PhiEtaCoinThr_eff1->SetBinError  (ib+1, Ly_effErr) ;
	       //PhiEtaCoinThr2
	       n_Ly_hitOn = hist_PhiEtaCoinThr2->GetBinContent(ib+1) ;  
	       Ly_eff    = float(n_Ly_hitOn)/float(n_Ly_TrPrj);
	       
	       Ly_effErr = sqrt( fabs( n_Ly_hitOn) / n_Ly_TrPrj ) *
	  	 	    sqrt( 1. - fabs( n_Ly_hitOn) / n_Ly_TrPrj ) /
	  		    sqrt( n_Ly_TrPrj ) ;
	       hist_PhiEtaCoinThr_eff2->SetBinContent(ib+1, Ly_eff)    ;
	       hist_PhiEtaCoinThr_eff2->SetBinError  (ib+1, Ly_effErr) ;
	       //PhiEtaCoinThr3
	       n_Ly_hitOn = hist_PhiEtaCoinThr3->GetBinContent(ib+1) ;  
	       Ly_eff    = float(n_Ly_hitOn)/float(n_Ly_TrPrj);
	       
	       Ly_effErr = sqrt( fabs( n_Ly_hitOn) / n_Ly_TrPrj ) *
	  	 	    sqrt( 1. - fabs( n_Ly_hitOn) / n_Ly_TrPrj ) /
	  		    sqrt( n_Ly_TrPrj ) ;
	       hist_PhiEtaCoinThr_eff3->SetBinContent(ib+1, Ly_eff)    ;
	       hist_PhiEtaCoinThr_eff3->SetBinError  (ib+1, Ly_effErr) ;
	       //PhiEtaCoinThr4
	       n_Ly_hitOn = hist_PhiEtaCoinThr4->GetBinContent(ib+1) ;  
	       Ly_eff    = float(n_Ly_hitOn)/float(n_Ly_TrPrj);
	       
	       Ly_effErr = sqrt( fabs( n_Ly_hitOn) / n_Ly_TrPrj ) *
	  	 	    sqrt( 1. - fabs( n_Ly_hitOn) / n_Ly_TrPrj ) /
	  		    sqrt( n_Ly_TrPrj ) ;
	       hist_PhiEtaCoinThr_eff4->SetBinContent(ib+1, Ly_eff)    ;
	       hist_PhiEtaCoinThr_eff4->SetBinError  (ib+1, Ly_effErr) ;
	       //PhiEtaCoinThr5
	       n_Ly_hitOn = hist_PhiEtaCoinThr5->GetBinContent(ib+1) ;  
	       Ly_eff    = float(n_Ly_hitOn)/float(n_Ly_TrPrj);
	       
	       Ly_effErr = sqrt( fabs( n_Ly_hitOn) / n_Ly_TrPrj ) *
	  	 	    sqrt( 1. - fabs( n_Ly_hitOn) / n_Ly_TrPrj ) /
	  		    sqrt( n_Ly_TrPrj ) ;
	       hist_PhiEtaCoinThr_eff5->SetBinContent(ib+1, Ly_eff)    ;
	       hist_PhiEtaCoinThr_eff5->SetBinError  (ib+1, Ly_effErr) ;
	     }
	     
	     
	            
	  }
	
	  // write out histogram
	  TDirectory* dir = f->GetDirectory( dir_trigger_track.c_str() ) ;  
	  if ( dir != 0 ) {
	    dir->cd() ;
	    hist_MuctpiThr_eff0->Write("",TObject::kOverwrite);
	    hist_MuctpiThr_eff1->Write("",TObject::kOverwrite);
	    hist_MuctpiThr_eff2->Write("",TObject::kOverwrite);
	    hist_MuctpiThr_eff3->Write("",TObject::kOverwrite);
	    hist_MuctpiThr_eff4->Write("",TObject::kOverwrite);
	    hist_MuctpiThr_eff5->Write("",TObject::kOverwrite);
	    hist_PadThr_eff0->Write("",TObject::kOverwrite);
	    hist_PadThr_eff1->Write("",TObject::kOverwrite);
	    hist_PadThr_eff2->Write("",TObject::kOverwrite);
	    hist_PadThr_eff3->Write("",TObject::kOverwrite);
	    hist_PadThr_eff4->Write("",TObject::kOverwrite);
	    hist_PadThr_eff5->Write("",TObject::kOverwrite);
	    hist_PhiEtaCoinThr_eff0->Write("",TObject::kOverwrite);
	    hist_PhiEtaCoinThr_eff1->Write("",TObject::kOverwrite);
	    hist_PhiEtaCoinThr_eff2->Write("",TObject::kOverwrite);
	    hist_PhiEtaCoinThr_eff3->Write("",TObject::kOverwrite);
	    hist_PhiEtaCoinThr_eff4->Write("",TObject::kOverwrite);
	    hist_PhiEtaCoinThr_eff5->Write("",TObject::kOverwrite);
	  }
	}

	
	// layer efficiency
	std::string LyHit_name, LyPrj_name, LyEff_name ;
	LyHit_name = dir_glob_track   + "Hit_on_Track_Layer"       ; 
	LyPrj_name = dir_glob_track   + "Track_Projected_on_Layer" ;
	LyEff_name = dir_glob_track   + "Layer_Efficiency"         ;  
	
	if ( RPCCheckHistogram(f,LyHit_name.c_str()) && RPCCheckHistogram(f,LyPrj_name.c_str()) && RPCCheckHistogram(f,LyEff_name.c_str()) ) {
	  TH1I* hist_LyHit = (TH1I*)(f->Get( LyHit_name.c_str()));
	  TH1I* hist_LyPrj = (TH1I*)(f->Get( LyPrj_name.c_str()));
	  TH1F* hist_LyEff = (TH1F*)(f->Get( LyEff_name.c_str()));
	
	  int nb = hist_LyEff->GetNbinsX() ;
	  double  Ly_eff, Ly_effErr ;
	  for ( int ib=0; ib!=nb; ib++ ) {
	     float n_Ly_hitOn = hist_LyHit->GetBinContent(ib+1) ;  
	     float n_Ly_TrPrj = hist_LyPrj->GetBinContent(ib+1) ;
	     
	     if ( n_Ly_TrPrj >0 ) {
	       Ly_eff    = float(n_Ly_hitOn)/float(n_Ly_TrPrj);
	       
	       Ly_effErr = sqrt( fabs( n_Ly_hitOn) / n_Ly_TrPrj ) *
	  		   sqrt( 1. - fabs( n_Ly_hitOn) / n_Ly_TrPrj ) /
	  		   sqrt( n_Ly_TrPrj ) ;
	       hist_LyEff->SetBinContent(ib+1, Ly_eff)    ;
	       hist_LyEff->SetBinError  (ib+1, Ly_effErr) ;
	     }       
	  }
	
	  // write out histogram
	  TDirectory* dir = f->GetDirectory( dir_glob_track.c_str() ) ;  
	  if ( dir != 0 ) {
	    dir->cd() ;
	    //hist_LyEff->Write();
	    hist_LyEff->Write("",TObject::kOverwrite);
	  }
	}
	
	
	// layer efficiency Side A
	std::string LyHit_SideA_name, LyPrj_SideA_name, LyEff_SideA_name ;
	LyHit_SideA_name = dir_sideA_track   + "Layer_HitOnTrack_sideA"; 
	LyPrj_SideA_name = dir_sideA_track   + "Layer_TrackProj_sideA" ;
	LyEff_SideA_name = dir_sideA_track   + "Layer_Efficiency_sideA";  
	
	if ( RPCCheckHistogram(f,LyHit_SideA_name.c_str()) && RPCCheckHistogram(f,LyPrj_SideA_name.c_str()) && RPCCheckHistogram(f,LyEff_SideA_name.c_str()) ) {
	  TH1I* hist_LyHit_SideA = (TH1I*)(f->Get( LyHit_SideA_name.c_str()));
	  TH1I* hist_LyPrj_SideA = (TH1I*)(f->Get( LyPrj_SideA_name.c_str()));
	  TH1F* hist_LyEff_SideA = (TH1F*)(f->Get( LyEff_SideA_name.c_str()));
	
	  int nb = hist_LyEff_SideA->GetNbinsX() ;
	  double  Ly_eff_SideA, Ly_effErr_SideA ;
	  for ( int ib=0; ib!=nb; ib++ ) {
	     float n_Ly_hitOn_SideA = hist_LyHit_SideA->GetBinContent(ib+1) ;  
	     float n_Ly_TrPrj_SideA = hist_LyPrj_SideA->GetBinContent(ib+1) ;
	     
	     if ( n_Ly_TrPrj_SideA >0 ) {
	       Ly_eff_SideA    = float(n_Ly_hitOn_SideA)/float(n_Ly_TrPrj_SideA);
	       
	       Ly_effErr_SideA = sqrt( fabs( n_Ly_hitOn_SideA) / n_Ly_TrPrj_SideA ) *
	  		   sqrt( 1. - fabs( n_Ly_hitOn_SideA) / n_Ly_TrPrj_SideA ) /
	  		   sqrt( n_Ly_TrPrj_SideA ) ;
	       hist_LyEff_SideA->SetBinContent(ib+1, Ly_eff_SideA)    ;
	       hist_LyEff_SideA->SetBinError  (ib+1, Ly_effErr_SideA) ;
	     }       
	  }
	
	  // write out histogram
	  TDirectory* dir = f->GetDirectory( dir_sideA_track.c_str() ) ;  
	  if ( dir != 0 ) {
	    dir->cd() ;
	    //hist_LyEff->Write();
	    hist_LyEff_SideA->Write("",TObject::kOverwrite);
	  }
	}
		
	// layer efficiency Side C
	std::string LyHit_SideC_name, LyPrj_SideC_name, LyEff_SideC_name ;
	LyHit_SideC_name = dir_sideC_track   + "Layer_HitOnTrack_sideC"; 
	LyPrj_SideC_name = dir_sideC_track   + "Layer_TrackProj_sideC" ;
	LyEff_SideC_name = dir_sideC_track   + "Layer_Efficiency_sideC";  
	
	if ( RPCCheckHistogram(f,LyHit_SideC_name.c_str()) && RPCCheckHistogram(f,LyPrj_SideC_name.c_str()) && RPCCheckHistogram(f,LyEff_SideC_name.c_str()) ) {
	  TH1I* hist_LyHit_SideC = (TH1I*)(f->Get( LyHit_SideC_name.c_str()));
	  TH1I* hist_LyPrj_SideC = (TH1I*)(f->Get( LyPrj_SideC_name.c_str()));
	  TH1F* hist_LyEff_SideC = (TH1F*)(f->Get( LyEff_SideC_name.c_str()));
	
	  int nb = hist_LyEff_SideC->GetNbinsX() ;
	  double  Ly_eff_SideC, Ly_effErr_SideC ;
	  for ( int ib=0; ib!=nb; ib++ ) {
	     float n_Ly_hitOn_SideC = hist_LyHit_SideC->GetBinContent(ib+1) ;  
	     float n_Ly_TrPrj_SideC = hist_LyPrj_SideC->GetBinContent(ib+1) ;
	     
	     if ( n_Ly_TrPrj_SideC >0 ) {
	       Ly_eff_SideC    = float(n_Ly_hitOn_SideC)/float(n_Ly_TrPrj_SideC);
	       
	       Ly_effErr_SideC = sqrt( fabs( n_Ly_hitOn_SideC) / n_Ly_TrPrj_SideC ) *
	  		   sqrt( 1. - fabs( n_Ly_hitOn_SideC) / n_Ly_TrPrj_SideC ) /
	  		   sqrt( n_Ly_TrPrj_SideC ) ;
	       hist_LyEff_SideC->SetBinContent(ib+1, Ly_eff_SideC)    ;
	       hist_LyEff_SideC->SetBinError  (ib+1, Ly_effErr_SideC) ;
	     }       
	  }	
	  // write out histogram
	  TDirectory* dir = f->GetDirectory( dir_sideC_track.c_str() ) ;  
	  if ( dir != 0 ) {
	    dir->cd() ;
	    //hist_LyEff->Write();
	    hist_LyEff_SideC->Write("",TObject::kOverwrite);
	  }
	}

			
	
	int rpc_eventstotal = 0;
	if ( RPCCheckHistogram(f,(dir_ov_raw + "Number_of_RPC_hits_per_event").c_str() ) ) {
	  TH1I* rpc_hits = (TH1I*)(f->Get((dir_ov_raw + "Number_of_RPC_hits_per_event").c_str() ) ) ;
	  rpc_eventstotal = int(rpc_hits->GetEntries() );
	}
	
	
	// distribution plot all atlas 
	std::string AverageEff_C_name		= dir_sideC_track   + "Efficiency_Distribution_sideC" 		;
	std::string AverageEff_A_name		= dir_sideA_track   + "Efficiency_Distribution_sideA"		;
	std::string AverageGapEff_C_name	= dir_sideC_track   + "GapEfficiency_Distribution_sideC"	;
	std::string AverageGapEff_A_name	= dir_sideA_track   + "GapEfficiency_Distribution_sideA"	;
	std::string AverageNoiseCorr_C_name	= dir_sideC_track   + "NoiseCorr_Distribution_sideC"	   	;
	std::string AverageNoiseCorr_A_name	= dir_sideA_track   + "NoiseCorr_Distribution_sideA"	   	;
	std::string AverageNoiseTot_C_name	= dir_sideC_track   + "NoiseTot_Distribution_sideC"		;
	std::string AverageNoiseTot_A_name	= dir_sideA_track   + "NoiseTot_Distribution_sideA"		;
	std::string AverageCS_C_name		= dir_sideC_track   + "CS_Distribution_sideC"			;
	std::string AverageCS_A_name		= dir_sideA_track   + "CS_Distribution_sideA"			;
	std::string AverageRes_CS1_C_name	= dir_sideC_track   + "Res_CS1_Distribution_sideC"        	;
	std::string AverageRes_CS1_A_name	= dir_sideA_track   + "Res_CS1_Distribution_sideA"        	;
	std::string AverageRes_CS2_C_name	= dir_sideC_track   + "Res_CS2_Distribution_sideC"        	;
	std::string AverageRes_CS2_A_name	= dir_sideA_track   + "Res_CS2_Distribution_sideA"        	;
	std::string AverageRes_CSmore2_C_name	= dir_sideC_track   + "Res_CSmore2_Distribution_sideC"	  	;
	std::string AverageRes_CSmore2_A_name	= dir_sideA_track   + "Res_CSmore2_Distribution_sideA"	  	;
	std::string AverageRes_CS1rms_C_name	= dir_sideC_track   + "Res_CS1rms_Distribution_sideC"	  	;
	std::string AverageRes_CS1rms_A_name	= dir_sideA_track   + "Res_CS1rms_Distribution_sideA"	  	;
	std::string AverageRes_CS2rms_C_name	= dir_sideC_track   + "Res_CS2rms_Distribution_sideC"	  	;
	std::string AverageRes_CS2rms_A_name	= dir_sideA_track   + "Res_CS2rms_Distribution_sideA"	  	;
	std::string AverageRes_CSmore2rms_C_name= dir_sideC_track   + "Res_CSmore2rms_Distribution_sideC"       ;
	std::string AverageRes_CSmore2rms_A_name= dir_sideA_track   + "Res_CSmore2rms_Distribution_sideA"       ;
	std::string AverageOccupancy_C_name	= dir_sideC_track   + "Occupancy_Distribution_sideC"	 	;
	std::string AverageOccupancy_A_name	= dir_sideA_track   + "Occupancy_Distribution_sideA"	 	;
	std::string AverageTime_C_name		= dir_sideC_track   + "Time_Distribution_sideC"	    		;
	std::string AverageTime_A_name		= dir_sideA_track   + "Time_Distribution_sideA"	    		;
	
	TH1F* h_AverageEff_C		        = NULL;
	TH1F* h_AverageEff_A		        = NULL;
	TH1F* h_AverageGapEff_C		        = NULL;
	TH1F* h_AverageGapEff_A		        = NULL;
	TH1F* h_AverageNoiseCorr_C	        = NULL;
	TH1F* h_AverageNoiseCorr_A	        = NULL;
	TH1F* h_AverageNoiseTot_C 	        = NULL;
	TH1F* h_AverageNoiseTot_A	        = NULL;
	TH1F* h_AverageCS_C		        = NULL;
	TH1F* h_AverageCS_A		        = NULL;
	TH1F* h_AverageRes_CS1_C  	        = NULL;
	TH1F* h_AverageRes_CS1_A  	        = NULL;
	TH1F* h_AverageRes_CS2_C  	        = NULL;
	TH1F* h_AverageRes_CS2_A  	        = NULL;
	TH1F* h_AverageRes_CSmore2_C	        = NULL;
	TH1F* h_AverageRes_CSmore2_A	        = NULL;
	TH1F* h_AverageRes_CS1rms_C	        = NULL;
	TH1F* h_AverageRes_CS1rms_A	        = NULL;
	TH1F* h_AverageRes_CS2rms_C	        = NULL;
	TH1F* h_AverageRes_CS2rms_A	        = NULL;
	TH1F* h_AverageRes_CSmore2rms_C	        = NULL;
	TH1F* h_AverageRes_CSmore2rms_A	        = NULL;
	TH1F* h_AverageOccupancy_C	        = NULL;
	TH1F* h_AverageOccupancy_A	        = NULL;
	TH1F* h_AverageTime_C		        = NULL;
	TH1F* h_AverageTime_A		        = NULL;
	
	
	if ( RPCCheckHistogram( f, AverageEff_C_name.c_str()	      	) ){ h_AverageEff_C 	   	= (TH1F*) ( f->Get( AverageEff_C_name.c_str()  	  	 ) );}
	if ( RPCCheckHistogram( f, AverageEff_A_name.c_str()	      	) ){ h_AverageEff_A 	   	= (TH1F*) ( f->Get( AverageEff_A_name.c_str()  	  	 ) );}
	if ( RPCCheckHistogram( f, AverageGapEff_C_name.c_str()       	) ){ h_AverageGapEff_C	   	= (TH1F*) ( f->Get( AverageGapEff_C_name.c_str()	 ) );}
	if ( RPCCheckHistogram( f, AverageGapEff_A_name.c_str()       	) ){ h_AverageGapEff_A	   	= (TH1F*) ( f->Get( AverageGapEff_A_name.c_str()	 ) );}
	if ( RPCCheckHistogram( f, AverageNoiseCorr_C_name.c_str()    	) ){ h_AverageNoiseCorr_C  	= (TH1F*) ( f->Get( AverageNoiseCorr_C_name.c_str()	 ) );}
	if ( RPCCheckHistogram( f, AverageNoiseCorr_A_name.c_str()    	) ){ h_AverageNoiseCorr_A  	= (TH1F*) ( f->Get( AverageNoiseCorr_A_name.c_str()	 ) );}
	if ( RPCCheckHistogram( f, AverageNoiseTot_C_name.c_str()     	) ){ h_AverageNoiseTot_C	= (TH1F*) ( f->Get( AverageNoiseTot_C_name.c_str()	 ) );}
	if ( RPCCheckHistogram( f, AverageNoiseTot_A_name.c_str()     	) ){ h_AverageNoiseTot_A	= (TH1F*) ( f->Get( AverageNoiseTot_A_name.c_str()	 ) );}
	if ( RPCCheckHistogram( f, AverageCS_C_name.c_str()	      	) ){ h_AverageCS_C  	   	= (TH1F*) ( f->Get( AverageCS_C_name.c_str()		 ) );}
	if ( RPCCheckHistogram( f, AverageCS_A_name.c_str()	      	) ){ h_AverageCS_A  	   	= (TH1F*) ( f->Get( AverageCS_A_name.c_str()		 ) );}
	if ( RPCCheckHistogram( f, AverageRes_CS1_C_name.c_str()      	) ){ h_AverageRes_CS1_C	   	= (TH1F*) ( f->Get( AverageRes_CS1_C_name.c_str()	 ) );}
	if ( RPCCheckHistogram( f, AverageRes_CS1_A_name.c_str()      	) ){ h_AverageRes_CS1_A	   	= (TH1F*) ( f->Get( AverageRes_CS1_A_name.c_str()	 ) );}
	if ( RPCCheckHistogram( f, AverageRes_CS2_C_name.c_str()      	) ){ h_AverageRes_CS2_C	   	= (TH1F*) ( f->Get( AverageRes_CS2_C_name.c_str()	 ) );}
	if ( RPCCheckHistogram( f, AverageRes_CS2_A_name.c_str()      	) ){ h_AverageRes_CS2_A	   	= (TH1F*) ( f->Get( AverageRes_CS2_A_name.c_str()	 ) );}
	if ( RPCCheckHistogram( f, AverageRes_CSmore2_C_name.c_str()    ) ){ h_AverageRes_CSmore2_C    	= (TH1F*) ( f->Get( AverageRes_CSmore2_C_name.c_str()    ) );}
	if ( RPCCheckHistogram( f, AverageRes_CSmore2_A_name.c_str()    ) ){ h_AverageRes_CSmore2_A    	= (TH1F*) ( f->Get( AverageRes_CSmore2_A_name.c_str()    ) );}
        if ( RPCCheckHistogram( f, AverageRes_CS1rms_C_name.c_str()     ) ){ h_AverageRes_CS1rms_C     	= (TH1F*) ( f->Get( AverageRes_CS1rms_C_name.c_str()	 ) );}
	if ( RPCCheckHistogram( f, AverageRes_CS1rms_A_name.c_str()     ) ){ h_AverageRes_CS1rms_A     	= (TH1F*) ( f->Get( AverageRes_CS1rms_A_name.c_str()	 ) );}
	if ( RPCCheckHistogram( f, AverageRes_CS2rms_C_name.c_str()     ) ){ h_AverageRes_CS2rms_C     	= (TH1F*) ( f->Get( AverageRes_CS2rms_C_name.c_str()	 ) );}
	if ( RPCCheckHistogram( f, AverageRes_CS2rms_A_name.c_str()     ) ){ h_AverageRes_CS2rms_A     	= (TH1F*) ( f->Get( AverageRes_CS2rms_A_name.c_str()	 ) );}
	if ( RPCCheckHistogram( f, AverageRes_CSmore2rms_C_name.c_str() ) ){ h_AverageRes_CSmore2rms_C 	= (TH1F*) ( f->Get( AverageRes_CSmore2rms_C_name.c_str() ) );}
	if ( RPCCheckHistogram( f, AverageRes_CSmore2rms_A_name.c_str() ) ){ h_AverageRes_CSmore2rms_A 	= (TH1F*) ( f->Get( AverageRes_CSmore2rms_A_name.c_str() ) );}
	if ( RPCCheckHistogram( f, AverageOccupancy_C_name.c_str()      ) ){ h_AverageOccupancy_C	= (TH1F*) ( f->Get( AverageOccupancy_C_name.c_str()	 ) );}
	if ( RPCCheckHistogram( f, AverageOccupancy_A_name.c_str()      ) ){ h_AverageOccupancy_A	= (TH1F*) ( f->Get( AverageOccupancy_A_name.c_str()	 ) );}
	if ( RPCCheckHistogram( f, AverageTime_C_name.c_str()	      	) ){ h_AverageTime_C	   	= (TH1F*) ( f->Get( AverageTime_C_name.c_str() 	  	 ) );}
	if ( RPCCheckHistogram( f, AverageTime_A_name.c_str()	      	) ){ h_AverageTime_A	   	= (TH1F*) ( f->Get( AverageTime_A_name.c_str() 	  	 ) );}
	
	if ( h_AverageEff_C	       ) h_AverageEff_C 	  ->Reset() ;
	if ( h_AverageEff_A	       ) h_AverageEff_A 	  ->Reset() ;
	if ( h_AverageGapEff_C	       ) h_AverageGapEff_C	  ->Reset() ;
	if ( h_AverageGapEff_A	       ) h_AverageGapEff_A	  ->Reset() ;
	if ( h_AverageNoiseCorr_C      ) h_AverageNoiseCorr_C	  ->Reset() ;
	if ( h_AverageNoiseCorr_A      ) h_AverageNoiseCorr_A	  ->Reset() ;
	if ( h_AverageNoiseTot_C       ) h_AverageNoiseTot_C	  ->Reset() ;
	if ( h_AverageNoiseTot_A       ) h_AverageNoiseTot_A	  ->Reset() ;
	if ( h_AverageCS_C	       ) h_AverageCS_C  	  ->Reset() ;
	if ( h_AverageCS_A	       ) h_AverageCS_A  	  ->Reset() ;
	if ( h_AverageRes_CS1_C	       ) h_AverageRes_CS1_C	  ->Reset() ;
	if ( h_AverageRes_CS1_A	       ) h_AverageRes_CS1_A	  ->Reset() ;
	if ( h_AverageRes_CS2_C	       ) h_AverageRes_CS2_C	  ->Reset() ;
	if ( h_AverageRes_CS2_A	       ) h_AverageRes_CS2_A	  ->Reset() ;
	if ( h_AverageRes_CSmore2_C    ) h_AverageRes_CSmore2_C	  ->Reset() ;
	if ( h_AverageRes_CSmore2_A    ) h_AverageRes_CSmore2_A	  ->Reset() ;
	if ( h_AverageRes_CS1rms_C     ) h_AverageRes_CS1rms_C	  ->Reset() ;
	if ( h_AverageRes_CS1rms_A     ) h_AverageRes_CS1rms_A	  ->Reset() ;
	if ( h_AverageRes_CS2rms_C     ) h_AverageRes_CS2rms_C	  ->Reset() ;
	if ( h_AverageRes_CS2rms_A     ) h_AverageRes_CS2rms_A	  ->Reset() ;
	if ( h_AverageRes_CSmore2rms_C ) h_AverageRes_CSmore2rms_C->Reset() ;
	if ( h_AverageRes_CSmore2rms_A ) h_AverageRes_CSmore2rms_A->Reset() ;
	if ( h_AverageOccupancy_C      ) h_AverageOccupancy_C     ->Reset() ;
	if ( h_AverageOccupancy_A      ) h_AverageOccupancy_A     ->Reset() ;
	if ( h_AverageTime_C	       ) h_AverageTime_C  	  ->Reset() ;
	if ( h_AverageTime_A	       ) h_AverageTime_A  	  ->Reset() ;
	 
	// summary plots
	int countpanelindb = 0 ;
	int countpaneleff0 = 0 ;
	int countpaneltrack0 = 0 ;
	for (int i_sec=0; i_sec!=15*1+1; i_sec++) {
	  char sector_char[100]   ;
	  std::string sector_name ;
          sprintf(sector_char,"_Sector%.2d",i_sec+1) ;  // sector number with 2 digits
          sector_name = sector_char ;
	  std::cout << " RPC sector_name processing  "<< sector_name<<std::endl;
	  
	  std::string TrackProj_name		  	= dir_sum_track + "SummaryTrackProj"  		  	+ sector_name ;
	  std::string HitOnTrack_name		  	= dir_sum_track + "SummaryHitOnTrack" 		  	+ sector_name ;
	  std::string HitOnTrackCross_name	  	= dir_sum_track + "SummaryHitOnTrack_withCrossStrip"    + sector_name ;
	  std::string Eff_name			  	= dir_sum_track + "SummaryEfficiency" 		 	+ sector_name ;
	  std::string GapEff_name		  	= dir_sum_track + "SummaryGapEfficiency"		+ sector_name ;
	  std::string NoiseCorr_name		  	= dir_sum_track + "SummaryNoiseCorr"  		  	+ sector_name ;
	  std::string NoiseCorr_s_name		  	= dir_sum_track + "SummaryNoiseCorr_NotNorm"  		+ sector_name ;
	  std::string NoiseTot_name  		  	= dir_sum_track + "SummaryNoiseTot"			+ sector_name ;
	  std::string NoiseTot_s_name  		  	= dir_sum_track + "SummaryNoiseTot_NotNorm"		+ sector_name ;
	  std::string Res_CS1_name            	  	= dir_sum_track + "SummaryRes_CS1"			+ sector_name ;
	  std::string Res_CS1_s_name            	= dir_sum_track + "SummaryRes_CS1_NotNorm"		+ sector_name ;
	  std::string Res_CS1_square_name      	  	= dir_sum_track + "SummaryRes_CS1_square"		+ sector_name ;
	  std::string Res_CS1_entries_name     	  	= dir_sum_track + "SummaryRes_CS1_entries"		+ sector_name ;
	  std::string Res_CS2_name             	  	= dir_sum_track + "SummaryRes_CS2"			+ sector_name ;
	  std::string Res_CS2_s_name             	= dir_sum_track + "SummaryRes_CS2_NotNorm"		+ sector_name ;
	  std::string Res_CS2_square_name      	  	= dir_sum_track + "SummaryRes_CS2_square"		+ sector_name ;
	  std::string Res_CS2_entries_name     	  	= dir_sum_track + "SummaryRes_CS2_entries"		+ sector_name ;
	  std::string Res_CSmore2_name         	  	= dir_sum_track + "SummaryRes_CSmore2"		  	+ sector_name ;
	  std::string Res_CSmore2_s_name       	  	= dir_sum_track + "SummaryRes_CSmore2_NotNorm"	  	+ sector_name ;
	  std::string Res_CSmore2_square_name  	  	= dir_sum_track + "SummaryRes_CSmore2_square" 	  	+ sector_name ;
	  std::string Res_CSmore2_entries_name 	  	= dir_sum_track + "SummaryRes_CSmore2_entries"	  	+ sector_name ;
	  std::string CS_name			  	= dir_sum_track + "SummaryCS" 			  	+ sector_name ;
	  std::string CS_s_name			  	= dir_sum_track + "SummaryCS_NotNorm"		  	+ sector_name ;
	  std::string CS_square_name 		  	= dir_sum_track + "SummaryCS_square"  		  	+ sector_name ;
	  std::string CS_entries_name		  	= dir_sum_track + "SummaryCS_entries" 		  	+ sector_name ;
	  std::string CS1_entries_name		  	= dir_sum_track + "SummaryCS1_entries" 		  	+ sector_name ;
	  std::string CS2_entries_name		  	= dir_sum_track + "SummaryCS2_entries" 		  	+ sector_name ;
	  std::string Time_name			  	= dir_sum_track + "SummaryTime"			  	+ sector_name ;
	  std::string Time_s_name		  	= dir_sum_track + "SummaryTime_NotNorm"			+ sector_name ;
	  std::string Time_square_name		  	= dir_sum_track + "SummaryTime_square"		  	+ sector_name ;
	  std::string Occupancy_name   		  	= dir_sum_track + "SummaryOccupancy"  		  	+ sector_name ;
	  std::string Occupancy_s_name   		= dir_sum_track + "SummaryOccupancy_NotNorm"	  	+ sector_name ;
	  std::string PanelId_name                      = dir_sum_track + "SummaryPanelID"	                + sector_name ;
	  
	  // distribution plot per sector 
	  std::string EffSecDist_name			= dir_sum_track + "SummaryEffDistriPerSector"		+ sector_name ;		
	  std::string GapEffSecDist_name		= dir_sum_track + "SummaryGapEffDistriPerSector"	+ sector_name ;
	  std::string NoiseCorrSecDist_name		= dir_sum_track + "SummaryNoiseCorrDistriPerSector"	+ sector_name ;
	  std::string NoiseTotSecDist_name		= dir_sum_track + "SummaryNoiseTotDistriPerSector"	+ sector_name ;
	  std::string CSSecDist_name			= dir_sum_track + "SummaryCSDistriPerSector"		+ sector_name ;
	  std::string Res_CS1SecDist_name		= dir_sum_track + "SummaryRes_CS1DistriPerSector"	+ sector_name ;
	  std::string Res_CS2SecDist_name		= dir_sum_track + "SummaryRes_CS2DistriPerSector"	+ sector_name ;
	  std::string Res_CSmore2SecDist_name		= dir_sum_track + "SummaryRes_CSmore2DistriPerSector"	+ sector_name ;
	  std::string Res_CS1_rmsSecDist_name		= dir_sum_track + "SummaryRes_CS1rmsDistriPerSector"	+ sector_name ;
	  std::string Res_CS2_rmsSecDist_name		= dir_sum_track + "SummaryRes_CS2rmsDistriPerSector"	+ sector_name ;
	  std::string Res_CSmore2_rmsSecDist_name	= dir_sum_track + "SummaryRes_CSmore2rmsDistriPerSector"+ sector_name ;
	  std::string TimeSecDist_name			= dir_sum_track + "SummaryTimeDistriPerSector"		+ sector_name ;
	  std::string OccupancySecDist_name		= dir_sum_track + "SummaryOccupancyDistriPerSector"	+ sector_name ;
	  
	  TH1F* h_TrackProj	      	  = NULL;
	  TH1F* h_HitOnTrack          	  = NULL;
	  TH1F* h_HitOnTrackCross     	  = NULL;
	  TH1F* h_Eff	      	      	  = NULL;
	  TH1F* h_GapEff	      	  = NULL;
	  TH1F* h_NoiseCorr           	  = NULL;
	  TH1F* h_NoiseCorr_s          	  = NULL;
	  TH1F* h_NoiseTot            	  = NULL;
	  TH1F* h_NoiseTot_s           	  = NULL;
	  TH1F* h_Res_CS1                 = NULL;   
	  TH1F* h_Res_CS1_s               = NULL;
	  TH1F* h_Res_CS1_square          = NULL;   
	  TH1F* h_Res_CS1_entries     	  = NULL;  
	  TH1F* h_Res_CS2  	      	  = NULL; 
	  TH1F* h_Res_CS2_s  	      	  = NULL; 
	  TH1F* h_Res_CS2_square          = NULL;   
	  TH1F* h_Res_CS2_entries     	  = NULL;  
	  TH1F* h_Res_CSmore2	      	  = NULL;
	  TH1F* h_Res_CSmore2_s	      	  = NULL; 
	  TH1F* h_Res_CSmore2_square  	  = NULL;
	  TH1F* h_Res_CSmore2_entries 	  = NULL;
	  TH1F* h_CS                  	  = NULL;
	  TH1F* h_CS_s                 	  = NULL;
	  TH1F* h_CS_square           	  = NULL;
	  TH1F* h_CS_entries          	  = NULL;
	  TH1F* h_CS1_entries          	  = NULL;
	  TH1F* h_CS2_entries          	  = NULL;
	  TH1F* h_Time                	  = NULL;
	  TH1F* h_Time_s               	  = NULL;
	  TH1F* h_Time_square         	  = NULL;
	  TH1F* h_Occupancy           	  = NULL;
	  TH1F* h_Occupancy_s          	  = NULL;
	  TH1F* h_PanelId		  = NULL;
	  
	  TH1F* h_EffSecDist	          = NULL;
	  TH1F* h_GapEffSecDist	          = NULL;
	  TH1F* h_NoiseCorrSecDist        = NULL;  
	  TH1F* h_NoiseTotSecDist         = NULL; 
	  TH1F* h_CSSecDist	          = NULL;
	  TH1F* h_Res_CS1SecDist	  = NULL;    
	  TH1F* h_Res_CS2SecDist	  = NULL;    
	  TH1F* h_Res_CSmore2SecDist      = NULL;
	  TH1F* h_Res_CS1_rmsSecDist      = NULL; 
	  TH1F* h_Res_CS2_rmsSecDist      = NULL;
	  TH1F* h_Res_CSmore2_rmsSecDist  = NULL;
	  TH1F* h_TimeSecDist	          = NULL; 
	  TH1F* h_OccupancySecDist        = NULL;
	  
	  if ( RPCCheckHistogram( f, TrackProj_name.c_str()	     	) ){ h_TrackProj	      = (TH1F*) ( f->Get( TrackProj_name.c_str()	      ) );}
	  if ( RPCCheckHistogram( f, HitOnTrack_name.c_str()	     	) ){ h_HitOnTrack	      = (TH1F*) ( f->Get( HitOnTrack_name.c_str()	      ) );}
	  if ( RPCCheckHistogram( f, HitOnTrackCross_name.c_str()    	) ){ h_HitOnTrackCross        = (TH1F*) ( f->Get( HitOnTrackCross_name.c_str()        ) );}
	  if ( RPCCheckHistogram( f, Eff_name.c_str()		     	) ){ h_Eff		      = (TH1F*) ( f->Get( Eff_name.c_str()		      ) );}
	  if ( RPCCheckHistogram( f, GapEff_name.c_str()	     	) ){ h_GapEff		      = (TH1F*) ( f->Get( GapEff_name.c_str()		      ) );}
	  if ( RPCCheckHistogram( f, NoiseCorr_name.c_str()	     	) ){ h_NoiseCorr	      = (TH1F*) ( f->Get( NoiseCorr_name.c_str()	      ) );}
	  if ( RPCCheckHistogram( f, NoiseCorr_s_name.c_str()	     	) ){ h_NoiseCorr_s	      = (TH1F*) ( f->Get( NoiseCorr_s_name.c_str()	      ) );}
	  if ( RPCCheckHistogram( f, NoiseTot_name.c_str()	     	) ){ h_NoiseTot 	      = (TH1F*) ( f->Get( NoiseTot_name.c_str() 	      ) );}
	  if ( RPCCheckHistogram( f, NoiseTot_s_name.c_str()	     	) ){ h_NoiseTot_s 	      = (TH1F*) ( f->Get( NoiseTot_s_name.c_str() 	      ) );}
	  if ( RPCCheckHistogram( f, Res_CS1_name.c_str()	     	) ){ h_Res_CS1  	      = (TH1F*) ( f->Get( Res_CS1_name.c_str()  	      ) );}
	  if ( RPCCheckHistogram( f, Res_CS1_s_name.c_str()	     	) ){ h_Res_CS1_s  	      = (TH1F*) ( f->Get( Res_CS1_s_name.c_str()  	      ) );}
	  if ( RPCCheckHistogram( f, Res_CS1_square_name.c_str()     	) ){ h_Res_CS1_square	      = (TH1F*) ( f->Get( Res_CS1_square_name.c_str()	      ) );}
	  if ( RPCCheckHistogram( f, Res_CS1_entries_name.c_str()    	) ){ h_Res_CS1_entries        = (TH1F*) ( f->Get( Res_CS1_entries_name.c_str()        ) );}
	  if ( RPCCheckHistogram( f, Res_CS2_name.c_str()	     	) ){ h_Res_CS2  	      = (TH1F*) ( f->Get( Res_CS2_name.c_str()  	      ) );} 
	  if ( RPCCheckHistogram( f, Res_CS2_s_name.c_str()	     	) ){ h_Res_CS2_s  	      = (TH1F*) ( f->Get( Res_CS2_s_name.c_str()  	      ) );} 
	  if ( RPCCheckHistogram( f, Res_CS2_square_name .c_str()    	) ){ h_Res_CS2_square	      = (TH1F*) ( f->Get( Res_CS2_square_name .c_str()        ) );} 
	  if ( RPCCheckHistogram( f, Res_CS2_entries_name.c_str()    	) ){ h_Res_CS2_entries        = (TH1F*) ( f->Get( Res_CS2_entries_name.c_str()        ) );} 
	  if ( RPCCheckHistogram( f, Res_CSmore2_name.c_str()	     	) ){ h_Res_CSmore2	      = (TH1F*) ( f->Get( Res_CSmore2_name.c_str()	      ) );} 
	  if ( RPCCheckHistogram( f, Res_CSmore2_s_name.c_str()	     	) ){ h_Res_CSmore2_s	      = (TH1F*) ( f->Get( Res_CSmore2_s_name.c_str()	      ) );} 
	  if ( RPCCheckHistogram( f, Res_CSmore2_square_name.c_str() 	) ){ h_Res_CSmore2_square     = (TH1F*) ( f->Get( Res_CSmore2_square_name.c_str()     ) );}
	  if ( RPCCheckHistogram( f, Res_CSmore2_entries_name.c_str()	) ){ h_Res_CSmore2_entries    = (TH1F*) ( f->Get( Res_CSmore2_entries_name.c_str()    ) );}
	  if ( RPCCheckHistogram( f, CS_name.c_str()	 		) ){ h_CS	              = (TH1F*) ( f->Get( CS_name.c_str()		      ) );}
	  if ( RPCCheckHistogram( f, CS_s_name.c_str()	 		) ){ h_CS_s	              = (TH1F*) ( f->Get( CS_s_name.c_str()		      ) );}
	  if ( RPCCheckHistogram( f, CS_square_name .c_str()   		) ){ h_CS_square    	      = (TH1F*) ( f->Get( CS_square_name .c_str()	      ) );}
	  if ( RPCCheckHistogram( f, CS_entries_name.c_str()  		) ){ h_CS_entries  	      = (TH1F*) ( f->Get( CS_entries_name.c_str()	      ) );}
	  if ( RPCCheckHistogram( f, CS1_entries_name.c_str()  		) ){ h_CS1_entries  	      = (TH1F*) ( f->Get( CS1_entries_name.c_str()	      ) );}
	  if ( RPCCheckHistogram( f, CS2_entries_name.c_str()  		) ){ h_CS2_entries  	      = (TH1F*) ( f->Get( CS2_entries_name.c_str()	      ) );}
	  if ( RPCCheckHistogram( f, Time_name.c_str()    		) ){ h_Time        	      = (TH1F*) ( f->Get( Time_name.c_str()		      ) );}
	  if ( RPCCheckHistogram( f, Time_s_name.c_str()    		) ){ h_Time_s        	      = (TH1F*) ( f->Get( Time_s_name.c_str()		      ) );}
	  if ( RPCCheckHistogram( f, Time_square_name.c_str() 		) ){ h_Time_square  	      = (TH1F*) ( f->Get( Time_square_name.c_str()	      ) );}
	  if ( RPCCheckHistogram( f, Occupancy_name.c_str()  		) ){ h_Occupancy   	      = (TH1F*) ( f->Get( Occupancy_name.c_str()	      ) );}
	  if ( RPCCheckHistogram( f, Occupancy_s_name.c_str()  		) ){ h_Occupancy_s   	      = (TH1F*) ( f->Get( Occupancy_s_name.c_str()	      ) );}
	  if ( RPCCheckHistogram( f, PanelId_name.c_str()               ) ){ h_PanelId                = (TH1F*) ( f->Get( PanelId_name.c_str()                ) );}
	  
	  if ( RPCCheckHistogram( f, EffSecDist_name.c_str()	     	) ){ h_EffSecDist	      = (TH1F*) ( f->Get( EffSecDist_name.c_str()	      ) );}   
	  if ( RPCCheckHistogram( f, GapEffSecDist_name.c_str()       	) ){ h_GapEffSecDist	      = (TH1F*) ( f->Get( GapEffSecDist_name.c_str()	      ) );}  
	  if ( RPCCheckHistogram( f, NoiseCorrSecDist_name.c_str()      ) ){ h_NoiseCorrSecDist       = (TH1F*) ( f->Get( NoiseCorrSecDist_name.c_str()       ) );}  
	  if ( RPCCheckHistogram( f, NoiseTotSecDist_name.c_str()       ) ){ h_NoiseTotSecDist        = (TH1F*) ( f->Get( NoiseTotSecDist_name.c_str()        ) );}  
	  if ( RPCCheckHistogram( f, CSSecDist_name.c_str()	        ) ){ h_CSSecDist	      = (TH1F*) ( f->Get( CSSecDist_name.c_str()	      ) );}  
	  if ( RPCCheckHistogram( f, Res_CS1SecDist_name.c_str()        ) ){ h_Res_CS1SecDist	      = (TH1F*) ( f->Get( Res_CS1SecDist_name.c_str()	      ) );}  
	  if ( RPCCheckHistogram( f, Res_CS2SecDist_name.c_str()        ) ){ h_Res_CS2SecDist	      = (TH1F*) ( f->Get( Res_CS2SecDist_name.c_str()	      ) );}	   
	  if ( RPCCheckHistogram( f, Res_CSmore2SecDist_name.c_str()    ) ){ h_Res_CSmore2SecDist     = (TH1F*) ( f->Get( Res_CSmore2SecDist_name.c_str()     ) );}	 
	  if ( RPCCheckHistogram( f, Res_CS1_rmsSecDist_name.c_str()    ) ){ h_Res_CS1_rmsSecDist     = (TH1F*) ( f->Get( Res_CS1_rmsSecDist_name.c_str()     ) );}   
	  if ( RPCCheckHistogram( f, Res_CS2_rmsSecDist_name.c_str()    ) ){ h_Res_CS2_rmsSecDist     = (TH1F*) ( f->Get( Res_CS2_rmsSecDist_name.c_str()     ) );}   
	  if ( RPCCheckHistogram( f, Res_CSmore2_rmsSecDist_name.c_str()) ){ h_Res_CSmore2_rmsSecDist = (TH1F*) ( f->Get( Res_CSmore2_rmsSecDist_name.c_str() ) );}  
	  if ( RPCCheckHistogram( f, TimeSecDist_name.c_str()	     	) ){ h_TimeSecDist	      = (TH1F*) ( f->Get( TimeSecDist_name.c_str()	      ) );}	 
	  if ( RPCCheckHistogram( f, OccupancySecDist_name.c_str()  	) ){ h_OccupancySecDist       = (TH1F*) ( f->Get( OccupancySecDist_name.c_str()       ) );}	    
	  
	  if ( h_EffSecDist	        ) h_EffSecDist  	  ->Reset() ;
	  if ( h_GapEffSecDist          ) h_GapEffSecDist	  ->Reset() ;
	  if ( h_NoiseCorrSecDist       ) h_NoiseCorrSecDist	  ->Reset() ;
	  if ( h_NoiseTotSecDist        ) h_NoiseTotSecDist	  ->Reset() ;
	  if ( h_CSSecDist	        ) h_CSSecDist		  ->Reset() ;
	  if ( h_Res_CS1SecDist         ) h_Res_CS1SecDist	  ->Reset() ;
	  if ( h_Res_CS2SecDist         ) h_Res_CS2SecDist	  ->Reset() ;
	  if ( h_Res_CSmore2SecDist     ) h_Res_CSmore2SecDist    ->Reset() ;
	  if ( h_Res_CS1_rmsSecDist     ) h_Res_CS1_rmsSecDist    ->Reset() ;
	  if ( h_Res_CS2_rmsSecDist     ) h_Res_CS2_rmsSecDist    ->Reset() ;
	  if ( h_Res_CSmore2_rmsSecDist ) h_Res_CSmore2_rmsSecDist->Reset() ;
	  if ( h_TimeSecDist	        ) h_TimeSecDist 	  ->Reset() ;
	  if ( h_OccupancySecDist       ) h_OccupancySecDist	  ->Reset() ;
	  if ( h_Eff    	        ) h_Eff         	  ->Reset() ;
	  
	  // efficiency
	  if ( h_TrackProj && h_HitOnTrack && h_Eff ) {
	    for ( int ib = 0; ib != h_TrackProj->GetNbinsX(); ib++ ) {
	      if ( h_PanelId ) { if ( (h_PanelId->GetBinContent(ib+1) )==0 ) { continue; } } 
	      
	      n_hit_f = h_HitOnTrack -> GetBinContent(ib+1) ;
	      n_tr_p  = h_TrackProj  -> GetBinContent(ib+1) ;
	      
	     //if ( n_tr_p>50 ) {    
	      if ( n_tr_p!=0) { panel_eff = n_hit_f/n_tr_p ; }
	      else { panel_eff=0. ; }
	      if ( n_tr_p!=0 ) {
	        panel_err_eff = sqrt( fabs(n_hit_f)/n_tr_p ) *
	                        sqrt( 1. - fabs(n_hit_f)/n_tr_p ) /
		                sqrt( n_tr_p ) ;
	      }
	      else { panel_err_eff=0. ; } 
	      h_Eff -> SetBinContent(ib+1, panel_eff) 	        ;
	      h_Eff -> SetBinError  (ib+1, panel_err_eff)	;
	      if ( h_EffSecDist ) h_EffSecDist->Fill(panel_eff) ;
	      if ( ib>( h_TrackProj->GetNbinsX()/2 ) ) { if ( h_AverageEff_A ) h_AverageEff_A->Fill(panel_eff) ; } 
	      else                                     { if ( h_AverageEff_C ) h_AverageEff_C->Fill(panel_eff) ; }
	    //}
	  }
	    // write out histogram
	    TDirectory* dir = f->GetDirectory( dir_sum_track.c_str() ) ;
	    if ( dir != 0 ) {
	      dir->cd() ;
	      h_Eff -> Write("",TObject::kOverwrite);
	      if ( h_EffSecDist ) h_EffSecDist ->Write("",TObject::kOverwrite) ;
	    }
	  }
	  
	  // gap efficiency
	  if ( h_TrackProj && h_HitOnTrack && h_HitOnTrackCross && h_GapEff ) {
	    h_GapEff->Reset();
	    nEta=0; nPhi=0; nEtaPhi=0;
	    int sign=1;
	    int bmin=0;
	    bmin = int( h_GapEff->GetXaxis()->GetXmin() ) ;
	    for ( int ib = 0; ib !=h_TrackProj->GetNbinsX(); ib++ ) {
	      if ( h_PanelId ) { if ( (h_PanelId->GetBinContent(ib+1) )==0 ) { continue; } }
	      if ( (bmin + ib )%2 != 0 ) continue; // Phi panel
	      if ( (bmin + ib )<0     ) { sign=-1; } 
	      else                      { sign= 1; }
	      
	      nEta    = h_HitOnTrack     ->GetBinContent( ib+1	      ) ;
	      nPhi    = h_HitOnTrack     ->GetBinContent( ib+1 + sign ) ;
	      nEtaPhi = h_HitOnTrackCross->GetBinContent( ib+1	      ) ;
	      n_tr_p  = h_TrackProj      ->GetBinContent( ib+1        ) ;
	      
	      if ( n_tr_p>0 ) { 
	        gapEff    = (nEta+nPhi-nEtaPhi) / n_tr_p  ; 
                //std::cout << " gapEff " << gapEff <<"  nEta  "<<nEta<<"  nPhi  "<<nPhi<< "  nEtaPhi  "<<nEtaPhi <<"  n_tr_p  "<<n_tr_p <<std::endl;
	
		gapErrEff = sqrt( fabs( nEta+nPhi-nEtaPhi)/n_tr_p ) *
	                    sqrt( 1. - fabs( nEta+nPhi-nEtaPhi)/n_tr_p ) /
		            sqrt( n_tr_p ) ;
                //std::cout <<"sqrt( fabs( nEta+nPhi-nEtaPhi -0.5)/n_tr_p )"<<sqrt( fabs( nEta+nPhi-nEtaPhi -0.5)/n_tr_p )<<"sqrt( 1. - fabs( nEta+nPhi-nEtaPhi -0.5)/n_tr_p )"<<sqrt( 1. - fabs( nEta+nPhi-nEtaPhi -0.5)/n_tr_p )<< " gapErrEff " << gapErrEff <<std::endl;
              }
	      else  { 
	        gapEff    = 0.; 
	        gapErrEff = 0.;
              }
	      h_GapEff -> SetBinContent(ib+1, gapEff    );
	      h_GapEff -> SetBinError  (ib+1, gapErrEff );
	      if ( h_GapEffSecDist ) h_GapEffSecDist->Fill(gapEff);
	      if ( ib>( h_TrackProj->GetNbinsX()/2 )  ) { if ( h_AverageGapEff_A ) h_AverageGapEff_A->Fill(gapEff); }
	      else                                      { if ( h_AverageGapEff_C ) h_AverageGapEff_C->Fill(gapEff); }
	    }
	    TDirectory* dir = f->GetDirectory( dir_sum_track.c_str() ) ;
	    if ( dir != 0 ) {
	      dir->cd() ;
	      h_GapEff -> Write("",TObject::kOverwrite) ;
	      if ( h_GapEffSecDist ) h_GapEffSecDist-> Write("",TObject::kOverwrite) ;
	    }
	  }
	  
	  
	  //residuals CS = 1
	  res_mean=0; res2_mean=0; res_RMS=0; 
	  if ( h_Res_CS1 && h_Res_CS1_s && h_Res_CS1_square && h_Res_CS1_entries ) {
	    for ( int ib = 0; ib != h_Res_CS1->GetNbinsX(); ib++ ) {
	      if ( h_PanelId ) { if ( (h_PanelId->GetBinContent(ib+1) )==0 ) { continue; }  }
	      if ( (h_Res_CS1_entries->GetBinContent(ib+1) )!=0 ) {
	        res_mean  = (h_Res_CS1_s->GetBinContent(ib+1) )/ (h_Res_CS1_entries->GetBinContent(ib+1) )       ;
	        res2_mean = (h_Res_CS1_square->GetBinContent(ib+1) )/ (h_Res_CS1_entries->GetBinContent(ib+1) );
	        res_RMS   = sqrt( (fabs( res2_mean-res_mean*res_mean ))/( h_Res_CS1_entries->GetBinContent(ib+1) ) ) ;  
	      
	        h_Res_CS1->SetBinContent(ib+1, res_mean);
	        h_Res_CS1->SetBinError(  ib+1, res_RMS );
		if ( h_Res_CS1SecDist     ) h_Res_CS1SecDist    ->Fill( res_mean );
		if ( h_Res_CS1_rmsSecDist ) h_Res_CS1_rmsSecDist->Fill( res_RMS  );
		if ( ib>( h_Res_CS1->GetNbinsX()/2 ) ) {
		  if ( h_AverageRes_CS1_A    ) h_AverageRes_CS1_A    ->Fill( res_mean );
		  if ( h_AverageRes_CS1rms_A ) h_AverageRes_CS1rms_A ->Fill( res_RMS  );
		}
		else {
		  if ( h_AverageRes_CS1_C     ) h_AverageRes_CS1_C   ->Fill( res_mean );
		  if ( h_AverageRes_CS1rms_C  ) h_AverageRes_CS1rms_C->Fill( res_RMS  );
		}
	      }
	    } // end for bins
	    TDirectory* dirRes1 = f->GetDirectory( dir_sum_track.c_str() ) ;
	    if ( dirRes1 != 0 ) {
	      dirRes1->cd() ;
	      h_Res_CS1->Write("",TObject::kOverwrite);
              if ( h_Res_CS1SecDist     ) h_Res_CS1SecDist    ->Write("",TObject::kOverwrite);
	      if ( h_Res_CS1_rmsSecDist ) h_Res_CS1_rmsSecDist->Write("",TObject::kOverwrite);
	    }
	  }
	  
	  //residuals CS = 2
	  res_mean=0; res2_mean=0; res_RMS=0; 
	  if ( h_Res_CS2 && h_Res_CS2_s && h_Res_CS2_square && h_Res_CS2_entries ) {
	  
	    for ( int ib = 0; ib != h_Res_CS2->GetNbinsX(); ib++ ) {
	      if ( h_PanelId ) { if ( (h_PanelId->GetBinContent(ib+1) )==0 ) { continue; }  }
	      if ( (h_Res_CS2_entries->GetBinContent(ib+1) )!=0 ) {
	        res_mean  = (h_Res_CS2_s->GetBinContent(ib+1) )/ (h_Res_CS2_entries->GetBinContent(ib+1) )       ;
	        res2_mean = (h_Res_CS2_square->GetBinContent(ib+1) )/ (h_Res_CS2_entries->GetBinContent(ib+1) );
	        res_RMS   = sqrt( fabs ( ( res2_mean-res_mean*res_mean )/( h_Res_CS2_entries->GetBinContent(ib+1) ) ) ) ;  
	      
	        h_Res_CS2->SetBinContent(ib+1, res_mean);
	        h_Res_CS2->SetBinError(  ib+1, res_RMS );
		if ( h_Res_CS2SecDist     ) h_Res_CS2SecDist    ->Fill( res_mean );
		if ( h_Res_CS2_rmsSecDist ) h_Res_CS2_rmsSecDist->Fill( res_RMS  );
		if ( ib >( h_Res_CS2->GetNbinsX()/2 ) ) {
		  if ( h_AverageRes_CS2_A     ) h_AverageRes_CS2_A    ->Fill( res_mean );
		  if ( h_AverageRes_CS2rms_A  ) h_AverageRes_CS2rms_A ->Fill( res_RMS  );
		}
		else {
		  if ( h_AverageRes_CS2_C     ) h_AverageRes_CS2_C    ->Fill( res_mean );
		  if ( h_AverageRes_CS2rms_C  ) h_AverageRes_CS2rms_C ->Fill( res_RMS  );
		}
	      }
	    }
	    TDirectory* dirRes2 = f->GetDirectory( dir_sum_track.c_str() ) ;
	    if ( dirRes2 != 0 ) {
	      dirRes2->cd() ;
	      h_Res_CS2->Write("",TObject::kOverwrite);
	      if ( h_Res_CS2SecDist     ) h_Res_CS2SecDist    ->Write("",TObject::kOverwrite);
	      if ( h_Res_CS2_rmsSecDist ) h_Res_CS2_rmsSecDist->Write("",TObject::kOverwrite);
	    }
	  }
	  
	  //residuals CS > 2
	  res_mean=0; res2_mean=0; res_RMS=0; 
	  if ( h_Res_CSmore2 && h_Res_CSmore2_s && h_Res_CSmore2_square && h_Res_CSmore2_entries ) {
	  
	    for ( int ib = 0; ib != h_Res_CSmore2->GetNbinsX(); ib++ ) {
	      if ( h_PanelId ) { if ( (h_PanelId->GetBinContent(ib+1) )==0 ) { continue; }  }
	      if ( (h_Res_CSmore2_entries->GetBinContent(ib+1) )!=0 ) {
	        res_mean  = (h_Res_CSmore2_s->GetBinContent(ib+1) )/ (h_Res_CSmore2_entries->GetBinContent(ib+1) )       ;
	        res2_mean = (h_Res_CSmore2_square->GetBinContent(ib+1) )/ (h_Res_CSmore2_entries->GetBinContent(ib+1) );
	        res_RMS   = sqrt( fabs( ( res2_mean-res_mean*res_mean )/( h_Res_CSmore2_entries->GetBinContent(ib+1) ) ) ) ;  
	      
	        h_Res_CSmore2->SetBinContent(ib+1, res_mean);
	        h_Res_CSmore2->SetBinError(  ib+1, res_RMS );
		if ( h_Res_CSmore2SecDist     ) h_Res_CSmore2SecDist    ->Fill( res_mean );
		if ( h_Res_CSmore2_rmsSecDist ) h_Res_CSmore2_rmsSecDist->Fill( res_RMS  );
		if ( ib > (h_Res_CSmore2->GetNbinsX()/2 ) ) {
          	  if ( h_AverageRes_CSmore2_A     )  h_AverageRes_CSmore2_A    ->Fill( res_mean );
		  if ( h_AverageRes_CSmore2rms_A  )  h_AverageRes_CSmore2rms_A ->Fill( res_RMS  );
		}
		else {
		  if ( h_AverageRes_CSmore2_C     )  h_AverageRes_CSmore2_C    ->Fill( res_mean );
		  if ( h_AverageRes_CSmore2rms_C  )  h_AverageRes_CSmore2rms_C ->Fill( res_RMS  );
		}
	      }
	    }
	    TDirectory* dirResp2 = f->GetDirectory( dir_sum_track.c_str() ) ;
	    if ( dirResp2 != 0 ) {
	      dirResp2->cd() ;
	      h_Res_CSmore2->Write("",TObject::kOverwrite);
	      if ( h_Res_CSmore2SecDist     ) h_Res_CSmore2SecDist    ->Write("",TObject::kOverwrite);
	      if ( h_Res_CSmore2_rmsSecDist ) h_Res_CSmore2_rmsSecDist->Write("",TObject::kOverwrite);
	    }
	  }
	  
	  // occupancy
	  if ( (rpc_eventstotal>0) && h_Occupancy && h_Occupancy_s ) {
	    for ( int ib = 0; ib != h_Occupancy->GetNbinsX(); ib++ ) {
	      if ( h_PanelId ) { if ( (h_PanelId->GetBinContent(ib+1) )==0 ) { continue; }  }
	      panel_occ = h_Occupancy_s->GetBinContent(ib+1) ;
	      panel_occ = panel_occ/float(rpc_eventstotal) ;
	      
	      h_Occupancy->SetBinContent(ib+1, panel_occ       ) ;
	      h_Occupancy->SetBinError(  ib+1, sqrt(panel_occ) ) ;
	      
	      if (panel_occ>0 ) {
	        panel_occ = log10( panel_occ ) ;
              }
	      else {
	        panel_occ = -10 ;
	      }
	      if ( h_OccupancySecDist ) h_OccupancySecDist->Fill( panel_occ );
	      if ( ib>( h_PanelId->GetNbinsX()/2) ) { if ( h_AverageOccupancy_A ) h_AverageOccupancy_A->Fill( panel_occ ); }
	      else 				    { if ( h_AverageOccupancy_C ) h_AverageOccupancy_C->Fill( panel_occ ); }
	    }
	    // write occupancy histograms
	    TDirectory* dirOcc = f->GetDirectory( dir_sum_track.c_str() ) ;
	    if ( dirOcc !=0 ) {
	      dirOcc->cd();
	      h_Occupancy       ->Write("",TObject::kOverwrite);
	      h_OccupancySecDist->Write("",TObject::kOverwrite);
	    }
	  }
	  
	  
	  // CS
	  if ( h_CS && h_CS_s && h_CS_square && h_CS_entries ){
	    for ( int ib = 0; ib != h_CS->GetNbinsX(); ib++ ) {
	      if ( h_PanelId ) { if ( (h_PanelId->GetBinContent(ib+1) )==0 ) { continue; }  }
	      panelCS_entries = h_CS_entries->GetBinContent(ib+1);
	      panelCS	      = h_CS_s->GetBinContent(ib+1)      ;
	      panelCS2	      = h_CS_square->GetBinContent(ib+1) ;
	      if ( panelCS_entries !=0 ) {
	        panelCS_mean  = panelCS/panelCS_entries  ;
		panelCS2_mean = panelCS2/panelCS_entries ;
		panelCS_RMS   = sqrt( fabs( (panelCS2_mean - panelCS_mean*panelCS_mean ) / panelCS_entries ) ) ;
		
		h_CS->SetBinContent( ib+1, panelCS_mean ) ;
		h_CS->SetBinError  ( ib+1, panelCS_RMS  ) ;
		
		if ( h_CSSecDist ) h_CSSecDist->Fill( panelCS_mean );
		if ( ib > ( h_CS->GetNbinsX()/2 ) ) { if ( h_AverageCS_A ) h_AverageCS_A->Fill( panelCS_mean ); }
		else                                { if ( h_AverageCS_C ) h_AverageCS_C->Fill( panelCS_mean ); }
	      }
	    }
	    // write CS histograms
	    TDirectory* dirCS = f->GetDirectory( dir_sum_track.c_str() ) ;
	    if ( dirCS!=0 ) {
	      dirCS->cd();
	      h_CS->Write("",TObject::kOverwrite) ;
	      if ( h_CSSecDist ) h_CSSecDist->Write("",TObject::kOverwrite) ;
	    }
	     
	  }
	  
	  // time
	  if ( h_Time && h_Time_s && h_Time_square && h_CS_entries ) {
	    for ( int ib = 0; ib != h_Time->GetNbinsX(); ib++ ) {
	      if ( h_PanelId ) { if ( (h_PanelId->GetBinContent(ib+1) )==0 ) { continue; }  }
	      if ( h_CS_entries->GetBinContent(ib+1) !=0 ) {
	        Time_mean  = (h_Time_s->GetBinContent(ib+1) )/ (h_CS_entries->GetBinContent(ib+1) )	           ;
	        Time2_mean = (h_Time_square->GetBinContent(ib+1) )/ (h_CS_entries->GetBinContent(ib+1) )	   ;
	        Time_RMS   = sqrt(fabs( ( Time2_mean-Time_mean*Time_mean )/( h_CS_entries->GetBinContent(ib+1) )) );
	      
	        h_Time->SetBinContent(ib+1, Time_mean);
	        h_Time->SetBinError(  ib+1, Time_RMS );
	      
	        if ( h_TimeSecDist ) h_TimeSecDist->Fill( Time_mean );
		if ( ib>( h_Time->GetNbinsX()/2 )  ) { if ( h_AverageTime_A ) h_AverageTime_A->Fill( Time_mean ); }
		else                                 { if ( h_AverageTime_C ) h_AverageTime_C->Fill( Time_mean ); }
	      }
	    }
	    // write time histograms
	    TDirectory* dirTime = f->GetDirectory( dir_sum_track.c_str() ) ;
	    if ( dirTime != 0 ) {
	      dirTime->cd();
	      h_Time->Write("",TObject::kOverwrite) ;
	      if ( h_TimeSecDist ) h_TimeSecDist->Write("",TObject::kOverwrite) ;
	    }
	  }
	  // noise	      
          noiseErrNorm = 18257.42 ;
	  // noiseErr = sqrt( counts ) / ( area * T * events )
	  // noiseErr = sqrt(noise_bin * area* 0.2 ) / ( area * time * events )
	  // noiseErr = sqrt( noise_bin ) * noiseErrNorm / events
	  // noiseErrNorm = sqrt(0.2) / sqrt( time * area )
	  // assuming average area of RPC panels = 15000 cm2
	  // noiseErrNorm = 18257.42 
	  if ( (rpc_eventstotal>0) && h_NoiseCorr && h_NoiseCorr_s && h_NoiseTot && h_NoiseTot_s && h_CS_entries) {
	    //std::cout << " Taglio Eventi " << std::endl;
	    for ( int ib = 0; ib != h_NoiseCorr->GetNbinsX(); ib++ ) {
	      if ( h_PanelId ) { if ( (h_PanelId->GetBinContent(ib+1) )==0 ) { continue; }  }
	      if ( h_CS_entries->GetBinContent(ib+1) ==0)continue;
	      noiseCorr    = h_NoiseCorr_s->GetBinContent(ib+1) ;
	      noiseCorrErr = sqrt( noiseCorr ) * noiseErrNorm / float(rpc_eventstotal) ;
	      noiseCorr = (noiseCorr*1000000)/float(rpc_eventstotal) ;
	      h_NoiseCorr->SetBinContent(ib+1, noiseCorr);
	      h_NoiseCorr->SetBinError(ib+1, noiseCorrErr);
	      if ( h_NoiseCorrSecDist ) h_NoiseCorrSecDist->Fill( noiseCorr );
	      
	      noiseTot    = h_NoiseTot_s->GetBinContent(ib+1)   ;
	      noiseTotErr = sqrt( noiseTot ) * noiseErrNorm / float(rpc_eventstotal) ;
	      noiseTot  = (noiseTot*1000000)/float(rpc_eventstotal) ;
	      h_NoiseTot->SetBinContent(ib+1, noiseTot    ) ;
	      h_NoiseTot->SetBinError(ib+1, noiseTotErr )   ;
	      
	      if ( h_NoiseTotSecDist ) h_NoiseTotSecDist->Fill( noiseTot );	      
	      
	      if ( ib > ( h_NoiseCorr->GetNbinsX()/2 ) ) { 
	        if ( h_AverageNoiseTot_A  ) h_AverageNoiseTot_A->Fill ( noiseTot  ); 
		if ( h_AverageNoiseCorr_A ) h_AverageNoiseCorr_A->Fill( noiseCorr );
              }
	      else { 
	        if ( h_AverageNoiseTot_C  ) h_AverageNoiseTot_C ->Fill( noiseTot  ); 
		if ( h_AverageNoiseCorr_C ) h_AverageNoiseCorr_C->Fill( noiseCorr );
	      }
	    }
	    TDirectory* dirNoise = f->GetDirectory( dir_sum_track.c_str() ) ;
	    if ( dirNoise!=0 ) {
	      dirNoise->cd();
	      h_NoiseCorr->Write("",TObject::kOverwrite) ;
	      h_NoiseTot ->Write("",TObject::kOverwrite) ;
	      
	      if ( h_NoiseCorrSecDist ) h_NoiseCorrSecDist->Write("",TObject::kOverwrite) ;
	      if ( h_NoiseTotSecDist  ) h_NoiseTotSecDist ->Write("",TObject::kOverwrite) ;
	    }
	  }
	  
	  
	 ///create and insert entries in SQLite DB 
         //  bool do_asciiCoolFile1  = true;
	   float effeta            = -9999;	  float effphi  	  = -9999;  	 char arr_effeta  	  [10];      char arr_effphi	      [10];	    
           float erreffeta         = -1; 	  float erreffphi	  =-1;  	 char arr_erreffeta	  [10];      char arr_erreffphi	      [10];	    
	   float reseta_cs1        = -9999;	  float resphi_cs1	  = -9999;  	 char arr_reseta_cs1	  [10];      char arr_resphi_cs1        [10];	    
           float errreseta_cs1     = -1; 	  float errresphi_cs1	  =-1;  	 char arr_errreseta_cs1	  [10];      char arr_errresphi_cs1     [10];	    
	   float reseta_cs2        = -9999;	  float resphi_cs2	  = -9999;  	 char arr_reseta_cs2	  [10];      char arr_resphi_cs2        [10];	    
           float errreseta_cs2     = -1; 	  float errresphi_cs2	  =-1;  	 char arr_errreseta_cs2	  [10];      char arr_errresphi_cs2     [10];	    
	   float reseta_csother    = -9999;	  float resphi_csother	  = -9999;  	 char arr_reseta_csother    [10];      char arr_resphi_csother    [10];	    
           float errreseta_csother = -1; 	  float errresphi_csother =-1;  	 char arr_errreseta_csother [10];      char arr_errresphi_csother [10];	    
	   float timeeta           = -9999;	  float timephi 	  = -9999;  	 char arr_timeeta 	  [10];      char arr_timephi	      [10];	    
           float errtimeeta        = -1; 	  float errtimephi	  =-1;  	 char arr_errtimeeta	  [10];      char arr_errtimephi        [10];	    
	   float noiseeta          = -9999;	  float noisephi	  = -9999;  	 char arr_noiseeta	  [10];      char arr_noisephi	      [10];	    
           float errnoiseeta       = -1; 	  float errnoisephi	  =-1;  	 char arr_errnoiseeta	  [10];      char arr_errnoisephi       [10];	    
	   float noiseeta_cor      = -9999;	  float noisephi_cor	  = -9999;  	 char arr_noiseeta_cor	  [10];      char arr_noisephi_cor      [10];	    
           float errnoiseeta_cor   = -1; 	  float errnoisephi_cor   =-1;  	 char arr_errnoiseeta_cor   [10];      char arr_errnoisephi_cor   [10];	    
	   float cl_sizeeta        = -9999;	  float cl_sizephi	  = -9999;  	 char arr_cl_sizeeta	  [10];      char arr_cl_sizephi        [10];	    
           float errcl_sizeeta     = -1; 	  float errcl_sizephi	  =-1;  	 char arr_errcl_sizeeta	  [10];      char arr_errcl_sizephi     [10];	    
  		
           float eta_effphi        =  0;          float phi_effeta        = 0;      		
	 
	   int   PanelCode   = 0;
	   int   Binposition   = 0;
	   int   TableVersion = 1;     //Version of Cool Table Formatting according with Andrea Di Simone Table
	   int   n_tr_peta    = 0;     //Number of eta tracks reconstructed on a gas volume
	   int   n_tr_pphi    = 0;     //Number of phi tracks reconstructed on a gas volume
	   int   NumberOfInfo = 1;     //Number of information for each variable 1= only pannel, >1 depending on tipy of monitoring setting and strip pannel geometry
	   int   StripCluster = 0;     //Number of strip that realize de reduction bin and the strip pannel geometry 0=no strip info,if panel has 24 strip: 24=strip info, 3   = cluster of 8 strip, 6 = cluster of 4 strip and so on
	   

 
           coolrpc.coolDbFolder("sqlite://;schema=RPCDQMFOFFLINE.db;dbname=RPC_DQA","/OFFLINE/OFFLINE_DQMF");
	   int nbin = 0;
	   if ( h_Eff ) nbin = h_Eff ->GetNbinsX() ;
	   for(int ibin=1 ; ibin!=nbin+1 ; ibin++){
	    if ( h_PanelId )PanelCode = (int)h_PanelId-> GetBinContent(ibin) ;
	    if(PanelCode ==0)continue;
  	     if(ibin%2==0){ 
	     
	      if (h_TrackProj) n_tr_pphi  =(int)h_TrackProj   -> GetBinContent(ibin) ;
	      if(n_tr_pphi <1000 ) continue;

	      if ( h_Eff	 )effphi	      = h_Eff		  ->GetBinContent(ibin) ;    
	      sprintf(arr_effphi		,    "%f ", effphi	      ) ;   arr_effphi	       [5]   =0;
	      if ( h_Eff	 )erreffphi	      = h_Eff		  ->GetBinError  (ibin) ;    
	      sprintf(arr_erreffphi	,    "%f ", erreffphi	      ) ;   arr_erreffphi        [5]   =0;
	      if ( h_Res_CS1	 )resphi_cs1	      = h_Res_CS1	  ->GetBinContent(ibin) ;    
	      sprintf(arr_resphi_cs1	,    "%f ", resphi_cs1        ) ;   arr_resphi_cs1       [5]   =0;
	      if ( h_Res_CS1	 )errresphi_cs1       = h_Res_CS1	  ->GetBinError  (ibin) ;    
	      sprintf(arr_errresphi_cs1	,    "%f ", errresphi_cs1     ) ;   arr_errresphi_cs1    [5]   =0;
	      if ( h_Res_CS2	 )resphi_cs2	      = h_Res_CS2	  ->GetBinContent(ibin) ;    
	      sprintf(arr_resphi_cs2	,    "%f ", resphi_cs2        ) ;   arr_resphi_cs2       [5]   =0;
	      if ( h_Res_CS2	 )errresphi_cs2       = h_Res_CS2	  ->GetBinError  (ibin) ;    
	      sprintf(arr_errresphi_cs2	,    "%f ", errresphi_cs2     ) ;   arr_errresphi_cs2    [5]   =0;
	      if ( h_Res_CSmore2 )resphi_csother      = h_Res_CSmore2	  ->GetBinContent(ibin) ;    
	      sprintf(arr_resphi_csother	,    "%f ", resphi_csother    ) ;   arr_resphi_csother   [5]   =0;
	      if ( h_Res_CSmore2 )errresphi_csother   = h_Res_CSmore2	  ->GetBinError  (ibin) ;    
	      sprintf(arr_errresphi_csother,    "%f ", errresphi_csother ) ;   arr_errresphi_csother[5]   =0;
	      if ( h_Time	 )timephi	      = h_Time  	  ->GetBinContent(ibin) ;    
	      sprintf(arr_timephi  	,    "%f ", timephi	      ) ;   arr_timephi	       [5]   =0;
              if ( h_Time	 )errtimephi	      = h_Time  	  ->GetBinError  (ibin) ;    
	      sprintf(arr_errtimephi	,    "%f ", errtimephi        ) ;   arr_errtimephi       [5]   =0;
	      if ( h_NoiseTot	 )noisephi	      = h_NoiseTot	  ->GetBinContent(ibin) ;    
	      sprintf(arr_noisephi 	,    "%f ", noisephi	      ) ;   arr_noisephi         [5]   =0;
	      if ( h_NoiseTot	 )errnoisephi	      = h_NoiseTot	  ->GetBinError  (ibin) ;    
	      sprintf(arr_errnoisephi	,    "%f ", errnoisephi       ) ;   arr_errnoisephi      [5]   =0;
	      if ( h_NoiseCorr   )noisephi_cor        = h_NoiseCorr	  ->GetBinContent(ibin) ;    
	      sprintf(arr_noisephi_cor	,    "%f ", noisephi_cor      ) ;   arr_noisephi_cor     [5]   =0;
	      if ( h_NoiseCorr   )errnoisephi_cor     = h_NoiseCorr	  ->GetBinError  (ibin) ;    
	      sprintf(arr_errnoisephi_cor  ,    "%f ", errnoisephi_cor   ) ;   arr_errnoisephi_cor  [5]   =0;
	      if ( h_CS 	 )cl_sizephi	      = h_CS		  ->GetBinContent(ibin) ;    
	      sprintf(arr_cl_sizephi	,    "%f ", cl_sizephi        ) ;   arr_cl_sizephi       [5]   =0;
	      if ( h_CS 	 )errcl_sizephi       = h_CS		  ->GetBinError  (ibin) ;    
	      sprintf(arr_errcl_sizephi	,    "%f ", errcl_sizephi     ) ;   arr_errcl_sizephi    [5]   =0;
	     
	     }else{
	      if (h_TrackProj) {n_tr_peta  =(int)h_TrackProj   -> GetBinContent(ibin) ;}
              if(n_tr_peta <1000) continue;

 	      if ( h_Eff	)effeta 	     = h_Eff		 ->GetBinContent(ibin) ;      
	      sprintf(arr_effeta  	 ,    "%f ", effeta	       ) ;   arr_effeta		[5]   =0;
	      if ( h_Eff	)erreffeta	     = h_Eff		 ->GetBinError  (ibin) ;      
	      sprintf(arr_erreffeta	 ,    "%f ", erreffeta         ) ;   arr_erreffeta	[5]   =0;
	      if ( h_Res_CS1	)reseta_cs1	     = h_Res_CS1	 ->GetBinContent(ibin) ;      
	      sprintf(arr_reseta_cs1	 ,    "%f ", reseta_cs1        ) ;   arr_reseta_cs1	[5]   =0;
	      if ( h_Res_CS1	)errreseta_cs1       = h_Res_CS1	 ->GetBinError  (ibin) ;      
	      sprintf(arr_errreseta_cs1	 ,    "%f ", errreseta_cs1     ) ;   arr_errreseta_cs1	[5]   =0;
	      if ( h_Res_CS2	)reseta_cs2	     = h_Res_CS2	 ->GetBinContent(ibin) ;      
	      sprintf(arr_reseta_cs2	 ,    "%f ", reseta_cs2        ) ;   arr_reseta_cs2	[5]   =0;
	      if ( h_Res_CS2	)errreseta_cs2       = h_Res_CS2	 ->GetBinError  (ibin) ;      
	      sprintf(arr_errreseta_cs2	 ,    "%f ", errreseta_cs2     ) ;   arr_errreseta_cs2	[5]   =0;
              if ( h_Res_CSmore2)reseta_csother      = h_Res_CSmore2	 ->GetBinContent(ibin) ;      
	      sprintf(arr_reseta_csother   ,    "%f ", reseta_csother    ) ;   arr_reseta_csother	[5]   =0;
	      if ( h_Res_CSmore2)errreseta_csother   = h_Res_CSmore2	 ->GetBinError  (ibin) ;      
	      sprintf(arr_errreseta_csother,    "%f ", errreseta_csother ) ;   arr_errreseta_csother[5]   =0;
	      if ( h_Time	)timeeta	     = h_Time		 ->GetBinContent(ibin) ;      
	      sprintf(arr_timeeta 	 ,    "%f ", timeeta	       ) ;   arr_timeeta  	[5]   =0;
	      if ( h_Time	)errtimeeta	     = h_Time		 ->GetBinError  (ibin) ;      
	      sprintf(arr_errtimeeta	 ,    "%f ", errtimeeta        ) ;   arr_errtimeeta	[5]   =0;
	      if ( h_NoiseTot	)noiseeta	     = h_NoiseTot	 ->GetBinContent(ibin) ;      
	      sprintf(arr_noiseeta	 ,    "%f ", noiseeta	       ) ;   arr_noiseeta 	[5]   =0;
	      if ( h_NoiseTot	)errnoiseeta	     = h_NoiseTot	 ->GetBinError  (ibin) ;      
	      sprintf(arr_errnoiseeta	 ,    "%f ", errnoiseeta       ) ;   arr_errnoiseeta	[5]   =0;
	      if ( h_NoiseCorr  )noiseeta_cor	     = h_NoiseCorr	 ->GetBinContent(ibin) ;      
	      sprintf(arr_noiseeta_cor	 ,    "%f ", noiseeta_cor      ) ;   arr_noiseeta_cor	[5]   =0;
	      if ( h_NoiseCorr  )errnoiseeta_cor     = h_NoiseCorr	 ->GetBinError  (ibin) ;      
	      sprintf(arr_errnoiseeta_cor  ,    "%f ", errnoiseeta_cor   ) ;   arr_errnoiseeta_cor  [5]   =0;
	      if ( h_CS 	)cl_sizeeta	     = h_CS		 ->GetBinContent(ibin) ;      
	      sprintf(arr_cl_sizeeta	 ,    "%f ", cl_sizeeta        ) ;   arr_cl_sizeeta	[5]   =0;
	      if ( h_CS 	)errcl_sizeeta       = h_CS		 ->GetBinError  (ibin) ;      
	      sprintf(arr_errcl_sizeeta	 ,    "%f ", errcl_sizeeta     ) ;   arr_errcl_sizeeta	[5]   =0;	     
	     
	    
              //std::cout<<"PanelCode  "<<PanelCode<<" etaprimo "<<"\n";
 
	      char recEta   [4000]; //eff_eta, res_cs1, res_cs2, res_csother, time, mean and rms
	      char detEta   [4000]; //noise, noise_corr, cs, mean and rms
	      char recPhi1  [4000]; //eff_phi, res_cs1, res_cs2, res_csother, time, only mean
 	      char recPhi2  [4000]; //eff_phi, res_cs1, res_cs2, res_csother, time, only rms
	      char detPhi1  [4000]; //noise, noise_corr, cs, mean and rms
	      char detPhi2  [4000];
	      sprintf(recEta,  "%5d %5d %5d %5d %s %s %s %s %s %s %s %s %s %s ", TableVersion,  n_tr_peta, NumberOfInfo, StripCluster, arr_effeta, arr_erreffeta, arr_reseta_cs1, arr_errreseta_cs1, arr_reseta_cs2, arr_errreseta_cs2, arr_reseta_csother, arr_errreseta_csother, arr_timeeta, arr_errtimeeta) ;  
	      sprintf(detEta,  "%s %s %s %s %s %s ", arr_noiseeta, arr_errnoiseeta, arr_noiseeta_cor, arr_errnoiseeta_cor, arr_cl_sizeeta, arr_errcl_sizeeta) ;  
	      sprintf(recPhi1, "%5d %5d %5d %s %s %s %s %s ", n_tr_pphi, NumberOfInfo, StripCluster, arr_effphi, arr_resphi_cs1, arr_resphi_cs2, arr_resphi_csother, arr_timephi) ;  
	      sprintf(recPhi2, "%s %s %s %s %s ", arr_erreffphi, arr_errresphi_cs1, arr_errresphi_cs2, arr_errresphi_csother, arr_errtimephi) ;  
	      sprintf(detPhi1, "%s %s %s %s %s %s ", arr_noisephi, arr_errnoisephi, arr_noisephi_cor, arr_errnoisephi_cor, arr_cl_sizephi, arr_errcl_sizephi) ;  
	      sprintf(detPhi2, "0 ") ;  
 	      std::string cool_tag="Reco";		        
              coolrpc.setSince(0U,0U);		
              coolrpc.setUntil(4294967295U,0U);	
 	      coolrpc.insert_withTag(run_number*0+429496729U,PanelCode,recEta,detEta,recPhi1,recPhi2,detPhi1,detPhi2, cool_tag);			    
	     }
	    }
	   
////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
	   
            int   TableVersionCondDB  = 2 ;         //RPC conditionDB table versioning
	    float gapeffeta           = -9999;	       char arr_gapeffeta 	 [10];  	   
            float errgapeffeta        = -1;	       char arr_errgapeffeta	 [10];  	   
            float gapeffphi	      = -9999;	       char arr_gapeffphi 	 [10];
	    float errgapeffphi        = -1;            char arr_errgapeffphi	 [10];
	    float gapeff              = -9999;			    
            float errgapeff           = -1;
	    float entriesCSeta        = -1;
	    int   entriesCS1eta	      = -1;
	    int   entriesCS2eta	      = -1;
	    float entriesCSphi        = -1;
	    int   entriesCS1phi	      = -1;
	    int   entriesCS2phi	      = -1;
            float rateCS1eta	      = -1;	       char arr_rateCS1eta	 [10]; 
            float rateCS2eta	      = -1;	       char arr_rateCS2eta	 [10]; 
            float rateCSmore2eta      = -1;	       char arr_rateCSmore2eta	 [10]; 
            float rateCS1phi	      = -1;	       char arr_rateCS1phi	 [10]; 
            float rateCS2phi	      = -1;	       char arr_rateCS2phi	 [10]; 
            float rateCSmore2phi      = -1;	       char arr_rateCSmore2phi	 [10]; 
	    
	    

 	    coolrpc.coolDbFolder("sqlite://;schema=RPCConditionDB.db;dbname=RPC_DQA","/OFFLINE/FINAL");
  	    std::string dir_cool_raw = run_dir + "/Muon/MuonRawDataMonitoring/RPC/CoolDB/";
	    TDirectory* coolFolder = f->GetDirectory( dir_cool_raw.c_str() ) ;
            
	    if ( coolFolder != 0 ) {
	  	  	  
	    std::vector<std::string> layerList ;
	    layerList.push_back( "LowPt0"   );
	    layerList.push_back( "LowPt1"   );
	    layerList.push_back( "Pivot0"   );
	    layerList.push_back( "Pivot1"   );
	    layerList.push_back( "HighPt0"  );
	    layerList.push_back( "HighPt1"  );
	    int NumberLayerStrip      = 0;
	    int PanelStripId          = 0;
	    int StripProfileContenent = 0;
	  
	    for ( std::vector<std::string>::const_iterator iter=layerList.begin(); iter!=layerList.end(); iter++ ) {
	      for ( int i_dblPhi=0; i_dblPhi!=2*1+1; i_dblPhi++ ) {
	        char coolName[40];
		sprintf(coolName, "Sector%.2d_%s_dblPhi%d", i_sec+1, (*iter).c_str(), i_dblPhi+1 );
		std::string stripId_name       = dir_cool_raw + coolName + "_PanelId" ;
	        std::string stripProfile_name  = dir_cool_raw + coolName + "_Profile" ;
		//std::cout <<stripProfile_name << std::endl;
		TH1F* h_stripId      = NULL;
	        TH1F* h_stripProfile = NULL;
	        
		if ( RPCCheckHistogram(f, stripId_name.c_str()      ) ) { h_stripId      = (TH1F*) ( f->Get(stripId_name.c_str())      );} 
		if ( RPCCheckHistogram(f, stripProfile_name.c_str() ) ) { h_stripProfile = (TH1F*) ( f->Get(stripProfile_name.c_str()) );} 
		
		if ( h_stripId && h_stripProfile ) {
		 
		  int  SingleStripsValue    = 0;
		  int  StripsOnPanel        = 1; //number of strip on panel
		  char SingleStripsStatus  [80]  ;
		  char SingleStripsStatusOK[80]  ;
		  std::string PanelStripsStatus  ;
		  std::string PanelStripsStatusOK;
		  
		  NumberLayerStrip = h_stripProfile->GetNbinsX() ;
		  for(int Nstrips=1 ; Nstrips!=NumberLayerStrip+1 ; Nstrips++){
		    PanelStripId          = (int)h_stripId      -> GetBinContent(Nstrips) ;
		    StripProfileContenent = (int)h_stripProfile -> GetBinContent(Nstrips) ;
		    float StripOccupancy = (float)(StripProfileContenent)/rpc_eventstotal;
		    if (StripOccupancy==0) SingleStripsValue=0;	   
		    if (StripOccupancy>0   && StripOccupancy<0.9) SingleStripsValue=5;
		    if (StripOccupancy>0.9) SingleStripsValue=9;
		    		    
		    if(h_stripId-> GetBinCenter(Nstrips) > 0){ 
		     sprintf(SingleStripsStatus  , "%d 000.0 0.000|", SingleStripsValue );
		     sprintf(SingleStripsStatusOK,  "5 000.0 0.000|"                    );
		    }else{ 
		     sprintf(SingleStripsStatus, "|000.0 0.000 %d", SingleStripsValue );
		     sprintf(SingleStripsStatusOK, "|000.0 0.000 5"                   );
		    }
		    PanelStripsStatus   = PanelStripsStatus   +   SingleStripsStatus;
		    PanelStripsStatusOK = PanelStripsStatusOK + SingleStripsStatusOK;
		    
		    

		    if((int)h_stripId -> GetBinContent(Nstrips)==(int)h_stripId  -> GetBinContent(Nstrips+1))StripsOnPanel++;                      
		    //std::cout <<Nstrips<<" "<< h_stripId-> GetBinCenter(Nstrips)<< " "<< SingleStripsStatus <<" PanelStripsStatus " << PanelStripsStatus <<" PanelStripsId " << PanelStripId <<std::endl;

		    if((int)h_stripId -> GetBinContent(Nstrips)!=(int)h_stripId  -> GetBinContent(Nstrips+1)){
		    //std::cout <<StripsOnPanel<<" StripsOnPanel "<< std::endl;
  
		    if(h_stripId-> GetBinCenter(Nstrips) < 0){		    		    
		     //std::cout << " PanelStripsStatus " << PanelStripsStatus <<std::endl;		    
		     std::reverse(PanelStripsStatus  .begin(), PanelStripsStatus  .end());				    
		     std::reverse(PanelStripsStatusOK.begin(), PanelStripsStatusOK.end());		
		    }
                       
		      for(int ibin=1 ; ibin!=nbin+1 ; ibin++){
                        if ( h_PanelId )PanelCode = (int)h_PanelId-> GetBinContent(ibin) ;
	                if(PanelCode !=PanelStripId)continue;
			  if(ibin%2!=0){
			    if (h_TrackProj) {n_tr_peta  =(int)h_TrackProj   -> GetBinContent(ibin) ;}
			      //if(n_tr_peta >0){				
				if ( h_PanelId )Binposition = (int)h_PanelId-> GetBinCenter(ibin) ;
				int ibin_perp=0;
	                        if(Binposition>0){ 
				 ibin_perp=ibin+1;
			         if ( h_Eff 	)eta_effphi 	     = h_Eff		 ->GetBinContent(ibin+1) ;
			        }else{
				 ibin_perp=ibin-1;
			         if ( h_Eff 	)eta_effphi 	     = h_Eff		 ->GetBinContent(ibin-1) ;
 			        }
			    	if ( h_GapEff	  )gapeff	       = h_GapEff	   ->GetBinContent(ibin) ;
			    	if ( h_GapEff	  )errgapeff	       = h_GapEff	   ->GetBinError  (ibin) ;				
 			    	if ( h_Eff	  )effeta	       = h_Eff  	   ->GetBinContent(ibin) ;
 			    	if ( h_Eff	  )erreffeta	       = h_Eff  	   ->GetBinError  (ibin) ;
				
				gapeffeta   =               gapeff;
				errgapeffeta=            errgapeff;
				EffThreshold = (effeta<Minimum_efficiency)|| (eta_effphi<Minimum_efficiency);
				
				
				if ( h_Res_CS1    )reseta_cs1	       = h_Res_CS1	   ->GetBinContent(ibin) ;
 			    	if ( h_Res_CS1    )errreseta_cs1       = h_Res_CS1	   ->GetBinError  (ibin) ;
 			    	if ( h_Res_CS2    )reseta_cs2	       = h_Res_CS2	   ->GetBinContent(ibin) ;
 			    	if ( h_Res_CS2    )errreseta_cs2       = h_Res_CS2	   ->GetBinError  (ibin) ;
 			    	if ( h_Res_CSmore2)reseta_csother      = h_Res_CSmore2     ->GetBinContent(ibin) ;
 			    	if ( h_Res_CSmore2)errreseta_csother   = h_Res_CSmore2     ->GetBinError  (ibin) ;
			    	if ( h_NoiseTot   )noiseeta	       = h_NoiseTot	   ->GetBinContent(ibin) ;
 			    	if ( h_NoiseTot   )errnoiseeta         = h_NoiseTot	   ->GetBinError  (ibin) ;	     
				if ( h_NoiseCorr  )noiseeta_cor	       = h_NoiseCorr	   ->GetBinContent(ibin) ;      
	                        if ( h_NoiseCorr  )errnoiseeta_cor     = h_NoiseCorr	   ->GetBinError  (ibin) ;      
 			    	if ( h_CS	  )cl_sizeeta	       = h_CS		   ->GetBinContent(ibin) ;
 			    	if ( h_CS	  )errcl_sizeeta       = h_CS		   ->GetBinError  (ibin) ;
 			    	if ( h_CS_entries )entriesCSeta        = h_CS_entries      ->GetBinContent(ibin) ;
  			    	if ( h_CS_entries )entriesCS1eta       = h_CS1_entries     ->GetBinContent(ibin) ;
			    	if ( h_CS_entries )entriesCS2eta       = h_CS2_entries     ->GetBinContent(ibin) ;
                                //std::cout << " entriesCSeta " << entriesCSeta <<std::endl;
				if (entriesCSeta>0){
				 //std::cout << " entriesCS1eta " << entriesCS1eta <<std::endl;
                                 rateCS1eta     =entriesCS1eta/entriesCSeta;
                                 rateCS2eta     =entriesCS2eta/entriesCSeta;
                                 rateCSmore2eta =(entriesCSeta-(entriesCS1eta+entriesCS2eta))/entriesCSeta;
                                }

				if (applyEffThreshold){ 
				 if(effeta<Minimum_efficiency&&eta_effphi<Minimum_efficiency){
				   effeta      =Minimum_efficiency;
				   gapeffeta   =Minimum_efficiency;
				   erreffeta   =                 0.1;
				   errgapeffeta=                 0.1;
				   PanelStripsStatus   = PanelStripsStatusOK;
				   cl_sizeeta     =   1;
				   errcl_sizeeta  = 0.1,
			           rateCS1eta     =   1;
                                   rateCS2eta     =   0;
                                   rateCSmore2eta =   0; 
				 }
				 else if(effeta<Minimum_efficiency&&eta_effphi>Minimum_efficiency){
				   effeta      = Minimum_efficiency;
				   gapeffeta   =            eta_effphi;
				   erreffeta   =                   0.1;
				   errgapeffeta=                   0.1;
				   PanelStripsStatus   = PanelStripsStatusOK;
				   cl_sizeeta     =   1;
				   errcl_sizeeta  = 0.1,
			           rateCS1eta     = 1;
                                   rateCS2eta     = 0;
                                   rateCSmore2eta = 0;
				 }
				 else if(effeta>Minimum_efficiency&&eta_effphi<Minimum_efficiency){
				   gapeffeta   =              effeta;
				   errgapeffeta=                 0.1;
				 }
				}
        		        //std::cout << "  erreffeta  "<<erreffeta << "  effeta  "<<effeta <<"  errgapeff  "<<errgapeff << "  gapeff  "<<gapeff <<" rateCS1eta  "<<  rateCS1eta << " rateCS2eta " << rateCS2eta <<std::endl;
	
			    	sprintf(arr_effeta	   ,	"%f ", effeta		 ) ;   arr_effeta 	  [5]	=0;
			    	sprintf(arr_erreffeta	   ,	"%f ", erreffeta	 ) ;   arr_erreffeta	  [5]	=0;
			    	sprintf(arr_gapeffeta	   ,	"%f ", gapeffeta	 ) ;   arr_gapeffeta	  [5]	=0;
			    	sprintf(arr_errgapeffeta     ,	"%f ", errgapeffeta	 ) ;   arr_errgapeffeta	  [5]	=0;
			    	sprintf(arr_reseta_cs1	   ,	"%f ", reseta_cs1	 ) ;   arr_reseta_cs1	  [5]	=0;
			    	sprintf(arr_errreseta_cs1    ,	"%f ", errreseta_cs1	 ) ;   arr_errreseta_cs1    [5]	=0;
			    	sprintf(arr_reseta_cs2	   ,	"%f ", reseta_cs2	 ) ;   arr_reseta_cs2	  [5]	=0;
			    	sprintf(arr_errreseta_cs2    ,	"%f ", errreseta_cs2	 ) ;   arr_errreseta_cs2    [5]	=0;
			    	sprintf(arr_reseta_csother   ,	"%f ", reseta_csother	 ) ;   arr_reseta_csother   [5]	=0;
			    	sprintf(arr_errreseta_csother,	"%f ", errreseta_csother ) ;   arr_errreseta_csother[5]	=0;
			    	sprintf(arr_noiseeta	   ,	"%f ", noiseeta 	 ) ;   arr_noiseeta	  [5]	=0;
			    	sprintf(arr_errnoiseeta	   ,	"%f ", errnoiseeta	 ) ;   arr_errnoiseeta	  [5]	=0;
				sprintf(arr_noiseeta_cor	   ,    "%f ", noiseeta_cor      ) ;   arr_noiseeta_cor	  [5]   =0;
                                sprintf(arr_errnoiseeta_cor  ,    "%f ", errnoiseeta_cor   ) ;   arr_errnoiseeta_cor  [5]   =0;
			    	sprintf(arr_cl_sizeeta	   ,	"%f ", cl_sizeeta	 ) ;   arr_cl_sizeeta	  [5]	=0;
			    	sprintf(arr_errcl_sizeeta    ,	"%f ", errcl_sizeeta	 ) ;   arr_errcl_sizeeta    [5]	=0;
			    	sprintf(arr_rateCS1eta       ,	"%f ", rateCS1eta	 ) ;   arr_rateCS1eta       [5]	=0;
			    	sprintf(arr_rateCS2eta       ,	"%f ", rateCS2eta	 ) ;   arr_rateCS2eta       [5]	=0;
			    	sprintf(arr_rateCSmore2eta   ,	"%f ", rateCSmore2eta	 ) ;   arr_rateCSmore2eta   [5]	=0;
	
			        char PanelRes   [255]; //eff_eta, res_cs1, res_cs2, res_csother, time, mean and rms
			        char StripStatus   [4096]; //strips status 0 to 9 for dead noisy strips
				
 			        sprintf(PanelRes,  "%d %d %d %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s", TableVersionCondDB,  n_tr_peta, StripsOnPanel,  arr_effeta, arr_erreffeta,  arr_gapeffeta, arr_errgapeffeta, arr_reseta_cs1, arr_errreseta_cs1, arr_reseta_cs2, arr_errreseta_cs2, arr_reseta_csother, arr_errreseta_csother, arr_noiseeta, arr_errnoiseeta, arr_noiseeta_cor, arr_errnoiseeta_cor, arr_cl_sizeeta, arr_errcl_sizeeta, arr_rateCS1eta, arr_rateCS2eta, arr_rateCSmore2eta) ;
 			        sprintf(StripStatus, "%s", PanelStripsStatus.c_str()) ;
				std::string cool_tagCondDB="RecoCondDB";		        
                                coolrpc.setSince(0U,0U);		
                                coolrpc.setUntil(4294967295U,0U);	
 			        coolrpc.insertCondDB_withTag(run_number*0+429496729U,PanelCode,PanelRes, StripStatus,cool_tagCondDB);
			        if(printout&&EffThreshold)std::cout << stripProfile_name << " under THR "<< EffThreshold <<" "<< PanelCode << " ibin " << ibin << " h_EffEta " << h_Eff->GetBinContent(ibin) <<" h_EffPhi " << h_Eff->GetBinContent(ibin_perp) << " h_GapEffEta " << h_GapEff->GetBinContent(ibin) <<" h_GapEffPhi " << h_GapEff->GetBinContent(ibin_perp) << " cool_EtaEff "<< effeta <<" cool_GapEffEta "<< gapeffeta <<" --- Eta Summary " << PanelRes<< " --- StripStatus " << StripStatus << std::endl;
				if(printout&&EffThreshold)std::cout<<"inCOOL_ETA_id_ntrk_panelEff_gapEff "<<PanelCode<<" "<<n_tr_peta<<" "<<(int)(n_tr_peta*effeta)<<" "<<(int)(n_tr_peta*gapeffeta)<<std::endl;	
			        countpanelindb++;
			        if(effeta==0.0)countpaneleff0++;
			        if(n_tr_peta==0)countpaneltrack0++;
			      
			    }else{
			     if (h_TrackProj) n_tr_pphi  =(int)h_TrackProj   -> GetBinContent(ibin) ;
 			     //if(n_tr_pphi >0){
 
			     if ( h_PanelId )Binposition = (int)h_PanelId-> GetBinCenter(ibin) ;
			     int ibin_perp=0;
			     if(Binposition>0){
			       ibin_perp=ibin-1;
			       if ( h_GapEff	)gapeff 	     = h_GapEff 	 ->GetBinContent(ibin-1) ;
			       if ( h_GapEff	)errgapeff	     = h_GapEff 	 ->GetBinError  (ibin-1) ;
			       if ( h_Eff 	)phi_effeta 	     = h_Eff		 ->GetBinContent(ibin-1) ;
			       
			     }else{
			       ibin_perp=ibin+1;
			       if ( h_GapEff	)gapeff 	     = h_GapEff 	 ->GetBinContent(ibin+1) ;
 			       if ( h_GapEff	)errgapeff	     = h_GapEff 	 ->GetBinError  (ibin+1) ;
			       if ( h_Eff 	)phi_effeta 	     = h_Eff		 ->GetBinContent(ibin+1) ;
 			     }
			     if ( h_Eff 	)effphi 	     = h_Eff		 ->GetBinContent(ibin) ;
 			     if ( h_Eff 	)erreffphi	     = h_Eff		 ->GetBinError  (ibin) ;
			     

			     gapeffphi   =               gapeff;
			     errgapeffphi=            errgapeff;
			     EffThreshold = (effphi<Minimum_efficiency)|| (phi_effeta<Minimum_efficiency);
			     
			     
			     if ( h_Res_CS1	)resphi_cs1	     = h_Res_CS1	 ->GetBinContent(ibin) ;
			     if ( h_Res_CS1	)errresphi_cs1       = h_Res_CS1	 ->GetBinError  (ibin) ;
 			     if ( h_Res_CS2	)resphi_cs2	     = h_Res_CS2	 ->GetBinContent(ibin) ;
 			     if ( h_Res_CS2	)errresphi_cs2       = h_Res_CS2	 ->GetBinError  (ibin) ;
 			     if ( h_Res_CSmore2 )resphi_csother      = h_Res_CSmore2	 ->GetBinContent(ibin) ;
 			     if ( h_Res_CSmore2 )errresphi_csother   = h_Res_CSmore2	 ->GetBinError  (ibin) ;
 			     if ( h_NoiseTot	)noisephi	     = h_NoiseTot	 ->GetBinContent(ibin) ;
 			     if ( h_NoiseTot	)errnoisephi	     = h_NoiseTot	 ->GetBinError  (ibin) ;
			     if ( h_NoiseCorr   )noisephi_cor        = h_NoiseCorr	 ->GetBinContent(ibin) ;    
	                     if ( h_NoiseCorr   )errnoisephi_cor     = h_NoiseCorr	 ->GetBinError  (ibin) ;    
 			     if ( h_CS  	)cl_sizephi	     = h_CS		 ->GetBinContent(ibin) ;
 			     if ( h_CS  	)errcl_sizephi       = h_CS		 ->GetBinError  (ibin) ;
 			     if ( h_CS_entries  )entriesCSphi	     = h_CS_entries	 ->GetBinContent(ibin) ;
  			     if ( h_CS_entries  )entriesCS1phi	     = h_CS1_entries	 ->GetBinContent(ibin) ;
			     if ( h_CS_entries  )entriesCS2phi	     = h_CS2_entries	 ->GetBinContent(ibin) ;
                             if(entriesCSphi>0){
			      rateCS1phi     =entriesCS1phi/entriesCSphi;
                              rateCS2phi     =entriesCS2phi/entriesCSphi;
                              rateCSmore2phi =(entriesCSphi-(entriesCS1phi+entriesCS2phi))/entriesCSphi;
 			     }	
			     
			     
			     if (applyEffThreshold){ 
			      if(effphi<Minimum_efficiency&&phi_effeta<Minimum_efficiency){
				  effphi      =Minimum_efficiency;
				  gapeffphi   =Minimum_efficiency;
				  erreffphi   =                 0.1;
				  errgapeffphi=                 0.1;
				  PanelStripsStatus   = PanelStripsStatusOK;
				  cl_sizephi     =   1;
				  errcl_sizephi  = 0.1,
			          rateCS1phi     = 1;
                                  rateCS2phi     = 0;
                                  rateCSmore2phi = 0;
			      }
			      else if(effphi<Minimum_efficiency&&phi_effeta>Minimum_efficiency){
				  effphi      =       Minimum_efficiency;
				  gapeffphi   =                 phi_effeta;
				  erreffphi   =                        0.1;
				  errgapeffphi=                        0.1;
				  PanelStripsStatus   = PanelStripsStatusOK;
				  cl_sizephi     =   1;
				  errcl_sizephi  = 0.1,
			          rateCS1phi     = 1;
                                  rateCS2phi     = 0;
                                  rateCSmore2phi = 0;
			      }
			      else if(effphi>Minimum_efficiency&&phi_effeta<Minimum_efficiency){   
				  gapeffphi   =              effphi; 
				  errgapeffphi=                 0.1;
				  PanelStripsStatus   = PanelStripsStatusOK;
			      }  
			     }
			     
			     
			     
			     
			     
			     
			     	    
        		     //std::cout << "  erreffphi  "<<erreffphi << "  effphi  "<<effphi <<"  errgapeff  "<<errgapeff << "  gapeff  "<<gapeff << "  rateCS1phi  "<<rateCS1phi<< " rateCS2phi   "<<rateCS2phi<<std::endl;
			     
			     
			     sprintf(arr_effphi		,    "%f ", effphi	      ) ;   arr_effphi	       [5]   =0;
			     sprintf(arr_erreffphi	,    "%f ", erreffphi	      ) ;   arr_erreffphi        [5]   =0;
 			     sprintf(arr_gapeffphi	,    "%f ", gapeffphi	      ) ;   arr_gapeffphi        [5]   =0;
			     sprintf(arr_errgapeffphi	,    "%f ", errgapeffphi      ) ;   arr_errgapeffphi     [5]   =0;
			     sprintf(arr_resphi_cs1	,    "%f ", resphi_cs1        ) ;   arr_resphi_cs1       [5]   =0;
			     sprintf(arr_errresphi_cs1	,    "%f ", errresphi_cs1     ) ;   arr_errresphi_cs1    [5]   =0;
			     sprintf(arr_resphi_cs2	,    "%f ", resphi_cs2        ) ;   arr_resphi_cs2       [5]   =0;
			     sprintf(arr_errresphi_cs2	,    "%f ", errresphi_cs2     ) ;   arr_errresphi_cs2    [5]   =0;
			     sprintf(arr_resphi_csother	,    "%f ", resphi_csother    ) ;   arr_resphi_csother   [5]   =0;
			     sprintf(arr_errresphi_csother,    "%f ", errresphi_csother ) ;   arr_errresphi_csother[5]   =0;
			     sprintf(arr_noisephi 	,    "%f ", noisephi	      ) ;   arr_noisephi         [5]   =0;
			     sprintf(arr_errnoisephi	,    "%f ", errnoisephi       ) ;   arr_errnoisephi      [5]   =0;
			     sprintf(arr_noisephi_cor     ,    "%f ", noisephi_cor      ) ;   arr_noisephi_cor     [5]   =0;
                             sprintf(arr_errnoisephi_cor  ,    "%f ", errnoisephi_cor   ) ;   arr_errnoisephi_cor  [5]   =0;
			     sprintf(arr_cl_sizephi	,    "%f ", cl_sizephi        ) ;   arr_cl_sizephi       [5]   =0;
			     sprintf(arr_errcl_sizephi	,    "%f ", errcl_sizephi     ) ;   arr_errcl_sizephi    [5]   =0;
			     sprintf(arr_rateCS1phi	,    "%f ", rateCS1phi        ) ;   arr_rateCS1phi       [5]   =0;
			     sprintf(arr_rateCS2phi	,    "%f ", rateCS2phi        ) ;   arr_rateCS2phi       [5]   =0;
			     sprintf(arr_rateCSmore2phi	,    "%f ", rateCSmore2phi    ) ;   arr_rateCSmore2phi   [5]   =0;
			    		
			     char PanelRes   [255]; //eff_eta, res_cs1, res_cs2, res_csother, time, mean and rms
			     char StripStatus   [4096]; //strips status 0 to 9 for dead noisy strips
 			     sprintf(PanelRes,  "%d %d %d %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s", TableVersionCondDB,  n_tr_pphi, StripsOnPanel,  arr_effphi, arr_erreffphi,  arr_gapeffphi, arr_errgapeffphi, arr_resphi_cs1, arr_errresphi_cs1, arr_resphi_cs2, arr_errresphi_cs2, arr_resphi_csother, arr_errresphi_csother, arr_noisephi, arr_errnoisephi, arr_noisephi_cor, arr_errnoisephi_cor, arr_cl_sizephi, arr_errcl_sizephi, arr_rateCS1phi, arr_rateCS2phi, arr_rateCSmore2phi) ;
 			     sprintf(StripStatus, "%s", PanelStripsStatus.c_str()) ;
 			     std::string cool_tag="RecoCondDB";		        
                             coolrpc.setSince(0U,0U);		
                             coolrpc.setUntil(4294967295U,0U);	
 			     coolrpc.insertCondDB_withTag(run_number*0+429496729U,PanelCode,PanelRes, StripStatus,cool_tag);
			     if(printout&&EffThreshold)std::cout << stripProfile_name << " under THR "<< EffThreshold <<" " << PanelCode << " ibin " << ibin << " h_EffPhi " << h_Eff->GetBinContent(ibin) <<" h_EffEta " << h_Eff->GetBinContent(ibin_perp) << " h_GapEffPhi " << h_GapEff->GetBinContent(ibin) <<" h_GapEffEta " << h_GapEff->GetBinContent(ibin_perp) << " cool_PhiEff "<< effphi <<" cool_GapEffPhi "<< gapeffphi <<" --- Phi Summary " << PanelRes<< " --- StripStatus " << StripStatus << std::endl;
			     if(printout&&EffThreshold)std::cout<<"inCOOL_PHI_id_ntrk_panelEff_gapEff "<<PanelCode<<" "<<n_tr_pphi<<" "<<(int)(n_tr_pphi*effphi)<<" "<<(int)(n_tr_pphi*gapeffphi)<<std::endl;
			    countpanelindb++;
			    if(effphi==0.0)countpaneleff0++;
			    if(n_tr_pphi==0)countpaneltrack0++;
			  }		
	    	          StripsOnPanel=1;
	      	          PanelStripsStatus.clear();
	      	          PanelStripsStatusOK.clear();
 		        }
			//if(StripsOnPanel>1) std::cout<<stripProfile_name<< " bin " << Nstrips << " Not found PanelStripId " << PanelStripId<< std::endl;		
	    	        StripsOnPanel=1;
	      	        PanelStripsStatus.clear();
	      	        PanelStripsStatusOK.clear();			
		      }
                    }		  
		  }				
	        } // end loop on DoubletPhi	
	      } // end loop on layers
	    }// end Cool Folder	  
	    std::cout <<"Count RC panels in DB " << countpanelindb << " Count RPCpanels in DB with zero efficiency "<< countpaneleff0 <<" Count RPCpanels in DB with zero track "<<countpaneltrack0 <<std::endl;	    
  	  } // end for sectors
	  std::cout <<"Count RC panels in DB " << countpanelindb << " Count RPCpanels in DB with zero efficiency "<< countpaneleff0 <<" Count RPCpanels in DB with zero track "<<countpaneltrack0 <<std::endl;
	// write distribution plots all ATLAS
	TDirectory* dirA = f->GetDirectory( dir_sideA_track.c_str() ) ;
	if ( dirA != 0 ) {
	  dirA->cd();
	  if ( h_AverageEff_A            ) h_AverageEff_A	     ->Write("",TObject::kOverwrite) ;
	  if ( h_AverageGapEff_A         ) h_AverageGapEff_A         ->Write("",TObject::kOverwrite) ;
	  if ( h_AverageRes_CS1_A        ) h_AverageRes_CS1_A        ->Write("",TObject::kOverwrite) ;
	  if ( h_AverageRes_CS1rms_A     ) h_AverageRes_CS1rms_A     ->Write("",TObject::kOverwrite) ;
	  if ( h_AverageRes_CS2_A        ) h_AverageRes_CS2_A        ->Write("",TObject::kOverwrite) ;
	  if ( h_AverageRes_CS2rms_A     ) h_AverageRes_CS2rms_A     ->Write("",TObject::kOverwrite) ;
	  if ( h_AverageRes_CSmore2_A    ) h_AverageRes_CSmore2_A    ->Write("",TObject::kOverwrite) ;
	  if ( h_AverageRes_CSmore2rms_A ) h_AverageRes_CSmore2rms_A ->Write("",TObject::kOverwrite) ;
	  if ( h_AverageOccupancy_A      ) h_AverageOccupancy_A      ->Write("",TObject::kOverwrite) ;
	  if ( h_AverageCS_A             ) h_AverageCS_A             ->Write("",TObject::kOverwrite) ;
	  if ( h_AverageTime_A           ) h_AverageTime_A           ->Write("",TObject::kOverwrite) ;
	  if ( h_AverageNoiseCorr_A      ) h_AverageNoiseCorr_A      ->Write("",TObject::kOverwrite) ;
	  if ( h_AverageNoiseTot_A       ) h_AverageNoiseTot_A       ->Write("",TObject::kOverwrite) ;
	}
	TDirectory* dirC = f->GetDirectory( dir_sideC_track.c_str() ) ;
	if ( dirC != 0 ) {
	  dirC->cd();
	  if ( h_AverageEff_C            ) h_AverageEff_C	     ->Write("",TObject::kOverwrite) ;
	  if ( h_AverageGapEff_C         ) h_AverageGapEff_C         ->Write("",TObject::kOverwrite) ;
	  if ( h_AverageRes_CS1_C        ) h_AverageRes_CS1_C        ->Write("",TObject::kOverwrite) ;
	  if ( h_AverageRes_CS1rms_C     ) h_AverageRes_CS1rms_C     ->Write("",TObject::kOverwrite) ;
	  if ( h_AverageRes_CS2_C        ) h_AverageRes_CS2_C        ->Write("",TObject::kOverwrite) ;
	  if ( h_AverageRes_CS2rms_C     ) h_AverageRes_CS2rms_C     ->Write("",TObject::kOverwrite) ;
	  if ( h_AverageRes_CSmore2_C    ) h_AverageRes_CSmore2_C    ->Write("",TObject::kOverwrite) ;
	  if ( h_AverageRes_CSmore2rms_C ) h_AverageRes_CSmore2rms_C ->Write("",TObject::kOverwrite) ;
	  if ( h_AverageOccupancy_C      ) h_AverageOccupancy_C      ->Write("",TObject::kOverwrite) ;
	  if ( h_AverageCS_C             ) h_AverageCS_C             ->Write("",TObject::kOverwrite) ;
	  if ( h_AverageTime_C           ) h_AverageTime_C           ->Write("",TObject::kOverwrite) ;
	  if ( h_AverageNoiseCorr_C      ) h_AverageNoiseCorr_C      ->Write("",TObject::kOverwrite) ;
	  if ( h_AverageNoiseTot_C       ) h_AverageNoiseTot_C       ->Write("",TObject::kOverwrite) ;
	}
	
		 
      }
    }else {
      delete obj_run;
    } // end if tdir_run !=0    
    
  }
  f->Close();
  delete f;
  //std::cout << "\n";
  //std::cout << "Finish RPC analysis"<<std::endl;
}



 


bool
MonitoringFile::
RPCCheckHistogram(TFile* f,const char* HistoName)
 {
  if (!(f->Get(HistoName))) {
    //std::cerr << "RPC PostProcessing: no such histogram \""<< HistoName << "\"\n"; 
    return false;
  }
  else return true;
 }

}
