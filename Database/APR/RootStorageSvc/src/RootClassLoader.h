/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//====================================================================
//        Generic class handler definition
//--------------------------------------------------------------------
//
//        Package    : POOLCore/StorageSvc (The POOL project)
//
//        Author     : M.Frank
//====================================================================
#ifndef POOL_ROOTSTORAGESVC_ROOTCLASSLOADER_H
#define POOL_ROOTSTORAGESVC_ROOTCLASSLOADER_H

#include "StorageSvc/DbClassLoader.h"
#include "TClassGenerator.h"

/*
 *   pool namespace 
 */
namespace pool  {

  /** @class RootClassLoader, RootClassLoader.h
    *
    * @author  M.Frank
    * @date    1/8/2002
    * @version 1.0
    */
  class RootClassLoader : public TClassGenerator, public DbClassLoader  {
  protected:
    /// Flag to indicate the class conversion is ongoing
    bool m_isConverting;
    bool m_ignoreLoad;

    // internal access to ROOT TClass objects
    TClass* i_loadClass(const char* name, bool silent);

    // internal access to ROOT TClass objects
    DbStatus i_loadClassEx(const std::string& name, bool silent);

    /// checks if the typname refers to a primitive type (like int)
    bool	isPrimitiveType(const char* typname);

   public:

    /// Standard constructor
    RootClassLoader();

    /// Standard destructor
    ~RootClassLoader();

    /// Release resource
    virtual void release();

    /// Load the class unconditionally
    /** @param   name   [IN]   Name of the native class
      *
      * @return  DbStatus code indicating success or failure.
      */
    virtual DbStatus loadClass(const std::string& name);

    /** Note, that the name of the native class may not correspond
      * to the class name of the LCG reflection interface
      * 
      * @param   name   [IN]   Name of the native class
      *
      * @return  DbStatus code indicating success or failure.
      */
    virtual DbStatus loadNativeClass(const std::string& name);

    /// Unload the class
    /** @param   name   [IN]   Name of the native class
      *
      * @return  DbStatus code indicating success or failure.
      */
    virtual DbStatus unloadClass(const std::string& name);

    /// Overloading TClassGenerator
    virtual TClass *GetClass(const char* name, Bool_t load);

    /// Overloading TClassGenerator
    virtual TClass *GetClass(const char* name, Bool_t load, Bool_t silent);

    /// Overloading TClassGenerator
    virtual TClass *GetClass(const type_info& typeinfo, Bool_t load);

    /// Overloading TClassGenerator
    virtual TClass *GetClass(const type_info& typeinfo, Bool_t load, Bool_t silent);
  };
}
#endif // POOL_ROOTSTORAGESVC_ROOTCLASSLOADER_H
