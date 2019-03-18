/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAPOOLCNVSVC_CONDATTRLISTCOLLADDRESS_H
#define ATHENAPOOLCNVSVC_CONDATTRLISTCOLLADDRESS_H
/** @file CondAttrListCollAddress.h
 *  @brief This file contains the class definition for the CondAttrListCollAddress class.
 *  @author RD Schaffer <R.D.Schaffer@cern.ch>
 *  $Id: CondAttrListCollAddress.h,v 1.2 2007-02-15 18:45:41 schaffer Exp $
 **/

// AthenaAttrList
#include "AthenaPoolUtilities/CondAttrListCollection.h"

// Framework include files
#include "GaudiKernel/GenericAddress.h"

// Forward declarations
class IRegistry;

/** @class CondAttrListCollAddress 
 *  @brief This class provides the an IOpaqueAddress/GenericAddress
 *  which can hold a pointer to an CondAttrListCollection object
 *
 **/
class CondAttrListCollAddress : public GenericAddress {
public:
    /// Dummy constructor
    CondAttrListCollAddress();
    /// Copy Constructor
    CondAttrListCollAddress(const CondAttrListCollAddress& copy);
    /// Assignment.
    CondAttrListCollAddress& operator= (const CondAttrListCollAddress& copy);
    /// Constructor from GA
    CondAttrListCollAddress(const GenericAddress& copy);
    /// Standard Constructor
    CondAttrListCollAddress( long svc,
			  const CLID& clid,
			  const std::string& p1="", 
			  const std::string& p2="",
			  unsigned long ip1=0,
			  unsigned long ip2=0,
			  CondAttrListCollection* attrListColl=0);

    /// Standard Destructor
    virtual ~CondAttrListCollAddress();

    /// Access to AttributeList
    CondAttrListCollection*  attrListColl    ();

    /// Add AttributeList
    void                  setAttrListColl (CondAttrListCollection* attrListColl);

private:

    /// Pointer to an CondAttrListCollection
    CondAttrListCollection* m_attrListColl;
};

///  Inline definitions
inline     
CondAttrListCollAddress::CondAttrListCollAddress() 
	: 
	GenericAddress(),
	m_attrListColl(0)
{}

inline     
CondAttrListCollAddress::CondAttrListCollAddress(const CondAttrListCollAddress& copy)
	:
	GenericAddress(copy),
	m_attrListColl(copy.m_attrListColl)
{
    if (m_attrListColl) m_attrListColl->addRef();
}

inline     
CondAttrListCollAddress&
CondAttrListCollAddress::operator=(const CondAttrListCollAddress& copy)
{
  if (this != &copy) {
    GenericAddress::operator= (copy);
    if (m_attrListColl) m_attrListColl->release();
    m_attrListColl = copy.m_attrListColl;
    if (m_attrListColl) m_attrListColl->addRef();
  }
  return *this;
}

inline     
CondAttrListCollAddress::CondAttrListCollAddress(const GenericAddress& copy)
	:
	GenericAddress(copy),
	m_attrListColl(0)
{}

inline     
CondAttrListCollAddress::CondAttrListCollAddress( long svc,
					      const CLID& clid,
					      const std::string& p1, 
					      const std::string& p2,
					      unsigned long ip1,
					      unsigned long ip2,
					      CondAttrListCollection* attrListColl)
	:
	GenericAddress( svc, clid, p1, p2, ip1, ip2),
	m_attrListColl(attrListColl)
{
    if (m_attrListColl) m_attrListColl->addRef();
}

inline     
CondAttrListCollAddress::~CondAttrListCollAddress()   
{
    if (m_attrListColl) m_attrListColl->release();
}

inline CondAttrListCollection*
CondAttrListCollAddress::attrListColl    ()
{
    return m_attrListColl;
}

inline void
CondAttrListCollAddress::setAttrListColl (CondAttrListCollection* attrListColl)
{
    if (m_attrListColl) m_attrListColl->release();
    m_attrListColl = attrListColl;
    if (m_attrListColl) m_attrListColl->addRef();
}

#endif // ATHENAPOOLCNVSVC_CONDATTRLISTCOLLADDRESS_H

