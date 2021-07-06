/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STORAGESVC_DATABASECONNECTION_H
#define STORAGESVC_DATABASECONNECTION_H

#include <string>

/*
 *   POOL namespace
 */
namespace pool {

  /** @class DatabaseConnection DatabaseConnection.h StorageSvc/DatabaseConnection.h
    *
    * Definition of the DbConnection class. The connection holds
    * data specific to dealing with one type of Database.
    *
    * @author: M.Frank
    * @version 1.0
    */
  class DatabaseConnection   {
  protected:
    /// Destructor (called only by sub-classes)
    virtual ~DatabaseConnection()   {     }
  public:
    /// Access Database identifier
    virtual const std::string& name() const = 0;
    /// Access technoliogy type
    virtual int type() const = 0;
    /// Access object identifier
    virtual void* handle() = 0;
    virtual const void* handle() const = 0;
  };
}      // End namespace pool

#endif /// STORAGESVC_DATABASECONNECTION_H
