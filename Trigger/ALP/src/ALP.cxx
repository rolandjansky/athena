// Dear emacs, this is -*- c++ -*-
/*

Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

Author: Sami Kama 2017
*/

#include "ALP/ALP.h"
// needed for system calls
#include <errno.h>
#include <sys/prctl.h>
#include <signal.h>
#include <sys/wait.h>
#include <cstdio>
#include <cstdlib>
#include <dlfcn.h>
#include <unistd.h>

// needed for io, threads etc
#include <string>
#include <cstring>
#include <iostream>
#include <boost/thread.hpp>
#include "boost/date_time/posix_time/posix_time.hpp"
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>
#include <ers/ers.h>
#include "ALP/Issues.h"
//needed for IPC initialization
//#include "ipc/core.h"
#include "hltinterface/IPCControlInterface.h"
//InformationService
#include "hltinterface/IInfoRegister.h"
// Data Source
#include "hltinterface/DataSource.h"
//dynlibs Module
//#include "dynlibs/Module.h"
#include "ALP/PluginLoader.h"
#include "ALP/DataSourceExceptions.h"
//ISObjects
#include "hltinterface/ContainerFactory.h"
#include "hltinterface/GenericHLTContainer.h"
//needed temporarily for IS publication.
#include "TH1F.h"
#include "dirent.h"
//Random number generator
#include <random>
#include "ALP/ALP_utils.h"

extern char **environ;

namespace HLTPU {
  //fix this!
  class ScopedISHelper{
  public:
    ScopedISHelper(long LB,long ev){
      m_tree.put("eventNumber",ev);
      m_tree.put("LBNumber",LB);
      try{
	hltinterface::IInfoRegister::instance()->beginEvent(m_tree);
      }catch(const std::exception &ex){
	ERS_LOG("Caught exception while calling beginEvent for event "
		<<ev<<" at LB "<<LB
		<<". Exception was "<<ex.what());
      }
    }
    ~ScopedISHelper(){
      try{
	hltinterface::IInfoRegister::instance()->endEvent(m_tree);
      }catch(const std::exception &ex){
	ERS_LOG("Caught exception while calling beginEvent for event "<<m_tree.get<long>("eventNumber")
		<<" at LB "<<m_tree.get<long>("LBNumber")
		<<". Exception was "<<ex.what());
      }
    }
  private:
    boost::property_tree::ptree m_tree;
  };
}
#define GCIncrIntField(obj,field,val) \
  obj->setIntField(field,obj->getIntField(field)+val)
#define GCDecrIntField(obj,field,val) \
  obj->setIntField(field,obj->getIntField(field)-val)

ALP::ALP(){
  m_myPid=getpid();
  m_myPgid=getpgid(0);
  std::cout<<ALPUtils::getTimeTag()<<"Constructing ALP Interface with pid "<<m_myPid<<" and pgid "<<m_myPgid<<std::endl;
  //  ERS_LOG("Constructing ALP Interface with pid "<<m_myPid<<" and pgid "<<m_myPgid);
  std::cout<<ALPUtils::getTimeTag()<<"Library built on "<<__DATE__<<" "<<__TIME__<<std::endl;
  //ERS_LOG("Library built on "<<__DATE__<<" "<<__TIME__<<std::endl);
  m_nannyWork=false;
  m_nannyThread=0;
  m_motherPublisher=0;
  m_numChildren=1;
  m_HLTSteering=0;
  m_dataSource=0;
  m_myName=getenv("TDAQ_APPLICATION_NAME");
  m_processEvents=true;
  m_childLogPath="/tmp/";
  // to be removed
  char * tmp = getenv("TDAQ_IS_SERVER");
  if(tmp){
    m_ISSName=std::string(tmp);
  }else{
    m_ISSName="DF";
  }
  m_publishInterval=30;
  //  m_motherInfo=std::make_shared<ALP::ALPMotherInfo>();
  auto inst=hltinterface::ContainerFactory::getInstance();
  m_motherInfo=inst->constructContainer("PU_MotherInfo",
		"ALPMotherInfo",
		hltinterface::GenericHLTContainer::UntilEOR,
		hltinterface::GenericHLTContainer::WRITE);
  //size_t MINumKills,MINumForks,MIUnexpectedChildExits,MINumRequested,MINumActive,MINumExited;
  m_MINumKills=inst->addInt(m_motherInfo,"NumKills");
  m_MINumForks=inst->addInt(m_motherInfo,"NumForks");
  m_MIUnexpectedChildExits=inst->addInt(m_motherInfo,"UnexpectedChildExits");
  m_MINumRequested=inst->addInt(m_motherInfo,"NumRequested");
  m_MINumActive=inst->addInt(m_motherInfo,"NumActive");
  m_MINumExited=inst->addInt(m_motherInfo,"NumExited");
  m_childInfo=inst->constructContainer("PU_ChildInfo",
		"ALPInfo",
		hltinterface::GenericHLTContainer::UntilEOR,
		hltinterface::GenericHLTContainer::WRITE);
  
  m_CINumEvents=inst->addInt(m_childInfo,"NumEvents");
  m_CIAcceptedEvents=inst->addInt(m_childInfo,"AcceptedEvents");
  m_CIRejectedEvents=inst->addInt(m_childInfo,"RejectedEvents");
  m_CIL1ResultFetchTimeouts=inst->addInt(m_childInfo,"L1ResultFetchTimeouts");
  m_CISoftTimeouts=inst->addInt(m_childInfo,"Softtimeouts");

  m_CILongestWaitForL1Result=inst->addInt(m_childInfo,"LongestWaitForL1Result");
  m_CILongestProcessingTime=inst->addInt(m_childInfo,"LongestProcessingTime");
  m_CIAverageProcessingTime=inst->addFloat(m_childInfo,"AverageProcessingTime");
  m_CIAverageAcceptTime=inst->addFloat(m_childInfo,"AverageAcceptTime");
 
  m_CIAverageRejectTime=inst->addFloat(m_childInfo,"AverageRejectTime");
  m_CIAverageL1ResultTime=inst->addFloat(m_childInfo,"AverageL1ResultTime");
  m_CITimePercentInProcessing=inst->addFloat(m_childInfo,"TimePercentInProcessing");
  m_CITimePercentInAccept=inst->addFloat(m_childInfo,"TimePercentInAccept");
 
  m_CITimePercentInReject=inst->addFloat(m_childInfo,"TimePercentInReject");
  m_CITimePercentInWait=inst->addFloat(m_childInfo,"TimePercentInWait");
  m_CITimePercentInSend=inst->addFloat(m_childInfo,"TimePercentInSend");;
    
  m_configTree=new boost::property_tree::ptree();
  m_prepareForRunTree=new boost::property_tree::ptree();
  m_forkDelay=-1;
  m_preforkSleep=-1;
  m_terminationStarted=false;
  m_dumpFD=false;
  m_dumpThreads=false;
  m_lbNum=0;
  m_evtNum=0;
  m_CTPROBId=0x770001;
  m_timerWork=true;
  m_l1ResultTimeout=100000;//in ms
  m_softTimeout=60000;//in ms;
  m_saveConfigOpts=false;
  m_threadsExist=false;
  m_softTOTrigger=false;
  m_hardTOTrigger=false;
  m_FinalizeTimeout=120;
  m_publisherThread=0;
  m_infoService=0;
  m_hardTimeout=60;
  m_interEventSleep_ms=-1;
  m_interEventSpread_ms=-1;
  m_keepNumForks=false;
  m_termStagger=0;
  m_skipFinalize=false;
  m_skipFinalizeWorker=false;
  m_exitImmediately=false;
  m_eventsInInterval=0;
  m_acceptedInInterval=0;
  m_rejectedInInterval=0;
  m_ipcc=0;
}


ALP::~ALP(){
  std::cout<<ALPUtils::getTimeTag()<<"Destructing ALP Interface with pid "<<m_myPid<<std::endl;
  //  delete m_dataSource;
  //delete m_infoService;
  //delete m_HLTSteering;
  delete m_configTree;
  delete m_prepareForRunTree;
  stopNanny();
  stopMotherPublisher();

}

