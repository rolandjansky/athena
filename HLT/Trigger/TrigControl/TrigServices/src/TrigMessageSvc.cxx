/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/Message.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "TrigMessageSvc.h"
#include "TrigHLTIssues.h"
#include "TrigMonitorBase/TrigLockedHist.h"

#include <TH1I.h>
#include <TH2I.h>

#include <sstream>
#include <iostream>

#include <pthread.h>
#include <boost/thread/recursive_mutex.hpp>


static pthread_mutex_t msgsvcmutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;

using namespace std;

static std::string levelNames[MSG::NUM_LEVELS];

// ERS debug level for which to enforce DEBUG OutputLevel
static int ersGaudiDebugOffset = 8;

// Constructor
TrigMessageSvc::TrigMessageSvc( const std::string& name, ISvcLocator* svcloc )
  : base_class(name, svcloc)
  , m_doSuppress{false}
  , m_running{false}
  , m_canEnter{false}
  , m_evtStore{"StoreGateSvc/StoreGateSvc", name}
  , m_msgCountHist{nullptr}
  , m_msgCountSrcHist{nullptr}
{
  m_defaultStream = &std::cout;
  m_outputLevel   = MSG::NIL;
  declareProperty( "Format",      m_defaultFormat = "% F%18W%S%7W%R%T %0W%M" );
  declareProperty( "timeFormat",  m_defaultTimeFormat = "%Y-%m-%d %H:%M:%S,%f" );

  declareProperty( "showStats",    m_stats = false );
  declareProperty( "statLevel",    m_statLevel = 0, "Show statistics for messages >= level" );
  declareProperty( "statLevelRun", m_statLevelRun = 0, "Show per-run statistics for messages >= level" );
  
  declareProperty( "publishStats", m_publishStats = false,
                   "Publish message statistics as histogram");
  
  declareProperty( "publishLevel", m_publishLevel = MSG::INFO,
                   "Publish message statistics for this and higher message levels");
  
  declareProperty( "resetStatsAtBeginRun", m_resetStats = false );  

  declareProperty( "printEventIDLevel", m_eventIDLevel = MSG::WARNING,
                   "Print event ID for this and higher message levels");
                   
  // Special properties to control output level of individual sources
  declareProperty( "setVerbose",  m_thresholdProp[MSG::VERBOSE] );
  declareProperty( "setDebug",    m_thresholdProp[MSG::DEBUG] );
  declareProperty( "setInfo",     m_thresholdProp[MSG::INFO] );
  declareProperty( "setWarning",  m_thresholdProp[MSG::WARNING] );
  declareProperty( "setError",    m_thresholdProp[MSG::ERROR] );
  declareProperty( "setFatal",    m_thresholdProp[MSG::FATAL] );
  declareProperty( "setAlways",   m_thresholdProp[MSG::ALWAYS] );

  declareProperty( "useColors",        m_color=false);
  m_color.declareUpdateHandler(&TrigMessageSvc::initColors, this);

  declareProperty( "fatalColorCode",   m_logColors[MSG::FATAL] );
  declareProperty( "errorColorCode",   m_logColors[MSG::ERROR] );
  declareProperty( "warningColorCode", m_logColors[MSG::WARNING] );
  declareProperty( "infoColorCode",    m_logColors[MSG::INFO] );
  declareProperty( "debugColorCode",   m_logColors[MSG::DEBUG] );
  declareProperty( "verboseColorCode", m_logColors[MSG::VERBOSE] );
  declareProperty( "alwaysColorCode",  m_logColors[MSG::ALWAYS] );

  m_defaultLimit = 500;
  declareProperty( "fatalLimit",    m_msgLimit[MSG::FATAL]   = m_defaultLimit );
  declareProperty( "errorLimit",    m_msgLimit[MSG::ERROR]   = m_defaultLimit );
  declareProperty( "warningLimit",  m_msgLimit[MSG::WARNING] = m_defaultLimit );
  declareProperty( "infoLimit",     m_msgLimit[MSG::INFO]    = m_defaultLimit );
  declareProperty( "debugLimit",    m_msgLimit[MSG::DEBUG]   = m_defaultLimit );
  declareProperty( "verboseLimit",  m_msgLimit[MSG::VERBOSE] = m_defaultLimit );
  declareProperty( "alwaysLimit",   m_msgLimit[MSG::ALWAYS]  = 0 );

  declareProperty( "defaultLimit",  m_msgLimit[MSG::NIL]     = m_defaultLimit );

  declareProperty( "enableSuppression", m_suppress = false,
                   "Enable message suppression");
  
  declareProperty( "suppressRunningOnly", m_suppressRunningOnly = true,
                   "Use message suppression only during RUNNING state");

  m_suppress.declareUpdateHandler(&TrigMessageSvc::setupSuppression, this);
  m_suppressRunningOnly.declareUpdateHandler(&TrigMessageSvc::setupSuppression, this);
  
  /* Special properties to control output to ERS of individual sources.
     The syntax is as follows (these are NOT regular expressions):
     
     useErsFatal = []                       # forward none (default)
     useErsFatal = ['*']                    # forward all
     useErsFatal = ['CoreDumpSvc','MyAlg']  # forward these sources
     useErsFatal = ['*','!MyAlg']           # forward all except MyAlg
  */
    
  const vector<string> defaultErsUse;
  declareProperty( "useErsVerbose",  m_useERS[MSG::VERBOSE] = defaultErsUse );
  declareProperty( "useErsDebug",    m_useERS[MSG::DEBUG]   = defaultErsUse );
  declareProperty( "useErsInfo",     m_useERS[MSG::INFO]    = defaultErsUse );
  declareProperty( "useErsWarning",  m_useERS[MSG::WARNING] = defaultErsUse );
  declareProperty( "useErsError",    m_useERS[MSG::ERROR]   = defaultErsUse );
  declareProperty( "useErsFatal",    m_useERS[MSG::FATAL]   = defaultErsUse );
  declareProperty( "useErsAlways",   m_useERS[MSG::ALWAYS]  = defaultErsUse );

  declareProperty( "alwaysUseMsgStream", m_alwaysUseMsgStream = true,
                   "Print all messages to MsgStream, even if forwarded to ERS");
  
  declareProperty( "useErsRunningOnly", m_useErsRunningOnly = false,
                   "Forward messages to ERS only during RUNNING state");  
                   
  declareProperty( "forceOutputLevel", m_forceOutputLevel = false,
                   "Enforce the global OutputLevel for all sources");

  for (int ic=0; ic<MSG::NUM_LEVELS; ++ic) {
    m_logColors[ic].declareUpdateHandler(&TrigMessageSvc::setupColors, this);
    m_msgLimit[ic].declareUpdateHandler(&TrigMessageSvc::setupLimits, this);
    m_thresholdProp[ic].declareUpdateHandler(&TrigMessageSvc::setupThreshold, this);
  }

  levelNames[0] = "NIL";
  levelNames[1] = "VERBOSE";
  levelNames[2] = "DEBUG";
  levelNames[3] = "INFO";
  levelNames[4] = "WARNING";
  levelNames[5] = "ERROR";
  levelNames[6] = "FATAL";
  levelNames[7] = "ALWAYS";

}
//#############################################################################
// Destructor
TrigMessageSvc::~TrigMessageSvc()
{
  pthread_mutex_destroy(&msgsvcmutex);
}

