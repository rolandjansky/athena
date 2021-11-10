/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////////////////////////
//Methods to Divide TGC Histograms to calculate efficiency and occupancy after merging.
//Author: Akimasa Ishikawa (akimasa.ishikawa@cern.ch)
//Date:   Aug 2009
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "DataQualityUtils/MonitoringFile.h"

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
#include "TF1.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TBranch.h"
#include "TGraphAsymmErrors.h"

namespace dqutils {
  
  void
  MonitoringFile::TGCRawHistogramDivision(const std::string & inFilename)
  {
    PostProcessorFileWrapper mf( inFilename , "TGC Raw Histogram Division");
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

    std::stringstream ss;
    TString sac[2]={"_A","_C"};
    TString sws[2]={"Wire_","Strip_"};
    
    TIter next_run ( mf.GetListOfKeys() );
    TKey* key_run(0);
    
    while((key_run=dynamic_cast<TKey*> (next_run()))!=0){
      if (!key_run->IsFolder()) continue;
      TString run_dir = key_run->GetName();
      if (!run_dir.Contains("run"))continue;
      
      std::string run_dir2 = run_dir.Data();
      //int run_number = atoi( (run_dir2.substr(4, run_dir2.size()-4 )).c_str() );
      
      TString seff;
      TString snum;
      TString sden;
      
      TH1F *h1eff(0);
      TH1F *h1num(0);
      TH1F *h1den(0);
      TH2F *h2eff(0);
      TH2F *h2num(0);
      TH2F *h2den(0);
      
      //===TGCRaw
      TString tgc_dir = run_dir + "/Muon/MuonRawDataMonitoring/TGC/";
      
      TString tgc_global_dir = tgc_dir + "Global/";
      TString tgc_sub_dir[2] = {tgc_dir + "TGCEA/", tgc_dir + "TGCEC/"};
      
      for(int ac=0;ac<2;ac++){
        TString eff_dir = tgc_sub_dir[ac] + "Efficiency/";
        TDirectory* dir = mf.GetDirectory(eff_dir);
        if(!dir){
          //std::cerr<< "TGCHistogramDivision: directory "<<eff_dir<<" not found"<<std::endl;
          return;
        }
	
        ss.str("");
        ss << "Efficiency" << sac[ac];
        seff   = eff_dir + ss.str();
        ss.str("");
        ss << "Efficiency" << sac[ac] << "_Numerator";
        snum   = eff_dir + "NumDenom/" + ss.str();
        ss.str("");
        ss << "Efficiency" << sac[ac] << "_Denominator";
        sden = eff_dir + "NumDenom/" + ss.str();
        
        h1eff = 0;
        mf.get(seff, h1eff);
        h1num = 0;
        mf.get(snum, h1num);
        h1den = 0;
        mf.get(sden, h1den);

        if( h1eff && h1num && h1den){
          TGCResetContents(h1eff);
          h1eff->Divide(h1num, h1den, 1., 1., "B");
          
          // save the summary histogram
          dir->cd();
          h1eff->Write("",TObject::kOverwrite);
        }
	else{
	  //std::cerr <<"TGC PostProcessing: no such histogram (or num/denom)!! "<< seff << std::endl;
	  continue;
	}
	
        for(int ws=0;ws<2;ws++) {
          ss.str("");
          ss << sws[ws] << "Efficiency" << sac[ac];
          seff   = eff_dir + ss.str();
          ss.str("");
          ss << sws[ws] << "Efficiency" << sac[ac] << "_Numerator";
          snum   = eff_dir + "NumDenom/" + ss.str();
          ss.str("");
          ss << sws[ws] << "Efficiency" << sac[ac] << "_Denominator";
          sden = eff_dir + "NumDenom/" + ss.str();
          
	  h1eff = 0;
	  mf.get(seff, h1eff);
	  h1num = 0;
	  mf.get(snum, h1num);
	  h1den = 0;
	  mf.get(sden, h1den);
          
          if(h1eff && h1num && h1den){
            TGCResetContents(h1eff);
            h1eff->Divide(h1num, h1den, 1., 1., "B");
	    
            // save the summary histogram
            dir->cd();
	    h1eff->Write("",TObject::kOverwrite);
          }
	  else{
	    //std::cerr <<"TGC PostProcessing: no such histogram (or num/denom)!! "<< seff << std::endl;
	    continue;
	  }
	  mf.Write();
	  
          //efficiency map for current, previous and next.
          std::string sbc[3]={"_Previous", "", "_Next" };
          for(int bc=0;bc<3;bc++){
            ss.str("");
            ss << sws[ws] << "Efficiency_Map" << sbc[bc] << sac[ac];
            seff   = eff_dir + ss.str();
            ss.str("");
            ss << sws[ws] << "Efficiency_Map" << sbc[bc] << sac[ac] << "_Numerator";
            snum   = eff_dir + "NumDenom/" + ss.str();
            ss.str("");
            ss << sws[ws] << "Efficiency_Map" << sac[ac] << "_Denominator";
            sden = eff_dir + "NumDenom/" + ss.str();
	    
	    h2eff = 0;
	    mf.get(seff, h2eff);
	    h2num = 0;
	    mf.get(snum, h2num);
	    h2den = 0;
	    mf.get(sden, h2den);
	    
            if(h2eff && h2num && h2den){
              TGCResetContents(h2eff);
              h2eff->Divide(h2num, h2den, 1., 1., "B");

              // save the summary histogram
	      dir->cd();
	      h2eff->Write("",TObject::kOverwrite);
            }
	    else{
	      //std::cerr <<"TGC PostProcessing: no such histogram (or num/denom)!! "<< seff << std::endl;
	      continue;
	    }

          }//bc
        }//ws
      }//ac
      //===End TGCRaw
    }// run_dir loop

    mf.Write();
  }
  
