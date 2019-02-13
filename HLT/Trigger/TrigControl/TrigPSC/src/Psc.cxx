/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file Psc.cxx
 * @author Frank Winklmeier, Ricardo Abreu
 *
 * @brief Base class for HLT Pesa Steering Controller.
 *
 */

#include "TrigPSC/Psc.h"
#include "TrigPSC/Config.h"
#include "TrigPSC/PscIssues.h"
#include "TrigPSC/Utils.h"
#include "TrigKernel/ITrigEventLoopMgr.h"

#include "eformat/ROBFragment.h"
#include "ers/ers.h"

#undef _POSIX_C_SOURCE
#undef _XOPEN_SOURCE
#include <Python.h>

// Include files for Gaudi
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IAppMgrUI.h"
#include "GaudiKernel/IEventProcessor.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/IAlgManager.h"
#include "GaudiKernel/IAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/System.h"

// Athena includes
#include "RDBAccessSvc/IRDBAccessSvc.h"

// CORAL includes
#include "CoralKernel/Context.h"
#include "RelationalAccess/IConnectionService.h"

#include <sstream>
#include <algorithm>

#include <boost/lexical_cast.hpp>
#include <boost/property_tree/xml_parser.hpp>

using namespace boost::property_tree;
using std::vector;
using std::string;
using eformat::ROBFragment;

namespace
{
  //////////////////////////////////////////////////////////////////////////////
  string to_string(const ptree& p)
  {
    using T = ptree::key_type;
    std::ostringstream oss;
    xml_parser::write_xml(oss, p,
        xml_parser::xml_writer_make_settings<T>(' ', 2));
    return oss.str();
  }
}

//--------------------------------------------------------------------------------
// C'tor and D'tor
//--------------------------------------------------------------------------------
psc::Psc::Psc () :
      m_run_number(0),
      m_pesaAppMgr(0),
      m_nameEventLoopMgr("EventLoopMgr"),
      m_interactive(false),
      m_config(0)
{
}

psc::Psc::~Psc ()
{
  if (m_pesaAppMgr) { 
    m_pesaAppMgr->release() ; 
  }

  delete m_config; m_config = 0;
}

//--------------------------------------------------------------------------------
// Configure transition
//--------------------------------------------------------------------------------

