/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "GaudiKernel/IJobOptionsSvc.h"
#include "AthenaMonitoring/AthenaMonManager.h"
#include "AthenaMonitoring/ManagedMonitorToolTest.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/IToolSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "xAODEventInfo/EventInfo.h"

#include "TROOT.h"
#include "TH1.h"
#include "TH2.h"
#include "TGraph.h"
#include "TTree.h"
#include "TProfile.h"
#include "TProfile2D.h"

#include <vector>
#include <iostream>
#include <fstream>
#include <string>

//for lumi
#include "LumiBlockComps/CoolQuery.h"

#include "LumiBlockComps/ILumiCalcSvc.h"
#include "LumiBlockData/LumiBlockCollection.h"
#include "AthenaKernel/IOVRange.h"
#include "AthenaKernel/IOVTime.h"


#include "egammaPerformance/IEgammaMonTool.h"
#include "egammaPerformance/egammaMonException.h"

using namespace std;

IEgammaMonTool::IEgammaMonTool(const std::string & type, const std::string & myname, const IInterface* parent)
  : ManagedMonitorToolBase(type, myname, parent), m_log(0) {
  m_log = new MsgStream(msgSvc(), name());
}

IEgammaMonTool::~IEgammaMonTool() {
  // delete MonGroups
  for (std::map<std::string, MonGroup *>::iterator i = m_group.begin(); i != m_group.end(); i++) {
    if (i->second)
      delete i->second;
  }
  // Someone else seems to delete the ones below latter on!!!
  // Don't delete them here!
  /*
  // delete trees
  for (std::map<std::string, TTree *>::iterator i = m_tree.begin(); i != m_tree.end(); i++) {
    if (i->second)
      delete i->second;
  }
  // delete TH1s
  for (std::map<std::string, TH1 *>::iterator i = m_hist.begin(); i != m_hist.end(); i++) {
    if (i->second)
      delete i->second;
  }
  // delete TH2s
  for (std::map<std::string, TH2 *>::iterator i = m_hist2.begin(); i != m_hist2.end(); i++) {
    if (i->second)
      delete i->second;
  }
  */
  if (m_log)
    delete m_log;
}

StatusCode IEgammaMonTool::initialize() {
  StatusCode sc;

  sc = service("StoreGateSvc", m_storeGate);
  if(sc.isFailure()) {
    ATH_MSG_FATAL( "Unable to locate Service StoreGateSvc" );
    return sc;
  }
 
  sc = service("THistSvc", m_histsvc);
  if( sc.isFailure() ) {
    ATH_MSG_FATAL( "Unable to locate Service THistSvc" );
    return sc;
  }

  sc = ManagedMonitorToolBase::initialize();
  if (sc.isFailure())  {
    ATH_MSG_ERROR( "Unable to call ManagedMonitoringToolBase::initialize" );
    return sc;
  }

  try {
    sc = init();
  } catch(const egammaMonException &e) {
    ATH_MSG_ERROR( "Exception thrown: " << e.msg() );
    return StatusCode::FAILURE;
  } catch(...) {
    ATH_MSG_ERROR( "init() from child tool has failed by unknown reasons" );
    return StatusCode::FAILURE;
  }

  return sc;
}

int IEgammaMonTool::getL1info() {
  
  const xAOD::EventInfo* evtInfo(0);
  StatusCode sc = m_storeGate->retrieve(evtInfo);
  if (sc.isFailure())  {
    return 0;
  } else {
    return evtInfo->level1TriggerType();
  }
}




int IEgammaMonTool::getRunNr() {

  const xAOD::EventInfo* evtInfo(0);
  StatusCode sc = m_storeGate->retrieve(evtInfo);
  if (sc.isFailure())  {
    ATH_MSG_WARNING("missing event_ID" );
    return -1;
  } else {
    return evtInfo->runNumber();
  }
}

