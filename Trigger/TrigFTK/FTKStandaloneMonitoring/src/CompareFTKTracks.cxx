/*
Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#include "FTKStandaloneMonitoring/CompareFTKTracks.h"
#include <iostream>
#include <string>
CompareFTKTracks::CompareFTKTracks(std::vector<const FTK_RawTrack *> &ref, std::vector<const FTK_RawTrack *> &test, std::vector<TH1D *> &vec_histo, std::map<std::string , TH1D * > &map_histo, std::map<std::string , TH2D * > &map_histo_2D){
      m_ref=ref;
      m_test=test;
      m_vec_histo=vec_histo;
      m_map_histo=map_histo;
      m_map_histo_2D=map_histo_2D;
}
void CompareFTKTracks::AssociateTracks(){
      m_associator= new FTKTrkAssoc("FTKTrkAssoc", m_dmax);
      m_associator->match( m_ref,m_test);
      std::cout<<"Of the N ref tracks ("<<m_ref.size()<<") wrt to the test tracks ("<<m_test.size()<<"), the number of matched tracks is "<<m_associator->size()<<std::endl;
}

void CompareFTKTracks::TestEquivalence(){
      const FTK_RawTrack* match_track = NULL;
      int itrk=0;
      for(auto& ref_track : m_ref){
         itrk+=1;
         match_track = m_associator->matched(ref_track);
	 if (!match_track) {std::cout<<"the "<<itrk<<"-th track has no matched track"<<std::endl; continue;}
	 if (ref_track->getPhi()!=0&&abs(match_track->getPhi()/ref_track->getPhi()-1.)>0.001){	    
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
     else std::cout<<"variable not found!!"<<variable<<std::endl;
     return outvar;
}
void CompareFTKTracks::FillHistos(){
      const FTK_RawTrack* match_track = NULL;
      int itrk=0;
      int HWSWmatched=0;
      int HWSWdifferent=0;
      int HWonly=0;
      for(auto& ref_track : m_ref){
         itrk+=1;
         match_track = m_associator->matched(ref_track);
	 //no SW matched track
	 if (!match_track) {
	     std::cout<<"the "<<itrk<<"-th track has no matched track"<<std::endl; 
             for (auto& imap : m_map_histo){
	         if (imap.first.find("HWonly")==std::string::npos) continue;
		 std::size_t pos = imap.first.find("_");      
        	 std::string istr = imap.first.substr (pos+1);		 
                 if (istr.find("ETA_PHI")==std::string::npos){
		     imap.second->Fill(GetValue(istr,ref_track));
		 }
		 else {		     
		     std::string etastr("eta");
		     std::string phistr("phi");
		     imap.second->Fill(GetValue(etastr,ref_track),GetValue(phistr,ref_track));
		 }    
             }
	     HWonly+=1;
	     continue;
	 }
	 //completely matched tracks    
	 else if (!(
	      (ref_track->getPhi()!=0&&abs(match_track->getPhi()/ref_track->getPhi()-1.)>0.001)&&
	      (ref_track->getInvPt()!=0&&abs(match_track->getInvPt()/ref_track->getInvPt()-1.)>0.001)&&
	      (ref_track->getD0()!=0&&abs(match_track->getD0()/ref_track->getD0()-1.)>0.001)&&
	      (ref_track->getZ0()!=0&&abs(match_track->getZ0()/ref_track->getZ0()-1.)>0.001)&&
	      (ref_track->getChi2()!=0&&abs(match_track->getChi2()/ref_track->getChi2()-1.)>0.001)&&
	      (ref_track->getCotTh()!=0&&abs(match_track->getCotTh()/ref_track->getCotTh()-1.)>0.001)
	      )
	     ){
             for (auto& imap : m_map_histo){
	         if (imap.first.find("HWSW")==std::string::npos) continue;
		 std::size_t pos = imap.first.find("_");      
        	 std::string istr = imap.first.substr (pos+1);		 
                 if (istr.find("ETA_PHI")==std::string::npos){
		     imap.second->Fill(GetValue(istr,match_track));
		 }
		 else {		     
		     std::string etastr("eta");
		     std::string phistr("phi");
		     imap.second->Fill(GetValue(etastr,ref_track),GetValue(phistr,ref_track));
		 }    
             }
	     HWSWmatched+=1;
	 }
	 //not matched tracks
	 else{    	     
	     for (auto& imap : m_map_histo){
		std::size_t pos = imap.first.find("_");      
        	std::string istr = imap.first.substr (pos+1);
		double refval,testval;
		refval=GetValue(istr,ref_track);
		testval=GetValue(istr,match_track);
		if (imap.first.find("res")!=std::string::npos) {
                    imap.second->Fill( (testval-refval)/(refval));
        	}

	     }
	     HWSWdifferent+=1;
	 }   
         m_vec_histo.at(0)->Fill( (1./match_track->getInvPt()-1./ref_track->getInvPt())/(1./ref_track->getInvPt()));
      
         m_vec_histo.at(1)->Fill( (match_track->getD0()-ref_track->getD0())/(ref_track->getD0()));
      }
      m_map_histo["nTrk_only_hw"]->Fill(HWonly); 
      m_map_histo["nTrk_different_hw_sw"]->Fill(HWSWdifferent); 
      m_map_histo["nTrk_same_hw_sw"]->Fill(HWSWmatched); 
}