bool psc::Psc::configure(const ptree& config)
{
  psc::Utils::ScopeTimer timer("Psc configuration");

  ERS_DEBUG(1, "psc::Psc::configure ptree:\n" << to_string(config));
  try
  {
    m_config = new Config(config);
  }
  catch(const std::exception& e)
  {
    ERS_PSC_ERROR("Cannot configure. " << e.what());
    return false;
  }

  // Print PSC configuration
  ERS_LOG("---> Dump of config cache:   \n" << m_config->dumpOptions() );
  ERS_LOG("---> Pesa JobOptions file is = " << m_config->getOption("JOBOPTIONSPATH") );

  // -----------------------------
  // Create C++ ApplicationMgr
  // -----------------------------
  ERS_DEBUG(1,"---> Create Pesa Application Manager");
  m_pesaAppMgr = Gaudi::createApplicationMgr();
  ERS_DEBUG(1,"m_pesaAppMgr = " << m_pesaAppMgr);

  if( !m_pesaAppMgr ) {
    ERS_PSC_ERROR("Error while creating the ApplicationMgr");
    return false;
  }

  // configure Pesa
  ERS_DEBUG(1,"---> Configure Pesa Property Manager");
  SmartIF<IProperty> propMgr ( m_pesaAppMgr );
  if( !propMgr.isValid() ) {
    ERS_PSC_ERROR("Error retrieving IProperty interface of ApplicationMgr");
    return false;
  }

  // Set the JobOptionsSvcType
  StatusCode sc;
  sc = propMgr->setProperty( "JobOptionsSvcType", m_config->getOption("JOBOPTIONSSVCTYPE"));
  if (sc.isFailure()) {
    ERS_PSC_ERROR("Can not set Property = JobOptionsSvcType");
    return false;
  }

  // Set the JobOptionsType
  sc = propMgr->setProperty( "JobOptionsType", m_config->getOption("JOBOPTIONSTYPE"));
  if (sc.isFailure()) {
    ERS_PSC_ERROR("Can not set Property = JobOptionsType");
    return false;
  }

  // Set JobOptionsPath
  sc = propMgr->setProperty( "JobOptionsPath", m_config->getOption("JOBOPTIONSPATH") );
  if (sc.isFailure()) {
    ERS_PSC_ERROR("Can not set Property = JobOptionsPath");
    return false;
  }

  // Select the MessageSvc Type
  sc = propMgr->setProperty( "MessageSvcType", m_config->getOption("MESSAGESVCTYPE"));
  if (sc.isFailure()) {
    ERS_PSC_ERROR("Can not set Property = MessageSvcType");
    return false;
  }

  // Need to setup the python interpreter if:
  //  1) run from job options
  //  2) run from DB but user specified a pre/postcommand
  //  3) run from DB but user specified a logLevel via "-l" in athenaMT/PT
  bool jobOptConfig, needPython;
  if ( m_config->getOption("JOBOPTIONSTYPE") == "NONE" ) {
    jobOptConfig = needPython = true;
  }
  else if ( m_config->getOption("JOBOPTIONSTYPE") == "DB" ) {
    jobOptConfig = needPython = false;
    if ( (m_config->getOption("PRECOMMAND")!="") || (m_config->getOption("POSTCOMMAND")!="") ) {
      needPython = true;
    }
    if (m_config->didUserSetLogLevel()) {
      needPython = true;
    }
  }
  else {
    ERS_PSC_ERROR("Unsupported configuration method \"" << m_config->getOption("JOBOPTIONSTYPE") << "\"");
    return false;
  }


  if ( jobOptConfig || needPython ) {
    // ----------------------------
    // Configuration with .py files
    // ----------------------------

    // Try to initialize the Python interpreter
    if ( ! Py_IsInitialized() ) {
      ERS_DEBUG(1,"Initializing Python interpreter");

      if (!PyEval_ThreadsInitialized()) PyEval_InitThreads();
      Py_Initialize();

      // check
      if ( ! Py_IsInitialized() ) {
        ERS_PSC_ERROR("Error: Python could not been intialized.");
        return false;
      }

      // init the sys.argv...
      PySys_SetArgv(System::argc(),System::argv());
    }
    else {
      ERS_DEBUG(1,"Python interpreter already initialized");
    }  

    // Copy Config.optmap to python module
    // This is how we transfer the options to the python setup script
    PyObject* pModule = PyImport_ImportModule("TrigPSC.PscConfig");
    if ( pModule ) {
      PyObject* optmap = PyObject_GetAttrString(pModule, "optmap");
      if ( optmap ) {
        std::map<std::string,std::string>::const_iterator iter;
        for (iter=m_config->optmap.begin(); iter!=m_config->optmap.end(); ++iter) {
          PyObject* v = PyString_FromString(iter->second.c_str());
          std::vector<char> writable(iter->first.size() + 1);
          std::copy(iter->first.begin(), iter->first.end(), writable.begin());
          PyMapping_SetItemString(optmap, &writable[0], v);
        }
        Py_DECREF(optmap);
      }
      else {
        ERS_DEBUG(1, "Could not import TrigPSC.PscConfig.optmap");
      }
      Py_DECREF(pModule);
    }

    // Setup the file inclusion
    std::vector<std::string> pyCmds;
    pyCmds.push_back("from AthenaCommon.Include import include");
    // have C++ bool representation be recognized in python:
    pyCmds.push_back("true, false = True, False");
    if ( !psc::Utils::execPython(pyCmds) ) {
      ERS_PSC_ERROR("Athena 'include' setup failed.");
      return false;
    }       
  }

  if ( jobOptConfig ) {
    // Do the python setup (including user job options)
    std::string pyBasicFile = m_config->getOption("PYTHONSETUPFILE") ;
    if ( !psc::Utils::pyInclude(pyBasicFile) ) {
      ERS_PSC_ERROR("Basic Python configuration failed.");
      return false;
    }  
  }

  if ( !jobOptConfig ) {
    // -----------------------------
    // Configuration from database
    // -----------------------------

    // Run pre-command (you're on your own, no basic python setup)
    std::string cmd = m_config->getOption("PRECOMMAND");
    if ( cmd != "" ) {
      ERS_LOG("Running pre-configure command '" << cmd << "'");
      if ( !psc::Utils::execPython(cmd) ) {
        ERS_PSC_ERROR("Pre-configure command failed.");
        return false;
      }
    }

    // Configure ApplicationMgr
    ERS_DEBUG(1,"Configure ApplicationMgr from database.");
    StatusCode sc = m_pesaAppMgr->configure();
    if( sc.isFailure() ) {
      ERS_PSC_ERROR("Error while configuring the ApplicationMgr");
      return false;
    }

    // Do the basic python setup if postcommand or logLevel was changed
    if ( needPython ) {
      // Normally this is TrigPSC/TrigPSCPythonDbSetup
      std::string pyBasicFile = m_config->getOption("PYTHONSETUPFILE") ;
      if ( pyBasicFile != "" ) {
	if ( !psc::Utils::pyInclude(pyBasicFile) ) {
	  ERS_PSC_ERROR("Basic Python configuration failed.");
	  return false;
	}
      }
    }   
  }

  // Check if running interactively
  // If yes, delay AppMgr::initialize() until connect
  if ( needPython ) {
    PyObject* pModule = PyImport_ImportModule("TrigPSC.PscConfig");
    if ( pModule ) {
      PyObject* pInteractive = PyObject_GetAttrString(pModule, "interactive");
      if ( pInteractive && PyBool_Check(pInteractive) ) {
        m_interactive = (pInteractive==Py_True);
        ERS_DEBUG(1, "TrigPSC.PscConfig.interactive = " << m_interactive);
        Py_DECREF(pInteractive);
      }
      else {
        ERS_DEBUG(1, "Could not read TrigPSC.PscConfig.interactive");
      }
      Py_DECREF(pModule);
    }
    else {
      ERS_DEBUG(1, "Could not import TrigPSC.PscConfig");
    }
  }

  if ( m_interactive ) ERS_LOG("Running in interactive mode");

  ERS_DEBUG(1,"Configured ApplicationMgr in state: " << m_pesaAppMgr->FSMState());

  ServiceHandle<IJobOptionsSvc> p_jobOptionSvc("JobOptionsSvc","psc::Psc");
  SmartIF<IProperty> jos_propif{&(*p_jobOptionSvc)};
  if(m_config->didUserSetLogLevel())
    jos_propif->setProperty("OutputLevel", m_config->getLogLevelAsNumStr());

  // Write Data Flow parameters in JobOptions catalogue

  // map of dataflow parameters to store in the JobOptions Catalogue
  auto props = std::map<std::string,std::string>{
    {"DF_PartitionName", "DF_PARTITION_NAME"},
    {"DF_ApplicationName", "DF_APPLICATIONNAME"},
    {"DF_MachineName", "DF_MACHINE_NAME"},
    {"DF_Ppid", "DF_PPID"},
    {"DF_Pid", "DF_PID"},
    {"DF_HostId", "DF_HOST_ID"},
    {"DF_RandomSeed", "DF_RANDOM_SEED"}};
  if(!setDFProperties(props))
    return false;

  // Write list of configured ROB IDs into the JobOptions Catalogue
  if ( m_config->enabled_robs.size() != 0 ) {

    StatusCode sc = p_jobOptionSvc->addPropertyToCatalogue("DataFlowConfig",
        SimpleProperty< std::vector<uint32_t> >("DF_Enabled_ROB_IDs",m_config->enabled_robs));
    if ( sc.isFailure() ) {
      ERS_PSC_ERROR("psc::Psc::configure: Error could not write list of configured ROB IDs in JobOptions Catalogue: "
                      <<" number of ROB IDs read from OKS = " << m_config->enabled_robs.size()) ;
      return false;
    }

    ERS_DEBUG(1,"psc::Psc::configure: Wrote configuration for enabled ROBs in JobOptions Catalogue: "
              <<" number of ROB IDs read from OKS = " << m_config->enabled_robs.size());
  }

  // Write list of configured Sub Det IDs into the JobOptions Catalogue
  if ( m_config->enabled_SubDets.size() != 0 ) {

    StatusCode sc = p_jobOptionSvc->addPropertyToCatalogue("DataFlowConfig",
	SimpleProperty< std::vector<uint32_t> >("DF_Enabled_SubDet_IDs",m_config->enabled_SubDets));
    if ( sc.isFailure() ) {
      ERS_PSC_ERROR("psc::Psc::configure: Error could not write list of configured sub detector IDs in JobOptions Catalogue: "
                      <<" number of Sub Det IDs read from OKS = " << m_config->enabled_SubDets.size()) ;
      return false;
    }

    ERS_DEBUG(1,"psc::Psc::configure: Wrote configuration for enabled sub detectors in JobOptions Catalogue: "
              <<" number of Sub Det IDs read from OKS = " << m_config->enabled_SubDets.size());
  }

  // Write configuration for HLT muon calibration infrastructure in JobOptions catalogue
  if ( (m_config->getOption("MUONCALBUFFERNAME") != "NONE") && (m_config->getOption("MUONCALBUFFERNAME") != "") ) {
    std::map<std::string, std::string>  muoncal_properties;
    muoncal_properties["MuonCalBufferName"]  = "MUONCALBUFFERNAME" ;
    muoncal_properties["MuonCalBufferSize"]  = "MUONCALBUFFERSIZE" ;

    for(std::map<std::string, std::string>::const_iterator it = muoncal_properties.begin(); it != muoncal_properties.end(); ++it) {
      StatusCode sc = p_jobOptionSvc->addPropertyToCatalogue("MuonHltCalibrationConfig",
          StringProperty(it->first,m_config->getOption(it->second)));
      if ( sc.isFailure() ) {
        ERS_PSC_ERROR("psc::Psc::configure: Error could not write HLT Muon Calibration buffer parameter: "
            << it->first << " = "
            << m_config->getOption(it->second) << " in JobOptions Catalogue.") ;
        return false;
      }
    }

    ERS_DEBUG(1,"psc::Psc::configure: Wrote configuration for HLT Muon Calibration in JobOptions Catalogue: "
        <<" MuonCalBufferName = " << m_config->getOption("MUONCALBUFFERNAME")
        <<" MuonCalBufferSize = " << m_config->getOption("MUONCALBUFFERSIZE") );
  }

  // Write configuration specific to athena (HltEventLoopMgr)
  if(!setAthenaProperties()) return false;

  if ( !jobOptConfig ) {
    // Run post-command
    std::string cmd = m_config->getOption("POSTCOMMAND");
    if ( cmd != "" ) {  
      ERS_LOG("Running post-configure command '" << cmd << "'");
      if ( !psc::Utils::execPython(cmd) ) {
        ERS_PSC_ERROR("Post-configure command failed.");
        return false;
      }
    }
  }    

  if ( !m_interactive ) {
    if ( !doAppMgrInit() ) return false;
  }

  return true;
}


