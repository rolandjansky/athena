/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STORAGESVC_DBCONNECTION_H
#define STORAGESVC_DBCONNECTION_H

#include "StorageSvc/DatabaseConnection.h"
#include <string>

/*
 *   POOL namespace
 */
namespace pool {

  // Forward declarations
  class DbDatabase;

  /** @class DbConnection DbConnection.h StorageSvc/DbConnection.h
    *
    * Definition of the DbConnection class. The connection holds
    * data specific to dealing with one type of Database.
    * 
    * This is NOT a public class...Let's see if there is anyone
    * out there capable of respecting this.
    *
    * @author: M.Frank
    * @version 1.0
    */
  class DbConnection : public DatabaseConnection  {
  private:
    /// Reference count
    int m_refCount;
    /// Connection type
    int m_type;
    /// Name of this connection
    std::string m_name;
    /// True handle
    void* m_handle;
  public:
    /// Constructor with initializing arguments
    DbConnection(int typ, const std::string& nam, void* hdl) 
    : DatabaseConnection(), 
      m_refCount(0), m_type(typ), m_name(nam), m_handle(hdl)   {              }
    /// Copy Constructor
    DbConnection(const DbConnection& c) 
    : DatabaseConnection(),
      m_refCount(0), m_type(c.m_type), m_name(c.m_name), m_handle(c.m_handle){}
    /// Standard destructor
    virtual ~DbConnection()                     {                             }

    DbConnection& operator= (const DbConnection&) = delete;

    /// Release token: Decrease reference count and eventually delete.
    int release()    {
      int cnt = --m_refCount;
      if ( 0 >= cnt )   {
        delete this;
      }
      return cnt;
    }
    /// Increase reference count 
    int addRef()                              {  return ++m_refCount;         }
    /// Access object identifier
    virtual void* handle()                    {  return m_handle;             }
    virtual const void* handle() const        {  return m_handle;             }
    /// Access Database identifier
    virtual const std::string& name() const   {  return m_name;               }
    /// Access technoliogy type
    virtual int type() const                  {  return m_type;               }
  };
}      // namespace pool

#endif /// STORAGESVC_DBCONNECTION_H