bool ALP::configure(const boost::property_tree::ptree& args ) {
  //ERS_LOG("--ALP_ConfigureStart. using the following configuration tree: ");
  std::cout<<ALPUtils::getTimeTag()<<"--ALP_ConfigureStart. using the following configuration tree: "<<std::endl;
  printPtree(args,"");
  std::cout.flush();
  *m_configTree=args;
  m_numChildren=args.get("Configuration.ALPApplication.numForks",4);
  m_FinalizeTimeout=args.get("Configuration.ALPApplication.finalizeTimeout",120);
  double softTOfraction=args.get<double>("Configuration.ALPApplication.softTimeoutFraction",0.80);
  m_hardTimeout=args.get("Configuration.ALPApplication.HardTimeout",60000);
  m_softTimeout=m_hardTimeout*softTOfraction;
  m_childLogPath=std::string(args.get_child("Configuration.Partition.LogRoot").data());
  std::string ipclib("CorbaIPCControl");
  std::vector<std::string> libs,mods;
  try{
    ipclib=std::string(args.get_child("Configuration.ALPApplication.IPCController").data());
  }catch(boost::property_tree::ptree_bad_path &ex){
  }catch(...){
  }

  std::cout<<ALPUtils::getTimeTag()<<"Trying to open ipc control library "<<ipclib<<std::endl;

  try{
    ALPNS::PluginLoader::addPlugin("IPCController",libs);
  }catch(std::exception &ex){
    std::cerr<<ALPUtils::getTimeTag()<<"Error loading IPCController libraries "<<ex.what()<<std::endl;
    std::string errMsg=std::string("IPCController library load failed with \"")+ex.what()+"\"";
    //ers::fatal(ALPIssues::DLLIssue(ERS_HERE,errMsg.c_str()));
    throw ALPIssues::DLLIssue(ERS_HERE,errMsg.c_str());
    return false;
  }
  libs.resize(0);
  typedef hltinterface::IPCControlInterface* (*ipccreator)(void);
  ipccreator ipccfact=ALPNS::PluginLoader::get("IPCController")->function<ipccreator>("create_ipccontroller");
  m_ipcc=ipccfact();
  m_ipcc->initializeIPC(args);
  try{
    //std::pair<boost::property_tree::ptree::const_assoc_iterator,boost::property_tree::ptree::const_assoc_iterator> extras=
    auto extras=args.get_child("Configuration.ALPApplication.extraParams").equal_range("parameter");
    for(auto it=extras.first;it!=extras.second;++it){
      std::vector<std::string> tokens;
      std::string data=std::string(it->second.data());
      boost::algorithm::split(tokens,data,boost::is_any_of("="));
      if(tokens.size()>1){
	if(tokens.at(0)==std::string("forkDelay")){
	  int forkDelay=::strtol(tokens.at(1).c_str(),0,10);
	  if(errno==ERANGE||errno==EINVAL){
	    std::cerr<<ALPUtils::getTimeTag()<<"Fork delay is set incorrectly! offending line is \""<<data<<"\""<<std::endl;
	    errno=0;
	  }else{
	    m_forkDelay=forkDelay;
	  }
	}else if(tokens.at(0)==std::string("preForkSleep")){
	  int preforkSleep=::strtol(tokens.at(1).c_str(),0,10);
	  if(errno==ERANGE||errno==EINVAL){
	    std::cerr<<ALPUtils::getTimeTag()<<"Fork sleep is set incorrectly! offending line is \""<<data<<"\""<<std::endl;
	    errno=0;
	  }else{
	    m_preforkSleep=preforkSleep;
	  }
	}else if(tokens.at(0)==std::string("publishInterval")){
	  int publishInterval=::strtol(tokens.at(1).c_str(),0,10);
	  if(errno==ERANGE||errno==EINVAL){
	    std::cerr<<ALPUtils::getTimeTag()<<"publication interval is set incorrectly! offending line is \""<<data<<"\""<<std::endl;
	    errno=0;
	  }else{
	    m_publishInterval=publishInterval;
	    if(m_publishInterval<10)m_publishInterval=10;//5 second publish interval minimum
	  }
	}else if(tokens.at(0)==std::string("keepForks")){
	  int keepForks=::strtol(tokens.at(1).c_str(),0,10);
	  if(errno==ERANGE||errno==EINVAL){
	    std::cerr<<ALPUtils::getTimeTag()<<"keepForks parameter set incorrectly (0 or !=0) \""<<data<<"\""<<std::endl;
	    errno=0;
	  }else{
	    m_keepNumForks=(keepForks!=0);
	  }
	}else if(tokens.at(0)==std::string("dumpFDs")){
	  int dump=::strtol(tokens.at(1).c_str(),0,10);
	  if(errno==ERANGE||errno==EINVAL){
	    std::cerr<<ALPUtils::getTimeTag()<<"DumpFD is set incorrectly! offending line is \""<<data<<"\""<<std::endl;
	    errno=0;
	  }else{
	    m_dumpFD=(dump!=0);
	  }
	}else if(tokens.at(0)==std::string("dumpThreads")){
	  int dump=::strtol(tokens.at(1).c_str(),0,10);
	  if(errno==ERANGE||errno==EINVAL){
	    std::cerr<<ALPUtils::getTimeTag()<<"dumpThreads is set incorrectly! offending line is \""<<data<<"\""<<std::endl;
	    errno=0;
	  }else{
	    m_dumpThreads=(dump!=0);
	  }
	}else if(tokens.at(0)==std::string("L1ResultTimeout")){
	  int dump=::strtol(tokens.at(1).c_str(),0,10);
	  if(errno==ERANGE||errno==EINVAL){
	    std::cerr<<ALPUtils::getTimeTag()<<"L1Result timeout set incorrectly! offending line is \""<<data<<"\""<<std::endl;
	    errno=0;
	  }else{
	    m_l1ResultTimeout=((dump<1000)?1000:dump);
	  }
	}else if(tokens.at(0)==std::string("SaveInputParams")){
	  int dump=::strtol(tokens.at(1).c_str(),0,10);
	  if(errno==ERANGE||errno==EINVAL){
	    std::cerr<<ALPUtils::getTimeTag()<<"SaveInputParams set incorrectly! offending line is \""<<data<<"\""<<std::endl;
	    errno=0;
	  }else{
	    m_saveConfigOpts=(dump!=0);
	  }
	}else if(tokens.at(0)==std::string("SkipFinalizeWorker")){
	  int dump=::strtol(tokens.at(1).c_str(),0,10);
	  if(errno==ERANGE||errno==EINVAL){
	    std::cerr<<ALPUtils::getTimeTag()<<"SkipFinalizeWorker set incorrectly! offending line is \""<<data<<"\""<<std::endl;
	    errno=0;
	  }else{
	    m_skipFinalizeWorker=(dump!=0);
	  }
	}else if(tokens.at(0)==std::string("SkipFinalize")){
	  int dump=::strtol(tokens.at(1).c_str(),0,10);
	  if(errno==ERANGE||errno==EINVAL){
	    std::cerr<<ALPUtils::getTimeTag()<<"SkipFinalize set incorrectly! offending line is \""<<data<<"\""<<std::endl;
	    errno=0;
	  }else{
	    m_skipFinalize=(dump!=0);
	  }
	}else if(tokens.at(0)==std::string("ExitWithoutCleanup")){
	  int dump=::strtol(tokens.at(1).c_str(),0,10);
	  if(errno==ERANGE||errno==EINVAL){
	    std::cerr<<ALPUtils::getTimeTag()<<"ExitWithoutCleanup set incorrectly! offending line is \""<<data<<"\""<<std::endl;
	    errno=0;
	  }else{
	    m_exitImmediately=(dump!=0);
	  }
	}else if(tokens.at(0)==std::string("CTPROBId")){ //needs to be in HEXADECIMAL
	  int dump=::strtol(tokens.at(1).c_str(),0,16);
	  if(errno==ERANGE||errno==EINVAL){
	    std::cerr<<ALPUtils::getTimeTag()<<"CTPROBId is set incorrectly! offending line is \""<<data<<"\""<<std::endl;
	    errno=0;
	  }else{
	    m_CTPROBId=dump;
	  }
	}else if(tokens.at(0)==std::string("InterEventSleep_ms")){
	  int interSleep=::strtol(tokens.at(1).c_str(),0,10);
	  if(errno==ERANGE||errno==EINVAL){
	    std::cerr<<ALPUtils::getTimeTag()<<"Inter event sleep is set incorrectly! offending line is \""<<data<<"\""<<std::endl;
	    errno=0;
	  }else{
	    m_interEventSleep_ms=interSleep;
	  }
	}else if(tokens.at(0)==std::string("InterEventSpread_ms")){
	  int interSpread=::strtol(tokens.at(1).c_str(),0,10);
	  if(errno==ERANGE||errno==EINVAL){
	    std::cerr<<ALPUtils::getTimeTag()<<"Inter event sleep is set incorrectly! offending line is \""<<data<<"\""<<std::endl;
	    errno=0;
	  }else{
	    m_interEventSpread_ms=interSpread;
	  }
	}else if(tokens.at(0)==std::string("MaxTermStagger_s")){
	  int interSpread=::strtol(tokens.at(1).c_str(),0,10);
	  if(errno==ERANGE||errno==EINVAL){
	    std::cerr<<ALPUtils::getTimeTag()<<"Inter event sleep is set incorrectly! offending line is \""<<data<<"\""<<std::endl;
	    errno=0;
	  }else{
	    m_termStagger=interSpread;
	  }
	}
      }
    }
  }catch(boost::property_tree::ptree_bad_path &ex){
    std::cerr<<ALPUtils::getTimeTag()<<"No ExtraParams "<<std::endl;
    ERS_DEBUG(1,"there is no extraParams, skipping");
  }catch (...){
    std::cerr<<ALPUtils::getTimeTag()<<"I caught something that I don't know (i.e. catch(...) caught the exception) "<<std::endl;
  }

  ERS_DEBUG(1,"Configuring. my pid is "<<m_myPid);
  //load data source

  ERS_DEBUG(1,"Loading DataSource library");
  libs.emplace_back(args.get_child("Configuration.ALPApplication.DataSourceLibrary").data());
  //check whether running DFDataSource or not
  {
    dlerror();
    void * handle=dlopen(libs.back().c_str(),RTLD_LAZY|RTLD_LOCAL);
    if(!handle){
      char b[4000];
      snprintf(b,4000,"lib%s.so",libs.back().c_str());
      ERS_DEBUG(1,"Can't find library '"<<libs.back()<<"' trying '"<<b<<"'"); 
      handle=dlopen(b,RTLD_LAZY|RTLD_LOCAL);
    }

    if(handle){
      dlerror();
      void* sym=dlsym(handle,"create_hltmp_datasource");
      char *errv=dlerror();
      if(!sym){// then library don't have necessary function. probably dfinterfacedcm.so
	ERS_LOG("Can't find symbol create_hltmp_datasource in '"<<libs.back()<<"' adding DFDataSource ");
	libs.insert(libs.begin(),"DFDataSource");
	if(errv){
	  ERS_DEBUG(1,"dlsym error was "<<errv);
	}
      }else{
	ERS_LOG("Find symbol create_hltmp_datasource in '"<<libs.back()<<"'. Bypassing DFDataSource");
      }
      if(dlclose(handle)){
	ERS_DEBUG(1,"Closing library handle after plugin check failed!");
      }
    }else{// couldn't open handle revert back to old behavior
      ERS_LOG("Can't dlopen library  '"<<libs.back()<<"' or lib"<<libs.back()<<".so adding DFDataSource");
      libs.insert(libs.begin(),"DFDataSource");
    }
  }

  try{
    ALPNS::PluginLoader::addPlugin("DataSource",libs);
  }catch(std::exception &ex){
    std::cerr<<ALPUtils::getTimeTag()<<"Error loading datasource libraries "<<ex.what()<<std::endl;
    std::string errMsg=std::string("DataSource library load failed with \"")+ex.what()+"\"";
    ers::fatal(ALPIssues::DLLIssue(ERS_HERE,errMsg.c_str()));
    throw ALPIssues::DLLIssue(ERS_HERE,errMsg.c_str());
    return false;
  }
  //load information service
  ERS_DEBUG(1,"Loading InformationService library");
  libs.clear();
  libs.push_back(args.get_child("Configuration.ALPApplication.InfoServiceLibrary").data());
  try{
    ALPNS::PluginLoader::addPlugin("InfoService",libs);
  }catch(std::exception &ex){
    std::string errMsg=std::string("InfoService library load failed with \"")+ex.what()+"\";";
    ers::fatal(ALPIssues::DLLIssue(ERS_HERE,errMsg.c_str()));
    throw ALPIssues::DLLIssue(ERS_HERE,errMsg.c_str());
    return false;
  }
  //load HLTImplementation libraries
  libs.clear();
  ERS_DEBUG(1,"Loading HLTSteering libraries");
  BOOST_FOREACH(const boost::property_tree::ptree::value_type &v,
		args.get_child("Configuration.ALPApplication.HLTImplementationLibraries")){
    std::cout<<ALPUtils::getTimeTag()<<"HLT Library= "<<v.second.data()<<std::endl;
    libs.push_back(v.second.data());
  }
  try{
    ALPNS::PluginLoader::addPlugin("HLTImplementation",libs);
  }catch(std::exception &ex){
    std::string errMsg=std::string("HLTSteering libraries load failed with \"")+ex.what()+"\";";
    ers::fatal(ALPIssues::ConfigurationIssue(ERS_HERE,errMsg.c_str()));
    throw ALPIssues::ConfigurationIssue(ERS_HERE,errMsg.c_str());
    return false;
  }
  //construct the objects
  typedef hltinterface::HLTInterface* (*hltcreator)(void);
  typedef hltinterface::DataSource* (*dscreator)(void);  
  typedef hltinterface::IInfoRegister* (*isvccreator)(void);  

  ERS_DEBUG(1,"Instantiating DataSource implementation");
  dscreator dsc=ALPNS::PluginLoader::get("DataSource")->function<dscreator>("create_hltmp_datasource");
  if(!dsc){
    std::cerr<<ALPUtils::getTimeTag()<<"Can't get DataSource factory function. Check configuration! Can't continue. exiting"<<std::endl;
    ers::fatal(ALPIssues::ConfigurationIssue(ERS_HERE,"Can't get DataSource factory function. Check configuration! Can't continue. exiting"));
    //exit(EXIT_FAILURE);
    throw ALPIssues::ConfigurationIssue(ERS_HERE,"Can't get DataSource factory function. Check configuration! Can't continue. exiting");
    return false;
  }
  m_dataSource=dsc();
  ERS_DEBUG(1,"Instantiating Info Service implementation");
  isvccreator isc=ALPNS::PluginLoader::get("InfoService")->function<isvccreator>("create_hltmp_infoservice");
  if(!isc){
    std::cerr<<ALPUtils::getTimeTag()<<"Can't get InfoService factory function. Check configuration! Can't continue. exiting"<<std::endl;
    ers::fatal(ALPIssues::ConfigurationIssue(ERS_HERE,"Can't get InfoService factory function. Check configuration! Can't continue. exiting"));
    //exit(EXIT_FAILURE);
    throw ALPIssues::ConfigurationIssue(ERS_HERE,"Can't get InfoService factory function. Check configuration! Can't continue. exiting");
    return false;
  }

  m_infoService=isc();
  ERS_DEBUG(1,"Instantiating HLTSteering implementation");
  hltcreator hltc=ALPNS::PluginLoader::get("HLTImplementation")->function<hltcreator>("hlt_factory");
  if(!hltc){
    std::cerr<<ALPUtils::getTimeTag()<<"Can't get HLTSteering factory function. Check configuration! Can't continue. exiting"<<std::endl;
    ers::fatal(ALPIssues::ConfigurationIssue(ERS_HERE,"Can't get HLTSteering factory function. Check configuration! Can't continue. exiting"));
    throw ALPIssues::ConfigurationIssue(ERS_HERE,"Can't get HLTSteering factory function. Check configuration! Can't continue. exiting");
    return false;
    //exit(EXIT_FAILURE);
  }

  m_HLTSteering=hltc();
  bool retVal=true;

  //configure infoservice
  {
    ERS_DEBUG(1,"Configuring Info Service implementation");
    boost::property_tree::ptree conf=args.get_child("Configuration.ALPApplication.InfoService");
    try{
      boost::optional<const boost::property_tree::ptree&> extraParms=args.get_child_optional("Configuration.ALPApplication.extraParams");
      if(extraParms){
	conf.add_child("extraParams",(*extraParms));
      }
      try{
	retVal=retVal&&m_infoService->configure(conf);
      }catch(ers::Issue &ex){
	ers::fatal(ALPIssues::ConfigurationIssue(ERS_HERE," InfoService configuration failed! Check configuration",ex));
	throw ALPIssues::ConfigurationIssue(ERS_HERE," InfoService configuration failed! Check configuration",ex);
	return false;
      }	
      if(!retVal){
	ers::fatal(ALPIssues::ConfigurationIssue(ERS_HERE," InfoService configuration failed! Check configuration"));
	throw ALPIssues::ConfigurationIssue(ERS_HERE," InfoService configuration failed! Check configuration");
      }
    }catch(std::exception &ex){
      std::cerr<<ALPUtils::getTimeTag()<<"Caught exception \""<<ex.what()<<"\" during InfoService configuration"<<std::endl; 
      std::string errMsg=std::string("InfoService configuration failed with \"")+ex.what()+"\" Check configuration";
      ers::fatal(ALPIssues::ConfigurationIssue(ERS_HERE,errMsg.c_str()));
      throw ALPIssues::ConfigurationIssue(ERS_HERE,errMsg.c_str());
      return false;
    }
  }

  //configure data source
  {
    ERS_DEBUG(1,"Configuring DataSource implementation");    
    // const boost::property_tree::ptree& conf=args.get_child("Configuration.ALPApplication.DataSource");
    // boost::property_tree::ptree ptemp(conf);
    try{
      // DummyDataSource cannot do EB without ROB info ??
      m_configTree->put("Configuration.ALPApplication.DataSource.L1ResultTimeout",m_l1ResultTimeout);
      bool r=m_dataSource->configure(*m_configTree);
      retVal=retVal&&r;
      if(!r){
	ers::fatal(ALPIssues::ConfigurationIssue(ERS_HERE," DataSource configuration failed! Check configuration"));
	throw ALPIssues::ConfigurationIssue(ERS_HERE," DataSource configuration failed! Check configuration");
	return false;
      }
    }catch(std::exception &ex){
      std::cerr<<ALPUtils::getTimeTag()<<"Caught exception \""<<ex.what()<<"\" during DataSource configuration"<<std::endl; 
      std::string errMsg=std::string("DataSource configuration failed with \"")+ex.what()+"\" Check configuration";
      ers::fatal(ALPIssues::ConfigurationIssue(ERS_HERE,errMsg.c_str()));
      throw ALPIssues::ConfigurationIssue(ERS_HERE,errMsg.c_str());
      return false;
    }
  }

  // configure HLT
  {
    ERS_DEBUG(1,"Configuring HLTSteering implementation");
    try{
      bool r=m_HLTSteering->configure(args);
      retVal=retVal&&r;
      if(!r){
	ers::fatal(ALPIssues::ConfigurationIssue(ERS_HERE,"HLT configuration failed! Check configuration"));
	throw ALPIssues::ConfigurationIssue(ERS_HERE,"HLT configuration failed! Check configuration");
	return false;
      }
    }catch(std::exception &ex){
      std::cerr<<ALPUtils::getTimeTag()<<"Caught exception \""<<ex.what()<<"\" during Steering configuration"<<std::endl; 
      std::string errMsg=std::string("HLTSteering configuration failed with \"")+ex.what()+"\" Check configuration";
      ers::fatal(ALPIssues::ConfigurationIssue(ERS_HERE,errMsg.c_str()));
      throw ALPIssues::ConfigurationIssue(ERS_HERE,errMsg.c_str());
      return false;
    }
  }

  //::sleep(1);
  //ERS_INFO("Steering code loading failed continuing with dummy");  
  //return retVal;
  if(!retVal){
    throw ALPIssues::ConfigurationIssue(ERS_HERE,"Configure failed! Check Configuration");
  }

  // m_motherInfo->LastUserCommand=std::vector<std::string>();
  // m_motherInfo->LastStateTransition="Configure";
  // m_motherInfo->LastExitedChild="";
  m_motherInfo->setIntField(m_MINumKills,0);
  m_motherInfo->setIntField(m_MINumForks,0);
  m_motherInfo->setIntField(m_MIUnexpectedChildExits,0);
  m_motherInfo->setIntField(m_MINumRequested,m_numChildren);
  m_motherInfo->setIntField(m_MINumActive,0);
  m_motherInfo->setIntField(m_MINumExited,0);
  // char * tmp = getenv("TDAQ_PARTITION");
  // std::shared_ptr<IPCPartition> m_part(0);
  // ERS_LOG("Starting IS Publishing");
  // if(tmp){
  //   ERS_LOG("Using partition "<<tmp<<" server "<<m_ISSName<<" with object name "<<m_myName<<".PU_MotherInfo");
  //   try{
  //     m_part=std::make_shared<IPCPartition>(tmp);
  //   }catch(std::exception &ex){
  //     ERS_LOG("Can't create partition object "<<ex.what());
  //   }
  // }
  // if(m_part){
  //   auto id=std::make_shared<ISInfoDictionary>(*m_part);
  //   std::string objName=m_ISSName+"."+m_myName+".PU_MotherInfo";
  //   publishMotherInfo(id,objName);
  // }
  ERS_LOG("--ALP_ConfigureEnd ");
  if(m_saveConfigOpts){
    ALPUtils::dump2File("Configure.xml",args);
    try{
      std::ofstream of("Environ.sh",std::ofstream::out|std::ofstream::trunc);
      for (char **env=environ; *env!=0;env++){
	of<<"export "<<*env<<std::endl;
      }
      of.close();
    }catch(std::exception &ex){
      std::cerr<<ALPUtils::getTimeTag()<<"Failed to dump the environment to file. Error was "<<ex.what()<<std::endl;
    }
  }
  return retVal;
}