bool psc::Psc::doAppMgrInit()
{
  // Initialize the application manager
  StatusCode sc = m_pesaAppMgr->initialize();
  ERS_DEBUG(1,"Initialize ApplicationMgr : " << m_pesaAppMgr->FSMState() <<
      ", Status = " << sc.getCode());
  if( sc.isFailure() ) {
    ERS_PSC_ERROR("Error while initializing the ApplicationMgr");
    return false;
  }

  // Handle to service locator
  m_svcLoc = SmartIF<ISvcLocator>( m_pesaAppMgr );
  if ( !m_svcLoc.isValid() ) {
    ERS_PSC_ERROR("Error retrieving Service Locator:") ;
    return false;
  }

  SmartIF<IProperty> propMgr ( m_pesaAppMgr );
  if( !propMgr.isValid() ) {
    ERS_PSC_ERROR("Error retrieving IProperty interface of ApplicationMgr");
    return false;
  }

  // Find the basename for the EventLoopMgr  
  std::string value ;
  sc = propMgr->getProperty( "EventLoop", value );
  if( sc.isFailure() ) {
    ERS_PSC_ERROR("Error while retrieving Property \'EventLoop\'.");
  } else {
    m_nameEventLoopMgr.assign(value, value.find_first_of("\"")+1,
        value.find_last_of("\"")-value.find_first_of("\"")-1) ;
  }

  return true;
}