int IEgammaMonTool::getEventNr() {
  const xAOD::EventInfo* evtInfo(0);
  StatusCode sc = m_storeGate->retrieve(evtInfo);

  if (sc.isFailure())  {
    ATH_MSG_WARNING("missing event_ID" );
    return -1;
  } else {
    return evtInfo->eventNumber();
  }
}

int IEgammaMonTool::getLumiBlockNr() {
  const xAOD::EventInfo* evtInfo(0);
  StatusCode sc = m_storeGate->retrieve(evtInfo);
  if (sc.isFailure())  {
    return -1;
  } else {
    return evtInfo->lumiBlock();
  }
}
 
//GYS. In the case the event is flagged bad due to LAr noise or LAr data error this is true
int IEgammaMonTool::hasBadLar() {

const xAOD::EventInfo* event_info;
StatusCode sc = m_storeGate->retrieve( event_info );
if (sc.isFailure()) {
  ATH_MSG_WARNING("Could not get Lar event info!");
 }

 xAOD::EventInfo::EventFlagErrorState error_state = event_info->errorState(xAOD::EventInfo::LAr);

// Only removing events with Error not with Warning
//if (error_state==xAOD::EventInfo::Warning)
//  {
//   ATH_MSG_DEBUG("LAr event noisy");
//   return 1;
//  }

 if (error_state==xAOD::EventInfo::Error)
   {
     ATH_MSG_DEBUG("LAr event data integrity error");
     return 1;
   }

 ATH_MSG_DEBUG("No LAr noise");
return 0;

}


