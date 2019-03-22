/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#include "TrigMessageSvc.h"
#include "GaudiKernel/IAppMgrUI.h"
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/Message.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/System.h"

#include "ers/ers.h"

#include <fstream>
#include <iostream>
#include <sstream>

// Declare ERS issue type
ERS_DECLARE_ISSUE(ers, HLTMessage, , )

static const std::string levelNames[MSG::NUM_LEVELS] = {"NIL",     "VERBOSE", "DEBUG", "INFO",
                                                        "WARNING", "ERROR",   "FATAL", "ALWAYS"};

namespace {
  /// Calculate message hash ignoring any digits
  size_t msgHash(const Message& msg)
  {
    std::string s = msg.getSource() + msg.getMessage();
    s.erase(std::remove_if(s.begin(), s.end(), [](char c) { return std::isdigit(c); }), s.end());
    return std::hash<std::string>()(s);
  }
} // namespace

TrigMessageSvc::TrigMessageSvc(const std::string& name, ISvcLocator* svcloc) :
    base_class(name, svcloc)
{
  m_outputLevel.declareUpdateHandler([svcloc](Gaudi::Details::PropertyBase&) {
    SmartIF<IAppMgrUI> app = svcloc;
    if (app) app->outputLevelUpdate();
  });

  for (int ic = 0; ic < MSG::NUM_LEVELS; ++ic) {
    m_msgLimit[ic].declareUpdateHandler(&TrigMessageSvc::setupLimits, this);
    m_thresholdProp[ic].declareUpdateHandler(&TrigMessageSvc::setupThreshold, this);
  }

  std::fill(std::begin(m_msgCount), std::end(m_msgCount), 0);
}

StatusCode TrigMessageSvc::initialize()
{
  StatusCode sc = Service::initialize();
  if (sc.isFailure()) return sc;

  m_doSuppress = m_suppress;

  if (m_color) {
    std::cout << "TrigMessageSvc WARNING: Colors are not supported by TrigMessageSvc" << std::endl;
  }
  return StatusCode::SUCCESS;
}

StatusCode TrigMessageSvc::reinitialize()
{
  m_state = Gaudi::StateMachine::OFFLINE;
  return initialize();
}

void TrigMessageSvc::setupLimits(Gaudi::Details::PropertyBase& prop)
{
  // Just report problems in the settings of the limits and unknown limit
  // parameters
  if (prop.name() == "alwaysLimit") {
    Gaudi::Property<int>* p = dynamic_cast<Gaudi::Property<int>*>(&prop);
    if (p && p->value() != 0) {
      std::cout << "TrigMessageSvc ERROR: cannot suppress ALWAYS messages" << std::endl;
      p->setValue(0);
    }
  }
  else if (prop.name() == "defaultLimit") {
    for (int i = MSG::VERBOSE; i < MSG::NUM_LEVELS; ++i) {
      if (i != MSG::ALWAYS) {
        m_msgLimit[i] = m_msgLimit[MSG::NIL].value();
      }
    }
  }
  else if (prop.name() != "fatalLimit" && prop.name() != "errorLimit" &&
           prop.name() != "warningLimit" && prop.name() == "infoLimit" &&
           prop.name() == "debugLimit" && prop.name() == "verboseLimit") {
    std::cout << "TrigMessageSvc ERROR: Unknown message limit parameter: " << prop.name()
              << std::endl;
    return;
  }
}

void TrigMessageSvc::setupThreshold(Gaudi::Details::PropertyBase& prop)
{
  static const std::array<std::pair<const char*, MSG::Level>, 7> tbl{{{"setFatal", MSG::FATAL},
                                                                      {"setError", MSG::ERROR},
                                                                      {"setWarning", MSG::WARNING},
                                                                      {"setInfo", MSG::INFO},
                                                                      {"setDebug", MSG::DEBUG},
                                                                      {"setVerbose", MSG::VERBOSE},
                                                                      {"setAlways", MSG::ALWAYS}}};

  auto i = std::find_if(
      std::begin(tbl), std::end(tbl),
      [&](const std::pair<const char*, MSG::Level>& t) { return prop.name() == t.first; });
  if (i == std::end(tbl)) {
    std::cerr << "TrigMessageSvc ERROR: Unknown message threshold parameter: " << prop.name()
              << std::endl;
    return;
  }
  int ic = i->second;

  Gaudi::Property<std::vector<std::string>>* sap =
      dynamic_cast<Gaudi::Property<std::vector<std::string>>*>(&prop);
  if (!sap) {
    std::cerr << "could not dcast " << prop.name()
              << " to a Gaudi::Property<std::vector<std::string>> (which it "
                 "should be!)"
              << std::endl;
  }
  else {
    for (auto& i : sap->value()) setOutputLevel(i, ic);
  }
}

