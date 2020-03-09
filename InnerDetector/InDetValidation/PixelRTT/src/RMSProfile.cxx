/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RMSProfile_cxx
#define RMSProfile_cxx


#include <string>
#include <cmath>
#include <stdexcept>

#include "PixelRTT/RMSProfile.h"


RMSProfile::RMSProfile():TProfile(){
	m_rms_graph = nullptr;
}


RMSProfile::RMSProfile(const TProfile &profile):TProfile(profile){
	m_rms_graph = nullptr;
}


RMSProfile::RMSProfile(const char *name,const char *title,
	Int_t nbinsx,Double_t xlow,Double_t xup, Option_t *option)
   		:TProfile(name,title,nbinsx,xlow,xup,option){
	m_rms_graph = nullptr;
}


RMSProfile::RMSProfile(const char *name,const char *title,
	Int_t nbinsx,Double_t xlow,Double_t xup,
	Double_t ylow,Double_t yup,Option_t *option)
   		:TProfile(name,title,nbinsx,xlow,xup,ylow,yup,option){
	m_rms_graph = nullptr;
}
	

RMSProfile::RMSProfile(const char *name,const char *title,
	Int_t nbinsx,const Float_t *xbins, Option_t *option)
   		:TProfile(name,title,nbinsx,xbins,option){
	m_rms_graph = nullptr;		
}


RMSProfile::RMSProfile(const char *name,const char *title,
	Int_t nbinsx,const Double_t  *xbins, Option_t *option)
		:TProfile(name,title,nbinsx,xbins,option){
	m_rms_graph = nullptr;
}

RMSProfile::RMSProfile(const char *name,const char *title,
	Int_t nbinsx,const Double_t *xbins,
	Double_t ylow,Double_t yup, Option_t *option)
   		:TProfile(name,title,nbinsx,xbins,ylow,yup,option){
	m_rms_graph = nullptr;
}
	

TProfile * 
RMSProfile::newRMSProfile() const{
  auto newProfile = (TProfile *)(TProfile::Clone());
  Int_t nbinsx = GetNbinsX();
	for(Int_t j=0;j<nbinsx+1;j++){
		Double_t entries = GetBinEntries(j);
		Double_t RMS_val=0, RMS_err = 0, err_recal =0,rms_recal=0; 
		if(entries > 1){
			RMS_val = GetBinError(j) * std::sqrt(entries);
			RMS_err = RMS_val* M_SQRT1_2 / std::sqrt(entries-1);
			rms_recal= RMS_val * entries;
			err_recal = std::sqrt(
				std::abs(RMS_err*RMS_err*entries*entries 
					- RMS_val*RMS_val*entries));
		}
			
		newProfile->SetBinContent(j+1,rms_recal);
		newProfile->SetBinEntries(j+1,entries);
		newProfile->SetBinError(j+1,err_recal);
	}
	std::string rmsname = std::string(GetName()) + std::string("RMS");
	std::string rmstitle = std::string("RMS ") + std::string(GetTitle());
	newProfile->SetTitle(rmstitle.c_str());
	newProfile->SetName(rmsname.c_str());
	return newProfile;
}

TProfile *RMSProfile::GetTheRMSProfile(){		
	if (not m_rms_graph) m_rms_graph = newRMSProfile();
  return m_rms_graph;
}


int RMSProfile::Write(const char* name, Int_t option, Int_t bufsize) const {
	int value = ((TProfile*)(TProfile::Clone()))->Write(name,option,bufsize);
	if (m_rms_graph){
	  value += m_rms_graph->Write(name,option,bufsize);
	} else {
	  throw (std::runtime_error("rms profile pointer undefined in RMSProfile::Write (const)"));
	}
	return value;
}

int RMSProfile::Write(const char* name, Int_t option, Int_t bufsize) {
	int value = ((TProfile*)(TProfile::Clone()))->Write(name,option,bufsize);
	value += GetTheRMSProfile()->Write(name,option,bufsize);
	return value;
}
		

void RMSProfile::DrawRMS(Option_t* option){
	GetTheRMSProfile()->Draw(option);
}


const char *RMSProfile::ClassName() const{
	return "RMSProfile";
}

TObject *RMSProfile::Clone(const char* name) const{
	return new RMSProfile(*((TProfile *)TProfile::Clone(name)));
}


#endif
