// this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigTimerSvc.cxx,v 1.12 2009-04-15 14:00:22 tbold Exp $
// $Name: not supported by cvs2svn $
#include "TrigTimeAlgs/TrigTimerSvc.h"
#include <algorithm>
#include <string>
#include <vector>


TrigTimerSvc::TrigTimerSvc( const std::string& name, ISvcLocator* svcloc ) 
    : base_class(name, svcloc),
      m_includeName(".+"),     // by default all is allowed
      m_excludeName("()") {    // by default empty string is disallowed
    declareProperty("IncludeName", m_includeName, "Regex matching timer names, if do not match a fake(disabled) timer will be given out.");
    declareProperty("ExcludeName", m_excludeName, "Regex matching timer names, if matches a fake(disabled) timer will be given out.");
    declareProperty("dumpFinalStatistics" , m_dumpFinalStatistics=false, "Make extended printout of timers during finalization");
    //declareInterface<ITrigTimerSvc>(this);
}

TrigTimerSvc::~TrigTimerSvc() {
    std::vector < TrigTimer* >::iterator it =  m_allTimers.begin();
    for (; it != m_allTimers.end(); it++) {
	delete (*it);
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
    ATH_MSG_INFO( name() << ": Start of run initialisation" );
    
    ATH_CHECK( setProperties() );

    // compile regexes
    m_includeRegex = boost::regex(m_includeName);
    m_excludeRegex = boost::regex(m_excludeName);

    return StatusCode::SUCCESS;
}


StatusCode TrigTimerSvc::finalize ( ) {
  if ( m_dumpFinalStatistics ) {
    ATH_MSG_INFO( name() << ": End of Run Statistics" );
    this->print();
  }
  // should delete timers? 
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
    std::vector < TrigTimer* >::iterator it =  m_itemList.begin();
    for (; it != m_itemList.end(); it++) {
	if ((*it)->name().length() > maxname) maxname = static_cast<int>((*it)->name().length());
	if (!(*it)->propName().empty()) {
	    if ((*it)->propName().length() > maxname) maxname = static_cast<int>((*it)->propName().length());
	}
    }

    it =  m_itemList.begin();
    for (; it != m_itemList.end(); it++) {
	std::string nam = (*it)->name();
      
	for (unsigned int i = static_cast<int>(nam.length()); i<maxname; i++) 
	    nam.insert(i," ");

	ATH_MSG_INFO( nam << "  mean(t) : " << std::setw(7) << std::fixed << std::setprecision(2) << (*it)->mean()
                      << " ms      rms : " << std::setw(7) << std::fixed << std::setprecision(2) << (*it)->rms() << " ms " 
                      << " N: " << std::setw(6) << std::fixed << std::setprecision(2) << (*it)->number_of_measurements() << " Integral: " << std::setw(7) << std::fixed << std::setprecision(2) <<  (*it)->number_of_measurements()*(*it)->mean() );
    }
  
    ATH_MSG_INFO( "------------------------------------------------------------------------------" );
    it =  m_itemList.begin();
    for (; it != m_itemList.end(); it++) {
	if (!(*it)->propName().empty()) {
	    std::string nam = (*it)->name();
	    for (unsigned int i = static_cast<int>(nam.length()); i<maxname; i++) nam.insert(i," ");
	    ATH_MSG_INFO( nam << "  mean(t) : " << std::setw(12) << (*it)->mean()<< 
                          " ms      rms : " << std::setw(12) << (*it)->rms() << " ms " );

	    nam = (*it)->propName();
	    for (unsigned int i = static_cast<int>(nam.length()); i<maxname; i++) nam.insert(i," ");
	    if ((*it)->meanPropVal() == 0) {
              ATH_MSG_INFO( nam << "  mean(N) : " << std::setw(12) );
	    } else {
              ATH_MSG_INFO( nam << "  mean(N) : " << std::setw(12) << (*it)->meanPropVal() << 
		    "   mean(t/N) : " 
                            << 1000.*(*it)->meanTimePerObject() << " us " );
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

  
