/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaMonitoring/AthenaMonManager.h"
#include "AthenaMonitoring/ManagedMonitorToolTest.h"

#include "GaudiKernel/StatusCode.h"
#include "StoreGate/StoreGateSvc.h"
#include "EventInfo/TriggerInfo.h"
#include "EventInfo/EventInfo.h"
#include <EventInfo/EventID.h>

#include "TROOT.h"
#include "TH1.h"
#include "TH2.h"
#include "TTree.h"
#include "TNtuple.h"
#include "TGraph.h"
#include "TProfile.h"

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <memory>

#include "TrigHLTMonitoring/IHLTMonTool.h"
#include "TrigHLTMonitoring/HLTMonException.h"
#include "GaudiKernel/GaudiException.h"

#include "LumiCalc/CoolQuery.h"   
                                        
using namespace std;

IHLTMonTool::IHLTMonTool(const std::string & type, const std::string & myname, const IInterface* parent)
  : ManagedMonitorToolBase(type, myname, parent), m_log(0), m_cafonly(0),
    m_storeGate("StoreGateSvc",myname),
    m_inputMetaStore("StoreGateSvc/InputMetaDataStore",myname),
    m_configsvc("TrigConf::TrigConfigSvc/TrigConfigSvc",myname), //pickup previously configured configSvc from svcMgr (same as TDT)
    m_tdthandle("Trig::TrigDecisionTool/TrigDecisionTool"),
    m_configTool("") //defaults to empty
{
  declareProperty("CAFonly", m_cafonly);
  declareProperty("TDT", m_tdthandle);
  declareProperty("TrigConfigTool",m_configTool);
  declareProperty("IgnoreTruncationCheck",m_ignoreTruncationCheck=false); //Default check for truncate HLTResult
  m_log = new MsgStream(msgSvc(), name());
}

IHLTMonTool::~IHLTMonTool() {
  // delete MonGroups
  for (std::map<std::string, MonGroup *>::iterator i = m_group.begin(); i != m_group.end(); i++) {
    if (i->second)
      delete i->second;
  }
  if (m_log)
    delete m_log;
}

StatusCode IHLTMonTool::initialize() {

  // retrieve the trigger decision tool
  ATH_CHECK(  m_tdthandle.retrieve() );

  // After retrieve enable Expert methods
  getTDT()->ExperimentalAndExpertMethods()->enable();

  ATH_CHECK( m_storeGate.retrieve() );
  ATH_CHECK( m_inputMetaStore.retrieve() );

  if(m_configTool.empty()){
      ATH_MSG_INFO("No TrigConfigTool provided, using TrigConfigSvc (default)");
      StatusCode sc = m_configsvc.retrieve();
      if ( sc.isFailure() ) {
          ATH_MSG_WARNING("Could not retrieve TrigConfigSvc - trying TrigConf::xAODConfigTool");
          m_configTool = ToolHandle<TrigConf::ITrigConfigTool>("TrigConf::xAODConfigTool");
          ATH_CHECK( m_configTool.retrieve() );
      }
  }
  else {
    ATH_CHECK( m_configTool.retrieve() );
  }

  ATH_CHECK( ManagedMonitorToolBase::initialize() );

  StatusCode sc;
  try {
    sc = init();
  } catch(const GaudiException &e) {
    ATH_MSG_ERROR("Exception thrown: " << e.message());
    return StatusCode::FAILURE;
  } catch(...) {
    ATH_MSG_ERROR("init() from child tool has failed by unknown reasons");
    return StatusCode::FAILURE;
  }

  return sc;
}

int IHLTMonTool::getL1info() {
  
  int lvl1info = 0;
  
  const EventInfo* EventInfo(0);
  StatusCode sc = evtStore()->retrieve(EventInfo);
  if (sc.isFailure())  {
    return lvl1info;
  } else {
    const TriggerInfo* triggerInfo = EventInfo->trigger_info();
    if (triggerInfo) {
      return triggerInfo->level1TriggerType();
    } else {
      return lvl1info;
    }
  }
}