/// Initialize Service 
StatusCode TrigMessageSvc::initialize() {

  StatusCode sc;
  sc = Service::initialize();
  if( sc.isFailure() ) return sc;
  
  m_running = false;
  m_canEnter = true;
  
#ifdef _WIN32
  m_color = false;
#endif  
  
  // Reset message counters
  resetMsgStats();
  m_sourceMapTotal.clear();

  // Initialize message suppression
  setupSuppression(m_suppress);

  m_colMap["black"]  = MSG::BLACK;
  m_colMap["red"]    = MSG::RED;
  m_colMap["green"]  = MSG::GREEN;
  m_colMap["yellow"] = MSG::YELLOW;
  m_colMap["blue"]   = MSG::BLUE;
  m_colMap["purple"] = MSG::PURPLE;
  m_colMap["cyan"]   = MSG::CYAN;
  m_colMap["white"]  = MSG::WHITE;

  /*
   * m_state is set in the base class sysInitialize after initialize succeeded.
   * However, we need to set it already here otherwise the following
   * use of IncidentSvc/JobOptionsSvc creates a service initialization loop.
   */
  m_state = Gaudi::StateMachine::INITIALIZED;

  /*
   * This will force the OutputLevel of all components to be the global
   * OutputLevel (m_outputLevel of this service). It should cover the use-cases:
   *    1) 'MsgStream(msgSvc(),name())'
   *    2) 'MsgStream::setLevel(m_outputLevel)'
   * For 2) we rely on the fact that the MessageSvc is created/initialized after
   * the JobOptionsSvc in ApplicationMgr::i_startup().
   */
  int ers_debug = ers::Configuration::instance().debug_level();
  if ( ers_debug >= ersGaudiDebugOffset+1 ) {
    setOutputLevel(MSG::VERBOSE);
    m_forceOutputLevel.setValue(true);
  }
  else if ( ers_debug >= ersGaudiDebugOffset ) {
    setOutputLevel(MSG::DEBUG);
    m_forceOutputLevel.setValue(true);
  }
  
  if ( m_forceOutputLevel.value() ) {
    resetOutputLevels();             // Clear all individual OutputLevels (to cover use-case 1)
    setOutputLevelViaJobOptSvc();    // Set 'OutputLevel' in JobOptionsSvc (to cover use-case 2)
    reportMessage(name(), MSG::INFO, "Enforcing global OutputLevel = "+levelNames[m_outputLevel.value()]);
  }
  
  // Register incident handlers
  ServiceHandle<IIncidentSvc> incSvc("IncidentSvc",name());
  sc = incSvc.retrieve();
  if (sc.isFailure()) {
    reportMessage(name(), MSG::WARNING, "Cannot find IncidentSvc");
  }
  else {
    incSvc->addListener(this, "BeginRun", LONG_MAX);
    incSvc->addListener(this, "EndOfBeginRun", LONG_MAX);
    incSvc->addListener(this, "EndRun", LONG_MAX);
  }

 return StatusCode::SUCCESS;
}

//#############################################################################

/// Reinitialize Service 
StatusCode TrigMessageSvc::reinitialize() {
  m_state = Gaudi::StateMachine::OFFLINE;
  return initialize();
}


