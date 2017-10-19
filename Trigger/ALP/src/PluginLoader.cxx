/*

Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

Author: Sami Kama 2017
*/

// Class to manage the library loading.
//Author: Sami Kama

#include "ALP/PluginLoader.h"

std::unique_ptr<std::map<std::string,std::vector<std::string> > > ALPNS::PluginLoader::m_units;
std::unique_ptr<std::map<std::string,std::shared_ptr<void > > > ALPNS::PluginLoader::m_libHandles;

bool ALPNS::PluginLoader::addPlugin(const std::string &unitName,const std::vector<std::string> &libList){
  if(!m_units){
    m_units.reset(new std::map<std::string,std::vector<std::string> > ());
  }
  if(!m_libHandles){
    m_libHandles.reset(new std::map<std::string,std::shared_ptr<void > >  );
  }
  auto res=m_units->insert(std::make_pair(unitName,libList));
  if(res.second){
    char* err(0);
    for(const auto &l:libList){
      dlerror();
      if(m_libHandles->find(l)!=m_libHandles->end())continue;
      void* handle=dlopen(l.c_str(),RTLD_LAZY|RTLD_GLOBAL);
      if(!handle){
	err=dlerror();
	std::cerr<<"Plugin "<<unitName<<" opening library "
		 <<l <<" failed with "<<err<<std::endl;
      }else{
	auto s=std::shared_ptr<void>(handle,ALPNS::LibUnloader(l));
	m_libHandles->insert(std::make_pair(l,s));
      }
    }
  }
  return res.second;
}

std::shared_ptr<void> ALPNS::PluginLoader::getHandle(const std::string& libName){
  if(!m_libHandles){
    m_libHandles.reset(new std::map<std::string,std::shared_ptr<void > > ());
  }
  if(!m_units){
    m_units.reset(new std::map<std::string,std::vector<std::string> > ());
  }
  auto it=m_libHandles->find(libName);
  if(it!=m_libHandles->end()){
    return it->second;
  }
  return std::shared_ptr<void>();
}

std::shared_ptr<ALPNS::Plugin> ALPNS::PluginLoader::get(const std::string& pluginName){
  auto it =m_units->find(pluginName);
  if (it!=m_units->end()){
    return std::make_shared<ALPNS::Plugin>(it->second);
  }
  return std::shared_ptr<ALPNS::Plugin>(0);
}

