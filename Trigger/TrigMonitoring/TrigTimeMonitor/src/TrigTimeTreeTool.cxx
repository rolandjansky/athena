/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ThreadGaudi.h"

#include "TrigTimeAlgs/TrigTimer.h"
#include "TrigTimeAlgs/ITrigTimerSvc.h"

#include "TrigTimeMonitor/TrigTimeTreeEntry.h"
#include "TrigTimeMonitor/TrigTimeTreeTool.h"

#include <algorithm>
#include <TTree.h>

TrigTimeTreeTool::TrigTimeTreeTool(const std::string & type, const std::string & name,
				       const IInterface* parent) 
    :  MonitorToolBase(type, name, parent), 
       m_path("/EXPERT/TIMER_NTUPLES/"),
       m_localTree(false),
       m_doStamps(false)

{
    declareInterface<IMonitorToolBase>(this);
    declareProperty("BookingDir", m_path);
    declareProperty("Key", m_key);
    declareProperty("DoStamps", m_doStamps);
}


TrigTimeTreeTool::~TrigTimeTreeTool() {
    
}

StatusCode TrigTimeTreeTool::initialize() {
    return StatusCode::SUCCESS;    
}

StatusCode TrigTimeTreeTool::bookHists() {

  // Cleanup before re-booking
  m_entries.clear();
  
  ServiceHandle<ITrigTimerSvc> timerSvc("TrigTimerSvc",name());  
  ATH_CHECK(timerSvc.retrieve());
  ServiceHandle<ITHistSvc> rootHistSvc("THistSvc",name());  
  ATH_CHECK(rootHistSvc.retrieve());

  // find out for whom we are running (i.e. Algo)    
  const Algorithm *parentAlg = dynamic_cast<const Algorithm*>(parent());
  if ( parentAlg ) 
	m_parentName = getGaudiThreadGenericName(parentAlg->name());

  std::string treeName = m_path + m_parentName;
  // try to get global tree  make my own tree if can not get global
  if ( rootHistSvc->getTree(m_path, m_tree).isFailure() ) {
	
	m_tree = new TTree ( m_parentName.c_str(), "timing ntuple");    
	m_localTree = true;
    
	if ( rootHistSvc->regTree(treeName, m_tree).isFailure() ) {
      ATH_MSG_WARNING("Unable to register TTree");
	}

  } else {     
    ATH_MSG_DEBUG("Attached info to global tree");
	m_localTree = false;
  }
  
  
  // loop over all the timers and make a mediating 
  // object beetween them and ntuple
  // namely TTreeEntry
  std::vector < TrigTimer* >::iterator tIter;
  for ( tIter = timerSvc->begin(); tIter != timerSvc->end(); ++tIter ){
	if ( selected((*tIter)->name()) ) {
      
      TrigTimeTreeEntry *etemp = new TrigTimeTreeEntry(*tIter, m_doStamps);
      m_entries.push_back(etemp);
      
      // book it in n-tuple
      if ( etemp->addToTree(m_tree, m_branches).isFailure() ) {
		ATH_MSG_WARNING("Unable to add the timomg info branch " 
                        << (*tIter)->name() << "to the tree");
      }
	}
  }
  return  StatusCode::SUCCESS;
}

StatusCode TrigTimeTreeTool::fillHists() {

  ATH_MSG_DEBUG(" execHLTAlgorithm");
  
  std::vector<TrigTimeTreeEntry *>::iterator teIter;
  for (  teIter = m_entries.begin(); teIter != m_entries.end(); ++teIter ) {
	(*teIter)->fill();
  }
  
  if ( m_localTree )
	m_tree->Fill();
    
  return  StatusCode::SUCCESS;
}

StatusCode TrigTimeTreeTool::finalHists() {
  return  StatusCode::SUCCESS;
}


bool TrigTimeTreeTool::selected(const std::string& name) {
  if (m_key.empty()) {
	// no specific Keys are selected ... record
	// then all the timers which parent algo registered
	if ( ! m_parentName.empty() ) {
      if ( name.find(m_parentName) == 0 ) // name starts from algo name
		return true;
	}
	return false;
  }
  
  // exact match (maybe consider regex here)
  std::vector<std::string>::const_iterator key 
	= find(m_key.begin(), m_key.end(), name);
  
  if ( key != m_key.end() )
	return true;
  return false;
}




