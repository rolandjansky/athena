/*

Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

Author: Sami Kama 2017

*/
#include "DFStreamEventSelector.h"
#include <functional>
#include <memory>
#include <errno.h>
#include <unistd.h>
#include <dlfcn.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include "hltinterface/DataSource.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODEventInfo/EventAuxInfo.h"

DFStreamEventSelector::DFStreamEventSelector(const std::string &name, ISvcLocator* pSvcLocator):AthService(name,pSvcLocator),	
												m_incidentSvc("IncidentSvc", name),
												m_evtStore("StoreGateSvc", name),
												m_robProvider("ROBDataProviderSvc", name)
{

}

DFStreamEventSelector::~DFStreamEventSelector(){}

StatusCode DFStreamEventSelector::initialize(){
  
  char* errmsg;
  if(!m_ds){
    //http://stackoverflow.com/questions/12358843/why-are-function-pointers-and-data-pointers-incompatible-in-c-c
    std::string libName(m_plugin.value());
    //is full lib? check for .so extension
    if(libName.size()>3 && libName.substr(libName.size()-3)!=".so"){//not full lib
      libName=std::string("lib")+libName+std::string(".so");
    }
    
    union{
      hltinterface::DataSource* (*fptr)(void);
      void *ptr;
    } uc;

    union{
      void (*fptr)(hltinterface::DataSource*);
      void *ptr;
    } ud;

    void* handle=dlopen(libName.c_str(),RTLD_LAZY|RTLD_LOCAL);
    if(!handle){
      ATH_MSG_FATAL("Can't open "<<libName<<" error is "<<dlerror());
      return StatusCode::FAILURE;
    }
    dlerror();
    uc.ptr=dlsym(handle,"create_hltmp_datasource");
    if((errmsg=dlerror())!=NULL){
      ATH_MSG_FATAL("Can't load symbol 'create_hltmp_datasource' from "<<libName);
      return StatusCode::FAILURE;
    }
    dlerror();
    ud.ptr=dlsym(handle,"destroy_hltmp_datasource");
    if((errmsg=dlerror())!=NULL){
      ATH_MSG_FATAL("Can't load symbol 'destroy_hltmp_datasource' from "<<libName);
      return StatusCode::FAILURE;
    }
    dlerror();
    m_ds=std::shared_ptr<hltinterface::DataSource>(uc.fptr(),std::ptr_fun(ud.fptr));
  }
  if(!m_ds){
    ATH_MSG_FATAL("DataSource creation failed");
    return StatusCode::FAILURE;
  }
  boost::property_tree::ptree pt;
  int fl=  boost::property_tree::xml_parser::no_comments|
    boost::property_tree::xml_parser::trim_whitespace;
  try{
    boost::property_tree::xml_parser::read_xml(m_pluginConfig.value(),pt,fl);
  }catch(std::exception &ex){
    ATH_MSG_FATAL("Caught exception when parsing ptree. Exception was:"<<ex.what());
    return StatusCode::FAILURE;
  }

  try{    
    if(!m_ds->configure(pt)){
      ATH_MSG_FATAL("DataSource Configuration failed!");      
      return StatusCode::FAILURE;
    }
  }catch(std::exception &ex){
    ATH_MSG_FATAL("DataSource Configuration failed with "<<ex.what());
    return StatusCode::FAILURE;
  }catch(...){
    ATH_MSG_FATAL("DataSource Configuration failed with an unknown exception");
    return StatusCode::FAILURE;
  }
  if(!m_robProvider.retrieve().isSuccess()){
    ATH_MSG_FATAL("Cant retrieve ROBDataProviderSvc");
    return StatusCode::FAILURE;
  }
  if(!m_evtStore.retrieve().isSuccess()){
    ATH_MSG_FATAL("Cant retrieve EventStore");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode DFStreamEventSelector::start(){
  boost::property_tree::ptree conf;
  try{
    m_ds->prepareForRun(conf);
  }catch(std::exception &ex){
    ATH_MSG_FATAL("DataSource preparation failed with "<<ex.what());
    return StatusCode::FAILURE;
  }catch(...){
    ATH_MSG_FATAL("DataSource preparation failed with an unknown exception");
    return StatusCode::FAILURE;
  }
  conf.put("start_id",0);
  conf.put("stride",1);
  conf.put("appName","Test");// used by the PSC
  conf.put("clientName","Test");
  conf.put("workerId",0);//used by PSC
  conf.put("numberOfWorkers",1);// used by PSC
  try{
    m_ds->prepareWorker(conf);
  }catch(std::exception &ex){
    ATH_MSG_FATAL("DataSource preparation failed with "<<ex.what());
    return StatusCode::FAILURE;
  }catch(...){
    ATH_MSG_FATAL("DataSource preparation failed with an unknown exception");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode DFStreamEventSelector::stop(){
  boost::property_tree::ptree conf;
  try{
    m_ds->finalizeWorker(conf);
  }catch(std::exception &ex){
    ATH_MSG_FATAL("DataSource finalization failed with "<<ex.what());
    return StatusCode::FAILURE;
  }catch(...){
    ATH_MSG_FATAL("DataSource finalization failed with an unknown exception");
    return StatusCode::FAILURE;
  }
  try{
    m_ds->finalize(conf);
  }catch(std::exception &ex){
    ATH_MSG_FATAL("DataSource finalization failed with "<<ex.what());
    return StatusCode::FAILURE;
  }catch(...){
    ATH_MSG_FATAL("DataSource finalization failed with an unknown exception");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;

}

StatusCode DFStreamEventSelector::finalize(){
  m_ds.reset();
  if(!m_robProvider.release().isSuccess()){
    ATH_MSG_FATAL("Cant release ROBDataProviderSvc");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode DFStreamEventSelector::createContext(EvtContext*& c) const{
  c=new DFContext();
  if(c)return StatusCode::SUCCESS;
  return StatusCode::SUCCESS;
}


StatusCode DFStreamEventSelector::next(EvtContext& /*c*/) const{
  std::vector<eformat::ROBFragment<const uint32_t*> > data;
  uint32_t lvl1id(0);
  uint64_t gid(0);
  uint64_t lumiBlock(0);
  try{
    m_ds->getL1Result(data,lvl1id,gid,lumiBlock);
  }catch(std::exception &ex){
    ATH_MSG_FATAL("DataSource getL1Result failed with "<<ex.what());
    return StatusCode::FAILURE;
  }catch(...){
    ATH_MSG_FATAL("DataSource getL1Result failed");
    return StatusCode::FAILURE;
  }
  auto evInfo=new xAOD::EventInfo();
  auto evInfoAux=new xAOD::EventAuxInfo();
  evInfo->setStore(evInfoAux);
  evInfo->setEventNumber(gid);
  evInfo->setLumiBlock(lumiBlock);
  if(!m_evtStore->record(evInfo,"EventInfo").isSuccess()){
    ATH_MSG_FATAL("EventInfo registration to storegate failed");
    return StatusCode::FAILURE;
  }
  if(!m_evtStore->record(evInfoAux,"EventInfoAux").isSuccess()){
    ATH_MSG_FATAL("EventInfo registration to storegate failed");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}


StatusCode DFStreamEventSelector::next(EvtContext& c,int jump) const{
  for(int i=0;i<jump;i++){
    if(next(c)!=StatusCode::SUCCESS){
      return StatusCode::FAILURE;
    }
  }
  
  return StatusCode::SUCCESS;
}

StatusCode DFStreamEventSelector::previous(EvtContext& /*c*/) const{
  ATH_MSG_INFO("Not implemented");
  return StatusCode::FAILURE;
}
//Can't really jump should we just read 
StatusCode DFStreamEventSelector::previous(EvtContext& /*c*/,int /*jump*/) const{
  ATH_MSG_INFO("Not implemented");
  return StatusCode::FAILURE;
}

StatusCode DFStreamEventSelector::last(EvtContext& /*c*/) const{
  ATH_MSG_INFO("Not implemented");
  return StatusCode::FAILURE;
}

StatusCode DFStreamEventSelector::rewind(EvtContext& /*c*/) const{
  ATH_MSG_INFO("Not implemented");
  return StatusCode::FAILURE;
}

StatusCode DFStreamEventSelector::createAddress(const EvtContext& /*c*/,IOpaqueAddress*& iop) const{
  iop=0;
  return StatusCode::SUCCESS;
}

StatusCode DFStreamEventSelector::releaseContext(EvtContext*& c)const{
  delete c;
  c=0;
  return StatusCode::SUCCESS;
}

StatusCode DFStreamEventSelector::resetCriteria(const std::string& /*cr*/,Context& /*c*/)const{
  return StatusCode::SUCCESS;
}
