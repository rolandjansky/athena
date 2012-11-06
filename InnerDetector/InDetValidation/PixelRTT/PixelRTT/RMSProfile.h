/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RMSProfile_h
#define RMSProfile_h

#include <TProfile.h>

class RMSProfile: public TProfile{
private:
	TProfile *rms_graph;

public:
	RMSProfile();
	RMSProfile(const char *name,const char *title,
			Int_t nbinsx,Double_t xlow,Double_t xup, Option_t *option="");
	RMSProfile(const char *name,const char *title,
			Int_t nbinsx,Double_t xlow,Double_t xup,
			Double_t ylow,Double_t yup,Option_t *option="");
	RMSProfile(const char *name,const char *title,
			Int_t nbinsx,const Float_t *xbins, Option_t *option="");
	RMSProfile(const char *name,const char *title,
			Int_t nbinsx,const Double_t  *xbins, Option_t *option="");
	RMSProfile(const char *name,const char *title,
			Int_t nbinsx,const Double_t *xbins,
			Double_t ylow,Double_t yup, Option_t *option="");
	RMSProfile(const TProfile &profile);
//	RMSProfile(const char *u_name,TDirectory * my_dir);
		
	virtual ~RMSProfile(){};

	TObject *Clone(const char *name = "") const;

	TProfile *GetTheRMSProfile();

	virtual int Write(const char* name = 0, Int_t option = 0, Int_t bufsize = 0);
	virtual int Write(const char* name = 0, Int_t option = 0, Int_t bufsize = 0) const;
		
	void DrawRMS(Option_t* option = "");
	
	virtual const char *ClassName() const;

};


#endif
