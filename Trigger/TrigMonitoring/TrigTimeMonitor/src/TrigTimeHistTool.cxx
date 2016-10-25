/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ThreadGaudi.h"

#include "TrigTimeAlgs/ITrigTimerSvc.h"
#include "TrigTimeAlgs/TrigTimer.h"
#include "TrigTimeMonitor/TrigTimeHistTool.h"

#include "boost/algorithm/string/erase.hpp"

#include <vector>
#include <algorithm>
#include <utility>
#include <TH1F.h>
#include <TH2F.h>

TrigTimeHistTool::TrigTimeHistTool(const std::string & type, const std::string & name,
				   const IInterface* parent) 
  :  TrigMonitorToolBase(type, name, parent), 
     m_path("TIMERS"),
     m_bins(50),
     m_doPerObjHist(false),
     m_doPerLBGroups(false),
     m_includeName(".+"),     // this matches all, by default all timers are histogrammed
     m_excludeName("()")      // this matches only empty string so should not block any timer
 {
  m_timerHistLimits.push_back(0.);
  m_timerHistLimits.push_back(200.);
  m_timerPerObjHistLimits.push_back(0.);
  m_timerPerObjHistLimits.push_back(200.);

  declareInterface<IMonitorToolBase>(this);
  declareProperty("Key", m_key, "Regex macthing timer names, if not set only consider timers of parent");
  declareProperty("BookingDir", m_path, "Prefered booking path for histogram, if blank the algorithm name used" );
  declareProperty("NumberOfHistBins", m_bins, "Number of bins");
  declareProperty("TimerHistLimits", m_timerHistLimits, "Limits of the histogram");
  declareProperty("DoPerObjHist", m_doPerObjHist, "Do histogram time/(propery value) if value != 0");
  declareProperty("TimerPerObjHistLimits", m_timerPerObjHistLimits, "Limits of histogram with time/property value");
  declareProperty("GroupedTimers", m_regexGroups, "Regex matching timers to be joined in single histogram (regex captures will be removed from bin label)");
  declareProperty("DoPerLBGroups", m_doPerLBGroups, "Publish grouped histograms per LB as TProfile");
  declareProperty("IncludeName", m_includeName, "Regex matching timer names, if do not match then this timer is histogrammed (default accepts all).");
  declareProperty("ExcludeName", m_excludeName, "Regex matching timer names, if matches then this timer is not histogrammed (default matches nothing).");
  
}

TrigTimeHistTool::~TrigTimeHistTool()
{
}

StatusCode TrigTimeHistTool::initialize()
{
  ATH_CHECK(TrigMonitorToolBase::initialize());
  return StatusCode::SUCCESS;
}