bool ALP::connect(const boost::property_tree::ptree& args) {
  //std::cout << "Executing connect..." << std::endl;
  ERS_LOG("--ALP_ConnectStart. using the following configuration tree: ");
  //ERS_LOG("Executing connect... "<<m_myPid);
  printPtree(args,"");
  //std::cout<<ALPUtils::getTimeTag()<<"My Pid is "<<m_myPid<<std::endl;
  if(m_saveConfigOpts){
    ALPUtils::dump2File("Connect.xml",args);
  }

  bool retVal=true;
  if(m_HLTSteering){
    retVal=(m_HLTSteering)->connect(args);
  }
  //::sleep(1);
  if(!retVal){
    ers::fatal(ALPIssues::TransitionIssue(ERS_HERE,"HLTSteering connect() transition failed. Check configuration"));
    throw ALPIssues::TransitionIssue(ERS_HERE,"HLTSteering connect() transition failed. Check configuration");
    return false;
  }
  ERS_LOG("--ALP_ConnectEnd. ");

  return retVal;
}

bool ALP::prepareForRun(const boost::property_tree::ptree& args) {
  //  std::cout<<"Starting prepareForRun with arguments"<<std::endl;
  ERS_LOG("Starting prepare for run, pid= "<<m_myPid);
  ERS_LOG("--ALP_PrepareForRunStart. using the following configuration tree: ");
  //ERS_INFO("Starting prepare for run, pid="<<m_myPid);
  printPtree(args,"");
  //  std::cerr<<"My Pid is "<<m_myPid<<std::endl;
  if(m_saveConfigOpts){
    ALPUtils::dump2File("PrepareForRun.xml",args);
  }

  *m_prepareForRunTree=args;
  bool retVal=true;
  // m_motherInfo->LastUserCommand=std::vector<std::string>();
  // m_motherInfo->LastStateTransition="prepareForRun";
  // m_motherInfo->LastExitedChild="";

  m_motherInfo->setIntField(m_MINumKills,0);
  m_motherInfo->setIntField(m_MINumForks,0);
  m_motherInfo->setIntField(m_MIUnexpectedChildExits,0);
  m_motherInfo->setIntField(m_MINumRequested,m_numChildren);
  m_motherInfo->setIntField(m_MINumActive,0);
  m_motherInfo->setIntField(m_MINumExited,0);

  if(m_infoService){
    bool ret=false;
    try{
      ret=m_infoService->prepareForRun(args);
    }catch(ers::Issue &ex){
      ers::fatal(ALPIssues::TransitionIssue(ERS_HERE,"InfoService failed to complete prepareForRun",ex));
    }catch(...){
      ret=false;
    }
    if(!ret){
      ERS_LOG("InfoService prepareForRun failed");
      return false;
    }
  }

  if(m_HLTSteering){
    try{
      retVal=m_HLTSteering->prepareForRun(args);
    }catch(std::exception &ex){
      std::string errMsg=std::string("PSC threw an exception \"")+ex.what()+"\"";
      ers::fatal(ALPIssues::UnexpectedIssue(ERS_HERE,errMsg.c_str()));
      throw ALPIssues::UnexpectedIssue(ERS_HERE,errMsg.c_str());
      return false;
    }
  }
  if(!retVal){
    ers::fatal(ALPIssues::TransitionIssue(ERS_HERE,"PSC failed prepareForRun transition."));
    throw ALPIssues::TransitionIssue(ERS_HERE,"PSC failed prepareForRun transition.");
    return retVal;
  }
  if(m_nannyThread){
    ERS_LOG("Waiting for nanny thread to Join");
    //ERS_INFO("Waiting for nanny thread to Join");
    stopNanny();
  }
  if(m_motherPublisher){
    ERS_LOG("Waiting for MotherInfo publisher thread to Join");
    //ERS_INFO("Waiting for nanny thread to Join");
    stopMotherPublisher();
  }
  if(m_numChildren<0)m_numChildren=-m_numChildren;
  // ERS_INFO("Preparing to fork "<<m_numChildren<<" child processes. SHUTTING DOWN IPC."
  // 	   <<" Mother process will be outside IPC until forking is completed!.");
  std::cout<<ALPUtils::getTimeTag()<<"Preparing to fork "<<m_numChildren<<" child processes. SHUTTING DOWN IPC."
	   <<" Mother process will be outside IPC until forking is completed!."<<std::endl;

  //sleep(30);
  //printOpenFDs();
  // if(m_childInfo){
  //   m_childInfo.reset();
  // }
  //m_childInfo=new ALP::ALPInfo();
  m_ipcc->shutdownIPC(args);
  //remove this in real system.
  std::cerr<<ALPUtils::getTimeTag()<<"IPC shutdown completed"<<std::endl;
  //  sleep(30);
  //printOpenFDs();
  std::random_device rgen;
  std::mt19937 rEngine(rgen());//seed with a real number possibly from /dev/urandom 
  std::uniform_int_distribution<int> uniform_dist(0,((m_forkDelay>0)?m_forkDelay:1000));
  if(m_preforkSleep>0){
    std::cout<<ALPUtils::getTimeTag()<<"Sleeping for "<<m_preforkSleep<<" milliseconds before starting fork process"<<std::endl;
    try{
      boost::this_thread::sleep(boost::posix_time::milliseconds(m_preforkSleep));//sleep for given milliseconds
    }catch(boost::thread_interrupted &ex){
    }
  }
  for(int i =1;i<=m_numChildren;i++){
    if(m_forkDelay>0){
      try{
	boost::this_thread::sleep(boost::posix_time::milliseconds(uniform_dist(rEngine)));//add a random delay
      }catch(boost::thread_interrupted &ex){
      }

    }
    pid_t t=forkChildren(i);
    if(t!=0){// mother process
      errno=0;
      int spidRet=setpgid(t,m_myPgid);
      if(spidRet!=0){
	//char buff[200];
	//strerror_r(errno,buff,200);
	std::string errNo;
	if(errno==EACCES){
	  errNo="EACCESS";
	}else if(errno==EINVAL){
	  errNo="EINVAL";
	}else if(errno==EPERM){
	  errNo="EPERM";
	}else if(errno==ESRCH){
	  errNo="ESRCH";
	}else{
	  errNo="Unexpected error";
	}
	std::cerr<<ALPUtils::getTimeTag()<<"setpgid failed with "<<spidRet<<" "<<errNo<<std::endl;
	ers::error(ALPIssues::UnexpectedIssue(ERS_HERE,"Can't set pgid. ZOMBIE INFESTATION RISK!!!"));
      }
      //m_motherInfo->setField(m_MINumActive,m_motherInfo->getIntField(m_MINumActive)+1);
      GCIncrIntField(m_motherInfo,m_MINumActive,1);
      m_myChildren[t]=i;
      m_posPidMap[i]=t;
    }else{ //forked children
      m_myPos=i;
      return doProcessLoop(args,i);
    }
  }//Forking loop
  
  //char *dummv=0;
  //int dummc=0;
  //sleep(300);
  // try{
  //   IPCCore::init(dummc,&dummv);
  //   std::cerr<<ALPUtils::getTimeTag()<<"******************************  IPC INIT SUCCEEDED  ******************************"<<std::endl;
  // }catch(std::exception &ex){
  //   std::cerr<<"******************************   ERROR  ******************************"<<std::endl;
  //   std::cerr<<ALPUtils::getTimeTag()<<"IPC Reinitialization failed for pid="<<m_myPid<<" with "<<ex.what()<<std::endl;
  //   std::cerr<<"******************************   ERROR  ******************************"<<std::endl;
  //   std::string errMsg=std::string("IPC Initialization failed with \"")+ex.what()+"\"";
  //   ers::fatal(ALPIssues::UnexpectedIssue(ERS_HERE,errMsg.c_str()));
  //   throw ALPIssues::UnexpectedIssue(ERS_HERE,errMsg.c_str());
  //   return false;
  //   //ERS_LOG("IPC Reinitialization failed");
  // }
  m_ipcc->initializeIPC(args);
  ERS_DEBUG(0,"And we are back!");
  startNanny();
  startMotherPublisher();
  try{
    boost::this_thread::sleep(boost::posix_time::milliseconds(1000));//sleep for given milliseconds  
  }catch(boost::thread_interrupted &ex){
  }

  ERS_LOG("--ALP_prepareForRun End. ");
  if(m_motherInfo->getIntField(m_MINumActive)!=(uint)m_numChildren){
    ers::warning(ALPIssues::UnexpectedIssue(ERS_HERE,"Some children exited immediately after forking!"));
    return false;
  }
  return true;
}