StatusCode TrigMessageSvc::finalize()
{
  m_suppress = false;
  std::ostringstream os;

  if (m_stats) {
    os << "Summarizing all message counts"
       << " (severity >= " << levelNames[m_statLevel] << ")" << std::endl;
  }
  else {
    os << "Listing sources of suppressed message: " << std::endl;
  }

  os << "=====================================================" << std::endl;
  os << " Message Source              |   Level |    Count" << std::endl;
  os << "-----------------------------+---------+-------------" << std::endl;

  bool found(false);

  for (auto itr = m_sourceMap.begin(); itr != m_sourceMap.end(); ++itr) {
    for (unsigned int ic = 0; ic < MSG::NUM_LEVELS; ++ic) {
      if ((m_suppress && itr->second.msg[ic] >= abs(m_msgLimit[ic]) && m_msgLimit[ic] != 0) ||
          (m_stats && itr->second.msg[ic] > 0 && ic >= m_statLevel.value())) {
        os << " ";
        os.width(28);
        os.setf(std::ios_base::left, std::ios_base::adjustfield);
        os << itr->first;
        os << "|";

        os.width(8);
        os.setf(std::ios_base::right, std::ios_base::adjustfield);
        os << levelNames[ic];
        os << " |";

        os.width(9);
        os << itr->second.msg[ic];
        os << std::endl;

        found = true;
      }
    }
  }
  os << "=====================================================" << std::endl;
  if (found || m_stats) {
    reportMessage(name(), MSG::INFO, os.str());
  }

  return StatusCode::SUCCESS;
}

void TrigMessageSvc::reportMessage(const Message& msg, int outputLevel)
{
  std::unique_lock<std::recursive_mutex> lock(m_reportMutex);
  i_reportMessage(msg, outputLevel);
}

/**
 *  Internal implementation of reportMessage(const Message&,int) without lock.
 */
void TrigMessageSvc::i_reportMessage(const Message& msg, int outputLevel)
{
  const int key = msg.getType();
  ++m_msgCount[key];

  const Message* cmsg = &msg;

  if (m_doSuppress || m_stats.value()) {

    // Increase the counter of 'key' type of messages for the source and
    // get the new value.
    int nmsg = ++(m_sourceMap[msg.getSource()].msg[key]);

    const int msgLimit = m_msgLimit[key].value();
    if (m_doSuppress) {
      if (msgLimit > 0) { // regular suppression
        if (nmsg > msgLimit) return;
        if (nmsg == msgLimit) {
          std::string txt = levelNames[key] + " message limit (" + std::to_string(msgLimit) +
                            ") reached for " + msg.getSource() + ". Suppressing further output.";
          cmsg = new Message(msg.getSource(), MSG::WARNING, std::move(txt));
        }
      }
    }
    else if (msgLimit < 0) { // logarithmic suppression
      // Calculate message hash
      const unsigned int mh = msgHash(*cmsg);
      // Check if we saw this message already and increase counter
      auto m = m_msgHashCount.find(mh);
      if (m != m_msgHashCount.end()) {
        nmsg = ++m->second;
      }
      else {
        nmsg = m_msgHashCount[mh] = 1;
      }
      if (nmsg == abs(msgLimit)) {
        std::ostringstream os;
        os << msg.getMessage() << " [Message limit (" << abs(msgLimit)
           << ") reached. Log-suppression of further output.]";
        cmsg = new Message(msg.getSource(), msg.getType(), os.str());
      }
      else if (nmsg > abs(msgLimit)) {
        const int everyNth = (int)exp10((int)log10(nmsg));
        if ((nmsg % everyNth) == 0) {
          std::ostringstream os;
          os << msg.getMessage() << " [suppressed " << everyNth << " similar messages]";
          cmsg = new Message(msg.getSource(), msg.getType(), os.str());
        }
      }
    }
  }

  // Print the message
  if (key >= outputLevel) {
    if (m_eventIDLevel != MSG::NIL && key >= static_cast<int>(m_eventIDLevel)) {
      cmsg->setFormat(m_defaultFormat + " %E");
    }
    else {
      cmsg->setFormat(m_defaultFormat);
    }
    cmsg->setTimeFormat(m_defaultTimeFormat);
    (*m_defaultStream) << *cmsg << std::endl << std::flush;

    // ERS forwarding
    if (passErsFilter(cmsg->getSource(), m_useERS[key])) {
      i_reportERS(*cmsg);
    }
  }

  if (cmsg != &msg) {
    delete cmsg;
  }
}