//#############################################################################
void TrigMessageSvc::handle(const Incident& inc) {

  // We cannot use start/stop for the MessageSvc due to bug #46216
  if (inc.type()=="BeginRun") {
    if (m_suppressRunningOnly.value()) {
      m_doSuppress = m_suppress.value();
    }
    if (m_publishStats) bookHistograms();
  }
  else if (inc.type()=="EndOfBeginRun") {
    if (m_resetStats) {
      reportMessage(name(), MSG::INFO, "Resetting message counts");
      resetMsgStats();
    }
    m_running = true;
  }
  else if (inc.type()=="EndRun") {
    m_running = false;
    if (m_resetStats) reportMsgStats(MSG::INFO);
    if (m_suppressRunningOnly.value()) m_doSuppress = false;
  }
}


//#############################################################################
void TrigMessageSvc::resetMsgStats()
{
  // Copy current counts to total counts map
  std::map<std::string,msgAry>::const_iterator itr;
  std::map<std::string,msgAry>::iterator tot;
  for (itr=m_sourceMap.begin(); itr!=m_sourceMap.end(); ++itr) {
    tot = m_sourceMapTotal.find(itr->first);
    if (tot==m_sourceMapTotal.end()) {  // new entry
      for (int i=0; i<MSG::NUM_LEVELS; i++) m_sourceMapTotal[itr->first].msg[i] = itr->second.msg[i];
    }
    else {  // existing entry
      for (int i=0; i<MSG::NUM_LEVELS; i++) tot->second.msg[i] += itr->second.msg[i];
    }
  }
      
  m_sourceMap.clear();
  m_msgCountMap.clear();
  for (int i=0; i<MSG::NUM_LEVELS; i++) m_msgCount[i] = 0;
}


// ---------------------------------------------------------------------------
// Book the message stats histograms
// ---------------------------------------------------------------------------
void TrigMessageSvc::bookHistograms()
{
  ServiceHandle<ITHistSvc> histSvc("THistSvc",name());
  if ( histSvc.retrieve().isFailure() ) {
    reportMessage(name(), MSG::WARNING,
                  "Cannot find THistSvc. Message stats will not be published.");
    return;
  }
  
  // monitoring information root directory
  const string path = "/EXPERT/" + name() + "/";
  const int nLevelBins = MSG::NUM_LEVELS - m_publishLevel;
  m_msgCountHist = new TH1I("MessageCount", "Messages while RUNNING;Severity;Count",
                            nLevelBins, 0, nLevelBins);

  const int nSrcBins = 1;
  m_msgCountSrcHist = new TH2I("MessageCountBySource", "Messages while RUNNING;Severity;Source",
                               nLevelBins, 0, nLevelBins, nSrcBins, 0, nSrcBins);

    
  for (int i=m_publishLevel; i<MSG::NUM_LEVELS; i++) {
    m_msgCountHist->GetXaxis()->SetBinLabel(i-m_publishLevel+1, levelNames[i].c_str());
    m_msgCountSrcHist->GetXaxis()->SetBinLabel(i-m_publishLevel+1, levelNames[i].c_str());    
  }
  
  if ( histSvc->regHist(path + m_msgCountHist->GetName(), m_msgCountHist).isFailure() ) {
    reportMessage(name(), MSG::WARNING, "Cannot register monitoring histogram 'MessageCount'");
  }  
  if ( histSvc->regHist(path + m_msgCountSrcHist->GetName(), m_msgCountSrcHist).isFailure() ) {
    reportMessage(name(), MSG::WARNING, "Cannot register monitoring histogram 'MessageCountBySource'");
  }
}

void TrigMessageSvc::initColors(Property& /*prop*/) {

  if (m_color == true) {

    if (m_logColors[MSG::FATAL].value().size() == 0) {
      vector<string> fatDef;
      fatDef.push_back( "[94;101;1m" );
      m_logColors[MSG::FATAL].set( fatDef );
    } else {
      TrigMessageSvc::setupColors( m_logColors[MSG::FATAL] );
    }
    
    if (m_logColors[MSG::ERROR].value().size() == 0) {
      vector<string> errDef;
      errDef.push_back( "[97;101;1m" );
      m_logColors[MSG::ERROR].set( errDef );
    } else {
      TrigMessageSvc::setupColors( m_logColors[MSG::ERROR] );
    }
    
    if (m_logColors[MSG::WARNING].value().size() == 0) {
      vector<string> warDef;
      warDef.push_back( "[93;1m" );
      m_logColors[MSG::WARNING].set( warDef );
    } else {
      TrigMessageSvc::setupColors( m_logColors[MSG::WARNING] );
    }

  } else {
    
    // reset all color codes;
    for (int ic=0; ic<MSG::NUM_LEVELS; ++ic) {
      vector<string> def;
      m_logColors[ic].set( def );
    }

  }

}

//#############################################################################

