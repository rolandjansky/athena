/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////////////////////////
//Methods to Process TGC Histograms after merging.
//Author: Akimasa Ishikawa (akimasa.ishikawa@cern.ch)
//Date:   Aug 2009
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "DataQualityUtils/MonitoringFile.h"
#include "DataQualityUtils/CoolTgc.h"

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include <sstream>
#include <vector>
#include <utility>
#include <map>
#include <string>

#include "TH1F.h"
#include "TH2F.h"
#include "TFile.h"
#include "TClass.h"
#include "TKey.h"
#include "TMath.h"
#include "TTree.h"
#include "TBranch.h"
#include "TGraph.h"

namespace dqutils {

  static const float TGCChamberLowOccupancyCut  = 1.0e-6;
  static const float TGCChamberHighOccupancyCut  = 0.005;
  static const float TGCChannelOccupancyCut  = 0.01;
  static const float TGCChamberEfficiencyCut = 0.7;
  static const float TGCChamberTimingCut = 0.95;

  static const bool tgc_debug = false;

  void 
  MonitoringFile::TGCPostProcess( const std::string & inFilename, bool /* isIncremental */ ) 
  {
    //start postprocessing
    std::vector< std::pair< std::string, float > > NoisyChambers;
    std::vector< std::pair< std::string, float > > DeadChambers;

    std::vector< std::pair< std::string, float > > NoisyChannels;

    std::vector< std::pair< std::string, float > > LowEfficiencyChambers;

    std::vector< std::pair< std::string, float > > ReadoutTimingChambers;
    std::vector< std::pair< std::string, float > > TriggerTimingChambers;

    TGCLV1HistogramDivision( inFilename);
    TGCRawHistogramDivision( inFilename);
    TGCChamberOccupancy( inFilename, NoisyChambers, DeadChambers );
    //    TGCChannelOccupancy( inFilename, run_dir, NoisyChannels );
    
    TGCChamberEfficiency( inFilename, LowEfficiencyChambers );
    TGCChamberTiming( inFilename, ReadoutTimingChambers, TriggerTimingChambers);
    //DQA results to COOL

    return;
  }//MonitoringFile::TGCPostProcess

  bool
  MonitoringFile::TGCCheckHistogram(TFile *f, TString &hname)
  {
    if (!(f->Get(hname))) {
      //std::cerr << "TGC PostProcessing: no such histogram!! : "<< hname << std::endl;
      gDirectory->pwd();
      gDirectory->ls();
      return false;
    }
    return true;
  }//MonitoringFile::TGCCheckHistogram

