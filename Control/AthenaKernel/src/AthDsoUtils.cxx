///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// @file DsoUtils.h
/// @brief A mixed bag of "portable" utils for DSO (Dynamic Shared Objects)
/// @author S.Binet <binet@cern.ch>
/// @author Markus Frank <Markus.Frank@cern.ch> (Win32 code)
//  note: This, I believe, should be part of Reflex::SharedLibrary

// STL includes
#include <string.h>
#include <string>

#include "GaudiKernel/System.h"
#include "AthenaKernel/AthDsoUtils.h"

namespace Athena {
namespace DsoUtils {

inline 
std::string
libNativeName( const std::string& libName )
{
#if defined(_WIN32)
  return libName+".dll";
#elif defined(__linux) || defined(__APPLE__)
  return "lib"+libName+".so";
#else
  // variant of the GIGO design pattern
  return libName;
#endif
}

static
std::string
dsoName( const RootDataMember& mem )
{
   const char *lib = mem.GetSharedLibs();
   return lib? lib : "";
}
   
bool 
inDso( const RootDataMember& mem,
       const std::string& dsoname )
{
#ifdef _WIN32
  char sep = '\\';
#else
  char sep = '/';
#endif

  std::string srcname = dsoName(mem);
  if (srcname.empty()) {
    // we do not know the name of the library, let's guess it's OK
    return true;
  }

  std::string::size_type pos = dsoname.find_last_of(sep);
  std::string curname;
  if (std::string::npos == pos) {
    curname = dsoname;
  } else {
    curname = dsoname.substr(pos+1);
  }

  return srcname == curname;
}

} // end namespace DsoUtils

} // end namespace Athena