bool ALP::process(const std::vector<eformat::ROBFragment<const uint32_t*> >& /*l1r*/, 
		      hltinterface::HLTResult& /*hltr*/,
		      const hltinterface::EventId& ){
  ERS_LOG("Process method is called! This shouldn't have happened!");
  ers::fatal(ALPIssues::UnexpectedIssue(ERS_HERE,"Process method called!"));
  throw ALPIssues::UnexpectedIssue(ERS_HERE,"Process method called!");
  return false;
}

bool ALP::stopRun(const boost::property_tree::ptree& args) {
  //std::cout << "Stopping the run with args..." << std::endl;
  ERS_LOG("--ALP_stopRun Start. ");
  ERS_LOG("Stopping the run from the mother process, pid="<<m_myPid);
  //ERS_INFO("Stopping the run, pid="<<m_myPid);
  printPtree(args,"stopRun ");
  if(m_saveConfigOpts){
   ALPUtils::dump2File("StopRun.xml",args);
  }
  stopNanny();
  //int timeout=args.get("Configuration.ALPApplication.ApplicationTimeout",55);
  ERS_LOG("Starting terminator thread to kill children in "<<m_FinalizeTimeout<<" seconds");
  boost::thread *terminator=new boost::thread(&ALP::terminateChildren,this,m_FinalizeTimeout);
  collectChildExitStatus();
  m_availableSlots.clear();
  m_diedChildren.clear();
  m_posPidMap.clear();
  m_myChildren.clear();
  m_exitedChildren.clear();
  m_childPidMap.clear();
  terminator->interrupt();
  terminator->join();
  delete terminator;
  terminator=0;
  bool retVal=m_HLTSteering->stopRun(args);

  //m_dataSource->finalize(args);
  m_infoService->finalize(args);
  if(!m_keepNumForks)m_numChildren=m_configTree->get("Configuration.ALPApplication.numForks",4);
  ERS_LOG("--ALP_stopRun End. returning  "<<(retVal?"True":"False"));
  return retVal;
}

void ALP::terminateChildren(int timeOut){
  m_terminationStarted=false;
  if(timeOut>0){
    try{
      boost::this_thread::sleep(boost::posix_time::seconds(timeOut));
    }catch(boost::thread_interrupted &ex){
      //thread is interrupted, means children exited properly
      return;
    }
  }
  ERS_LOG("Reaping children");
  std::map<pid_t,int> pidsToKill(m_myChildren);//make a copy
  m_terminationStarted=true;
  for(std::map<pid_t,int>::iterator it=pidsToKill.begin();it!=pidsToKill.end();++it){
    if(::kill(it->first,0)==0){
      if(::kill(it->first,SIGKILL)!=0){
	//char buff[200];
	//strerror_r(errno,buff,200);
	ERS_LOG("Killing process id "<<it->first<<" failed with error \""<<strerror(errno)<<"\"");
      }else{
	GCIncrIntField(m_motherInfo,m_MINumKills,1);
	GCDecrIntField(m_motherInfo,m_MINumActive,1);
	// m_motherInfo->setIntField(m_MINumKills,m_motherInfo->getIntField(m_MINumKills)+1);
	// m_motherInfo->setIntField(m_MINumActive,m_motherInfo->getIntField(m_MINumActive)-1);
	ERS_LOG("Killed child process "<<it->first);
      }
    }
  }  
  return;
}

bool ALP::disconnect(const boost::property_tree::ptree& args) {
  //std::cout << "Executing disconnect with args..." << std::endl;
  ERS_LOG("--ALP_stopRun Start");
  ERS_LOG("Executing disconnect, pid="<<m_myPid);
  //ERS_INFO("Executing disconnect, pid="<<m_myPid);
  printPtree(args,"");
  ERS_LOG("Does this make sense for mother process?!");
  return m_HLTSteering->disconnect(args);
  // ::sleep(0.5);

  // return true;
}

bool ALP::unconfigure(const boost::property_tree::ptree& args) {
  //std::cout << "Executing unconfigure with args" << std::endl;
  ERS_LOG("--ALP_unconfigure Start");
  ERS_LOG("Executing unconfigure, pid="<<m_myPid);
  printPtree(args,"");
  if(m_saveConfigOpts){
    ALPUtils::dump2File("Unconfigure.xml",args);
  }
  std::cout<<ALPUtils::getTimeTag()<<"My Pid is "<<m_myPid<<std::endl;
  //::sleep(0.5);
  stopMotherPublisher();
  bool retVal=m_HLTSteering->unconfigure(args);
  std::cout<<ALPUtils::getTimeTag()<<"Returning "<<(retVal?"True":"False")<<" from unconfigure "<<std::endl;
  ERS_LOG("--ALP_unconfigure End");
  return retVal;
}

bool ALP::publishStatistics(const boost::property_tree::ptree& args) {
  //std::cout<<"Publish Stats with args"<<std::endl;
  ERS_LOG("Executing publish stats, pid="<<m_myPid);
  printPtree(args,"");

  std::cout<<ALPUtils::getTimeTag()<<"My Pid is "<<m_myPid<<std::endl;

  return true;
}

void ALP::timeOutReached(const boost::property_tree::ptree& args) {
  //std::cout << "Executing timeOutReached with args..." << std::endl;
  ERS_LOG("Executing timeOut Reached, pid="<<m_myPid);
  printPtree(args,"");

}

bool ALP::hltUserCommand(const boost::property_tree::ptree& args) {
  //std::cout << "Executing hltUserCommand with args..." << std::endl;
  ERS_LOG("Executing hltUserCommand, pid="<<m_myPid);
  printPtree(args,"userCommand->");
  std::string Command=args.get_child("Configuration.COMMANDNAME").data();
  std::stringstream oss;
  if(Command=="FORK"){
    int count=args.get("Configuration.COUNT",1);
    std::string forkArgs=args.get_child("Configuration.COUNT").data();
    //m_motherInfo->LastUserCommand=std::vector<std::string>{Command,forkArgs};
    stopNanny();
    stopMotherPublisher();
    // try{
    //   IPCCore::shutdown();
    //   std::cerr<<ALPUtils::getTimeTag()<<"******************************  IPC SHUTDOWN SUCCEEDED  ******************************"<<std::endl;
    // }catch(daq::ipc::NotInitialized &ex){
    //   std::cerr<<"******************************   ERROR  ******************************"<<std::endl;
    //   std::cerr<<ALPUtils::getTimeTag()<<"IPC shutdown failed with NotInitialized! reason= "<<ex.what()<<std::endl;
    //   std::cerr<<"******************************   ERROR  ******************************"<<std::endl;
      
    // }catch(daq::ipc::CorbaSystemException &ex){
    //   std::cerr<<"******************************   ERROR  ******************************"<<std::endl;
    //   std::cerr<<ALPUtils::getTimeTag()<<"IPC shutdown failed with CorbaSystemException! reason= "<<ex.what()<<std::endl;
    //   std::cerr<<"******************************   ERROR  ******************************"<<std::endl;
    // }catch(std::exception &ex){
    //   std::cerr<<ALPUtils::getTimeTag()<<"Caught unexpected exception"<<std::endl;
    //   std::string errMsg=std::string("Caught unexpected exception \"")+ex.what()+"\" during IPC Shutdown.!";
    //   ers::fatal(ALPIssues::UnexpectedIssue(ERS_HERE,errMsg.c_str()));
    // 	return false;
    // }
    m_ipcc->shutdownIPC(args);
    for(int i=0;i<count;i++){
      int pos;
      if(m_availableSlots.empty()){
	m_numChildren++;
	m_motherInfo->setIntField(m_MINumRequested,m_numChildren);
	// m_motherInfo->setIntField(m_MINumActive,m_motherInfo->getIntField(m_MINumActive)+1);
	GCIncrIntField(m_motherInfo,m_MINumActive,1);
	pos=m_numChildren;
	pid_t t=forkChildren(pos);
	if(t!=0){//mother 
	  errno=0;
	  int spidRet=setpgid(t,m_myPgid);
	  if(spidRet!=0){
	    //char buff[200];
	    //strerror_r(errno,buff,200);
	    std::string errNo;
	    if(errno==EACCES){
	      errNo="EACCESS";
	    }else if(errno==EINVAL){
	      errNo="EINVAL";
	    }else if(errno==EPERM){
	      errNo="EPERM";
	    }else if(errno==ESRCH){
	      errNo="ESRCH";
	    }else{
	      errNo="Unexpected error";
	    }
	    std::cerr<<ALPUtils::getTimeTag()<<"setpgid failed with "<<spidRet<<" "<<errNo<<std::endl;
	    oss<<ALPUtils::getTimeTag()<<"Can't set pgid for child "<<pos
	       <<" pid="<<t<<" setpgid failed with "<<spidRet<<" errNo="<<errNo<<std::endl;
	    //ers::warning(ALPIssues::UnexpectedIssue(ERS_HERE,"Can't set pgid"));
	  }
	  m_myChildren[t]=pos;
	  m_posPidMap[pos]=t;
	}else{//children
	  return doProcessLoop(*m_prepareForRunTree,pos);
	}
      }else{// avaliable slots exist
	pos=m_availableSlots.front();
	m_availableSlots.pop_front();
	pid_t t=forkChildren(pos);
	if(t!=0){//mother 
	  errno=0;
	  int spidRet=setpgid(t,m_myPgid);
	  if(spidRet!=0){
	    //char buff[200];
	    //strerror_r(errno,buff,200);
	    std::string errNo;
	    if(errno==EACCES){
	      errNo="EACCESS";
	    }else if(errno==EINVAL){
	      errNo="EINVAL";
	    }else if(errno==EPERM){
	      errNo="EPERM";
	    }else if(errno==ESRCH){
	      errNo="ESRCH";
	    }else{
	      errNo="Unexpected error";
	    }
	    std::cerr<<ALPUtils::getTimeTag()<<"setpgid failed with "<<spidRet<<" "<<errNo<<std::endl;
	    oss<<ALPUtils::getTimeTag()<<"Can't set pgid for child "<<pos<<" pid="<<t
	       <<" setpgid failed with "<<spidRet<<" errno "<<errNo<<std::endl;
	    //ers::warning(ALPIssues::UnexpectedIssue(ERS_HERE,"Can't set pgid"));
	  }
	  m_myChildren[t]=pos;
	  m_posPidMap[pos]=t;
	  //m_motherInfo->NumActive++;
	  GCIncrIntField(m_motherInfo,m_MINumActive,1);
	}else{//children
	  return doProcessLoop(*m_prepareForRunTree,pos);
	}
      }
    }
    if(oss.str().length()>0){
      ers::warning(ALPIssues::UnexpectedIssue(ERS_HERE,(std::string("Encountered errors during forking ")+oss.str()).c_str()));
    }
    // try{
    //   char *dummv=0;
    //   int dummc=0;
    //   IPCCore::init(dummc,&dummv);
    //   std::cerr<<ALPUtils::getTimeTag()<<"******************************  IPC INIT SUCCEEDED  ******************************"<<std::endl;
    //   std::cerr.flush();
    //   //sleep(60);
    // }catch(std::exception &ex){
    //   std::cerr<<"******************************   ERROR  ******************************"<<std::endl;
    //   std::cerr<<ALPUtils::getTimeTag()<<"IPC Reinitialization failed for pid="<<getpid()<<" with "<<ex.what()<<std::endl;
    //   std::cerr<<"******************************   ERROR  ******************************"<<std::endl;
    //   std::cerr.flush();
    // }
    m_ipcc->initializeIPC(args);
    startNanny();
    startMotherPublisher();
  }else if(Command=="KILL"){
    std::string childName=args.get_child("Configuration.CHILDNAME").data();
    std::cout<<ALPUtils::getTimeTag()<<"Old child name =\""<<childName<<"\""<<std::endl;
    boost::algorithm::trim(childName);
    std::cout<<ALPUtils::getTimeTag()<<"Trimmed child name =\""<<childName<<"\""<<std::endl;
    //m_motherInfo->LastUserCommand=std::vector<std::string>{Command,childName};
    stopNanny();
    std::map<std::string,pid_t>::iterator it=m_childPidMap.find(childName);
    if(it!=m_childPidMap.end()){
      if(::kill(it->second,0)==0){
	if(::kill(it->second,SIGKILL)!=0){
	  //char buff[200];
	  //strerror_r(errno,buff,200);
	  ERS_LOG("Killing process id "<<it->second<<" failed with error \""<<strerror(errno)<<"\"");
	}else{
	  ERS_LOG("Killed child process "<<it->first<<" pid="<<it->second);
	  // m_motherInfo->NumKills++;
	  // m_motherInfo->NumActive--;
	  GCIncrIntField(m_motherInfo,m_MINumKills,1);
	  GCDecrIntField(m_motherInfo,m_MINumActive,1);
	  //m_motherInfo->LastExitedChild=childName;
	  std::map<pid_t,int>::iterator itPid=m_myChildren.find(it->second);
	  if(itPid!=m_myChildren.end()){
	    m_availableSlots.push_back(itPid->second);
	  }
	  m_myChildren.erase(itPid);
	  m_childPidMap.erase(it);
	}
      }else{
	ERS_LOG("Child "<<childName<<" with pid "<<it->second<<" don't exist!");
      }
    }else{
      ERS_LOG("Child \""<<childName<<"\" don't exist!");      
    }
    startNanny();
  }else if(Command=="KILLALL"){
    stopNanny();
    terminateChildren(0);
    collectChildExitStatus();
  }else{
    std::string arg=args.get_child("Configuration.ARG").data();
    std::cout<<ALPUtils::getTimeTag()<<"Got Command \""<<Command<<"\" with arguments \""<<arg<<"\""<<std::endl;
    std::cout<<ALPUtils::getTimeTag()<<"Command transfer is not implemented yet"<<std::endl;
  }
  // BOOST_FOREACH(const boost::property_tree::ptree::value_type &v,
  // 		args.get_child("Configuration")){
			
  return true;
}

