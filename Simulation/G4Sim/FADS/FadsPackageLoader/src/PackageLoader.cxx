/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// This package header
#include "FadsPackageLoader/PackageLoader.h"

// For the string handling
#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>

// standard library
#include <iostream>
#include <string>
#include <cstdio>
#include <dlfcn.h>
#include <cstdlib>

namespace FADS {

std::string PackageLoader::defaultVersion;

struct SetPackageLoaderDefaultVersion {

  SetPackageLoaderDefaultVersion() {
    const char * version = getenv("ATLASSETTINGSCONFIG");
    if (version) PackageLoader::setDefaultVersion(version);
//    cout << "APPL_LIB_VERSION is " << version << endl; 
  }

};

void PackageLoader::setDefaultVersion(const char * version) { 
  defaultVersion=version;
}

void PackageLoader::setDefaultVersion(const std::string & version) { 
  defaultVersion=version;
}

const std::string &  PackageLoader::getDefaultVersion() { 
  return defaultVersion;
}

static SetPackageLoaderDefaultVersion setPackageLoaderDefaultVersion;


PackageLoader::Dict & PackageLoader::loaded() {
  static Dict localDict;
  return localDict;
}


void PackageLoader::confirm(const std::string & pack, const std::string & version) {
  CDI ol = loaded().find(pack);
  if (ol!=loaded().end()) 
    std::cerr << "confirming loading of package " << pack
	      << " already loaded with version \"" 
	      << (*ol).second.first << "\"" << std::endl;
  else loaded()[pack] = Elem(version,0);

}

void PackageLoader::dump(std::ostream & co) {

  // We call this again several times - I am having a lot of trouble imagining how this is necessary
//  Dict & localLoaded = loaded();

  co << "\nPackages loaded:" << std::endl;
  CDI p=loaded().begin();
  CDI e=loaded().end();
  for (;p!=e;p++)
    co << (*p).first << " version \"" << (*p).second.first << "\"\n";
  std::cout << std::endl;
}



bool PackageLoader::operator()(const std::string & packlist, const std::string& version) const {
  // tokenize
  if (packlist.empty()) return true;
  Dict & localLoaded = loaded();
  bool ok = true;

   // load
  boost::char_separator<char> sep(":");
  boost::tokenizer<boost::char_separator<char>> packs(packlist,sep);
  BOOST_FOREACH(std::string p, packs){
    CDI ol = localLoaded.find(p);
    if (ol==localLoaded.end()) {
      // new package
      std::cout << "loading package " << p 
		<< " version " << version << std::endl;
      std::string fLibPath("lib");
      fLibPath+=p; fLibPath+=".so";
      void * handle=dlopen(fLibPath.c_str(),RTLD_LAZY|RTLD_GLOBAL);
      if (handle==0) {
        std::cerr << "error in loading shared library: " << dlerror() << std::endl;
        ok = false;
      }
      CDI nl = localLoaded.find(p);
      if (nl!=localLoaded.end()) std::cout << "package " << p << ": loading confirmed" << std::endl;
      localLoaded[p] = Elem(version,handle);
    }
    else {
      // package already loaded 
      std::cerr << "asked to load package " << p << " version \"" << version << "\"" << std::endl;
      if ((*ol).second.first==version) std::cerr << "Warning: package already loaded" << std::endl;
      else if ((*ol).second.first=="Unknown") std::cerr << "Warning: package already loaded with unknown version" << std::endl;
      else { 
        std::cerr << "ERROR: package already loaded with version" << (*ol).second.first << std::endl;
        ok = false;
      }
    }

  }
  return ok;
}

bool PackageLoader::unload(const std::string & packlist) {
  // tokenize
  if (packlist.empty()) return true;
  Dict & localLoaded = loaded();
  bool ok = true;
   // load
  boost::char_separator<char> sep(":");
  boost::tokenizer<boost::char_separator<char>> packs(packlist,sep);
  BOOST_FOREACH(std::string p, packs){
    DI ol = localLoaded.find(p);
    if (ol==localLoaded.end()) {
      std::cerr<< "cannot unload not registered package " << p <<std::endl;
      ok=false;
    }
    else if((*ol).second.second==0) {
      std::cerr << "cannot unload package " << p 
		<< " handle is 0" << std::endl;
      ok=false;
    }
    else {
      dlclose((*ol).second.second);
      std::cout << "unloading package " << p 
		<< " version " << (*ol).second.first << std::endl;
      localLoaded.erase(ol);
    }
  }
  return ok;
}

}	// end namespace
