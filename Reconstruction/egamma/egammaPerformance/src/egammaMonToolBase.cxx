/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//
//      2014-05-21 Author: Remi Lafaye (Annecy) 
//
//      NAME:    egammaMonToolBase.cxx
//      PACKAGE: offline/Reconstruction/egamma/egammaPerformance
//      PURPOSE: Provides basic functionalities to egammaMonTool clsees
//          
/////////////////////////////////////////////////////////////


#include "egammaPerformance/egammaMonToolBase.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "StoreGate/StoreGateSvc.h"
#include "xAODEventInfo/EventInfo.h"
#include "AthenaMonitoring/AthenaMonManager.h"
#include "TH1F.h"
#include "TH2F.h"

egammaMonToolBase::egammaMonToolBase(const std::string & type, const std::string & name, const IInterface* parent)
  :  ManagedMonitorToolBase(type,name,parent)
{
  m_region.resize(NREGION,"");
  m_region[BARREL]="BARREL";
  m_region[CRACK]="CRACK";
  m_region[ENDCAP]="ENDCAP";
  m_region[FORWARD]="FORWARD";

  m_storeGate = 0;
}

egammaMonToolBase::~egammaMonToolBase()
{
}

StatusCode egammaMonToolBase::initialize()
{
  StatusCode sc = ManagedMonitorToolBase::initialize();
  if(sc.isFailure()) {
    ATH_MSG_FATAL( "ManagedMonitorToolBase::initialize() - Failed" );
  }

  sc = service("StoreGateSvc", m_storeGate);
  if(sc.isFailure()) {
    ATH_MSG_FATAL( "Unable to locate Service StoreGateSvc" );
  }
  return sc;
}

StatusCode egammaMonToolBase::bookHistograms()
{
  ATH_MSG_DEBUG("egammaMonToolBase::bookHistograms()");

  return StatusCode::SUCCESS;
}

void egammaMonToolBase::bookTH1F(TH1* &h, MonGroup& mygroup, const std::string hname, const std::string htitle, int nbins, float low, float high)
{
  h = new TH1F(hname.c_str(),htitle.c_str(),nbins,low,high);
  regHist(h,mygroup).ignore();
}

void egammaMonToolBase::bookTH2F(TH2* &h, MonGroup& mygroup, const std::string hname, const std::string htitle, int nbinsx, float xlow, float xhigh, int nbinsy, float ylow, float yhigh)
{
  h = new TH2F(hname.c_str(),htitle.c_str(),nbinsx,xlow,xhigh,nbinsy,ylow,yhigh);
  regHist(h,mygroup).ignore();
}

void egammaMonToolBase::bookTH1FperRegion(std::vector<TH1*> &vhist, MonGroup& mygroup, const std::string hname, const std::string htitle, int nbins, float low, float high, unsigned int max_region)
{
  std::string name, title;
  
  vhist.resize(NREGION,0);
  for(unsigned int ir=0;ir<=max_region;ir++) {
    // Create histograms
    name  = hname+"_"+m_region[ir];
    title = htitle+" "+m_region[ir];
    TH1 *h = new TH1F(name.c_str(),title.c_str(),nbins,low,high);
    vhist[ir] = h;
    //ATH_MSG_DEBUG("Histo: " << name.c_str() << " " << h);
    // Register histograms
    regHist(h,mygroup).ignore();
  }
}

void egammaMonToolBase::bookTH2FperRegion(std::vector<TH2*> &vhist, MonGroup& mygroup, const std::string hname, const std::string htitle, int nbinsx, float xlow, float xhigh, int nbinsy, float ylow, float yhigh, unsigned int max_region)
{
  std::string name, title;
  
  vhist.resize(NREGION,0);
  for(unsigned int ir=0;ir<=max_region;ir++) {
    // Create histograms
    name  = hname+"_"+m_region[ir];
    title = htitle+" "+m_region[ir];
    TH2 *h = new TH2F(name.c_str(),title.c_str(),nbinsx,xlow,xhigh,nbinsy,ylow,yhigh);
    vhist[ir] = h;
    // Register histograms
    regHist(h,mygroup).ignore();
  }
}

void egammaMonToolBase::fillTH1FperRegion(std::vector<TH1*> &vhist, unsigned int ir, float x)
{
  unsigned int size = vhist.size();
  if(ir>=size) return;
  TH1 *h = vhist[ir];
  //ATH_MSG_DEBUG("Histo: " << h);
  if(h) h->Fill(x);
 }

void egammaMonToolBase::fillTH2FperRegion(std::vector<TH2*> &vhist, unsigned int ir, float x, float y)
{
  unsigned int size = vhist.size();
  if(ir>=size) return;
  TH2 *h = vhist[ir];  if(h) h->Fill(x,y);
}


int egammaMonToolBase::GetRegion(float eta)
{
  float aeta = fabs(eta);
  // check if object is in barrel
  if( aeta < 1.37 )    return BARREL;
  // check if object is in end-caps
  if( aeta > 1.52) /* && aeta < 2.47 ) */ return ENDCAP;
  // check if object is in crack region
  //if( aeta > 1.37 && aeta < 1.52 ) 
  return CRACK;
  // Forward region needs to be checked differently! return FORWARD;
}

StatusCode egammaMonToolBase::fillHistograms()
{
  ATH_MSG_DEBUG("egammaMonToolBase::fillHistograms()");

  return StatusCode::SUCCESS;
}

//GYS. In the case the event is flagged bad due to LAr noise or LAr data error this is true
bool egammaMonToolBase::hasBadLar()
{
  const xAOD::EventInfo* event_info;
  StatusCode sc = m_storeGate->retrieve( event_info );
  if (sc.isFailure()) {
    ATH_MSG_WARNING("Could not get LAr event info!");
  }

  xAOD::EventInfo::EventFlagErrorState error_state = event_info->errorState(xAOD::EventInfo::LAr);

  // Only removing events with Error not with Warning
  //if (error_state==xAOD::EventInfo::Warning)
  //  {
  //   ATH_MSG_DEBUG("LAr event noisy");
  //   return true;
  //  }

  if (error_state==xAOD::EventInfo::Error)
    {
      ATH_MSG_DEBUG("LAr event data integrity error");
      return true;
   }
  
  ATH_MSG_DEBUG("No LAr noise");
  return false;
}

StatusCode egammaMonToolBase::procHistograms()
{
  if(endOfRun) {
    // Extra work to be done at end of run
  }

  return StatusCode::SUCCESS;
}
