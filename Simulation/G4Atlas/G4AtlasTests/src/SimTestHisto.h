/* -*- C++ -*- */

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASTESTS_SIMTESTHISTO_H
#define G4ATLASTESTS_SIMTESTHISTO_H
/** @file SimTestHisto.h
 * @author John Chapman - ATLAS Collaboration
 */

#include "AthenaKernel/errorcheck.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ServiceHandle.h"

class TProfile;
class TH1;
class TH2;

class SimTestHisto{
public:

  SimTestHisto():m_path("/truth/"){};
  ~SimTestHisto(){};

  IMessageSvc* msgSvc(){
    static IMessageSvc* mS = 0;
    if (!mS)
      {
	StatusCode status;
	ISvcLocator* svcLocator = Gaudi::svcLocator();
	status=svcLocator->service("MessageSvc", mS);
	if (status.isFailure())
	  std::cout<<" msgSvc(); could not access MessageSvc"<<std::endl;
	
      }
    return mS;     
    
  } 


  ITHistSvc* tHistSvc() {
    static ITHistSvc* hSvc = 0;
    if (!hSvc) {
      ISvcLocator* svcLocator = Gaudi::svcLocator();
      if (svcLocator->service("THistSvc", hSvc).isFailure()) {
	MsgStream log(msgSvc(),"SimTestHisto");
        log<<MSG::ERROR<<"Could not get the THistSvc!!!"<<endreq;
      }
    }
    return hSvc;
  }
  StatusCode registerHistogram(const std::string& path, TH1* hist) {

    if(tHistSvc()->regHist(path, hist).isFailure())
      return StatusCode::FAILURE;
    return StatusCode::SUCCESS;
  }
  
  
  StatusCode registerHistogram(const std::string& path, TH2* hist) {
    if(tHistSvc()->regHist(path, hist).isFailure())
      return StatusCode::FAILURE;
    return StatusCode::SUCCESS;
  }

protected:
  std::string m_path;

};

// note: var should be of type "TH1*" even if it is filled with a TProfile*
#define _TPROFILE(var,name,nbin,xmin,xmax)		       \
  if (!tHistSvc()->exists(m_path+name)) {		       \
    var = new TProfile(name,name,nbin,xmin,xmax);	       \
    CHECK(registerHistogram(m_path+name,var));		       \
  } else {						       \
    CHECK(tHistSvc()->getHist(m_path+name, var));	       \
  }

#define _TH1D(var,name,nbin,xmin,xmax)			       \
  if (!tHistSvc()->exists(m_path+name)) {		       \
    var = new TH1D(name,name,nbin,xmin,xmax);		       \
    var->StatOverflows();				       \
    CHECK(registerHistogram(m_path+name,var));		       \
  } else {						       \
    CHECK(tHistSvc()->getHist(m_path+name,var));	       \
  }

#define _TH1D_NOCHECK(var,name,nbin,xmin,xmax)			       \
  if (!tHistSvc()->exists(m_path+name)) {		       \
    var = new TH1D(name,name,nbin,xmin,xmax);		       \
    var->StatOverflows();				       \
    if(registerHistogram(m_path+name,var).isFailure())        \
  std::cout<<"Cannot register histogram "<<name<<std::endl;    \
  } else {						       \
    if(tHistSvc()->getHist(m_path+name,var).isFailure())       \
      std::cout<<"Cannot get histogram "<<name<<std::endl;     \
  }

#define _TH1D_WEIGHTED(var,name,nbin,xmin,xmax)	\
  _TH1D(var,name,nbin,xmin,xmax);		\
  var->Sumw2();

#define _TH2D_NOCHECK(var,name,nbinx,xmin,xmax,nbiny,ymin,ymax)	       \
  if (!tHistSvc()->exists(m_path+name)) {		       \
    var = new TH2D(name,name,nbinx,xmin,xmax,nbiny,ymin,ymax); \
    if(registerHistogram(m_path+name,var).isFailure())	       \
      std::cout<<"Cannot register histogram "<<name<<std::endl;\
  } else {						       \
    if(tHistSvc()->getHist(m_path+name,var).isFailure())       \
      std::cout<<"Cannot get histogram "<<name<<std::endl;     \
  }

#define _TH2D(var,name,nbinx,xmin,xmax,nbiny,ymin,ymax)	       \
  if (!tHistSvc()->exists(m_path+name)) {		       \
    var = new TH2D(name,name,nbinx,xmin,xmax,nbiny,ymin,ymax); \
    CHECK(registerHistogram(m_path+name,var));	               \
  } else {						       \
    CHECK(tHistSvc()->getHist(m_path+name,var));	       \
  }

#define _TH2D_WEIGHTED(var,name,nbinx,xmin,xmax,nbiny,ymin,ymax)	\
  _TH2D(var,name,nbinx,xmin,xmax,nbiny,ymin,ymax);			\
  var->Sumw2();

#define _SET_TITLE(var,title,xaxis,yaxis)				\
  var->SetXTitle(xaxis);						\
  var->SetYTitle(yaxis);						\
  var->SetTitle((std::string(var->GetName())+" : "+title).c_str());

//#define _SET_LOGX(var)
//TAxis *axis = var->GetXaxis();
//int bins = axis->GetNbin//s();

// Axis_t from = axis->GetXmin();
// Axis_t to = axis->GetXmax();
// Axis_t width = (to - from) / bins;
// Axis_t *new_bins = new Axis_t[bins + 1];

// for (int i = 0; i <= bins; i++) {
//   new_bins[i] = TMath::Power(10, from + i * width);
//
// }
// axis->Set(bins, new_bins);
// delete new_bins; 

#endif //G4ATLASTESTS_SIMTESTHISTO_H