bool ALP::doProcessLoop(const boost::property_tree::ptree& args,int childNo){
  errno=0;
  int spidRet=setpgid(0,m_myPgid);
  if(spidRet!=0){
    //char buff[200];
    //strerror_r(errno,buff,200);
    std::string errNo;
    if(errno==EACCES){
      errNo="EACCESS";
    }else if(errno==EINVAL){
      errNo="EINVAL";
    }else if(errno==EPERM){
      errNo="EPERM";
    }else if(errno==ESRCH){
      errNo="ESRCH";
    }else{
      errNo="Unexpected error";
    }
    ERS_LOG("setpgid failed with "<<spidRet<<" "<<errNo);
    ers::error(ALPIssues::UnexpectedIssue(ERS_HERE,"Can't set pgid! ZOMBIE INFESTATION RISK!"));
  }
  ERS_LOG("I am a forked child "<<childNo<<" with pid="<<m_myPid);
  //ERS_INFO("Dumping something to ers::info from pid "<<m_myPid);      
  boost::property_tree::ptree conf;
  conf.put("start_id",childNo);
  conf.put("stride",m_numChildren);
  conf.put("appName",m_myName);// used by the PSC
  conf.put("clientName",m_myName);
  conf.put("workerId",childNo);//used by PSC
  conf.put("numberOfWorkers",m_numChildren);// used by PSC
  if(m_infoService){
    try{
      m_infoService->prepareWorker(conf);
    }catch(ers::Issue &ex){
      ERS_LOG("InfoService prepareWorker failed");
      ers::fatal(ALPIssues::TransitionIssue(ERS_HERE,"InfoService failed to complete prepareWorker",ex));
      return false;
    }catch(std::exception &ex){
      ERS_LOG("InfoService prepareWorker failed");
      ers::fatal(ALPIssues::TransitionIssue(ERS_HERE,"InfoService failed to complete prepareWorker",ex));
      return false;
    }

  }
  ERS_DEBUG(1,"InfoService completed preparation.");
  if(m_dataSource){
    //if(!m_dataSource->prepareForRun(args)){
    try{
      ERS_DEBUG(1,"Trying prepareForRun for datasource");
      m_dataSource->prepareForRun(args);
    }catch(ers::Issue &ex){
      ERS_LOG("DataSource prepareForRun failed with"<<ex.what());
      std::string msg="DataSource failed to complete prepareForRun transition with message: ";
      msg+=ex.what();
      ers::fatal(ALPIssues::TransitionIssue(ERS_HERE,msg.c_str(),ex));
      return false;
    }catch(std::exception &ex){
      ERS_LOG("DataSource prepareForRun failed with"<<ex.what());
      std::string msg="DataSource failed to complete prepareForRun transition with message: ";
      msg+=ex.what();
      ers::fatal(ALPIssues::TransitionIssue(ERS_HERE,msg.c_str()));
      return false;
    }
    try{
      ERS_DEBUG(1,"Trying prepareWorker for datasource");
      m_dataSource->prepareWorker(conf);
    }catch(ers::Issue &ex){
      ERS_LOG("DataSource prepareWorker failed with"<<ex.what());
      std::string msg="DataSource failed to complete prepareWorker transition with message: ";
      msg+=ex.what();
      ers::fatal(ALPIssues::TransitionIssue(ERS_HERE,msg.c_str(),ex));
      return false;
    }catch(std::exception &ex){
      ERS_LOG("DataSource prepareWorker failed with"<<ex.what());
      std::string msg="DataSource failed to complete prepareWorker transition with message: ";
      msg+=ex.what();
      ers::fatal(ALPIssues::TransitionIssue(ERS_HERE,msg.c_str()));
      return false;
    }
    ERS_DEBUG(1,"DataSource completed preparation");
    if(m_HLTSteering){
      ERS_DEBUG(1,"Trying prepareWorker for PSC");
      if(!m_HLTSteering->prepareWorker(conf)){
	ERS_LOG("HLT Steering prepareWorker failed. !");
	ers::fatal(ALPIssues::TransitionIssue(ERS_HERE,"HLT Steering failed to complete prepareWorker"));
	return false;
      }
      std::vector<eformat::ROBFragment<const uint32_t*> > l1r;
      hltinterface::HLTResult hltr;
      hltinterface::EventId evId;
      evId.globalId=0;
      evId.lbNumber=0;
      evId.l1Id=0;
      uint32_t * fragmentBuff=new uint32_t[hltinterface::HLTResult::DEFAULT_MAX_RESULTSIZE]; //approximately 33MB i.e. 2^25 bytes
      hltr.fragment_pointer=fragmentBuff;
      hltr.max_result_size=hltinterface::HLTResult::DEFAULT_MAX_RESULTSIZE;
      ERS_LOG("--ALP_Starting Processing Loop");
      std::cerr<<ALPUtils::getTimeTag()<<" Starting processing loop"<<std::endl;
      m_childInfo->setIntField(m_CINumEvents,0);
      m_childInfo->setIntField(m_CIAcceptedEvents,0);
      m_childInfo->setIntField(m_CIRejectedEvents,0);
      m_childInfo->setIntField(m_CIL1ResultFetchTimeouts,0);
      m_childInfo->setIntField(m_CISoftTimeouts,0);
      m_childInfo->setIntField(m_CILongestWaitForL1Result,0);
      m_childInfo->setIntField(m_CILongestProcessingTime,0);
      m_childInfo->setFloatField(m_CIAverageAcceptTime,0);
      m_childInfo->setFloatField(m_CIAverageRejectTime,0);
      m_childInfo->setFloatField(m_CIAverageProcessingTime,0);
      m_childInfo->setFloatField(m_CIAverageL1ResultTime,0);
      m_childInfo->setFloatField(m_CITimePercentInProcessing,0);
      m_childInfo->setFloatField(m_CITimePercentInAccept,0);
      m_childInfo->setFloatField(m_CITimePercentInReject,0);
      m_childInfo->setFloatField(m_CITimePercentInWait,0);
      m_childInfo->setFloatField(m_CITimePercentInSend,0);

      m_accDuration=std::chrono::milliseconds(0);
      m_rejDuration=std::chrono::milliseconds(0);
      m_waitDuration=std::chrono::milliseconds(0);
      m_sendDuration=std::chrono::milliseconds(0);
      m_procDuration=std::chrono::milliseconds(0);
      m_totDuration=std::chrono::milliseconds(0);

      m_accDurationCum=std::chrono::milliseconds(0);
      m_rejDurationCum=std::chrono::milliseconds(0);
      m_waitDurationCum=std::chrono::milliseconds(0);
      m_sendDurationCum=std::chrono::milliseconds(0);
      m_procDurationCum=std::chrono::milliseconds(0);
      m_totDurationCum=std::chrono::milliseconds(0);

      m_histos.push_back(new TH1F("L1RequestTiming","L1Result receive times",1000,0.,1000.));
      m_histos.push_back(new TH1F("AcceptedTiming","Event Accept Duration",1000,0.,4000.));
      m_histos.push_back(new TH1F("RejectedTiming","Event Reject Duration",1000,0.,4000.));
      m_histos.push_back(new TH1F("ProcessingTiming","Event Processing Duration",1000,0.,4000.));
      hltinterface::IInfoRegister::instance()->registerTObject(std::string("ALP"),std::string("L1RequestTime"),m_histos[0]);
      hltinterface::IInfoRegister::instance()->registerTObject("ALP","AcceptDecisionTime",m_histos[1]);
      hltinterface::IInfoRegister::instance()->registerTObject("ALP","RejectDecisionTime",m_histos[2]);
      hltinterface::IInfoRegister::instance()->registerTObject("ALP","TotalProcessingTime",m_histos[3]);
      m_publisherThread=new boost::thread(&ALP::statsPublisher,this);
      ERS_LOG("Starting timeoutThread with "<<m_softTimeout/1000<<" seconds to timeout");
      m_timeoutThread.reset(new std::thread(std::bind(&ALP::runTimer,this)));
      ERS_LOG("--ALP_Processing Start. ");
      bool cleanExit=true;
      std::chrono::time_point<std::chrono::steady_clock> tStart;
      while(m_processEvents){
	uint32_t l1id=0;
	try{
	  m_timeoutCond.notify_all();
	  {
	    std::lock_guard<std::mutex> lock (m_statMutex);
	    tStart=std::chrono::steady_clock::now();
	  }
	  m_dataSource->getL1Result(l1r,l1id,evId.globalId,evId.lbNumber);
	  std::chrono::time_point<std::chrono::steady_clock> tL1=std::chrono::steady_clock::now();
	  {
	    std::lock_guard<std::mutex> lock(m_timeoutMutex);
	    m_softTOTrigger=true;
	    m_hardTOTrigger=false;
	    m_TOTimerStart=tL1;
	    m_timeoutCond.notify_all();
	  }
	  evId.l1Id=l1id;
	  auto dtime=std::chrono::duration_cast<std::chrono::milliseconds>(tL1-tStart);
	  {
	    std::lock_guard<std::mutex> lock(m_statMutex);
	    m_waitDuration+=dtime;
	    m_waitDurationCum+=dtime;	    
	  }
	  uint deltaT=dtime.count();
	  m_histos[0]->Fill(deltaT);
	  HLTPU::ScopedISHelper IInfoHelper(m_lbNum,m_evtNum);
	  tL1=std::chrono::steady_clock::now();
	  if(!m_HLTSteering->process(l1r,hltr,evId)){
	    ers::fatal(ALPIssues::UnexpectedIssue(ERS_HERE,
						     "Steering failed in process() method. Exiting!"));
	    return false;
	  }
	  {
	    std::lock_guard<std::mutex> lock(m_timeoutMutex);
	    m_softTOTrigger=false;
	    m_hardTOTrigger=false;
	    if(m_childInfo->getIntField(m_CILongestWaitForL1Result)<deltaT){
	      m_childInfo->setIntField(m_CILongestWaitForL1Result,deltaT);
	    }
	  }
	  std::chrono::time_point<std::chrono::steady_clock> tProc=std::chrono::steady_clock::now();
	  dtime=std::chrono::duration_cast<std::chrono::milliseconds>(tProc-tL1);
	  deltaT=dtime.count();
	  bool accept=((hltr.stream_tag.size()>0)?true:false);
	  tL1=std::chrono::steady_clock::now();
	  m_dataSource->sendResult(accept,l1id,hltr);
	  tProc=std::chrono::steady_clock::now();
	  {
	    std::lock_guard<std::mutex> lock(m_statMutex);
	    m_procDuration+=dtime;
	    m_procDurationCum+=dtime;
	    m_eventsInInterval++;
	  }
	  if(m_childInfo->getIntField(m_CILongestProcessingTime)<deltaT){
	    m_childInfo->setIntField(m_CILongestProcessingTime,deltaT);
	  }
	  if(accept){
	    // m_childInfo->AverageAcceptTime+=deltaT;
	    GCIncrIntField(m_childInfo,m_CIAcceptedEvents,1);
	    m_histos[1]->Fill(deltaT);
	    {
	      std::lock_guard<std::mutex> lock(m_statMutex);
	      m_accDuration+=dtime;
	      m_accDurationCum+=dtime;
	      m_acceptedInInterval++;
	    }
	  }else{
	    //m_childInfo->AverageRejectTime+=deltaT;
	    GCIncrIntField(m_childInfo,m_CIRejectedEvents,1);
	    m_histos[2]->Fill(deltaT);
	    {
	      std::lock_guard<std::mutex> lock(m_statMutex);
	      m_rejDuration+=dtime;
	      m_rejDurationCum+=dtime;
	      m_rejectedInInterval++;
	    }
	  }
	  m_histos[3]->Fill(deltaT);
	  GCIncrIntField(m_childInfo,m_CINumEvents,1);
	  dtime=std::chrono::duration_cast<std::chrono::milliseconds>(tProc-tL1);
	  {
	    std::lock_guard<std::mutex> lock(m_statMutex); 
	    m_sendDuration+=dtime;
	    m_sendDurationCum+=dtime;
	  }
	  //std::cerr<<"Result send "<<evtCount<<std::endl;
	}catch(ALPNS::DSErrors::NoMoreEvents &ex){
	  ERS_LOG("ALP Caught NoMoreEvents exception: "<<ex.what());
	  m_processEvents=false;
	  break;
	}catch(ALPNS::DSErrors::EventNotReady &ex){
	  continue;
	}catch(ALPNS::UnexpectedException &ex){
	  ers::fatal(ALPIssues::UnexpectedIssue(ERS_HERE,"Unexpected Exception happened",ex));
	  m_processEvents=false;
	  break;
	}catch(ers::Issue &ex){
	  ers::fatal(ALPIssues::UnexpectedIssue(ERS_HERE,"Got an uncaught ERS issue",ex));
	  m_processEvents=false;
	  break;
	}catch(std::exception &ex){
	  ERS_LOG("ALP Caught an exception in processing loop: "<<ex.what());
	  m_processEvents=false;
	  std::string errMsg=std::string("Caught an unexpected exception in processing loop \"")+ex.what()+"\" Exiting!";
	  ers::fatal(ALPIssues::UnexpectedIssue(ERS_HERE,errMsg.c_str()));
	  break;
	}catch(...){
	  ERS_LOG("ALP Caught an unexpected non-std exception: ALP Doesn't know this exception. Exiting! ");
	  std::cerr<<"ALP Caught an unexpected non-std exception: ALP Doesn't know this exception. Check Log files Exiting!"<<std::endl;
	  m_processEvents=false;
	  ers::fatal(ALPIssues::UnexpectedIssue(ERS_HERE,"Caught a non-std exception in processing loop! Check Log files! Exiting!"));
	  break;
	}
	l1r.clear();
	hltr.trigger_info.clear();
	hltr.stream_tag.clear();
	hltr.psc_errors.clear();   
	hltr.hltResult_robs.clear();
	{
	  std::lock_guard<std::mutex> lock(m_statMutex); 
	  auto tdiff=std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now()-tStart);
	  m_totDuration+=tdiff;
	  m_totDurationCum+=tdiff;
	}
	if(m_interEventSleep_ms>0){
	  int sleepDuration=m_interEventSleep_ms;
	  if(m_interEventSpread_ms>0){
	    std::random_device rgen;
	    std::mt19937 rEngine(rgen());//seed with a real number possibly from /dev/urandom 
	    std::uniform_int_distribution<int> uniform_dist(0,((m_interEventSpread_ms>0)?m_interEventSpread_ms:1000));
	    sleepDuration+=uniform_dist(rEngine);
	  }
	  std::this_thread::sleep_for(std::chrono::milliseconds(sleepDuration));// sleep for given milliseconds to reduce trigger rate
	}
      }//end while
      // do finalization.
      ERS_LOG("--ALP_Processing Ended. ");
      {
	std::lock_guard<std::mutex> lock(m_timeoutMutex);
	m_timerWork=false;
	m_softTOTrigger=false;
	m_hardTOTrigger=false;
      }
      m_timeoutCond.notify_all();
      m_timeoutThread->join();
      delete[] fragmentBuff;
      ERS_LOG("Processing loop finished. Finalizing");
      if(m_termStagger>0){
	waitForFreeMem(std::min((int)(m_FinalizeTimeout*0.7),m_termStagger));
      }
      if(!m_skipFinalize){
	if(!m_HLTSteering->stopRun(conf)){
	  ERS_LOG("HLT stopRun failed.");
	}else{
	  ERS_LOG("--ALP_stopRun Finished ");
	  if(!m_skipFinalizeWorker){
	    if(!m_HLTSteering->finalizeWorker(conf)){
	      ERS_LOG("HLT Finalize worker failed.");
	    }
	    ERS_LOG("--ALP_finalizeWorker Finished ");
	  }else{
	    ERS_LOG("--ALP_finalizeWorker Skipped! ");
	  }
	}
	ERS_LOG("--ALP Steering finalization completed");
	delete m_HLTSteering;
	ERS_LOG("--ALP Steering deletion completed");
      }else{
	ERS_LOG("--ALP_stopRun SKIPPED! ");       
      }
      m_dataSource->finalizeWorker(conf);
      m_dataSource->finalize(conf);
      delete m_dataSource;
      ERS_LOG("DataFlow finalization completed.");
      ERS_LOG("Waiting for last publication.");//need to fix this
      m_publisherThread->interrupt();
      m_publisherThread->join();
      delete m_publisherThread;
      m_publisherThread=0;
      ERS_LOG("Publisher thread joined. Shutting down InfoService.");//need to fix this
      m_infoService->finalizeWorker(conf);
      m_infoService->finalize(conf);
      delete m_infoService;
      m_infoService=0;
      //_exit(0);
      //delete m_childInfo;
      //m_childInfo=0;
      // for(uint i=0;i<m_histos.size();i++){
      // 	delete m_histos.at(i);
      // }
      m_histos.clear();
      ERS_LOG("Returning from cildren.");
      if(m_threadsExist){
	std::cerr<<ALPUtils::getTimeTag()<<" Threads existed during forking. calling std::_Exit()"<<std::endl;
	std::_Exit(EXIT_FAILURE);
      }
      if(m_exitImmediately){
	std::cerr<<ALPUtils::getTimeTag()<<" Direct exit requested. Calling std::_Exit()"<<std::endl;
	std::cerr.flush();
	std::_Exit(120);	
      }
      return cleanExit;
    }else{
      ERS_LOG("No HLT Steering defined! Returning false");
      ers::fatal(ALPIssues::ConfigurationIssue(ERS_HERE,"No HLT Steering defined yet prepareForRun is called!"));
      return false;
    }
  }
  return true;
}

