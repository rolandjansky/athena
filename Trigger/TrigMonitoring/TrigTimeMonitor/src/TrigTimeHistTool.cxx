/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "boost/foreach.hpp"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ThreadGaudi.h"

#include "TrigTimeAlgs/ITrigTimerSvc.h"
#include "TrigTimeAlgs/TrigTimer.h"

//#include "AthenaMonitoring/IMonitorToolBase.h"
#include "TrigTimeMonitor/TrigTimeHistTool.h"


#include <vector>
#include <algorithm>
#include <utility>
#include <TH1F.h>
#include <TH2F.h>
#include "boost/regex.hpp"


TrigTimeHistTool::TrigTimeHistTool(const std::string & type, const std::string & name,
				   const IInterface* parent) 
  :  TrigMonitorToolBase(type, name, parent), 
     m_path("TIMERS"),
     m_bins(50),
     m_doPerObjHist(false), 
     m_includeName(".+"),     // this matches all, by default all timers are histogrammed
     m_excludeName("()")      // this matches only empty string so shoudl not block any timer
 {
  m_timerHistLimits.push_back(0.);
  m_timerHistLimits.push_back(200.);
  m_timerPerObjHistLimits.push_back(0.);
  m_timerPerObjHistLimits.push_back(200.);

  declareInterface<IMonitorToolBase>(this);
  declareProperty("Key", m_key, "Exact names of timer to be monitored");
  declareProperty("BookingDir", m_path="TIMERS", "Prefered booking path for histogram, if blank the algorithm name used" );
  declareProperty("NumberOfHistBins", m_bins, "Number of bins");
  declareProperty("TimerHistLimits", m_timerHistLimits, "Limits of the histogram");
  declareProperty("DoPerObjHist", m_doPerObjHist=false, "Do histogram time/(propery value) if value != 0");
  declareProperty("TimerPerObjHistLimits", m_timerPerObjHistLimits, "Limits of histogram with time/property value");
  declareProperty("GroupedTimers", m_regexGroups, "Regex matching timers to be joined in single histogram");
  declareProperty("IncludeName", m_includeName, "Regex matching timer names, if do not match then this timer is histogrammed (default accepts all).");
  declareProperty("ExcludeName", m_excludeName, "Regex matching timer names, if matches then this timer is not histogrammed (default matches nothing).");
  
}


TrigTimeHistTool::~TrigTimeHistTool(){
}

StatusCode TrigTimeHistTool::initialize(){
  if ( TrigMonitorToolBase::initialize().isFailure() )
    return StatusCode::FAILURE;
  return StatusCode::SUCCESS;
}

