/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigTimeAlgs/TrigTimerSvc.h"
#include "TrigTimeAlgs/TrigTimer.h"

#include "TrigTimeMonitor/TrigTimeTreeEntry.h"
#include <TTree.h>


TrigTimeTreeEntry::TrigTimeTreeEntry ( TrigTimer* timer,  bool doStamps ) 
    :     m_timer(timer),
	  m_doStamps(doStamps) {    
}

void TrigTimeTreeEntry::fill() {
    m_bData.timerValue = m_timer->elapsed();
    if ( !m_timer->propName().empty() ) 
	m_bData.propValue = m_timer->propVal();
    if (m_doStamps) {
	m_absData.startTime = m_timer->getStartTime();
	m_absData.stopTime = m_timer->getStopTime();
    }
}

// booking ( property and the timer value sit in separate branches )
StatusCode TrigTimeTreeEntry::addToTree(TTree *tree, std::vector<TBranch*>& branches) {
    std::string name = m_timer->name();
    TBranch *b;
    replace(name.begin(), name.end(), '.', '_');
    replace(name.begin(), name.end(), ':', '_');

    if ( m_timer->propName() != "" ) {
	std::string pname = m_timer->propName();
	replace(pname.begin(), pname.end(), '.', '_');	

	b = tree->Branch(name.c_str(), &m_bData, 
			 ("elapsed/F:"+pname+"/I").c_str());
	if ( b )
	    branches.push_back(b);
	else
	    return StatusCode::FAILURE;   	    
    } else {
	b = tree->Branch(name.c_str(), &m_bData, 
			 "elapsed/F");
	if ( b )
	    branches.push_back(b);
	else
	    return StatusCode::FAILURE;   	    
    }

    // book a branch for absolute timers
    if (m_doStamps) {
	b = tree->Branch((name+"_absolute").c_str(), &m_absData, 
			 "start_sec/i:start_usec/i:stop_sec/i:stop_usec/i");
	if ( b )
	    branches.push_back(b);
	else
	    return StatusCode::FAILURE;   	    
    }

    return StatusCode::SUCCESS;
}