  void 
  MonitoringFile::TGCChamberOccupancy(const std::string & inFilename, 
				      std::vector< std::pair< std::string, float > >& noisychambers, 
				      std::vector< std::pair< std::string, float > >& deadchambers)
  {
    PostProcessorFileWrapper mf( inFilename , "TGC ChamberOcc Calculations");
    if (!mf.IsOpen()) {
      //std::cerr << "TGCPostProcess(): "
      //	<< "Input file not opened \n";
      return;
    }
    if(mf.GetSize()<1000.) {
      //std::cerr << "TGCPostProcess(): "
      //	<< "Input file empty \n";
      return; 
    }
    /*
    // get run directory name
    //Seemingly unnecessary lines are necessary
    TIter nextcd0(gDirectory->GetListOfKeys());
    TKey *key0 = (TKey*)nextcd0();
    TDirectory *dir0= dynamic_cast<TDirectory*> (key0->ReadObj());
    dir0->cd();
    ///
    */
    TIter next_run ( mf.GetListOfKeys() );
    TKey* key_run(0);
    while ((key_run = dynamic_cast<TKey*> ( next_run() )) !=0 ) {
      if (!key_run->IsFolder()) continue;
      TString run_dir = key_run->GetName();
      if (!run_dir.Contains("run"))continue;

      std::string run_dir2 = run_dir.Data();
      //int run_number = atoi( (run_dir2.substr(4, run_dir2.size()-4 )).c_str() );
      //run_number=run_number;

      TString tgc_dir = run_dir + "/Muon/MuonRawDataMonitoring/TGC/";

      TString tgc_global_dir = tgc_dir + "Global/";

      TString tgc_sub_dir[2] = {tgc_dir + "TGCEA/", tgc_dir + "TGCEC/"};

      TString sac[2]={"_A","_C"};
      TString side[2]={"A","C"};
      TString sws[2]={"Wire_","Strip_"};

      std::stringstream ss;

      //Summary histograms
      TString schambertypesummary[2][4][6];//[ws][station][eta]
      TH1F* chambertypesummary[2][4][6];//

      TString schambersummary[2][2];//[ac][ws]
      TH1F* chambersummary[2][2];//

      std::string type[17]={"_F_T1", "_E4_T3", "_E3_T6", "_E2_T7", "_E1_T8",
                            "_F_T2", "_E5_T4", "_E4_T6", "_E3_T7", "_E2_T8","_E1_T9",
                            "_F_T2", "_E5_T5", "_E4_T6", "_E3_T7", "_E2_T8","_E1_T9" };

      //get summary histograms
      for(int ws=0;ws<2;ws++){
        int ntype=0;
        for(int station=0;station<3;station++){
          for(int eta=0;eta<6;eta++){
            if(eta==5 && station==0)continue;
            if(eta<2 && station==3)continue;
            
            ss.str("");
            ss<<"Station"<<station+1<<type[ntype];
            schambertypesummary[ws][station][eta] = tgc_dir + "Global/Summary/" 
              + "Summary_Of_Log10_" + sws[ws] + "Occupancy_Per_Chamber_Type_" + ss.str();  
            if( tgc_debug ) std::cout << schambertypesummary[ws][station][eta] << std::endl;

	    chambertypesummary[ws][station][eta] = 0;
	    mf.get(schambertypesummary[ws][station][eta], chambertypesummary[ws][station][eta]);
	    if(!chambertypesummary[ws][station][eta]){
	      //std::cerr <<"TGC PostProcessing: no such histogram!! "<< schambertypesummary[ws][station][eta] << std::endl;
	      continue;
	    }
	    TGCResetContents(chambertypesummary[ws][station][eta]);

            ntype++;

          }//eta
        }//station

        for( int ac=0;ac<2;ac++){
          schambersummary[ac][ws] = tgc_sub_dir[ac] + "Summary/" 
            + "Summary_Of_Log10_" + sws[ws] + "Occupancy_Per_GasGap" + sac[ac];
          if( tgc_debug ) std::cout << schambersummary[ac][ws] << std::endl;
	  
	  chambersummary[ac][ws] = 0;
	  mf.get(schambersummary[ac][ws], chambersummary[ac][ws]);
	  if(!chambersummary[ac][ws]){
	    //std::cerr <<"TGC PostProcessing: no such histogram!! "<< schambersummary[ac][ws] << std::endl;
	    continue;
	  }
	  TGCResetContents(chambersummary[ac][ws]);

        }// ac
      }// ws

      //get number of events processed
      TString sentries = tgc_global_dir + "Event_Counter";
      
      TH1F *hentries = 0;
      mf.get(sentries, hentries);
      if(!hentries){
	//std::cerr <<"TGC PostProcessing: no such histogram!! "<< sentries << std::endl;
	continue;
      }
      
      double nentries = hentries->GetEntries();
  
      //calculate occupancies
      for(int ac=0;ac<2;ac++){
        for(int ws=0;ws<2;ws++){
          double min=1.;
          double max=0.;

          TString sprof = tgc_sub_dir[ac] + "Profile/" + sws[ws] + "Profile_Map" + sac[ac];
          TString soccu = tgc_sub_dir[ac] + "Occupancy/" + sws[ws] + "Occupancy_Map" + sac[ac];
          
	  TH2F *hprof = 0;
	  mf.get(sprof, hprof);
	  if(!hprof){
	    //std::cerr <<"TGC PostProcessing: no such histogram!! "<< sprof << std::endl;
	    continue;
	  }

	  TH2F *hoccu = 0;
	  mf.get(soccu, hoccu);
	  if(!hoccu){
	    //std::cerr <<"TGC PostProcessing: no such histogram!! "<< soccu << std::endl;
	    continue;
	  }
	  TGCResetContents(hoccu);
	  
          int nbinx = hprof->GetXaxis()->GetNbins();
          int nbiny = hprof->GetYaxis()->GetNbins();

          for(int binx=1; binx<=nbinx; binx++){
            for(int biny=1; biny<=nbiny; biny++){
              if( ws==1 && (binx==2||binx==9||binx==16||binx==23||binx==34) ) continue;//skip strip for layer2
              if( ( binx>=33 && binx<=43 ) &&
                  biny%2 == 0 )continue;// skip phi1,3 for forward region
              if( ( binx ==40 || binx==41 ) &&
                  ( biny ==19 || biny==35 || biny==43 ) ) continue;//skip phi9,17,21 for EI

              double num = hprof->GetBinContent(binx,biny);
              double den = nentries * nTGCWireStripMap( ws, binx-1, biny );
              double occu = 0.;
              double eoccu = 0.;
              if(num>0 && nentries>0){
                occu = (double)num/den;
                eoccu = sqrt( occu*(1.-occu)/den );
              }
              hoccu->SetBinContent(binx,biny,occu);
              hoccu->SetBinError(binx,biny,eoccu);

              // to set minimum and maximum
              if(occu<min && occu > 1e-8)min=occu;
              if(occu>max)max=occu;

              if( occu < 1.e-8 ){
                occu=1.e-8;
                eoccu=1.e-8;
              }
	      if(chambersummary[ac][ws])
		chambersummary[ac][ws]->Fill(log10(occu));

              int station=0;
              int eta=-1;
              if( binx < 16){
                station=0;
                eta=(binx-1)%5;
              } else if ( binx < 28 ){
                station=1;
                eta=(binx-16)%6;
              } else if ( binx < 40 ){
                station=2;
                eta=(binx-28)%6;
              } else {
                station=3;
                eta=(binx-40)%2;
              }
              if(station<3 && eta >=0)
		if(chambertypesummary[ws][station][eta])
		  chambertypesummary[ws][station][eta]->Fill(log10(occu));

              //high occupancy chamber
              if(occu > TGCChamberHighOccupancyCut){
                ss.str("");
                TString schamber = hprof->GetXaxis()->GetBinLabel(binx);
                int sector = (biny-1)/4 + 1;
                int phi = (biny-1)%4;
                ss<< side[ac];
                if(sector<10)ss<<"0";
                ss<< sector <<"phi"<<phi;
                if( tgc_debug ) std::cout<<"TGC noisy channel : " << ss.str() << " " << occu << std::endl;
                
                std::pair<std::string, float> p(ss.str(),occu);
                noisychambers.push_back(p);
              }else if( occu < TGCChamberLowOccupancyCut ){//too low occupancy
                ss.str("");
                TString schamber = hprof->GetXaxis()->GetBinLabel(binx);
                int sector = (biny-1)/4 + 1;
                int phi = (biny-1)%4;
                ss<< side[ac];
                if(sector<10)ss<<"0";
                ss<< sector <<"phi"<<phi;
                if( tgc_debug ) std::cout<<"TGC low occupancy chamber : " << ss.str() << " " << occu << std::endl;

                std::pair<std::string, float> p(ss.str(),occu);
                deadchambers.push_back(p);
              }

            }//biny
          }//binx
	  
          hoccu->SetMinimum(min*0.95);
          hoccu->SetMaximum(max*1.05);
	  
          TString occu_dir = tgc_sub_dir[ac] + "Occupancy/" ;
	  TDirectory* dir = mf.GetDirectory(occu_dir);
	  
          if(dir){
            dir->cd();
	    hoccu->Write("",TObject::kOverwrite);
          }else{
            //std::cerr<<"TGCChamberOccupancy: directory "<<occu_dir<<" not found"<<std::endl;
          }
	  
        }//ws
      }//ac
      mf.Write();

      // if directory is found, save the summary histogram
      TString sum_dir = tgc_dir + "Global/Summary";
      TDirectory* dir = mf.GetDirectory(sum_dir);

      if(dir){
        dir->cd();
        for(int ws=0;ws<2;ws++){
          for(int station=0;station<3;station++){
            for(int eta=0;eta<6;eta++){
              if(eta==5 && station==0)continue;
              if(eta<2 && station==3)continue;
	      if(chambertypesummary[ws][station][eta])
		chambertypesummary[ws][station][eta]->Write("",TObject::kOverwrite);

            }//eta
          }//station
        }//ws
      }else{
	//std::cerr<<"TGCChamberOccupancy: directory "<<sum_dir<<" not found"<<std::endl;
      }//if
      mf.Write();

      for( int ac=0 ;ac<2 ;ac++){
        // if directory is found, save the summary histogram
        sum_dir = tgc_sub_dir[ac] + "Summary/";
	dir = mf.GetDirectory(sum_dir);
	if(dir){
	  dir->cd();
	  for(int ws=0;ws<2;ws++){
	    if(chambersummary[ac][ws])
	      chambersummary[ac][ws]->Write("",TObject::kOverwrite);
	  }//ws
	}else{
	  //std::cerr<<"TGCChamberOccupancy: directory "<<sum_dir<<" not found"<<std::endl;
	}//if
      }//ac
      mf.Write();
      
    }//while

    mf.Write();
  }//MonitoringFile::TGCChamberOccupancy

