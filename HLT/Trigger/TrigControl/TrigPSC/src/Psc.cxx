/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IAppMgrUI.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/IAlgManager.h"
#include "GaudiKernel/IAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/System.h"
//#include "IOVDbSvc/IIOVCondDbSvc.h"
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
      m_failNextUsrCmd(false),
      m_sleepNextUsrCmd(0),
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
  if ((m_config->getOption("DLLNAME").size() != 0) && (m_config->getOption("FACTORYNAME").size() != 0)) {
    const std::string dllname = m_config->getOption("DLLNAME");
    const std::string factname = m_config->getOption("FACTORYNAME");

    if (m_config->getOption("FACTORYNAME")=="ApplicationMgr") {
      /*
       * We have to pre-load the DLL in case the factory name is identical
       * to the original application manager. Otherwise the ApplicationMgr from GaudiSvc
       * will be loaded since Gaudi::createApplicationMgr seems to trigger loading
       * of libGaudiSvc.
       */
      void* libHandle = 0;
      StatusCode sc = System::loadDynamicLib(dllname, &libHandle);
      if (sc.isSuccess()) {
        ERS_DEBUG(1,"Successfully pre-loaded " << dllname << "library");
      }
      else {
        ERS_DEBUG(1,"Failed pre-loading " << dllname << "library");
      }
    }

    m_pesaAppMgr = Gaudi::createApplicationMgr(dllname, factname);
  } else {
    m_pesaAppMgr = Gaudi::createApplicationMgr();
  }
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

  // Set EventSelector (should be NONE)
  sc = propMgr->setProperty( "EvtSel", m_config->getOption("EVENTSELECTOR"));
  if (sc.isFailure()) {
    ERS_PSC_ERROR("Can not set Property = EvtSel");
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
    pyCmds.push_back(std::string("include.setShowIncludes(") +
        m_config->getOption("SHOWINCLUDE") + std::string(")"));
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
      p_jobOptionSvc->release();
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
      p_jobOptionSvc->release();
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
        p_jobOptionSvc->release();
        return false;
      }
    }

    ERS_DEBUG(1,"psc::Psc::configure: Wrote configuration for HLT Muon Calibration in JobOptions Catalogue: "
        <<" MuonCalBufferName = " << m_config->getOption("MUONCALBUFFERNAME")
        <<" MuonCalBufferSize = " << m_config->getOption("MUONCALBUFFERSIZE") );
  }

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
    p_rdbAccessSvc->release();
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

  /*
  StatusCode sc ;
  // Finalize the application manager
  sc = m_pesaAppMgr->finalize();
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
  */

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
// Time out is reached
//--------------------------------------------------------------------------------
void psc::Psc::timeOutReached (const ptree& args)
{
  ERS_DEBUG(1, "Time out reached for HLT processing");

  // bind args to timeOutReached
  auto tor = [&args](ITrigEventLoopMgr * mgr)
             {return mgr->timeOutReached(args);};
  callOnEventLoopMgr<ITrigEventLoopMgr>(tor, "timeOutReached").ignore();
}



//--------------------------------------------------------------------------------
// User command. Can be sent via:
//     partition: rc_sendcommand -p [PART] -n [APP] USER command args
//     athenaXT:  hlt_user_command_client command args
//--------------------------------------------------------------------------------

namespace {
// Helper to format the user command and parameters
std::string fmtUserCommand(const std::string& usrCmd, const std::vector<std::string>& usrParam)
{
  std::ostringstream os;
  os << "['" << usrCmd << "'";
  std::vector<std::string>::const_iterator iter;
  for (iter=usrParam.begin(); iter!=usrParam.end(); ++iter) {
    os << " '" << *iter << "'";
  }
  os << "]";
  return os.str();
}
}


