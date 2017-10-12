/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file TrigPSC/src/Config.cxx
 * @author Frank Winklmeier, Werner Wiedenmann, Ricardo Abreu
 *
 * @brief PSC Configuration type
 */

#include "TrigPSC/Config.h"
#include "TrigPSC/PscIssues.h"
#include "TrigConfBase/TrigDBConnectionConfig.h"
#include "GaudiKernel/IMessageSvc.h"
#include "eformat/SourceIdentifier.h"
#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>
#include <array>
#include <climits>
#include <unistd.h>

using std::string;
using std::array;
using std::ostringstream;
using TrigConf::TrigDBConnectionConfig;
using namespace boost::property_tree;

namespace
{
  //////////////////////////////////////////////////////////////////////////////
  const string cf_path    = "Configuration.Partition.TriggerConfiguration"
                            ".TriggerConfiguration";
  const string hlt_path   = cf_path + ".hlt";
  const string db_path    = cf_path+".TriggerDBConnection.TriggerDBConnection";
  const string l1_path    = cf_path + ".L1TriggerConfiguration"
                            ".L1TriggerConfiguration";
  const string r2r_path   = "Configuration.ROS2ROBS";
  const string athlt_path = cf_path+".athenaHLTSpecificConfiguration";

  //////////////////////////////////////////////////////////////////////////////
  const ptree::value_type& getHltConfigImpl(const ptree& config)
  {
    const ptree& hltconf = config.get_child(hlt_path);
    if(hltconf.size() != 1)
    {
      ostringstream oss;
      oss << "Got " << hltconf.size() << " elements of HLTImplementation in the"
          " TriggerConfiguration, when exactly 1 element was expected";
      throw psc::ConfigurationIssue(ERS_HERE, oss.str().c_str());
    }

    return hltconf.front();
  }

  //////////////////////////////////////////////////////////////////////////////
  string plevelToStr(const ptree& level, const string& delim = ";")
  {
    string ret;
    for(const auto& item : level)
    {
      if(ret.size() != 0)
        ret.append(delim);
      ret.append(item.second.data());
    }

    return ret;
  }

  //////////////////////////////////////////////////////////////////////////////
  string plevelToStr(const boost::optional<const ptree&>& level,
                     const string& delim = ";")
  {
    return level ? plevelToStr(*level, delim) : "";
  }

  //////////////////////////////////////////////////////////////////////////////
  string log_level_as_num_str(const std::string& loglevel)
  {
    constexpr auto size = 7ul;
    constexpr array<const char*, size> lvl_keys = {{"VERBOSE",
                                                    "DEBUG",
                                                    "INFO",
                                                    "WARNING",
                                                    "ERROR",
                                                    "FATAL",
                                                    "ALWAYS"}};
    constexpr array<int, size> lvl_values = {{MSG::VERBOSE,
                                              MSG::DEBUG,
                                              MSG::INFO,
                                              MSG::WARNING,
                                              MSG::ERROR,
                                              MSG::FATAL,
                                              MSG::ALWAYS}};
    for(auto i = 0u; i < size; ++i)
      if(loglevel.find(lvl_keys[i]) != string::npos) // key is a substr of level
        return std::to_string(lvl_values[i]); // the first level that is found
                                              // is the most verbose one

    return "0";
  }
}

////////////////////////////////////////////////////////////////////////////////
psc::Config::Config(const ptree& config)
  : m_config(config)
{
  setup_optmap_defaults();

  fill_enabled_robs(config.get_child(r2r_path));
  fill_enabled_dets(config.get_child(r2r_path));

  const ptree::value_type& hltimpl = getHltConfigImpl(config);
  if(hltimpl.first == "HLTImplementationJobOptions")
  {
    ERS_DEBUG(1, "Job Options configuration");
    fillopt_jo(hltimpl.second);
  }
  else if(hltimpl.first == "HLTImplementationDB")
  {
    ERS_DEBUG(1, "DB configuration");
    fillopt_db(hltimpl.second);
  }
  else if(hltimpl.first == "HLTImplementationDBPython")
  {
    ERS_DEBUG(1, "DBPY configuration");
    ERS_PSC_WARNING("HLT configured with HLTImplementationDBPython object. This"
          " is for offline purposes only. Do not use this for online running!");
    fillopt_dbpy(hltimpl.second);
  }
  else
  {
    string msg = "Unknown HLTImplementation type: " + hltimpl.first;
    throw psc::ConfigurationIssue(ERS_HERE, msg.c_str());
  }

  fillopt_athenaHLT();
  setPInfo();
}

