// this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigTimerSvc.cxx,v 1.12 2009-04-15 14:00:22 tbold Exp $
// $Name: not supported by cvs2svn $
#include <algorithm>
#include <string>
#include <vector>
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"



TrigTimerSvc::TrigTimerSvc( const std::string& name, ISvcLocator* svcloc ) 
    : AthService(name, svcloc),
      m_includeName(".+"),     // by default all is allowed
      m_excludeName("()") {    // by default empty string is disallowed
    declareProperty("IncludeName", m_includeName, "Regex matching timer names, if do not match a fake(disabled) timer will be given out.");
    declareProperty("ExcludeName", m_excludeName, "Regex matching timer names, if matches a fake(disabled) timer will be given out.");
    declareProperty("dumpFinalStatistics" , m_dumpFinalStatistics=false, "Make extended printout of timers during finalization");
    //    declareInterface<ITrigTimerSvc>(this);

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
    MsgStream log(messageService(), name());
    log << MSG::INFO << name() << ": Start of run initialisation" << endreq;

    
    // Set my own properties
    StatusCode sc = setProperties();  
    if ( sc.isFailure() ) return sc;
  
    // compile regexes
    m_includeRegex = boost::regex(m_includeName);
    m_excludeRegex = boost::regex(m_excludeName);

    return sc;
}


StatusCode TrigTimerSvc::finalize ( ) {
  if ( m_dumpFinalStatistics ) {
    MsgStream log(messageService(), name());
    log << MSG::INFO << name() << ": End of Run Statistics" << endreq;
    this->print();
  }
    // should delete timers? 
    return StatusCode::SUCCESS;
}

TrigTimer* TrigTimerSvc::addItem(const std::string &itemName) {

    // Create a TrigTimer and return a pointer to it
    // If an item of that name already exists, flag error
    // by returning NULL
  
    MsgStream log(messageService(), name()); 

  
    // check against regexes
    bool active = true;
    boost::cmatch what;
    if ( ! boost::regex_match(itemName.c_str(), what, m_includeRegex) ) {
	log << MSG::INFO << "Timer " <<  itemName
	    << " not allowed, DOES NOT match IncludeName regex \""
	    << m_includeName << "\""<< endreq;
	active = false;
    } else if ( boost::regex_match(itemName.c_str(), what, m_excludeRegex) ) {
	log << MSG::INFO << "Timer " <<  itemName
	    << " not allowed, MATCHES ExcludeName regex \""
	    << m_excludeName << "\""<< endreq;
	active = false;
    }


    log << MSG::DEBUG << name() << " adding " << itemName  << endreq;
    TrigTimer* item = new TrigTimer(itemName, active);

    // this will be used to delete them at the end
    m_allTimers.push_back(item);

    // this is used to print, search ....
    if ( active )
	m_itemList.push_back(item);

    return item;
}

void TrigTimerSvc::print( ) {

    MsgStream log(messageService(), name());

    log << MSG::INFO << "==============================TrigTimerSvc::Print===============================" << endreq;
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

	log << MSG::INFO << nam << "  mean(t) : " << std::setw(7) << std::fixed << std::setprecision(2) << (*it)->mean()
	    << " ms      rms : " << std::setw(7) << std::fixed << std::setprecision(2) << (*it)->rms() << " ms " 
	    << " N: " << std::setw(6) << std::fixed << std::setprecision(2) << (*it)->number_of_measurements() << " Integral: " << std::setw(7) << std::fixed << std::setprecision(2) <<  (*it)->number_of_measurements()*(*it)->mean() << endreq;
    }
  
    log << MSG::INFO << "------------------------------------------------------------------------------" << endreq;
    it =  m_itemList.begin();
    for (; it != m_itemList.end(); it++) {
	if (!(*it)->propName().empty()) {
	    std::string nam = (*it)->name();
	    for (unsigned int i = static_cast<int>(nam.length()); i<maxname; i++) nam.insert(i," ");
	    log << MSG::INFO << nam << "  mean(t) : " << std::setw(12) << (*it)->mean()<< 
		" ms      rms : " << std::setw(12) << (*it)->rms() << " ms " << endreq;

	    nam = (*it)->propName();
	    for (unsigned int i = static_cast<int>(nam.length()); i<maxname; i++) nam.insert(i," ");
	    if ((*it)->meanPropVal() == 0) {
		log << MSG::INFO << nam << "  mean(N) : " << std::setw(12) << endreq;
	    } else {
		log << MSG::INFO << nam << "  mean(N) : " << std::setw(12) << (*it)->meanPropVal() << 
		    "   mean(t/N) : " 
		    << 1000.*(*it)->meanTimePerObject() << " us " << endreq; 
	    }
	}
    }

    log << MSG::INFO << "==============================TrigTimerSvc::Print===============================" << endreq;
}

std::vector < TrigTimer* >::iterator TrigTimerSvc::begin( void ) {
    return m_itemList.begin();
}
std::vector < TrigTimer* >::iterator TrigTimerSvc::end( void ){
    return m_itemList.end();
}

  
