//---------------------------------------------------------------------------//
//                                                                           //
// This file is part of the Pittsburgh Visualization System (PVS)            //
//                                                                           //
// Copyright (C) 2004 Joe Boudreau, University of Pittsburgh                 //
//                                                                           //
//  This program is free software; you can redistribute it and/or modify     //
//  it under the terms of the GNU General Public License as published by     //
//  the Free Software Foundation; either version 2 of the License, or        //
//  (at your option) any later version.                                      //
//                                                                           //
//  This program is distributed in the hope that it will be useful,          //
//  but WITHOUT ANY WARRANTY; without even the implied warranty of           //
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            //
//  GNU General Public License for more details.                             //
//                                                                           //
//  You should have received a copy of the GNU General Public License        //
//  along with this program; if not, write to the Free Software              //
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307     //
// --------------------------------------------------------------------------//


#include "QatDataAnalysis/IOLoader.h"
#include <iostream>
#include <map>
#include <sstream>
#include <unistd.h>
#include <dirent.h>
#include <dlfcn.h>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <cstdio>
class IOLoader::Clockwork {
public:



  typedef void * (*CreationMethod) ();

  std::string _loadFileName;

  std::map<std::string, CreationMethod>    _create;
  std::map<std::string, const IODriver *>  _driver;

  void loadDrivers() {
    //
    // This is only for atlas:
    //
    char *_ldlibpath = getenv("LD_LIBRARY_PATH");
    char *ldlibpath = _ldlibpath ? strdup(_ldlibpath):NULL;
    if (ldlibpath) {
      char *thisDir=strtok(ldlibpath,(char *)":");
      bool allLoaded=false;
      while (thisDir) {
	DIR *directory = opendir(thisDir);
	if (directory) {
	  dirent * entry = readdir(directory);
	  while (entry) {
	    
	    std::string  name = entry->d_name;
	    std::string  path = thisDir + std::string("/") + name;
	    std::string  basename, extension;
	    size_t  dotpos = name.find_last_of('.');
	    if (dotpos != name.npos) {
	      extension = name.substr(dotpos);
	      basename  = name.substr(0,dotpos);
	    }
	    
	    std::string shortName;
	    
	    // Here is a list of known drivers.  This is not nice but neither
	    // is CMT...
	    
	    if (name=="libQoot.so") shortName="RootDriver";

	    // .. and their is only one driver on the list..
	    
	    if (shortName!="") {
	      if (_create.find(shortName)==_create.end()) {
		
		std::string createFunctionName;
		createFunctionName=shortName+"Create";
		
		void *handle = dlopen(path.c_str(),RTLD_LAZY);
		if (!handle) std::cerr << dlerror() << std::endl;
		
		CreationMethod createMethod;
		union { CreationMethod func; void* data; } sym;
		
		sym.data= dlsym(handle, createFunctionName.c_str());
		createMethod = sym.func;
		_create[shortName]=createMethod;
		if (!createMethod) {
		  std::cout << basename << " load failed" << std::endl;
		}
		else {
		  allLoaded=true; // only one, so far...
		  break;
		}
	      }
	    }
	    entry = readdir(directory);
	  }
	  closedir(directory);
	  if (allLoaded) break;
	}
	else {
	  //perror(NULL);
	}
	thisDir=strtok(NULL, (char *) ":");
      }
    }
  }
};

IOLoader::IOLoader():c(new Clockwork()) {
  c->loadDrivers();
}

IOLoader::~IOLoader() {
  delete c;
}



const IODriver *IOLoader::ioDriver(const std::string & name ) const {
  std::map<std::string, const IODriver *>::iterator d = c->_driver.find(name);
  if (d==c->_driver.end()) {
    std::map<std::string, Clockwork::CreationMethod>::iterator m = c->_create.find(name);
    if (m!=c->_create.end()) {
      Clockwork::CreationMethod createMethod = (*m).second;
      IODriver *driver = (IODriver *) (*createMethod) ();
      c->_driver[name] =driver;
      return driver;
    }
    else {
      return NULL;
    }
  }
  else 
    return (*d).second;
  
}
