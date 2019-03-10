/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAPOOLCNVSVC_CONDATTRLISTVECADDRESS_H
#define ATHENAPOOLCNVSVC_CONDATTRLISTVECADDRESS_H
/** @file CondAttrListVecAddress.h
 *  @brief This file contains the class definition for the CondAttrListVecAddress class.
 *  @author Richard Hawkings <richard.hawkings@cern.ch>
 **/

#include "AthenaPoolUtilities/CondAttrListVec.h"

// Framework include files
#include "GaudiKernel/GenericAddress.h"

// Forward declarations
class IRegistry;

/** @class CondAttrListVecAddress 
 *  @brief This class provides the an IOpaqueAddress/GenericAddress
 *  which can hold a pointer to an AthenaAttributeList object
 *
 **/
class CondAttrListVecAddress : public GenericAddress {
public:
    /// Dummy constructor
    CondAttrListVecAddress();
    /// Copy Constructor
    CondAttrListVecAddress(const CondAttrListVecAddress& copy);
    /// Assignment
    // not allowed with new Gaudi
    CondAttrListVecAddress& operator=(const CondAttrListVecAddress& copy) = delete;
    /// Constructor from GA
    CondAttrListVecAddress(const GenericAddress& copy);
    /// Standard Constructor
    CondAttrListVecAddress( long svc,
			  const CLID& clid,
			  const std::string& p1="", 
			  const std::string& p2="",
			  unsigned long ip1=0,
			  unsigned long ip2=0,
			  CondAttrListVec* attrListVec=0);

    /// Standard Destructor
    virtual ~CondAttrListVecAddress();

    /// Access to CondAttrListVec
    CondAttrListVec*  attrListVec    ();

    /// Add CondAttrListVec
    void                  setAttrListVec (CondAttrListVec* attrListVec);

private:

    /// Pointer to a CondAttrListVec
    CondAttrListVec* m_attrListVec;
};

///  Inline definitions
inline     
CondAttrListVecAddress::CondAttrListVecAddress() 
	: 
	GenericAddress(),
	m_attrListVec(0)
{}

inline     
CondAttrListVecAddress::CondAttrListVecAddress(const CondAttrListVecAddress& copy)
	:
	GenericAddress(copy),
	m_attrListVec(copy.m_attrListVec)
{
    if (m_attrListVec) m_attrListVec->addRef();
}

// not allowed with new Gaudi
// inline     
// CondAttrListVecAddress&
// CondAttrListVecAddress::operator=(const CondAttrListVecAddress& copy)
// {
//   if (this != &copy) {
//     GenericAddress::operator= (copy);
//     if (m_attrListVec) m_attrListVec->release();
//     m_attrListVec = copy.m_attrListVec;
//     if (m_attrListVec) m_attrListVec->addRef();
//   }
//   return *this;
// }

inline     
CondAttrListVecAddress::CondAttrListVecAddress(const GenericAddress& copy)
	:
	GenericAddress(copy),
	m_attrListVec(0)
{}

inline     
CondAttrListVecAddress::CondAttrListVecAddress( long svc,
					      const CLID& clid,
					      const std::string& p1, 
					      const std::string& p2,
					      unsigned long ip1,
					      unsigned long ip2,
					      CondAttrListVec* attrListVec)
	:
	GenericAddress( svc, clid, p1, p2, ip1, ip2),
	m_attrListVec(attrListVec)
{
    if (m_attrListVec) m_attrListVec->addRef();
}

inline     
CondAttrListVecAddress::~CondAttrListVecAddress()   
{
    if (m_attrListVec) m_attrListVec->release();
}

inline CondAttrListVec*
CondAttrListVecAddress::attrListVec    ()
{
    return m_attrListVec;
}

inline void
CondAttrListVecAddress::setAttrListVec (CondAttrListVec* attrListVec)
{
    if (m_attrListVec) m_attrListVec->release();
    m_attrListVec = attrListVec;
    if (m_attrListVec) m_attrListVec->addRef();
}

#endif // ATHENAPOOLCNVSVC_CONDATTRLISTVECADDRESS_H

