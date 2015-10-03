/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//====================================================================
//        Root Database factories implementation
//--------------------------------------------------------------------
//
//        Package    : POOLCore/RootDb (The POOL project)
//
//        Author     : M.Frank
//====================================================================

#include "StorageSvc/DbPrint.h"
#include "StorageSvc/DbReflex.h"
#include "StorageSvc/DbClassHandler.h"

#include "TROOT.h"
#include "TClass.h"
//#include "TRealData.h"
//#include "TClassEdit.h"
//#include "TDataMember.h"

#include "CINTTypedefs.h"
#include "RootClassLoader.h"

using namespace pool;

#include <utility>
#include <vector>

#ifdef XAOD_ANALYSIS
#include "TError.h"
#endif


#if ROOT_VERSION_CODE < ROOT_VERSION(5,99,0)
#include "Cintex/Cintex.h"

namespace ROOT { namespace Cintex  {
  std::string CintName(const std::string&);
}}
inline
std::string CintName(const std::string& name) { return ROOT::Cintex::CintName(name); }

#else

inline
std::string CintName(const std::string& name) { return name; }

#endif


namespace {
  struct CallPreserve {
    bool& m_val;
    bool m_save;
    CallPreserve(bool& val) : m_val(val), m_save(val) { m_val = true; }
    ~CallPreserve() { m_val = m_save; }
  };
}

/// Standard constructor
RootClassLoader::RootClassLoader() 
: DbClassLoader(), m_isConverting(false), m_ignoreLoad(false)
{
#if ROOT_VERSION_CODE < ROOT_VERSION(5,99,0)
  static bool first = true;
  if ( first )  {
    first = false;
    m_ignoreLoad = true;
    // Need to preload STL dlls into ROOT to avoid mismatches in the 
    // Collection proxies.
#ifdef XAOD_ANALYSIS
long tmpError = gErrorIgnoreLevel;
gErrorIgnoreLevel=kError; //silences those ever-occuring but incredibly annoying warnings from vector.dll and vector-bool.dll
#endif
    gROOT->ProcessLine("#include <vector>");
    gROOT->ProcessLine("#include <list>");
    gROOT->ProcessLine("#include <map>");
    gROOT->ProcessLine("#include <set>");
#ifdef XAOD_ANALYSIS
gErrorIgnoreLevel=tmpError;
#endif
    m_ignoreLoad = false;
  }
  ROOT::GetROOT()->AddClassGenerator(this);
  ROOT::Cintex::Cintex::Enable();
#else  
  ROOT::GetROOT()->AddClassGenerator(this);
#endif
}

/// Standard destructor
RootClassLoader::~RootClassLoader() {
}

/// Release resource
void RootClassLoader::release() {
}

/// Load the native class unconditionally
DbStatus RootClassLoader::loadNativeClass(const std::string& name)  {
   return i_loadClassEx(name,false);
}

/// Load the class unconditionally
DbStatus RootClassLoader::loadClass(const std::string& name)  {
   return i_loadClassEx(name,false);
}

/// Load the class unconditionally
DbStatus RootClassLoader::i_loadClassEx(const std::string& name,bool silent)
{
  CallPreserve preserve(m_isConverting);
  TypeH rcl = DbReflex::forTypeName(name);
  TClass* cl = 0;
  bool load_attempted = false;
  if ( !rcl )  {
    cl = i_loadClass(name.c_str(),silent);
    load_attempted = true;
    if ( cl )  {
      rcl = DbReflex::forTypeName(cl->GetName());
      if ( !rcl && cl->GetTypeInfo() )  {
        rcl = DbReflex::forTypeInfo(*cl->GetTypeInfo());
      }
    }
  }
  if ( rcl )  {
    // First look in internal cache.
    IClassHandler* hnd = handler(DbReflex::fullTypeName(rcl), false);
    if ( 0 != hnd )  {
      cl = (TClass*)hnd->nativeClass();
      if ( 0 != cl )  {
        return Success;
      }
    }
    else  {
      if ( !cl && !load_attempted ) {
        cl = i_loadClass(name.c_str(),silent);
      }
      if ( cl )  {
        return setHandler(name, cl->GetName(), new DbClassHandler(rcl, cl));
      }
    }
  }
  if( !isPrimitiveType( name.c_str() ) ) {
     DbPrint log(  "RootStorageSvc::RootClassLoader");
     log << DbPrintLvl::Info
	 << "Failed to load dictionary for native class: \"" << name << "\""
	 << DbPrint::endmsg;
  }
  return pool::Error;
}