void TrigMessageSvc::setupColors(Property& prop) {

  if (! m_color) return;

  int ic;
  if (prop.name() == "fatalColorCode") {
    ic = MSG::FATAL;
  } else if (prop.name() == "errorColorCode") {
    ic = MSG::ERROR;
  } else if (prop.name() == "warningColorCode") {
    ic = MSG::WARNING;
  } else if (prop.name() == "infoColorCode") {
    ic = MSG::INFO;
  } else if (prop.name() == "debugColorCode") {
    ic = MSG::DEBUG;
  } else if (prop.name() == "verboseColorCode") {
    ic = MSG::VERBOSE;
  } else if (prop.name() == "alwaysColorCode") {
    ic = MSG::ALWAYS;
  } else {
    std::ostringstream internal_msg ;
    internal_msg << "ERROR: Unknown message color parameter: " << prop.name();
    if (passErsFilter(name(), m_useERS[MSG::ERROR])) {
      ers::error( ers::HLTMessage(ERS_HERE,internal_msg.str()) );
    } else {
      std::cerr << internal_msg.str() << std::endl;
    }
    return;
  }

  string code;
  vector<string>::const_iterator itr;
  itr = m_logColors[ic].value().begin();

  if ( m_logColors[ic].value().size() == 1 ) {

    if (*itr == "") {
      code = "";
    } else if (itr->substr(0,1) == "[") {
      code = "\033" + *itr;
    } else {
      code = "\033[" + colTrans(*itr, 90) + ";1m";
    }

  } else if (m_logColors[ic].value().size() == 2) {
    vector<string>::const_iterator itr2 = itr + 1;

    code =  "\033[" + colTrans(*itr, 90) + ";"
      + colTrans(*itr2, 100) + ";1m";

  }

  m_logColorCodes[ic] = code; 

}
//#############################################################################

void TrigMessageSvc::setupLimits(Property& prop) {
  const auto& pname = prop.name();
  if (pname == "alwaysLimit") {
    IntegerProperty *p = dynamic_cast<IntegerProperty*>(&prop);
    if (p && p->value() != 0) {
      std::ostringstream internal_msg ;
      internal_msg << "TrigMessageSvc ERROR: cannot suppress ALWAYS messages" ;
      if (passErsFilter(name(), m_useERS[MSG::ERROR])) {
        ers::error( ers::HLTMessage(ERS_HERE,internal_msg.str()) );
      } else {
        std::cerr << internal_msg.str() << std::endl;
      }
      p->setValue(0);
    }
  } else if (pname == "defaultLimit") {
    // Only change the limits that are still on the default value
    for (int i = MSG::VERBOSE; i< MSG::NUM_LEVELS; ++i) {
      if ( (i != MSG::ALWAYS) && (m_msgLimit[i].value() == m_defaultLimit) ) {
        m_msgLimit[i] = m_msgLimit[MSG::NIL].value();
      }
    }
  } else if (pname != "fatalLimit" &&
             pname != "errorLimit" &&
             pname != "warningLimit" &&
             pname != "infoLimit" &&
             pname != "debugLimit" &&
             pname != "verboseLimit") {
    std::ostringstream internal_msg ;
    internal_msg << "TrigMessageSvc ERROR: Unknown message limit parameter: " << prop.name() ;
    if (passErsFilter(name(), m_useERS[MSG::ERROR])) {
      ers::error( ers::HLTMessage(ERS_HERE,internal_msg.str()) );
    } else {
      std::cerr << internal_msg.str() << std::endl;
    }
    return;
  }
}
//#############################################################################

void TrigMessageSvc::setupThreshold(Property& prop) {

  int ic = 0;
  if (prop.name() == "setFatal") {
    ic = MSG::FATAL;
  } else if (prop.name() == "setError") {
    ic = MSG::ERROR;
  } else if (prop.name() == "setWarning") {
    ic = MSG::WARNING;
  } else if (prop.name() == "setInfo") {
    ic = MSG::INFO;
  } else if (prop.name() == "setDebug") {
    ic = MSG::DEBUG;
  } else if (prop.name() == "setVerbose") {
    ic = MSG::VERBOSE;
  } else if (prop.name() == "setAlways") {
    ic = MSG::ALWAYS;
  } else {
    std::ostringstream internal_msg ;
    internal_msg << "TrigMessageSvc ERROR: Unknown message theshold parameter: " << prop.name();
    if (passErsFilter(name(), m_useERS[MSG::ERROR])) {
      ers::error( ers::HLTMessage(ERS_HERE,internal_msg.str()) );
    } else {
      std::cerr << internal_msg.str() << std::endl;
    }
    return;
  }

  StringArrayProperty *sap = dynamic_cast<StringArrayProperty*>( &prop);
  if (sap == 0) {
    std::ostringstream internal_msg ;
    internal_msg << "could not dcast " << prop.name() 
		 << " to a StringArrayProperty (which it should be!)";
    if (passErsFilter(name(),m_useERS[MSG::ERROR])) {
      ers::error( ers::HLTMessage(ERS_HERE,internal_msg.str()) );
    } else {
      std::cerr << internal_msg.str() << std::endl;
    }
    return;
  } else {
    std::vector<std::string>::const_iterator itr;
    for ( itr =  sap->value().begin();
	  itr != sap->value().end(); 
	  ++itr) {
      setOutputLevel( *itr, ic );
    }
  }

}

//#############################################################################

bool TrigMessageSvc::passErsFilter(const std::string& source,
                                   const std::vector<std::string>& filter)
{
  if (m_useErsRunningOnly.value() and not m_running) return false;
  if (filter.empty()) return false;   // forward none
  vector<string>::const_iterator it = filter.begin();
  if (filter.size()==1 && (*it)=="*") return true;   // forward all

  bool pass(false);
  for (; it!=filter.end(); ++it) {
    if ((*it)=="*") pass=true;           // forward except if there is a veto later
    if (source==(*it)) return true;      // forward specific source
    if ("!"+source==(*it)) return false; // veto specific source
  }
  return pass;
}

  
//#############################################################################