//--------------------------------------------------------------------------------
// Connect transition
//--------------------------------------------------------------------------------

bool psc::Psc::connect (const ptree& /*args*/)
{
  psc::Utils::ScopeTimer timer("Psc connect");

  if ( m_interactive ) {
    if ( !doAppMgrInit() ) return false;
  }
  return true;
}


//--------------------------------------------------------------------------------
// PrepareForRun transition
//--------------------------------------------------------------------------------

bool psc::Psc::prepareForRun (const ptree& args)
{
  psc::Utils::ScopeTimer timer("Psc prepareForRun");

  ERS_DEBUG(1, "psc::Psc::prepareForRun ptree:\n" << to_string(args));

  try
  {
    // save run number
    m_run_number = args.get<uint32_t>("RunParams.run_number");
  }
  catch(const ptree_bad_path & e)
  {
    ERS_PSC_ERROR("Bad ptree path: \"" << e.path<ptree::path_type>().dump()
                  << "\" - " << e.what())

    return false;
  }
  catch(const ptree_bad_data & e)
  {
    ERS_PSC_ERROR("Bad ptree data: \"" << e.data<ptree::data_type>() << "\" - "
                  << e.what())

    return false;
  }

  // Start ApplicationMgr (which starts all services including EventLoopMgr)
  StatusCode sc = m_pesaAppMgr->start();
  if( !sc.isSuccess() )  {
    ERS_PSC_ERROR("Error executing ApplicationMgr::start() command ");
    return false;
  }

  // bind args to prepareForRun
  auto prep = [&args](ITrigEventLoopMgr* mgr){return mgr->prepareForRun(args);};
  if(!callOnEventLoopMgr<ITrigEventLoopMgr>(prep, "prepareForRun").isSuccess())
  {
    ERS_PSC_ERROR("Error preparing the EventLoopMgr");
    return false;
  }

  // Cleanup of dangling database connections from RDBAccessSvc
  ServiceHandle<IRDBAccessSvc> p_rdbAccessSvc("RDBAccessSvc","psc::Psc");
  if(p_rdbAccessSvc->shutdown("*Everything*")) {
    ERS_LOG("Cleaning up RDBAccessSvc connections");
  } else {
    ERS_PSC_ERROR("Cleaning up RDBAccessSvc connections failed");
    return false;
  }

  // sleep some time to allow the closing of DB connections;
  // actual timeout depends on connection parameters, we seem to have 5 seconds
  // timeouts in some places.
  sleep(6);
  // Instantiate connection service
  coral::Context& context = coral::Context::instance();
  // Load CORAL connection service
  coral::IHandle<coral::IConnectionService> connSvcH = context.query<coral::IConnectionService>();
  if (connSvcH.isValid()) {
     ERS_LOG("Cleaning up idle CORAL connections");
     connSvcH->purgeConnectionPool();
  }   

  return true;
}