////////////////////////////////////////////////////////////////////////////////
void psc::Config::prepareWorker(const boost::property_tree::ptree& args)
{
  updatePids();
  updateSeed();

  optmap["DF_APPLICATIONNAME"] = args.get_child("appName").data();
  optmap["DF_WORKER_ID"] = args.get_child("workerId").data();
  optmap["DF_NUMBER_OF_WORKERS"] = args.get_child("numberOfWorkers").data();
}

////////////////////////////////////////////////////////////////////////////////
std::string psc::Config::dumpOptions() const
{
  std::ostringstream ost;
  ost << " --- Dump of all options held by Config --- total size = " << optmap.size() <<"\n";
  unsigned pos = 0;
  for(std::map<std::string, std::string>::const_iterator
      it = optmap.begin(); it != optmap.end(); ++it, ++pos) {
    ost << " # = " << std::setw(3) << pos
        << " Option : " << "'" << it->first
        << "' = '" << it->second << "'\n";
  }

  // Print all enabled ROB ids
  ost << " --- Number of enabled ROB IDs read from OKS             = " << enabled_robs.size() << "\n";
  ost << "List of enabled ROBs: [";
  bool first = true;
  int index(0);
  eformat::SubDetector previous_subDet(eformat::OTHER);
  for(auto rob : enabled_robs)
  {
    if(!first) ost << ", ";
    if (eformat::helper::SourceIdentifier(rob).subdetector_id() != previous_subDet) {
      previous_subDet = eformat::helper::SourceIdentifier(rob).subdetector_id();
      index=0;
    }
    if(index%10==0) ost<<"\n"  ;
    ost << " 0x" << std::setw(6) << std::hex << rob << std::dec;
    first = false;
    ++index;
  }
  ost << "]\n";

  // Print all enabled Subdetectors ids
  ost << " --- Number of enabled Sub Detector configured = "
      << enabled_SubDets.size() << "\n";
  ost << "List of enabled Sub-Detectors: [\n";
  first = true;
  index = 0;
  for(auto det : enabled_SubDets)
  {
    if(!first)
      ost << ",\n";
    ++index;
    ost << std::setw(4) << index << ": 0x" << std::setw(2) << std::hex << det << std::dec
        << " (" << std::setw(26) << eformat::helper::SourceIdentifier((eformat::SubDetector)det,0).human_detector() << " )";
    first = false;
  }
  ost << "]";

  return ost.str();
}

////////////////////////////////////////////////////////////////////////////////
std::string psc::Config::getOption(const std::string& key) const 
{
  std::map<std::string, std::string>::const_iterator it = optmap.find(key); 
  if (it == optmap.end()) {
    ERS_PSC_WARNING("Could not find requested option = " << key) ;
    ERS_DEBUG(1, " " << dumpOptions() ) ;
    return "";
  } 
  return it->second;
}

////////////////////////////////////////////////////////////////////////////////
std::string psc::Config::toPython(const std::string& dictName) const
{
  ostringstream oss;
  oss << dictName << " = {";
  bool first(true);
  std::map<std::string, std::string>::const_iterator it = optmap.begin();
  for(;it != optmap.end(); ++it) {
    if (!first) {
      oss << ",";
    }
    oss << "'" << it->first << "':'" << it->second << "'";
    first = false;         
  }
  oss << "}" << std::endl;
  return oss.str();
}

////////////////////////////////////////////////////////////////////////////////
std::string psc::Config::getLogLevelAsNumStr() const
{
  return log_level_as_num_str(getOption("LOGLEVEL"));
}