StatusCode TrigTimeHistTool::bookHists(){
  MsgStream log(msgSvc(), name());

  // Cleanup before re-booking
  BOOST_FOREACH(struct TimerHist *t, m_entries){
    delete t;
  }



  m_entries.clear();

  // More cleanup before re-booking (added 28.09.10 by MM)
  for (std::map <std::string, struct TimerGroupHist *>::const_iterator g = m_groupHists.begin(); 
       g != m_groupHists.end(); g++) {
    (g->second)->m_timers.clear();
  }

  ITrigTimerSvc *m_timerSvc = 0;
  if (!service("TrigTimerSvc", m_timerSvc).isSuccess()) {
    log << MSG::ERROR << "Unable to locate TrigTimerSvc" << endreq;
    return StatusCode::FAILURE;
  }


  // find out for whom we are running (i.e. Algo)    
  const Algorithm *parentAlg = dynamic_cast<const Algorithm*>(parent());
  if ( parentAlg ) 
    m_parentName = getGaudiThreadGenericName(parentAlg->name());
  


  std::string path = (m_path == "") ? m_parentName : m_path;
  TrigMonGroup expertHistograms( this, path, expert );

  // compile regexes for grouping
  std::vector<std::pair<std::string,boost::regex> > matchingList;
  std::map<std::string, std::string>::const_iterator regIter;
  for (regIter = m_regexGroups.begin(); regIter != m_regexGroups.end(); regIter++) {
    matchingList.push_back(make_pair((*regIter).first,boost::regex((*regIter).second)));
  }

  // compile regexes for exclusion / inclusion
  boost::regex includeName(m_includeName);
  boost::regex excludeName(m_excludeName);


  // loop over all the timers and extract 
  std::vector < TrigTimer * >::iterator tIter;
  std::vector<std::pair<std::string,boost::regex> >::const_iterator matchIter;
  boost::cmatch what;
  for (tIter = m_timerSvc->begin(); tIter != m_timerSvc->end(); tIter++) {

    std::string name = (*tIter)->name();

    // replacement of . to _ will not be needed since the
    // timer names should now be: "algoA:mytimer"
    // I will assure it in TrigMonTHistSvc regex that name does not have "." 

    if ( !selected(name) ) {
      continue;
    }
    if ( ! boost::regex_match(name.c_str(), what, includeName) ) {
      log << MSG::DEBUG << "Skipping "<<name<<" because it does not match includeName regex " << m_includeName << endreq;
      continue;
    }

    if ( boost::regex_match(name.c_str(), what, excludeName) ) {
      log << MSG::DEBUG << "Skipping "<<name<<" because it matches match excludeName regex " << m_excludeName << endreq;
      continue;
    }
    log << MSG::VERBOSE << "Considering " << name <<endreq;


      
    replace(name.begin(), name.end(), '.', '_');
    replace(name.begin(), name.end(), ':', '_');
   
    bool isGrouped=false;
    for (matchIter=matchingList.begin(); matchIter!=matchingList.end(); matchIter++) {
      if ( boost::regex_match(name.c_str(), what, (*matchIter).second) ) {
	log << MSG::DEBUG << "Adding timer "<<name<<" to "<<(*matchIter).first<<" histogram"<<endreq;
	isGrouped=true;
	if (m_groupHists.find((*matchIter).first)==m_groupHists.end())
	  m_groupHists[(*matchIter).first]=new struct TimerGroupHist;
	m_groupHists[(*matchIter).first]->m_timers.push_back(*tIter);
      }
    }

    if (isGrouped) continue;

    std::string prop = (*tIter)->propName();
    replace(prop.begin(), prop.end(), '.', '_');
    replace(prop.begin(), prop.end(), ':', '_');

      
    struct TimerHist *th = new struct TimerHist;
    th->m_timer = *tIter;
    // create histograms
    th->m_timerHist = new TH1F(name.c_str(),
			       (name + ";time[ms];entries").c_str(),
			       m_bins,
			       m_timerHistLimits[0],
			       m_timerHistLimits[1]);

    if (expertHistograms.regHist(th->m_timerHist).isFailure())
      log << MSG::WARNING << "Unable to book timer histogram " 
	  << m_path + name << endreq;

    // create histogram with timing per object
    if (m_doPerObjHist && prop != "") {
      th->m_timerPerObjHist = new TH1F((name + prop ).c_str(),
				       (name + prop + ";time[ms];entries").c_str(),
				       m_bins,
				       m_timerPerObjHistLimits[0],
				       m_timerPerObjHistLimits[1]);

      if (expertHistograms.regHist(th->m_timerPerObjHist).isFailure())
	log << MSG::WARNING << "Unable to book timer histogram " 
	    << m_path + name + prop << endreq;
    }
    else
      th->m_timerPerObjHist = (TH1F *) 0;
	    
    m_entries.push_back(th);
    
  }

  // book the grouped histograms
  std::map <std::string, struct TimerGroupHist *>::const_iterator g;
  for (g = m_groupHists.begin(); g != m_groupHists.end(); g++) {
    std::vector<TrigTimer *>::const_iterator t;
    std::string histName=(*g).first;
    replace(histName.begin(), histName.end(), '.', '_');
    replace(histName.begin(), histName.end(), ':', '_');
    TH2F *timerHist = new TH2F(histName.c_str(),
			       (histName + ";;time[ms]").c_str(),
			       (*g).second->m_timers.size(),0,(*g).second->m_timers.size(),
			       m_bins,
			       m_timerHistLimits[0],
			       m_timerHistLimits[1]);
    int binNum=1;
    TAxis *xAxis=timerHist->GetXaxis();
    for(std::vector<TrigTimer *>::iterator iter=(*g).second->m_timers.begin();iter!=(*g).second->m_timers.end();iter++) {
      xAxis->SetBinLabel(binNum++,(*iter)->name().c_str());
    }
    if (expertHistograms.regHist(timerHist).isFailure())
      log << MSG::WARNING << "Unable to book timer histogram "
	  << m_path + histName << endreq;
    (*g).second->m_timerHist=timerHist;
  }
  return StatusCode::SUCCESS;  
}

StatusCode TrigTimeHistTool::fillHists(){
  std::vector < struct TimerHist *>::const_iterator t;
  for (t = m_entries.begin(); t != m_entries.end(); t++) {
    if ( !(*t)->m_timer->running() && (*t)->m_timer->wasRun() ) {
      (*t)->m_timerHist->Fill((*t)->m_timer->elapsed());
      if ((*t)->m_timerPerObjHist != (TH1F *) 0
	  && (*t)->m_timer->propVal() != 0) {
	(*t)->m_timerPerObjHist->Fill((*t)->m_timer->elapsed()
				      / (*t)->m_timer->propVal());
      }
    }
  }
  std::map <std::string, struct TimerGroupHist *>::const_iterator g;
  for (g = m_groupHists.begin(); g != m_groupHists.end(); g++) {
    std::vector<TrigTimer *>::const_iterator t;
    for (t= (*g).second->m_timers.begin(); t != (*g).second->m_timers.end();t++) {
      if ( !(*t)->running() && (*t)->wasRun() ) {
        (*g).second->m_timerHist->Fill((*t)->name().c_str(),(*t)->elapsed(),1.);
      }
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode TrigTimeHistTool::finalHists(){
  // should I print out some sophisticated statistical params of the hists?
  return StatusCode::SUCCESS;
}



bool TrigTimeHistTool::selected(const std::string & name){
  if (m_key.empty()) {
    // no specific Keys are selected ... record
    // then all the timers which parent algo registered
    if ( ! m_parentName.empty() ) {
      if ( name.find(m_parentName+":") == 0 ) // name starts from algo name
	return true;
    }
    return false;
  }
  // exact match (maybe consider regex here)
  std::vector < std::string >::const_iterator key =
    find(m_key.begin(), m_key.end(), name);

  if (key != m_key.end())
    return true;

  return false;
}