  void
  MonitoringFile::TGCLV1HistogramDivision(const std::string & inFilename)
  {
    PostProcessorFileWrapper mf( inFilename , "TGC LV1 Histogram Division");
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
    std::stringstream ss;
    TString sac[2]={"_A","_C"};
    //TString sws[2]={"Wire_","Strip_"};
    
    TIter next_run ( mf.GetListOfKeys() );
    TKey* key_run(0);
    while((key_run=dynamic_cast<TKey*> (next_run()))!=0){
      if (!key_run->IsFolder()) continue;
      TString run_dir = key_run->GetName();
      if (!run_dir.Contains("run"))continue;
      
      std::string run_dir2 = run_dir.Data();
      //int run_number = atoi( (run_dir2.substr(4, run_dir2.size()-4 )).c_str() );
      
      TString seff;
      TString snum;
      TString sden;
      TString seffg;
      
      TH1F *h1eff(0);
      TH1F *h1num(0);
      TH1F *h1den(0);
      TH2F *h2eff(0);
      TH2F *h2num(0);
      TH2F *h2den(0);
      TGraphAsymmErrors *geff(0);
      
      //===TGCLV1
      TString tgc_dir = run_dir + "/Muon/MuonRawDataMonitoring/TGCLV1/";

      TString tgc_global_dir = tgc_dir + "Global/";
      TString tgc_sub_dir[2] = {tgc_dir + "TGCEA/", tgc_dir + "TGCEC/"};

      std::string smuid[2] = {"_Muid","_Staco"};
      std::string spna[3] = {"_Posi","_Nega", ""};

      std::string sbc[3]={"_Previous", "", "_Next" };

      for(int ac=0;ac<2;ac++){
        TString eff_dir = tgc_sub_dir[ac] + "Eff/";
        TDirectory* dir = mf.GetDirectory(eff_dir);
        if(!dir){
          //std::cerr<< "TGCHistogramDivision: directory "<<eff_dir<<" not found"<<std::endl;
          return;
        }
	
        for(int pt=1;pt<=6;pt++){
          for(int pna=0;pna<3;pna++){
            for(int muid=0;muid<2;muid++){
              for(int pcn=0;pcn<3;pcn++){
		std::string tempeff, tempnum, tempden;
                //Efficiency
                ss.str("");
                ss << "Trigger_Efficiency" << spna[pna] << sbc[pcn] << "_PT" << pt << smuid[muid] << sac[ac];
                seff   = eff_dir + ss.str();
		tempeff = ss.str();
                ss.str("");
                ss << "Trigger_Efficiency" << spna[pna] << sbc[pcn] << "_PT" << pt << smuid[muid] << sac[ac] << "_Numerator";
                snum   = eff_dir + "NumDenom/" + ss.str();
		tempnum = ss.str();
                ss.str("");
                ss << "Trigger_Efficiency" << spna[pna] << "_PT" << pt << smuid[muid] << sac[ac] << "_Denominator";
                sden = eff_dir + "NumDenom/" + ss.str();
		tempden = ss.str();
		
		h2eff = 0;
		mf.get(seff, h2eff);
		h2num = 0;
		mf.get(snum, h2num);
		h2den = 0;
		mf.get(sden, h2den);
                if(h2eff && h2num && h2den){
                  TGCResetContents(h2eff);
                  h2eff->Divide(h2num, h2den, 1., 1., "B");
                  
                  // save the summary histogram
                  dir = mf.GetDirectory(eff_dir);
                  dir->cd();
		  h2eff->Write("",TObject::kOverwrite);
                }
		else{
		  //std::cerr <<"TGC PostProcessing: no such histogram (or num/denom)!! "<< seff << std::endl;
		  continue;
		}
		mf.Write();
              }//pcn
	      
              //Turn on
              ss.str("");
              ss << "Trigger_Turn_On_Curve" << spna[pna] << "_PT" << pt << smuid[muid] << sac[ac];
              seff   = eff_dir + ss.str();
              ss << "_Fit";
              seffg   = eff_dir + ss.str();
              ss.str("");
              ss << "Trigger_Turn_On_Curve" << spna[pna] << "_PT" << pt << smuid[muid] << sac[ac] << "_Numerator";
              snum   = eff_dir + "NumDenom/" + ss.str();
              ss.str("");
              ss << "Trigger_Turn_On_Curve" << spna[pna] << "_PT" << pt << smuid[muid] << sac[ac] << "_Denominator";
              sden = eff_dir + "NumDenom/" + ss.str();
 
	      h1eff = 0;
	      mf.get(seff, h1eff);
	      h1num = 0;
	      mf.get(snum, h1num);
	      h1den = 0;
	      mf.get(sden, h1den);
	      geff = 0;
	      mf.get(seffg, geff);

              if(h1eff && h1num && h1den){
                TGCResetContents(h1eff);
                h1eff->Divide(h1num, h1den, 1., 1., "B");

                // save the summary histogram
                dir = mf.GetDirectory(eff_dir);
                dir->cd();
		h1eff->Write("",TObject::kOverwrite);

                if(geff){
                  geff->BayesDivide(h1num, h1den);

                  // save the summary histogram
                  dir = mf.GetDirectory(eff_dir);
                  dir->cd();
		  geff->Write("",TObject::kOverwrite);
                }
              }
	      else{
		//std::cerr <<"TGC PostProcessing: no such histogram (or num/denom)!! "<< seff << std::endl;
		continue;
	      }
	      mf.Write();
            }//muid
          }//pna
	  mf.Write();
	  
	  //Rate Ratio
	  TString grate_dir = tgc_global_dir + "Rate/";
	  TString rate_dir = tgc_sub_dir[ac] + "Rate/";
	  TString rr_dir = tgc_sub_dir[ac] + "Rate/Ratio/";
	  dir = mf.GetDirectory(rr_dir);
	  if(!dir){
	    //std::cerr<< "TGCHistogramDivision: directory "<<rr_dir<<" not found"<<std::endl;
	    return;
	  }
	  
	  // trigger/10BCID
	  ss.str("");
	  ss << "Number_Of_PT" << pt << "_Triggers_Per_Event_Vs_10BCID" << sac[ac];
	  seff   = rr_dir + ss.str();
	  ss.str("");
	  ss << "Number_Of_PT" << pt << "_Triggers_In_10BCID" << sac[ac];
	  snum   = rate_dir + ss.str();
	  ss.str("");
	  ss << "Number_Of_Events_In_10BCID";
	  sden = grate_dir + ss.str();
	  
	  h1eff = 0;
	  mf.get(seff, h1eff);
	  h1num = 0;
	  mf.get(snum, h1num);
	  h1den = 0;
	  mf.get(sden, h1den);
	  
	  if( h1eff && h1num && h1den){
	    TGCResetContents(h1eff);
	    h1eff->Divide(h1num, h1den);
	    
	    // save the summary histogram
	    dir->cd();
	    h1eff->Write("",TObject::kOverwrite);
	  }
	  else{
	    //std::cerr <<"TGC PostProcessing: no such histogram (or num/denom)!! "<< seff << std::endl;
	    continue;
	  }
	  
          //trigger/2LB
          ss.str("");
          ss << "Number_Of_PT" << pt << "_Triggers_Per_Event_Vs_2LB" << sac[ac];
          seff   = rr_dir + ss.str();
          ss.str("");
          ss << "Number_Of_PT" << pt << "_Triggers_In_2LB" << sac[ac];
          snum   = rate_dir + ss.str();
          ss.str("");
          ss << "Number_Of_Events_In_2LB";
          sden = grate_dir + ss.str();
          
	  h1eff = 0;
	  mf.get(seff, h1eff);
	  h1num = 0;
	  mf.get(snum, h1num);
	  h1den = 0;
	  mf.get(sden, h1den);

          if( h1eff && h1num && h1den){
            TGCResetContents(h1eff);
            h1eff->Divide(h1num, h1den);
          
            // save the summary histogram
            dir->cd();
	    h1eff->Write("",TObject::kOverwrite);
          }
	  else{
	    //std::cerr <<"TGC PostProcessing: no such histogram (or num/denom)!! "<< seff << std::endl;
	    continue;
	  }
	  mf.Write();

        }//pt
	
        //Rate Ratio
        TString grate_dir = tgc_global_dir + "Rate/";
        TString rate_dir = tgc_sub_dir[ac] + "Rate/";
        TString rr_dir = tgc_sub_dir[ac] + "Rate/Ratio/";

        // trigger/10BCID
        ss.str("");
        ss << "Number_Of_SL_Triggers_Per_Event_Vs_10BCID" << sac[ac];
        seff   = rr_dir + ss.str();
        ss.str("");
        ss << "Number_Of_SL_Triggers_In_10BCID" << sac[ac];
        snum   = rate_dir + ss.str();
        ss.str("");
        ss << "Number_Of_Events_In_10BCID";
        sden = grate_dir + ss.str();
	
	h1eff = 0;
	mf.get(seff, h1eff);
	h1num = 0;
	mf.get(snum, h1num);
	h1den = 0;
	mf.get(sden, h1den);

        if(h1eff && h1num && h1den){
          TGCResetContents(h1eff);
          h1eff->Divide(h1num, h1den);
          
          // save the summary histogram
          dir = mf.GetDirectory(rr_dir);
          dir->cd();
	  h1eff->Write("",TObject::kOverwrite);
        }
	else{
	  //std::cerr <<"TGC PostProcessing: no such histogram (or num/denom)!! "<< seff << std::endl;
	  continue;
	}
	
        //trigger/2LB
        ss.str("");
        ss << "Number_Of_SL_Triggers_Per_Event_Vs_2LB" << sac[ac];
        seff   = rr_dir + ss.str();
        ss.str("");
        ss << "Number_Of_SL_Triggers_In_2LB" << sac[ac];
        snum   = rate_dir + ss.str();
        ss.str("");
        ss << "Number_Of_Events_In_2LB";
        sden = grate_dir + ss.str();
        
	h1eff = 0;
	mf.get(seff, h1eff);
	h1num = 0;
	mf.get(snum, h1num);
	h1den = 0;
	mf.get(sden, h1den);          
          
        if(h1eff && h1num && h1den){
          TGCResetContents(h1eff);
          h1eff->Divide(h1num, h1den);
          
          // save the summary histogram
          dir->cd();
	  h1eff->Write("",TObject::kOverwrite);
        }
	else{
	  //std::cerr <<"TGC PostProcessing: no such histogram (or num/denom)!! "<< seff << std::endl;
	  continue;
	}
	mf.Write();
      }//ac
      //====End TGCLV1
    }
    
    mf.Write();
  }
}//namespace