bool psc::Config::didUserSetLogLevel() const
{
  // Ugly since it relies on the default value being "INFO"
  // (and wrong if the user uses explicitlty requests "INFO")
  return getOption("LOGLEVEL").substr(0,4) != "INFO";
}

////////////////////////////////////////////////////////////////////////////////
void psc::Config::setup_optmap_defaults()
{
  optmap["SHOWINCLUDE"]       = "False";
  optmap["EVENTSELECTOR"]     = "NONE";
  optmap["LOGLEVEL"]          = "INFO";
  optmap["TRACEPATTERN"]      = "";
  optmap["PRECOMMAND"]        = "";
  optmap["POSTCOMMAND"]       = "";
  optmap["MUONCALBUFFERNAME"] = "";
  optmap["MUONCALBUFFERSIZE"] = "";
}

////////////////////////////////////////////////////////////////////////////////
void psc::Config::fillopt_jo(const ptree& hlt)
{
  optmap["JOBOPTIONSPATH"]  = hlt.get_child("jobOptionsPath").data();
  optmap["EVENTSELECTOR"]   = hlt.get_child("evtSel").data();
  optmap["SHOWINCLUDE"]     = hlt.get_child("showInclude").data();
  optmap["TRACEPATTERN"]    = hlt.get_child("tracePattern").data();
  optmap["PYTHONSETUPFILE"] = hlt.get_child("pythonSetupFile").data();
  optmap["JOBOPTIONSTYPE"]  = "NONE";
  optmap["LOGLEVEL"]        = plevelToStr(hlt.get_child_optional("logLevels"),
                                          ",");

  fillopt_py(hlt);
  fillopt_common(hlt);
}

////////////////////////////////////////////////////////////////////////////////
void psc::Config::fillopt_db(const ptree& hlt)
{
  TrigDBConnectionConfig dbconf;
  fill_dbCon(hlt, dbconf);

  optmap["JOBOPTIONSPATH"] = dbconf.toString();
  optmap["JOBOPTIONSTYPE"] = "DB";

  fillopt_common(hlt);
}

////////////////////////////////////////////////////////////////////////////////
void psc::Config::fillopt_dbpy(const ptree& hlt)
{
  fillopt_db(hlt);
  fillopt_py(hlt);
}

////////////////////////////////////////////////////////////////////////////////
void psc::Config::fillopt_py(const ptree& hlt)
{
  optmap["PRECOMMAND"]  = plevelToStr(hlt.get_child_optional("preCommands"));
  optmap["POSTCOMMAND"] = plevelToStr(hlt.get_child_optional("postCommands"));
}

////////////////////////////////////////////////////////////////////////////////
void psc::Config::fillopt_common(const ptree& hlt)
{
  boost::optional<const ptree&> circbuf = m_config.get_child_optional(
      "Configuration.HLTMPPUApplication.MuonCalibrationConfig.CircBuffer");
  if(circbuf)
  {
    optmap["MUONCALBUFFERNAME"] = circbuf->get_child("CircName").data();
    optmap["MUONCALBUFFERSIZE"] = circbuf->get_child("CircSize").data();
  }

  const ptree& com = hlt.get_child("HLTCommonParameters.HLTCommonParameters");
  optmap["MESSAGESVCTYPE"] = com.get_child("messageSvcType").data();
  optmap["JOBOPTIONSSVCTYPE"] = com.get_child("jobOptionsSvcType").data();
  optmap["DLLNAME"] = com.get_child("dllName").data();
  optmap["FACTORYNAME"] = com.get_child("factoryName").data();
  optmap["DF_PARTITION_NAME"] = m_config.get_child("Configuration.Partition."
                                                   "UID").data();
  optmap["DF_APPLICATIONNAME"] =
      m_config.get_child("Configuration.HLTMPPUApplication.UID").data();
}

////////////////////////////////////////////////////////////////////////////////
void psc::Config::fillopt_athenaHLT()
{
  const auto& ath_hlt = m_config.get_child_optional(athlt_path);
  if(ath_hlt)
  {
    const auto& llnode = ath_hlt->get_child_optional("logLevels");
    optmap["LOGLEVEL"] = plevelToStr(llnode, ",");

    const auto& psnode = ath_hlt->get_child_optional("pythonSetupFile");
    if(psnode)
      optmap["PYTHONSETUPFILE"] = psnode->data();
  }
}