TClass* RootClassLoader::i_loadClass(const char* classname, bool silent)  {
  if ( m_ignoreLoad )   {
    return 0;
  }
  else {
     if( isPrimitiveType( classname ) )  return 0;

     size_t len = ::strlen(classname);
     if ( len>1 && classname[len-1]=='*' ) return 0;

     TypeH rcl = DbReflex::forTypeName(classname);
     if ( rcl && (rcl.IsFundamental() || rcl.IsPointer()) )  {
	return 0;
     } 
     TClass* cl = TClass::GetClass(classname, kTRUE, silent);//FIXME
     //TClass* cl = rcl;
     if ( !cl ) {
	cl = CINT::Typedefs::apply(classname,silent);
	if ( !cl )  {
	   size_t idx;
           std::string root_name = ::CintName(classname);
	   //std::string root_name = classname;
	   cl = TClass::GetClass(root_name.c_str(), kTRUE, silent);//FIXME
	   //cl = TClass::GetClass(root_name.c_str(), kTRUE);
	   if ( !cl ) {
	      while( (idx=root_name.find("const ")) != std::string::npos )
		 root_name.replace(idx, 6, "");
	      cl = TClass::GetClass(root_name.c_str(), kTRUE, silent);//FIXME
	      //cl = TClass::GetClass(root_name.c_str(), kTRUE);
	   }
	}
     }
     return cl;
  }
  return 0;
}


bool RootClassLoader::isPrimitiveType(const char* typname)
{
   switch(typname[0])  {
    case 'b':
       if ( strcmp(typname,"bool")           ==0 ) return true;
       break;
    case 'l':
       if ( strncmp(typname,"lib",3)         ==0 ) return true;
       if ( strcmp(typname,"long")           ==0 ) return true;
       if ( strcmp(typname,"long long")      ==0 ) return true;
       if ( strcmp(typname,"long long int")  ==0 ) return true;
       break;
    case 'L':
       if ( strcmp(typname,"Long_t")         ==0 ) return true;
       if ( strcmp(typname,"Long64_t")       ==0 ) return true;
       break;
    case 'i':
       if ( strcmp(typname,"int")            ==0 ) return true;
       if ( strcmp(typname,"__int64")        ==0 ) return true;
       break;
    case 'I':
       if ( strcmp(typname,"Int_t")          ==0 ) return true;
       break;
    case 'e':
       if ( strncmp(typname,"enum ",5)       ==0 ) return true;
       break;
    case 'd':
       if ( strcmp(typname,"double")         ==0 ) return true;
       break;
    case 'D':
       if ( strcmp(typname,"Double_t")       ==0 ) return true;
       break;
    case 'f':
       if ( strcmp(typname,"float")          ==0 ) return true;
       break;
    case 'F':
       if ( strcmp(typname,"Float_t")        ==0 ) return true;
       break;
    case 's':
       if( strcmp(typname,"short")           ==0 ) return true;
       if( strcmp(typname,"short int")       ==0 ) return true;
       break;
    case 'S':
       if ( strcmp(typname,"Short_t")        ==0 ) return true;
       break;
    case 'u':
       if ( strncmp(typname,"unknown",7)     ==0 ) return true;
       if ( strcmp(typname,"unsigned int")   ==0 ) return true;
       if ( strcmp(typname,"unsigned short") ==0 ) return true;
       if ( strcmp(typname,"unsigned long")  ==0 ) return true;
       if ( strcmp(typname,"unsigned char")  ==0 ) return true;
       if ( strcmp(typname,"unsigned long long")      ==0 ) return true;
       if ( strcmp(typname,"unsigned long long int")  ==0 ) return true;
       break;
   }
   return false;
}


/// Load the class unconditionally
DbStatus RootClassLoader::unloadClass(const std::string& name)  {
  DbPrint log(  "RootClassLoader");
  log << DbPrintLvl::Error << "Unloading of reflection gateways currently not"
    << " supported... Class: \"" << name << "\"" << DbPrint::endmsg;
  return pool::Error;
}

/// Overloading TClassGenerator
TClass* RootClassLoader::GetClass(const char* classname, Bool_t load) {
  return GetClass(classname,load,kFALSE);
}

/// Overloading TClassGenerator
TClass* RootClassLoader::GetClass(const char* classname, Bool_t load, Bool_t silent) {
  if ( !m_isConverting && load )  {
    CallPreserve preserve(m_isConverting);
    return i_loadClass(classname,silent);
  }
  return 0;
}

/// Overloading TClassGenerator
TClass* RootClassLoader::GetClass(const type_info& typeinfo, Bool_t load) {
  return GetClass(typeinfo,load,kFALSE);
}

/// Overloading TClassGenerator
TClass* RootClassLoader::GetClass(const type_info& typeinfo, Bool_t load, Bool_t silent) {
  if ( !m_isConverting && load )  {
    CallPreserve preserve(m_isConverting);
    TypeH lcg_cl = DbReflex::forTypeInfo(typeinfo);
    if ( lcg_cl )  {
      if ( i_loadClassEx(lcg_cl.Name(),silent).isSuccess() )  {
        TClass* cl = TClass::GetClass(typeinfo, kTRUE, silent);
        return cl;
      }
    }
  }
  return 0;
}
