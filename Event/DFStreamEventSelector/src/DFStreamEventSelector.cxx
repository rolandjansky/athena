#include "DFStreamEventSelector.h"
#include "hltinterface/DataSource.h"
#include <functional>
#include <memory>
#include <errno.h>
#include <unistd.h>
#include <dlfcn.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

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
  return StatusCode::SUCCESS;
}

StatusCode DFStreamEventSelector::start(){
  
  return StatusCode::SUCCESS;
}
StatusCode DFStreamEventSelector::stop(){
  return StatusCode::SUCCESS;
}
StatusCode DFStreamEventSelector::finalize(){
  return StatusCode::SUCCESS;
}

StatusCode DFStreamEventSelector::createContext(EvtContext*& c) const{
  if(c)return StatusCode::SUCCESS;
  return StatusCode::SUCCESS;
}


StatusCode DFStreamEventSelector::next(EvtContext& /*c*/) const{
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

StatusCode DFStreamEventSelector::releaseContext(EvtContext*&)const{
  return StatusCode::SUCCESS;
}

StatusCode DFStreamEventSelector::resetCriteria(const std::string& /*cr*/,Context& /*c*/)const{
  return StatusCode::SUCCESS;
}