int IHLTMonTool::getRunNr() {

  int runnr = -1;
  const EventInfo* EventInfo(0);
  StatusCode sc = evtStore()->retrieve(EventInfo);
  if (sc.isFailure())  {
    return runnr;
  } else {
    if ( EventInfo->event_ID()) {
      //lumiBlockNumber = (int)  constEventInfo->event_ID()->lumi_block();
      runnr = (int) EventInfo->event_ID()->run_number();
    } else {
      ATH_MSG_WARNING("missing event_ID");
    }
    return runnr;
  }
}

int IHLTMonTool::getEventNr() {
  const EventInfo* EventInfo(0);
  StatusCode sc = evtStore()->retrieve(EventInfo);
  int eventnr = -1;
  if (sc.isFailure())  {
    return eventnr;
  } else {
    if ( EventInfo->event_ID()) {
      eventnr = (int) EventInfo->event_ID()->event_number();
    } else {
      ATH_MSG_WARNING("missing event_ID");
    }
    return eventnr;
  }
}

int IHLTMonTool::getLumiBlockNr() {
  const EventInfo* EventInfo(0);
  StatusCode sc = evtStore()->retrieve(EventInfo);
  int lbnr = -1;
  if (sc.isFailure())  {
    return lbnr;
  } else {
    if ( EventInfo->event_ID()) {
      lbnr = (int) EventInfo->event_ID()->lumi_block();
    } else {
      ATH_MSG_WARNING("missing event_ID()");
    }
    return lbnr;
  }
}

void IHLTMonTool::setCurrentMonGroup(const std::string &monGroup) {
  // Check if it exists!
  m_currentMonGroup = monGroup;
}

void IHLTMonTool::addMonGroup(MonGroup *m) {
  if (!m)
    throw GaudiException("Mongroup pointer is NULL", "HLTMonitoring",StatusCode::FAILURE);
  // Overwrite items!
  std::map<std::string, MonGroup *>::iterator i;
  if ((i = m_group.find(m->system())) != m_group.end()) {
    m_group.erase(i);
  }
  
  m_group.insert(std::pair<std::string, MonGroup *>(m->system(), m));
  setCurrentMonGroup(m->system());
}

void IHLTMonTool::addHistogram(TH1 *h, const std::string &monGroup) {
  if (!h)
    throw GaudiException("Histogram pointer is NULL","HLTMonitoring",StatusCode::FAILURE);
  //take ownership of the pointer.. in case except is thrown hPtr will delete h
  //std::auto_ptr<TH1> hPtr = std::auto_ptr<TH1>(h);
  
  std::string theMonGroup;
  
  std::map<std::string, MonGroup *>::iterator groupItr;
  if (monGroup == "") {
    theMonGroup = m_currentMonGroup;
  } else {
    theMonGroup = monGroup;
  }
  
  groupItr = m_group.find(theMonGroup);
  if (groupItr == m_group.end())
    throw GaudiException("Adding histogram to Mongroup failed","HLTMonitoring",StatusCode::FAILURE);

  StatusCode sc;
  sc = groupItr->second->regHist(h);
  if (sc.isFailure()) {
    throw GaudiException("Failure registering histogram","HLTMonitoring",StatusCode::FAILURE);
  }

  std::string name = groupItr->second->system();
  if (name.at(name.size()-1) != '/')
    name += '/';
  name += h->GetName();
  
  // Overwrite items!
  std::map<std::string, TH1 *>::iterator i;
  if ((i = m_hist.find(name)) != m_hist.end()) {
    m_hist.erase(i);
  }
  m_hist.insert(std::pair<std::string, TH1 *>(name, h));
}