//--------------------------------------------------------------------------------
// StopRun transition
//--------------------------------------------------------------------------------

bool psc::Psc::stopRun (const ptree& /*args*/)
{
  psc::Utils::ScopeTimer timer("Psc stopRun");

  if(!callOnEventLoopMgr<IService>(&IService::sysStop, "sysStop").isSuccess())
  {
    ERS_PSC_ERROR("Error stopping the EventLoopManager");
    return false;
  }

  // Stop ApplicationMgr and all services.
  // The EventLoopMgr is already stopped by above code but that's fine.
  auto sc = m_pesaAppMgr->stop();
  if ( !sc.isSuccess() ) {
    ERS_PSC_ERROR("Error executing stop(void) for ApplicationMgr");
    return false;
  }

  return true;
}


//--------------------------------------------------------------------------------
// Disconnect transition
//--------------------------------------------------------------------------------

bool psc::Psc::disconnect (const ptree& /*args*/)
{
  psc::Utils::ScopeTimer timer("Psc disconnect");

  return true;
}


//--------------------------------------------------------------------------------
// Unconfigure transition
//--------------------------------------------------------------------------------

bool psc::Psc::unconfigure (const ptree& /*args*/)
{
  psc::Utils::ScopeTimer timer("Psc unconfigure");
  return true;
}


//--------------------------------------------------------------------------------
// Publish statistics
//--------------------------------------------------------------------------------

bool psc::Psc::publishStatistics (const ptree& /*args*/)
{
  psc::Utils::ScopeTimer timer("Psc publishStatistics");
  return true;
}


//--------------------------------------------------------------------------------
// User command can be sent via:
//     partition: rc_sendcommand -p [PART] -n [APP] USER command args
// But they are no longer supported because they don't propagate from mother to child
//--------------------------------------------------------------------------------
bool psc::Psc::hltUserCommand(const ptree& args)
{
  ERS_DEBUG(1, "psc::Psc::hltUserCommand ptree:\n" << to_string(args));

  // Default if no action on command
  return true;
}