bool ALP::prepareWorker(const boost::property_tree::ptree& /*args*/) {
  ers::fatal(ALPIssues::UnexpectedIssue(ERS_HERE,"prepareWorker method called!"));
  return false;
}

bool ALP::finalizeWorker(const boost::property_tree::ptree& /*args*/) {
  ers::fatal(ALPIssues::UnexpectedIssue(ERS_HERE,"finalizeWorker method called!"));
  return true;
}

pid_t ALP::forkChildren(int pos){
  if(m_dumpFD)printOpenFDs("mother pre-fork->");
  if(m_dumpThreads)printTasks("mother pre-fork Tasks(threads)->");
  m_threadsExist=(countThreads()>1);
  pid_t t=fork();
  if(t!=0){// parent process
    std::cerr<<ALPUtils::getTimeTag()<<"Forked a child with pid "<<t<<std::endl;
    std::cout<<ALPUtils::getTimeTag()<<"AfterFork"<<std::endl;
    //m_motherInfo->NumForks++;
    GCIncrIntField(m_motherInfo,m_MINumKills,1);
    if(m_dumpFD)printOpenFDs("mother post-fork->");
    if(m_dumpThreads)printTasks("mother post-fork Tasks(threads)->");
    const int bufflen=8192;
    char buff[bufflen];
    char * TDAQAPPNAME=getenv("TDAQ_APPLICATION_NAME");
    snprintf(buff,bufflen,"%s-%02d",TDAQAPPNAME,pos);
    std::cout<<ALPUtils::getTimeTag()<<"Adding \""<<buff<<"\" with pid= "<<t<<std::endl;
    m_childPidMap[buff]=t;
    fflush(stdout);
    fflush(stderr);
    return t;
  }else{// child process detached from parent.
    if(m_dumpFD)printOpenFDs("child pre-redirection->");
    if(m_dumpThreads)printTasks("child pre-redirection Tasks(threads)->");
    //char *dummv=0;
    //int dummc=0;
    std::cerr<<ALPUtils::getTimeTag()<<"Fork done"<<std::endl;
    std::cout<<ALPUtils::getTimeTag()<<"Fork done"<<std::endl;
    m_myPid=getpid();
    char * TDAQAPPNAME=getenv("TDAQ_APPLICATION_NAME");
    std::string logsPath=m_childLogPath;
    //if(logEnv)logsPath=logEnv;
    auto tnow=std::chrono::system_clock::now();
    long tepoch=std::chrono::duration_cast<std::chrono::seconds>(tnow.time_since_epoch()).count();
    const int bufflen=8192;
    char buff[bufflen];
    if(logsPath.empty()){
      logsPath="/tmp";
    }
    snprintf(buff,bufflen,"%s:%02d",
	     TDAQAPPNAME,pos);
    setenv("TDAQ_APPLICATION_NAME",buff,1);
    snprintf(buff,bufflen,"%s/%s:%02d-%d-%ld.out",
	     logsPath.c_str(),TDAQAPPNAME,pos,m_myPid,tepoch);
    std::cerr<<ALPUtils::getTimeTag()<<"I am the child # "<<pos<<" with pid "<<m_myPid<<". Redirecting stdout to "<<buff<<std::endl;
    fflush(stdout);
    freopen(buff,"a",stdout);
    snprintf(buff,bufflen,"%s/%s:%02d-%d-%ld.err",
	     logsPath.c_str(),TDAQAPPNAME,pos,m_myPid,tepoch);
    std::cerr<<ALPUtils::getTimeTag()<<"I am the child # "<<pos<<" with pid "<<m_myPid<<". Redirecting stderr to "<<buff<<std::endl;

    fflush(stderr);
    freopen(buff,"a",stderr);
    //sleep(30);

    if(m_dumpFD)printOpenFDs("child post-redirection->");
    if(m_dumpThreads)printTasks("child post-redirection Tasks(threads)->");
    // try{
    //   IPCCore::init(dummc,&dummv);
    //   std::cerr<<ALPUtils::getTimeTag()<<"******************************  IPC INIT SUCCEEDED  ******************************"<<std::endl;
    //   std::cerr.flush();
    //   //sleep(60);
    // }catch(std::exception &ex){
    //   std::cerr<<"******************************   ERROR  ******************************"<<std::endl;
    //   std::cerr<<ALPUtils::getTimeTag()<<"IPC Reinitialization failed for pid="<<getpid()<<" with "<<ex.what()<<std::endl;
    //   std::cerr<<"******************************   ERROR  ******************************"<<std::endl;
    //   std::cerr.flush();
    // }
    boost::property_tree::ptree args;
    m_ipcc->initializeIPC(args);

    std::cerr<<ALPUtils::getTimeTag()<<"IPC reinitialization done "<<m_myPid<<std::endl;
    //printOpenFDs();
    prctl( PR_SET_PDEATHSIG, SIGTERM );
    ::signal(SIGCHLD,SIG_DFL);
    ::signal(SIGKILL,SIG_DFL);
    ::signal(SIGTERM,SIG_DFL);

    //ERS_LOG("I am the children with, pid="<<m_myPid<<". Redirecting output to logfiles ");
    //ERS_INFO("I am the children with, pid="<<m_myPid<<". Redirecting output to logfiles ");
    //char * logEnv=getenv("TDAQ_LOGS_PATH");
    ERS_LOG("I am the child # "<<pos<<" with, pid="<<m_myPid<<". Redirection is completed");
    //ERS_INFO("I am the children with, pid="<<m_myPid<<". Redirection is completed");
    snprintf(buff,bufflen,"%s-%02d",TDAQAPPNAME,pos);
    m_myName=buff;
  }
  return t;
}