void IHLTMonTool::addHistogram(TH2 *h, const std::string &monGroup) {
  if (!h)
    throw GaudiException("Histogram pointer is NULL","HLTMonitoring",StatusCode::FAILURE);
  //take ownership of the pointer.. in case except is thrown hPtr will delete h
  //std::auto_ptr<TH2> hPtr = std::auto_ptr<TH2>(h);

  std::string theMonGroup;
  
  std::map<std::string, MonGroup *>::iterator groupItr;
  if (monGroup == "") {
    theMonGroup = m_currentMonGroup;
  } else {
    theMonGroup = monGroup;
  }
  
  groupItr = m_group.find(theMonGroup);
  if (groupItr == m_group.end())
    throw GaudiException("Adding histogram (2D) to MonGroup","HLTMonitoring",StatusCode::FAILURE);

  StatusCode sc;
  sc = groupItr->second->regHist(h);
  if (sc.isFailure()) {
    throw GaudiException("Failure registering histogram)","HLTMonitoring",StatusCode::FAILURE);
  }

  std::string name = groupItr->second->system();
  if (name.at(name.size()-1) != '/')
    name += '/';
  name += h->GetName();
  
  // Overwrite items!
  std::map<std::string, TH2 *>::iterator i;
  if ((i = m_hist2.find(name)) != m_hist2.end()) {
    m_hist2.erase(i);
  }
  m_hist2.insert(std::pair<std::string, TH2 *>(name, h));
}
void IHLTMonTool::addTree(TTree *t, const std::string &monGroup) {
  if (!t)
    throw GaudiException("Tree pointer is NULL","HLTMonitoring",StatusCode::FAILURE);

  //take ownership of the pointer.. in case except is thrown tPtr will delete t
  //std::auto_ptr<TTree> tPtr = std::auto_ptr<TTree>(t);

  std::string theMonGroup;
  
  std::map<std::string, MonGroup *>::iterator groupItr;
  if (monGroup == "") {
    theMonGroup = m_currentMonGroup;
  } else {
    theMonGroup = monGroup;
  }
  groupItr = m_group.find(theMonGroup);
  if (groupItr == m_group.end())
    throw GaudiException("adding tree to mongroup failed","HLTMonitoring",StatusCode::FAILURE);

  StatusCode sc;
  sc = groupItr->second->regTree(t);
  if (sc.isFailure()) {
    throw GaudiException("registering tree failed","HLTMonitoring",StatusCode::FAILURE);
  }
  
  std::string name = groupItr->second->system();
  if (name.at(name.size()-1) != '/')
    name += '/';
  name += t->GetName();
  
  // Overwrite items!
  std::map<std::string, TTree *>::iterator i;
  if ((i = m_tree.find(name)) != m_tree.end()) {
    m_tree.erase(i);
  }
  m_tree.insert(std::pair<std::string, TTree *>(name, t));
}

void IHLTMonTool::addGraph(TGraph *g, const std::string &monGroup) {
  if (!g)
    throw GaudiException("Graph pointer is NULL","HLTMonitoring",StatusCode::FAILURE);

  //take ownership of the pointer.. in case except is thrown gPtr will delete g
  //std::auto_ptr<TGraph> gPtr = std::auto_ptr<TGraph>(g);


  std::string theMonGroup;
  
  std::map<std::string, MonGroup *>::iterator groupItr;
  if (monGroup == "") {
    theMonGroup = m_currentMonGroup;
  } else {
    theMonGroup = monGroup;
  }
  groupItr = m_group.find(theMonGroup);
  if (groupItr == m_group.end())
    throw GaudiException("Mongroup not found","HLTMonitoring",StatusCode::FAILURE);

  StatusCode sc;
  sc = groupItr->second->regGraph(g);
  if (sc.isFailure()) {
    throw GaudiException("failed to register graph","HLTMonitoring",StatusCode::FAILURE);
  }
  
  std::string name = groupItr->second->system();
  if (name.at(name.size()-1) != '/')
    name += '/';
  name += g->GetName();
  
  // Overwrite items!
  std::map<std::string, TGraph *>::iterator i;
  if ((i = m_graph.find(name)) != m_graph.end()) {
    m_graph.erase(i);
  }
  m_graph.insert(std::pair<std::string, TGraph *>(name, g));
}