void TrigMessageSvc::setupSuppression(Property& /*prop*/) {
  // (De)activate suppression
  if ( m_suppressRunningOnly.value() ) {
    m_doSuppress = (m_running && m_suppress.value());
  }
  else {
    m_doSuppress = m_suppress.value();
  }
}

//#############################################################################
/// Finalize Service
StatusCode TrigMessageSvc::finalize() {

  m_running = false;
  m_doSuppress = false;
  resetMsgStats();  // to get the final total sum
  reportMsgStats(m_statLevel.value(),true);
  m_state = Gaudi::StateMachine::OFFLINE;
  return StatusCode::SUCCESS;
}


//#############################################################################
/// Report message counts
void TrigMessageSvc::reportMsgStats(uint statLevel, bool total) {

  std::ostringstream os;

  if (m_stats) {
    os << "Summarizing all message counts";
    if (!total){
      const EventInfo* pEvent(0);
      if ( m_evtStore->retrieve(pEvent).isSuccess() ) {
        os  << " for run " << pEvent->event_ID()->run_number();
      }
    }	
    os << " (severity >= " << levelNames[statLevel] << ")" << endl;
  } else {
    os << "Listing sources of suppressed message: " << endl;
  }

  os << "==========================================================" << endl;
  os << " Message Source                   |   Level |    Count" << endl;
  os << "----------------------------------+---------+-------------" << endl;

  bool found(false);

  std::map<std::string,msgAry>::const_iterator itr = m_sourceMap.begin();
  std::map<std::string,msgAry>::const_iterator enditr = m_sourceMap.end();

  if (total) {
    itr = m_sourceMapTotal.begin();
    enditr = m_sourceMapTotal.end();
  }
  
  for (; itr!=enditr; ++itr) {
    for (unsigned int ic = 0; ic < MSG::NUM_LEVELS; ++ic) {
      if ( (m_suppress.value() && itr->second.msg[ic] >= abs(m_msgLimit[ic]) && m_msgLimit[ic] != 0 ) || 
	   (m_stats && itr->second.msg[ic] > 0 && ic >= statLevel) ) {
	os << " ";
	os.width(33);
	os.setf(ios_base::left,ios_base::adjustfield);
	os << itr->first;

	os << "|";

	os.width(8);
	os.setf(ios_base::right,ios_base::adjustfield);
	os << levelNames[ic];

	os << " |";

	os.width(9);
	os << itr->second.msg[ic];	

	os << endl;

	found = true;
      }
    }
  }
  os << "==========================================================" << endl;

  if (found || m_stats) {
    reportMessage(name(), MSG::INFO, os.str());
  }
}

//#############################################################################
std::string TrigMessageSvc::colTrans(std::string col, int offset) {
  ColorMap::const_iterator itr = m_colMap.find(col);
  int icol;
  if (itr != m_colMap.end()) {
    icol = offset + itr->second;
  } else {
    icol = offset + 8;
  }
  std::ostringstream os1;
      
  os1 << icol;

  return os1.str();

}

//#############################################################################
// ---------------------------------------------------------------------------
// Routine: reportMessage
// Purpose: dispatches a message to the relevant streams.
// ---------------------------------------------------------------------------
//

