/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//====================================================================
//
//        Package    : RootStorageSvc (The POOL project)
//
//        Author     : M.Frank
//====================================================================
#ifndef POOL_ROOTSTORAGESVC_CINT_TYPEDEF_H
#define POOL_ROOTSTORAGESVC_CINT_TYPEDEF_H

// C++ include files
#include <map>
#include <string>
#include <memory>

class TClass;

namespace CINT  {
  /** @class Type CINTTypedefs.h RootStorageSvc/CINTTypedefs.h
    *
    * Description
    * Implementation to overload typedefs in ROOT.
    *
    * @author  M.Frank
    * @date    1/2/2003
    * @version 1.0
    */
  class Typedefs  {
    friend class std::auto_ptr<Typedefs>;

  protected:
    typedef std::map<std::string,std::string> Definitions;
    /// Type definitions
    Definitions m_defs;

    /// Standard constructor
    Typedefs();
    /// Standard destructor
    virtual ~Typedefs();
    /// Call to apply typedefs on a per classname basis if requested.
    TClass* applyType(const std::string& type_name, bool silent);
  public:
    /// Apply type defs
    static TClass* apply(const std::string& type_name, bool silent);
  };
}
#endif // POOL_ROOTSTORAGESVC_CINT_TYPEDEF_H