void IHLTMonTool::addProfile(TProfile *h, const std::string &monGroup) {
  
  if (!h)
    throw GaudiException("Profile pointer is NULL","HLTMonitoring",StatusCode::FAILURE);
  
  //take ownership of the pointer.. in case except is thrown hPtr will delete h
  //std::auto_ptr<TProfile> hPtr = std::auto_ptr<TProfile>(h);
  

  std::string theMonGroup;
  
  std::map<std::string, MonGroup *>::iterator groupItr;
  if (monGroup == "") {
    theMonGroup = m_currentMonGroup;
  } else {
    theMonGroup = monGroup;
  }
  
  groupItr = m_group.find(theMonGroup);
  if (groupItr == m_group.end())
    throw GaudiException("Mongroup not found)","HLTMonitoring",StatusCode::FAILURE);

  
  StatusCode sc;
  sc = groupItr->second->regHist(h);
  if (sc.isFailure()) {
    throw GaudiException("failed registering profile","HLTMonitoring",StatusCode::FAILURE);
  }
  
  std::string name = groupItr->second->system();
  if (name.at(name.size()-1) != '/')
    name += '/';
  name += h->GetName();
  
  
  // Overwrite items!
  std::map<std::string, TProfile *>::iterator i;
  if ((i = m_profile.find(name)) != m_profile.end()) {
    m_profile.erase(i);
  } 
  
  m_profile.insert(std::pair<std::string, TProfile *>(name, h)); 
  
}

void IHLTMonTool::removeHistogram(const std::string &histName, const std::string &monGroup) {
  std::string theMonGroup;
  
  std::map<std::string, MonGroup *>::iterator groupItr;
  if (monGroup == "") {
    theMonGroup = m_currentMonGroup;
  } else {
    theMonGroup = monGroup;
  }
  
  groupItr = m_group.find(theMonGroup);
  if (groupItr == m_group.end()) {
    throw GaudiException("Mongroup not found","HLTMonitoring",StatusCode::SUCCESS);
  }

  TH1 *tmpHist = hist(histName, theMonGroup);
  std::map<std::string, TH1 *>::iterator itHist;
  if (groupItr->second->deregHist(tmpHist).isFailure()) {
    throw GaudiException("failed removing histogram","HLTMonitoring",StatusCode::FAILURE);
  }

  std::string fullPath = theMonGroup;
  fullPath += "/";
  fullPath += histName;
  itHist = m_hist.find(fullPath);
  if (itHist != m_hist.end()) {
    m_hist.erase(itHist);
  }
  delete tmpHist;
}

void IHLTMonTool::removeHistogram2(const std::string &histName, const std::string &monGroup) {
  std::string theMonGroup;
  
  std::map<std::string, MonGroup *>::iterator groupItr;
  if (monGroup == "") {
    theMonGroup = m_currentMonGroup;
  } else {
    theMonGroup = monGroup;
  }
  
  groupItr = m_group.find(theMonGroup);
  if (groupItr == m_group.end()) {
    throw GaudiException("Mongroup not found","HLTMonitoring",StatusCode::FAILURE);
  }

  TH2 *tmpHist = hist2(histName, theMonGroup);
  std::map<std::string, TH2 *>::iterator itHist;
  if (groupItr->second->deregHist(tmpHist).isFailure()) {
    throw GaudiException("failure deregistering 2D histogram","HLTMonitoring",StatusCode::FAILURE);
  }

  std::string fullPath = theMonGroup;
  fullPath += "/";
  fullPath += histName;
  itHist = m_hist2.find(fullPath);
  if (itHist != m_hist2.end()) {
    m_hist2.erase(itHist);
  }
  delete tmpHist;
}

void IHLTMonTool::removeProfile(const std::string &histName, const std::string &monGroup) {
  std::string theMonGroup;
  
  std::map<std::string, MonGroup *>::iterator groupItr;
  if (monGroup == "") {
    theMonGroup = m_currentMonGroup;
  } else {
    theMonGroup = monGroup;
  }
  
  groupItr = m_group.find(theMonGroup);
  if (groupItr == m_group.end()) {
    throw GaudiException("Mongroup not found","HLTMonitoring",StatusCode::FAILURE);
  }
  
  TProfile *tmpHist = profile(histName, theMonGroup);
  std::map<std::string, TProfile *>::iterator itHist;
  if (groupItr->second->deregHist(tmpHist).isFailure()) {
    throw GaudiException("failure deregistering profile","HLTMonitoring",StatusCode::FAILURE);
  }
  
  std::string fullPath = theMonGroup;
  fullPath += "/";
  fullPath += histName;
  itHist = m_profile.find(fullPath);
  if (itHist != m_profile.end()) {
    m_profile.erase(itHist);
  }
  
  delete tmpHist;
  
}