void TrigMessageSvc::reportMessage( const Message& msg, int outputLevel )    {

  pthread_mutex_lock(&msgsvcmutex);
  int key = msg.getType();
  ++m_msgCount[key];

  // Publish message statistics if enabled and only while RUNNING
  if ( m_publishStats && m_running && key>=static_cast<int>(m_publishLevel) ) {
    m_msgCountHist->Fill(key-m_publishLevel, 1);
    { // Adding bins on the fly needs to be protected by mutex
      scoped_lock_histogram lock;
      m_msgCountSrcHist->Fill(key-m_publishLevel, msg.getSource().c_str(), 1);
      m_msgCountSrcHist->LabelsDeflate("Y");
    }
  }

  const Message *cmsg = &msg;

  if ( m_doSuppress || m_stats.value() ) {
    int nmsg;    
    std::map<std::string,msgAry>::iterator itr = m_sourceMap.find(msg.getSource());
    if ( itr != m_sourceMap.end() ) {
      nmsg = ++(itr->second.msg[key]);
    }
    else {
      msgAry A;
      for (int i=0; i<MSG::NUM_LEVELS; ++i) A.msg[i] = 0;
      A.msg[key] = 1;
      m_sourceMap[msg.getSource()] = A;
      nmsg = 1;
    }
    
    if (m_doSuppress) {
      const int msgLimit = m_msgLimit[key].value();
      
      if ( msgLimit > 0 ) {         // regular suppression
	if (nmsg == msgLimit) {
          std::ostringstream os;
          os << levelNames[key] << " message limit (" << msgLimit << ") reached for "
             << msg.getSource() + ". Suppressing further output.";
	  cmsg = new Message(msg.getSource(), MSG::WARNING, os.str());
	  cmsg->setFormat(msg.getFormat());
          cmsg->setTimeFormat(msg.getTimeFormat());
	}
        else if (nmsg > msgLimit) {
          pthread_mutex_unlock(&msgsvcmutex);
          return;
        }
      }
      else if ( msgLimit < 0 ) {    // logarithmic suppression

        // Use source/level/message as identifier and calculate hash
        const unsigned int msgKey = msgHash(*cmsg);
        MsgCountMap::iterator itr = m_msgCountMap.find(msgKey);
        
        // Check if we saw this message already and increase counter
        if (itr != m_msgCountMap.end())
          nmsg = ++itr->second;
        else
          nmsg = m_msgCountMap[msgKey] = 1;

        if ( nmsg == abs(msgLimit) ) {
          std::ostringstream os;
          os << msg.getMessage() << " [Message limit (" << abs(msgLimit)
             << ") reached. Log-suppression of further output.]";
          cmsg = new Message(msg.getSource(), msg.getType(), os.str());
          cmsg->setFormat(msg.getFormat());
          cmsg->setTimeFormat(msg.getTimeFormat());
        }
        else if ( nmsg > abs(msgLimit) ) {          
          const int everyNth = (int)exp10((int)log10(nmsg));
          if ((nmsg % everyNth)==0) {
            std::ostringstream os;
            os << msg.getMessage() << " [suppressed " << everyNth << " similar messages]";
            cmsg = new Message(msg.getSource(), msg.getType(), os.str());
            cmsg->setFormat(msg.getFormat());
            cmsg->setTimeFormat(msg.getTimeFormat());
          }
          else {
            pthread_mutex_unlock(&msgsvcmutex);
            return;
          }
        }
      }
    }
  }   // suppression

  // Optional message trailer (ignored in hash)
  std::ostringstream msgTrailer;

  // Avoid recursion in case StoreGateSvc::retrieve prints a message
  if ( m_canEnter ) {
    m_canEnter = false;
    if ( m_running &&
         key >= static_cast<int>(m_eventIDLevel.value()) &&
         m_evtStore.retrieve().isSuccess() ) {
      const EventInfo* pEvent(0);
      if ( m_evtStore->retrieve(pEvent).isSuccess() ) {
        // Add EventID
        msgTrailer << " " << *pEvent->event_ID();
      }
    }
    m_canEnter = true;
  }

  StreamMap::const_iterator first = m_streamMap.lower_bound( key );
  if ( first != m_streamMap.end() ) {
    StreamMap::const_iterator last = m_streamMap.upper_bound( key );
    while( first != last ) {
      std::ostream& stream = *( (*first).second.second );
      stream << *cmsg << msgTrailer.str() << std::endl;
      ++first;
    }
  }
  else if ( key >= outputLevel )   {
    cmsg->setFormat(m_defaultFormat);
    cmsg->setTimeFormat(m_defaultTimeFormat);
    std::ostringstream out_string_stream ;
    if (!m_color) {
      (out_string_stream) << *cmsg << msgTrailer.str();
    } else {
      (out_string_stream) <<  m_logColorCodes[key] << *cmsg << msgTrailer.str() << "\033[m" ;
    }

    bool useERS = passErsFilter(cmsg->getSource(), m_useERS[key]);
    
    if ( m_alwaysUseMsgStream.value() || !useERS ) {
      // output to the default stream
      (*m_defaultStream) << out_string_stream.str() << std::endl << std::flush;
    }

    // output to ERS
    if (useERS) {
      
      /*
       * Create ERS context object
       *
       * The (cross-node) MRS throttling is based on filename+line_number, i.e. ignoring
       * the message text itself. We therefor use the message source as filename and the
       * message hash as line_number. That way the same message from different nodes
       * gets properly throttled by MRS.
       */
      const char* filename = cmsg->getSource().c_str();
      unsigned int line_number = msgHash(*cmsg);
      const char* function_name = filename;
      const char* package_name = "HLT";
      ers::LocalContext hlt_context_info(package_name, filename, line_number, function_name);

      /*
       * Create ERS issue object
       *
       * Add the source to the ERS message text. Otherwise the source is not visible
       * in the MRS monitor, for example.
       * The "HLT" qualifier can be used to filter ERS messages from the log file.
       */
      ers::HLTMessage ersMsg(hlt_context_info, cmsg->getSource() + ": " +  cmsg->getMessage() + msgTrailer.str());
      ersMsg.add_qualifier("HLT");   // used for filtering
      
      // forward Gaudi message to ERS
      switch (key) {
        case MSG::NIL:      ers::debug( ersMsg, 3 ); break;
        case MSG::VERBOSE:  ers::debug( ersMsg, 2 ); break;
        case MSG::DEBUG:    ers::debug( ersMsg, 1 ); break;
        case MSG::INFO:     ers::info( ersMsg );     break;
        case MSG::WARNING:  ers::warning( ersMsg );  break;
        case MSG::ERROR:    ers::error( ersMsg );    break;
        case MSG::FATAL:    ers::fatal( ersMsg );    break;
        default:
          std::ostringstream internal_msg ;
          internal_msg << "ERROR: Unknown message severity level: " << key ;
          ers::error( ers::HLTMessage(ERS_HERE,internal_msg.str()) );
      }
    }
  }
  
  if (cmsg != &msg) { delete cmsg; }
  pthread_mutex_unlock(&msgsvcmutex);
}


void TrigMessageSvc::reportMessage( const Message& msg )
{
  reportMessage(msg, outputLevel(msg.getSource()));
}

void TrigMessageSvc::reportMessage (const char* source,
                                int type,
                                const char* message) {
  Message msg( source, type, message);
  reportMessage( msg );
}


void TrigMessageSvc::reportMessage (const std::string& source,
                                int type,
                                const std::string& message) {
  Message msg( source, type, message);
  reportMessage( msg );
}