void ALP::doNannyWork(){
  int count=0;
  pid_t baby=0;
  while(m_nannyWork && baby!=-1){
    errno=0;
    int retVal=0;
    baby=waitpid(0,&retVal,WNOHANG);
    //ERS_LOG("DOING NANNY WORK! baby= "<<baby);
    if(baby==0){// if all children working sleep 1 seconds
      try{
	boost::this_thread::sleep(boost::posix_time::seconds(1));
      }catch(boost::thread_interrupted &ex){
	//don't need to sleep anymore
	if(!m_nannyWork) return;
      }
    }else if(baby==-1){//if no child remains
      //m_motherInfo->NumActive=0;
      m_motherInfo->setIntField(m_MINumActive,0);
      if(errno==ECHILD){
	ERS_LOG("All children are exited. Returning");
	return;
      }
      count++;
      //char buff[200];
      //strerror_r(errno,buff,200);
      ERS_LOG("waitpid returned "<<::strerror(errno));
      //ERS_INFO("waitpid returned "<<buff);
      if(count>10)return; //killswitch 

    }else{// something happened to children
      if(WIFEXITED(retVal)){
	//should we ask exit status to be 0 in order to catch failures?
	int exitStat=WEXITSTATUS(retVal);
	ERS_LOG("Child with PID="<<baby<<" exited normally with status="<<exitStat);
	// m_motherInfo->NumActive--;	
	// m_motherInfo->NumExited++;
	GCIncrIntField(m_motherInfo,m_MINumExited,1);
	GCDecrIntField(m_motherInfo,m_MINumActive,1);
	if(exitStat!=0){
	  char errbuff[200];
	  snprintf(errbuff,200,"Child pid= %d exited with unexpected return value %d ",baby,exitStat);
	  ers::warning(ALPIssues::ChildIssue(ERS_HERE, errbuff));
	  //m_motherInfo->UnexpectedChildExits++;
	  GCIncrIntField(m_motherInfo,m_MIUnexpectedChildExits,1);
	}
	std::map<pid_t,int>::iterator it=m_myChildren.find(baby);
	if(it!=m_myChildren.end()){
	  m_availableSlots.push_back(it->second);
	  // const int bufflen=8192;
	  // char buff[bufflen];
	  // char * TDAQAPPNAME=getenv("TDAQ_APPLICATION_NAME");
	  // snprintf(buff,bufflen,"%s-%02d",TDAQAPPNAME,it->second);
	  // m_motherInfo->LastExitedChild=buff;
	  m_myChildren.erase(it);// remove it from the map
	}
      }
      if(WIFSIGNALED(retVal)){
	int exitStat=WTERMSIG(retVal);
	ERS_LOG("Child with PID="<<baby<<" exited with a signal="<<WTERMSIG(retVal));
	// m_motherInfo->NumActive--;	
	// m_motherInfo->NumExited++;
	GCIncrIntField(m_motherInfo,m_MINumExited,1);
	GCDecrIntField(m_motherInfo,m_MINumActive,1);

	if(exitStat!=0){
	  char errbuff[200];
	  snprintf(errbuff,200,"Child pid= %d exited with signal  %d ",baby,exitStat);
	  // m_motherInfo->UnexpectedChildExits++;
	  GCIncrIntField(m_motherInfo,m_MIUnexpectedChildExits,1);
	  ers::warning(ALPIssues::ChildIssue(ERS_HERE, errbuff));
	}
	// std::map<pid_t,int>::iterator it=m_myChildren.find(baby);
	// if(it!=m_myChildren.end()){
	//   char buff[200];
	//   snprintf(buff,200,"%s-%02d",m_myName.c_str(),it->second);
	//   m_diedChildren[std::string(buff)]=it->second;
	//   m_availableSlots.push_back(it->second);
	//   m_myChildren.erase(it);// remove it from the map
	// }
	//TAKE ACTION
	//Is it safe to fork here?
      }
      if(WIFSTOPPED(retVal)){
	ERS_LOG("Child with PID="<<baby<<" stopped by a signal="<<WSTOPSIG(retVal));
	char errbuff[200];
	snprintf(errbuff,200,"Child pid= %d stopped",baby);
	ers::warning(ALPIssues::ChildIssue(ERS_HERE, errbuff));
	// std::map<pid_t,int>::iterator it=m_myChildren.find(baby);
	// if(it!=m_myChildren.end())m_myChildren.erase(it);// remove it from the map
	//TAKE ACTION
	//Is it safe to fork here?
      }
    }
  }
}

void ALP::collectChildExitStatus(){
  int count=0;
  pid_t baby=0;
  while(baby!=-1){
    errno=0;
    int retVal=0;
    baby=waitpid(0,&retVal,WNOHANG);
    if(baby==0){// if all children working sleep 1 seconds
      try{
	boost::this_thread::sleep(boost::posix_time::milliseconds(500));
      }catch(boost::thread_interrupted &ex){
      }
    }else if(baby==-1){//if no child remains
      if(errno==ECHILD){
	ERS_LOG("All children are exited. Returning");
	return;
      }
      count++;
      if(count>10){
	ERS_LOG("Returning because of killswitch");
	return; //killswitch 
      }
    }else{// something happened to children
      if(WIFEXITED(retVal)){
	//should we ask exit status to be 0 in order to catch failures?
	int exitStat=WEXITSTATUS(retVal);
	if(exitStat && !m_terminationStarted){
	  char errbuff[200];
	  snprintf(errbuff,200,"Child pid= %d exited with unexpected return value %d ",baby,exitStat);
	  ers::warning(ALPIssues::ChildIssue(ERS_HERE,errbuff ));
	}
	ERS_LOG("Child with PID="<<baby<<" exited with status="<<exitStat);
	std::map<pid_t,int>::iterator it=m_myChildren.find(baby);
	if(it!=m_myChildren.end()){
	  m_availableSlots.push_back(it->second);
	  m_myChildren.erase(it);// remove it from the map
	}
      }
      if(WIFSIGNALED(retVal)){
	int exitStat=WTERMSIG(retVal);
	if(exitStat && !m_terminationStarted){
	  char errbuff[200];
	  snprintf(errbuff,200,"Child pid= %d exited with unexpected return value %d ",baby,exitStat);
	  ers::warning(ALPIssues::ChildIssue(ERS_HERE,errbuff ));
	}
	ERS_LOG("Child with PID="<<baby<<" exited with a signal="<<WTERMSIG(retVal));
	// std::map<pid_t,int>::iterator it=m_myChildren.find(baby);
	// if(it!=m_myChildren.end()){
	//   m_availableSlots.push_back(it->second);
	//   m_myChildren.erase(it);// remove it from the map
	// }
	//TAKE ACTION
	//Is it safe to fork here?
      }
      if(WIFSTOPPED(retVal)){
	ERS_LOG("Child with PID="<<baby<<" stopped by a signal="<<WSTOPSIG(retVal));
	//TAKE ACTION
	//Is it safe to fork here?
      }
    }
  }
}

void ALP::printPtree(const boost::property_tree::ptree& args, std::string level){
  boost::property_tree::ptree::const_iterator it,itend=args.end();
  level+=" ";
  for(it=args.begin();it!=itend;++it){
    std::string val(it->second.get_value<std::string>());
    boost::algorithm::trim(val);
    std::cout<<level<<it->first<<" : "<<val<<std::endl;
    printPtree(it->second,level);
  }
}

void ALP::startNanny(){
  if(m_nannyThread)return;
  m_nannyWork=true;
  ERS_LOG("STARTING NANNY THREAD -- Mother process");
  m_nannyThread=new boost::thread(&ALP::doNannyWork,this);
}

void ALP::stopNanny(){
  m_nannyWork=false;
  if(!m_nannyThread)return;
  ERS_LOG("STOPPING NANNY THREAD -- Mother process");
  m_nannyThread->interrupt();
  //::sleep(1.5);
  m_nannyThread->join();
  delete m_nannyThread;
  m_nannyThread=0;
}

void ALP::statsPublisher(){
  // char * tmp = getenv("TDAQ_PARTITION");
  // IPCPartition *m_part=0;
  // ERS_LOG("Starting IS Publishing");
  // if(tmp){
  //   ERS_LOG("Using partition "<<tmp<<" server "<<m_ISSName<<" with object name "<<m_myName<<".PU_ChildInfo");
  //   try{
  //     m_part=new IPCPartition(tmp);
  //   }catch(std::exception &ex){
  //     ERS_LOG("Can't create partition object "<<ex.what());
  //   }
  // }
  // if(m_part){
  //   ISInfoDictionary id(*m_part);
  //   std::string objName=m_ISSName+"."+m_myName+".PU_ChildInfo";
  //   boost::chrono::steady_clock::time_point now=boost::chrono::steady_clock::now();
  //   boost::chrono::seconds timeFromEpoch=
  //     boost::chrono::duration_cast<boost::chrono::seconds>(now.time_since_epoch());    
  //   auto toNext=m_publishInterval-(timeFromEpoch.count()%m_publishInterval);
  //   auto sleepDuration=boost::chrono::seconds(m_publishInterval);
  //   auto pubTime=now+boost::chrono::seconds(toNext);
  //   try{
  //     boost::this_thread::sleep_until(pubTime);
  //   }catch(boost::thread_interrupted &ex){

  //   }
  //   try{
  //     {
  // 	std::lock_guard<std::mutex>(m_statMutex);
  // 	if(m_eventsInInterval){
  // 	  double ievts=1.0/m_eventsInInterval;
  // 	  m_childInfo->setFloatField(m_CIAverageL1ResultTime,m_waitDuration.count()*ievts);
  // 	  m_childInfo->setFloatField(m_CIAverageProcessingTime,m_procDuration.count()*ievts);
  // 	}
  // 	if(m_acceptedInInterval){
  // 	  m_childInfo->setFloatField(m_CIAverageAcceptTime,m_accDuration.count()/m_acceptedInInterval);
  // 	}
  // 	if(m_rejectedInInterval){
  // 	  m_childInfo->setFloatField(m_CIAverageRejectTime,m_rejDuration.count()/m_rejectedInInterval);
  // 	}
  // 	if(m_totDuration.count()){
  // 	  double invDur=1./m_totDuration.count();
  // 	  m_childInfo->setFloatField(m_CITimePercentInProcessing,m_procDuration.count()*invDur);
  // 	  m_childInfo->setFloatField(m_CITimePercentInAccept,m_accDuration.count()*invDur);
  // 	  m_childInfo->setFloatField(m_CITimePercentInReject,m_rejDuration.count()*invDur);
  // 	  m_childInfo->setFloatField(m_CITimePercentInWait,m_waitDuration.count()*invDur);
  // 	  m_childInfo->setFloatField(m_CITimePercentInSend,m_sendDuration.count()*invDur);
  // 	}
  // 	m_accDuration=std::chrono::milliseconds(0);
  // 	m_rejDuration=std::chrono::milliseconds(0);
  // 	m_waitDuration=std::chrono::milliseconds(0);
  // 	m_sendDuration=std::chrono::milliseconds(0);
  // 	m_procDuration=std::chrono::milliseconds(0);
  // 	m_totDuration=std::chrono::milliseconds(0);
  // 	m_eventsInInterval=0;
  // 	m_acceptedInInterval=0;
  // 	m_rejectedInInterval=0;
  // 	m_childInfo->setIntField(m_CILongestProcessingTime,0);
  //     }
  //     //id.checkin(objName,*m_childInfo);
  //   }catch(daq::is::Exception &ex){
  //     ERS_LOG("Caught exception "<<ex.what()<<" in first check-in");
  //   }
  //   pubTime+=sleepDuration;
  //   while(m_processEvents){
  //     try{
  // 	boost::this_thread::sleep_until(pubTime);
  // 	pubTime+=sleepDuration;
  //     }catch(boost::thread_interrupted &ex){
  // 	ERS_LOG("Publisher thread sleep is interrupted");
  // 	auto nextPoint=
  // 	  boost::chrono::duration_cast<boost::chrono::milliseconds>(boost::chrono::steady_clock::now().time_since_epoch());    
  // 	auto toNext=m_publishInterval*1000-(nextPoint.count()%(m_publishInterval*1000));
  // 	pubTime+=boost::chrono::milliseconds(toNext);
  //     }
  //     try{
  // 	{
  // 	  std::lock_guard<std::mutex>(m_statMutex);	  
  // 	  if(m_eventsInInterval){
  // 	    double ievts=1.0/m_eventsInInterval;
  // 	    m_childInfo->setFloatField(m_CIAverageL1ResultTime,m_waitDuration.count()*ievts);
  // 	    m_childInfo->setFloatField(m_CIAverageProcessingTime,m_procDuration.count()*ievts);
  // 	  }
  // 	  if(m_acceptedInInterval){
  // 	    m_childInfo->setFloatField(m_CIAverageAcceptTime,m_accDuration.count()/m_acceptedInInterval);
  // 	  }
  // 	  if(m_rejectedInInterval){
  // 	    m_childInfo->setFloatField(m_CIAverageRejectTime,m_rejDuration.count()/m_rejectedInInterval);
  // 	  }
  // 	  if(m_totDuration.count()){
  // 	    double invDur=1./(double)m_totDuration.count();
  // 	    m_childInfo->setFloatField(m_CITimePercentInProcessing,m_procDuration.count()*invDur);
  // 	    m_childInfo->setFloatField(m_CITimePercentInAccept,m_accDuration.count()*invDur);
  // 	    m_childInfo->setFloatField(m_CITimePercentInReject,m_rejDuration.count()*invDur);
  // 	    m_childInfo->setFloatField(m_CITimePercentInWait,m_waitDuration.count()*invDur);
  // 	    m_childInfo->setFloatField(m_CITimePercentInSend,m_sendDuration.count()*invDur);
  // 	  }
  // 	  m_accDuration=std::chrono::milliseconds(0);
  // 	  m_rejDuration=std::chrono::milliseconds(0);
  // 	  m_waitDuration=std::chrono::milliseconds(0);
  // 	  m_sendDuration=std::chrono::milliseconds(0);
  // 	  m_procDuration=std::chrono::milliseconds(0);
  // 	  m_totDuration=std::chrono::milliseconds(0);
  // 	  m_eventsInInterval=0;
  // 	  m_acceptedInInterval=0;
  // 	  m_rejectedInInterval=0;
  // 	  m_childInfo->setIntField(m_CILongestProcessingTime,0);
  // 	}
  // 	//id.checkin(objName,*m_childInfo);
  //     }catch(daq::is::Exception &ex){
  // 	ERS_LOG("Caught exception "<<ex.what()<<" while stats publication");
  //     }
  //   }
  // }else{
  //   ERS_LOG("Can't get partition object");
  // }
  // delete m_part;
}

