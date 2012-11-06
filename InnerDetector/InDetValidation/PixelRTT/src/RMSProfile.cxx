/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RMSProfile_cxx
#define RMSProfile_cxx

#include <iostream>
#include <string>
#include <cmath>
#include "TMath.h"
#include "PixelRTT/RMSProfile.h"


RMSProfile::RMSProfile():TProfile(){
	rms_graph = NULL;
}


RMSProfile::RMSProfile(const TProfile &profile):TProfile(profile){
	rms_graph = NULL;
}


RMSProfile::RMSProfile(const char *name,const char *title,
	Int_t nbinsx,Double_t xlow,Double_t xup, Option_t *option)
   		:TProfile(name,title,nbinsx,xlow,xup,option){
	rms_graph = NULL;
}


RMSProfile::RMSProfile(const char *name,const char *title,
	Int_t nbinsx,Double_t xlow,Double_t xup,
	Double_t ylow,Double_t yup,Option_t *option)
   		:TProfile(name,title,nbinsx,xlow,xup,ylow,yup,option){
	rms_graph = NULL;
}
	

RMSProfile::RMSProfile(const char *name,const char *title,
	Int_t nbinsx,const Float_t *xbins, Option_t *option)
   		:TProfile(name,title,nbinsx,xbins,option){
	rms_graph = NULL;		
}


RMSProfile::RMSProfile(const char *name,const char *title,
	Int_t nbinsx,const Double_t  *xbins, Option_t *option)
		:TProfile(name,title,nbinsx,xbins,option){
	rms_graph = NULL;
}

RMSProfile::RMSProfile(const char *name,const char *title,
	Int_t nbinsx,const Double_t *xbins,
	Double_t ylow,Double_t yup, Option_t *option)
   		:TProfile(name,title,nbinsx,xbins,ylow,yup,option){
	rms_graph = NULL;
}
	
/*
RMSProfile::RMSProfile(const char *name,TDirectory * my_dir){
	RMSProfile *swap = (RMSProfile *)my_dir->Get(name);
	((RMSProfile&)swap).Copy(*this); //
	//swap->Copy(*this);
	std::string rmsname = std::string(name) + "RMS";
	rms_graph = (TProfile *)my_dir->Get(rmsname.c_str());
}
*/		

TProfile *RMSProfile::GetTheRMSProfile(){		
	
	if(rms_graph != 0) return rms_graph;

	rms_graph = (TProfile *)(TProfile::Clone());
	Int_t nbinsx = GetNbinsX();
	for(Int_t j=0;j<nbinsx+1;j++){
		Double_t entries = GetBinEntries(j);
		Double_t RMS_val=0, RMS_err = 0, err_recal =0,rms_recal=0; 
		if(entries > 1){
			RMS_val = GetBinError(j) * sqrt(entries);
			RMS_err = RMS_val / TMath::Sqrt(2*(entries-1));
			rms_recal= RMS_val * entries;
			err_recal = TMath::Sqrt(
				fabs(RMS_err*RMS_err*entries*entries 
					- RMS_val*RMS_val*entries));
		}
			
		rms_graph->SetBinContent(j+1,rms_recal);
		rms_graph->SetBinEntries(j+1,entries);
		rms_graph->SetBinError(j+1,err_recal);
	}
	std::string rmsname = std::string(GetName()) + std::string("RMS");
	std::string rmstitle = std::string("RMS ") + std::string(GetTitle());
	rms_graph->SetTitle(rmstitle.c_str());
	rms_graph->SetName(rmsname.c_str());
	return rms_graph;			
}


int RMSProfile::Write(const char* name, Int_t option, Int_t bufsize){
	int value = ((TProfile*)(TProfile::Clone()))->Write(name,option,bufsize);
	value += GetTheRMSProfile()->Write(name,option,bufsize);
	return value;
}

int RMSProfile::Write(const char* name, Int_t option, Int_t bufsize) const{
	return Write(name,option,bufsize);
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