//Getting Online lumi info from lumiblocks
double IEgammaMonTool::getLumi(std::vector<string> m_triggers, int LBstart, int LBend, std::vector<double>& m_lumivec) {
//extract the online luminosity from COOL

//choosing online or offline lumi depends on the file that is used.

m_lumionl="COOLONL_TRIGGER/";
m_lumioff="COOLOFL_TRIGGER/";
m_mc_db="OFLP200";
m_data_db="COMP200";
m_parlumiestfolder = "/TRIGGER/OFLLUMI/LBLESTOFL";
m_parlvl1lblbfolder = "/TRIGGER/LUMI/LBLB";// for time information
m_lumitag = "OflLumi-7TeV-002";// for offline:  OflLumi_CosmicFake, OflLumi_TopMix
m_mc = false;
m_lbcollname = "LumiBlocks";

// by default we use the "offline data" database name
lumi_database = m_lumioff + m_data_db;
trig_database = m_trigger + m_data_db;
m_lumichannel = 998;
m_lumimethod = "ATLAS_PREFERRED";// offline channels: ATLAS_PREFERRED, OflLumi_Fake0, OflLumi_Fake:, TopMixLumi 

//the triggers we get from the option file and loop over then to get the total lumi.
//must not be overlapping triggers however
std::string triggerchain = "";

//Get the luminosity from 0 to the lumiblock of this event
int RunNumber=this->getRunNr();

//take current lumiblock if no start and end are given
if (!LBstart) LBstart = this->getLumiBlockNr()-1;//previous lumiblock
if (!LBend) LBend = this->getLumiBlockNr();

//fill the lumiblock collection, we dont have any good runs list in online, only offline...
LumiBlockCollection* iovc = new LumiBlockCollection();
iovc->push_back(new IOVRange(IOVTime(RunNumber, LBstart), IOVTime(RunNumber,LBend)));

//fill the lumi vector for each trigger consecutively
m_lumivec.clear();
  
for( std::vector<string>::const_iterator it = m_triggers.begin(); it != m_triggers.end(); ++it){
  triggerchain = *it;

  //this code copy/pasted from lumicalculator, since there is no good API

  CoolQuery * cq_lumi = NULL;
  CoolQuery * cq_trigger = NULL;
  ATH_MSG_INFO( "Luminosity database: " << lumi_database );
  ATH_MSG_INFO( "Trigger database: " << trig_database );

  cq_lumi = new CoolQuery(lumi_database, triggerchain);
  if (!cq_lumi->openDbConn()) {
    delete cq_lumi;
    return -1.;
  }

  cq_trigger = new CoolQuery(trig_database, triggerchain);
  if (!cq_trigger->openDbConn()) {
    delete cq_trigger;
    return -1.;
  }


  // initialize
  m_totalL = 0.;
  m_instLumi = 0.;
  m_intLumi = 0.;
  m_deltaT = 0.;
  m_lbstart = 0;
  m_lbstop = 0;
  m_lbstart_prev = 0;
  m_lbstop_prev = 0;
  m_clumiblocknbr = 0;
  m_clumiblocknbrend =0;

  std::set<unsigned long> lbrunset;
  lbrunset.clear();

  // Figure out trigger level  
  m_triglevel = cq_lumi->getTriggerLevel(triggerchain);
  if (m_triglevel == 0){
    // Check if on purpose, otherwise, give a warning
    if (triggerchain == "None") {
      ATH_MSG_INFO ("No trigger specified!" );
    } else {
      ATH_MSG_INFO ("Invalid trigger: [" << triggerchain << "] - will proceed with no trigger defined!");
    }
  }


  // do main LumiBlock loop
  //==============================

 
  for(LumiBlockCollection::const_iterator it = iovc->begin(); it != iovc->end(); ++it){
    const IOVRange * iovr = (*it);
    
    // Bookkeeping temporary results
    m_runnbr = iovr->start().run();
    m_lbstart = iovr->start().event();
    m_lbstop = iovr->stop().event();

    // Look for duplicate run/LB 
    if(m_lbstart_prev == m_lbstart && m_lbstop_prev == m_lbstop && m_runnbr_prev == m_runnbr){
      ATH_MSG_DEBUG("Skipping multiply stored IOVRange: [" << m_lbstart << "-" << m_lbstop << "]");
      continue;
    }

    // Update DB for this IOV range
    cq_trigger->setIOV(iovr->start().re_time(), iovr->stop().re_time());
    cq_lumi->setIOV(iovr->start().re_time(), iovr->stop().re_time());

 
 
    // Fetch up ALL info from COOL ( Note: Prescales must be here since it can change on LumiBlock boundaries )
    //=========================================================================================================
    // UTC nanoseconds since 1970
    std::map<cool::ValidityKey, cool::UInt63> L1starttime_map = cq_trigger->getObjMapFromFolderAtChan<cool::UInt63>("StartTime", m_parlvl1lblbfolder, 0);
    // UTC nanoseconds since 1970
    std::map<cool::ValidityKey, cool::UInt63> L1endtime_map = cq_trigger->getObjMapFromFolderAtChan<cool::UInt63>("EndTime", m_parlvl1lblbfolder, 0);
    //std::map<cool::ValidityKey, cool::UInt63>::iterator L1starttime_map_itr = L1starttime_map.begin();
    //std::map<cool::ValidityKey, cool::UInt63>::iterator L1endtime_map_itr = L1endtime_map.begin();

    std::map<cool::ValidityKey, float> OnlLumi = cq_lumi->getLumiIterator<float>("LBAvInstLumi", m_parlumiestfolder, m_lumitag, Lumiid); // Don't forget to change last parameter!
    std::map<cool::ValidityKey,float>::iterator itOL = OnlLumi.begin();

    if(m_mc == false) {
    
      for(; itOL != OnlLumi.end(); ++itOL){

        // Current ValidityKey:
        cool::ValidityKey currentVK = itOL->first;
          
        // Current IOVTime
        IOVTime curIOV(itOL->first >> 32, itOL->first & 0xFFFFFFFF); 
          
        // given in units of ub^{-1} = 10^{30} cm^{-2}
        m_instLumi = itOL->second;
        
        m_deltaT = (L1endtime_map.find(currentVK)->second - L1starttime_map.find(currentVK)->second)/1.0e+09;
          
	//store the luminosity for the whole run at once, so we only need one DB access
	m_lumivec.push_back(m_deltaT*m_instLumi);

      } // End of loop over lumi blocks

    } else { //if mc == true

      for(; itOL != OnlLumi.end(); ++itOL){

	// Fake luminosity for the MC
	m_lumivec.push_back(1.);

      } // for(; itOL...
    } // if m_mc == false

  } // end lb loop
 
 
   delete cq_trigger;
   delete cq_lumi;

}//loop over triggers
   delete iovc;

//m_lumivec=lumivec;

return m_totalL;

}





