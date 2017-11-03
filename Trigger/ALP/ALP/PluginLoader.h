// --*- c++ -*-- 
/*

Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

Author: Sami Kama 2017
*/

#ifndef __ALP_PLUGINLOADER_H
#define __ALP_PLUGINLOADER_H

#include <errno.h>
#include <dlfcn.h>
#include <unistd.h>
#include <vector>
#include <string>
#include <iostream>
#include <map>
#include <memory>

namespace ALPNS{
  class Plugin{
  public:
    Plugin(const std::vector<std::string>& libs):m_libs(libs){};
    template <typename T>
    T function(const std::string &name);
  private:
    std::vector<std::string> m_libs;
  };

  class LibUnloader{
  public:
    LibUnloader(const std::string& lname):m_libName(lname){};
    void operator()(void* handle)const{
      char* error;
      dlerror();
      int ret=dlclose(handle);
      if (ret!=0){
	if((error=dlerror())!=NULL){
	  std::cerr<<"Failed to close library "<<m_libName<<" Error is "<<error<<std::endl;
	}
      }
    }
  private:
    std::string m_libName;
  };

  class PluginLoader{
  public:
    PluginLoader();
    ~PluginLoader();
    static bool addPlugin(const std::string &unitName,const std::vector<std::string> &libList );
    static std::shared_ptr<void> getHandle(const std::string& libName);
    static std::shared_ptr<ALPNS::Plugin> get(const std::string& pluginName);
  private:
    static std::unique_ptr<std::map<std::string,std::vector<std::string> > >  m_units;
    static std::unique_ptr<std::map<std::string,std::shared_ptr<void> > >  m_libHandles;
  };
}

template <typename T>
T ALPNS::Plugin::function(const std::string &name){
  union{
    void *p;
    T origType;
  }u;
  u.p=0;
  //char *error(0);
  for(auto &l:m_libs){
    auto h=ALPNS::PluginLoader::getHandle(l);
    if(h){
      void* handle=h.get();
      dlerror();
      void* s=dlsym(handle,name.c_str());
      if(!dlerror()){
	u.p=s;
	return u.origType;
      }
    }
  }
  return u.origType;
}

#endif