TH1 *IHLTMonTool::hist(const std::string &histName, const std::string &monGroup) {
  std::string theMonGroup;
  
  std::map<std::string, MonGroup *>::iterator groupItr;
  if (monGroup == "") {
    theMonGroup = m_currentMonGroup;
  } else {
    theMonGroup = monGroup;
  }
  
  groupItr = m_group.find(theMonGroup);
  if (groupItr == m_group.end())
    throw GaudiException("Mongroup not found","HLTMonitoring",StatusCode::FAILURE);

  std::string name = groupItr->second->system();
  name += '/';
  name += histName;
  std::map<std::string, TH1 *>::iterator histItr = m_hist.find(name);
  if (histItr == m_hist.end())
    throw GaudiException("histogram not found","HLTMonitoring",StatusCode::FAILURE);
  if (!histItr->second)
    throw GaudiException("histogram is NULL pointer","HLTMonitoring",StatusCode::FAILURE);

  return histItr->second;
}

TH2 *IHLTMonTool::hist2(const std::string &histName, const std::string &monGroup) {
  std::string theMonGroup;
  
  std::map<std::string, MonGroup *>::iterator groupItr;
  if (monGroup == "") {
    theMonGroup = m_currentMonGroup;
  } else {
    theMonGroup = monGroup;
  }
  
  groupItr = m_group.find(theMonGroup);
  if (groupItr == m_group.end())
    throw GaudiException("mongroup not found","HLTMonitoring",StatusCode::FAILURE);

  std::string name = groupItr->second->system();
  name += '/';
  name += histName;
  std::map<std::string, TH2 *>::iterator histItr = m_hist2.find(name);
  if (histItr == m_hist2.end())
    throw GaudiException("2D histogram not found","HLTMonitoring",StatusCode::FAILURE);
  if (!histItr->second)
    throw GaudiException("2D histogram is NULL pointer","HLTMonitoring",StatusCode::FAILURE);

  return histItr->second;
}

TTree *IHLTMonTool::tree(const std::string &treeName, const std::string &monGroup) {
  std::string theMonGroup;
  std::map<std::string, MonGroup *>::iterator groupItr;
  if (monGroup == "") {
    theMonGroup = m_currentMonGroup;
  } else {
    theMonGroup = monGroup;
  }
  groupItr = m_group.find(theMonGroup);
  if (groupItr == m_group.end())
    throw GaudiException("mongroup not found","HLTMonitoring",StatusCode::FAILURE);
    
  std::string name = groupItr->second->system();
  name += '/';
  name += treeName;
  std::map<std::string, TTree *>::iterator treeItr = m_tree.find(name);
  if (treeItr == m_tree.end())
    throw GaudiException("tree not found","HLTMonitoring",StatusCode::FAILURE);
  if (!treeItr->second)
    throw GaudiException("tree is NULL pointer","HLTMonitoring",StatusCode::FAILURE);

  return treeItr->second;
}

TGraph *IHLTMonTool::graph(const std::string &graphName, const std::string &monGroup) {
  std::string theMonGroup;
  std::map<std::string, MonGroup *>::iterator groupItr;
  if (monGroup == "") {
    theMonGroup = m_currentMonGroup;
  } else {
    theMonGroup = monGroup;
  }
  groupItr = m_group.find(theMonGroup);
  if (groupItr == m_group.end())
    throw GaudiException("mongroup not found","HLTMonitoring",StatusCode::FAILURE);
    
  std::string name = groupItr->second->system();
  name += '/';
  name += graphName;
  std::map<std::string, TGraph *>::iterator graphItr = m_graph.find(name);
  if (graphItr == m_graph.end())
    throw GaudiException("grpah not found","HLTMonitoring",StatusCode::FAILURE);
  if (!graphItr->second)
    throw GaudiException("grpah is NULL pointer","HLTMonitoring",StatusCode::FAILURE);

  return graphItr->second;
}

