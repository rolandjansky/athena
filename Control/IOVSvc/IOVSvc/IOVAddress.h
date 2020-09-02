/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IOVSVC_IOVADDRESS_H
#define IOVSVC_IOVADDRESS_H

#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/ClassID.h"

class IOVAddress: public IOpaqueAddress {

protected:
  
  unsigned long   m_refCount;
  /// Storage type
  long            m_svcType;
  /// Class id
  CLID            m_clID;
  /// String parameters to be accessed
  std::string     m_par[2];
  /// Integer parameters to be accessed
  unsigned long   m_ipar[2];
  /// Pointer to corresponding directory
  IRegistry*      m_pRegistry;
  
  IOVRange m_range;

public:
 
  // Dummy constructor
  IOVAddress() 
    : m_refCount(0), 
      m_svcType(0),
      m_clID(0),
      m_pRegistry(0)
  {
    m_ipar[0]=m_ipar[1]==0xFFFFFFFF;
  }

  // Copy constructor
  IOVAddress(const IOVAddress& copy)
    : IOpaqueAddress(copy),
      m_refCount(0),
      m_svcType(copy.m_svcType),
      m_clID(copy.m_clID),
      m_pRegistry(copy.m_pRegistry),
      m_range(copy.m_range)
  {
    m_par[0]  = copy.m_par[0];
    m_par[1]  = copy.m_par[1];
    m_ipar[0] = copy.m_ipar[0];
    m_ipar[1] = copy.m_ipar[1];
  }

  /// Standard Constructor
  IOVAddress(long svc,
             const CLID& clid,
             const std::string& p1, 
             const std::string& p2,
             unsigned long ip1,
             unsigned long ip2,
             const IOVRange& range)  
    : m_refCount(0),
      m_svcType(svc),
      m_clID(clid),
      m_pRegistry(0),
      m_range(range)
  {
    m_par[0]  = p1;
    m_par[1]  = p2;
    m_ipar[0] = ip1;
    m_ipar[1] = ip2;
  }

  /// Standard Destructor
  virtual ~IOVAddress()   {
  }
  
  /// Add reference to object
  virtual unsigned long addRef   ()   {
    return ++m_refCount;
  }
  
  virtual unsigned long release  ()   {
    int cnt = --m_refCount;
    if ( 0 == cnt )   {
      delete this;
    }
    return cnt;
  }
  /// Pointer to directory
  virtual IRegistry* registry()   const     {
    return m_pRegistry;
  }
  /// Set pointer to directory
  virtual void setRegistry(IRegistry* pRegistry)   {
    m_pRegistry = pRegistry;
  }
  /// Access : Retrieve class ID of the link
  virtual const CLID& clID()  const   {
    return m_clID;
  }
  /// Access : Set class ID of the link
  virtual void setClID(const CLID& clid)   {
    m_clID = clid;
  }
  /// Access : retrieve the storage type of the class id
  virtual long svcType()  const    {
    return m_svcType;
  }
  /// Access : set the storage type of the class id
  virtual void setSvcType(long typ)    {
    m_svcType = typ;
  }
  /// Retrieve string parameters
  virtual const std::string* par() const   {
    return m_par;
  }
  /// Retrieve integer parameters
  virtual const unsigned long* ipar()  const  {
    return m_ipar;
  }
  
  /// Retrieve IOVRange
  virtual IOVRange range() const {
    return m_range;
  }

  virtual void setRange(IOVRange& range) {
    m_range = range;
  }

  /// Retrieve dbKey
  virtual const std::string& key() const {
    return m_par[0];
  }

  /// Retrieve dbtag
  virtual const std::string& tag() const {
    return m_par[1];
  }
  

private:
  IOVAddress& operator= (const IOVAddress&);
};

#endif
