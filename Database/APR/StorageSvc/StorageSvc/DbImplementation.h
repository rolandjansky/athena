/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DbImplementation.h 726071 2016-02-25 09:23:05Z krasznaa $
//====================================================================
//  Pool implmenetation class templates
//--------------------------------------------------------------------
//
//  Package    : StorageSvc (The POOL project)
//
//  @author      M.Frank
//====================================================================
#ifndef POOL_DBIMPLEMENTATION_H
#define POOL_DBIMPLEMENTATION_H 1

/*
 * POOL namespace declaration
 */
namespace pool    {

  // Forward declarations
  class IOODatabase;
  class IClassLoader;

  /** @class DbImplementation DbImplementation.h StorageSvc/DbImplementation.h
    *
    * "Generic" implementation base class
    *
    * Description: Generic helper class to implement stuff common to all
    * other classes. This generally are helpers.
    *
    * The basic implementation is record oriented and should be rather
    * reusable for all record oriented storage systems.
    *
    * @author  M.Frank
    * @version 1.0
    */
  class DbImplementation   {
  private:
    /// Pointer to OO DB implementation
    IOODatabase*  m_ooDB;
  protected:
    /// Standard destructor
    virtual ~DbImplementation();
  public:
    /// Constructor with initializing arguments
    DbImplementation(IOODatabase* idb);
    /// Access to Database implementation
    IOODatabase* db()  const           {   return m_ooDB;     }
    /// Access to the class loader
    IClassLoader* loader() const;
  };
}      // End namespace pool
#endif // POOL_DBIMPLEMENTATION_H