TProfile *IHLTMonTool::profile(const std::string &histName, const std::string &monGroup) {
  std::string theMonGroup;
  
  std::map<std::string, MonGroup *>::iterator groupItr;
  if (monGroup == "") {
    theMonGroup = m_currentMonGroup;
  } else {
    theMonGroup = monGroup;
  }
  
  groupItr = m_group.find(theMonGroup);
  if (groupItr == m_group.end())
    throw GaudiException("mongroup not found","HLTMonitoring",StatusCode::FAILURE);
  
  std::string name = groupItr->second->system();
  name += '/';
  name += histName;
 
  std::map<std::string, TProfile *>::iterator histItr = m_profile.find(name);
  
  if (histItr == m_profile.end())
    throw GaudiException("grpah not found","HLTMonitoring",StatusCode::FAILURE);
  if (!histItr->second)
    throw GaudiException("grpah is NULL pointer","HLTMonitoring",StatusCode::FAILURE);
  
  return histItr->second;
  
}

TNtuple *IHLTMonTool::ntuple(const std::string &treeName, const std::string &monGroup) {
  TTree *mtree = tree(treeName, monGroup);
  TNtuple *mntuple = dynamic_cast<TNtuple *>(mtree);
  if (!mntuple) {
    std::string theMonGroup;
    std::map<std::string, MonGroup *>::iterator groupItr;
    if (monGroup == "") {
      theMonGroup = m_currentMonGroup;
    } else {
      theMonGroup = monGroup;
    }
    throw GaudiException("Can't convert tree to ntuple","HLTMonitoring",StatusCode::FAILURE);
  }
  return mntuple;
}

StatusCode IHLTMonTool::bookHistogramsRecurrent() {
// For unmanaged histograms - not used
  StatusCode sc = StatusCode::SUCCESS;
  return sc;
}

StatusCode IHLTMonTool::bookHistograms() {
  StatusCode sc = StatusCode::SUCCESS;

// LS 20 Jan 2014 - No longer need to check interval. Now handled in the ManagedMonitorTool class
//  if (isNewRun) {
//    m_hist.clear();
//    m_hist2.clear();
//    m_tree.clear();
//    m_graph.clear();
//  }

  try {
    sc = book();
    if (sc.isFailure()) {
      ATH_MSG_ERROR("book() for " << name() << ", returned StatusCode::FAILURE!");
    }
  } catch(const GaudiException &e) {
    sc = StatusCode::FAILURE;
    ATH_MSG_ERROR("Exception thrown: " << e.message());
  } catch(...) {
    sc =StatusCode::FAILURE;
    ATH_MSG_ERROR("Unknown exception caught, while booking histograms");
  }
  return sc;
}

StatusCode IHLTMonTool::fillHistograms() {
  StatusCode sc = StatusCode::SUCCESS;
  try {
    ATH_MSG_DEBUG("Running fill() for " << name());
   
    // Do not require check on truncated HLTResult 
    if(m_ignoreTruncationCheck)
        sc=fill();
    else {
    // Require non-truncated HLTResult
        if(getTDT()->ExperimentalAndExpertMethods()->isHLTTruncated()) 
            ATH_MSG_WARNING("HLTResult truncated, skip HLT T0 monitoring for this event");
        else 
            sc= fill();
    }

    if (sc.isFailure()) {
      ATH_MSG_ERROR("fill() for " << name() << ", returned StatusCode::FAILURE!");
    }
  } catch(const GaudiException &e) {
    sc = StatusCode::FAILURE;
    ATH_MSG_ERROR("Exception thrown: " << e.message());
  } catch(...) {
    sc = StatusCode::FAILURE;
    ATH_MSG_ERROR("Unknown exception caught, while filling histograms");
  }
  return sc;
}

StatusCode IHLTMonTool::procHistograms() {
  StatusCode sc = StatusCode::SUCCESS;
  try {
    sc = proc();
    if (sc.isFailure()) {
      ATH_MSG_ERROR("proc() for " << name() << ", returned StatusCode::FAILURE!");
    }
  } catch(const GaudiException &e) {
    sc = StatusCode::FAILURE;
    ATH_MSG_ERROR("Exception thrown: " << e.message());
  } catch(...) {
    sc = StatusCode::FAILURE;
    ATH_MSG_ERROR("Unknown exception caught, while processing histograms");
  }
  return sc;
}