  void 
  MonitoringFile::TGCChamberEfficiency(const std::string & inFilename, 
				       std::vector< std::pair< std::string, 
				       float > >& loweffchambers)
  {
    PostProcessorFileWrapper mf( inFilename , "TGC ChamberEff Calculations");
    if (!mf.IsOpen()) {
      //std::cerr << "TGCPostProcess(): "
      //	<< "Input file not opened \n";
      return;
    }
    if(mf.GetSize()<1000.) {
      //std::cerr << "TGCPostProcess(): "
      //	<< "Input file empty \n";
      return;
    }
    /*
    // get run directory name
    //Seemingly unnecessary lines are necessary
    TIter nextcd0(gDirectory->GetListOfKeys());
    TKey *key0 = (TKey*)nextcd0();
    TDirectory *dir0= dynamic_cast<TDirectory*> (key0->ReadObj());
    dir0->cd();
    ///
    */
    TIter next_run ( mf.GetListOfKeys() );
    TKey* key_run(0);
    while ((key_run = dynamic_cast<TKey*> ( next_run() )) !=0 ) {
      if (!key_run->IsFolder()) continue;
      TString run_dir = key_run->GetName();
      if (!run_dir.Contains("run"))continue;

      std::string run_dir2 = run_dir.Data();
      //int run_number = atoi( (run_dir2.substr(4, run_dir2.size()-4 )).c_str() );
      //run_number=run_number;

      TString tgc_dir = run_dir + "/Muon/MuonRawDataMonitoring/TGC/";

      TString tgc_sub_dir[2] = {tgc_dir + "TGCEA/", tgc_dir + "TGCEC/"};

      TString sac[2]={"_A","_C"};
      TString sws[2]={"Wire_","Strip_"};

      std::stringstream ss;

      //Summary histograms
      TString schambertypesummary[2][3][6];//[ws][station][eta]
      TH1F* chambertypesummary[2][3][6];//

      TString schambersummary[2][2];//[ac][ws]
      TH1F* chambersummary[2][2];//

      std::string type[17]={"_F_T1", "_E4_T3", "_E3_T6", "_E2_T7", "_E1_T8",
                            "_F_T2", "_E5_T4", "_E4_T6", "_E3_T7", "_E2_T8","_E1_T9",
                            "_F_T2", "_E5_T5", "_E4_T6", "_E3_T7", "_E2_T8","_E1_T9" };
      
      //get summary histograms
      for(int ws=0;ws<2;ws++){
        int ntype=0;
        for(int station=0;station<3;station++){
          for(int eta=0;eta<6;eta++){
            if(eta==5 && station==0)continue;
            if(eta<2 && station==3)continue;
            
            ss.str("");
            ss<<"Station"<<station+1<<type[ntype];
            schambertypesummary[ws][station][eta] = tgc_dir + "Global/Summary/" 
              + "Summary_Of_" + sws[ws] + "Efficiency_Per_Chamber_Type_" + ss.str();  
            if( tgc_debug ) std::cout << schambertypesummary[ws][station][eta] << std::endl;

	    chambertypesummary[ws][station][eta] = 0;
	    mf.get(schambertypesummary[ws][station][eta], chambertypesummary[ws][station][eta]);
	    if(!chambertypesummary[ws][station][eta]){
	      //std::cerr <<"TGC PostProcessing: no such histogram!! "<< schambertypesummary[ws][station][eta] << std::endl;
	      continue;
	    }
	    TGCResetContents(chambertypesummary[ws][station][eta]);
	    
	    ntype++;

          }//eta
        }//station

        for( int ac=0 ;ac<2 ;ac++){
          schambersummary[ac][ws] = tgc_sub_dir[ac] + "Summary/" 
            + "Summary_Of_" + sws[ws] + "Efficiency_Per_GasGap" + sac[ac];
          if( tgc_debug ) std::cout << schambersummary[ac][ws] << std::endl;

	  chambersummary[ac][ws] = 0;
	  mf.get(schambersummary[ac][ws], chambersummary[ac][ws]);
	  if(!chambersummary[ac][ws]){
	    //std::cerr <<"TGC PostProcessing: no such histogram!! "<< schambersummary[ac][ws] << std::endl;
	    continue;
	  }
	  TGCResetContents(chambersummary[ac][ws]);

        }//ac
      }//ws

      for(int ac=0;ac<2;ac++){
        for(int ws=0;ws<2;ws++){
          TString seff = tgc_sub_dir[ac] + "Efficiency/" + sws[ws] + "Efficiency_Map" + sac[ac];
          TString sden = tgc_sub_dir[ac] + "Efficiency/NumDenom/" + sws[ws] + "Efficiency_Map" + sac[ac] + "_Denominator";
	  
	  TH2F *heff = 0;
	  mf.get(seff, heff);
	  if(!heff){
	    //std::cerr <<"TGC PostProcessing: no such histogram!! "<< seff << std::endl;
	    continue;
	  }
	  
	  TH2F *hden = 0;
	  mf.get(sden, hden);
	  if(!hden){
	    //std::cerr <<"TGC PostProcessing: no such histogram!! "<< sden << std::endl;
	  }

          int nbinx = heff->GetXaxis()->GetNbins();
          int nbiny = heff->GetYaxis()->GetNbins();

          for(int binx=1; binx<=nbinx; binx++){
            for(int biny=1; biny<=nbiny; biny++){
              //if( ( binx==5 || binx==10 || binx==15 ||
              //      binx==21 || binx==27 || binx==33 || binx>=39 ) &&
              //    biny%2 == 0 )continue;// skip phi1,3 for forward region
              //if( ( binx ==40 || binx==42 ) &&
              //    ( biny ==18 || biny==34 || biny==42 ) ) continue;//skip phi9,17,21 for EI
              if( ( binx>=33 && binx<=43 ) &&
                  biny%2 == 0 )continue;// skip phi1,3 for forward region
              if( ( binx ==40 || binx==41 ) &&
                  ( biny ==19 || biny==35 || biny==43 ) ) continue;//skip phi9,17,21 for EI


              double eff = heff->GetBinContent(binx,biny);
	      double denom = 1;
	      if(hden){
		denom = hden->GetBinContent(binx,biny);
	      }
	      
	      if(chambersummary[ac][ws])
		if(denom>0)
		  chambersummary[ac][ws]->Fill( eff );

              int station=0;
              int eta=-1;
              if( binx < 16){
                station=0;
                eta=(binx-1)%5;
              } else if ( binx < 28 ){
                station=1;
                eta=(binx-16)%6;
              } else if ( binx < 40 ){
                station=2;
                eta=(binx-28)%6;
              } else {
                station=3;
                eta=(binx-40)%2;
              }
              if(station<3 && eta >=0)
		if(chambertypesummary[ws][station][eta])
		  if(denom>0)
		    chambertypesummary[ws][station][eta]->Fill( eff );

              //low efficiency chambers
              if(eff<TGCChamberEfficiencyCut){
                ss.str("");
                TString schamber = heff->GetXaxis()->GetBinLabel(binx);
                int sector = (biny-1)/4 + 1;
                int phi = (biny-1)%4;
                ss<< sac[ac];
                if(sector<10)ss<<"0";
                ss<< sector <<"phi"<<phi;

                if( tgc_debug ) std::cout<<"TGC low efficiency chamber : " << ss.str() << " " << eff << std::endl;

                std::pair<std::string, float> p(ss.str(),eff);
                loweffchambers.push_back(p);
              }

            }//biny
          }//binx

          TString eff_dir = tgc_sub_dir[ac] + "Efficiency/" ;
          TDirectory* dir = mf.GetDirectory(eff_dir);

          if(dir){
            dir->cd();
	    heff->Write("",TObject::kOverwrite);
          }else{
            //std::cerr<<"TGCChamberEfficiency: directory "<<eff_dir<<" not found"<<std::endl;
          }

        }//ws
      }//ac

      mf.Write();
      // if directory is found, save the summary histogram
      TString sum_dir = tgc_dir + "Global/Summary";
      TDirectory* dir = mf.GetDirectory(sum_dir);


      if(dir){
        dir->cd();
        for(int ws=0;ws<2;ws++){
          for(int station=0;station<3;station++){
            for(int eta=0;eta<6;eta++){
              if(eta==5 && station==0)continue;
              if(eta<2 && station==3)continue;
	      if(chambertypesummary[ws][station][eta])
		chambertypesummary[ws][station][eta]->Write("",TObject::kOverwrite);

            }//eta
          }//station
        }//ws
      }else{
        //std::cerr<<"TGCChamberEfficiency: directory "<<sum_dir<<" not found"<<std::endl;
      }//if
      mf.Write();

      for( int ac=0 ;ac<2 ;ac++){
        // if directory is found, save the summary histogram
        sum_dir = tgc_sub_dir[ac] + "Summary/";
        dir = mf.GetDirectory(sum_dir);
        if(dir){
          dir->cd();
          for( int ws=0; ws<2 ; ws++ ){
	    if(chambersummary[ac][ws])
	      chambersummary[ac][ws]->Write("",TObject::kOverwrite);
          
          }//ws
        }else{
          //std::cerr<<"TGCChamberEfficiency: directory "<<sum_dir<<" not found"<<std::endl;
        }//if
      }//ac
      mf.Write();
    }//while
    mf.Write();
  }//MonitoringFile::TGCChamberEfficiency