void psc::Psc::doEventLoop()
{
  ERS_LOG("psc::Psc::doEventLoop: start of doEventLoop()");
  StatusCode sc;
  try
  {
    // bind maxevt=-1 (meaning all events) to executeRun
    auto exec = [](IEventProcessor * mgr)
                {return mgr->executeRun(-1);};
    sc = callOnEventLoopMgr<IEventProcessor>(exec, "executeRun");
  }
  catch(const ers::Issue& e)
  {
    ERS_PSC_ERROR("Caught an unexpected ers::Issue: '" << e.what() << "'");
    sc = StatusCode::FAILURE;
  }
  catch(const std::exception& e)
  {
    ERS_PSC_ERROR("Caught an unexpected std::exception: '" << e.what() << "'");
    sc = StatusCode::FAILURE;
  }
  catch(...)
  {
    ERS_PSC_ERROR("Caught an unknown exception");
    sc = StatusCode::FAILURE;
  }
  if (sc.isFailure()) {
    ERS_PSC_ERROR("psc::Psc::doEventLoop failed");
  }
  ERS_LOG("psc::Psc::doEventLoop: end of doEventLoop()");
}

bool psc::Psc::prepareWorker (const boost::property_tree::ptree& args)
{
  using namespace std;
  psc::Utils::ScopeTimer timer("Psc prepareWorker");

  ERS_LOG("Individualizing DF properties");

  m_config->prepareWorker(args);

  if (!setDFProperties({{"DF_Pid", "DF_PID"},
                        {"DF_Ppid", "DF_PPID"},
                        {"DF_RandomSeed", "DF_RANDOM_SEED"},
                        {"DF_WorkerId", "DF_WORKER_ID"},
                        {"DF_NumberOfWorkers", "DF_NUMBER_OF_WORKERS"},
                        {"DF_ApplicationName", "DF_APPLICATIONNAME"}})
                       ) return false;

  // bind args to hltUpdateAfterFork
  auto upd = [&args](ITrigEventLoopMgr * mgr)
             {return mgr->hltUpdateAfterFork(args);};
  if(!callOnEventLoopMgr<ITrigEventLoopMgr>(upd, "hltUpdateAfterFork").isSuccess())
  {
    ERS_PSC_ERROR("Error updating EventLoopMgr after fork");
    return false;
  }

  return true;
}

bool psc::Psc::finalizeWorker (const boost::property_tree::ptree& /*args*/)
{
  psc::Utils::ScopeTimer timer("Psc finalizeWorker");

  // Finalize the application manager
  StatusCode sc = m_pesaAppMgr->finalize();
  ERS_DEBUG(1,"Finalize ApplicationMgr: " << m_pesaAppMgr->FSMState()
      << ". Status : " << sc.getCode());

  if( sc.isFailure() ) {
    ERS_PSC_ERROR("Error while finalizing the ApplicationMgr.");
    return false;
  }

  // Terminate the application manager
  sc = m_pesaAppMgr->terminate();
  ERS_DEBUG(1,"Terminate ApplicationMgr: " << m_pesaAppMgr->FSMState() 
      << ". Status : " << sc.getCode());

  if ( sc.isFailure() ) {
    ERS_PSC_ERROR("Error while terminating the ApplicationMgr.");
    return false;
  }

  // Make sure we get a new instance the next time
  Gaudi::setInstance(static_cast<IAppMgrUI*>(0));

  //this object belongs to the real Psc implementation, so don't delete it!
  m_config = 0;

  return true;
}

bool psc::Psc::setDFProperties(std::map<std::string, std::string> name_tr_table)
{
  ServiceHandle<IJobOptionsSvc> p_jobOptionSvc("JobOptionsSvc","psc::Psc");
  for(const auto& prop : name_tr_table)
  {
    const auto& val = m_config->getOption(prop.second);
    auto sc = p_jobOptionSvc->addPropertyToCatalogue(
        "DataFlowConfig", StringProperty(prop.first, val));
    if(sc.isFailure())
    {
      ERS_PSC_ERROR("Error could not write Data Flow parameter: "
          << prop.first << " = " << val << " in JobOptions Catalogue.") ;
      return false;
    }
    else
      ERS_DEBUG(0,"Wrote configuration for Data Flow in JobOptions Catalogue: "
                  << prop.first << " = " << val);
  }

  return true;
}

