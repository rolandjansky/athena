/*
Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#include "FTKStandaloneMonitoring/CompareFTKTracks.h"
#include <iostream>
#include <string>

CompareFTKTracks::CompareFTKTracks(std::vector<const FTK_RawTrack *> &ref, std::vector<const FTK_RawTrack *> &test, std::map<std::string , TH1D * > &map_histo, std::map<std::string , TH2D * > &map_histo_2D){
      m_ref=ref;
      m_test=test;
      m_map_histo=map_histo;
      m_map_histo_2D=map_histo_2D;
}

void CompareFTKTracks::AssociateTracks(){
      m_associator= new FTKTrkAssoc("FTKTrkAssoc", m_dmax);
      m_associator->match( m_ref,m_test);
      std::cout<<"Of N ref trks ("<<m_ref.size()<<") wrt to test trk ("<<m_test.size()<<"), number of matched trks: "<<m_associator->size()<<std::endl;
}

void CompareFTKTracks::TestEquivalence(){
      const FTK_RawTrack* match_track = NULL;
      int itrk=0;
      for(auto& ref_track : m_ref){
         itrk+=1;
         match_track = m_associator->matched(ref_track);
	 if (!match_track) {std::cout<<"the "<<itrk<<"-th track has no matched track"<<std::endl; continue;}
	 if (ref_track->getPhi()!=0&&fabs(match_track->getPhi()/ref_track->getPhi()-1.)>0.001){	    
      	    std::cout<<"Wrong matching: Phi matched "<<match_track->getPhi()<<" ref "<<ref_track->getPhi()<<std::endl;
	    m_allmatched=false;
	 }
      }
      if (m_allmatched) std::cout<<"CompareFTKTracks: All matched tracks had the same phi"<<std::endl;
      else std::cout<<"CompareFTKTracks: Some matched tracks had different phi"<<std::endl;
}

double CompareFTKTracks::GetValue(std::string & variable, const FTK_RawTrack  *tmptrk){
     double outvar=0.;
     if (variable.find("pt")!=std::string::npos)        {outvar= 1./tmptrk->getInvPt();}
     else if (variable.find("d0")!=std::string::npos)   {outvar= tmptrk->getD0();}
     else if (variable.find("z0")!=std::string::npos)   {outvar= tmptrk->getZ0();}
     else if (variable.find("chi2")!=std::string::npos) {outvar= tmptrk->getChi2();}
     else if (variable.find("phi")!=std::string::npos)  {outvar= tmptrk->getPhi();}
     else if (variable.find("eta")!=std::string::npos)  {outvar= TMath::ASinH(tmptrk->getCotTh());}
     else std::cout<<"variable not found!! "<<variable<<std::endl;
     return outvar;
}

void CompareFTKTracks::FillHistos(){
      const FTK_RawTrack* match_track = NULL;
      int itrk=0;
      int HWSWmatched=0;
      int HWSWdifferent=0;
      int HWonly=0;
      int SWonly=0;
      // filling the SWonly histograms by looping over the test tracks
      for(auto& test_track : m_test){
          match_track = m_associator->revmatched(test_track);
	  if (!match_track) {
	     std::cout<<"the "<<itrk<<"-th SW track has no matched track"<<std::endl; 
             for (auto& imap : m_map_histo){
	         if (imap.first.find("SWonly")==std::string::npos) continue;
		 std::size_t pos = imap.first.find("_");      
        	 std::string istr = imap.first.substr (pos+1);		 
                 imap.second->Fill(GetValue(istr,test_track));
	     } 
             for (auto& imap : m_map_histo_2D){
		 std::size_t pos = imap.first.find("_");      
        	 std::string istr = imap.first.substr (pos+1);		 
	         if (imap.first.find("SWonly")==std::string::npos) continue;
		 if (istr.find("ETA_PHI")!=std::string::npos) {		     
		     std::string etastr("eta");
		     std::string phistr("phi");
		     imap.second->Fill(GetValue(etastr,test_track),GetValue(phistr,test_track));
		 }    
             }
	     SWonly+=1;	      
	  }    
      }  
      
      // loop over the reference tracks
      for(auto& ref_track : m_ref){
         itrk+=1;
         match_track = m_associator->matched(ref_track);
	 //no SW matched track -> HWonly histograms
	 if (!match_track) {
	     std::cout<<"the "<<itrk<<"-th HW track has no matched track"<<std::endl; 
             for (auto& imap : m_map_histo){
	         if (imap.first.find("HWonly")==std::string::npos) continue;
		 std::size_t pos = imap.first.find("_");      
        	 std::string istr = imap.first.substr (pos+1);		 
                 imap.second->Fill(GetValue(istr,ref_track));
	     } 
             for (auto& imap : m_map_histo_2D){
		 std::size_t pos = imap.first.find("_");      
        	 std::string istr = imap.first.substr (pos+1);		 
	         if (imap.first.find("HWonly")==std::string::npos) continue;
		 if (istr.find("ETA_PHI")!=std::string::npos) {		     
		     std::string etastr("eta");
		     std::string phistr("phi");
		     imap.second->Fill(GetValue(etastr,ref_track),GetValue(phistr,ref_track));
		 }    
             }
	     HWonly+=1;
	 }
	 //completely matched tracks : HWSW histograms   
	 else if (!(
	      (ref_track->getPhi()!=0&&fabs(match_track->getPhi()/ref_track->getPhi()-1.)>0.001)||
	      (ref_track->getInvPt()!=0&&fabs(match_track->getInvPt()/ref_track->getInvPt()-1.)>0.001)||
	      (ref_track->getD0()!=0&&fabs(match_track->getD0()/ref_track->getD0()-1.)>0.001)||
	      (ref_track->getZ0()!=0&&fabs(match_track->getZ0()/ref_track->getZ0()-1.)>0.001)||
	      (ref_track->getChi2()!=0&&fabs(match_track->getChi2()/ref_track->getChi2()-1.)>0.001)||
	      (ref_track->getCotTh()!=0&&fabs(match_track->getCotTh()/ref_track->getCotTh()-1.)>0.001)
	      )
	     ){	     
             for (auto& imap : m_map_histo){
	         if (imap.first.find("HWSW")==std::string::npos) continue;
		 std::size_t pos = imap.first.find("_");      
        	 std::string istr = imap.first.substr (pos+1);		 
                 imap.second->Fill(GetValue(istr,match_track));
	     } 
             for (auto& imap : m_map_histo_2D){
		std::size_t pos = imap.first.find("_");      
        	std::string istr = imap.first.substr (pos+1);
	         if (imap.first.find("HWSW")==std::string::npos) continue;
		 if (istr.find("ETA_PHI")!=std::string::npos) {		     
		     std::string etastr("eta");
		     std::string phistr("phi");
		     imap.second->Fill(GetValue(etastr,match_track),GetValue(phistr,match_track));
		 }    
             }
             
	     HWSWmatched+=1;
	 }
	 //not matched tracks -> HWvsSW histograms
	 else{
	     for (auto& imap : m_map_histo){
	        if (imap.first.find("HWvsSW")==std::string::npos && imap.first.find("res")==std::string::npos) continue;
		std::size_t pos = imap.first.find("_");      
        	std::string istr = imap.first.substr (pos+1);
		double refval,testval;
		refval=GetValue(istr,ref_track);
		testval=GetValue(istr,match_track);
		if (imap.first.find("res")!=std::string::npos) {
                    imap.second->Fill( (testval-refval)/(refval));
        	}
		else if (imap.first.find("HWvsSWdiff")!=std::string::npos) {
                    imap.second->Fill( (testval-refval));
        	}
		else if (imap.first.find("HWvsSWsw")!=std::string::npos) {
                    imap.second->Fill( testval);
        	}
		else if (imap.first.find("HWvsSWhw")!=std::string::npos) {
                    imap.second->Fill( refval);
        	}
             }
	     auto search_h2HW = m_map_histo_2D.find("HWvsSWhw_ETA_PHI");
	     std::string etastr("eta");
	     std::string phistr("phi");
	     if(search_h2HW != m_map_histo_2D.end()) search_h2HW->second->Fill(GetValue(etastr,ref_track),GetValue(phistr,ref_track));
	     else  std::cout << "Not found "<<search_h2HW->first<<std::endl;
	     auto search_h2SW = m_map_histo_2D.find("HWvsSWsw_ETA_PHI");
	     if(search_h2SW != m_map_histo_2D.end()) search_h2SW->second->Fill(GetValue(etastr,match_track),GetValue(phistr,match_track));
	     else  std::cout << "Not found "<<search_h2SW->first<<std::endl;
	     HWSWdifferent+=1;
	 }   
      }
      //histogram cointaining the number of SW tracks not matched to any HW track per event
      m_map_histo["nTrk_only_sw"]->Fill(SWonly); 
      //histogram cointaining the number of HW tracks not matched to any SW track per event
      m_map_histo["nTrk_only_hw"]->Fill(HWonly); 
      //histogram cointaining the number of HW tracks not completely matched 
      // to SW tracks per event (some differences in the track paramters)
      m_map_histo["nTrk_different_hw_sw"]->Fill(HWSWdifferent); 
      //histogram cointaining the number of HW tracks completely matched to SW tracks per event
      m_map_histo["nTrk_same_hw_sw"]->Fill(HWSWmatched); 
}
