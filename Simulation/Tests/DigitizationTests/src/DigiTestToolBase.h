/* -*- C++ -*- */

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DIGITIZATIONTESTS_DIGITESTTOOLBASE_H
#define DIGITIZATIONTESTS_DIGITESTTOOLBASE_H
/** @file DigiTestToolBase.h
 * @author John Chapman - ATLAS Collaboration
 */
#include "AthenaBaseComps/AthAlgTool.h"
#include "DigitizationTests/IDigiTestTool.h"

#include "AthenaKernel/errorcheck.h"
#include "GaudiKernel/ITHistSvc.h"
// #include "GaudiKernel/ISvcLocator.h"
// #include "GaudiKernel/Bootstrap.h"
// #include "GaudiKernel/ServiceHandle.h"

class TProfile;
class TH1;
class TH2;

class DigiTestToolBase : public extends<AthAlgTool, IDigiTestTool> {
public:
 /// \name structors and AlgTool implementation
 //@{
  DigiTestToolBase(const std::string& name, 
         const std::string& type,
         const IInterface* parent);
  virtual StatusCode initialize() {
    return StatusCode::SUCCESS;
  }
  //@}
  ITHistSvc* tHistSvc() {
	static ITHistSvc* hSvc = 0;
	if (!hSvc) {
      ISvcLocator* svcLocator = Gaudi::svcLocator();
      if (svcLocator->service("THistSvc", hSvc).isFailure()) {
        ATH_MSG_ERROR("Could not get the THistSvc!!!");
      }
    }
    return hSvc;
  }
  StatusCode registerHistogram(const std::string& path, TH1* hist) {
    CHECK(tHistSvc()->regHist(path, hist));
    return StatusCode::SUCCESS;
  }


  StatusCode registerHistogram(const std::string& path, TH2* hist) {
    CHECK(tHistSvc()->regHist(path, hist));
    return StatusCode::SUCCESS;
  }


 protected:
  // general prefix (e.g. "/truth/")
  std::string m_path;

  /// The MC truth key
  std::string m_key;
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

#define _TH1D_WEIGHTED(var,name,nbin,xmin,xmax)	\
  _TH1D(var,name,nbin,xmin,xmax);		\
  var->Sumw2();

#define _TH2D(var,name,nbinx,xmin,xmax,nbiny,ymin,ymax)	       \
  if (!tHistSvc()->exists(m_path+name)) {		       \
    var = new TH2D(name,name,nbinx,xmin,xmax,nbiny,ymin,ymax); \
    CHECK(registerHistogram(m_path+name,var));		       \
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


#endif