/**
 * Report message to online messaging system (ERS)
 */
void TrigMessageSvc::i_reportERS(const Message& msg) const
{
  /*
   * Create ERS context object
   *
   * The (cross-node) MRS throttling is based on filename+line_number, i.e.
   * ignoring the message text itself. We therefor use the message source as
   * filename and the message hash as line_number. That way the same message
   * from different nodes gets properly throttled by MRS.
   */
  const char* filename = msg.getSource().c_str();
  const char* function_name = "";
  const int line_number = msgHash(msg);
  const char* package_name = "HLT";
  ers::LocalContext hlt_context_info(package_name, filename, line_number, function_name);

  // Create ERS issue object
  Message m(msg);
  if (m_eventIDLevel != MSG::NIL && msg.getType() >= static_cast<int>(m_eventIDLevel)) {
    m.setFormat(m_ersFormat + " %E");
  }
  else {
    m.setFormat(m_ersFormat);
  }
  std::ostringstream oss;
  oss << m;
  ers::HLTMessage ersMsg(hlt_context_info, oss.str());
  ersMsg.add_qualifier("HLT"); // useful for filtering

  // Forward Message to ERS
  switch (msg.getType()) {
  case MSG::NIL: break;
  case MSG::VERBOSE: ers::debug(ersMsg, 2); break;
  case MSG::DEBUG: ers::debug(ersMsg, 1); break;
  case MSG::INFO: ers::info(ersMsg); break;
  case MSG::WARNING: ers::warning(ersMsg); break;
  case MSG::ERROR: ers::error(ersMsg); break;
  case MSG::FATAL: ers::fatal(ersMsg); break;
  default:
    std::ostringstream oss;
    oss << "Unknown message severity level: " << msg.getType() << " Original message was: " << m;
    ers::error(ers::HLTMessage(ERS_HERE, oss.str()));
  }
}

void TrigMessageSvc::reportMessage(const Message& msg)
{
  reportMessage(msg, outputLevel(msg.getSource()));
}

void TrigMessageSvc::reportMessage(const char* source, int type, const char* message)
{
  reportMessage(Message{source, type, message});
}

void TrigMessageSvc::reportMessage(const std::string& source, int type, const std::string& message)
{
  reportMessage(Message{source, type, message});
}

int TrigMessageSvc::outputLevel() const
{
  return m_outputLevel;
}

int TrigMessageSvc::outputLevel(const std::string& source) const
{
  std::unique_lock<std::recursive_mutex> lock(m_thresholdMapMutex);
  auto it = m_thresholdMap.find(source);
  return it != m_thresholdMap.end() ? it->second : m_outputLevel.value();
}

void TrigMessageSvc::setOutputLevel(int new_level)
{
  m_outputLevel = new_level;
}

void TrigMessageSvc::setOutputLevel(const std::string& source, int level)
{
  std::unique_lock<std::recursive_mutex> lock(m_thresholdMapMutex);

  // only write if we really have to...
  auto i = m_thresholdMap.find(source);
  if (i == m_thresholdMap.end()) {
    m_thresholdMap[source] = level;
  }
  else if (i->second != level) {
    i->second = level;
  }
}

void TrigMessageSvc::resetOutputLevels()
{
  std::unique_lock<std::recursive_mutex> lock(m_thresholdMapMutex);
  m_thresholdMap.clear();
}

int TrigMessageSvc::messageCount(MSG::Level level) const
{
  return m_msgCount[level];
}

bool TrigMessageSvc::passErsFilter(const std::string& source,
                                   const std::vector<std::string>& filter) const
{
  if (filter.empty()) return false; // forward none
  auto it = filter.begin();
  if (filter.size() == 1 && (*it) == "*") return true; // forward all

  bool pass(false);
  for (; it != filter.end(); ++it) {
    if ((*it) == "*") pass = true;           // forward except if there is a veto later
    if (source == (*it)) return true;        // forward specific source
    if ("!" + source == (*it)) return false; // veto specific source
  }
  return pass;
}