  void 
  MonitoringFile::TGCChamberTiming(const std::string & inFilename, 
                                   std::vector< std::pair< std::string, float > >& badrotimingchambers, 
                                   std::vector< std::pair< std::string, float > >& badtrgtimingchambers)
  {
    PostProcessorFileWrapper mf( inFilename , "TGC Chamber Timing Calculations");
    if (!mf.IsOpen()) {
      //std::cerr << "TGCPostProcess(): "
      //	<< "Input file not opened \n";
      return;
    }
    if(mf.GetSize()<1000.) {
      //std::cerr << "TGCPostProcess(): "
      //	<< "Input file empty \n";
      return; 
    }
    /*
    // get run directory name
    //Seemingly unnecessary lines are necessary
    TIter nextcd0(gDirectory->GetListOfKeys());
    TKey *key0 = (TKey*)nextcd0();
    TDirectory *dir0= dynamic_cast<TDirectory*> (key0->ReadObj());
    dir0->cd();
    ///
    */
    TIter next_run ( mf.GetListOfKeys() );
    TKey* key_run(0);
    while ((key_run = dynamic_cast<TKey*> ( next_run() )) !=0 ) {
      if (!key_run->IsFolder()) continue;
      TString run_dir = key_run->GetName();
      if (!run_dir.Contains("run"))continue;

      std::string run_dir2 = run_dir.Data();
      //int run_number = atoi( (run_dir2.substr(4, run_dir2.size()-4 )).c_str() );
      //run_number=run_number;

      TString tgc_dir = run_dir + "/Muon/MuonRawDataMonitoring/TGCLV1/";

      TString tgc_sub_dir[2] = {tgc_dir + "TGCEA/", tgc_dir + "TGCEC/"};

      TString sac[2]={"_A","_C"};

      TString ssllpt[2]={"SL_Timing","Low_Pt_Timing"};

      std::stringstream ss;
      //Summary histograms
      TString schambertypesummary[6];//[station][eta]
      TH1F* chambertypesummary[6];//

      std::string type[6]={"_F_T2", "_E5_T5", "_E4_T6", "_E3_T7", "_E2_T8","_E1_T9" };
  
      for(int sllpt=0;sllpt<2;sllpt++){//loop over SL/LPT
        for(int eta=0;eta<6;eta++){
          //get summary histograms
          ss.str("");
          ss<<type[eta];
          schambertypesummary[eta] = tgc_dir + "Global/Summary/" 
            + "Summary_Of_" + ssllpt[sllpt] + "_Per_Chamber_Type" + ss.str();  
          
	  chambertypesummary[eta] = 0;
	  mf.get(schambertypesummary[eta], chambertypesummary[eta]);
	  if(!chambertypesummary[eta]){
	    //std::cerr <<"TGC PostProcessing: no such histogram!! "<< schambertypesummary[eta] << std::endl;
	    continue;
	  }
	  TGCResetContents(chambertypesummary[eta]);
        }//eta

        for(int ac=0;ac<2;ac++){
          //get summary histogram
          TString ssum = tgc_sub_dir[ac] + "Summary/Summary_Of_" + ssllpt[sllpt] + sac[ac];
          TString stmap = tgc_sub_dir[ac] + ssllpt[sllpt] + "_Map" + sac[ac];
          TString stfrac = tgc_sub_dir[ac] + ssllpt[sllpt] + "_Fraction_Map" + sac[ac];

	  TH1F *hsum = 0;
	  mf.get(ssum, hsum);
	  if(!hsum){
	    //std::cerr <<"TGC PostProcessing: no such histogram!! "<< ssum << std::endl;
	    continue;
	  }

	  TH2F *htmap = 0;
	  mf.get(stmap, htmap);
	  if(!htmap){
	    //std::cerr <<"TGC PostProcessing: no such histogram!! "<< stmap << std::endl;
	    continue;
	  }

	  TH2F *htfrac = 0;
	  mf.get(stfrac, htfrac);
	  if(!htfrac){
	    //std::cerr <<"TGC PostProcessing: no such histogram!! "<< stfrac << std::endl;
	    continue;
	  }

          TGCResetContents(hsum);
          TGCResetContents(htfrac);

          int nbinx = htmap->GetXaxis()->GetNbins();
          int nbiny = htmap->GetYaxis()->GetNbins();

          if( nbinx !=18 || nbiny != 48 ) continue;

          for( int eta = 0 ; eta < 6 ; eta++ ){
            for( int phi48 = 0 ; phi48 < 48 ; phi48++ ){
              if( eta==5 && phi48%2 ==1 )continue;//Forward

              float p = htmap->GetBinContent( eta +  1, phi48 +1);
              float c = htmap->GetBinContent( eta +  7, phi48 +1);
              float n = htmap->GetBinContent( eta + 13, phi48 +1);
              float tot=p+n+c;

              float fp = 0;
              float fc = 0;
              float fn = 0;

              float efp = 0;
              float efc = 0;
              float efn = 0;

              if( tot != 0 ){
                fp = p/tot; efp = sqrt( fp * (1. - fp )/tot );
                fc = c/tot; efc = sqrt( fc * (1. - fc )/tot );
                fn = n/tot; efn = sqrt( fn * (1. - fn )/tot );
              }

              htfrac->SetBinContent( eta +  1, phi48 + 1, fp );
              htfrac->SetBinContent( eta +  7, phi48 + 1, fc );
              htfrac->SetBinContent( eta + 13, phi48 + 1, fn );
              htfrac->SetBinError( eta +  1, phi48 + 1, efp );
              htfrac->SetBinError( eta +  7, phi48 + 1, efc );
              htfrac->SetBinError( eta + 13, phi48 + 1, efn );
	      
	      if(chambertypesummary[eta])
		chambertypesummary[eta]->Fill(fc);
              hsum->Fill(fc);

              //low current fraction chambers
              if(fc < TGCChamberTimingCut){
                ss.str("");
                //std::string chamber = htiming->GetXaxis()->GetBinLabel(bin);
                ss<< sac[ac] << "phi" <<phi48+1<<"Eta"<<eta;

                if( tgc_debug ) std::cout<<"TGC "<<ssllpt[sllpt]<<" low current BCID fraction chamber : " << ss.str() << " " << fc << std::endl;

                std::pair<std::string, float> p(ss.str(), fc);
                if(sllpt==0){
                  badtrgtimingchambers.push_back(p);
                }else{
                  badrotimingchambers.push_back(p);
                }
              }

            }//phi48
          }//eta

          //write timing fraction
          TString timing_dir = tgc_sub_dir[ac];
          TDirectory* dir = mf.GetDirectory(timing_dir);

          if(dir){
            dir->cd();
	    htfrac->Write("",TObject::kOverwrite);
          }else{
            //std::cerr<<"TGCChamberTiming : directory "<<timing_dir<<" not found"<<std::endl;
          }

          //write summary of timing for each side
          TString sum_dir = tgc_sub_dir[ac] + "Summary/";
          dir = mf.GetDirectory(sum_dir);

          if(dir){
            dir->cd();
	    hsum->Write("",TObject::kOverwrite);
          }else{
            //std::cerr<<"TGCChamberTiming : directory "<<sum_dir<<" not found"<<std::endl;
          }

        }//ac
	mf.Write();
		
        //write summary of timing for each eta
        TString sum_dir = tgc_dir + "Global/Summary/";
        TDirectory* dir = mf.GetDirectory(sum_dir);
        
        if(dir){
          dir->cd();
	  for(int eta=0;eta<6;eta++){
	    if(chambertypesummary[eta])
	      chambertypesummary[eta]->Write("",TObject::kOverwrite);
	  }
	}
        else{
          //std::cerr<<"TGCChamberTiming : directory "<<sum_dir<<" not found"<<std::endl;
        }
	mf.Write();

      }//sllpt
    }//while
    mf.Write();
  }//MonitoringFile::TGCChamberTiming

