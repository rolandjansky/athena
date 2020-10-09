/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file Psc.cxx
 * @author Frank Winklmeier, Ricardo Abreu
 *
 * @brief Base class for HLT Pesa Steering Controller.
 *
 */

#include "TrigPSC/Psc.h"
#include "TrigPSC/PscIssues.h"
#include "TrigPSC/Utils.h"
#include "TrigKernel/ITrigEventLoopMgr.h"
#include "CxxUtils/checker_macros.h"

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
#include "Gaudi/Interfaces/IOptionsSvc.h"
#include "GaudiKernel/IAlgManager.h"
#include "GaudiKernel/IAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "Gaudi/Property.h"
#include "GaudiKernel/System.h"

#include <sstream>
#include <algorithm>
#include <vector>
#include <locale>
#include <codecvt>

#include <boost/property_tree/xml_parser.hpp>

#include "CxxUtils/checker_macros.h"

using namespace boost::property_tree;

namespace
{
  //////////////////////////////////////////////////////////////////////////////
  std::string to_string(const ptree& p)
  {
    using T = ptree::key_type;
    std::ostringstream oss;
    xml_parser::write_xml(oss, p,
        xml_parser::xml_writer_make_settings<T>(' ', 2));
    return oss.str();
  }
}

//--------------------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------------------
psc::Psc::~Psc ()
{
  if (m_pesaAppMgr) { 
    m_pesaAppMgr->release() ; 
  }
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
    m_config = std::make_unique<Config>(config);
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
  else if ( m_config->getOption("JOBOPTIONSTYPE") == "DB" ||
            m_config->getOption("JOBOPTIONSTYPE") == "FILE") {
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

      Py_Initialize();
      if (!PyEval_ThreadsInitialized()) PyEval_InitThreads();

      // check
      if ( ! Py_IsInitialized() ) {
        ERS_PSC_ERROR("Error: Python could not been intialized.");
        return false;
      }

      // init the sys.argv...
#if PY_MAJOR_VERSION < 3
      PySys_SetArgv(System::argc(),System::argv());
#else
  auto wargsinit = 
    []() { std::vector<std::wstring> wargs;
           int argc = System::argc();
           char** argv = System::argv();
           wargs.reserve (argc);
           using convert_t = std::codecvt_utf8<wchar_t>;
           std::wstring_convert<convert_t, wchar_t> strconverter;
           for (int i=0; i < argc; ++i) {
             wargs.push_back (strconverter.from_bytes (argv[i]));
           }
           return wargs;
  };
  static const std::vector<std::wstring> wargs = wargsinit();

  auto wargvinit =
    [](const std::vector<std::wstring>& wargs)
      { std::vector<const wchar_t*> wargv;
        int argc = System::argc();
        for (int i=0; i < argc; ++i) {
          wargv.push_back (wargs[i].data());
        }
        return wargv;
  };
  static const std::vector<const wchar_t*> wargv = wargvinit (wargs);

  // Bleh --- python takes non-const argv pointers.
  wchar_t** wargv_nc ATLAS_THREAD_SAFE = const_cast<wchar_t**> (wargv.data());
  PySys_SetArgv(System::argc(), wargv_nc);
#endif
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
#if PY_MAJOR_VERSION < 3
          PyObject* v = PyString_FromString(iter->second.c_str());
#else
          PyObject* v = PyUnicode_FromString(iter->second.c_str());
#endif
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

  ServiceHandle<Gaudi::Interfaces::IOptionsSvc> jobOptionSvc("JobOptionsSvc","psc::Psc");
  SmartIF<IProperty> jos_propif{&(*jobOptionSvc)};
  if(m_config->didUserSetLogLevel())
    jos_propif->setProperty("OutputLevel", m_config->getLogLevelAsNumStr()).ignore();

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
    jobOptionSvc->set("DataFlowConfig.DF_Enabled_ROB_IDs",
                      Gaudi::Utils::toString<std::vector<uint32_t>>(m_config->enabled_robs));
    ERS_DEBUG(1,"psc::Psc::configure: Wrote configuration for enabled ROBs in JobOptions Catalogue: "
              <<" number of ROB IDs read from OKS = " << m_config->enabled_robs.size());
  }

  // Write list of configured Sub Det IDs into the JobOptions Catalogue
  if ( m_config->enabled_SubDets.size() != 0 ) {
    jobOptionSvc->set("DataFlowConfig.DF_Enabled_SubDet_IDs",
                      Gaudi::Utils::toString<std::vector<uint32_t>>(m_config->enabled_SubDets));
    ERS_DEBUG(1,"psc::Psc::configure: Wrote configuration for enabled sub detectors in JobOptions Catalogue: "
              <<" number of Sub Det IDs read from OKS = " << m_config->enabled_SubDets.size());
  }

  // Write the maximum HLT output size into the JobOptions Catalogue
  if (std::string opt = m_config->getOption("MAXEVENTSIZEMB"); !opt.empty()) {
    jobOptionSvc->set("DataFlowConfig.DF_MaxEventSizeMB", opt);
    ERS_DEBUG(1,"psc::Psc::configure: Wrote DF_MaxEventSizeMB=" << opt << " in JobOptions Catalogue");
  }

  // Write configuration for HLT muon calibration infrastructure in JobOptions catalogue
  if ( (m_config->getOption("MUONCALBUFFERNAME") != "NONE") && (m_config->getOption("MUONCALBUFFERNAME") != "") ) {
    jobOptionSvc->set("MuonHltCalibrationConfig.MuonCalBufferName", m_config->getOption("MUONCALBUFFERNAME"));
    jobOptionSvc->set("MuonHltCalibrationConfig.MuonCalBufferSize", m_config->getOption("MUONCALBUFFERSIZE"));

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

  // Initializations needed for start()
  if(!callOnEventLoopMgr<ITrigEventLoopMgr>([&args](ITrigEventLoopMgr* mgr) {return mgr->prepareForStart(args);},
                                            "prepareForStart").isSuccess()) {
    return false;
  }

  // Start ApplicationMgr (which starts all services including EventLoopMgr)
  StatusCode sc = m_pesaAppMgr->start();
  if( !sc.isSuccess() )  {
    ERS_PSC_ERROR("Error executing ApplicationMgr::start() command ");
    return false;
  }

  // bind args to prepareForRun
  auto prep = [&args](ITrigEventLoopMgr* mgr) {
    // FIXME: ITrigEventLookMgr::prepareForRun is declared NOT_THREAD_SAFE.
    // Probably this method shoud also be NOT_THREAD_SAFE, but that's
    // awkward because it implements a tdaq interface from hltinterface.
    StatusCode ret ATLAS_THREAD_SAFE = mgr->prepareForRun (args);

    // This dance is needed to prevent RV optimization.
    // Otherwise, the optimizer loses the ATLAS_THREAD_SAFE attribute
    // on RET before the thread-safety checker gets to see the code.
    if (ret.isSuccess()) {
      return StatusCode (StatusCode::SUCCESS);
    }
    return ret;
  };
  if(!callOnEventLoopMgr<ITrigEventLoopMgr>(prep, "prepareForRun").isSuccess()) {
    return false;
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
    return false;
  }

  // Stop ApplicationMgr and all services.
  // The EventLoopMgr is already stopped by above code but that's fine.
  auto sc = m_pesaAppMgr->stop();
  if ( !sc.isSuccess() ) {
    ERS_PSC_ERROR("Error executing stop(void) for ApplicationMgr");
    return false;
  }

  // Workers: store histograms at the end of stop as the children may not
  // go through finalize with SkipFinalizeWorker=1.
  if (m_workerID != 0) {
    SmartIF<IService> histsvc = m_svcLoc->service("THistSvc", /*createIf=*/ false);
    if (histsvc.isValid()) {
      ERS_LOG("Finalize THistSvc to save histograms");
      if (histsvc->finalize().isFailure()) {
        ERS_PSC_ERROR("Error executing finalize for THistSvc");
      }
    }
  }


  return true;
}

bool psc::Psc::doAppMgrFinalize()
{
  if (m_pesaAppMgr==nullptr) return true; // already finalized

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
  Gaudi::setInstance(static_cast<IAppMgrUI*>(nullptr));
  m_pesaAppMgr = nullptr;

  return true;
}

//--------------------------------------------------------------------------------
// Disconnect transition
//--------------------------------------------------------------------------------

bool psc::Psc::disconnect (const ptree& /*args*/)
{
  psc::Utils::ScopeTimer timer("Psc disconnect");
  return doAppMgrFinalize();
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

bool psc::Psc::doEventLoop()
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
    return false;
  }
  ERS_LOG("psc::Psc::doEventLoop: end of doEventLoop()");
  return true;
}