void ALP::printOpenFDs(const std::string &header=""){
  DIR *dir;
  struct dirent *ent;
  pid_t mypid=getpid();
  if ((dir = opendir ("/proc/self/fd/")) != NULL) {
    /* print all the files and directories within directory */
    std::cout<<ALPUtils::getTimeTag()<<header<<" "<<"List of open FDs (one will be due to this call) pid="<<getpid()<<std::endl;
    while ((ent = readdir (dir)) != NULL) {
      std::string name(ent->d_name);
      if(name=="."||name=="..") continue;
      std::string typ;
      if(ent->d_type==DT_BLK){
	typ="BLOCK";
      }else if(ent->d_type==DT_CHR){
	typ="CHARACTER";
      }else if(ent->d_type==DT_DIR){
	typ="DIR";
      }else if(ent->d_type==DT_FIFO){
	typ="FIFO";
      }else if(ent->d_type==DT_LNK){
	typ="LINK";
	char buf[2001];
	std::string path="/proc/self/fd/"+name;
	int len=readlink(path.c_str(),buf,2000);
	if(len>0){
	  buf[len]='\0';
	  typ+=" -> "+std::string(buf);
	}
      }else if(ent->d_type==DT_REG){
	typ="FILE";
      }else if(ent->d_type==DT_SOCK){
	typ="SOCKET";
      }else if(ent->d_type==DT_UNKNOWN){
	typ="UNKNOWN";
      }
      std::cout<<header<<" "<<mypid<<" "<<name<<" type="<<typ<<std::endl;
    }
    closedir (dir);
  } else {
    /* could not open directory */
    perror ("");
    std::cerr<<"Can't open /proc/self/fd"<<std::endl;
    //return EXIT_FAILURE;
  }
}

void ALP::printTasks(const std::string &header=""){
  DIR *dir;
  struct dirent *ent;
  pid_t mypid=getpid();
  if ((dir = opendir ("/proc/self/task/")) != NULL) {
    /* print all the files and directories within directory */
    std::cout<<ALPUtils::getTimeTag()<<header<<" "<<"List of open FDs (one will be due to this call) pid="<<getpid()<<std::endl;
    while ((ent = readdir (dir)) != NULL) {
      std::string name(ent->d_name);
      if(name=="."||name=="..") continue;
      std::string typ;
      if(ent->d_type==DT_BLK){
	typ="BLOCK";
      }else if(ent->d_type==DT_CHR){
	typ="CHARACTER";
      }else if(ent->d_type==DT_DIR){
	typ="DIR";
      }else if(ent->d_type==DT_FIFO){
	typ="FIFO";
      }else if(ent->d_type==DT_LNK){
	typ="LINK";
	char buf[2001];
	std::string path="/proc/self/task/"+name;
	int len=readlink(path.c_str(),buf,2000);
	if(len>0){
	  buf[len]='\0';
	  typ+=" -> "+std::string(buf);
	}
      }else if(ent->d_type==DT_REG){
	typ="FILE";
      }else if(ent->d_type==DT_SOCK){
	typ="SOCKET";
      }else if(ent->d_type==DT_UNKNOWN){
	typ="UNKNOWN";
      }
      std::cout<<header<<" "<<mypid<<" "<<" threadPID= "<<name<<" type="<<typ<<std::endl;
    }
    closedir (dir);
  } else {
    /* could not open directory */
    perror ("");
    std::cerr<<"Can't open /proc/self/task"<<std::endl;
    //return EXIT_FAILURE;
  }
}
 
 int ALP::countThreads(){
   DIR *dir;
   struct dirent *ent;
   int nThreads=0;
   pid_t mypid=getpid();
   if ((dir = opendir ("/proc/self/task/")) != NULL) {
     while ((ent = readdir (dir)) != NULL) {
       std::string name(ent->d_name);
       if(name=="."||name=="..") continue;
       std::string typ;
       if(ent->d_type==DT_BLK){
	 typ="BLOCK";
       }else if(ent->d_type==DT_CHR){
	 typ="CHARACTER";
       }else if(ent->d_type==DT_DIR){
	 typ="DIR";
       }else if(ent->d_type==DT_FIFO){
	 typ="FIFO";
       }else if(ent->d_type==DT_LNK){
	 typ="LINK";
	 char buf[2001];
	 std::string path="/proc/self/task/"+name;
	 int len=readlink(path.c_str(),buf,2000);
	 if(len>0){
	   buf[len]='\0';
	   typ+=" -> "+std::string(buf);
	 }
       }else if(ent->d_type==DT_REG){
	 typ="FILE";
       }else if(ent->d_type==DT_SOCK){
	 typ="SOCKET";
       }else if(ent->d_type==DT_UNKNOWN){
	 typ="UNKNOWN";
       }
       int currPid=0;
       try{
	 currPid=std::stoi(name);
       }catch(std::exception &ex){
	 
       }
       if(mypid!=currPid){
	 nThreads++;
       }
     }
     closedir (dir);
     return nThreads;
   } else {
     /* could not open directory */
     perror ("");
     std::cerr<<"Can't open /proc/self/task"<<std::endl;
     return -1;
   }
   return nThreads;
 }

// void ALP::publishMotherInfo(std::shared_ptr<ISInfoDictionary> dict,const std::string& name){
//   try{
//     //dict->checkin(name,*m_motherInfo);
//     m_lastPublish=boost::chrono::steady_clock::now();
//   }catch(daq::is::Exception &ex){
//     ERS_LOG("Caught exception "<<ex.what()<<" while Object deletion and creation");
//   }
// }

void ALP::startMotherPublisher(){
  
  if(m_motherPublisher)return;
  m_publisherWork=true;
  ERS_LOG("Starting MotherInfo publisher thread");
  m_motherPublisher=new boost::thread(&ALP::doMotherPublication,this);
}

void ALP::stopMotherPublisher(){
  m_publisherWork=false;
  if(!m_motherPublisher)return;
  ERS_LOG("Stopping MotherInfo publisher thread");
  m_motherPublisher->interrupt();
  //::sleep(1.5);
  m_motherPublisher->join();
  delete m_motherPublisher;
  m_motherPublisher=0;
  std::this_thread::sleep_for(std::chrono::milliseconds(500));// wait for potential ipc operations
}

void ALP::doMotherPublication(){
  // char * tmp = getenv("TDAQ_PARTITION");
  // std::shared_ptr<IPCPartition> part;
  // std::shared_ptr<ISInfoDictionary> id;
  // std::string objName=m_ISSName+"."+m_myName+".PU_MotherInfo";
  // ERS_LOG("Starting Mother IS Publishing");

  // if(tmp){
  //   ERS_LOG("Using partition "<<tmp<<" server "<<m_ISSName<<" with object name "<<m_myName<<".PU_MotherInfo");
  //   try{
  //     part=std::make_shared<IPCPartition>(tmp);
  //   }catch(std::exception &ex){
  //     ERS_LOG("Can't create partition object "<<ex.what());
  //   }
  // }
  // if(part){
  //   id=std::make_shared<ISInfoDictionary>(*part);
  // }

  // boost::chrono::steady_clock::time_point now=boost::chrono::steady_clock::now();
  // boost::chrono::seconds timeFromEpoch=boost::chrono::duration_cast<boost::chrono::seconds>(now.time_since_epoch());
  // boost::chrono::seconds timeFromLast=timeFromEpoch-boost::chrono::duration_cast<boost::chrono::seconds>(m_lastPublish.time_since_epoch());

  // auto toNext=m_publishInterval-(timeFromEpoch.count()%m_publishInterval);
  // if((timeFromLast.count()>m_publishInterval)&&(toNext>m_publishInterval*0.5)){// we missed some publications
  //   publishMotherInfo(id,objName);
  // }
  // int64_t numPublishes=timeFromEpoch.count()/m_publishInterval+1;
  // boost::chrono::steady_clock::time_point nextPublish(boost::chrono::seconds(numPublishes*m_publishInterval));
  // while(m_publisherWork){
  //   try{
  //     boost::this_thread::sleep_until(nextPublish);
  //   }catch(boost::thread_interrupted &ex){
  //     //don't need to sleep anymore
  //     if(!m_publisherWork) return;
  //   }
  //   publishMotherInfo(id,objName);
  //   nextPublish+=boost::chrono::seconds(m_publishInterval);
  // }

}

void ALP::softTimeout(){
  ERS_LOG("Called softTimeout Evt= "<<m_evtNum<<", LB= "<<m_lbNum);
  boost::property_tree::ptree a;
  a.put("StartTime_s",std::chrono::duration_cast<std::chrono::seconds>(m_TOTimerStart.time_since_epoch()).count());
  m_HLTSteering->timeOutReached(a);
  m_softTOTrigger=false;
  m_hardTOTrigger=false;
  GCIncrIntField(m_childInfo,m_CISoftTimeouts,1);
}

void ALP::hardTimeout(){
  ERS_LOG("Called hardTimeout This shouldn't have happened Evt="<<m_evtNum<<", LB="<<m_lbNum);
  //  std::exit(3);
  m_hardTOTrigger=false;
}

void ALP::runTimer(){
  auto softDuration=std::chrono::milliseconds(m_softTimeout);
  auto hardDuration=std::chrono::milliseconds(m_hardTimeout);
  std::unique_lock<std::mutex> lock(m_timeoutMutex);
  while(m_timerWork){
    m_timeoutCond.wait_for(lock,std::chrono::seconds(1));
    auto now=std::chrono::steady_clock::now();
    if(m_softTOTrigger && (now>m_TOTimerStart+softDuration)){
      softTimeout();
    }
    if(m_hardTOTrigger && (now>m_TOTimerStart+hardDuration)){
      hardTimeout();
    }
  }
}

void ALP::waitForFreeMem(int maxSleep){
  if(m_myPos==0)return;
  if(maxSleep<=0)return;
  auto twait=std::chrono::steady_clock::now()+std::chrono::seconds(maxSleep);
  char buff[1000];
  std::ifstream selfMem("/proc/self/statm/");
  std::string line;
  std::getline(selfMem,line);
  long pageSize=sysconf(_SC_PAGESIZE);
  if(pageSize<1){
    ERS_LOG("Couldn't get page size. Errno was ="<<errno<< ". Assuming Pagesize= 4096. ");
    errno=0;
    pageSize=4096;
  }
  unsigned int vmTot,vmRss,vmShare,vmText,vmLib,vmData,vmDirty;
  
  sscanf(line.c_str(),"%u %u %u %u %u %u %u",&vmTot,&vmRss,&vmShare,&vmText,&vmLib,&vmData,&vmDirty);
  selfMem.close();
  vmTot*=pageSize;
  vmRss*=pageSize;
  std::set<int> activeSiblings;

  while(std::chrono::steady_clock::now()<twait){
    try{
      std::ifstream meminfo("/proc/meminfo");
      std::getline(meminfo,line);//MemTotal
      unsigned int memTotal=0,memFree=0,memAvail=0;
      sscanf(line.c_str(),"%s: %u kB",buff,&memTotal);
      std::getline(meminfo,line);//MemTotal
      sscanf(line.c_str(),"%s: %u kB",buff,&memFree);
      std::getline(meminfo,line);//MemTotal
      sscanf(line.c_str(),"%s: %u kB",buff,&memAvail);
      meminfo.close();
      for(auto it:m_posPidMap){// check siblings
	if(kill(it.second,0)==-1){// process is missing
	  errno=0;
	  if(activeSiblings.find(it.first)!=activeSiblings.end()){//process was not there before either
	    activeSiblings.erase(activeSiblings.find(it.first));//take it out
	  }
	}else{// process is active
	  activeSiblings.insert(it.first);
	}
      }
      if(activeSiblings.size()==0)break;
      if(memAvail-(vmRss*activeSiblings.size())>vmRss){//there is enough space for at least one more process
	break;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));// wait for other processes
    }catch(std::exception &ex){
      ERS_LOG("Failed reading proc memory information. "<<ex.what());
      break;
    }
  }
}