//#############################################################################
// ---------------------------------------------------------------------------
// Routine: sendMessage
// Purpose: finds a message for a given status code and dispatches it.
// ---------------------------------------------------------------------------
//

void TrigMessageSvc::reportMessage (const StatusCode& key,
                                const std::string& source)
{  
  pthread_mutex_lock(&msgsvcmutex);
  MessageMap::const_iterator first = m_messageMap.lower_bound( key );
  if ( first != m_messageMap.end() ) {
    MessageMap::const_iterator last = m_messageMap.upper_bound( key );
    while( first != last ) {
      Message msg = (*first).second;
      msg.setSource( source );
      std::ostringstream os1;
      os1 << "Status Code " << key.getCode() << std::ends;
      Message stat_code1( source, msg.getType(), os1.str() );
      reportMessage( stat_code1 );
      reportMessage( msg );
      ++first;
    }
  }
  else {
    Message mesg = m_defaultMessage;
    mesg.setSource( source );
      std::ostringstream os2;
    os2 << "Status Code " << key.getCode() << std::ends;
    Message stat_code2( source,  mesg.getType(), os2.str() );
    reportMessage( stat_code2 );
    reportMessage( mesg );
  }
  pthread_mutex_unlock(&msgsvcmutex);
}

//#############################################################################
// ---------------------------------------------------------------------------
// Routine: insertStream
// Purpose: inserts a stream for a message type.
// ---------------------------------------------------------------------------
//

void TrigMessageSvc::insertStream (int key,
                               const std::string& name,
                               std::ostream *stream)
{
  typedef StreamMap::value_type value_type;
  pthread_mutex_lock(&msgsvcmutex);  
  m_streamMap.insert( value_type( key, NamedStream(name,stream) ) );
  pthread_mutex_unlock(&msgsvcmutex);
}

//#############################################################################
// ---------------------------------------------------------------------------
// Routine: eraseStream
// Purpose: erases all the streams for all the message types.
// ---------------------------------------------------------------------------
//

void TrigMessageSvc::eraseStream()
{
  pthread_mutex_lock(&msgsvcmutex);
  m_streamMap.erase( m_streamMap.begin(), m_streamMap.end() );
  pthread_mutex_unlock(&msgsvcmutex);
}

//#############################################################################
// ---------------------------------------------------------------------------
// Routine: eraseStream
// Purpose: erases all the streams for a message type.
// ---------------------------------------------------------------------------
//

void TrigMessageSvc::eraseStream( int message_type )
{
  pthread_mutex_lock(&msgsvcmutex);
  m_streamMap.erase( message_type );
  pthread_mutex_unlock(&msgsvcmutex);
}

//#############################################################################
// ---------------------------------------------------------------------------
// Routine: eraseStream
// Purpose: erases one stream for a message type.
// ---------------------------------------------------------------------------
//

void TrigMessageSvc::eraseStream( int key, std::ostream* stream )   {
  pthread_mutex_lock(&msgsvcmutex);
  if ( 0 != stream )    {
    bool changed = true;
    while( changed ) {
      changed = false;
      StreamMap::iterator first = m_streamMap.lower_bound( key );
      StreamMap::iterator last = m_streamMap.upper_bound( key );
      while( first != last ) {
        if ( (*first).second.second == stream ) {
          m_streamMap.erase( first );
          changed = true;
          break;
        }
      }
    }      
  }
  pthread_mutex_unlock(&msgsvcmutex);
}

//#############################################################################
// ---------------------------------------------------------------------------
// Routine: eraseStream
// Purpose: erases one stream for all message types.
// ---------------------------------------------------------------------------
//

void TrigMessageSvc::eraseStream( std::ostream* stream )    {
  pthread_mutex_lock(&msgsvcmutex);
  if ( 0 != stream )    {
    bool changed = true;
    while( changed ) {
      changed = false;
      StreamMap::iterator first = m_streamMap.begin();
      while( first != m_streamMap.end() ) {
        if ( (*first).second.second == stream ) {
          m_streamMap.erase( first );
          changed = true;
          break;
        }
      }
    }      
  }
  pthread_mutex_unlock(&msgsvcmutex);
}


//#############################################################################
// ---------------------------------------------------------------------------
// Routine: insertMessage
// Purpose: inserts a message for a status code.
// ---------------------------------------------------------------------------
//

void TrigMessageSvc::insertMessage( const StatusCode& key, const Message& msg )
{
  typedef MessageMap::value_type value_type;
  pthread_mutex_lock(&msgsvcmutex);  
  m_messageMap.insert( value_type( key, msg ) );
  pthread_mutex_unlock(&msgsvcmutex);
}

//#############################################################################
// ---------------------------------------------------------------------------
// Routine: eraseMessage
// Purpose: erases all the messages for all the status codes.
// ---------------------------------------------------------------------------
//

void TrigMessageSvc::eraseMessage()
{
  pthread_mutex_lock(&msgsvcmutex);  
  m_messageMap.erase( m_messageMap.begin(), m_messageMap.end() );
  pthread_mutex_unlock(&msgsvcmutex);
}

//#############################################################################
// ---------------------------------------------------------------------------
// Routine: eraseMessage
// Purpose: erases all the messages for a status code.
// ---------------------------------------------------------------------------
//

void TrigMessageSvc::eraseMessage( const StatusCode& key )
{
  pthread_mutex_lock(&msgsvcmutex);
  m_messageMap.erase( key );
  pthread_mutex_unlock(&msgsvcmutex);
}