bool psc::Psc::setAthenaProperties() {
  // Use the IProperty interface of the ApplicationManager to find the EventLoopMgr name
  SmartIF<IProperty> propMgr(m_pesaAppMgr);
  if (!propMgr.isValid()) {
    ERS_PSC_ERROR("Error retrieving IProperty interface of ApplicationMgr");
    return false;
  }
  std::string eventLoopMgrName;
  if (propMgr->getProperty("EventLoop", eventLoopMgrName).isFailure()) {
    ERS_PSC_ERROR("Error while retrieving the property ApplicationMgr.EventLoop");
    return false;
  }

  // Use the JobOptionsSvc to write athena-specific options in JobOptions Catalogue of EventLoopMgr
  ServiceHandle<IJobOptionsSvc> p_jobOptionSvc("JobOptionsSvc","psc::Psc");

  std::string opt = m_config->getOption("HARDTIMEOUT");
  if (!opt.empty()) {
    StatusCode sc = p_jobOptionSvc->addPropertyToCatalogue(
      eventLoopMgrName,
      FloatProperty("HardTimeout",std::stof(opt))
    );
    if (sc.isFailure()) {
      ERS_PSC_ERROR("Error could not write the " << eventLoopMgrName
                    << ".HardTimeout property in JobOptions Catalogue");
      return false;
    }
  }
  else {
    ERS_PSC_ERROR("Failed to get the HARDTIMEOUT property from the configuration tree");
    return false;
  }

  opt = m_config->getOption("SOFTTIMEOUTFRACTION");
  if (!opt.empty()) {
    StatusCode sc = p_jobOptionSvc->addPropertyToCatalogue(
      eventLoopMgrName,
      FloatProperty("SoftTimeoutFraction",std::stof(opt))
    );
    if (sc.isFailure()) {
      ERS_PSC_ERROR("Error could not write the " << eventLoopMgrName
                    << ".SoftTimeoutFraction property in JobOptions Catalogue");
      return false;
    }
  }
  else {
    ERS_PSC_ERROR("Failed to get the SOFTTIMEOUTFRACTION property from the configuration tree");
    return false;
  }

  /* The names "EventDataSvc" and "AvalancheSchedulerSvc" are hard-coded below, because it is not possible
  to retrieve them from HltEventLoopMgr properties before it is initialised. Here we need to set the properties
  before the services are initialised." */

  opt = m_config->getOption("NEVENTSLOTS");
  if (!opt.empty()) {
    StatusCode sc = p_jobOptionSvc->addPropertyToCatalogue(
      "EventDataSvc",
      IntegerProperty("NSlots",std::stoi(opt))
    );
    if (sc.isFailure()) {
      ERS_PSC_ERROR("Error could not write the EventDataSvc.NSlots property in JobOptions Catalogue");
      return false;
    }
  }
  else {
    ERS_PSC_ERROR("Failed to get the NEVENTSLOTS property from the configuration tree");
    return false;
  }

  opt = m_config->getOption("NTHREADS");
  if (!opt.empty()) {
    StatusCode sc = p_jobOptionSvc->addPropertyToCatalogue(
      "AvalancheSchedulerSvc",
      IntegerProperty("ThreadPoolSize",std::stoi(opt))
    );
    if (sc.isFailure()) {
      ERS_PSC_ERROR("Error could not write the AvalancheSchedulerSvc.ThreadPoolSize property in JobOptions Catalogue");
      return false;
    }
  }
  else {
    ERS_PSC_ERROR("Failed to get the NTHREADS property from the configuration tree");
    return false;
  }

  return true;
}


template <typename T>
StatusCode psc::Psc::callOnEventLoopMgr(std::function<StatusCode (T*)> func,
                                        const std::string& name) const
{
  T* processingMgr{nullptr};
  StatusCode sc = m_svcLoc->service(m_nameEventLoopMgr, processingMgr);
  if(!sc.isSuccess()) {
    ERS_PSC_ERROR("Error retrieving EventLoopMgr = '" << m_nameEventLoopMgr << "'" );
    return sc;
  }

  // Call the given function of the EventLoopMgr
  sc = func(processingMgr);   // processingMgr->func()
  if(!sc.isSuccess())
  {
    ERS_PSC_ERROR("Error executing " << name << " for EventLoopMgr = '"
                  << m_nameEventLoopMgr << "'") ;
    return sc;
  }

  return sc;
}
