/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
//  Customization of ManagedMonitorToolBase fro Trigger         
//  	    
//
// ********************************************************************

#include "GaudiKernel/ITHistSvc.h"
#include "TrigMonitorBase/TrigMonitorToolBase.h"
#include "TrigMonitorBase/TrigLBNHist.h"

#include <boost/lexical_cast.hpp>
#include <iostream>


TrigMonitorToolBase::TrigMonitorToolBase(const std::string & type, 
					 const std::string & name,
					 const IInterface* parent)
  : base_class(type, name, parent),
    m_histSvc("THistSvc", name), // there is somthing fish
    m_execs(0),
    m_lbnHistoryDepth(0)
{
  //  declareInterface<IMonitorToolBase>(this);
  declareProperty("Scaler", m_prescale=1, "Defines how often call the tool");
  declareProperty("LBNHistoryDepth", m_lbnHistoryDepth=2, "Defines after how many LBNs we should drop histogram");
  declareProperty("LBNHistoryGroup", m_lbnHistoryGroup=1, "Defines how many LBNs we should be grouped together in the histogram");
}


TrigMonitorToolBase::~TrigMonitorToolBase()
{}


StatusCode TrigMonitorToolBase::initialize() {
  ATH_CHECK( m_histSvc.retrieve() );
  return StatusCode::SUCCESS;
}


StatusCode TrigMonitorToolBase::setupOutputStreams(std::vector<std::string> ) { 
  return StatusCode::SUCCESS;
}


std::string TrigMonitorToolBase::level2string( Level l ) {
  switch (l){
  case debug:
    return "/DEBUG/";
    break;
  case expert:
    return "/EXPERT/";
    break;
  case shift:
    return "/SHIFT/";
    break;
  case express:
    return "/EXPRESS/";
    break;
  case runsum:
    return "/RUNSUM/";
    break;
  }
  return "/UNSPECIFIED_NONE_OF_EXPERT_DEBUG_SHIFT_EXPRESS_RUNSUM/";
}


bool TrigMonitorToolBase::preSelector() {
  m_execs++;
  if (m_execs >= m_prescale) {
    m_execs = 0;
    return false; //if ths is false the tool will be run
  }
  return true;
}


/////////////////////////////////////////////////////////////////////////
// code form monitoring group
// 

TrigMonitorToolBase::TrigMonGroup::TrigMonGroup(TrigMonitorToolBase* tool, 
						const std::string& algo, 
						Level l ) 
  : m_tool(tool), m_algo(algo), m_level(l) { }

StatusCode TrigMonitorToolBase::TrigMonGroup::regHist(TH1* h) {
  return m_tool->m_histSvc->regHist(m_tool->level2string(m_level)+m_algo+"/"+h->GetName(), h);
}


StatusCode TrigMonitorToolBase::TrigMonGroup::regHist(ITrigLBNHist* h) {
  h->setDepth(m_tool->m_lbnHistoryDepth);
  h->setGroup(m_tool->m_lbnHistoryGroup);

  h->setITHistSvc(&*(m_tool->m_histSvc));
  h->setPath(m_tool->level2string(m_level)+m_algo);
  h->reg();
  return StatusCode::SUCCESS;
}


StatusCode TrigMonitorToolBase::TrigMonGroup::deregHist(TH1* h) {
  return m_tool->m_histSvc->deReg(h);
}