bool psc::Psc::prepareWorker (const boost::property_tree::ptree& args)
{
  psc::Utils::ScopeTimer timer("Psc prepareWorker");

  if ( Py_IsInitialized() ) {
    ERS_DEBUG(1, "Post-fork initialization of Python interpreter");
#if PY_VERSION_HEX >= 0x03070000
    PyOS_AfterFork_Child();
#else
    PyOS_AfterFork();
#endif

    /* Release the Python GIL (which we inherited from the mother)
       to avoid dead-locking on the first call to Python. Only relevant
       if Python is initialized and Python-based algorithms are used. */
    if (PyEval_ThreadsInitialized()) {
      ERS_DEBUG(1, "Releasing Python GIL");
      PyEval_SaveThread();
    }
  }

  m_workerID = args.get<int>("workerId");

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
    return false;
  }

  return true;
}

bool psc::Psc::finalizeWorker (const boost::property_tree::ptree& /*args*/)
{
  psc::Utils::ScopeTimer timer("Psc finalizeWorker");
  return doAppMgrFinalize();
}

bool psc::Psc::setDFProperties(std::map<std::string, std::string> name_tr_table)
{
  ServiceHandle<Gaudi::Interfaces::IOptionsSvc> jobOptionSvc("JobOptionsSvc","psc::Psc");
  for(const auto& prop : name_tr_table)
  {
    const auto& val = m_config->getOption(prop.second);
    jobOptionSvc->set("DataFlowConfig."+prop.first, val);
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
  ServiceHandle<Gaudi::Interfaces::IOptionsSvc> jobOptionSvc("JobOptionsSvc","psc::Psc");

  std::string opt = m_config->getOption("HARDTIMEOUT");
  if (!opt.empty()) {
    jobOptionSvc->set(eventLoopMgrName+".HardTimeout", opt);
  }
  else {
    ERS_PSC_ERROR("Failed to get the HARDTIMEOUT property from the configuration tree");
    return false;
  }

  opt = m_config->getOption("SOFTTIMEOUTFRACTION");
  if (!opt.empty()) {
    jobOptionSvc->set(eventLoopMgrName+".SoftTimeoutFraction", opt);
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
    jobOptionSvc->set("EventDataSvc.NSlots", opt);
  }
  else {
    ERS_PSC_ERROR("Failed to get the NEVENTSLOTS property from the configuration tree");
    return false;
  }

  opt = m_config->getOption("NTHREADS");
  if (!opt.empty()) {
    jobOptionSvc->set("AvalancheSchedulerSvc.ThreadPoolSize", opt);
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