  void 
  MonitoringFile::TGCChannelOccupancy(const std::string & inFilename, 
				      std::vector< std::pair< std::string, 
				      float > >& noisychannels)
  {
    PostProcessorFileWrapper mf( inFilename , "TGC ChannelOcc Calculations");
    if (!mf.IsOpen()) {
      //std::cerr << "TGCPostProcess(): "
      //	<< "Input file not opened \n";
      return;
    }
    if(mf.GetSize()<1000.) {
      //std::cerr << "TGCPostProcess(): "
      //	<< "Input file empty \n";
      return; 
    }
    /*
    // get run directory name
    //Seemingly unnecessary lines are necessary
    TIter nextcd0(gDirectory->GetListOfKeys());
    TKey *key0 = (TKey*)nextcd0();
    TDirectory *dir0= dynamic_cast<TDirectory*> (key0->ReadObj());
    dir0->cd();
    ///
    */
    TIter next_run ( mf.GetListOfKeys() );
    TKey* key_run(0);
    while ((key_run = dynamic_cast<TKey*> ( next_run() )) !=0 ) {
      if (!key_run->IsFolder()) continue;
      TString run_dir = key_run->GetName();
      if (!run_dir.Contains("run"))continue;

      std::string run_dir2 = run_dir.Data();
      //int run_number = atoi( (run_dir2.substr(4, run_dir2.size()-4 )).c_str() );
      //run_number=run_number;

      TString tgc_dir = run_dir + "/Muon/MuonRawDataMonitoring/TGC/";

      TString tgc_global_dir = tgc_dir + "Global/";
      TString tgc_sub_dir[2] = {tgc_dir + "TGCEA/", tgc_dir + "TGCEC/"};

      TString sac[2]={"_A","_C"};
      TString sws[2]={"Wire_","Strip_"};
      TString slay[7]={"1","2","3","4","5","6","7"};
      std::stringstream ss;

      //get number of events processed
      TString sentries = tgc_global_dir + "Event_Counter";

      TH1F *hentries = 0;
      mf.get(sentries, hentries);
      if(!hentries){
	//std::cerr <<"TGC PostProcessing: no such histogram!! "<< sentries << std::endl;
	continue;
      }

      double nentries = hentries->GetEntries();

      for(int ac=0;ac<2;ac++){
        for(int ws=0;ws<2;ws++){
          for(int lay=0;lay<7;lay++){
            if( ws==1 && lay==1)continue;
            for(int subsect=1;subsect<=48;subsect++){
              //get each subsector histogram
              int sector, phi4;
              TGCsubsect2sectorphi(subsect, sector, phi4);

              ss.str("");
              if(sector<10) ss<<"0";
              ss<<sector<< "_Layer" << slay[lay] << "_Phi" << phi4;
              TString sprof = tgc_sub_dir[ac] + "Profile/" + sws[ws] + "Hit_Profile" + sac[ac] + ss.str();

	      TH1F *hprof = 0;
	      mf.get(sprof, hprof);
	      if(!hprof){
		//std::cerr <<"TGC PostProcessing: no such histogram!! "<< sprof << std::endl;
		continue;
	      }
	      
              int nbin = hprof->GetXaxis()->GetNbins();

              for(int bin=1; bin<=nbin; bin++){
                double num = hprof->GetBinContent(bin);
                float occu = 0.;
                if(nentries>0)
                  occu = (float)num/nentries;
                if(occu > TGCChannelOccupancyCut){
                  ss.str("");
                  ss<< hprof->GetName() <<" " << bin;
                  if( tgc_debug ) std::cout<<"TGC noisy channel : " << ss.str() << " " << occu << std::endl;

                  std::pair<std::string, float> p(ss.str(),occu);
                  noisychannels.push_back(p);
                }
              }//bin
            }//subsect
          }//lay
        }//loop over ws
      }//loop over ac
    }//while
  }//MonitoringFile::TGCChannelOccupancy

