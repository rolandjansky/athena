/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAPOOLCNVSVC_ATHENAATTRLISTADDRESS_H
#define ATHENAPOOLCNVSVC_ATHENAATTRLISTADDRESS_H
/** @file AthenaAttrListAddress.h
 *  @brief This file contains the class definition for theAthenaAttrListAddress class.
 *  @author RD Schaffer <R.D.Schaffer@cern.ch>
 *  $Id: AthenaAttrListAddress.h,v 1.4 2008-08-28 12:23:21 schaffer Exp $
 **/

// AthenaAttrList
#include "AthenaPoolUtilities/AthenaAttributeList.h"

// Framework include files
#include "GaudiKernel/GenericAddress.h"

// Forward declarations
class IRegistry;

/** @class AthenaAttrListAddress 
 *  @brief This class provides the an IOpaqueAddress/GenericAddress
 *  which can hold a pointer to an AthenaAttributeList object
 *
 **/
class AthenaAttrListAddress : public GenericAddress {
public:
    /// Dummy constructor
    AthenaAttrListAddress();
    /// Copy Constructor
    AthenaAttrListAddress(const AthenaAttrListAddress& copy);
    /// Constructor from GA
    AthenaAttrListAddress(const GenericAddress& copy);
    /// Standard Constructor
    AthenaAttrListAddress( long svc,
			  const CLID& clid,
			  const std::string& p1="", 
			  const std::string& p2="",
			  unsigned long ip1=0,
			  unsigned long ip2=0,
			  AthenaAttributeList* attrList=0);

    /// Standard Destructor
    virtual ~AthenaAttrListAddress();

    /// Assignment is not implemented.
    AthenaAttrListAddress& operator= (const AthenaAttrListAddress& copy) = delete;

    /// Access to AttributeList
    AthenaAttributeList*  attrList    ();

    /// Add AttributeList
    void                  setAttrList (AthenaAttributeList* attrList);

private:

    /// Pointer to an AthenaAttributeList
    AthenaAttributeList* m_attrList;
};

///  Inline definitions
inline     
AthenaAttrListAddress::AthenaAttrListAddress() 
	: 
	GenericAddress(),
	m_attrList(0)
{}

inline     
AthenaAttrListAddress::AthenaAttrListAddress(const AthenaAttrListAddress& copy)
	:
	GenericAddress(copy),
	m_attrList(copy.m_attrList)
{
    if (m_attrList) m_attrList->addRef();
}


inline     
AthenaAttrListAddress::AthenaAttrListAddress(const GenericAddress& copy)
	:
	GenericAddress(copy),
	m_attrList(0)
{}

inline     
AthenaAttrListAddress::AthenaAttrListAddress( long svc,
					      const CLID& clid,
					      const std::string& p1, 
					      const std::string& p2,
					      unsigned long ip1,
					      unsigned long ip2,
					      AthenaAttributeList* attrList)
	:
	GenericAddress( svc, clid, p1, p2, ip1, ip2),
	m_attrList(attrList)
{
    if (m_attrList) m_attrList->addRef();
}

inline     
AthenaAttrListAddress::~AthenaAttrListAddress()   
{
    if (m_attrList) m_attrList->release();
}


inline AthenaAttributeList*
AthenaAttrListAddress::attrList    ()
{
    return m_attrList;
}

inline void
AthenaAttrListAddress::setAttrList (AthenaAttributeList* attrList)
{
    if (m_attrList) m_attrList->release();
    m_attrList = attrList;
    if (m_attrList) m_attrList->addRef();
}

#endif // ATHENAPOOLCNVSVC_ATHENAATTRLISTADDRESS_H