bool psc::Psc::hltUserCommand(const ptree& args)
{
  // TODO: change this once the parameters are decided

  ERS_DEBUG(1, "psc::Psc::hltUserCommand ptree:\n" << to_string(args));

  const std::string& usrCmd = args.get_child("COMMAND").data();
  std::vector<std::string> usrParam;
  for(auto p : args.get_child("PARAMS"))
    usrParam.push_back(p.second.data());

  // Print all received user commands to log file
  if (ers::debug_level()>=1) {
    ERS_DEBUG(1, "Received user command " << fmtUserCommand(usrCmd,usrParam));
  }

  /*
   * In tdaq-02-00-01 the first usrParam will contain all parameters separated
   * by spaces. This might change in future releases.
   */
  std::vector<std::string> params;
  if ( !usrParam.empty() ) {
    std::stringstream ss(usrParam[0]);
    std::string buf;
    // Split the first parameter
    while ( ss >> buf ) params.push_back(buf);

    // Append any remaining parameters
    if ( usrParam.size() > 1 ) params.insert(params.end(), usrParam.begin()+1, usrParam.end());
  }


  // Check if user command overrides were requested
  if ( m_sleepNextUsrCmd > 0 ) {
    ERS_PSC_WARNING("Sleeping for " << m_sleepNextUsrCmd << " seconds before executing " << usrCmd);
    sleep(m_sleepNextUsrCmd);
    m_sleepNextUsrCmd = 0;
  }

  if ( m_failNextUsrCmd ) {
    ERS_PSC_WARNING("Will not execute " << usrCmd << ". Return failure instead.");
    m_failNextUsrCmd = false;
    return false;
  }

  // ------------------------------------------------
  // HLT_SetProperty Client Property VALUE
  // ------------------------------------------------
  if ( usrCmd == "HLT_SetProperty" ) {

    if ( params.size()!=3 ) {
      ERS_PSC_ERROR("User command " << fmtUserCommand(usrCmd,usrParam) << " has " << params.size() <<
          " arguments, expected 3");
      return true;
    }
    ERS_LOG("Processing command " << fmtUserCommand(usrCmd,usrParam));

    // Try to find algorithm with this name
    SmartIF<IProperty> prop;
    ServiceHandle<IAlgManager> algMgr("ApplicationMgr", "psc::Psc");
    prop = algMgr->algorithm<IProperty>(params[0], false);
    algMgr->release();

    // If not found, try services (Tools are not supported at this point)
    if (!prop.isValid()) {
      ServiceHandle<ISvcLocator> svcLoc("ApplicationMgr", "psc::Psc");
      prop = svcLoc->service<IProperty>(params[0], false);
      svcLoc->release();
    }

    if (!prop.isValid()) {
      ERS_LOG("Cannot retrieve " << params[0]);
      return true;
    }

    if (prop->setProperty(params[1], params[2]).isSuccess()) {
      ERS_PSC_WARNING("HLT property changed: " << params[0] << "." << params[1]
                                               << " = " << params[2]);
    }
    else {
      ERS_LOG("ERROR setting property " << params[0] << "." << params[1]);
    }

    return true;
  }
  // ---------------------------
  // HLT_FailNextUserCommand
  // ---------------------------
  else if ( usrCmd == "HLT_FailNextUsrCmd" ) {
    m_failNextUsrCmd = true;
    ERS_PSC_WARNING("Will return failure instead of executing next user command");
  }
  // ------------------------------
  // HLT_TimeoutNextUserCommand SEC
  // ------------------------------
  else if ( usrCmd == "HLT_SleepNextUsrCmd" ) {
    if ( params.empty() ) {
      ERS_PSC_ERROR("User command '" << usrCmd <<"' has no parameters.");
      return false;
    }

    // Convert time to uint
    try {
      m_sleepNextUsrCmd = boost::lexical_cast<uint>(params[0]);
    }
    catch (boost::bad_lexical_cast &) {
      ERS_PSC_ERROR("Cannot convert parameter '" << params[0]
                                                           << "' of user command '" << usrCmd << "' to unsigned integer.");
      m_sleepNextUsrCmd = 0;
      return false;
    }

    ERS_PSC_WARNING("Will sleep for " << m_sleepNextUsrCmd <<
        " seconds before executing next user command");
  }
  else {
    ERS_DEBUG(1, "Ignoring unknown user command '" << usrCmd << "'");
  }

  // Default if no action on command
  return true;
}

bool psc::Psc::process(const vector<ROBFragment<const uint32_t*> >& l1r,
                       hltinterface::HLTResult& hltr,
                       const hltinterface::EventId& evId)
{
  // protect against empty L1 result vector
  if ( l1r.empty() ) {
    ERS_PSC_ERROR("psc::Psc::process: Received no L1 Result ROBs.");
    return false;
  }

  uint32_t lvl1_id = l1r[0].rod_lvl1_id();
  ERS_DEBUG(2,"psc::Psc::process: Start process() of event " << lvl1_id);

  //
  //--- Process Event/RoIs with the event loop manager and fill HLT Decision
  //

  StatusCode sc;
  try
  {
    // bind l1r, hltr, and evId to processRoIs
    auto proc = [&](ITrigEventLoopMgr * mgr)
                {return mgr->processRoIs(l1r, hltr, evId);};
    sc = callOnEventLoopMgr<ITrigEventLoopMgr>(proc, "processRoIs");
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

  ERS_DEBUG(2,"psc::Psc::process: Event " << lvl1_id  << " has /n" <<
      "       # of stream tags        = " << hltr.stream_tag.size()         << " /n"  <<
      "       # of HLT info words      = " << hltr.trigger_info.size()          << " /n"  <<
      "       # of PSC error words = " << hltr.psc_errors.size()         << " /n"  <<
      "       pointer to HLT result    = 0x" << std::hex << hltr.fragment_pointer << std::dec << " /n"
  );

  if( sc.isSuccess() || sc.isRecoverable() ) {
    return true;
  } else {
    ERS_PSC_ERROR("psc::Psc::process: Error in processing RoIs for Level 1 ID ="
                  << lvl1_id << " in the EventLoopMgr '"
                  << m_nameEventLoopMgr << "'.");
    return false;
  }
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

  // find all available EventLoopMgr and call hltUpdateAfterFork 
  SmartIF<ISvcLocator> svcLoc( m_pesaAppMgr );
  if ( !svcLoc.isValid() ) {
    ERS_PSC_ERROR("Error retrieving Service Locator:") ;
    return false;
  }

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
      p_jobOptionSvc->release();
      return false;
    }
    else
      ERS_DEBUG(0,"Wrote configuration for Data Flow in JobOptions Catalogue: "
                  << prop.first << " = " << val);
  }

  return true;
}

template <typename T>
StatusCode psc::Psc::callOnEventLoopMgr(std::function<StatusCode (T*)> func,
                                        const std::string& name) const
{
  static T* processingMgr = 0;
  StatusCode sc;
  if(!processingMgr) // if we already got it, no need to find it again
  {
    SmartIF<ISvcLocator> svcLoc(m_pesaAppMgr);
    if(!svcLoc.isValid())
    {
      ERS_PSC_ERROR("Error retrieving Service Locator") ;
      return StatusCode::FAILURE;
    }

    // Retrieve the EventLoopMgr
    sc = svcLoc->service(m_nameEventLoopMgr, processingMgr);
    if(!sc.isSuccess())
    {
      ERS_PSC_ERROR("Error retrieving EventLoopMgr = '"
          << m_nameEventLoopMgr << "'" );
      return sc;
    }
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