void IEgammaMonTool::setCurrentMonGroup(const std::string &monGroup) {
  // Check if it exists!
  m_currentMonGroup = monGroup;
}

void IEgammaMonTool::addMonGroup(MonGroup *m) {
  if (!m)
    throw egammaMonException("IEgammaMonTool::addMonGroup(MonGroup *m == 0)!");
  // Overwrite items!
  std::map<std::string, MonGroup *>::iterator i;
  if ((i = m_group.find(m->system())) != m_group.end()) {
    m_group.erase(i);
  }
  
  m_group.insert(std::pair<std::string, MonGroup *>(m->system(), m));
  setCurrentMonGroup(m->system());
}

void IEgammaMonTool::addProfile2d(TProfile2D *h, const std::string &monGroup) {
 
  if (!h)
    throw GaudiException("Profile2D pointer is NULL","IEgammaMonTool",StatusCode::FAILURE);
 
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
    throw GaudiException("Mongroup not found","IEgammaMonTool",StatusCode::FAILURE);

 
  StatusCode sc;
  sc = groupItr->second->regHist(h);
  if (sc.isFailure()) {
    throw GaudiException("failed registering profile 2D","IEgammaMonTool",StatusCode::FAILURE);
  }

  std::string name = groupItr->second->system();
  if (name.at(name.size()-1) != '/')
    name += '/';
  name += h->GetName();
  
  
  // Overwrite items!
  std::map<std::string, TProfile2D *>::iterator i;
  if ((i = m_profile2d.find(name)) != m_profile2d.end()) {
    m_profile2d.erase(i);
  }
  
  m_profile2d.insert(std::pair<std::string, TProfile2D *>(name, h));

}
void IEgammaMonTool::addProfile(TProfile *h, const std::string &monGroup) {
 
  if (!h)
    throw GaudiException("Profile pointer is NULL","IEgammaMonTool",StatusCode::FAILURE);
 
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
    throw GaudiException("Mongroup not found","IEgammaMonTool",StatusCode::FAILURE);

 
  StatusCode sc;
  sc = groupItr->second->regHist(h);
  if (sc.isFailure()) {
    throw GaudiException("failed registering profile","IEgammaMonTool",StatusCode::FAILURE);
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

void IEgammaMonTool::addHistogram(TH1 *h, const std::string &monGroup) {
  if (!h)
    throw egammaMonException("IEgammaMonTool::addHistogram(TH1 *h == 0, ...)!");
  
  std::string theMonGroup;
  
  std::map<std::string, MonGroup *>::iterator groupItr;
  if (monGroup == "") {
    theMonGroup = m_currentMonGroup;
  } else {
    theMonGroup = monGroup;
  }
  
  groupItr = m_group.find(theMonGroup);
  if (groupItr == m_group.end())
    throw egammaMonException(std::string(std::string("Adding histogram ") + h->GetName()) + std::string(" failed: no MonGroup named ") + theMonGroup);

  StatusCode sc;
  sc = groupItr->second->regHist(h);
  if (sc.isFailure()) {
    throw egammaMonException(std::string("Failure registering histogram ") + h->GetName());
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

void IEgammaMonTool::addHistogram(TH2 *h, const std::string &monGroup) {
  if (!h)
    throw egammaMonException("IEgammaMonTool::addHistogram(TH2 *h == 0, ...)!");
  
  std::string theMonGroup;
  
  std::map<std::string, MonGroup *>::iterator groupItr;
  if (monGroup == "") {
    theMonGroup = m_currentMonGroup;
  } else {
    theMonGroup = monGroup;
  }
  
  groupItr = m_group.find(theMonGroup);
  if (groupItr == m_group.end())
    throw egammaMonException(std::string(std::string("Adding histogram (2D) ") + h->GetName()) + std::string(" failed: no MonGroup named ") + theMonGroup);

  StatusCode sc;
  sc = groupItr->second->regHist(h);
  if (sc.isFailure()) {
    throw egammaMonException(std::string("Failure registering histogram ") + h->GetName());
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

void IEgammaMonTool::addHistogram(TGraph *h, const std::string &monGroup) {
  if (!h)
    throw egammaMonException("IEgammaMonTool::addHistogram(TGraph *h == 0, ...)!");

  std::string theMonGroup;

  std::map<std::string, MonGroup *>::iterator groupItr;
  if (monGroup == "") {
    theMonGroup = m_currentMonGroup;
  } else {
    theMonGroup = monGroup;
  }

  groupItr = m_group.find(theMonGroup);
  if (groupItr == m_group.end())
    throw egammaMonException(std::string(std::string("Adding histogram (2D) ") + h->GetName()) + std::string(" failed: no MonGroup named ") + theMonGroup);

  StatusCode sc;
  sc = groupItr->second->regGraph(h);
  if (sc.isFailure()) {
    throw egammaMonException(std::string("Failure registering histogram ") + h->GetName());
  }

  std::string name = groupItr->second->system();
  if (name.at(name.size()-1) != '/')
    name += '/';
  name += h->GetName();

  // Overwrite items!
  std::map<std::string, TGraph *>::iterator i;
  if ((i = m_graph.find(name)) != m_graph.end()) {
    m_graph.erase(i);
  }
  m_graph.insert(std::pair<std::string, TGraph *>(name, h));
}


void IEgammaMonTool::addTree(TTree *t, const std::string &monGroup) {
  if (!t)
    throw egammaMonException("IEgammaMonTool::addTree(TTree *t == 0, ...)!");

  std::string theMonGroup;
  
  std::map<std::string, MonGroup *>::iterator groupItr;
  if (monGroup == "") {
    theMonGroup = m_currentMonGroup;
  } else {
    theMonGroup = monGroup;
  }
  groupItr = m_group.find(theMonGroup);
  if (groupItr == m_group.end())
    throw egammaMonException(std::string(std::string("Adding tree ") + t->GetName()) + std::string(" failed: no MonGroup named ") + theMonGroup);

  StatusCode sc;
  sc = groupItr->second->regTree(t);
  if (sc.isFailure()) {
    throw egammaMonException(std::string("Failure registering tree ")+t->GetName());
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

TH1 *IEgammaMonTool::hist(const std::string &histName, const std::string &monGroup) {
  std::string theMonGroup;
  
  std::map<std::string, MonGroup *>::iterator groupItr;
  if (monGroup == "") {
    theMonGroup = m_currentMonGroup;
  } else {
    theMonGroup = monGroup;
  }
  
  groupItr = m_group.find(theMonGroup);
  if (groupItr == m_group.end())
    throw egammaMonException(std::string(std::string("Getting histogram ") + histName) + std::string(" failed: no MonGroup named ") + theMonGroup);

  std::string name = groupItr->second->system();
  name += '/';
  name += histName;
  std::map<std::string, TH1 *>::iterator histItr = m_hist.find(name);
  if (histItr == m_hist.end())
    throw egammaMonException(std::string(std::string("Getting histogram with path ") + name) + std::string(" failed! No such object!"));
  if (!histItr->second)
    throw egammaMonException(std::string(std::string("Getting histogram with path ") + name) + std::string(" failed! It is a NULL pointer!"));

  return histItr->second;
}

TProfile2D *IEgammaMonTool::profile2d(const std::string &histName, const std::string &monGroup) {
  std::string theMonGroup;
 
  std::map<std::string, MonGroup *>::iterator groupItr;
  if (monGroup == "") {
    theMonGroup = m_currentMonGroup;
  } else {
    theMonGroup = monGroup;
  }
 
  groupItr = m_group.find(theMonGroup);
  if (groupItr == m_group.end())
    throw GaudiException("mongroup not found","IEgammaMonTool",StatusCode::FAILURE);
 
  std::string name = groupItr->second->system();
  name += '/';
  name += histName;
 
  std::map<std::string, TProfile2D *>::iterator histItr = m_profile2d.find(name);
 
  if (histItr == m_profile2d.end())
    throw GaudiException("grpah not found","IEgammaMonTool",StatusCode::FAILURE);
  if (!histItr->second)
    throw GaudiException("grpah is NULL pointer","IEgammaMonTool",StatusCode::FAILURE);
 
  return histItr->second;
 
}
TProfile *IEgammaMonTool::profile(const std::string &histName, const std::string &monGroup) {
  std::string theMonGroup;
 
  std::map<std::string, MonGroup *>::iterator groupItr;
  if (monGroup == "") {
    theMonGroup = m_currentMonGroup;
  } else {
    theMonGroup = monGroup;
  }
 
  groupItr = m_group.find(theMonGroup);
  if (groupItr == m_group.end())
    throw GaudiException("mongroup not found","IEgammaMonTool",StatusCode::FAILURE);
 
  std::string name = groupItr->second->system();
  name += '/';
  name += histName;
 
  std::map<std::string, TProfile *>::iterator histItr = m_profile.find(name);
 
  if (histItr == m_profile.end())
    throw GaudiException("grpah not found","IEgammaMonTool",StatusCode::FAILURE);
  if (!histItr->second)
    throw GaudiException("grpah is NULL pointer","IEgammaMonTool",StatusCode::FAILURE);
 
  return histItr->second;
 
}

TH2 *IEgammaMonTool::hist2(const std::string &histName, const std::string &monGroup) {
  std::string theMonGroup;
  
  std::map<std::string, MonGroup *>::iterator groupItr;
  if (monGroup == "") {
    theMonGroup = m_currentMonGroup;
  } else {
    theMonGroup = monGroup;
  }
  
  groupItr = m_group.find(theMonGroup);
  if (groupItr == m_group.end())
    throw egammaMonException(std::string(std::string("Getting histogram (2D) ") + histName) + std::string(" failed: no MonGroup named ") + theMonGroup);

  std::string name = groupItr->second->system();
  name += '/';
  name += histName;
  std::map<std::string, TH2 *>::iterator histItr = m_hist2.find(name);
  if (histItr == m_hist2.end())
    throw egammaMonException(std::string(std::string("Getting histogram (2D) with path ") + name) + std::string(" failed! No such object!"));
  if (!histItr->second)
    throw egammaMonException(std::string(std::string("Getting histogram (2D) with path ") + name) + std::string(" failed! It is a NULL pointer!"));

  return histItr->second;
}

TGraph *IEgammaMonTool::graph(const std::string &histName, const std::string &monGroup) {
  std::string theMonGroup;

  std::map<std::string, MonGroup *>::iterator groupItr;
  if (monGroup == "") {
    theMonGroup = m_currentMonGroup;
  } else {
    theMonGroup = monGroup;
  }

  groupItr = m_group.find(theMonGroup);
  if (groupItr == m_group.end())
    throw egammaMonException(std::string(std::string("Getting histogram (2D) ") + histName) + std::string(" failed: no MonGroup named ") + theMonGroup);

  std::string name = groupItr->second->system();
  name += '/';
  name += histName;
  std::map<std::string, TGraph *>::iterator histItr = m_graph.find(name);
  if (histItr == m_graph.end())
    throw egammaMonException(std::string(std::string("Getting histogram (2D) with path ") + name) + std::string(" failed! No such object!"));
  if (!histItr->second)
    throw egammaMonException(std::string(std::string("Getting histogram (2D) with path ") + name) + std::string(" failed! It is a NULL pointer!"));

  return histItr->second;
}


TTree *IEgammaMonTool::tree(const std::string &treeName, const std::string &monGroup) {
  std::string theMonGroup;
  std::map<std::string, MonGroup *>::iterator groupItr;
  if (monGroup == "") {
    theMonGroup = m_currentMonGroup;
  } else {
    theMonGroup = monGroup;
  }
  groupItr = m_group.find(theMonGroup);
  if (groupItr == m_group.end())
    throw egammaMonException(std::string(std::string("Getting tree ") + treeName) + std::string(" failed: no MonGroup named ") + theMonGroup);
    
  std::string name = groupItr->second->system();
  name += '/';
  name += treeName;
  std::map<std::string, TTree *>::iterator treeItr = m_tree.find(name);
  if (treeItr == m_tree.end())
    throw egammaMonException(std::string(std::string("Getting tree with path ") + name) + std::string(" failed! No such object!"));
  if (!treeItr->second)
    throw egammaMonException(std::string(std::string("Getting tree with path ") + name) + std::string(" failed! It is a NULL pointer!"));

  return treeItr->second;
}

//-REMI append _old
StatusCode IEgammaMonTool::bookHistograms_old(bool isNewEventsBlock, bool isNewLumiBlock, bool isNewRun) {
  StatusCode sc = StatusCode::SUCCESS;

  if (isNewRun) {
    m_hist.clear();
    m_hist2.clear();
    m_tree.clear();
  }

  try {
    sc = book(isNewEventsBlock, isNewLumiBlock, isNewRun);
    if (sc.isFailure()) {
      ATH_MSG_ERROR( "book() for " << name() << ", returned StatusCode::FAILURE!" );
    }
  } catch(const egammaMonException &e) {
    if (sc.isFailure()) {
    }
    ATH_MSG_ERROR( "Exception thrown: " << e.msg() );
    return StatusCode::FAILURE;
  } catch(...) {
    if (sc.isFailure()) {
    }
    ATH_MSG_ERROR( "Unknown exception caught, while booking histograms" );
    return StatusCode::FAILURE;
  }
  return sc;
}

StatusCode IEgammaMonTool::fillHistograms() {
  StatusCode sc = StatusCode::SUCCESS;
  try {
    ATH_MSG_DEBUG("Running fill( for " << name() );
    sc = fill();
    if (sc.isFailure()) {
      ATH_MSG_ERROR( "fill() for " << name() << ", returned StatusCode::FAILURE!" );
    }
  } catch(const egammaMonException &e) {
    if (sc.isFailure()) {
    }
    ATH_MSG_ERROR( "Exception thrown: " << e.msg() );
    return StatusCode::FAILURE;
  } catch(...) {
    if (sc.isFailure()) {
    }
    ATH_MSG_ERROR( "Unknown exception caught, while filling histograms" );
    return StatusCode::FAILURE;
  }
  return sc;
}

//-REMI append _old
StatusCode IEgammaMonTool::procHistograms_old(bool isNewEventsBlock, bool isNewLumiBlock, bool isNewRun) {
  StatusCode sc = StatusCode::SUCCESS;
  try {
    sc = proc(isNewEventsBlock, isNewLumiBlock, isNewRun);
    if (sc.isFailure()) {
      ATH_MSG_ERROR( "proc() for " << name() << ", returned StatusCode::FAILURE!" );
    }
  } catch(const egammaMonException &e) {
    if (sc.isFailure()) {
    }
    ATH_MSG_ERROR( "Exception thrown: " << e.msg() );
    return StatusCode::FAILURE;
  } catch(...) {
    if (sc.isFailure()) {
    }
    ATH_MSG_ERROR( "Unknown exception caught, while processing histograms" );
    return StatusCode::FAILURE;
  }
  return sc;
}

