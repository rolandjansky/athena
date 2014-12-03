/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//====================================================================
//  Generic class handler definition
//--------------------------------------------------------------------
//
//  Package    : POOLCore/StorageSvc (The POOL project)
//
//  @author      M.Frank
//====================================================================
#ifndef POOL_STORAGESVC_DBCLASSLOADER_H
#define POOL_STORAGESVC_DBCLASSLOADER_H

#include "StorageSvc/IClassLoader.h"

/*
 *    pool namespace 
 */
namespace pool  {
  // Forward declarations
  class IOODatabase;
  class ClassHandlers;

  /** class DbClassLoader, DbClassLoader.h, POOLCore/DbClassLoader.h
    *
    * @author  M.Frank
    * @date    1/8/2002
    * @version 1.0
    */
  class DbClassLoader : virtual public IClassLoader  {
  private:
    /// LCG Class handler container
    ClassHandlers*      m_lcgHandlers;
    /// Native Class handler container
    ClassHandlers*      m_natHandlers;
  public:
    /// Default constructor
    DbClassLoader();
    /// Standard destructor
    virtual ~DbClassLoader();
    /// Release resource
    virtual void release();

    DbClassLoader (const DbClassLoader&) = delete;
    DbClassLoader& operator= (const DbClassLoader&) = delete;

    /// Load the class unconditionally. To be overloaded in a real implementation
    virtual DbStatus loadClass(const std::string& name);
    
    /// Load the native class unconditionally. To be overloaded in a real implementation
    /** Note, that the name of the native class may not correspond
      * to the class name of the LCG reflection interface
      * 
      * @param   name   [IN]   Name of the native class
      *
      * @return  DbStatus code indicating success or failure.
      */
    virtual DbStatus loadNativeClass(const std::string& name);

    /// Load the native class unconditionally.
    /** Note, that the name of the native classes may not correspond
      * to the class name of the LCG reflection interface
      * 
      * @param   names  [IN]   Vector with the names of the native classes
      *                        to be loaded.
      *
      * @return  DbStatus code indicating success or failure.
      */
    virtual DbStatus loadNativeClasses( const std::vector<std::string>& names);

    /// Unload the class
    virtual DbStatus unloadClass(const std::string& name);

    /// Access to the class handler by it's LCG class name
    /** @param name    [IN]  Class name as it is known to the LCG dictionary.
      * @param force   [IN]  Flag to force the class loading.
      * @return Pair with the pointers to the LCG and native class description.
      */
    virtual IClassHandler* handler( const std::string& name,
                                    bool force_load=true);

    /// Access to the class handler by it's native class name
    /** @param name    [IN]  Class name as it is known to the native dictionary.
      * @param force   [IN]  Flag to force the class loading.
      * @return Pair with the pointers to the LCG and native class description.
      */
    virtual IClassHandler* nativeHandler( const std::string& name,
                                          bool force_load=true);

    /// Set a user handler in place to deal with special class treatment
    /** @param lcg_name [IN]  Class name as it is known to the LCG dictionary.
      * @param nat_name [IN]  Class name as it is known to the native dictionary.
      * @param hndl     [IN]  Pointer to class handler
      * @return Status code indicating success or failure.
      */
    virtual DbStatus setHandler(      const std::string& lcg_name, 
                                      const std::string& nat_name, 
                                      IClassHandler* hndl);
    /// Remove a user handler
    virtual DbStatus removeHandler(const std::string& cl_name);
  };
}      // End namespace pool
#endif // POOL_STORAGESVC_DBCLASSLOADER_H
