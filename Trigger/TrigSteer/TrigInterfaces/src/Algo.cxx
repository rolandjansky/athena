/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <sstream>

#include "TrigInterfaces/AlgoConfig.h"
#include "TrigNavigation/TriggerElement.h"

#include "TrigTimeAlgs/TrigTimerSvc.h"
#include "AthenaMonitoring/IMonitorToolBase.h"
#include "TrigSteeringEvent/TrigOperationalInfo.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "TrigInterfaces/Algo.h"
#include "StoreGate/StoreGateSvc.h"

using namespace HLT;
using namespace std;


namespace localns {
  static const std::vector<unsigned int> emptyUintVector;
}

bool Algo::m_isReRunning = false;
bool Algo::m_isNoSelection = false;



Algo::Algo(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
    m_config(0),
    m_monitoringStarted(false),
    m_ecMapSize(0),    
    m_doAuditors(false),
    m_timerSvc(0),
    m_totalTime(0),
    m_monitors(this)
{
  declareProperty("doTiming",  m_doTiming = false, "Enables timing measurement for this algorithm");
  declareProperty("doOperationalInfo", m_doOperationalInfo = false, "Adds to each prcessed RoI OperataionalInfo object with timing");
  declareProperty( "AthenaMonTools",  m_monitors, "List of monitoring tools to be run withi this instance, if incorrect then tool is silently skipped.");

  declareProperty("ErrorCodeMap",       m_errorCodeMap,        "ErrorCodes remapping" );
}

Algo::~Algo()
{}


ITrigTimerSvc* Algo::timerSvc() const
{
  if (!m_doTiming) return 0;

  if (!m_timerSvc) {
    std::cout << "Algo::m_timerSvc is not (yet) initialized - either "
	      << "1) Algo::initialize() has not been called yet "
	      << "2) An earlier error ocurred --- "
	      << "returning null pointer" << std::endl;
  }

  return m_timerSvc;
}


TrigTimer* Algo::addTimer(std::string label, std::string propName)
{
  if (!timerSvc()) return 0;
  std::string prefix = name() + ":";

  TrigTimer* timer =  timerSvc()->addItem(prefix + label);
  m_timers[label] = timer;
  declareMonitoredObject(label, (const TrigTimer*&)m_timers[label], &TrigTimer::elapsed);

  if (propName != "") { 
    timer->propName(prefix + propName); 
    declareMonitoredObject(label+"_"+propName, (const TrigTimer*&)m_timers[label], &TrigTimer::propVal);
  }

  return timer;
}


TrigTimer* Algo::timer(std::string label) const
{
  if (!timerSvc()) return 0;

  std::map<std::string, TrigTimer*>::const_iterator t = m_timers.find(label);
  if (t == m_timers.end()) {
    ATH_MSG_ERROR( " trying to access non-existent timer " << label);
    return 0;
  }

  return (*t).second;
}

TrigTimer* Algo::totalTimeTimer() const {
  return m_totalTime;
}


StatusCode Algo::initialize()
{
  map<string,string>::const_iterator mitr(m_errorCodeMap.begin());
  map<string,string>::const_iterator mend(m_errorCodeMap.end());
  while ( mitr!=mend ) {
    ErrorCode in  = getErrorCodePosFromStr((*mitr).first);
    ErrorCode out = getErrorCodePosFromStr((*mitr).second);
    ATH_MSG_DEBUG( name() << "\tMapping " << strErrorCode(in) << "\t-> " << strErrorCode(out));
    
    if ( m_ecMap.find(in) == m_ecMap.end() ) m_ecMap[in] = out;
    else {
      ATH_MSG_WARNING( "ErrorCode " << in << " already in map - not adding");
    }
    ++mitr;
  }
  m_ecMapSize = m_ecMap.size();



  // Retrieve TrigTimer service.
  //  m_timerSvc = 0;

  if (m_doTiming || m_config -> getSteeringOPILevel() > 0) {

    StatusCode sc = service("TrigTimerSvc", m_timerSvc);
    if (sc.isFailure()) {
      ATH_MSG_ERROR( "unable to locate timing service TrigTimerSvc");
      m_timerSvc = 0;
      return sc;
    }
  }

  m_totalTime = (m_doTiming ? addTimer("TotalTime") : 0);

  // Always have valid timer when OPI is collected
  if ( m_config -> getSteeringOPILevel() > 0) {
    if ( !m_totalTime || !m_totalTime->isActive() ) {
      
      TrigTimer *opiTimer = 0;
      std::vector < TrigTimer* >::iterator ti;
      for ( ti = m_timerSvc->begin(); ti != m_timerSvc->end(); ++ti ) {
	TrigTimer *curTimer = *ti;
	if(curTimer && curTimer->name() == "ALGO::OPITimer") { 
	  opiTimer = curTimer;
	  break;
	}
      }
      
      if(!opiTimer) opiTimer = m_timerSvc -> addItem("ALGO::OPITimer");      
      m_totalTime = opiTimer;
    }
  }


  declareMonitoredVariable("ReturnErrorCode", m_returnErrorCode);
  declareMonitoredVariable("UnmappedErrorCode", m_unmappedErrorCode);


  // call initialize from derived class:
  HLT::ErrorCode stat = hltInitialize();
  if (stat != HLT::OK) {
    ATH_MSG_ERROR ( "Failed to initialize: " << name());
    return StatusCode::FAILURE;    
  }

  ATH_MSG_DEBUG ( "initialization done" );
    

  // monitoring
  if ( initializeMonitors().isFailure() ) {
    ATH_MSG_WARNING ( "Unable to initialize all monitors, continuing anyway" );
  }


  // Get AuditAlgorithms property from base class
  BooleanProperty prop("AuditAlgorithms",false);
  if (getProperty(&prop).isSuccess()) m_doAuditors = prop.value();

  if (m_doAuditors)
    auditorSvc();

  return StatusCode::SUCCESS;
}


StatusCode Algo::start()
{
  if ( hltStart() != HLT::OK ) return StatusCode::FAILURE;

  if ( startMonitors().isFailure() ) {
    ATH_MSG_WARNING ( "Unable to initialize at start all monitors, continuing anyway" );
  }

  return StatusCode::SUCCESS;
}

StatusCode Algo::stop()
{
  if ( hltStop() != HLT::OK ) return StatusCode::FAILURE;

  if ( stopMonitors().isFailure() ) {
    ATH_MSG_WARNING ("Unable to unitialize at stop all monitors, continuing anyway" );
  }
  return StatusCode::SUCCESS;
}


StatusCode Algo::beginRun()
{
  if ( hltBeginRun() != HLT::OK) return StatusCode::FAILURE;

  if ( beginRunMonitors().isFailure() ) {
    ATH_MSG_WARNING ("Unable to initialize at beginRun all monitors, continuing anyway");
  }
  return StatusCode::SUCCESS;
}


StatusCode Algo::endRun()
{
  if (hltEndRun() != HLT::OK ) return StatusCode::FAILURE;

  if ( endRunMonitors().isFailure() ) {
    ATH_MSG_WARNING ( "Unable to uninitialize at endRun all monitors, continuing anyway");
  }
  return StatusCode::SUCCESS;
}

StatusCode Algo::finalize()
{
  // call initialize from derived class:
  HLT::ErrorCode stat = hltFinalize();

  if (stat != HLT::OK) return StatusCode::FAILURE;
  return StatusCode::SUCCESS;
}


TEConstVec Algo::constify(const TEVec& vec)
{
  TEConstVec constVec;

  for (TEVec::const_iterator iter = vec.begin(); iter != vec.end(); ++iter)
    constVec.push_back(*iter);

  return constVec;
}


ErrorCode Algo::processTEs(TEVec& outputTEs)
{
  if (!runAlgo()) return HLT::OK;

  beforeEventMonitors().ignore();
  
  ErrorCode status = HLT::OK;

  // For each TE in the TE vector, ...
  for (std::vector<TriggerElement*>::iterator outputTE = outputTEs.begin();
       outputTE != outputTEs.end(); ++outputTE) {

    if (!(*outputTE)->getActiveState()) continue;
    TEConstVec inputs = constify(Navigation::getDirectPredecessors( *outputTE ));
    
    beforeExecMonitors().ignore();

    ErrorCode ec = plugin_processTE(inputs, *outputTE);
            
    mapAndRecordError(ec, *outputTE);

    afterExecMonitors().ignore();    

    if ( m_config -> getSteeringOPILevel() > 0 ) {
      addOperationalInfo(*outputTE);
      addSteeringOperationalInfo(true, outputTEs.size(), *outputTE);
    }

    status = (ec > status) ? ec : status;

    if (status.action() > HLT::Action::CONTINUE  ) {
      afterEventMonitors().ignore();
      return status;
    }
  }

  afterEventMonitors().ignore();
  return status;
}


bool Algo::reset() {
  m_returnErrorCode   = HLT::OK;
  m_unmappedErrorCode = HLT::OK;
  if ( hltEndEvent() != HLT::OK ) return false;
  return true;
}


void Algo::mapAndRecordError(ErrorCode& errorCode, TriggerElement* te) {
  m_unmappedErrorCode = errorCode;
  if (errorCode == HLT::OK)
    return;


  ATH_MSG_VERBOSE ( " got error for remapping " << strErrorCode(errorCode) );

  
  if (te) te->setErrorState();

  if ( m_ecMapSize == 0 ) {
    // remember most severe error
    m_returnErrorCode = m_returnErrorCode > errorCode ? m_returnErrorCode : errorCode;
    return;
  } 


  map<ErrorCode,ErrorCode>::const_iterator mit(m_ecMap.find(errorCode));

  // do we need to map this code?
  if ( mit == m_ecMap.end() ) {
    // remember most severe error
    m_returnErrorCode = m_returnErrorCode > errorCode ? m_returnErrorCode : errorCode;
    return;
  }

  ATH_MSG_VERBOSE (" mapping " << strErrorCode(mit->first) << " -> "
		   << strErrorCode(mit->second) );

  // remember most severe error
  m_returnErrorCode = m_returnErrorCode > mit->second ? m_returnErrorCode : mit->second;
}



/*  Monitors */
/******************************************************************/
StatusCode Algo::initializeMonitors() {

  if ( m_monitors.retrieve().isFailure() ) {
    ATH_MSG_ERROR ( "Monitoring tools not initialized: " << m_monitors );
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode Algo::startMonitors() {
  ToolHandleArray<IMonitorToolBase>::iterator it;
  for ( it = m_monitors.begin(); it != m_monitors.end(); ++it ) {
    if ( (*it)->bookHists().isFailure() ) {
      ATH_MSG_WARNING ( "Monitoring tool: " <<  (*it)
			<< " in Algo: " << name()
			<< " can't book histograms successfully, remove it or fix booking problem" );
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_DEBUG ( "Monitoring tool: " <<  (*it)
		      << " in Algo: " << name()
		      << " bookHists successful");
    }
  }
  return StatusCode::SUCCESS;
}


StatusCode Algo::stopMonitors()
{
  StatusCode sc = StatusCode::SUCCESS;
  ToolHandleArray<IMonitorToolBase>::iterator it;
  for ( it = m_monitors.begin(); it != m_monitors.end(); ++it ) {
    if ( (*it)->finalHists().isFailure() ) {
      sc = StatusCode::FAILURE;
      ATH_MSG_WARNING ( "Monitoring tool: " << (*it) << " in Algo: " << name() << " finalHists failed");
    }
  }
  return sc;
}


StatusCode Algo::beginRunMonitors()
{
  return StatusCode::SUCCESS;
}


StatusCode Algo::endRunMonitors()
{
  return StatusCode::SUCCESS;
}


StatusCode Algo::beforeEventMonitors() {

  ATH_MSG_VERBOSE ( "beforeEventMonitors " << name() );
    
  if (m_doAuditors) {
    auditorSvc()->before(IAuditor::Execute, this);
  }

  return StatusCode::SUCCESS;
}


StatusCode Algo::afterEventMonitors() {
  
  ATH_MSG_VERBOSE ( "afterEventMonitors " << name() );

  if (m_doAuditors) {
    StatusCode sc;
    auditorSvc()->after(IAuditor::Execute, this, sc);
    sc.ignore();
  }

  return StatusCode::SUCCESS;
}


StatusCode Algo::beforeExecMonitors() {
  //std::cout << name() << ": beforeExecMonitors" << std::endl;
  if ( m_monitoringStarted )
    return StatusCode::SUCCESS;
  
  m_monitoringStarted = true;


  ATH_MSG_VERBOSE ( "beforeExecMonitors " << name() );
  if (totalTimeTimer()) totalTimeTimer()->start();

  IMonitoredAlgo::VariablesMap::const_iterator it;

  for ( it = variables().begin(); it != variables().end(); ++it ) {
    it->second->reset();
  }
  
  
      
  return StatusCode::SUCCESS;
}


StatusCode Algo::afterExecMonitors() {
  //std::cout << name() << ": afterExecMonitors" << std::endl;
  if ( ! m_monitoringStarted )
    return StatusCode::SUCCESS;
  m_monitoringStarted=false;


  ATH_MSG_VERBOSE ("afterExecMonitors " << name());

  if (totalTimeTimer()) totalTimeTimer()->stop();

  ToolHandleArray<IMonitorToolBase>::iterator it;
  for ( it = m_monitors.begin(); it != m_monitors.end(); ++it ) {
    if (!(*it)->preSelector()) (*it)->fillHists().ignore();
  }
  return StatusCode::SUCCESS;
}


StatusCode Algo::finalizeMonitors()
{
  return StatusCode::SUCCESS;
}


void Algo::addOperationalInfo(TriggerElement* te) {
  TrigOperationalInfo* info = new TrigOperationalInfo();
  for ( std::map<std::string, TrigTimer*>::iterator i = m_timers.begin(); i != m_timers.end(); ++i ) {
    info->set(i->second->name(), i->second->elapsed());
  }
  if (!m_config || !m_config->getNavigation()) return;
  std::string key;
  m_config->getNavigation()->attachFeature(te, info, HLT::Navigation::ObjectCreatedByNew, key);
}


void Algo::addSteeringOperationalInfo(bool wasRun, unsigned int /*ntes*/, TriggerElement* te) {
    // collect operational information: create new TrigOperationalInfo
  if(!m_config || !(m_config -> getSteeringOPILevel() > 0) || !(m_config -> getSteeringOPI())) {
    return;
  }
  // get current TOI and create unique prefix
  TrigOperationalInfo *steer_opi = m_config -> getSteeringOPI();
  std::string prefix = "ALGO:"+name();

  // determine number of times this function was called for this TOI
  const std::string callkey = prefix+":Call";
  unsigned int icall = 0;

  // Don't want to use "defined" any more as we no longer have map behavior here. 
  const std::vector<std::string>& keys = steer_opi->getKeys();
  const std::vector<float>& values = steer_opi->getValues();

  // Find any previous "call", reverse-iterate until we come upon a "CHAIN" at which point we must stop
  for (unsigned int pos = keys.size() - 1; pos > 0; --pos) {
    if (keys[pos] == callkey) {
      icall = floor(values[pos]) + 1; // Increment value
      steer_opi->updateAtLocation( pos, static_cast<float>(icall) );
      break;
    } else if (keys.at(pos).compare(0, 3, "SEQ") == 0) {
      break; // Only look back as far as the last SEQUENCE call
    }
  }

  // This is what we replaced wit the above loop, because OPI does not behave "map like" any more (duplicate keys are allowed)
  // if(steer_opi -> defined(callkey)) {
  //   icall = static_cast<unsigned int>(steer_opi -> get(callkey)) + 1;
  // }

  // set current function call number, to be used next time
  // Now we only do this if icall == 0, else we already had one and have updated it in the loop above
  if (icall == 0) {
    steer_opi -> set(callkey, static_cast<float>(icall));
  }

  // now create unique prefix
  prefix = prefix + ":" + std::to_string(icall);

  // Iterate over ROI TEs and collect ROI words
  if(te && m_config->getNavigation()) {
    const std::vector<TriggerElement*> &roi_vec = Navigation::getRoINodes(te);
    std::string roi_istr_pref = prefix + ":RoiId:";
    for(unsigned int i = 0; i < roi_vec.size(); ++i) {
      TriggerElement *roiTE = roi_vec[i];
      if(!roiTE) continue;

      // Extract ROI data
      const TrigRoiDescriptor *roi_des = 0;
      m_config -> getNavigation() -> getFeature<TrigRoiDescriptor>(roiTE, roi_des);
   
      std::string roi_istr = roi_istr_pref + std::to_string(i);

      if(roi_des) {
	      // RoI node has RoI descriptor - save RoiId
	      steer_opi -> set(roi_istr, static_cast<float>(roi_des->roiId()));
      } else {
	      // RoI node is either Energy or JetEt: assume there are less than 255 rois!
	      steer_opi -> set(roi_istr, 255.0);	
      }
    }
  }

  if (wasRun) {
    if (totalTimeTimer()) {
        steer_opi -> set(prefix+":TotalTime", totalTimeTimer()->elapsed());
        steer_opi -> set(prefix+":StartSec",  totalTimeTimer()->getStartTime().tv_sec % 3600);
        steer_opi -> set(prefix+":StartUSec", totalTimeTimer()->getStartTime().tv_usec);
        steer_opi -> set(prefix+":StopSec",   totalTimeTimer()->getStopTime().tv_sec % 3600);
        steer_opi -> set(prefix+":StopUSec",  totalTimeTimer()->getStopTime().tv_usec);
    } else {
      steer_opi -> set(prefix+":TotalTime", 0.0);
    }
  } else {
    steer_opi -> set(prefix+":Cached", 1);
  }
}
