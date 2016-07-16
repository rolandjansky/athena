/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//
//      2014-05-21 Author: Remi Lafaye (Annecy) 
//      2015-02-28 Author: Bertrand LAFORGE (LPNHE Paris)
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
#include "TProfile.h"

#include "TrigDecisionTool/TrigDecisionTool.h"
#include <string>

egammaMonToolBase::egammaMonToolBase(const std::string & type, const std::string & name, const IInterface* parent)
  :  ManagedMonitorToolBase(type,name,parent), m_trigdec("Trig::TrigDecisionTool/TrigDecisionTool"), m_UseTrigger(false)
{

  // trigger tool
                                                                                                                                                  
  declareProperty("EgUseTrigger", m_UseTrigger);
  declareProperty("EgTrigDecisionTool", m_trigdec);

  // Name of trigger (automatic configuration)                                                          
                                                                      
  declareProperty("EgTrigger", m_Trigger, "Name of Trigger in express stream");

  // mutiple usage of the tool requires a configurable name for Groups

  declareProperty("EgGroupExtension", m_GroupExtension="", "Name of Group Extension");

  m_region.resize(NREGION,"");
  m_region[BARREL]="BARREL";
  m_region[CRACK]="CRACK";
  m_region[ENDCAP]="ENDCAP";
  m_region[FORWARD]="FORWARD";

  m_currentLB = 0;
  m_storeGate = 0;
}

egammaMonToolBase::~egammaMonToolBase()
{
  ATH_MSG_DEBUG("egammaMonToolBase::~egammaMonToolBase()");
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

  if (m_UseTrigger) {
    sc = m_trigdec.retrieve();
    if (!sc.isFailure()){
      ATH_MSG_DEBUG("TriggerTool found");
    } 
    else {
      ATH_MSG_FATAL("Unable to retrieve TriggerTool" );
    }
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

void egammaMonToolBase::bookTProfile(TProfile* &h, MonGroup& mygroup, const std::string hname, const std::string htitle, int nbins, float xlow, float xhigh, float ylow, float yhigh)
{
  h = new TProfile(hname.c_str(),htitle.c_str(),nbins,xlow,xhigh,ylow,yhigh);
  regHist(h,mygroup).ignore();
}

void egammaMonToolBase::bookTH1FperRegion(std::vector<TH1*> &vhist, MonGroup& mygroup, const std::string hname, const std::string htitle, int nbins, float low, float high, unsigned int min_region, unsigned int max_region)
{
  std::string name, title;
  
  vhist.resize(NREGION,0);
  for(unsigned int ir=min_region;ir<=max_region;ir++) {
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

void egammaMonToolBase::bookTH2FperRegion(std::vector<TH2*> &vhist, MonGroup& mygroup, const std::string hname, const std::string htitle, int nbinsx, float xlow, float xhigh, int nbinsy, float ylow, float yhigh, unsigned int min_region, unsigned int max_region)
{
  std::string name, title;
  
  vhist.resize(NREGION,0);
  for(unsigned int ir=min_region;ir<=max_region;ir++) {
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
  if(h) h->Fill(x);
 }

void egammaMonToolBase::fillTH2FperRegion(std::vector<TH2*> &vhist, unsigned int ir, float x, float y)
{
  unsigned int size = vhist.size();
  if(ir>=size) return;
  TH2 *h = vhist[ir];  
  if(h) h->Fill(x,y);
}

int egammaMonToolBase::GetForwardRegion(float eta)
{
  float aeta = fabs(eta);
  if( aeta < 3.2 ) return ENDCAP;
  return FORWARD;
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
  // Forward region not checked here! See GetForwardRegion()
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
  if(endOfRunFlag()) {
    // Extra work to be done at end of run
  }

  return StatusCode::SUCCESS;
}

void egammaMonToolBase::fillEfficiencies(TH1* h, TH1* href)
{
  int nbins = h->GetNbinsX();
  if(href->GetNbinsX()!=nbins) {
    ATH_MSG_WARNING("egammaMonToolBase::FillEfficiencies(): histograms have different number of bins, can not divide!");
    return;
  }

  ATH_MSG_DEBUG("egammaMonToolBase::FillEfficiencies(): start new computation");

  for(int i=1;i<nbins+1;++i){
    double eps     = 0.;
    double err     = 0.;
    double Yref    = href->GetBinContent(i);
    if(Yref>0) {
      double A   = h->GetBinContent(i);
      eps = A/Yref;
      err = sqrt(eps*(1-eps)/Yref);

      // convert to percent 
      eps *= 100.;
      err *= 100.;

    } 

    h->SetBinContent(i,eps);
    h->SetBinError(i,err);
  }
  return;
}

bool egammaMonToolBase::hasGoodTrigger(std::string comment){

  if (!m_UseTrigger) {
    ATH_MSG_DEBUG("No Trigger request for that monitoring tool");
    return true; // return true if no trigger selection is expected
  }

  ATH_MSG_DEBUG( "Size of " << comment << " Trigger vector = "<< m_Trigger.size() );
  bool triggerfound = false;
  for (unsigned int i = 0 ; i < m_Trigger.size() ; ++i){
    ATH_MSG_DEBUG(comment << " Trigger " << m_Trigger.at(i)  << " for that event ?");
    if (m_trigdec->isPassed(m_Trigger.at(i)))
    {
      triggerfound=true;
      ATH_MSG_DEBUG(comment << " Trigger " << m_Trigger.at(i)  << " found for that event");
    }
  }
  
  if (!triggerfound)  ATH_MSG_DEBUG("No " << comment << " Trigger found for that event");

  // Alternative code if we need to get a list of avalaible triggers in the stream
  if (!m_trigdec.retrieve().isFailure()){
    const std::vector<std::string>& vec = m_trigdec->getListOfTriggers();
    ATH_MSG_DEBUG( "Size of Trigger vector in xAOD file = "<< vec.size());
    for (unsigned int i=0 ; i < vec.size() ; ++i) {
      if (m_trigdec->isPassed(vec[i])) {
	ATH_MSG_DEBUG("Active Trigger found : " << vec[i] );
      }
      //      else {
      //	ATH_MSG_DEBUG("Passive Trigger found : " << vec[i] );
      //      }
    } 
  }
  else {
    ATH_MSG_DEBUG("Trigger m_trigdec is null !!!");
  }
  
  // only fill histograms if the event has been correctly triggered or if no trigger is expected.
  return triggerfound;
}