  void 
  MonitoringFile::TGCsubsectbin2stationeta(int subsect, int bin, int& station, int& eta)
  {
    if(subsect%2 == 1 ){//including forward chamber
      if(bin<=15){
        station = 1;
        eta = (bin-1)%5;
      }else if( bin <=27 ){
        station = 2;
        eta = (bin-16)%6;
      }else{
        station = 3;
        eta = (bin-28)%6;
      }
    }else{
      if(bin<=12){
        station = 1;
        eta = (bin-1)%4 + 1;
      }else if( bin <=22 ){
        station = 2;
        eta = (bin-13)%5 + 1;
      }else{
        station = 3;
        eta = (bin-23)%5 + 1;
      }
    }

  }//MonitoringFile::TGCsubsectbin2stationeta

  void 
  MonitoringFile::TGCsubsect2sectorphi(int subsect, int& sector, int& phi4)
  {
    sector = ( subsect+1 ) / 4 + 1;
    if( sector >12 ) sector=1;
    phi4 = (subsect + 1 ) % 4 ;

  }//MonitoringFile::TGCsubsect2sectorphi

  int
  MonitoringFile::nTGCWireStripMap(int ws, int etac, int phi48)//[0:1][0:42][1:48]
  {
    
    int layer=0;
    int eta = etac;
    int st =0;
//    if      ( etac <= 4  ){ layer = 0; eta = 4  - etac; st = 42;}
//    else if ( etac <= 9  ){ layer = 1; eta = 9  - etac; st = 42;}
//    else if ( etac <= 14 ){ layer = 2; eta = 14 - etac; st = 42;}
//    else if ( etac <= 20 ){ layer = 3; eta = 20 - etac; st = 44;}
//    else if ( etac <= 26 ){ layer = 4; eta = 26 - etac; st = 44;}
//    else if ( etac <= 32 ){ layer = 5; eta = 32 - etac; st = 46;}
//    else if ( etac <= 38 ){ layer = 6; eta = 38 - etac; st = 46;}
//    else if ( etac <= 40 ){ layer = 7; eta = 40 - etac; st = 48;}
//    else if ( etac <= 42 ){ layer = 8; eta = 42 - etac; st = 48;}

    int binx = etac+1;
    if( binx%7>=1 && binx%7<=3 && binx<=28) {layer = (binx-1)%7; eta=4-(binx-1)/7; st = 42;} 
    else if( binx%7>=4 && binx%7<=5 && binx<=28) {layer = (binx-1)%7; eta=5-(binx-1)/7; st = 44;} 
    else if( (binx-1)%7>=5 && binx<=28) {layer = (binx-1)%7; eta=5-(binx-1)/7; st = 46;} 
    else if( binx>28 && binx<=30) {layer = binx-28+3-1; eta=1; st = 44; }
    else if( binx>30 && binx<=32) {layer = binx-28+3-1; eta=1; st = 46; }
    else if( binx>32 && binx<=35) {layer = binx-32-1; eta=0; st = 42; }
    else if( binx>35 && binx<=37) {layer = binx-32-1; eta=0; st = 44; }
    else if( binx>37 && binx<=39) {layer = binx-32-1; eta=0; st = 46; }
    else if( binx>39 && binx<=41) {layer = binx-32-1; eta=1; st = 48; }
    else if( binx>41 && binx<=43) {layer = binx-34-1; eta=0; st = 48; }

    
    if( eta == 0 )st-=1;

    
    int nwire = getTGCNumberOfWires( st, layer, eta, phi48 );


    //number of strips is always 32 except no chamber region which should be nwire==0.
    if(ws==1){
      if(nwire==0) return 0;
      return 32;
    }

    return nwire;

  }

