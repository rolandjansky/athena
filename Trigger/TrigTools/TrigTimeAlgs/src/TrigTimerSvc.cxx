// this is -*- c++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigTimeAlgs/TrigTimerSvc.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/IIncidentSvc.h"

#include "TH1F.h"

#include <algorithm>
#include <string>
#include <vector>


TrigTimerSvc::TrigTimerSvc( const std::string& name, ISvcLocator* svcloc ) 
  : base_class(name, svcloc),
    m_includeName(".+"),     // by default all is allowed
    m_excludeName("()"),     // by default empty string is disallowed
    m_histProp_timerCalls(Gaudi::Histo1DDef("Number of timer calls/event",0,10000,1000))
{    
  declareProperty("IncludeName", m_includeName, "Regex matching timer names, if do not match a fake(disabled) timer will be given out.");
  declareProperty("ExcludeName", m_excludeName, "Regex matching timer names, if matches a fake(disabled) timer will be given out.");
  declareProperty("dumpFinalStatistics" , m_dumpFinalStatistics=false, "Make extended printout of timers during finalization");
  declareProperty("HistTimerCalls", m_histProp_timerCalls, "Histogram binning for timer calls");
}

TrigTimerSvc::~TrigTimerSvc() {
  for (auto timer : m_allTimers) {
	delete timer;
  }  
}

StatusCode TrigTimerSvc::queryInterface (const InterfaceID& riid, void** ppvInterface ) {

  if ( IID_ITrigTimerSvc.versionMatch(riid) ) { 
	*ppvInterface = static_cast<ITrigTimerSvc*> (this); 
  } else { 
	return AthService::queryInterface(riid, ppvInterface) ; 
  }
  return StatusCode::SUCCESS;
}

StatusCode TrigTimerSvc::initialize ( ) {
  
  // compile regexes
  m_includeRegex = boost::regex(m_includeName);
  m_excludeRegex = boost::regex(m_excludeName);
  
  ServiceHandle<IIncidentSvc> incSvc("IncidentSvc",name());
  ATH_CHECK(incSvc.retrieve());
  incSvc->addListener(this, "EndEvent");

  return StatusCode::SUCCESS;
}

StatusCode TrigTimerSvc::start ( ) {
  // Book histogram
  ServiceHandle<ITHistSvc> histSvc("THistSvc", name());
  ATH_CHECK(histSvc.retrieve());
  const std::string histPath = "/EXPERT/HLTFramework/" + name() + "/";
  const auto& v = m_histProp_timerCalls.value();
  m_hist_timerCalls = new TH1F("TimerCalls", v.title().c_str(), v.bins(), v.lowEdge(), v.highEdge());
  ATH_CHECK(histSvc->regHist(histPath + m_hist_timerCalls->GetName(), m_hist_timerCalls));

  return StatusCode::SUCCESS;
}

void TrigTimerSvc::handle(const Incident&)
{
  // Fill histogram with number of timer calls since last event
  long long N(0);
  for (const TrigTimer* t : m_itemList) 
    N += t->number_of_measurements();
  
  m_hist_timerCalls->Fill(N-m_timerCalls);
  m_timerCalls = N;
}

StatusCode TrigTimerSvc::finalize ( ) {

  if ( m_dumpFinalStatistics ) this->print();

  // Print basic statistics
  long long N(0);
  for (const TrigTimer* t : m_itemList) 
    N += t->number_of_measurements();

  ATH_MSG_INFO("Number of active/total timers: " << m_itemList.size() << "/" << m_allTimers.size());
  ATH_MSG_INFO("Total measurements: " << N);

  return StatusCode::SUCCESS;
}

TrigTimer* TrigTimerSvc::addItem(const std::string &itemName) {

  // Create a TrigTimer and return a pointer to it
  // If an item of that name already exists, flag error
  // by returning NULL
  
  // check against regexes
  bool active = true;
  boost::cmatch what;
  if ( ! boost::regex_match(itemName.c_str(), what, m_includeRegex) ) {
    ATH_MSG_INFO( "Timer " <<  itemName
                  << " not allowed, DOES NOT match IncludeName regex \""
                  << m_includeName << "\"");
	active = false;
  } else if ( boost::regex_match(itemName.c_str(), what, m_excludeRegex) ) {
    ATH_MSG_INFO( "Timer " <<  itemName
                  << " not allowed, MATCHES ExcludeName regex \""
                  << m_excludeName << "\"");
	active = false;
  }


  ATH_MSG_DEBUG( name() << " adding " << itemName  );
  TrigTimer* item = new TrigTimer(itemName, active);

  // this will be used to delete them at the end
  m_allTimers.push_back(item);

  // this is used to print, search ....
  if ( active )
	m_itemList.push_back(item);

  return item;
}

void TrigTimerSvc::print( ) {

  ATH_MSG_INFO( "==============================TrigTimerSvc::Print===============================" );
  unsigned int maxname=0;
  for (const TrigTimer* t : m_itemList) {
	if (t->name().length() > maxname) maxname = static_cast<int>(t->name().length());
	if (!t->propName().empty()) {
      if (t->propName().length() > maxname) maxname = static_cast<int>(t->propName().length());
	}
  }

  for (const TrigTimer* t : m_itemList) {
	std::string nam = t->name();
      
	for (unsigned int i = static_cast<int>(nam.length()); i<maxname; i++) 
      nam.insert(i," ");

	ATH_MSG_INFO( nam << "  mean(t) : " << std::setw(7) << std::fixed << std::setprecision(2) << t->mean()
                  << " ms      rms : " << std::setw(7) << std::fixed << std::setprecision(2) << t->rms() << " ms " 
                  << " N: " << std::setw(6) << std::fixed << std::setprecision(2) << t->number_of_measurements() 
                  << " Integral: " << std::setw(7) << std::fixed << std::setprecision(2) <<  t->number_of_measurements()*t->mean() );
  }
  
  ATH_MSG_INFO( "------------------------------------------------------------------------------" );
  for (const TrigTimer* t : m_itemList) {
	if (!t->propName().empty()) {
      std::string nam = t->name();
      for (unsigned int i = static_cast<int>(nam.length()); i<maxname; i++) nam.insert(i," ");
      ATH_MSG_INFO( nam << "  mean(t) : " << std::setw(12) << t->mean()<< 
                    " ms      rms : " << std::setw(12) << t->rms() << " ms " );

      nam = t->propName();
      for (unsigned int i = static_cast<int>(nam.length()); i<maxname; i++) nam.insert(i," ");
      if (t->meanPropVal() == 0) {
        ATH_MSG_INFO( nam << "  mean(N) : " << std::setw(12) );
      } else {
        ATH_MSG_INFO( nam << "  mean(N) : " << std::setw(12) << t->meanPropVal() << 
                      "   mean(t/N) : " 
                      << 1000. * t->meanTimePerObject() << " us " );
      }
	}
  }

  ATH_MSG_INFO( "==============================TrigTimerSvc::Print===============================" );
}

std::vector < TrigTimer* >::iterator TrigTimerSvc::begin( void ) {
  return m_itemList.begin();
}
std::vector < TrigTimer* >::iterator TrigTimerSvc::end( void ){
  return m_itemList.end();
}

  