////////////////////////////////////////////////////////////////////////////////
void psc::Config::setPInfo()
{
  char cstr_host[HOST_NAME_MAX];
  gethostname(cstr_host, sizeof(cstr_host));
  optmap["DF_MACHINE_NAME"] = string(cstr_host);

  ostringstream oss;
  oss << "0x" << std::hex << gethostid() ;
  optmap["DF_HOST_ID"] = oss.str();

  updatePids();
  updateSeed();
}

////////////////////////////////////////////////////////////////////////////////
void psc::Config::fill_enabled_robs(const ptree& r2r)
{
  std::set<uint32_t> set_enabled_robs;
  for(const auto& ros : r2r)
    for(const auto& rob: ros.second)
      set_enabled_robs.insert(rob.second.get_value<uint32_t>());

  enabled_robs.reserve(set_enabled_robs.size());
  for(const auto& it_rob: set_enabled_robs) enabled_robs.push_back(it_rob);
}

////////////////////////////////////////////////////////////////////////////////
void psc::Config::fill_enabled_dets(const ptree& r2r)
{
  std::set<eformat::SubDetector> set_enabled_subDets;
  for(const auto& ros : r2r)
    for(const auto& rob: ros.second)
      set_enabled_subDets.insert(eformat::helper::SourceIdentifier(rob.second.get_value<uint32_t>()).subdetector_id());

  enabled_SubDets.reserve(set_enabled_subDets.size());
  for(const auto& it_det: set_enabled_subDets) enabled_SubDets.push_back(it_det);
}

////////////////////////////////////////////////////////////////////////////////
void psc::Config::fill_dbCon(const ptree& hlt, TrigDBConnectionConfig& dbcon)
{
  const auto& dbpt = m_config.get_child(db_path);
  const auto& alias = dbpt.get_child("Alias").data();

  if(alias.empty())
  {
    dbcon.m_server = dbpt.get_child("Server").data();
    dbcon.m_user = dbpt.get_child("User").data();
    dbcon.m_password = dbpt.get_child("Password").data();
    dbcon.m_schema = dbpt.get_child("Name").data();
  }
  else
    dbcon.m_server = alias;

  dbcon.setTypeFromStr(dbpt.get_child("Type").data());
  dbcon.setSmKeyFromStr(dbpt.get_child("SuperMasterKey").data());
  dbcon.setHltKeysFromStr(hlt.get_child("hltPrescaleKey").data());
  dbcon.setLvl1KeyFromStr(m_config.get_child(l1_path + ".Lvl1PrescaleKey").data());
  dbcon.diggestStr(plevelToStr(
        hlt.get_child_optional("additionalConnectionParameters"), ":"));
}

////////////////////////////////////////////////////////////////////////////////
void psc::Config::updatePids()
{
  optmap["DF_PID"] = boost::lexical_cast<string>(getpid());
  optmap["DF_PPID"] = boost::lexical_cast<string>(getppid());
}

////////////////////////////////////////////////////////////////////////////////
void psc::Config::updateSeed()
{
  // this is a string representing a number, in hexadecimal format, that fits
  // in an int. The leftmost sizeof(int)/2 bytes of this number are the
  // rightmost sizeof(int)/2 bytes of the host id of this machine. The rightmost
  // sizeof(int)/2 bytes of this number are the rightmost sizeof(int)/2 bytes of
  // this process's id.
  //
  // e.g. if we are running in a machine where sizeof(int) is 4, the machine's
  // host id is 0xffffffff12344321 and this process's id is 0xabcddcba,
  // the random seed becomes 0x4321dcba

  unsigned int extract_mask = 0;
  for(unsigned i = 0; i < sizeof(int)/2; ++i)
    extract_mask |= UCHAR_MAX << (i*CHAR_BIT);

  ostringstream oss;

  oss << "0x" << std::hex << (gethostid() & extract_mask)
                          << (getpid() & extract_mask);

  optmap["DF_RANDOM_SEED"] = oss.str();
}