  int  
  MonitoringFile::getTGCNumberOfWires( const int istationName, const int layer, const int istationEta, const int istationPhi )
  {
    int nWire  = 0;
    if(istationName==42){ //TGC1 Endcap //41:T1F 42:T1E 43:T2F 44:T2E 45:T3F 46:T3E 47:T3F 48:T4E (T4 means inner small wheel TGCs, EI/FI)

      if(layer==0){
        if(istationEta==1){ // T3 (E4) wire 1ch-91ch (total 91ch) 
          nWire = 92;
        } else  if(istationEta==2){ //T6 (E3) 91ch-152ch (62ch)
          nWire = 61;
        } else if(istationEta==3){ //T7 (E2) 152ch-174ch (23ch)
          nWire = 23;
        } else if(istationEta==4){ //T8 (E1) 173ch-196ch(24ch)
          nWire = 24;
        }
      }else if( layer ==1 || layer ==2){
        if(istationEta==1){ // T3 (E4) wire 1ch-91ch (total 91ch) 
          nWire = 91;
        } else  if(istationEta==2){ //T6 (E3) 91ch-152ch (62ch)
          nWire = 62;
        } else if(istationEta==3){ //T7 (E2) 152ch-174ch (23ch)
          nWire = 23;
        } else if(istationEta==4){ //T8 (E1) 173ch-196ch(24ch)
          nWire = 24;
        }
      }
      
    } else if(istationName==41){ // TGC1 Forward

      if(layer==0 || layer ==2) nWire = 105;
      else if(layer==1) nWire = 104;
      
    }  else if(istationName==44){ // TGC2 Endcap
      if(istationEta==1){ // T4 (E5) wire 1ch-91ch (total 91ch) 
        nWire = 110;
      } else  if(istationEta==2){ //T6 (E4) 109ch-211ch (103ch)
        nWire = 103;
      } else if(istationEta==3){ //T7 (E3) 211ch-242ch (23ch)
        nWire = 32;
      } else if(istationEta==4){ //T8 (E2) 241ch-272ch (32ch)
        nWire = 32;
      } else if(istationEta==5){ //T9 (E1) 271ch-302ch(32ch)
        nWire = 32;
      }
    } else if(istationName==43){ // TGC2 Forward
      nWire = 125;
    }  else if(istationName==46){ // TGC3 Endcap
      if(istationEta==1){ // T5 (E5) wire 1ch-110ch (total 110ch) 
        nWire = 96;
      } else  if(istationEta==2){ //T6 (E4) 95ch-200ch (106ch)
        nWire = 106;
      } else if(istationEta==3){ //T7 (E3) 200ch-231ch (32ch)
        nWire = 32;
      } else if(istationEta==4){ //T8 (E2) 231ch-2
        nWire = 30;
      } else if(istationEta==5){ //T9 (E1) 260ch-290ch(31ch)
        nWire = 31;
      }
    } else if(istationName==45){ // TGC3 Forward
      nWire = 122;
    } else if(istationName==48){ // EI
      //stationPhi 1-8 -> 1-8
      //           9-15 -> 10-16
      //           16-18 -> 18-20
      //           19-21 -> 22-24
      if(istationPhi==2 || istationPhi==11 || istationPhi==13 || istationPhi==14 || istationPhi==15 || istationPhi==16 || istationPhi==20|| istationPhi==21 ){
        //short 16
        nWire=16;
      }else{
        //long  24
        nWire=24;
      }
    } else if(istationName==47){ // FI
      if(istationPhi == 2 || istationPhi == 5 || istationPhi == 8 || istationPhi == 11 || istationPhi == 14 || istationPhi == 17 || istationPhi == 20 || istationPhi == 23 ){
        //short 30
        nWire=30;
      }else{
        //long  32
        nWire=32;
      }
    }
    return nWire;
  }
  
  void
  MonitoringFile::TGCResetContents(TH1* h) {    
  // copy all functions of histogram before resetting
    TList* h_funcs; 
    h_funcs = dynamic_cast<TList*> (h->GetListOfFunctions()->Clone());
    h->Reset(); 
    //now insert these functions back into the hist
    TIter iterE(h_funcs);
    while ( (iterE()) ){h->GetListOfFunctions()->Add(*iterE);}
    delete h_funcs; 
    // are the functions still valid after list deletion?
    //http://root.cern.ch/root/html/TList#TList:_TList  should be!
    //  TIter iterTest(EffBCap->GetListOfFunctions());
    //  while( iterTest() ) std::cout << (*iterTest)->GetName() << std::endl;    
  }

}//namespace