//#############################################################################
// ---------------------------------------------------------------------------
// Routine: eraseMessage
// Purpose: erases one message for a status code.
// ---------------------------------------------------------------------------
//

void TrigMessageSvc::eraseMessage( const StatusCode& key, const Message& msg )
{
  pthread_mutex_lock(&msgsvcmutex);
  bool changed = true;
  while( changed ) {
    changed = false;
    MessageMap::iterator first = m_messageMap.lower_bound( key );
    MessageMap::iterator last = m_messageMap.upper_bound( key );
    while( first != last ) {
      const Message& message = (*first).second;
      if ( message == msg ) {
        m_messageMap.erase( first );
        changed = true;
        break;
      }
    }      
  }
  pthread_mutex_unlock(&msgsvcmutex);
}

// ---------------------------------------------------------------------------
int TrigMessageSvc::outputLevel()   const {
// ---------------------------------------------------------------------------
  return m_outputLevel;
}
// ---------------------------------------------------------------------------
int TrigMessageSvc::outputLevel( const std::string& source )   const {
// ---------------------------------------------------------------------------
  ThresholdMap::const_iterator it;

  pthread_mutex_lock(&msgsvcmutex);
  it = m_thresholdMap.find( source );
  int level;

  if( it != m_thresholdMap.end() ) {
    level = (*it).second;
  }
  else {
    level = m_outputLevel;
  }
  pthread_mutex_unlock(&msgsvcmutex);
  return level;
}

// ---------------------------------------------------------------------------
void TrigMessageSvc::setOutputLevel(int new_level)    {
// ---------------------------------------------------------------------------
  pthread_mutex_lock(&msgsvcmutex);  
  m_outputLevel = new_level;
  pthread_mutex_unlock(&msgsvcmutex);
}

// ---------------------------------------------------------------------------
void TrigMessageSvc::setOutputLevel(const std::string& source, int level)    {
// ---------------------------------------------------------------------------
  // Ignore in case we enforce the global OutputLevel
  if (m_forceOutputLevel.value()) return;
  
  pthread_mutex_lock(&msgsvcmutex);
  std::pair<ThresholdMap::iterator, bool> p;
  p = m_thresholdMap.insert(ThresholdMap::value_type( source, level) );
  if( p.second == false ) {
    // Already existing an output level for that source. Erase and enter it again
    m_thresholdMap.erase ( p.first );
    m_thresholdMap.insert(ThresholdMap::value_type( source, level) );
  }
  pthread_mutex_unlock(&msgsvcmutex);
}


// ---------------------------------------------------------------------------
void TrigMessageSvc::setOutputLevelViaJobOptSvc() {
// ---------------------------------------------------------------------------
  ServiceHandle<IJobOptionsSvc> jobOptSvc("JobOptionsSvc", name());    
  if ( jobOptSvc.retrieve()==StatusCode::FAILURE ) {
    reportMessage(name(), MSG::ERROR, "Cannot retrieve JobOptionsSvc");
    return;
  }

  std::vector<std::string> clients = jobOptSvc->getClients();
  std::vector<std::string>::iterator cl;
  std::vector<const Property*>::const_iterator pr;
  
  // Loop over all clients/properties and set OutputLevel accordingly
  for (cl=clients.begin(); cl!=clients.end(); ++cl) {
    const std::vector<const Property*> *props = jobOptSvc->getProperties(*cl);
    for (pr=props->begin(); pr!=props->end(); ++pr) {
      if ( (*pr)->name()=="OutputLevel" ) {        
        if ( jobOptSvc->addPropertyToCatalogue(*cl, m_outputLevel).isSuccess() ){
          std::ostringstream msg;
          msg << "Setting " << (*pr)->name() << ".OutputLevel = " << m_outputLevel.value();
          reportMessage(name(), MSG::DEBUG, msg.str());
        }
        else {
          std::ostringstream msg;
          msg << "Cannot set " << (*pr)->name() << ".OutputLevel";
          reportMessage(name(), MSG::WARNING, msg.str());
        }
      }
    }
  }
}
                                                               
// ---------------------------------------------------------------------------
void TrigMessageSvc::resetOutputLevels()    {
// ---------------------------------------------------------------------------
  m_thresholdMap.clear();  
}

// ---------------------------------------------------------------------------
std::string TrigMessageSvc::getLogColor(int logLevel) const   {
// ---------------------------------------------------------------------------
  if (logLevel < MSG::NUM_LEVELS) {
    return m_logColorCodes[logLevel];
  } else {
    return "";
  }
}

// ---------------------------------------------------------------------------
int TrigMessageSvc::messageCount( MSG::Level level) const   {
  return m_msgCount[level];
}
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Calculate message hash (ignoring any numbers)
// ---------------------------------------------------------------------------
unsigned int TrigMessageSvc::msgHash(const Message& msg)
{
  // Message type is lowest decimal
  unsigned int hash = msg.getType();
  
  // Add all characters of source
  const std::string& src = msg.getSource();
  for (size_t i=0; i<src.length(); ++i) {
    hash += 10*(int)src[i];
  }

  // Add all letters of message
  const std::string& str = msg.getMessage();
  for (size_t i=0; i<str.length(); ++i) {
    const char& ch = str[i];
    if (ch<',' || ch>'9') hash += 10*(int)ch;
  }
     
  return hash;
}