StatusCode TrigTimeHistTool::bookHists()
{
  // Cleanup before re-booking
  for (auto t : m_entries) delete t;
  for (const auto& g :m_groupHists) g.second->m_timers.clear();
  m_entries.clear();

  ServiceHandle<ITrigTimerSvc> timerSvc("TrigTimerSvc",name());
  ATH_CHECK(timerSvc.retrieve());

  // find out for whom we are running (i.e. Algo)    
  const Algorithm *parentAlg = dynamic_cast<const Algorithm*>(parent());
  if ( parentAlg ) 
    m_parentName = getGaudiThreadGenericName(parentAlg->name());
  

  std::string path = (m_path == "") ? m_parentName : m_path;
  TrigMonGroup expertHistograms( this, path, expert );

  // compile regexes for grouping
  std::vector<std::pair<std::string,boost::regex> > matchingList;
  for (const auto& reg : m_regexGroups) {
    matchingList.push_back(make_pair(reg.first, boost::regex(reg.second)));
  }

  // compile regexes for exclusion / inclusion
  boost::regex includeName(m_includeName);
  boost::regex excludeName(m_excludeName);
  boost::regex key(m_key);

  // loop over all the timers and extract 
  std::vector < TrigTimer * >::const_iterator tIter;
  for (tIter = timerSvc->begin(); tIter != timerSvc->end(); ++tIter) {

    std::string name = (*tIter)->name();

    // replacement of . to _ will not be needed since the
    // timer names should now be: "algoA:mytimer"
    // I will assure it in TrigMonTHistSvc regex that name does not have "." 

    if ( !selected(name, key) ) {
      continue;
    }
    if ( ! boost::regex_match(name, includeName) ) {
      ATH_MSG_DEBUG("Skipping " << name << " because it does not match includeName regex " << m_includeName);
      continue;
    }
    if ( boost::regex_match(name, excludeName) ) {
      ATH_MSG_DEBUG("Skipping " << name << " because it matches match excludeName regex " << m_excludeName);
      continue;
    }

    ATH_MSG_VERBOSE("Considering " << name);
         
    bool isGrouped = false;
    for (const auto& m : matchingList) {
      if ( boost::regex_match(name, m.second) ) {
        ATH_MSG_DEBUG("Adding timer " << name << " to " << m.first << " histogram");
        isGrouped=true;
        if (m_groupHists.find(m.first)==m_groupHists.end())
          m_groupHists[m.first] = new struct TimerGroupHist;
        m_groupHists[m.first]->m_timers.push_back(*tIter);
      }
    }

    if (isGrouped) continue;

    // Cleanup characters for histogram names
    std::string prop = (*tIter)->propName();
    replace(name.begin(), name.end(), '.', '_');
    replace(name.begin(), name.end(), ':', '_');
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
      ATH_MSG_WARNING("Unable to book timer histogram " << m_path << name);

    // create histogram with timing per object
    if (m_doPerObjHist && prop != "") {
      th->m_timerPerObjHist = new TH1F((name + prop ).c_str(),
                                       (name + prop + ";time[ms];entries").c_str(),
                                       m_bins,
                                       m_timerPerObjHistLimits[0],
                                       m_timerPerObjHistLimits[1]);

      if (expertHistograms.regHist(th->m_timerPerObjHist).isFailure())
        ATH_MSG_WARNING("Unable to book timer histogram " << m_path << name << prop);
    }
    else
      th->m_timerPerObjHist = 0;
	    
    m_entries.push_back(th);
    
  }

  // book the grouped histograms
  int ig(-1);
  for (const auto& g : m_groupHists) {
    ig++;
    std::string histName = g.first;
    replace(histName.begin(), histName.end(), '.', '_');
    replace(histName.begin(), histName.end(), ':', '_');

    // Regular TH2F or TProfile for LB-histograms
    TAxis *xAxis;
    if (m_doPerLBGroups) {
      g.second->m_timerProf = new
        TrigLBNHist<TProfile>(TProfile(histName.c_str(),
                                       (histName + ";;time[ms]").c_str(),
                                       g.second->m_timers.size(), 0, g.second->m_timers.size()));
      xAxis = g.second->m_timerProf->GetXaxis();
    }
    else {
      g.second->m_timerHist = new TH2F(histName.c_str(),
                                       (histName + ";;time[ms]").c_str(),
                                       g.second->m_timers.size(), 0, g.second->m_timers.size(),
                                       m_bins,
                                       m_timerHistLimits[0],
                                       m_timerHistLimits[1]);
      xAxis = g.second->m_timerHist->GetXaxis();
    }

    // Assign bin labels
    int binNum = 1;
    boost::smatch what;
    for (TrigTimer* timer : g.second->m_timers) {
      std::string label = timer->name();
      // If there is a capture in the regex, remove these matches from the bin label
      if (boost::regex_match(timer->name(), what, matchingList[ig].second) && what.size()>1) {
        for (size_t i=1; i<what.size(); ++i) {
          boost::erase_all(label, std::string(what[i]));
        }
      }
      xAxis->SetBinLabel(binNum++, label.c_str());
    }
    // Register histogram
    StatusCode sc = m_doPerLBGroups ?
      expertHistograms.regHist((ITrigLBNHist*)g.second->m_timerProf) :
      expertHistograms.regHist(g.second->m_timerHist);
    
    if (sc.isFailure())
        ATH_MSG_WARNING("Unable to book timer histogram " << m_path << histName);
  }
  return StatusCode::SUCCESS;  
}

StatusCode TrigTimeHistTool::fillHists()
{
  for (const struct TimerHist* t : m_entries) {
    if ( !t->m_timer->running() && t->m_timer->wasRun() ) {
      t->m_timerHist->Fill(t->m_timer->elapsed());
      if (t->m_timerPerObjHist != 0 && t->m_timer->propVal() != 0) {
        t->m_timerPerObjHist->Fill(t->m_timer->elapsed() / t->m_timer->propVal());
      }
    }
  }
  for (const auto& g : m_groupHists) {
    int bin(0);
    for (const TrigTimer* t : g.second->m_timers) {
      if ( !t->running() && t->wasRun() ) {
        // For performance reasons, the filling is done via the x-value (first bin: x=0)
        if (m_doPerLBGroups) {
          g.second->m_timerProf->Fill((Double_t)bin, t->elapsed(), 1.);
        }
        else {
          g.second->m_timerHist->Fill((Float_t)bin, t->elapsed(), 1.);
        }
      }
      bin++;
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode TrigTimeHistTool::finalHists()
{
  return StatusCode::SUCCESS;
}


bool TrigTimeHistTool::selected(const std::string& name, const boost::regex& regex){
  if (regex.size()==0) {
    // no specific Keys are selected ... record
    // then all the timers which parent algo registered
    if ( ! m_parentName.empty() ) {
      if ( name.find(m_parentName+":") == 0 ) // name starts from algo name
        return true;
    }
  }
  else {
    return boost::regex_match(name, regex);
  }

  return false;  
}
