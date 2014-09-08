/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//====================================================================
//
//        Package    : RootStorageSvc (The POOL project)
//
//        Author     : M.Frank
//====================================================================

#include "CINTTypedefs.h"
#include "TClass.h"

#include <cstdlib>
#include <fstream>
#include <sstream>


static std::auto_ptr<CINT::Typedefs> s_defs(0);

/// Standard constructor
CINT::Typedefs::Typedefs()  {
  const char* defs = ::getenv("POOL_ROOTDEFS");
  if ( 0 == defs ) defs = ::getenv("POOLDB_ROOTDEFS");
  if ( defs )  {
    char buff[1024];
    std::string idq = defs;
    size_t idx = idq.find(";");
    if ( idx == std::string::npos ) idx = idq.find(":"); // WIN32 vs. Linux
    while ( !idq.empty() )  {
      std::string fname = idq.substr(0,idx);
      std::fstream in(fname.c_str(),std::ios_base::in);
      memset(buff,0,sizeof(buff));
      while ( in.getline(buff,sizeof(buff),'\n').good() )   {
        buff[sizeof(buff)-1]=0;
        if ( ::strlen(buff) )  {
          std::istringstream instr(buff);
          std::string dummy, old, type;
          instr >> dummy;
          if ( dummy == "typedef" )  {
            instr >> old >> type;
            size_t len = (type[type.length()-1]==';') ? type.length()-1 : type.length();
            m_defs[type.substr(0,len)] = old;
          }
          else if ( dummy == "typeremove" )  {
            instr >> type;
            size_t len = (type[type.length()-1]==';') ? type.length()-1 : type.length();
            m_defs.erase(type.substr(0,len));
          }
        }
        memset(buff,0,sizeof(buff));
      }
      if ( idx != std::string::npos ) {
        std::string tmp = idq.substr(fname.length()+1);
        idq = tmp;
        idx = idq.find(";");
        if ( idx == std::string::npos ) idx = idq.find(":"); // WIN32 vs. Linux
        continue;
      }
      break;
    }
  }
}

/// Standard destructor
CINT::Typedefs::~Typedefs()  {
  m_defs.clear();
}


#if ROOT_VERSION_CODE < ROOT_VERSION(5,99,0)
#include "Api.h"

/// Call to apply typedefs on a per classname basis if requested.
TClass* CINT::Typedefs::applyType(const std::string& type_name, bool silent)  {
  Definitions::const_iterator i=m_defs.find(type_name);
  if ( i != m_defs.end() )  {
    std::string typ = (*i).first;
    std::string old = (*i).second;
    TClass* cl = TClass::GetClass(old.c_str(), kTRUE, silent);
    if ( cl )  {
      G__ClassInfo info(old.c_str());
      if ( info.IsValid() )  {
        G__search_typename2(typ.c_str(),'u',info.Tagnum(),0,-1);
        G__setnewtype(-1,NULL,0);
        return cl;
      }
    }
  }
  return 0;
}
#else
#include <iostream>
using namespace std;
TClass* CINT::Typedefs::applyType(const std::string& type_name, bool )  {
   cout << "RootStorageSvc::CINT::Typedefs::applyType(" << type_name << ") not implemented in ROOT6" << endl;
   return 0;
}
#endif

TClass* CINT::Typedefs::apply(const std::string& cl_name, bool silent) {
  if ( 0 == s_defs.get() )  {
    s_defs = std::auto_ptr<Typedefs>(new Typedefs());
  }
  return s_defs->applyType(cl_name,silent);
}
